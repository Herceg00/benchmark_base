#ifndef BFS_H
#define BFS_H

#include <stdexcept>
#include <cmath>
#include <limits>
#include <list>

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

template<typename EDGES_AT, typename INDEX_AT>
void Init(EDGES_AT edges, size_t edge_count, INDEX_AT index, size_t vertex_count, size_t graph_scale, INDEX_AT v_array,INDEX_AT e_array)
{
	GenEdges(edges, edge_count, graph_scale);
	SortEdges(edges, edge_count);
	GenEdgeIndex(edges, edge_count, vertex_count, index);

    std::vector<std::vector<int>> graph_info(vertex_count + 1);

    for (size_t i = 0; i < edge_count; i++) {
        int src_id = edges[i][0];
        int dst_id = edges[i][1];
        graph_info[src_id].push_back(dst_id);
    }

    int current_edge = 0;
    e_array[0] = 0;
    for (size_t cur_vertex = 0; cur_vertex < vertex_count; cur_vertex++) {
        int src_id = cur_vertex;
        for (std::vector<int>::size_type i = 0; i < graph_info[src_id].size(); i++) {
            e_array[current_edge] = graph_info[src_id][i];
            current_edge++;
        }
        v_array[cur_vertex + 1] = current_edge;
    }
    v_array[0] = 0;
}

template <typename EDGES_AT, typename WEIGHT_AT, typename INDEX_AT>
void Kernel(size_t vertex_count, INDEX_AT v_array, INDEX_AT e_array, WEIGHT_AT _levels)
{
    int _source_vertex;
    while(true) {
        _source_vertex = rand()%vertex_count;
        if (v_array[_source_vertex + 1] - v_array[_source_vertex] > 0)
            break;
    }
    // Mark all the vertices as not visited
    for(size_t i = 0; i < vertex_count; i++)
        _levels[i] = 0;

    // Create a queue for BFS
    std::list<int> queue;

    // Mark the current node as visited and enqueue it
    _levels[_source_vertex] = -1;
    queue.push_back(_source_vertex);

    while(!queue.empty())
    {
        // Dequeue a vertex from queue and print it
        int s = queue.front();
        queue.pop_front();

        const long long edge_start = v_array[s];
        const int connections_count = v_array[s + 1] - v_array[s];

        for(int edge_pos = 0; edge_pos < connections_count; edge_pos++)
        {
            long long int global_edge_pos = edge_start + edge_pos;
            int v = e_array[global_edge_pos];
            if (_levels[v] == -1)
            {
                _levels[v] = _levels[s] + 1;
                queue.push_back(v);
            }
        }
    }
}

#endif
