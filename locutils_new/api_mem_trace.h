#ifndef API_MEM_TRACE_H
#define API_MEM_TRACE_H

#include "api.h"
#include "mtlogger.h"

namespace locality {

/**
static class, that holds memory tracer profiler functions
*/
class memory_trace : public profiler
{
private:
	static bool& Enabled()
	{
		static bool enabled;
		return enabled;
	}

	static std::string& Path()
	{
		static std::string path;
		return path;
	}

public:
	static bool IsEnabled()
	{
		return Enabled();
	}

	static void Init(std::string path)
	{
		Path() = path;
		Enabled() = true;

		SubscribeNotify(memory_trace::NotifyAccess);
		SubscribeRotate(memory_trace::Rotate);
	}

	static MTFileLogger& GetLogger()
	{
		static MTFileLogger logger(new FileConstructor(Path()));

		return logger;
	}

	static void Print(int N, double time)
	{
		profiler::PrintSizeProf(N);
		profiler::PrintTimeProf(time);

		GetLogger().Print();
	}

	static void NotifyAccess(std::string var_name, size_t index, void* ptr, size_t local_index_offset, int type_size)
	{
		GetLogger().NotifyAccess(var_name, index, ptr, local_index_offset, type_size);
	}

	static void Rotate(std::string name)
	{
		GetLogger().Rotate(name);
	}
};

} /*namespace*/

#endif /*API_MEM_TRACE_H*/
