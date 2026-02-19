#pragma once

#include "utils.hpp"

#include "graph.hpp"
#include "config.hpp"
#include "heap.hpp"

namespace PostProcessor {

	void FixPartitionDisbalance(
		const Graph&         graph,
		const int_t          k,
		      Vector<int_t>& partition
	);

	void ImproveFinalPartition(
		const Graph&         graph,
		const int_t          k,
		      Vector<int_t>& partition
	);
	
};