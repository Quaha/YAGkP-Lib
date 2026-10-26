#include "yagkp/coarsening.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

#include "yagkp/config.hpp"
#include "yagkp/queue.hpp"
#include "yagkp/random.hpp"

namespace yagkp::Coarser {

	namespace {

		// This function builds the coarse level based on the found matching
		CoarseLevel ProcessMatching(
		    const CoarseLevel& level,
		    const Graph& graph,
		    const std::vector<int_t>& matching,
		    const std::vector<int_t>& matching_edge_weights
		) {
			// 1. Filling coarse vectors

			std::vector<int_t> uncoarse_to_coarse(graph.n, -1);

			std::vector<std::vector<int_t>> coarse_to_uncoarse;
			coarse_to_uncoarse.reserve(graph.n / 2 + 1);

			int_t vertex_count = 0;

			for (int_t curr_V = 0; curr_V < graph.n; ++curr_V) {
				if (uncoarse_to_coarse[curr_V] != -1)
					continue;

				int_t next_V = matching[curr_V];
				std::vector<int_t> component;

				if (next_V == -1) {
					uncoarse_to_coarse[curr_V] = vertex_count;
					component.push_back(curr_V);
				}
				else if (curr_V < next_V) {
					uncoarse_to_coarse[curr_V] = vertex_count;
					uncoarse_to_coarse[next_V] = vertex_count;
					component.push_back(curr_V);
					component.push_back(next_V);
				}

				coarse_to_uncoarse.push_back(std::move(component));
				++vertex_count;
			}

			// 2. Building graph

			Graph coarsened_graph;
			coarsened_graph.n = coarse_to_uncoarse.size();
			coarsened_graph.vertex_weights.resize(coarsened_graph.n, 0);

			for (int_t curr_V = 0; curr_V < coarsened_graph.n; ++curr_V) {
				for (int_t next_V: coarse_to_uncoarse[curr_V]) {
					coarsened_graph.vertex_weights[curr_V] += graph.vertex_weights[next_V];
				}
			}

			coarsened_graph.total_vertex_weight = graph.total_vertex_weight;

			// 3. Edges
			std::vector<int_t> upper_xadj(coarsened_graph.n + 1, 0);
			for (int_t c_curr_V = 0; c_curr_V < coarsened_graph.n; ++c_curr_V) {
				int_t upper_degree = 0;
				for (int_t u_curr_V: coarse_to_uncoarse[c_curr_V]) {
					upper_degree += graph.xadj[u_curr_V + 1] - graph.xadj[u_curr_V];
				}
				upper_xadj[c_curr_V + 1] = upper_xadj[c_curr_V] + upper_degree;
			}

			std::vector<int_t> upper_adjncy(upper_xadj[coarsened_graph.n]);
			std::vector<int_t> upper_edge_weights(upper_xadj[coarsened_graph.n]);
			std::vector<int_t> degree(coarsened_graph.n, 0);
			std::vector<int_t> marker(coarsened_graph.n, -1);

			for (int_t c_curr_V = 0; c_curr_V < coarsened_graph.n; ++c_curr_V) {
				const int_t begin = upper_xadj[c_curr_V];
				int_t end         = begin;

				for (int_t u_curr_V: coarse_to_uncoarse[c_curr_V]) {
					for (auto [u_next_V, w]: graph[u_curr_V]) {
						int_t c_next_V = uncoarse_to_coarse[u_next_V];
						if (c_next_V == c_curr_V) {
							continue;
						}
						if (marker[c_next_V] == -1) {
							marker[c_next_V]        = end;
							upper_adjncy[end]       = c_next_V;
							upper_edge_weights[end] = w;
							++end;
						}
						else {
							upper_edge_weights[marker[c_next_V]] += w;
						}
					}
				}

				for (int_t pos = begin; pos < end; ++pos) {
					marker[upper_adjncy[pos]] = -1;
				}
				degree[c_curr_V] = end - begin;
			}

			coarsened_graph.xadj.resize(coarsened_graph.n + 1);
			coarsened_graph.xadj[0] = 0;
			for (int_t c_curr_V = 0; c_curr_V < coarsened_graph.n; ++c_curr_V) {
				coarsened_graph.xadj[c_curr_V + 1] =
				    coarsened_graph.xadj[c_curr_V] + degree[c_curr_V];
			}

			coarsened_graph.m = coarsened_graph.xadj[coarsened_graph.n];
			coarsened_graph.adjncy.resize(coarsened_graph.m);
			coarsened_graph.edge_weights.resize(coarsened_graph.m);

			for (int_t c_curr_V = 0; c_curr_V < coarsened_graph.n; ++c_curr_V) {
				std::copy_n(
				    upper_adjncy.begin() + upper_xadj[c_curr_V],
				    degree[c_curr_V],
				    coarsened_graph.adjncy.begin() + coarsened_graph.xadj[c_curr_V]
				);
				std::copy_n(
				    upper_edge_weights.begin() + upper_xadj[c_curr_V],
				    degree[c_curr_V],
				    coarsened_graph.edge_weights.begin() + coarsened_graph.xadj[c_curr_V]
				);
			}

			// 4. Importance

			std::vector<int_t> included_ew_in_vertex(coarsened_graph.n, 0);
			for (int_t curr_V = 0; curr_V < coarsened_graph.n; ++curr_V) {
				for (int_t prev_V: coarse_to_uncoarse[curr_V]) {
					included_ew_in_vertex[curr_V] += level.included_ew_in_vertex[prev_V];
				}
				if (coarse_to_uncoarse[curr_V].size() == 2) {
					included_ew_in_vertex[curr_V] +=
					    matching_edge_weights[coarse_to_uncoarse[curr_V][0]];
				}
			}

			// 5. Results

			CoarseLevel new_level;

			new_level.uncoarse_to_coarse    = std::move(uncoarse_to_coarse);
			new_level.coarse_to_uncoarse    = std::move(coarse_to_uncoarse);
			new_level.coarsened_graph       = std::move(coarsened_graph);
			new_level.included_ew_in_vertex = std::move(included_ew_in_vertex);

			return new_level;
		}

		template <typename EdgeRating>
		CoarseLevel GetLevelByMatching(
		    const CoarseLevel& level,
		    const Graph& graph,
		    const int_t k,
		    const EdgeRating& edge_rating
		) {
			const int_t n = graph.n;

			std::vector<int_t> permutation = GetRandomPermutation(n);

			std::vector<int_t> matching(n, -1);

			int_t max_allowed_size = graph.getSumOfVertexWeights();
			if (ProgramConfig::coarsening_clusterization_prohibition) {
				max_allowed_size = (int_t)(((fp_t)(max_allowed_size) / (fp_t)(k)) *
				                           ProgramConfig::coarsening_clusterization_size_factor);
			}

			[[maybe_unused]] const int_t grain_size = ProgramConfig::parallel_loop_grain_size;
			[[maybe_unused]] const bool is_parallel =
			    n >= ProgramConfig::parallel_loop_min_vertices;

#pragma omp taskloop default(none) shared(graph, edge_rating, permutation, matching)               \
    firstprivate(n, max_allowed_size) grainsize(grain_size) if (is_parallel)
			for (int_t i = 0; i < n; ++i) {
				const int_t curr_V = permutation[i];
				if (parallel::RelaxedLoad(matching[curr_V]) != -1) {
					continue;
				}
				int_t best_V;
				fp_t best_rating;
				bool found = false;

				for (auto [next_V, w]: graph[curr_V]) {
					if (graph.vertex_weights[curr_V] + graph.vertex_weights[next_V] >
					    max_allowed_size)
						continue;
					if (parallel::RelaxedLoad(matching[next_V]) != -1)
						continue;

					fp_t rating = edge_rating(curr_V, next_V, w);
					if (!found || rating > best_rating) {
						best_V      = next_V;
						best_rating = rating;
						found       = true;
					}
				}

				if (found) {
					parallel::RelaxedStore(matching[best_V], curr_V);
					parallel::RelaxedStore(matching[curr_V], best_V);
				}
			}

#pragma omp taskloop default(none) shared(matching) firstprivate(n)                                \
    grainsize(grain_size) if (is_parallel)
			for (int_t curr_V = 0; curr_V < n; ++curr_V) {
				const int_t next_V = parallel::RelaxedLoad(matching[curr_V]);
				if (next_V != -1 && parallel::RelaxedLoad(matching[next_V]) != curr_V) {
					parallel::RelaxedStore(matching[curr_V], int_t(-1));
				}
			}

			std::vector<int_t> matching_edge_weights(n, 0);
#pragma omp taskloop default(none) shared(graph, matching, matching_edge_weights) firstprivate(n)  \
    grainsize(grain_size) if (is_parallel)
			for (int_t curr_V = 0; curr_V < n; ++curr_V) {
				if (matching[curr_V] == -1) {
					continue;
				}
				for (auto [next_V, w]: graph[curr_V]) {
					if (next_V == matching[curr_V]) {
						matching_edge_weights[curr_V] = w;
						break;
					}
				}
			}
			return ProcessMatching(level, graph, matching, matching_edge_weights);
		}

		CoarseLevel RandomMatching(const CoarseLevel& level, const Graph& graph, const int_t k) {
			return GetLevelByMatching(level, graph, k, [](int_t, int_t, int_t) { return 0; });
		}

		CoarseLevel LightEdgeMatching(const CoarseLevel& level, const Graph& graph, const int_t k) {
			return GetLevelByMatching(level, graph, k, [](int_t, int_t, int_t w) { return -w; });
		}

		CoarseLevel HeavyEdgeMatching(const CoarseLevel& level, const Graph& graph, const int_t k) {
			return GetLevelByMatching(level, graph, k, [](int_t, int_t, int_t w) { return w; });
		}

		CoarseLevel HeavyCliqueMatching(
		    const CoarseLevel& level,
		    const Graph& graph,
		    const int_t k
		) {
			auto clique_density = [&level](int_t curr_V, int_t next_V, int_t w) -> fp_t {
				int_t total_W = level.coarsened_graph.vertex_weights[curr_V] +
				                level.coarsened_graph.vertex_weights[next_V];
				return 2.0 *
				       fp_t(
				           w + level.included_ew_in_vertex[curr_V] +
				           level.included_ew_in_vertex[next_V]
				       ) /
				       fp_t(
				           std::max(
				               1ll,
				               static_cast<long long>(total_W) * static_cast<long long>(total_W - 1)
				           )
				       );
			};
			return GetLevelByMatching(level, graph, k, clique_density);
		}

		CoarseLevel FillLevel(const CoarseLevel& level, const Graph& graph, const int_t k) {
			using Method = ProgramConfig::CoarseningMethod;

			switch (ProgramConfig::coarsening_method) {
			case Method::RandomMatching:
				return RandomMatching(level, graph, k);

			case Method::LightEdgeMatching:
				return LightEdgeMatching(level, graph, k);

			case Method::HeavyEdgeMatching:
				return HeavyEdgeMatching(level, graph, k);

			case Method::HeavyCliqueMatching:
				return HeavyCliqueMatching(level, graph, k);
			}

			throw std::logic_error("Unknown processing method");
		}
	} // namespace

	std::vector<CoarseLevel> GetCoarseLevels(const Graph& graph, const int_t k) {
		std::vector<CoarseLevel> levels;
		levels.reserve(ProgramConfig::coarsening_iterations_limit + 1);

		// Entry-level initialization
		std::vector<int_t> base_uncoarse_to_coarse(graph.n);
		std::iota(base_uncoarse_to_coarse.begin(), base_uncoarse_to_coarse.end(), 0);

		std::vector<std::vector<int_t>> base_coarse_to_uncoarse(graph.n);
		for (int_t i = 0; i < graph.n; ++i) {
			base_coarse_to_uncoarse[i] = std::vector<int_t>(1, i);
		}

		std::vector<int_t> base_included_ew_in_vertex(graph.n, 0);

		levels.push_back(
		    CoarseLevel{
		        base_uncoarse_to_coarse,
		        base_coarse_to_uncoarse,
		        graph,
		        base_included_ew_in_vertex
		    }
		);

		for (int_t i = 0;
		     i < ProgramConfig::coarsening_iterations_limit &&
		     levels[i].coarsened_graph.n >
		         std::max<int_t>(
		             2,
		             std::min(ProgramConfig::coarsening_vertex_count_limit, graph.n / k / 2)
		         );
		     ++i) {

			CoarseLevel new_level = FillLevel(levels[i], levels[i].coarsened_graph, k);

			if (new_level.coarsened_graph.n >
			    ProgramConfig::coarsening_min_reduction_fraction * levels[i].coarsened_graph.n) {
				break;
			}

			levels.push_back(std::move(new_level));
		}
		return levels;
	}
} // namespace yagkp::Coarser