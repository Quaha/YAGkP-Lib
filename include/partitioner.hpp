#pragma once

#include "utils.hpp"

#include "graph.hpp"

#include "coarsening.hpp"
#include "bipartitioner.hpp"
#include "uncoarsening.hpp"
#include "post_processing.hpp"

#include "metrics.hpp"

namespace Partitioner {

    void GetGraphKPartition(
        const Graph&         graph,
        const int_t          k,
              Vector<int_t>& partition
    );

    void RecursivePartition(
        const Graph&         graph,
        const int_t          k,
              Vector<int_t>& partition,
              int_t          offset
    );
};