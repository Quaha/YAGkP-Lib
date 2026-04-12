#pragma once

#include "coarse_level.hpp"
#include "config.hpp"
#include "graph.hpp"
#include "heap.hpp"
#include "metrics.hpp"
#include "utils.hpp"

namespace Uncoarser {
	// levels: hierarchy of coarse levels from source graph to coarest | levels
	// partition: partition of the coarsest graph                      | {Part::First, Part::Second}
	// returns: partition of the original graph                        | {Part::First, Part::Second, Part::First,
	// Part::Second}
	Vector<Part> RestorePartition(const Vector<CoarseLevel>& levels, Vector<Part> partition);

	// coarse_level: coarse level data with mapping from finer to coarser vertices | coarse_level
	// coarse_partition: partition of the coarse graph (size = |V_coarse|)         | {Part::First, Part::Second}
	// returns: partition of the finer graph obtained by direct mapping            | {Part::First, Part::First,
	// Part::Second, Part::Second}
	Vector<Part> DirectMapping(const CoarseLevel& coarse_level, const Vector<Part>& coarse_partition);

	// previous_graph: finer graph to refine partition on                          | graph (|V| = 4, edge weights = 1)
	// coarse_level: coarse level data with mapping from finer to coarser vertices | coarse_level
	// coarse_partition: partition of the coarse graph                             | {Part::First, Part::Second}
	// returns: refined partition of the finer graph using Kernighan-Lin algorithm | {Part::First, Part::Second,
	// Part::First, Part::Second}
	Vector<Part> KernighanLin(const Graph& previous_graph, const CoarseLevel& coarse_level, const Vector<Part>& coarse_partition);

	Vector<Part> KernighanLinBlocking(const Graph& previous_graph, Vector<Part> current_partition);

	Vector<Part> KernighanLinBlocking(const Graph& previous_graph, const CoarseLevel& coarse_level, const Vector<Part>& coarse_partition);
} // namespace Uncoarser