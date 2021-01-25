#ifndef TSP
#define TSP

#include <limits>

const int BENCH_COUNT = 1;

const char* type_names[BENCH_COUNT] = {
	"tsp_1", // 0
};

template <typename T, typename AT2_P, typename AT2_D, typename AT>
void Init(AT2_P paths, AT2_D dist, AT tmp, size_t size)
{
	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < 1u << size; j++)
		{
			paths[i][j] = 100 * locality::utils::RRand(i, j);
		}

	for(size_t i = 0; i < size + 1; i++)
		for(size_t j = 0; j < size + 1; j++)
		{
			dist[i][j] = 100 * locality::utils::RRand(j, i);
		}

	for(size_t i = 0; i < size; i++)
		tmp[i] = 0;

}

//http://www.dreamincode.net/forums/topic/190644-travelling-salesman-problem-with-dynamic-programming/page__p__1119847&#entry1119847
template <typename T, typename AT2_P, typename AT2_D, typename AT>
void Kernel1(AT2_P paths, AT2_D dist, AT tmp, T& result, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	/** Initialisiere alle mit Mächtigkeitszahl 2 in paths **/
	for(int k=2; k <= size; k++) // Ohne 0 & 1 für k, bis und mit size
	{
		paths[k-1][1^(1<<(k-1))] = dist[k][1]; // Stadt 1 + Stadt k
	}

	/** Berechne die überigen Mächtigkeitszahlen in paths **/
	for(int s=3; s <= size; s++)                 // Mächtigkeitszahl erhöhen, ohne 0,1,2
	{
		for(int i=0; i<s-1; i++)
			tmp[i] = i;

		while(true)
		{          
			int S = 1;

			for(int i=0; i < s-1; i++)
				S = S^(1<<(tmp[i]+1));

			for(int i=0; i < s-1; i++) //Alle Städte in S (tmp) durchgehen
			{
				int k = tmp[i]+2;
				paths[k-1][S] = std::numeric_limits<int>::max();

				for(int j=0; j < s-1; j++)  // Zweite Verbindungsstadt finden
				{
					int m = tmp[j]+2;
					if(m == k) // m != k
						continue;
					paths[k-1][S] = std::min(paths[m-1][S^(1<<(k-1))] + dist[m][k], paths[k-1][S]); 
				}
			}

			//Compute next Combination
			int i = (s-1) - 1;
			++tmp[i];

			while ((i > 0) && (tmp[i] >= size-1 - (s-1) + 1 + i))
			{
				--i;
				++tmp[i];
			}

			if (tmp[0] > (size-1) - (s-1)) break;
			
			for (i = i + 1; i < (s-1); ++i)
				tmp[i] = tmp[i - 1] + 1;
		}
	}

	/** Optimum berechnen **/
	result = std::numeric_limits<int>::max();

	for(int k=2; k <= size; k++)
	{
		result = std::min(paths[k-1][(1<<(size))-1]+dist[1][k], result);
	}

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT2_P, typename AT2_D, typename AT>
void CallKernel(int core_type, AT2_P paths, AT2_D dist, AT tmp, T& result, size_t size)
{
	switch(core_type)
	{
		case 0: Kernel1<T, AT2_P, AT2_D, AT>(paths, dist, tmp, result, size); break;

		default: fprintf(stderr, "unexpected core type");
	}
}

#endif /*TSP*/
