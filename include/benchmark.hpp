#pragma once

#include <iostream>

#include "graph.hpp"
#include "partitioner.hpp"
#include "metrics.hpp"

#include "utils.hpp"

void PrintBenchmark() {

	std::cout << std::fixed << std::setprecision(10);

    const String base_folder = "../data";
    const String format = ".mtx";
    const Vector<String> files = GetFileNames(base_folder, format);

    Vector<int_t> ks;

    //ks.push_back(2_i);
    //ks.push_back(4_i);
    //ks.push_back(8_i);
    //ks.push_back(16_i);
    //ks.push_back(32_i);
    ks.push_back(64_i);
    ks.push_back(128_i);
    //ks.push_back(256_i);

    std::cout << "accuracy (imbalance): " << ProgramConfig::accuracy * 100.0_r << "%" << "\n";

    for (const auto& path : files) {
        String filename = std::filesystem::path(path).filename().string();

        std::cout << "==============================================\n";
        std::cout << "Graph: " << filename << "\n";

        Graph<int_t, real_t> g(path, "mtx", true);

        std::cout << "n = " << g.getVerticesCount() << ", m = " << g.getEdgesCount() << "\n";

        for (int_t k : ks) {
            Vector<int_t> partition;

            Partitioner::GetGraphKPartition(g, k, partition);
            real_t edge_cut = PartitionMetrics::GetEdgeCut(g, partition);

            real_t real_accuracy = PartitionMetrics::GetAccuracy(g, k, partition);

            std::cout << "k = " << k << " | edge cut = " << edge_cut << " | real imbalance = " << real_accuracy * 100.0_r << "%" << "\n";
            std::cout << "Max part size = " << PartitionMetrics::GetMaxPartWeight(g, k, partition);
            std::cout << " | Optimal part size = " << c<real_t>(g.getSumOfVertexWeights()) / c<real_t>(k) << "\n\n";
        }
    }
}
