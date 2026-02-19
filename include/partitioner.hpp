#pragma once

#include "utils.hpp"
#include "graph.hpp"
#include "coarsening.hpp"
#include "bipartitioner.hpp"
#include "uncoarsening.hpp"
#include "post_processing.hpp"
#include "metrics.hpp"

namespace Partitioner {
    // graph: input graph to partition          | graph
    // k: number of parts (k >= 1)              | 3
    // returns: vector of part indices (0..k-1) | {0, 0, 1, 1, 2}
    Vector<int_t> GetGraphKPartition(
        const Graph& graph,
        const int_t  k
    );

    // graph: input graph to partition                      | graph
    // k: number of parts for this recursive call (k >= 1)  | 2
    // offset: starting index for parts in this call        | 2
    // returns: vector of part indices (offset..offset+k-1) | {2, 2, 3, 3}
    Vector<int_t> RecursivePartition(
        const Graph& graph,
        const int_t  k,
              int_t  offset = 0
    );
};