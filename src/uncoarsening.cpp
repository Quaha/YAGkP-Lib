#include "uncoarsening.hpp"

#include "buckets.hpp"

namespace Uncoarser {

	Vector<Part> RestorePartition(const Vector<CoarseLevel>& levels, Vector<Part> partition, const int_t C1, const int_t C2) {
		switch (ProgramConfig::uncoarsening_method) {
		case ProgramConfig::UncoarseningMethod::DirectMapping:
			for (int_t i = levels.size() - 1; i > 0; i--) {
				partition = Uncoarser::DirectMapping(levels[i], partition);
			}
			break;

		case ProgramConfig::UncoarseningMethod::KernighanLin:
			for (int_t i = levels.size() - 1; i > 0; i--) {
				partition = Uncoarser::KernighanLinBlocking(levels[i - 1].coarsened_graph, levels[i], partition, C1, C2);
			}
			break;

		default:
			throw std::logic_error("Unknown processing method");
		}

		return partition;
	}

	Vector<Part> DirectMapping(const CoarseLevel& coarse_level, const Vector<Part>& coarse_partition) {
		const int_t n = coarse_level.uncoarse_to_coarse.size();
		Vector<Part> prev_partition(n);

		for (int_t i = 0; i < n; i++) {
			prev_partition[i] = coarse_partition[coarse_level.uncoarse_to_coarse[i]];
		}

		return prev_partition;
	}

	inline int_t GetGain(int_t V, const Graph& graph, const Vector<Part>& partition) {
		int_t gain = 0;
		for (auto [next_V, w]: graph[V]) {
			if (partition[V] != partition[next_V]) {
				gain += w;
			}
			else {
				gain -= w;
			}
		}
		return gain;
	}

	inline void UpdateNearGains(int_t V, const Graph& graph, const Vector<Part>& partition, const Vector<bool>& blocked, BucketPQ& heap1, BucketPQ& heap2) {
		for (auto [near_V, w]: graph[V]) {
			if (blocked[near_V]) {
				continue;
			}
			int_t delta;
			if (partition[near_V] == partition[V]) {
				delta = -2 * w;
			}
			else {
				delta = 2 * w;
			}
			if (partition[near_V] == Part::First) {
				heap1.add(delta, near_V);
			}
			else {
				heap2.add(delta, near_V);
			}
		}
	}

	Vector<Part> KernighanLinBlocking(const Graph& graph, Vector<Part> current_partition,
	                                  const int_t C1, const int_t C2) {
		int_t n = graph.n;

		int_t max_possible_gain = 0;
		for (int_t curr_V = 0; curr_V < n; curr_V++) {
			int_t current_possible_gain = 0;
			for (auto [next_V, w]: graph[curr_V]) {
				current_possible_gain += w;
			}
			setmax(max_possible_gain, current_possible_gain);
		}

		{ // initial balance fixing
			int_t weight1 = 0;
			int_t weight2 = 0;

			Vector<bool> blocked(n, false);

			BucketPQ heap1(n, -max_possible_gain, max_possible_gain);
			BucketPQ heap2(n, -max_possible_gain, max_possible_gain);
			for (int_t curr_V = 0; curr_V < n; curr_V++) {
				int_t gain = GetGain(curr_V, graph, current_partition);

				if (current_partition[curr_V] == Part::First) {
					heap1.insert(gain, curr_V);
					weight1 += graph.getVertexWeight(curr_V);
				}
				else {
					heap2.insert(gain, curr_V);
					weight2 += graph.getVertexWeight(curr_V);
				}
			}

			while (weight1 > C1) {
				auto [gain, curr_V] = heap1.extract();
				blocked[curr_V]     = true;

				int weight = graph.getVertexWeight(curr_V);

				if (weight + weight2 <= C2) {
					weight1 -= weight;
					weight2 += weight;

					current_partition[curr_V] = GetOtherPart(current_partition[curr_V]);
					UpdateNearGains(curr_V, graph, current_partition, blocked, heap1, heap2);
				}
				else {
					break;
				}
			}
			while (weight2 > C2) {
				auto [gain, curr_V] = heap2.extract();
				blocked[curr_V]     = true;

				int weight = graph.getVertexWeight(curr_V);

				if (weight + weight1 <= C1) {
					weight2 -= weight;
					weight1 += weight;

					current_partition[curr_V] = GetOtherPart(current_partition[curr_V]);
					UpdateNearGains(curr_V, graph, current_partition, blocked, heap1, heap2);
				}
				else {
					break;
				}
			}
		}

		int_t current_edgecut = PartitionMetrics::GetEdgeCut(graph, current_partition);

		Vector<Part> best_partition = current_partition;
		int_t best_edgecut          = current_edgecut;

		for (int_t run_number = 0; run_number < ProgramConfig::uncoarsening_KernighanLin_runs; run_number++) {

			int_t weight1 = 0;
			int_t weight2 = 0;

			Vector<bool> blocked(n, false);

			BucketPQ heap1(n, -max_possible_gain, max_possible_gain);
			BucketPQ heap2(n, -max_possible_gain, max_possible_gain);
			for (int_t curr_V = 0; curr_V < n; curr_V++) {
				int_t gain = GetGain(curr_V, graph, current_partition);
				if (current_partition[curr_V] == Part::First) {
					heap1.insert(gain, curr_V);
					weight1 += graph.getVertexWeight(curr_V);
				}
				else {
					heap2.insert(gain, curr_V);
					weight2 += graph.getVertexWeight(curr_V);
				}
			}

			Vector<int_t> gains;
			Vector<int_t> vertices;

			int_t current_run_best_edgecut = current_edgecut;

			int_t waste_cnt = 0;

			while (waste_cnt < ProgramConfig::uncoarsening_KernighanLin_waste_limit) {

				std::pair<int_t, int_t> data;
				if (weight1 > C1) {
					if (heap1.empty()) {
						break;
					}
					data = heap1.extract();

					int weight = graph.getVertexWeight(data.second);
					if (weight2 + weight > C2) {
						continue;
					}
				}
				else if (weight2 > C2) {
					if (heap2.empty()) {
						break;
					}
					data = heap2.extract();

					int weight = graph.getVertexWeight(data.second);
					if (weight1 + weight > C1) {
						continue;
					}
				}
				else {
					if (heap1.empty() && heap2.empty()) {
						break;
					}
					else if (heap1.empty()) {
						data       = heap2.extract();
						int weight = graph.getVertexWeight(data.second);
						if (weight1 + weight > C1) {
							continue;
						}
					}
					else if (heap2.empty()) {
						data       = heap1.extract();
						int weight = graph.getVertexWeight(data.second);
						if (weight2 + weight > C2) {
							continue;
						}
					}
					else {
						auto [gain1, temp1] = heap1.top();
						auto [gain2, temp2] = heap2.top();

						if (weight1 >= weight2) {
							data       = heap1.extract();
							int weight = graph.getVertexWeight(data.second);
							if (weight2 + weight > C2) {
								continue;
							}
						}
						else {
							data       = heap2.extract();
							int weight = graph.getVertexWeight(data.second);
							if (weight1 + weight > C1) {
								continue;
							}
						}
					}
				}

				auto [gain, curr_V] = data;
				blocked[curr_V]     = true;

				if (gain < 0) {
					waste_cnt++;
				}
				else {
					waste_cnt = 0;
				}

				gains.push_back(gain);
				vertices.push_back(curr_V);

				current_edgecut -= gain;
				setmin(current_run_best_edgecut, current_edgecut);

				if (current_partition[curr_V] == Part::First) {
					weight1 -= graph.getVertexWeight(curr_V);
					weight2 += graph.getVertexWeight(curr_V);
				}
				else {
					weight1 += graph.getVertexWeight(curr_V);
					weight2 -= graph.getVertexWeight(curr_V);
				}
				current_partition[curr_V] = GetOtherPart(current_partition[curr_V]);

				UpdateNearGains(curr_V, graph, current_partition, blocked, heap1, heap2);
			}

			while (current_edgecut > current_run_best_edgecut) {
				int_t gain = gains.back();
				gains.pop_back();
				int_t curr_V = vertices.back();
				vertices.pop_back();

				current_edgecut += gain;
				current_partition[curr_V] = GetOtherPart(current_partition[curr_V]);
			}

			if (current_edgecut < best_edgecut) {
				best_edgecut   = current_edgecut;
				best_partition = current_partition;
			}
		}

		return best_partition;
	}

	Vector<Part> KernighanLinBlocking(const Graph& previous_graph, const CoarseLevel& coarse_level,
	                                  const Vector<Part>& coarse_partition, const int_t C1, const int_t C2) {
		return KernighanLinBlocking(previous_graph, DirectMapping(coarse_level, coarse_partition), C1, C2);
	}
} // namespace Uncoarser