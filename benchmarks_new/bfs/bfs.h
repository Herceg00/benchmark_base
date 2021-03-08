#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <stdexcept>
#include <cmath>
#include <limits>

typedef std::pair<int, float> edge;

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
void Init(EDGES_AT edges, size_t edge_count, INDEX_AT index, WEIGHT_AT weights, size_t vertex_count, size_t graph_scale,INDEX_AT v_array,INDEX_AT e_array)
{
	GenEdges(edges, edge_count, graph_scale);
	GenWeights(weights, edge_count);
	SortEdges(edges, edge_count);
	GenEdgeIndex(edges, edge_count, vertex_count, index);

    std::vector<std::vector<edge> > graph_info(vertices_count + 1);

    for (long long int i = 0; i < edges_count; i++) {
        int src_id = edges[i][0];
        int dst_id = edges[i][1];
        float weight = _weigths[i];
        graph_info[src_id].push_back(std::pair<int, float>(dst_id, weight));
    }

    int current_edge = 0;
    e_array[0] = 0;
    for (int cur_vertex = 0; cur_vertex < vertices_count; cur_vertex++) {
        int src_id = cur_vertex;

        for (int i = 0; i < graph_info[src_id].size(); i++) {
            e_array[current_edge] = graph_info[src_id][i].first;
            weights[current_edge] = graph_info[src_id][i].second;
            current_edge++;
        }
        v_array[cur_vertex + 1] = current_edge;
    }
    v_array[0] = 0;
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


template <typename EDGES_AT, typename WEIGHT_AT, typename INDEX_AT>
void Kernel(int core_type, EDGES_AT edges, size_t edge_count, WEIGHT_AT weights, size_t vertex_count, WEIGHT_AT d, INDEX_AT v_array, INDEX_AT e_array, WEIGHT_AT _levels)
{
    // Mark all the vertices as not visited
    for(int i = 0; i < vertices_count; i++)
        _levels[i] = 0;

    // Create a queue for BFS
    list<int> queue;

    // Mark the current node as visited and enqueue it
    _levels[_source_vertex] = 1;
    queue.push_back(_source_vertex);

    while(!queue.empty())
    {
        // Dequeue a vertex from queue and print it
        int s = queue.front();
        queue.pop_front();

        const long long edge_start = vertex_pointers[s];
        const int connections_count = vertex_pointers[s + 1] - vertex_pointers[s];

        for(int edge_pos = 0; edge_pos < connections_count; edge_pos++)
        {
            long long int global_edge_pos = edge_start + edge_pos;
            int v = adjacent_ids[global_edge_pos];
            if (_levels[v] == 0)
            {
                _levels[v] = _levels[s] + 1;
                queue.push_back(v);
            }
        }
    }

}

#endif
