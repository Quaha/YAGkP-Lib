#pragma once

#include "yagkp/utils.hpp"
#include "yagkp/graph.hpp"

namespace Bipartitioner {
	Vector<Part> GetGraphBipartition(const Graph& graph, const int_t C1, const int_t C2);

	Vector<Part> GraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2);

	Vector<Part> GreedyGraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2);
} // namespace Bipartitioner