#pragma once

#include "graph.hpp"
#include "kaHIP_interface.h"

namespace KaHIPPartitioner {

	inline Vector<int_t> GetGraphKPartition(const Graph& g, int_t k, double imbalance = 0.03) {
		int n = g.n;

		// KaHIP работает с int*, у тебя int_t — копируем если нужно
		Vector<int> xadj(g.xadj.begin(), g.xadj.end());
		Vector<int> adjncy(g.adjncy.begin(), g.adjncy.end());
		Vector<int> vwgt(g.vertex_weights.begin(), g.vertex_weights.end());

		// KaHIP принимает целые веса рёбер
		Vector<int> adjcwgt(g.edge_weights.size());
		for (size_t i = 0; i < g.edge_weights.size(); i++) {
			adjcwgt[i] = static_cast<int>(g.edge_weights[i]);
		}

		Vector<int_t> partition(n);
		int edge_cut = 0;
		int nparts   = static_cast<int>(k);

		kaffpa(
		    &n,
		    vwgt.data(),
		    xadj.data(),
		    adjcwgt.data(),
		    adjncy.data(),
		    &nparts,
		    &imbalance,
		    false,  // suppress output
		    0,      // seed
		    STRONG, // mode: FAST / ECO / STRONG
		    &edge_cut,
		    partition.data());

		return partition;
	}

} // namespace KaHIPPartitioner