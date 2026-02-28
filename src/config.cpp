#include "config.hpp"

namespace ProgramConfig {
    // --- Global parameters ---
    real_t accuracy = 0.05;

    // --- Coarsening parameters ---
    CoarseningMethod coarsening_method = CoarseningMethod::HeavyCliqueMatching;

    int_t coarsening_itarations_limit = 40;
    int_t coarsening_vertix_count_limit = 500;

    bool coarsening_clusterization_prohibition = false;
	real_t coarsening_clusterization_size_factor = 0.5;

    // --- Bipartitioning parameters ---
    BipartitioningMethod bipartitioning_method = BipartitioningMethod::GreedyGraphGrowingAlgorithm;

    int_t bipartitioning_GraphGrowingAlgorithm_launches_count = 100;
    int_t bipartitioning_GreedyGraphGrowingAlgorithm_launches_count = 100;

    // --- Uncoarsening parameters ---
	UncoarseningMethod uncoarsening_method = UncoarseningMethod::KernighanLin;   

	bool uncoarsening_KernighanLin_use_blocking = true;

	// --- Post processing parameters ---

    // Correctness is guaranteed only for graphs with vertex weights equal to 1
	bool post_processing_disbalance_fix = true;
	bool post_processing_improvement = true;
}
