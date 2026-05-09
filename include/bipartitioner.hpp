#pragma once

#include "config.hpp"
#include "graph.hpp"
#include "heap.hpp"
#include "metrics.hpp"
#include "utils.hpp"

namespace Bipartitioner {
	Vector<Part> GetGraphBipartition(const Graph& graph, const int_t C1, const int_t C2);

	Vector<Part> GraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2);

	Vector<Part> GreedyGraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2);

	Vector<Part> FlowAlgorithm(const Graph& graph);
} // namespace Bipartitioner