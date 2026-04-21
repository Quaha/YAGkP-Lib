#include "partitioner.hpp"

namespace Partitioner {
	Vector<int_t> GetGraphKPartition(const Graph& graph, const int_t k) {

		fp_t opt_part_value = fp_t(graph.getSumOfVertexWeights()) / fp_t(k);

		const int M = std::max(int_t(opt_part_value * (1.0 + ProgramConfig::imbalance) + EPS), int_t(std::ceil(opt_part_value + EPS))); // max part weight

		int_t best_edgecut = std::numeric_limits<int_t>::max();
		Vector<int_t> best_partition;

		for (int_t i = 0; i < ProgramConfig::runs; ++i) {
			Vector<int_t> partition = RecursivePartition(graph, k, M, 0);

			int_t edgecut = PartitionMetrics::GetEdgeCut(graph, partition);

			if (edgecut < best_edgecut) {
				best_edgecut   = edgecut;
				best_partition = partition;
			}
		}

		return best_partition;
	}

	Vector<int_t> RecursivePartition(const Graph& graph, const int_t k, const int_t M, int_t offset) {
		Vector<int_t> partition(graph.n, offset);
		if (k == 1) {
			return partition;
		}

		Vector<CoarseLevel> coarse_levels = Coarser::GetCoarseLevels(graph, k);
		const Graph& coarsened_graph      = coarse_levels.back().coarsened_graph;

		int_t k1 = k / 2;
		int_t k2 = k - k1;

		int_t C1 = M * k1;
		int_t C2 = M * k2;

		Vector<Part> initial_partition  = Bipartitioner::GetGraphBipartition(coarsened_graph, C1, C2);
		Vector<Part> restored_partition = Uncoarser::RestorePartition(coarse_levels, initial_partition, C1, C2);

		Vector<int_t> first_part_vertices, second_part_vertices;
		for (int_t i = 0; i < graph.n; ++i) {
			if (restored_partition[i] == Part::First) {
				first_part_vertices.push_back(i);
			}
			else {
				second_part_vertices.push_back(i);
			}
		}

		Graph first_graph  = graph.selectSubgraph(first_part_vertices);
		Graph second_graph = graph.selectSubgraph(second_part_vertices);

		Vector<int_t> partition1 = RecursivePartition(first_graph, k1, M, offset);
		Vector<int_t> partition2 = RecursivePartition(second_graph, k2, M, offset + k1);

		for (int_t i = 0; i < (int_t)first_part_vertices.size(); i++) {
			partition[first_part_vertices[i]] = partition1[i];
		}
		for (int_t i = 0; i < (int_t)second_part_vertices.size(); i++) {
			partition[second_part_vertices[i]] = partition2[i];
		}

		return partition;
	}
} // namespace Partitioner