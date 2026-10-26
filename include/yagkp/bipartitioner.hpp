#pragma once

#include <vector>

#include "yagkp/graph.hpp"
#include "yagkp/types.hpp"

namespace yagkp::Bipartitioner {
	std::vector<Part> GetGraphBipartition(const Graph& graph, const int_t C1, const int_t C2);
} // namespace yagkp::Bipartitioner