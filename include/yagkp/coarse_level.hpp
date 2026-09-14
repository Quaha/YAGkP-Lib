#pragma once

#include "graph.hpp"
#include "utils.hpp"

struct CoarseLevel {
	Vector<int_t> uncoarse_to_coarse;
	Vector<Vector<int_t>> coarse_to_uncoarse;
	Graph coarsened_graph;
	Vector<int_t> included_ew_in_vertex;
};