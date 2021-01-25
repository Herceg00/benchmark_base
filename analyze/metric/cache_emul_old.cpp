#include <set>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <iterator>
#include <algorithm>

using namespace std;

const int reg_size = 8;

const int L1_size = 32 * 1024;
const int L2_size = 256 * 1024;
const int L3_size = 12288 * 1024;

const int cache_line = 64;

const int K_R = 1;
const int K_L1 = 3;
const int K_L2 = 15;
const int K_L3 = 144;
const int K_M = 390;

// represents fully accotiative cache objects with LRU removing policy, based on queue
class Cache
{
private:
public:
		string name;
		size_t size;
		int cache_line;
		int penalty;

		typedef list<size_t> cache_cont;

		cache_cont cache;
		set<size_t> check_set;

	Cache(size_t _size, int _penalty, string _name, int _cache_line):
		name(_name),
		size(_size/_cache_line),
		cache_line(_cache_line),
		penalty(_penalty)
	{}

	size_t AddrToC(size_t addr)
	{
		return addr / cache_line;
	}

	bool IsIn(size_t addr)
	{
		return check_set.find(AddrToC(addr)) != check_set.end();
	}

// adds elem to cache or update it position in queue if it presents
	void Add(size_t addr)
	{
		size_t cache_addr = AddrToC(addr);

		if(!IsIn(addr))
		{
			cache.push_back(cache_addr);
			check_set.insert(cache_addr);


///			Debug("  " + self.name + "  added " + str(addr))

			if(cache.size() > size) // remove last if full
			{
				size_t rem = cache.front();
				cache.pop_front();
				check_set.erase(rem);
			}
				//Debug("  " + self.name + "  removed " + str(rem))
		}
		else
		{
			cache.remove(cache_addr);
			cache.push_back(cache_addr);

//			Debug("  " + self.name + "  updated " + str(addr))
		}
	}
};

Cache reg(reg_size, K_R, "reg", 1);

Cache L1(L1_size, K_L1, "l1", cache_line);
Cache L2(L2_size, K_L2, "l2", cache_line);
Cache L3(L3_size, K_L3, "l3", cache_line);

const int N_CACHES = 4;

//emulate memory flow
long Emul(vector<size_t> elems)
{
	long ticks = 0;

	Cache caches[] = {reg, L1, L2, L3};

	int i = 0;
	int max = elems.size();

	for(vector<size_t> :: iterator it = elems.begin(); it != elems.end(); it++)
	{
		i += 1;
		if(i % 1000 == 0)
			printf("progress %.1f%%\n", (100.0 * i / max));

		bool counted = false;

// search it in all caches
		for(int i = 0; i < N_CACHES; i++)
		{
			bool found = caches[i].IsIn(*it);

			if(found)
			{
				ticks += caches[i].penalty;
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
			caches[i].Add(*it);

		//Debug("Current ticks:                  " + str(ticks))
		//Debug("")
	}

	return ticks;
}

int main(int argc, char** argv)
{

	vector<size_t> v;
	v.reserve(10000);
	ifstream is(argv[1]);

    size_t x = 0;

	while(is >> x)
	{
	    v.push_back(x);
	}

	printf("%lg\n", 1.0 * Emul(v) / v.size());

	return 0;
}