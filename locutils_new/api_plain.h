#ifndef API_PLAIN_H
#define API_PLAIN_H

#include "api.h"

namespace locality {

class plain : public profiler
{
public:
	static void PrintSize(uint64_t size)
	{
		std::cout << FormatPrefix() << "type : not profiled task size ; val : " << size <<  " ; units : elements(task specific) " << std::endl;
	}

	static void PrintTime(double time)
	{
		std::cout << FormatPrefix() << "type : time ; val : " << time <<  " ; units : sec " << std::endl;
	}

	static void Print(int N, double time)
	{
		PrintSize(N);
		PrintTime(time);
	}
};

} /*namespace*/

#endif /*API_PLAIN_H*/
