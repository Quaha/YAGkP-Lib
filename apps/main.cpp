#include <iostream>

#include "config.hpp"

#include "utils.hpp"

#include "graph.hpp"
#include "partitioner.hpp"
#include "metrics.hpp"

#include "benchmark.hpp"

using namespace std;

int main() {

    ProgramStatistics::InitMatchingStatistics();

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    ProgramConfig::coarsening_method = ProgramConfig::CoarseningMethod::HeavyEdgeMatching;
	ProgramConfig::bipartitioning_method = ProgramConfig::BipartitioningMethod::GraphGrowingAlgorithm;
	ProgramConfig::uncoarsening_method = ProgramConfig::UncoarseningMethod::DirectMapping;

    ProgramConfig::coarsening_clusterization_prohibition = true;
	ProgramConfig::coarsening_clusterization_size_factor = 10000.15_r;

    ProgramConfig::coarsening_vertix_count_limit = 500_i;
    ProgramConfig::coarsening_itarations_limit = 40_i;


    ProgramConfig::accuracy = 0.05_r;

	ProgramConfig::post_processing_disbalance_fix = false;
	ProgramConfig::post_processing_improvement = false;

    PrintBenchmark();

    ProgramStatistics::PrintMatchingStatistics();

    return 0;
}