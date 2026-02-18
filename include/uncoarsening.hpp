#pragma once

#include <numeric>

#include "config.hpp"

#include "utils.hpp"
#include "graph.hpp"

#include "coarse_level.hpp"

class Uncoarser {
public:

	template <typename vw_t, typename ew_t>
	static void RestorePartition(
		const Vector<CoarseLevel<vw_t, ew_t>>& levels,
			  Vector<int_t>&                   partition
	) {
		switch (ProgramConfig::uncoarsening_method) {
		case ProgramConfig::UncoarseningMethod::DirectMapping:
			for (int_t i = levels.size() - 1; i > 0; --i) {
				partition = Uncoarser::DirectMapping<vw_t, ew_t>(levels[i - 1], levels[i], partition);
			}
			break;
		case ProgramConfig::UncoarseningMethod::KernighanLin:
			for (int_t i = levels.size() - 1; i > 0; --i) {
				partition = Uncoarser::KernighanLin<vw_t, ew_t>(levels[i - 1], levels[i], partition);
			}
			break;

		default:
			throw std::runtime_error("Unknown uncoarsening method in ProgramConfig.");
		}
	}

	template <typename vw_t, typename ew_t>
	static Vector<int_t> DirectMapping(
		const CoarseLevel<vw_t, ew_t>& prev_level,
		const CoarseLevel<vw_t, ew_t>& level,
		const Vector<int_t>&		   coarse_partition
	) {
		const int_t n = level.uncoarse_to_coarse.size();
		Vector<int_t> prev_partition(n);

		for (int_t i = 0; i < n; ++i) {
			prev_partition[i] = coarse_partition[level.uncoarse_to_coarse[i]];
		}

		return prev_partition;
	}

	template <typename vw_t, typename ew_t>
	static Vector<int_t> KernighanLin(
		const CoarseLevel<vw_t, ew_t>& prev_level,
		const CoarseLevel<vw_t, ew_t>& level,
		const Vector<int_t>& coarse_partition
	) {
		const int_t n = level.uncoarse_to_coarse.size();

		Vector<int_t> prev_partition = DirectMapping<vw_t, ew_t>(prev_level, level, coarse_partition);

		const Graph<vw_t, ew_t>& graph = prev_level.coarsed_graph;

		Vector<bool> blocked(n, false);

		if (ProgramConfig::uncoarsening_KernighanLin_use_blocking) {

			vw_t total_weight0 = c<vw_t>(0);
			vw_t total_weight1 = c<vw_t>(0);

			for (int_t i = 0; i < n; ++i) {
				if (prev_partition[i] == 0) {
					total_weight0 += graph.getVertexWeight(i);
				}
				else {
					total_weight1 += graph.getVertexWeight(i);
				}
			}

			for (int_t i = 0; i < n; ++i) {
				if (prev_partition[i] == 0 && total_weight0 < total_weight1) {
					blocked[i] = true;

				}
				if (prev_partition[i] == 1 && total_weight1 < total_weight0) {
					blocked[i] = true;
				}
			}
		}

		IndexedHeap<ew_t> heap(n);

		for (int_t start_V = 0; start_V < n; ++start_V) {
			ew_t inc_w = c<ew_t>(0);
			ew_t dec_w = c<ew_t>(0);

			if (blocked[start_V]) continue;

			for (auto [next_V, w] : graph[start_V]) {
				if (prev_partition[next_V] == prev_partition[start_V]) {
					inc_w += w;
				}
				else {
					dec_w += w;
				}
			}

			heap.push(inc_w - dec_w, start_V);
		}

		while (!heap.empty()) {
			auto [priority, curr_V] = heap.extract();
			blocked[curr_V] = true;

			if (priority > c<ew_t>(0)) {
				break;
			}

			prev_partition[curr_V] = 1 - prev_partition[curr_V];

			for (auto [next_V, w1] : graph[curr_V]) {
				if (!blocked[next_V]) {
					ew_t inc_w = c<ew_t>(0);
					ew_t dec_w = c<ew_t>(0);
					for (auto [near_V, w2] : graph[next_V]) {
						if (prev_partition[near_V] == prev_partition[next_V]) {
							inc_w += w2;
						}
						else {
							dec_w += w2;
						}
					}
					heap.push(inc_w - dec_w, next_V);
				}
			}
		}

		return prev_partition;
	}
};