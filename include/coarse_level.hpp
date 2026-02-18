#pragma once

#include "utils.hpp"
#include "graph.hpp"

template <typename vw_t, typename ew_t>
struct CoarseLevel {
	Vector<int_t>		  uncoarse_to_coarse;
	Vector<Vector<int_t>> coarse_to_uncoarse;
	Graph<vw_t, ew_t>	  coarsed_graph;
	Vector<ew_t>		  vertexmportance;
};