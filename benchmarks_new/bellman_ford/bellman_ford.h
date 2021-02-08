#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <stdexcept>
#include <cmath>
#include <limits>

template<typename EDGES_AT>
void GenEdges(EDGES_AT edges, size_t edge_count, size_t graph_scale)
{
	const double A = 0.57;
	const double B = 0.19;
	const double C = 0.19;

	for(size_t i = 0; i < edge_count; i++)
	{
		int i1 = 0;
		int i2 = 0;

		for(int j = graph_scale - 1; j >= 0; j--)
		{
			double r = locality::utils::RRand(i * graph_scale + j);

			if(r < A){}
			else if(r < A + B)
			{
				i1 += 1 << j;
			}
			else if(r < A + B + C)
			{
				i2 += 1 << j;
			}
			else
			{
				i1 += 1 << j;
				i2 += 1 << j;
			}

		}

		edges[i][0] = i1;
		edges[i][1] = i2;
	}
}

template<typename EDGES_AT>
void PrintEdges(EDGES_AT edges, size_t edge_count)
{
	for(size_t i = 0; i < edge_count; i++)
	{
		std::cout << edges[i][0] << " " << edges[i][1] << std::endl;
	}

	std::cout << std::endl;
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

/**
gen index for sorted list of edges
*/
template<typename EDGES_AT, typename INDEX_AT>
void GenEdgeIndex(EDGES_AT edges, size_t edge_count, size_t vertex_count, INDEX_AT index)
{

	for(size_t i = 0; i < vertex_count; i++)
		index[i] = -1;

	size_t current_vertex = edges[0][0];
	size_t offset = 0;

	index[current_vertex] = 0;

	while(true)
	{
		while(edges[offset][0] == current_vertex)
		{
			offset++;
			if(offset >= edge_count)
				break;
		}

		if(offset >= edge_count)
			break;

		index[edges[offset][0]] = offset;

		current_vertex = edges[offset][0];
	}
}

static const int MAX_WEIGHT = 100;

template<typename WEIGHT_AT>
void GenWeights(WEIGHT_AT weights, size_t vertex_count)
{
	for(size_t i = 0; i < vertex_count; i++)
		weights[i] = locality::utils::RRand(i) * MAX_WEIGHT + 1;
}

template<typename EDGES_AT, typename INDEX_AT, typename WEIGHT_AT>
void Init(EDGES_AT edges, size_t edge_count, INDEX_AT index, WEIGHT_AT weights, size_t vertex_count, size_t graph_scale)
{
	GenEdges(edges, edge_count, graph_scale);
	GenWeights(weights, edge_count);
	SortEdges(edges, edge_count);
	GenEdgeIndex(edges, edge_count, vertex_count, index);
}

template <typename WEIGHT_AT>
long Check(WEIGHT_AT result, size_t vertex_count)
{
	long sum = 0;

	for(size_t i = 0; i < vertex_count; i++)
	{
		sum += result[i];
		sum = sum % 0xffff;
	}

	return sum;
}

template <typename EDGES_AT, typename INDEX_AT, typename WEIGHT_AT>
void Kernel(EDGES_AT edges, size_t edge_count, INDEX_AT, WEIGHT_AT weights, size_t vertex_count, WEIGHT_AT d)
{
	for(size_t i = 0; i < vertex_count; i++)
		d[i] = -1; 

	d[0] = 0;

	for(size_t i = 1; i < vertex_count; i++)
		for(size_t h = 0; h < edge_count; h++)
		{
			int tmp = d[edges[h][0]] + weights[h];
			if(d[i] == -1 || d[edges[h][1]] > tmp)
				d[i] = tmp;
		}

}

#endif
