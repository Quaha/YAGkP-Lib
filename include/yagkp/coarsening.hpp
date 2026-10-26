#pragma once

#include <vector>

#include "yagkp/coarse_level.hpp"
#include "yagkp/graph.hpp"
#include "yagkp/types.hpp"

namespace yagkp::Coarser {
	std::vector<CoarseLevel> GetCoarseLevels(const Graph& graph, const int_t k);
} // namespace yagkp::Coarser