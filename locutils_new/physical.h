#ifndef PHYSICAL_H
#define PHYSICAL_H

#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

namespace locality {

#define PAGE_SIZE 4096

struct SInfo
{
	uint64_t ptr;
	uint64_t phys;
	uint64_t swapped;
};

static SInfo VirtToPhys(int pid, uint64_t ptr)
{
	SInfo info;
	info.ptr = ptr;

	char pagemap_s[256] = "";
	snprintf(pagemap_s, 256, "/proc/%d/pagemap", pid);
	int pagemap_f = open(pagemap_s, O_RDONLY);
	if(pagemap_f == -1)
	{
		fprintf(stderr, "can not open %s\n", pagemap_s);
		exit(1);
	}

	char maps_s[256] = "";
	snprintf(maps_s, 256, "/proc/%d/maps", pid);
	FILE* maps_f = fopen(maps_s, "r");
	
	if(!maps_f)
	{
		fprintf(stderr, "can not open %s\n", maps_s);
		exit(1);
	}

	char line [256];

	while(fgets(line, 256, maps_f) != NULL)
	{
		unsigned long vas;
		unsigned long vae;

		/*scan for the virtual addresses*/
		int n = sscanf(line, "%lX-%lX", &vas, &vae);

		if(n != 2)
		{
			fprintf(stderr, "Invalid line read from %s\n", maps_s);
			continue;
		}

		if(!(ptr >= vas && ptr < vae))
			continue;

		int64_t index  = (ptr / PAGE_SIZE) * sizeof(int64_t);
		off64_t offset;

		offset = lseek64(pagemap_f, index, SEEK_SET);
		if (offset != index)
		{
			fprintf(stderr, "Error seeking to offset:");//%lli, index:%lli.\n", (long long int)offset, (long long int)index);
			exit(1);
		}

		uint64_t page_stat;

		ssize_t val = read(pagemap_f, &page_stat, sizeof(uint64_t));

		assert(val > 0);

		info.phys = ((page_stat & 0xffffffffffff) * PAGE_SIZE) | (ptr & 0xfff);
		info.swapped = (page_stat >> 62) & 1;

		close(pagemap_f);
		fclose(maps_f);

		return info;
	}

	fprintf(stderr, "process accessed unknown memeory\n");
	fprintf(stderr, "it is a bug in wrapper virt-to-phys parser or error in the program\n");
	fprintf(stderr, "physical access log is screwed\n");

	return info;
}

static uint64_t GetPhys(void* ptr)
{
	SInfo info = VirtToPhys(getpid(), reinterpret_cast<uint64_t>(ptr));

	if(info.swapped)
	{
		// try to get it
		volatile char tmp = reinterpret_cast<char*>(ptr)[0];

		printf("%c - from swap!\n", tmp);

		info = VirtToPhys(getpid(), reinterpret_cast<uint64_t>(ptr));

		if(info.swapped) // .. still swapped
		{
			fprintf(stderr, "we are in the Matrix, i can not go on\n");
			exit(0);
		}
	}
	return info.phys;
}

} /*namespace*/

#endif /*PHYSICAL_H*/
