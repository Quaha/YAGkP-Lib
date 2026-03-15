#pragma once

#include "graph.hpp"
#include "utils.hpp"

struct CoarseLevel {
	Vector<int_t> uncoarse_to_coarse;
	Vector<Vector<int_t>> coarse_to_uncoarse;
	Graph coarsed_graph;
	Vector<int_t> vertexmportance;
};