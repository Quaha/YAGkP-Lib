#pragma once

#include "config.hpp"

#include "utils.hpp"
#include "graph.hpp"

#include "coarse_level.hpp"

namespace Coarser {

	Vector<CoarseLevel> GetCoarseLevels(
		const Graph& graph,
		const int_t  k
	);

	void FillLevel(
		const CoarseLevel& level,
		const Graph&       graph,
		      CoarseLevel& new_level,
		const int_t        k
	);

	void RandomMatching(
		const CoarseLevel& level,
		const Graph&       graph,
		      CoarseLevel& new_level,
		const int_t        k
	);

	void LightEdgeMatching(
		const CoarseLevel& level,
		const Graph&       graph,
		      CoarseLevel& new_level,
		const int_t        k
	);

	void HeavyEdgeMatching(
		const CoarseLevel& level,
		const Graph&       graph,
		      CoarseLevel& new_level,
		const int_t        k
	);

	void HeavyCliqueMatching(
		const CoarseLevel& level,
		const Graph&       graph,
		      CoarseLevel& new_level,
		const int_t        k
	);

	void ProcessMatching(
		const CoarseLevel&   level,
		const Graph&         graph,
		      CoarseLevel&   new_level,
		const Vector<int_t>& matching,
		const Vector<int_t>& matching_edge_weights
	);
};