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
	extern real_t accuracy;

	extern bool use_multilevel_scheme;

	// --- Coarsening parameters ---
	extern CoarseningMethod coarsening_method;

	extern int_t coarsening_itarations_limit;
	extern int_t coarsening_vertix_count_limit;

	extern bool coarsening_clusterization_prohibition;
	extern real_t coarsening_clusterization_size_factor;

	// --- Bipartitioning parameters ---
	extern BipartitioningMethod bipartitioning_method;

	extern int_t bipartitioning_GraphGrowingAlgorithm_launches_count;
	extern int_t bipartitioning_GreedyGraphGrowingAlgorithm_launches_count;

	// --- Uncoarsening parameters ---
	extern UncoarseningMethod uncoarsening_method;

	extern bool uncoarsening_KernighanLin_use_blocking;

	extern int_t uncoarsening_KernighanLin_runs;
	extern int_t uncoarsening_KernighanLin_waste_limit;

	// --- Post processing parameters ---

	// Correctness is guaranteed only for graphs with vertex weights equal to 1
	extern bool post_processing_disbalance_fix;
	extern bool post_processing_improvement;
} // namespace ProgramConfig
