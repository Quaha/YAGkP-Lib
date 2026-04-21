#include "bipartitioner.hpp"

#include <iostream>

namespace Bipartitioner {

	Vector<Part> GetGraphBipartition(const Graph& graph, const int_t C1, const int_t C2) {
		switch (ProgramConfig::bipartitioning_method) {
		case (ProgramConfig::BipartitioningMethod::GraphGrowingAlgorithm):
			return GraphGrowingAlgorithm(graph);

		case (ProgramConfig::BipartitioningMethod::GreedyGraphGrowingAlgorithm):
			return GreedyGraphGrowingAlgorithm(graph, C1, C2);

		default:
			throw std::logic_error("Unknown processing method");
		}
	}

	Vector<Part> GraphGrowingAlgorithm(const Graph& graph) {
		const int_t n = graph.n;

		int_t total_weight = graph.getSumOfVertexWeights();

		int_t ideal_weight = total_weight / 2;
		int_t max_allowed  = (ProgramConfig::imbalance + 1.0) * ideal_weight;

		Vector<Part> best_partition;
		int_t best_edge_cut;

		bool found = false;

		for (int_t i = 0; i < ProgramConfig::bipartitioning_GraphGrowingAlgorithm_launches_count; ++i) {

			Vector<Part> partition(n, Part::First);
			Vector<bool> visited(n, false);

			Queue<int_t> q;
			q.reserve(n / 2);

			Vector<int_t> order = GetRandomPermutation(n);

			for (int_t start_V: order) {
				if (graph.vertex_weights[start_V] <= max_allowed) {
					q.push(start_V);
					visited[start_V] = true;
					break;
				}
			}

			int_t current_weight = 0;

			while (!q.empty()) {
				int_t curr_V = q.front();
				q.pop();

				if (current_weight + graph.vertex_weights[curr_V] > max_allowed) {
					continue;
				}

				partition[curr_V] = Part::Second;
				current_weight += graph.vertex_weights[curr_V];

				for (auto [next_V, w]: graph[curr_V]) {
					if (!visited[next_V]) {
						visited[next_V] = true;
						q.push(next_V);
					}
				}
			}

			int_t edge_cut = PartitionMetrics::GetEdgeCut(graph, partition);

			if (!found || edge_cut < best_edge_cut) {
				found          = true;
				best_partition = partition;
				best_edge_cut  = edge_cut;
			}
		}
		return best_partition;
	}

	Vector<Part> GreedyGraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2) {
		const int_t n = graph.n;

		Vector<Part> best_partition;
		int_t best_edge_cut = std::numeric_limits<int_t>::max();

		for (int_t i = 0; i < ProgramConfig::bipartitioning_GreedyGraphGrowingAlgorithm_launches_count; ++i) {

			Vector<Part> partition(n, Part::Second);
			Vector<bool> blocked(n, false);

			int_t current_weight = 0;
			IndexedHeap<int_t> heap(n); // sort values in increasing order by value

			Vector<int_t> order = GetRandomPermutation(n);

			bool flag = true;
			while (flag) {
				flag = false;

				for (int_t V: order) {
					if (!blocked[V] && graph.getVertexWeight(V) + current_weight <= C1) {
						flag         = true;
						partition[V] = Part::First;

						blocked[V] = true;
						current_weight += graph.getVertexWeight(V);

						for (auto [next_V, w1]: graph[V]) {
							int_t gain = -w1;
							for (auto [near_V, w2]: graph[next_V]) {
								if (partition[near_V] == Part::Second) {
									gain += w2;
								}
							}
							heap.push(gain, next_V);
						}

						break;
					}
				}

				while (!heap.empty()) {
					auto [priority, curr_V] = heap.extract();

					blocked[curr_V] = true;

					if (current_weight + graph.vertex_weights[curr_V] > C1) {
						continue;
					}

					current_weight += graph.vertex_weights[curr_V];

					partition[curr_V] = Part::First;
					for (auto [next_V, w1]: graph[curr_V]) {
						if (blocked[next_V])
							continue;

						int_t gain = 0;

						for (auto [near_V, w2]: graph[next_V]) {
							if (partition[near_V] == Part::Second) {
								gain += w2;
							}
							else {
								gain -= w2;
							}
						}

						heap.push(gain, next_V);
					}
				}
			}

			int_t edge_cut = PartitionMetrics::GetEdgeCut(graph, partition);

			if (edge_cut < best_edge_cut) {
				best_partition = partition;
				best_edge_cut  = edge_cut;
			}
		}

		return best_partition;
	}
} // namespace Bipartitioner