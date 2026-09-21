#pragma once

#include <vector>

#include "yagkp/graph.hpp"
#include "yagkp/types.hpp"

namespace yagkp {
	namespace Partitioner {
		// graph: input graph to partition          | graph
		// k: number of parts (k >= 1)              | 3
		// returns: vector of part indices (0..k-1) | {0, 0, 1, 1, 2}
		std::vector<int_t> GetGraphKPartition(const Graph& graph, const int_t k);

		// graph: input graph to partition                      | graph
		// k: number of parts for this recursive call (k >= 1)  | 2
		// offset: starting index for parts in this call        | 2
		// returns: vector of part indices (offset..offset+k-1) | {2, 2, 3, 3}
		std::vector<int_t> RecursivePartition(const Graph& graph, const int_t k, const int_t M, int_t offset);
	} // namespace Partitioner
} // namespace yagkp