#pragma once

#include "utils.hpp"
#include "graph.hpp"
#include "metrics.hpp"
#include "config.hpp"
#include "heap.hpp"

namespace Bipartitioner {
    Vector<Part> GetGraphBipartition(
        const Graph& graph
    );

    Vector<Part> GraphGrowingAlgorithm(
        const Graph& graph
    );

    Vector<Part> GreedyGraphGrowingAlgorithm(
        const Graph& graph
    );
};