#include "bipartitioner.hpp"

#include <iostream>

namespace Bipartitioner {

	Vector<Part> GetGraphBipartition(const Graph& graph, const int_t C1, const int_t C2) {
		switch (ProgramConfig::bipartitioning_method) {
		case (ProgramConfig::BipartitioningMethod::GraphGrowingAlgorithm):
			return GraphGrowingAlgorithm(graph, C1, C2);

		case (ProgramConfig::BipartitioningMethod::GreedyGraphGrowingAlgorithm):
			return GreedyGraphGrowingAlgorithm(graph, C1, C2);
		case (ProgramConfig::BipartitioningMethod::FlowAlgorithm):
			return FlowAlgorithm(graph);

		default:
			throw std::logic_error("Unknown processing method");
		}
	}

	Vector<Part> GraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2) {
		const int_t n = graph.n;

		int_t total_weight = graph.getSumOfVertexWeights();

		Vector<Part> best_partition;
		int_t best_edge_cut = std::numeric_limits<int_t>::max();

		for (int_t i = 0; i < ProgramConfig::bipartitioning_launches_count; ++i) {

			Vector<Part> partition(n, Part::Second);
			Vector<bool> visited(n, false);

			Queue<int_t> q;
			q.reserve(n / 2);

			Vector<int_t> order = GetRandomPermutation(n);

			for (int_t start_V: order) {
				if (graph.vertex_weights[start_V] <= C1) {
					q.push(start_V);
					visited[start_V] = true;
					break;
				}
			}

			int_t current_weight = 0;

			while (!q.empty()) {
				int_t curr_V = q.front();
				q.pop();

				if (current_weight + graph.vertex_weights[curr_V] > C1) {
					continue;
				}

				partition[curr_V] = Part::First;
				current_weight += graph.vertex_weights[curr_V];

				for (auto [next_V, w]: graph[curr_V]) {
					if (!visited[next_V]) {
						visited[next_V] = true;
						q.push(next_V);
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

	Vector<Part> GreedyGraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2) {
		const int_t n = graph.n;

		Vector<Part> best_partition;
		int_t best_edge_cut = std::numeric_limits<int_t>::max();

		for (int_t i = 0; i < ProgramConfig::bipartitioning_launches_count; ++i) {

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

	Vector<Part> FlowAlgorithm(const Graph& graph) {
		int n = graph.n;

		Vector<Part> best_partition;
		int_t best_edge_cut = std::numeric_limits<int_t>::max();

		for (int_t i = 0; i < ProgramConfig::bipartitioning_launches_count; ++i) {

			auto [s, t] = GetDifferentRandomInts(n);

			int_t max_possible_flow = 0;
			Vector<std::map<int, int>> flows(n);
			for (int_t u = 0; u < n; u++) {
				for (auto [v, c]: graph[u]) {
					if (u < v) {
						max_possible_flow += c;
					}
					flows[u][v] = 0;
				}
			}

			Vector<int_t> used(n, 0);
			int_t tmr = 0;

			auto dfs = [&](auto self, int_t curr_V, int_t min_delta, int_t lowest_add)->int_t {
				if (used[curr_V] == tmr) {
					return 0;
				}
				if (curr_V == t) {
					return min_delta;
				}
				used[curr_V] = tmr;
				for (auto [next_V, cap]: graph[curr_V]) {
					int f = flows[curr_V][next_V];
					
					if (cap - f >= lowest_add) {
						int delta = self(self, next_V, std::min(min_delta, cap - f), lowest_add);
						if (delta > 0) {
							flows[curr_V][next_V] += delta;
							flows[next_V][curr_V] -= delta;
							return delta;
						}
					}
				}
				return 0;
			};

			int flow = 0;

			while (max_possible_flow > 0) {

				while (true) {
					++tmr;
					int delta = dfs(dfs, s, std::numeric_limits<int_t>::max(), max_possible_flow);

					if (delta == 0) {
						break;
					}
					flow += delta;
				}

				max_possible_flow /= 2;
			}

			if (flow < best_edge_cut) {
				best_edge_cut = flow;

				best_partition.assign(n, Part::Second);

				Queue<int_t> q;
				used[s] = tmr + 1;
				q.push(s);

				while (!q.empty()) {
					int curr_V = q.front(); q.pop();

					best_partition[curr_V] = Part::First;
					for (auto [next_V, w]: graph[curr_V]) {
						if (used[next_V] == tmr) {
							q.push(next_V);
							used[next_V] = tmr + 1;
						}
					}
				}
			}
		}

		return best_partition;
	}
} // namespace Bipartitioner