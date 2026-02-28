#include "uncoarsening.hpp"

namespace Uncoarser {

	Vector<Part> RestorePartition(
		const Vector<CoarseLevel>& levels,
			  Vector<Part>         partition
	) {
		switch (ProgramConfig::uncoarsening_method) {
		case ProgramConfig::UncoarseningMethod::DirectMapping:
			for (int_t i = levels.size() - 1; i > 0; i--) {
				partition = Uncoarser::DirectMapping(levels[i], partition);
			}
			break;
		case ProgramConfig::UncoarseningMethod::KernighanLin:
			for (int_t i = levels.size() - 1; i > 0; i--) {
				partition = Uncoarser::KernighanLin(levels[i - 1].coarsed_graph, levels[i], partition);
			}
			break;
		}
		return partition;
	}

	Vector<Part> DirectMapping(
		const CoarseLevel&   coarse_level,
		const Vector<Part>& coarse_partition
	) {
		const int_t n = coarse_level.uncoarse_to_coarse.size();
		Vector<Part> prev_partition(n);

		for (int_t i = 0; i < n; i++) {
			prev_partition[i] = coarse_partition[coarse_level.uncoarse_to_coarse[i]];
		}

		return prev_partition;
	}

	Vector<Part> KernighanLin(
		const Graph&         previous_graph,
		const CoarseLevel&   coarse_level,
		const Vector<Part>& coarse_partition
	) {
		int_t n = previous_graph.n;
		Vector<Part> prev_partition = DirectMapping(coarse_level, coarse_partition);

		Vector<bool> blocked(n, false);
		if (ProgramConfig::uncoarsening_KernighanLin_use_blocking) {

			int_t total_weight0 = 0;
			int_t total_weight1 = 0;

			for (int_t i = 0; i < n; i++) {
				if (prev_partition[i] == Part::First) {
					total_weight0 += previous_graph.getVertexWeight(i);
				}
				else {
					total_weight1 += previous_graph.getVertexWeight(i);
				}
			}

			for (int_t i = 0; i < n; i++) {
				if (prev_partition[i] == Part::First && total_weight0 < total_weight1) {
					blocked[i] = true;

				}
				if (prev_partition[i] == Part::Second && total_weight1 < total_weight0) {
					blocked[i] = true;
				}
			}
		}

		IndexedHeap<int_t> heap(n);

		for (int_t start_V = 0; start_V < n; start_V++) {
			int_t gain = 0;

			if (blocked[start_V]) continue;

			for (auto [next_V, w] : previous_graph[start_V]) {
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

			prev_partition[curr_V] = GetOtherPart(prev_partition[curr_V]);

			for (auto [next_V, w1] : previous_graph[curr_V]) {
				if (!blocked[next_V]) {
					int_t gain = 0;
					for (auto [near_V, w2] : previous_graph[next_V]) {
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