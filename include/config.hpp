#pragma once

#include "utils.hpp"

namespace ProgramConfig {
    // --- Coarsening methods ---
    enum class CoarseningMethod {
        RandomMatching,
        LightEdgeMatching,
        HeavyEdgeMatching,
        HeavyCliqueMatching,
    };

    // --- Bipartitioning methods ---
    enum class BipartitioningMethod {
        GraphGrowingAlgorithm,
        GreedyGraphGrowingAlgorithm,
    };

    // --- Uncoarsening methods ---
    enum class UncoarseningMethod {
        DirectMapping,
        KernighanLin
    };

    // --- Global parameters ---
    inline real_t accuracy = 0.05;

    // --- Coarsening parameters ---
    inline CoarseningMethod coarsening_method = CoarseningMethod::HeavyEdgeMatching;

    inline int_t coarsening_itarations_limit = 40;
    inline int_t coarsening_vertix_count_limit = 100;

    inline bool coarsening_clusterization_prohibition = false;
	inline real_t coarsening_clusterization_size_factor = 0.5;

    // --- Bipartitioning parameters ---
    inline BipartitioningMethod bipartitioning_method = BipartitioningMethod::GraphGrowingAlgorithm;

    inline int_t bipartitioning_GraphGrowingAlgorithm_launches_count = 100;
    inline int_t bipartitioning_GreedyGraphGrowingAlgorithm_launches_count = 100;

    // --- Uncoarsening parameters ---
	inline UncoarseningMethod uncoarsening_method = UncoarseningMethod::DirectMapping;   

	inline bool uncoarsening_KernighanLin_use_blocking = true;

	// --- Post processing parameters ---

    // Correctness is guaranteed only for graphs with vertex weights equal to 1
	inline bool post_processing_disbalance_fix = true;
	inline bool post_processing_improvement = true;
}
