#pragma once

#include "config.hpp"
#include "graph.hpp"
#include "heap.hpp"
#include "metrics.hpp"
#include "utils.hpp"

namespace Bipartitioner {
	Vector<Part> GetGraphBipartition(const Graph& graph);

	Vector<Part> GraphGrowingAlgorithm(const Graph& graph);

	Vector<Part> GreedyGraphGrowingAlgorithm(const Graph& graph);
}; // namespace Bipartitioner