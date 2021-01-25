// http://www.geeksforgeeks.org/greedy-algorithms-set-9-boruvkas-algorithm/

#ifndef BORUVKA_H
#define BORUVKA_H

#include <stdexcept>
#include <cmath>
#include <limits>
#include <iostream>


template<typename EDGES_AT>
void PrintEdges(EDGES_AT edges, size_t edge_count)
{
	for(size_t i = 0; i < edge_count; i++)
	{
		std::cout << edges[i][0] << " " << edges[i][1] << std::endl;
	}

	std::cout << std::endl;
}

template<typename EDGES_AT>
bool present(EDGES_AT edges, size_t edge_count, size_t from, size_t to)
{
	for(size_t i = 0; i < edge_count; i++)
	{
		if(edges[i][0] == from && edges[i][1] == to)
			return true;
		if(edges[i][1] == from && edges[i][0] == to)
			return true;
	}

	return false;
}

template<typename EDGES_AT>
void GenEdges(EDGES_AT edges, size_t edge_count, size_t vertex_count)
{
	size_t counter = 0;

	for(size_t i = 0; i < edge_count; i++)
	{
		size_t from = 0;
		size_t to = 0;

		while(from == to || present(edges, i, from, to))
		{
			from = locality::utils::RRand(counter * 2 + 0) * (vertex_count);
			to = locality::utils::RRand(counter * 2 + 1) * (vertex_count);
			counter++;
			
			if(counter > edge_count * 10)
				break;
		}

		edges[i][0] = from;
		edges[i][1] = to;
	}
}

/**
sort edges list by first vertex
*/
template<typename EDGES_AT>
void SortEdges(EDGES_AT edges, size_t edge_count)
{
	for (size_t left_index = 0, right_index = edge_count - 1; left_index < right_index;)
	{
		for (size_t index = left_index; index < right_index; index++)
		{
			if (edges[index + 1][0] < edges[index][0])
			{
				std::swap(edges[index][0], edges[index + 1][0]);
				std::swap(edges[index][1], edges[index + 1][1]);
			}
		}
		right_index--;

		for (size_t index = right_index; index > left_index; index--)
		{
			if (edges[index - 1][0] >  edges[index][0])
			{
				std::swap(edges[index - 1][0], edges[index][0]);
				std::swap(edges[index - 1][1], edges[index][1]);
			}
		}
		left_index++;
	}
}

static const int MAX_WEIGHT = 100;

template<typename WEIGHT_AT>
void GenWeights(WEIGHT_AT weights, size_t vertex_count)
{
	for(size_t i = 0; i < vertex_count; i++)
		weights[i] = locality::utils::RRand(i) * MAX_WEIGHT + 1;
}

template<typename EDGES_AT, typename WEIGHT_AT>
void Init(EDGES_AT edges, size_t edge_count, WEIGHT_AT weights, size_t vertex_count)
{
	GenEdges(edges, edge_count, vertex_count);
	GenWeights(weights, edge_count);
	SortEdges(edges, edge_count);
}

template <typename SUBSET_AT>
long Check(SUBSET_AT result, size_t vertex_count)
{
	long sum = 0;

	for(size_t i = 0; i < vertex_count; i++)
	{
		sum += result[i].parent;
		sum = sum % 0xffff;
	}

	return sum;
}

// A structure to represent a subset for union-find
struct SSubset
{
	int parent;
	int rank;
};

// A utility function to find set of an element i
// (uses path compression technique)
template <typename SUBSET_AT>
int Find(SUBSET_AT subsets, int i)
{
	// find root and make root as parent of i
	// (path compression)
	if (subsets[i].parent != i)
	  subsets[i].parent = Find(subsets, subsets[i].parent);
 
	return subsets[i].parent;
}
 
// A function that does union of two sets of x and y
// (uses union by rank)
template <typename SUBSET_AT>
void Union(SUBSET_AT subsets, int x, int y)
{
	int xroot = Find(subsets, x);
	int yroot = Find(subsets, y);
 
	// Attach smaller rank tree under root of high
	// rank tree (Union by Rank)
	if (subsets[xroot].rank < subsets[yroot].rank)
		subsets[xroot].parent = yroot;
	else if (subsets[xroot].rank > subsets[yroot].rank)
		subsets[yroot].parent = xroot;
 
	// If ranks are same, then make one as root and
	// increment its rank by one
	else
	{
		subsets[yroot].parent = xroot;
		subsets[xroot].rank++;
	}
}

template <typename EDGES_AT, typename SUBSET_AT, typename INDEX_AT, typename WEIGHT_AT>
void Kernel(EDGES_AT edges, size_t edge_count, SUBSET_AT subsets, INDEX_AT cheapest, WEIGHT_AT weights, size_t vertex_count)
{
	LOC_PAPI_BEGIN_BLOCK
 
	// Create vertex_count subsets with single elements
	for (size_t v = 0; v < vertex_count; ++v)
	{
		subsets[v].parent = v;
		subsets[v].rank = 0;
		cheapest[v] = -1;
	}
 
	// Initially there are vertex_count different trees.
	// Finally there will be one tree that will be MST
	int numTrees = vertex_count;
	int MSTweight = 0;
 
	// Keep combining components (or sets) until all
	// compnentes are not combined into single MST.
	while (numTrees > 1)
	{
		for (size_t v = 0; v < vertex_count; ++v)
		{
			cheapest[v] = -1;
		}
		// Traverse through all edges and update
		// cheapest of every component
		for (size_t i=0; i<edge_count; i++)
		{
			// Find components (or sets) of two corners
			// of current edge
			int set1 = Find(subsets, edges[i][0]);
			int set2 = Find(subsets, edges[i][1]);
 
			// If two corners of current edge belong to
			// same set, ignore current edge
			if (set1 == set2)
				continue;
 
			// Else check if current edge is closer to previous
			// cheapest edges of set1 and set2

			else
			{
				int w = weights[i];	

				if (cheapest[set1] == -1 || weights[cheapest[set1]] > w)
					cheapest[set1] = (int)i;
 
				if (cheapest[set2] == -1 || weights[cheapest[set2]] > w)
					cheapest[set2] = (int)i;
			}
		}

		int old_num = numTrees;

		// Consider the above picked cheapest edges and add them
		// to MST
		for (size_t i=0; i<vertex_count; i++)
		{
			// Check if cheapest for current set exists
			if (cheapest[i] != -1)
			{
				int set1 = Find(subsets, edges[cheapest[i]][0]);
				int set2 = Find(subsets, edges[cheapest[i]][1]);
 
				if (set1 == set2)
					continue;

				MSTweight += weights[cheapest[i]];

				//printf("Edge %ld-%ld included in MST\n", edges[cheapest[i]][0], edges[cheapest[i]][1]);
				// Do a union of set1 and set2 and decrease number
				// of trees
				Union(subsets, set1, set2);
				numTrees--;
			}
		}

		if(numTrees == old_num)
		{
			std::cout << "bad ending, " << 100 * numTrees / vertex_count << "% skipped :(" << std::endl;
		}
	}
 
	LOC_PAPI_END_BLOCK
}

#endif
