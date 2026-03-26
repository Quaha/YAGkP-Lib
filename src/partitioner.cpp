#include "partitioner.hpp"

namespace Partitioner {
	Vector<int_t> GetGraphKPartition(const Graph& graph, const int_t k) {
		vector<int_t> partition = RecursivePartition(graph, k, 0);
		PostProcessor::FixPartitionDisbalance(graph, k, partition);
		return partition;
	}

	Vector<int_t> RecursivePartition(const Graph& graph, const int_t k, int_t offset) {
		Vector<int_t> partition(graph.n, offset);
		if (k == 1) {
			return partition;
		}

		Vector<CoarseLevel> coarse_levels = Coarser::GetCoarseLevels(graph, k);
		const Graph& coarsed_graph        = coarse_levels.back().coarsed_graph;

		Vector<Part> restored_partition;

		Vector<Part> initial_partition = Bipartitioner::GetGraphBipartition(coarsed_graph);
		restored_partition             = Uncoarser::RestorePartition(coarse_levels, initial_partition);

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

		int_t total_W       = graph.getSumOfVertexWeights();
		int_t first_graph_W = first_graph.getSumOfVertexWeights();

		double ratio_left = (double)(first_graph_W) / (double)(total_W);

		int_t k1 = std::min(k - 1, std::max<int_t>(1, std::round((double)(k)*ratio_left)));
		int_t k2 = k - k1;

		Vector<int_t> partition1 = RecursivePartition(first_graph, k1, offset);
		Vector<int_t> partition2 = RecursivePartition(second_graph, k2, offset + k1);

		for (int_t i = 0; i < first_part_vertices.size(); i++) {
			partition[first_part_vertices[i]] = partition1[i];
		}
		for (int_t i = 0; i < second_part_vertices.size(); i++) {
			partition[second_part_vertices[i]] = partition2[i];
		}

		return partition;
	}
}; // namespace Partitioner