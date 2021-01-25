#ifndef PROFILER_H
#define PROFILER_H

#include "logger.h"

extern const char *__progname;

namespace locality {

/**
static class, that holds general profiling functions
we need class to avoid multiple definitions, when used in some hard cases
*/
class profiler
{
private:
	static std::string& Prefix()
	{
		static std::string prefix("");

		return prefix;
	}

	static bool& Logging()
	{
		static bool logging = false;

		return logging;
	}

	typedef void (*FNotify) (std::string, size_t, void*, size_t, int);
	typedef void (*FRotate) (std::string);

protected:
	static std::vector<FNotify>& NotifySubscribers()
	{
		static std::vector<FNotify> subscribers;
		return subscribers;
	}

	static std::vector<FRotate>& RotateSubscribers()
	{
		static std::vector<FRotate> subscribers;
		return subscribers;
	}

	static void SubscribeNotify(FNotify f)
	{
		NotifySubscribers().push_back(f);
	}

	static void SubscribeRotate(FRotate f)
	{
		RotateSubscribers().push_back(f);
	}

//---------------------------

	static void PrintAlloc(uint64_t count)
	{
		std::cout << FormatPrefix() << "type : allocated memory ; val : " << count << " ; units : B " << std::endl;
	}

	static void PrintTransf(uint64_t size)
	{
		std::cout << FormatPrefix() << "type : transfered data size ; val : " << size <<  " ; units : B " << std::endl;
	}

	static void PrintSizeProf(uint64_t size)
	{
		std::cout << FormatPrefix() << "type : profiled task size ; val : " << size <<  " ; units : elements(task specific) " << std::endl;
	}

	static void PrintTimeProf(double time)
	{
		std::cout << FormatPrefix() << "type : profiled time ; val : " << time <<  " ; units : sec " << std::endl;
	}

public:
	static std::string GetPrefix()
	{
		return Prefix();
	}

	static std::string FormatPrefix()
	{
		return "[ *** locinfo *** ] ; prog : " + std::string(__progname) + " ; prefix : " + Prefix() + " ; ";
	}

	static std::string GetThreadedPrefix(std::string thread_id)
	{
		return "[ *** locinfo *** ] ; prog : " + std::string(__progname) + "_" + thread_id + " ; prefix : " + Prefix() + " ; ";
	}

	static bool GetLogging()
	{
		return Logging();
	}

	static void Logging(bool val)
	{
		Logging() = val;
	}

	static void Rotate(std::string prefix)
	{
		Prefix() = prefix;

		for(std::vector<FRotate>::iterator it = RotateSubscribers().begin(); it != RotateSubscribers().end(); it++)
			(*it)(prefix);
	}

	static void NotifyAllocation(size_t )
	{

	}

	static void NotifyAccess(std::string var_name, size_t index, void* ptr, size_t local_index_offset, int type_size)
	{
		if(!GetLogging())
			return;

#pragma omp critical
{
		for(std::vector<FNotify>::iterator it = NotifySubscribers().begin(); it != NotifySubscribers().end(); it++)
			(*it)(var_name, index, ptr, local_index_offset, type_size);
}
	}
};

} /*namespace*/

#endif /*PROFILER_H*/

