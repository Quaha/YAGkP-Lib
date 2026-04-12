#pragma once

#include "graph.hpp"

namespace PartitionMetrics {
	// graph: input graph                                                   | graph
	// partition: vector of part indices for each vertex (size = |V|)       | {0, 1, 2}
	// returns: total weight of edges connecting different parts (edge cut) | 4
	double GetEdgeCut(const Graph& graph, const Vector<int_t>& partition);

	// graph: input graph                                                   | graph
	// partition: vector of parts for each vertex (size = |V|)              | {Part::First, Part::Second, Part::First}
	// returns: total weight of edges connecting different parts (edge cut) | 4
	double GetEdgeCut(const Graph& graph, const Vector<Part>& partition);

	// graph: input graph                                                                 | graph (|V| = 6, weight(Ui) = 1)
	// k: number of parts                                                                 | 4
	// partition: vector of part indices for each vertex                                  | {0, 1, 0, 2, 3, 1}
	// returns: vector of relative weights of each part (fraction of total vertex weight) | {0.33, 0.33, 0.16, 0.16}
	Vector<double> GetBalances(const Graph& graph, const int_t k, const Vector<int_t>& partition);

	// graph: input graph                                               | graph (|V| = 6, weight(Ui) = 1)
	// k: number of parts                                               | 4
	// partition: vector of part indices                                | {0, 1, 0, 2, 3, 1}
	// returns: imbalance measure = k * max(part_weight_fraction - 1/k) | 0.33
	double GetAccuracy(const Graph& graph, const int_t k, const Vector<int_t>& partition);

	// graph: input graph                                   | graph (|V| = 6, weight(Ui) = 1)
	// k: number of parts                                   | 4
	// partition: vector of part indices                    | {0, 1, 0, 2, 3, 1}
	// returns: maximum total vertex weight among all parts | 2
	int_t GetMaxPartWeight(const Graph& graph, const int_t k, const Vector<int_t>& partition);
} // namespace PartitionMetrics