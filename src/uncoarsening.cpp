#include "uncoarsening.hpp"

namespace Uncoarser {

	Vector<Part> RestorePartition(const Vector<CoarseLevel>& levels, Vector<Part> partition) {
		switch (ProgramConfig::uncoarsening_method) {
		case ProgramConfig::UncoarseningMethod::DirectMapping:
			for (int_t i = levels.size() - 1; i > 0; i--) {
				partition = Uncoarser::DirectMapping(levels[i], partition);
			}
			break;
			
		case ProgramConfig::UncoarseningMethod::KernighanLin:
			if (ProgramConfig::uncoarsening_KernighanLin_use_blocking) {
				for (int_t i = levels.size() - 1; i > 0; i--) {
					partition = Uncoarser::KernighanLinBlocking(levels[i - 1].coarsed_graph, levels[i], partition);
				}
			}
			else {
				for (int_t i = levels.size() - 1; i > 0; i--) {
					partition = Uncoarser::KernighanLin(levels[i - 1].coarsed_graph, levels[i], partition);
				}
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

	Vector<Part> KernighanLin(const Graph& previous_graph, const CoarseLevel& coarse_level,
	                          const Vector<Part>& coarse_partition) {
		int_t n = previous_graph.n;

		Vector<Part> current_partition = DirectMapping(coarse_level, coarse_partition);
		int_t current_edgecut          = PartitionMetrics::GetEdgeCut(previous_graph, current_partition);

		Vector<Part> best_partition = current_partition;
		int_t best_edgecut          = current_edgecut;

		for (int_t run_number = 0; run_number < ProgramConfig::uncoarsening_KernighanLin_runs; run_number++) {

			IndexedHeap2<int_t, int_t> heap;
			for (int_t curr_V = 0; curr_V < n; curr_V++) {
				int_t gain = 0;
				for (auto [next_V, w]: previous_graph[curr_V]) {
					if (current_partition[curr_V] != current_partition[next_V]) {
						gain += w;
					}
					else {
						gain -= w;
					}
				}
				heap.insert(gain, curr_V);
			}

			Vector<int_t> gains;
			Vector<int_t> vertices;

			int_t current_run_best_edgecut = current_edgecut;

			int_t waste_cnt = 0;

			while (!heap.empty() && waste_cnt < ProgramConfig::uncoarsening_KernighanLin_waste_limit) {
				auto [gain, curr_V] = heap.extract();

				if (gain < 0) {
					waste_cnt++;
				}
				else {
					waste_cnt = 0;
				}

				gains.push_back(gain);
				vertices.push_back(curr_V);

				current_edgecut -= gain;
				current_run_best_edgecut = std::min(current_run_best_edgecut, current_edgecut);

				for (auto [next_V, w]: previous_graph[curr_V]) {
					if (current_partition[curr_V] == current_partition[next_V]) {
						heap.change(2 * w, next_V);
					}
					else {
						heap.change(-2 * w, next_V);
					}
				}

				current_partition[curr_V] = GetOtherPart(current_partition[curr_V]);
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

	Vector<Part> KernighanLinBlocking(const Graph& previous_graph, Vector<Part> current_partition) {
		int_t n               = previous_graph.n;
		int_t current_edgecut = PartitionMetrics::GetEdgeCut(previous_graph, current_partition);

		Vector<Part> best_partition = current_partition;
		int_t best_edgecut          = current_edgecut;

		for (int_t run_number = 0; run_number < ProgramConfig::uncoarsening_KernighanLin_runs; run_number++) {

			int_t weight1 = 0;
			int_t weight2 = 0;

			IndexedHeap2<int_t, int_t> heap1;
			IndexedHeap2<int_t, int_t> heap2;
			for (int_t curr_V = 0; curr_V < n; curr_V++) {
				int_t gain = 0;
				for (auto [next_V, w]: previous_graph[curr_V]) {
					if (current_partition[curr_V] != current_partition[next_V]) {
						gain += w;
					}
					else {
						gain -= w;
					}
				}
				if (current_partition[curr_V] == Part::First) {
					heap1.insert(gain, curr_V);
					weight1 += previous_graph.getVertexWeight(curr_V);
				}
				else {
					heap2.insert(gain, curr_V);
					weight2 += previous_graph.getVertexWeight(curr_V);
				}
			}

			Vector<int_t> gains;
			Vector<int_t> vertices;

			int_t current_run_best_edgecut = current_edgecut;

			int_t waste_cnt = 0;

			while (waste_cnt < ProgramConfig::uncoarsening_KernighanLin_waste_limit) {

				std::pair<int_t, int_t> data;
				if (weight1 > weight2) {
					if (heap1.empty()) {
						break;
					}
					data = heap1.extract();
				}
				else {
					if (heap2.empty()) {
						break;
					}
					data = heap2.extract();
				}

				auto [gain, curr_V] = data;

				if (gain < 0) {
					waste_cnt++;
				}
				else {
					waste_cnt = 0;
				}

				gains.push_back(gain);
				vertices.push_back(curr_V);

				current_edgecut -= gain;
				current_run_best_edgecut = std::min(current_run_best_edgecut, current_edgecut);

				for (auto [next_V, w]: previous_graph[curr_V]) {
					if (current_partition[curr_V] == current_partition[next_V]) {
						heap1.change(2 * w, next_V);
						heap2.change(2 * w, next_V);
					}
					else {
						heap1.change(-2 * w, next_V);
						heap2.change(-2 * w, next_V);
					}
				}

				if (current_partition[curr_V] == Part::First) {
					weight1 -= previous_graph.getVertexWeight(curr_V);
					weight2 += previous_graph.getVertexWeight(curr_V);
				}
				else {
					weight1 += previous_graph.getVertexWeight(curr_V);
					weight2 -= previous_graph.getVertexWeight(curr_V);
				}
				current_partition[curr_V] = GetOtherPart(current_partition[curr_V]);
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
	                                  const Vector<Part>& coarse_partition) {
		int_t n = previous_graph.n;

		Vector<Part> current_partition = DirectMapping(coarse_level, coarse_partition);
		int_t current_edgecut          = PartitionMetrics::GetEdgeCut(previous_graph, current_partition);

		Vector<Part> best_partition = current_partition;
		int_t best_edgecut          = current_edgecut;

		for (int_t run_number = 0; run_number < ProgramConfig::uncoarsening_KernighanLin_runs; run_number++) {

			int_t weight1 = 0;
			int_t weight2 = 0;

			IndexedHeap2<int_t, int_t> heap1;
			IndexedHeap2<int_t, int_t> heap2;
			for (int_t curr_V = 0; curr_V < n; curr_V++) {
				int_t gain = 0;
				for (auto [next_V, w]: previous_graph[curr_V]) {
					if (current_partition[curr_V] != current_partition[next_V]) {
						gain += w;
					}
					else {
						gain -= w;
					}
				}
				if (current_partition[curr_V] == Part::First) {
					heap1.insert(gain, curr_V);
					weight1 += previous_graph.getVertexWeight(curr_V);
				}
				else {
					heap2.insert(gain, curr_V);
					weight2 += previous_graph.getVertexWeight(curr_V);
				}
			}

			Vector<int_t> gains;
			Vector<int_t> vertices;

			int_t current_run_best_edgecut = current_edgecut;

			int_t waste_cnt = 0;

			while (waste_cnt < ProgramConfig::uncoarsening_KernighanLin_waste_limit) {

				std::pair<int_t, int_t> data;
				if (weight1 > weight2) {
					if (heap1.empty()) {
						break;
					}
					data = heap1.extract();
				}
				else {
					if (heap2.empty()) {
						break;
					}
					data = heap2.extract();
				}

				auto [gain, curr_V] = data;

				if (gain < 0) {
					waste_cnt++;
				}
				else {
					waste_cnt = 0;
				}

				gains.push_back(gain);
				vertices.push_back(curr_V);

				current_edgecut -= gain;
				current_run_best_edgecut = std::min(current_run_best_edgecut, current_edgecut);

				for (auto [next_V, w]: previous_graph[curr_V]) {
					if (current_partition[curr_V] == current_partition[next_V]) {
						heap1.change(2 * w, next_V);
						heap2.change(2 * w, next_V);
					}
					else {
						heap1.change(-2 * w, next_V);
						heap2.change(-2 * w, next_V);
					}
				}

				if (current_partition[curr_V] == Part::First) {
					weight1 -= previous_graph.getVertexWeight(curr_V);
					weight2 += previous_graph.getVertexWeight(curr_V);
				}
				else {
					weight1 += previous_graph.getVertexWeight(curr_V);
					weight2 -= previous_graph.getVertexWeight(curr_V);
				}
				current_partition[curr_V] = GetOtherPart(current_partition[curr_V]);
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

	Vector<Part> OldKernighanLin(const Graph& previous_graph, const CoarseLevel& coarse_level,
	                             const Vector<Part>& coarse_partition) {
		int_t n                     = previous_graph.n;
		Vector<Part> prev_partition = DirectMapping(coarse_level, coarse_partition);

		Vector<bool> blocked(n, false);

		IndexedHeap<int_t> heap(n);

		for (int_t start_V = 0; start_V < n; start_V++) {
			int_t gain = 0;

			if (blocked[start_V])
				continue;

			for (auto [next_V, w]: previous_graph[start_V]) {
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
			blocked[curr_V]         = true;

			if (priority > 0) {
				break;
			}

			prev_partition[curr_V] = GetOtherPart(prev_partition[curr_V]);

			for (auto [next_V, w1]: previous_graph[curr_V]) {
				if (!blocked[next_V]) {
					int_t gain = 0;
					for (auto [near_V, w2]: previous_graph[next_V]) {
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

} // namespace Uncoarser