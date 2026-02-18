#pragma once

#include "utils.hpp"

#include "graph.hpp"

#include "coarsening.hpp"
#include "bipartitioner.hpp"
#include "uncoarsening.hpp"
#include "post_processing.hpp"

#include "metrics.hpp"

class Partitioner {
public:

	template <typename vw_t, typename ew_t>
	static void GetGraphKPartition(
		const Graph<vw_t, ew_t>& graph,
		const int_t              k,
		      Vector<int_t>&     partition
	) {
		partition.resize(graph.n, -1);
		RecursivePartition<vw_t, ew_t>(graph, k, partition, 0);

		PostProcessor::FixPartitionDisbalance<vw_t, ew_t>(graph, k, partition);
	}

    template <typename vw_t, typename ew_t>
    static void RecursivePartition(
        const Graph<vw_t, ew_t>& graph,
        const int_t              k,
              Vector<int_t>&     partition,
              int_t              offset
    ) {
        if (k == 1) {
            std::fill(partition.begin(), partition.end(), offset);
            return;
        }

        Vector<CoarseLevel<vw_t, ew_t>> levels = Coarser::GetCoarseLevels(graph, k);

        const Graph<vw_t, ew_t>& coarse_graph = levels.back().coarsed_graph;

        Vector<int_t> coarse_partition;
   
        Bipartitioner::GetGraphBipartition(coarse_graph, coarse_partition);
		Uncoarser::RestorePartition<vw_t, ew_t>(levels, coarse_partition);

        Vector<int_t> left_part_vertices, right_part_vertices;
        for (int_t i = 0; i < graph.n; ++i) {
            if (coarse_partition[i] == 0) {
                left_part_vertices.push_back(i);
            }
            else {
                right_part_vertices.push_back(i);
            }
        }

        Graph<vw_t, ew_t> left_graph = graph.selectSubgraph(left_part_vertices);
        Graph<vw_t, ew_t> right_graph = graph.selectSubgraph(right_part_vertices);


        vw_t total_W = graph.getSumOfVertexWeights();
        vw_t left_W = left_graph.getSumOfVertexWeights();

        real_t total_parts = static_cast<real_t>(k);
        real_t ratio_left = static_cast<real_t>(left_W) / static_cast<real_t>(total_W);

        int_t left_k = std::min(k - 1, std::max<int_t>(1, std::round(total_parts * ratio_left)));
        int_t right_k = k - left_k;


        Vector<int_t> left_part(left_graph.n, -1);
        Vector<int_t> right_part(right_graph.n, -1);

        RecursivePartition<vw_t, ew_t>(left_graph, left_k, left_part, offset);
        RecursivePartition<vw_t, ew_t>(right_graph, right_k, right_part, offset + left_k);

        for (int_t i = 0; i < left_part_vertices.size(); ++i) {
            partition[left_part_vertices[i]] = left_part[i];
        }

        for (int_t i = 0; i < right_part_vertices.size(); ++i) {
            partition[right_part_vertices[i]] = right_part[i];
        }
    }
};