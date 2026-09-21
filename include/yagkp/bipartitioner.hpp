#pragma once

#include <vector>

#include "yagkp/graph.hpp"
#include "yagkp/types.hpp"

namespace yagkp {
	namespace Bipartitioner {
		std::vector<Part> GetGraphBipartition(const Graph& graph, const int_t C1, const int_t C2);

		std::vector<Part> GraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2);

		std::vector<Part> GreedyGraphGrowingAlgorithm(const Graph& graph, const int_t C1, const int_t C2);
	} // namespace Bipartitioner
} // namespace yagkp