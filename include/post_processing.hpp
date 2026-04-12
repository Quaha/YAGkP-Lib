#pragma once

#include "config.hpp"
#include "graph.hpp"
#include "heap.hpp"
#include "utils.hpp"

namespace PostProcessor {
	void FixPartitionDisbalance(const Graph& graph, const int_t k, Vector<int_t>& partition);

	void ImproveFinalPartition(const Graph& graph, const int_t k, Vector<int_t>& partition);
} // namespace PostProcessor