#ifndef FWRAP_H
#define FWRAP_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <ostream>

namespace locality {

template <typename T>
class SequenceStorage
{
	std::string base_fname;

	std::string current_fname;
	int rotation;

	std::vector<T> store;

	uint64_t bytes;

	bool Rewrite(const std::string& fname)
	{
		if(!store.size())
			return false; 

		std::ofstream fout(fname.c_str());

		if(!fout.is_open())
			throw std::runtime_error("could not create log file " + fname);

		std::copy(store.begin(), store.end(), std::ostream_iterator<T>(fout, "\n"));

		fout.close();

		store.clear();

		std::cout << std::endl << "Total bytes for " << fname
			<< " : " << bytes << std::endl << std::endl;
		bytes = 0;

		return true;
	}

public:
	void Add(T val, int type_size)
	{
		store.push_back(val);
		bytes += type_size;
	}

	void Add(T val)
	{
		store.push_back(val);
	}

	void Rotate()
	{
		Rewrite(current_fname);

		rotation ++;

		current_fname = base_fname + "." + utils::ToString(rotation);
	}

	void Rotate(std::string str)
	{
		Rewrite(current_fname);

		current_fname = base_fname + "." + str;
	}

	bool Close()
	{
		return Rewrite(current_fname);
	}

	SequenceStorage(std::string _base_fname):
		base_fname(_base_fname),
		current_fname(_base_fname),
		rotation(0),
		bytes(0)
	{
		store.reserve(10000); // some random reserve
	}

	~SequenceStorage()
	{
		if(Close())
			std::cout << "closed " << base_fname << std::endl << std::endl;
		else
			std::cerr << "ERROR: could not close " << base_fname << std::endl << std::endl;
	}
};

} /*namespace*/

#endif /*FWRAP_H*/
