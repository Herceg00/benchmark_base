#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iterator>
#include <map>
#include <set>
#include <queue>
#include <cstdarg>
#include <vector>
#include <list>
#include <stdexcept>

#include "utils.h"

#include "physical.h"
#include "fwrap.h"

// ******************************************************************************************
//   logging
// ******************************************************************************************

namespace locality {

typedef uint64_t index_type;

class CVGAccumulator
{
protected:
	unsigned int window_size;
	unsigned int shift;

// which rect presents
	std::set<index_type> cache_line_occupied;
// how many times does it present
	std::map<index_type, int> cache_line_count;

// result
	N128 res;

// elems count
	uint64_t count;

// windows count
	uint64_t wind_count;

// window contents
	std::list<index_type> values;

//	histogram
	std::vector<index_type> histogram;

public:
	virtual void AddVal(index_type val)
	{
// add to window
		index_type page_val = val >> shift;
		values.push_back(page_val);
		
// fill first values
		if(count < window_size)
		{
			cache_line_count[page_val]++;
			cache_line_occupied.insert(page_val);
		}
		else
		{
			index_type fist_addr = values.front();
			values.pop_front();

// reduce occurance count
			cache_line_count[fist_addr] -= 1;

// if it was last - remove it
			if(cache_line_count[fist_addr] == 0)
				cache_line_occupied.erase(fist_addr);

// add new
			cache_line_count[page_val]++;
			cache_line_occupied.insert(page_val);

// that is amount of "rects" - accumulate it
			res.Add(cache_line_occupied.size());
			histogram.at(cache_line_occupied.size())++;

			wind_count++;
		}

		count++;
	}

	virtual void Reset()
	{
		count = 0;
		res = 0;
		wind_count = 0;
		cache_line_occupied.clear();
		cache_line_count.clear();

		std::list<index_type> empty;
		std::swap(values, empty);
		std::fill(histogram.begin(), histogram.end(), 0);
	}

	virtual double GetAvg()
	{
		return res.GetDiv(wind_count);
	}

	virtual uint64_t GetWindows()
	{
		return wind_count;
	}

	virtual uint64_t GetInstr()
	{
		return count;
	}

	virtual const std::vector<index_type>& GetHisto()
	{
		return histogram;
	}

	CVGAccumulator(int _window_size, int _shift):
		window_size(_window_size),
		shift(_shift)
	{
		histogram.resize(_window_size+1);
		Reset();
	}

	virtual ~CVGAccumulator()
	{}
};

enum E_LOGGER {FILE_LOGGER, RECT_LOGGER};

class Logger
{
protected:
	unsigned long long allocated_mem;

public:
	virtual E_LOGGER GetType() = 0;

	virtual void Rotate() = 0;
	virtual void Rotate(std::string str) = 0;

	virtual void AddVal(std::string var_name, size_t index, void* ptr
		, size_t local_index_offset, int type_size) = 0;

	void AllocateMem(unsigned long long count)
	{
		allocated_mem += count;
	}

	Logger():
		allocated_mem(0)
	{}

	virtual ~Logger()
	{
//		PrintAlloc(allocated_mem);
	};
};

template <typename T>
class CVGLogger : public Logger
{
private:
	T acc;

public:
	virtual E_LOGGER GetType()
	{
		return RECT_LOGGER;
	}

	virtual void AddVal(std::string, size_t, void* ptr, size_t, int)
	{
		acc.AddVal(reinterpret_cast<index_type>(ptr));
	}

	double GetAvg()
	{
		return acc.GetAvg();
	}

	uint64_t GetWindows()
	{
		return acc.GetWindows();
	}

	uint64_t GetInstr()
	{
		return acc.GetInstr();
	}

	const std::vector<index_type>& GetHisto()
	{
		return acc.GetHisto();
	}

	virtual void Rotate()
	{
		acc.Reset();
	}

	virtual void Rotate(std::string)
	{
		Rotate();
	}

	CVGLogger(int window_size, int shift):
		acc(window_size, shift)
	{}
};

class FileLogger : public Logger
{
private:
	typedef std::map<std::string, SequenceStorage<index_type>*> files_map;

	files_map files;
	std::string base_dir;
	std::string name;
	std::string current_suffix;

	SequenceStorage<index_type> virt_logger;
	SequenceStorage<index_type> phys_logger;
	SequenceStorage<index_type> unified_logger;
	SequenceStorage<index_type> united_logger;

	bool phys_enabled;

	void CloseFiles()
	{
		for(files_map :: iterator it = files.begin(); it != files.end(); it++)
			delete (*it).second;

		files.clear();
	}

public:
	virtual E_LOGGER GetType()
	{
		return FILE_LOGGER;
	}

	SequenceStorage<index_type>* GetFile(const std::string& path)
	{
		SequenceStorage<index_type>* store = NULL;

		if(files.find(path) == files.end())
		{
			store = new SequenceStorage<index_type>(path.c_str());

			files[path] = store;
		}
		else
			store = files[path];

		return store;
	}

	void Phys(bool val)
	{
		phys_enabled = val;
	}

	virtual void Rotate()
	{
		phys_logger.Rotate();
		virt_logger.Rotate();
		unified_logger.Rotate();
		united_logger.Rotate();

		for(files_map :: iterator it = files.begin(); it != files.end(); it++)
			it->second->Rotate();

		std::cout << "files rotated" << std::endl;
	}

	virtual void Rotate(std::string str)
	{
		phys_logger.Rotate(str);
		virt_logger.Rotate(str);
		unified_logger.Rotate(str);
		united_logger.Rotate(str);

		current_suffix = str;

		std::cout << "files rotated, suffix appended: " << str << std::endl;
	}

	virtual void AddVal(std::string var_name, size_t index, void* ptr, size_t local_index_offset, int type_size)
	{
		std::string path = base_dir + name + "." + var_name + "." + current_suffix;

		SequenceStorage<index_type>* store = GetFile(path);

		store->Add(index, type_size);

		virt_logger.Add(reinterpret_cast<index_type>(ptr), type_size);

		unified_logger.Add(local_index_offset + index, type_size);
		united_logger.Add(index, type_size);

		if(phys_enabled)
			phys_logger.Add(GetPhys(ptr));
	}

	FileLogger(std::string _base_dir, std::string _name):
		base_dir(_base_dir),
		name(_name),
		virt_logger(base_dir + name + ".virt.log"),
		phys_logger(base_dir + name + ".phys.log"),
		unified_logger(base_dir + name + ".unified.log"),
		united_logger(base_dir + name + ".united.log"),
		phys_enabled(false)
	{
		std::cout << "logger init done in, base dir: " << base_dir << std::endl;
	}

	~FileLogger()
	{
		CloseFiles();
		std::cout << "logger finished: " << base_dir << "/" << name << std::endl;
	}

};

} /*namespace*/

#endif /*LOGGER_H*/
