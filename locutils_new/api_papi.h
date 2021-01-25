#ifndef API_PAPI_H
#define API_PAPI_H

#include "papi_wrapper.h"
#include "api.h"

#ifdef LOC_PAPI

#define PASTER(x,y) x ## y
#define EVALUATOR(x,y) PASTER(x,y)

#define LOC_PAPI_INIT locality::papi::Init(EVALUATOR(locality::papi::Set, LOC_PAPI));

	#define LOC_PAPI_BEGIN_BLOCK locality::SPapiSet sets[LOC_THREADS]; \
		_Pragma("omp parallel for num_threads(LOC_THREADS)") \
		for(int outer = 0; outer < LOC_THREADS; outer++) \
		{ \
			sets[outer] = locality::papi::Start(); \
		} \

	#define LOC_PAPI_END_BLOCK _Pragma("omp parallel for num_threads(LOC_THREADS)") \
		for(int outer = 0; outer < LOC_THREADS; outer++) \
		{ \
			locality::papi::Stop(sets[outer]); \
			locality::papi::Print(sets[outer]); \
		}

#else

	#define LOC_PAPI_INIT
	#define LOC_PAPI_BEGIN_BLOCK for(int i = 0; i < LOC_THREADS; i++){}
	#define LOC_PAPI_END_BLOCK for(int i = 0; i < LOC_THREADS; i++){}

#endif

namespace locality {

/**
static class, that holds papi profiling functions
*/
class papi : public profiler
{
private:
	static bool& Enabled()
	{
		static bool enabled;
		return enabled;
	}

	static SPapiSet& Config()
	{
		static SPapiSet config;
		return config;
	}

	typedef SPapiSet(*papi_generator)();

	static papi_generator& Generator()
	{
		static papi_generator generator;
		return generator;
	}

public:

	static bool IsEnabled()
	{
		return Enabled();
	}

	static void Init(papi_generator generator)
	{
		Generator() = generator;
		Enabled() = true;

		PapiController :: Init();
	}

	static SPapiSet Set1()
	{
		SPapiSet cache_set;

		cache_set.Add("load and store inst", PAPI_LST_INS)
			.Add("L1 data miss", PAPI_L1_DCM)
			.Add("L3 total miss", PAPI_L3_TCM);

		return cache_set;
	}

	static SPapiSet Set2()
	{
		SPapiSet cache_set;

		cache_set.Add("L2 data miss", PAPI_L2_DCM)
			.Add("L2 data hit", PAPI_L2_DCH);

		return cache_set;
	}

	static SPapiSet Set3()
	{
		SPapiSet cache_set;

		cache_set
			.Add("L2 total acc", PAPI_L2_TCA)
			.Add("L3 total acc", PAPI_L3_TCA)
			.Add("L2 total miss", PAPI_L2_TCM);

		return cache_set;
	}

	static SPapiSet Start()
	{
		SPapiSet set = Generator()();
		PapiController::Start(set);

		return set;
	}

	static void Stop(const SPapiSet& set)
	{
		PapiController::Stop(set);
	}

	static void Print(const SPapiSet& set)
	{
		counter_type* values = PapiController :: Read(set);

#pragma omp critical
{
		for(int i = 0; i < set.count; i++)
			std::cout << GetThreadedPrefix(utils::ToString(utils::GetThreadID())) << "type : " << set.names[i] 
				<< " ; val : " << values[i] << " ; units : pcs" << std::endl;
}
	}
};

} /*namespace*/

#endif /*API_PAPI_H*/
