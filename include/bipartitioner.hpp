#pragma once

#include "utils.hpp"
#include "graph.hpp"
#include "metrics.hpp"

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

    // --- CODE ---

    void GetGraphBipartition(
        const Graph&          graph,
              Vector<int_t>&  partition
    ) {
        switch (ProgramConfig::bipartitioning_method) {
        case ProgramConfig::BipartitioningMethod::GraphGrowingAlgorithm:
            partition = GraphGrowingAlgorithm(graph);
            break;
        case ProgramConfig::BipartitioningMethod::GreedyGraphGrowingAlgorithm:
            partition = GreedyGraphGrowingAlgorithm(graph);
            break;

        default:
            throw std::runtime_error("Unknown bipartitioning method in ProgramConfig.");
        }

	}

    Vector<int_t> GraphGrowingAlgorithm(
        const Graph& graph
    ) {
        const int_t n = graph.n;

        int_t total_weight = graph.getSumOfVertexWeights();

        int_t ideal_weight = total_weight / (int_t)(2);
        int_t max_allowed = (ProgramConfig::accuracy + 1.0) * ideal_weight;

        Vector<int_t> best_partition;
        int_t best_edge_cut;

        bool found = false;

        for (int_t i = 0; i < ProgramConfig::bipartitioning_GraphGrowingAlgorithm_launches_count; ++i) {

            Vector<int_t> partition(n, 0);
            Vector<bool> visited(n, false);

            std::queue<int_t> q;

            Vector<int_t> order = GetRandomPermutation(n);

            for (int_t start_V : order) {
                if (graph.vertex_weights[start_V] <= max_allowed) {
                    q.push(start_V);
                    partition[start_V] = 1;
                    visited[start_V] = true;
                    break;
                }
            }

            int_t current_weight = (int_t)(0);

            while (!q.empty()) {
                int_t curr_V = q.front(); q.pop();

                if (current_weight + graph.vertex_weights[curr_V] > max_allowed) {
                    continue;
                }

                partition[curr_V] = 1;
                current_weight += graph.vertex_weights[curr_V];

                for (auto [next_V, w] : graph[curr_V]) {
                    if (!visited[next_V]) {
                        visited[next_V] = true;
                        q.push(next_V);
                    }
                }
            }

            int_t edge_cut = PartitionMetrics::GetEdgeCut(graph, partition);

            if (!found || edge_cut < best_edge_cut) {
                found = true;
                best_partition = partition;
                best_edge_cut = edge_cut;
            }
        }
        return best_partition;
    }

    Vector<int_t> GreedyGraphGrowingAlgorithm(
        const Graph& graph
    ) {
        const int_t n = graph.n;

        int_t ideal_weight = graph.getSumOfVertexWeights() / (int_t)(2);
        int_t max_allowed = (ProgramConfig::accuracy + 1.0) * ideal_weight;

        Vector<int_t> best_partition;
        int_t best_edge_cut;

        bool found = false;

        for (int_t i = 0; i < ProgramConfig::bipartitioning_GreedyGraphGrowingAlgorithm_launches_count; ++i) {

            Vector<int_t> partition(n, 0);
			Vector<bool> blocked(n, false);
            
            int_t current_weight = (int_t)(0);
			IndexedHeap<int_t> heap(n); // sort values in increasing order by value

			Vector<int_t> order = GetRandomPermutation(n);

            bool flag = true;
            while (flag) {
                flag = false;

                for (int_t V: order) {
                    if (!blocked[V] && graph.getVertexWeight(V) + current_weight <= max_allowed) {
                        flag = true;
                        partition[V] = 1;
                        blocked[V] = true;

                        for (auto [next_V, w1] : graph[V]) {
                            int_t inc_w = (int_t)(0);
                            int_t dec_w = w1;
                            for (auto [near_V, w2] : graph[next_V]) {
                                if (partition[near_V] == 0) {
                                    inc_w += w2;
                                }
                            }
                            heap.push(inc_w - dec_w, next_V);
                        }

                        break;
                    }
				}

                while (!heap.empty()) {
                    auto [priority, curr_V] = heap.extract();

                    blocked[curr_V] = true;

                    if (current_weight + graph.vertex_weights[curr_V] > max_allowed) {
                        continue;
                    }

                    current_weight += graph.vertex_weights[curr_V];

                    partition[curr_V] = 1;
                    for (auto [next_V, w1] : graph[curr_V]) {
                        if (blocked[next_V]) continue;
                        int_t inc_w = (int_t)(0);
                        int_t dec_w = (int_t)(0);

                        for (auto [near_V, w2] : graph[next_V]) {
                            if (partition[near_V] == 0) {
                                inc_w += w2;
                            }
                            else if (partition[near_V] == 1) {
                                dec_w += w2;
                            }
                        }

                        heap.push(inc_w - dec_w, next_V);
                    }
                }
            }

            int_t edge_cut = PartitionMetrics::GetEdgeCut(graph, partition);

            if (!found || edge_cut < best_edge_cut) {
                found = true;
                best_partition = partition;
                best_edge_cut = edge_cut;
            }
        }
        return best_partition;
    }
};