#pragma once

#include "graph.hpp"

namespace PartitionMetrics {

	int_t GetEdgeCut(
		const Graph&        graph,
		const Vector<int_t> partition
	);

	Vector<real_t> GetBalances(
		const Graph&         graph,
		const int_t		     k,
		const Vector<int_t>& partition
	);

	real_t GetAccuracy(
		const Graph&         graph,
		const int_t		     k,
		const Vector<int_t>& partition
	);

	int_t GetMaxPartWeight(
		const Graph&         graph,
		const int_t			 k,
		const Vector<int_t>& partition
	);
};