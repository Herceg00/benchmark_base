#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <time.h>

const int ADDITIONAL_TIME_ITERS = 4;

size_t M = 512*1024*1024;
size_t IND_SIZE = 16*1024;
size_t MAX_L = 65536;
size_t MAX_S = 16*1024*1024;

void Save(double val)
{
	FILE* f = fopen("/tmp/stupidgcc", "w");
	fprintf(f, "%f\n", val);
	fclose(f);
}

/// generates array of indicies, ditributed accroding to alpha
/// names are from the article..
size_t* MakeInd(size_t M, double alpha, size_t L, size_t ind_size)
{
	timespec dummy;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &dummy);
	srand48(dummy.tv_nsec);

	size_t* res = new size_t[ind_size];

	for(size_t i = 0; i < ind_size; i++)
		res[i] = pow(drand48(), 1.0 / alpha) * (M / L - 1) * L;

	return res;
}

double TimeDiff(timespec start, timespec end)
{
	return 1.0 * (end.tv_sec - start.tv_sec) + 1.0 * (end.tv_nsec - start.tv_nsec) / 1e9;
}

double* MakeData(size_t M)
{
	timespec dummy;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &dummy);
	srand48(dummy.tv_nsec);

	double* data = new double[M];

	for(size_t i = 0; i < M; i++)
		data[i] = drand48();
	
	return data;
}

/// single random apex iteration
double RandApex(size_t M, double alpha, size_t L, size_t ind_size)
{
	timespec start, end;

	double time = -1;

	for(int iter = 0; iter < ADDITIONAL_TIME_ITERS; iter++)
	{
		double* data = MakeData(M);
		size_t* ind = MakeInd(M, alpha, L, ind_size);

		double res = 0;

clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

		for(size_t i = 0; i < ind_size; i++)
		{
			size_t index = ind[i];

			for(size_t k = 0; k < L; k++)
				res += data[index + k];
		}

clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

		Save(res);
		double next_time = TimeDiff(start, end);

		if(next_time < time || time < 0)
			time = next_time;

		delete[] ind;
		delete[] data;
	}

	return ind_size * (1 + L)  / time / 1e9; 
}
/*
/// single stride apex iteration
double StrideApex(size_t M, size_t S)
{
	timespec start, end;

	double time = -1;

	for(int iter = 0; iter < ADDITIONAL_TIME_ITERS; iter++)
	{

		double res = 0;

clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

		for(size_t i = 0; i < M / S; i++)
		{
			res += data[i * S];
		}

clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

		if(res > 123.45678 && res < 123.45687) // do not dare to optimize something out
			printf("%f\n", res);

		double next_time = TimeDiff(start, end);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return M / S / time / 1e9;
}

void PlotRandCsv()
{
	printf("alpha\\size,");

	for(size_t L = 1; L <= MAX_L; L *= 2)
		printf("%zu, ", L);

	printf("\n");

	for(double alpha = 1.0; alpha > 0.0001; alpha /= 2)
	{
		printf("%f, ", alpha);

		for(size_t L = 1; L <= MAX_L; L *= 2)
			printf("%.2f, ", RandApex(M, alpha, L, IND_SIZE));

		printf("\n");
	}

	delete[] data;
}
*/
void PlotRandMatrix()
{
	for(double alpha = 1.0; alpha > 0.00004; alpha /= 2)
	{
		for(size_t L = 1; L <= MAX_L; L *= 2)
			printf("%.2f ", RandApex(M, alpha, L, IND_SIZE));
		printf("\n");
	}
}
/*
void PlotStrideCsv()
{
	for(size_t S = 1; S < MAX_S; S *= 2)
		printf("%zu,", S);

	printf("\n");

	for(size_t S = 1; S < MAX_S; S *= 2)
		printf("%.2f,", StrideApex(M, S));

	printf("\n");

	delete[] data;
}

void PlotStrideMatrix()
{
	for(size_t S = 1; S < MAX_S; S *= 2)
	{
		printf("%.2f ", StrideApex(M, S));
	}

	printf("\n");

	delete[] data;
}
*/
int main()
{
	PlotRandMatrix();

	return 0;	
}