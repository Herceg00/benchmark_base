#include <set>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <iterator>
#include <algorithm>

using namespace std;

const int reg_size = 8;

const int L1_size = 2 * 1024;
const int L2_size = 256 * 1024;
const int L3_size = 128 * 1024;

const int cache_line = 64;

const int K_R = 1;
const int K_L1 = 4;
const int K_L2 = 10;
const int K_L3 = 50;
const int K_M = 190;

// represents fully accotiative cache objects with LRU removing policy, based on queue
template<typename T>
class Cache
{
private:
public:
		string name;
		T size;
		int cache_line;
		int penalty;

		typedef list<T> cache_cont;

		cache_cont cache;
		map<T, typename cache_cont :: iterator> check_map;

	Cache(size_t _size, int _penalty, string _name, int _cache_line):
		name(_name),
		size(_size/_cache_line),
		cache_line(_cache_line),
		penalty(_penalty)
	{}

	T AddrToC(size_t addr)
	{
		return addr / cache_line;
	}

	bool IsIn(size_t addr)
	{
		return check_map.find(AddrToC(addr)) != check_map.end();
	}

// adds elem to cache or update it position in queue if it presents
	void Add(size_t addr)
	{
		T cache_addr = AddrToC(addr);

		if(!IsIn(addr))
		{
			cache.push_back(cache_addr);
			check_map[cache_addr] = --cache.end();

///			Debug("  " + self.name + "  added " + str(addr))

			if(cache.size() > size) // remove last if full
			{
				T rem = cache.front();
				cache.pop_front();
				check_map.erase(rem);
			}
				//Debug("  " + self.name + "  removed " + str(rem))
		}
		else
		{
			cache.erase(check_map[cache_addr]);
			cache.push_back(cache_addr);

			check_map.erase(cache_addr);
			check_map[cache_addr] = --cache.end();

//			Debug("  " + self.name + "  updated " + str(addr))
		}
	}
};

Cache<size_t> reg(reg_size, K_R, "reg", 1);

Cache<size_t> L1(L1_size, K_L1, "l1", cache_line);
Cache<size_t> L2(L2_size, K_L2, "l2", cache_line);
Cache<size_t> L3(L3_size, K_L3, "l3", cache_line);

//emulate memory flow
long Emul(vector<size_t> elems)
{
	long ticks = 0;

	Cache<size_t>* caches[] = {&reg, &L1, &L3};//, &L2, &L3};
	
	const int N_CACHES = sizeof(caches) / sizeof(Cache<size_t>*);

	int i = 0;
	int max = elems.size();

	for(vector<size_t> :: iterator it = elems.begin(); it != elems.end(); it++)
	{
		i += 1;
		if(i % 1000 == 0)
			printf("progress %.1f%% -- %ld\n", (100.0 * i / max), ticks);

		bool counted = false;

// search it in all caches
		for(int i = 0; i < N_CACHES; i++)
		{
			bool found = caches[i]->IsIn(*it);

			if(found)
			{
				ticks += caches[i]->penalty;
				counted = true;
				//Debug(str(elem) + " found in " + cache.name)
				break;
			}
		}
// if not found - memory access
		if(!counted)
		{
			ticks += K_M;
			//Debug(str(elem) + " memory access")
		}

// update/add it to all caches
		for(int i = 0; i < N_CACHES; i++)
			caches[i]->Add(*it);

		if(L1.check_map.size() < L1.size)
			ticks = 0;

		//Debug("Current ticks:                  " + str(ticks))
		//Debug("")
	}

	return ticks;
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("usage ./emul <mem profile filename>\n");
		return 1;
	}

	vector<size_t> v;
	v.reserve(10000);
	ifstream is(argv[1]);

    size_t x = 0;

	int count = 1000000;

	while(is >> x && count-- > 0)
	{
	    v.push_back(x);
	}

	if(v.size() == 0)
	{
		printf("file does not exists or does not contain proper profile\n");
		return 1;
	}

	printf("%s\n", argv[1]);
	printf("%lg\n", 1.0 * Emul(v) / v.size());

	return 0;
}