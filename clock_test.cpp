#include "locutils/papi_wrapper.h"

#include <ctime>
#include <iostream>

using namespace std;

typedef void (*func_t)();

void Measure(func_t func)
{
	SPapiSet clock_counter;
	clock_counter.Add("clock", PAPI_TOT_CYC);

	CPapiController :: Start(clock_counter);
	clock_t begin = clock();

	func();

	clock_t end = clock();
	
	counter_type* values = CPapiController :: Read(clock_counter);

	cout << "counter : " << values[0] << " ; clock() : " <<  end - begin << endl;

	CPapiController :: Stop(clock_counter);
}

//const int N = 128*1;

double a[N][N] = {0};
double b[N][N] = {0};
double c[N][N] = {0};

void Nothing()
{
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			for(int k = 0; k < N; k++)
			{
				a[i][j] += b[i][k] * c[k][j];
			}
}

int main()
{
	CPapiController :: Init();

//	Measure(Nothing);
	cout << CLOCKS_PER_SEC << endl;

	return 0;
}