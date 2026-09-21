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

	/**
	 * @brief Projects the partition of the coarsened graph onto the the previous coarse level.
	 *
	 * @param coarse_level coarse level data with mapping from finer to coarser vertices
	 * @param coarse_partition partition of the coarse graph
	 * @return partition of the previous coarsening level graph
	 */
	std::vector<Part>
	DirectMapping(const CoarseLevel& coarse_level, const std::vector<Part>& coarse_partition);

	/**
	 * @brief
	 *
	 * @param previous_graph Projects the partition of the coarsened graph onto the the previous
	 * coarse level and does some improvements
	 * @param coarse_level coarse level data with mapping from finer to coarser vertices
	 * @param coarse_partition partition of the coarse graph
	 * @param C1 restriction on the 1st part
	 * @param C2 restriction on the 2nd part
	 * @return improved partition of the previous coarsening level graph
	 */
	std::vector<Part> KernighanLinBlocking(
	    const Graph& previous_graph,
	    const CoarseLevel& coarse_level,
	    const std::vector<Part>& coarse_partition,
	    const int_t C1,
	    const int_t C2
	);
} // namespace yagkp::Uncoarser