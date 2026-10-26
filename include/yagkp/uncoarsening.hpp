#pragma once

#include <vector>

#include "yagkp/coarse_level.hpp"
#include "yagkp/graph.hpp"
#include "yagkp/types.hpp"

namespace yagkp::Uncoarser {
	/**
	 * @brief Projects the partition of the coarsened graph onto the uncoarsened graph.
	 *
	 * @param levels hierarchy of coarse levels from source graph to coarest
	 * @param partition partition of the coarsest graph
	 * @param C1 restriction on the 1st part
	 * @param C2 restriction on the 2nd part
	 * @return partition of the original graph
	 */
	std::vector<Part> RestorePartition(
	    const std::vector<CoarseLevel>& levels,
	    std::vector<Part> partition,
	    const int_t C1,
	    const int_t C2
	);
} // namespace yagkp::Uncoarser