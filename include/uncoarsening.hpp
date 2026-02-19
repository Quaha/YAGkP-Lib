#pragma once

#include "config.hpp"
#include "utils.hpp"
#include "graph.hpp"
#include "heap.hpp"
#include "coarse_level.hpp"

namespace Uncoarser {
	void RestorePartition(
		const Vector<CoarseLevel>& levels,
		      Vector<int_t>&       partition
	);

	Vector<int_t> DirectMapping(
		const CoarseLevel&   prev_level,
		const CoarseLevel&   level,
		const Vector<int_t>& coarse_partition
	);

	Vector<int_t> KernighanLin(
		const CoarseLevel&   prev_level,
		const CoarseLevel&   level,
		const Vector<int_t>& coarse_partition
	);
};