#pragma once

#include "utils.hpp"
#include "graph.hpp"
#include "metrics.hpp"
#include "config.hpp"

#include "heap.hpp"

namespace Bipartitioner {

    void GetGraphBipartition(
        const Graph& graph,
        Vector<int_t>& partition
    );

    Vector<int_t> GraphGrowingAlgorithm(
        const Graph& graph
    );

    Vector<int_t> GreedyGraphGrowingAlgorithm(
        const Graph& graph
    );

};