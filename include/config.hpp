#pragma once

#include "utils.hpp"

struct ProgramConfig {
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
	inline static double imbalance = 0.05;

	// --- Coarsening parameters ---
	inline static CoarseningMethod coarsening_method = CoarseningMethod::HeavyCliqueMatching;

	inline static int_t coarsening_iterations_limit   = 40;
	inline static int_t coarsening_vertex_count_limit = 500;

	inline static bool coarsening_clusterization_prohibition   = false;
	inline static double coarsening_clusterization_size_factor = 0.5;

	// --- Bipartitioning parameters ---
	inline static BipartitioningMethod bipartitioning_method = BipartitioningMethod::GreedyGraphGrowingAlgorithm;

	inline static int_t bipartitioning_GraphGrowingAlgorithm_launches_count       = 100;
	inline static int_t bipartitioning_GreedyGraphGrowingAlgorithm_launches_count = 100;

	// --- Uncoarsening parameters ---
	inline static UncoarseningMethod uncoarsening_method = UncoarseningMethod::KernighanLin;

	inline static int_t uncoarsening_KernighanLin_runs        = 3;
	inline static int_t uncoarsening_KernighanLin_waste_limit = 50;

	// --- Post processing parameters ---

	// Correctness is guaranteed only for graphs with vertex weights equal to 1
	inline static bool post_processing_imbalance_fix = false;
	inline static bool post_processing_improvement   = false;
};
