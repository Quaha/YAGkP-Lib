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
	inline static fp_t imbalance = 0.05;
	inline static int_t runs     = 2;

	// --- Coarsening parameters ---
	inline static CoarseningMethod coarsening_method = CoarseningMethod::HeavyCliqueMatching;

	inline static int_t coarsening_iterations_limit   = 40;
	inline static int_t coarsening_vertex_count_limit = 500;

	inline static bool coarsening_clusterization_prohibition = false;
	inline static fp_t coarsening_clusterization_size_factor = 0.5;

	// --- Bipartitioning parameters ---
	inline static BipartitioningMethod bipartitioning_method = BipartitioningMethod::GreedyGraphGrowingAlgorithm;

	inline static int_t bipartitioning_GraphGrowingAlgorithm_launches_count       = 50;
	inline static int_t bipartitioning_GreedyGraphGrowingAlgorithm_launches_count = 50;

	// --- Uncoarsening parameters ---
	inline static UncoarseningMethod uncoarsening_method = UncoarseningMethod::KernighanLin;

	inline static int_t uncoarsening_KernighanLin_runs        = 5;
	inline static int_t uncoarsening_KernighanLin_waste_limit = 100;
};
