#ifndef API_CVG_H
#define API_CVG_H

#include "api.h"
#include "mtlogger.h"

namespace locality {

/**
static class, that holds cvg profiler functions
*/
class cvg : public profiler
{
private:
	static int& WindowSize()
	{
		static int window_size;

		return window_size;
	}

	static int& Shift()
	{
		static int shift;

		return shift;
	}

	static bool& Enabled()
	{
		static bool enabled;
		return enabled;
	}

public:
	static void Init(int window_size, int shift)
	{
		WindowSize() = window_size;
		Shift() = shift;

		Enabled() = true;

		SubscribeNotify(cvg::NotifyAccess);
		SubscribeRotate(cvg::Rotate);
	}

	static int GetWindowSize()
	{
		return WindowSize();
	}

	static int GetShift()
	{
		return Shift();
	}

	static bool IsEnabled()
	{
		return Enabled();
	}

//-------------------------------

	static MTCVGLogger& GetLogger()
	{
		static MTCVGLogger logger(new CVGConstructor(GetWindowSize(), GetShift()));

		return logger;
	}

	static void Print(int N, double time)
	{
		PrintSizeProf(N);
	   	PrintTimeProf(time);

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

#endif /*API_CVG_H*/
