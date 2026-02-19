#include "uncoarsening.hpp"

namespace Uncoarser {

	void RestorePartition(
		const Vector<CoarseLevel>& levels,
			  Vector<int_t>&       partition
	) {
		switch (ProgramConfig::uncoarsening_method) {
		case ProgramConfig::UncoarseningMethod::DirectMapping:
			for (int_t i = levels.size() - 1; i > 0; i--) {
				partition = Uncoarser::DirectMapping(levels[i - 1], levels[i], partition);
			}
			break;
		case ProgramConfig::UncoarseningMethod::KernighanLin:
			for (int_t i = levels.size() - 1; i > 0; i--) {
				partition = Uncoarser::KernighanLin(levels[i - 1], levels[i], partition);
			}
			break;

		default:
			throw std::runtime_error("Unknown uncoarsening method in ProgramConfig.");
		}
	}

	Vector<int_t> DirectMapping(
		const CoarseLevel&   prev_level,
		const CoarseLevel&   level,
		const Vector<int_t>& coarse_partition
	) {
		const int_t n = level.uncoarse_to_coarse.size();
		Vector<int_t> prev_partition(n);

		for (int_t i = 0; i < n; i++) {
			prev_partition[i] = coarse_partition[level.uncoarse_to_coarse[i]];
		}

		return prev_partition;
	}

	Vector<int_t> KernighanLin(
		const CoarseLevel&   prev_level,
		const CoarseLevel&   level,
		const Vector<int_t>& coarse_partition
	) {
		int_t n = level.uncoarse_to_coarse.size();
		Vector<int_t> prev_partition = DirectMapping(prev_level, level, coarse_partition);
		const Graph& graph = prev_level.coarsed_graph;

		Vector<bool> blocked(n, false);
		if (ProgramConfig::uncoarsening_KernighanLin_use_blocking) {

			int_t total_weight0 = 0;
			int_t total_weight1 = 0;

			for (int_t i = 0; i < n; i++) {
				if (prev_partition[i] == 0) {
					total_weight0 += graph.getVertexWeight(i);
				}
				else {
					total_weight1 += graph.getVertexWeight(i);
				}
			}

			for (int_t i = 0; i < n; i++) {
				if (prev_partition[i] == 0 && total_weight0 < total_weight1) {
					blocked[i] = true;

				}
				if (prev_partition[i] == 1 && total_weight1 < total_weight0) {
					blocked[i] = true;
				}
			}
		}

		IndexedHeap<int_t> heap(n);

		for (int_t start_V = 0; start_V < n; start_V++) {
			int_t gain = 0;

			if (blocked[start_V]) continue;

			for (auto [next_V, w] : graph[start_V]) {
				if (prev_partition[next_V] == prev_partition[start_V]) {
					gain += w;
				}
				else {
					gain -= w;
				}
			}

			heap.push(gain, start_V);
		}

		while (!heap.empty()) {
			auto [priority, curr_V] = heap.extract();
			blocked[curr_V] = true;

			if (priority > 0) {
				break;
			}

			prev_partition[curr_V] = 1 - prev_partition[curr_V];

			for (auto [next_V, w1] : graph[curr_V]) {
				if (!blocked[next_V]) {
					int_t gain = 0;
					for (auto [near_V, w2] : graph[next_V]) {
						if (prev_partition[near_V] == prev_partition[next_V]) {
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

		return prev_partition;
	}
};