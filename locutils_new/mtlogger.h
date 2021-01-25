#ifndef MTLOGGER_H
#define MTLOGGER_H

#include <map>
#include <exception>

#include "logger.h"
#include "api.h"

namespace locality {

class FileConstructor
{
private:
	std::string path;

public:
	FileConstructor(std::string _path):
		path(_path)
	{}

	FileLogger* CreateLogger(std::string name)
	{
		return new FileLogger(path, name);
	}
};

class CVGConstructor
{
private:
	int window_size;
	int shift;

public:

	CVGConstructor(int _window_size, int _shift):
		window_size(_window_size),
		shift(_shift)
	{}

	CVGLogger<CVGAccumulator>* CreateLogger(std::string)
	{
		return new CVGLogger<CVGAccumulator>(window_size, shift);
	}
};

template <typename TLogger, typename TConstructor>
class MTLogger
{
protected:
	typedef std::map<uint64_t, TLogger*> id_logger_map;
	id_logger_map thread_map;
	TConstructor* constructor;

public:
	MTLogger(TConstructor* _constructor):
		constructor(_constructor)
	{}

	virtual ~MTLogger()
	{
		std::cout << "killing logger" << std::endl;

		for(typename id_logger_map::iterator it = thread_map.begin(); it != thread_map.end(); it++)
		{
			if(it->second)
				delete it->second;
		}

		thread_map.clear();
	}

	void Rotate(std::string name)
	{
		for(typename id_logger_map::iterator it = thread_map.begin(); it != thread_map.end(); it++)
			it->second->Rotate(name);
	}

	void NotifyAccess(std::string var_name, size_t index, void* ptr, size_t local_index_offset, int type_size)
	{
		uint64_t thread_id = utils::GetThreadID();

		if(thread_map.find(thread_id) == thread_map.end())
		{
			std::string new_name = utils::ToString(__progname) + std::string("_") + utils::ToString(thread_id);
			thread_map[thread_id] = constructor->CreateLogger(new_name);

			thread_map[thread_id]->Rotate(profiler::GetPrefix());

			std::cout << "added: " << new_name << std::endl;
		}

		typename id_logger_map::iterator log = thread_map.find(thread_id);

		log->second->AddVal(var_name, index, ptr, local_index_offset, type_size);
	}

	virtual void Print() = 0;
};

class MTFileLogger : public MTLogger<FileLogger, FileConstructor>
{
private:

public:
	MTFileLogger(FileConstructor* constructor):
		MTLogger<FileLogger, FileConstructor>(constructor)
	{}

	void Print()
	{}


	virtual ~MTFileLogger(){};
};

class MTCVGLogger : public MTLogger<CVGLogger<CVGAccumulator>, CVGConstructor>
{
private:

public:
	MTCVGLogger(CVGConstructor* constructor):
		MTLogger<CVGLogger<CVGAccumulator>, CVGConstructor>(constructor)
	{}

	static void PrintInstr(uint64_t instr, std::string thread_id)
	{
#pragma omp critical
{
		std::cout << profiler::GetThreadedPrefix(thread_id) << "type : instructions; val : " << instr << " ; units : instr " << std::endl;
}
	}

	static void PrintRects(double rects, std::string thread_id)
	{
#pragma omp critical
{
		std::cout << profiler::GetThreadedPrefix(thread_id) << "type : rects ; val : " << rects << " ; units : rects " << std::endl;
}
	}

	static void PrintWindows(uint64_t windows, std::string thread_id)
	{
#pragma omp critical
{
		std::cout << profiler::GetThreadedPrefix(thread_id) << "type : windows; val : " << windows << " ; units : windows " << std::endl;
}
	}

	static void PrintHisto(const std::vector<uint64_t> histogram, std::string thread_id)
	{
#pragma omp critical
{
		std::cout << profiler::GetThreadedPrefix(thread_id) << "type : histo ; val :";
		
		for(std::vector<uint64_t>::const_iterator it = histogram.begin(); it != histogram.end(); it++)
			std::cout << " " << *it;

		std::cout << " ; units : array " << std::endl;
}
	}

	void Print()
	{
		for(id_logger_map::iterator it = thread_map.begin(); it != thread_map.end(); it++)
		{
			PrintRects(it->second->GetAvg(), utils::ToString(it->first));
			PrintWindows(it->second->GetWindows(), utils::ToString(it->first));
			PrintInstr(it->second->GetInstr(), utils::ToString(it->first));
			PrintHisto(it->second->GetHisto(), utils::ToString(it->first));
		}
	}

	virtual ~MTCVGLogger(){};
};

} /*namespace*/

#endif /*MTLOGGER_H*/
