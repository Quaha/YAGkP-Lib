#include "coarsening.hpp"

namespace Coarser {

	Vector<CoarseLevel> GetCoarseLevels(
		const Graph& graph,
		const int_t  k
	) {
		Vector<CoarseLevel> levels;
		levels.reserve(ProgramConfig::coarsening_itarations_limit + 1);

		// Entry-level initialization
		Vector<int_t> base_uncoarse_to_coarse(graph.n);
		std::iota(base_uncoarse_to_coarse.begin(), base_uncoarse_to_coarse.end(), 0);

		Vector<Vector<int_t>> base_coarse_to_uncoarse(graph.n);
		for (int_t i = 0; i < graph.n; ++i) {
			base_coarse_to_uncoarse[i] = Vector<int_t>(1, i);
		}

		Vector<int_t> base_vertexmportance(graph.n, (int_t)(0));

		levels.push_back(CoarseLevel(base_uncoarse_to_coarse, base_coarse_to_uncoarse, graph, base_vertexmportance));

		for (int_t i = 0; i < ProgramConfig::coarsening_itarations_limit && levels[i].coarsed_graph.n > ProgramConfig::coarsening_vertix_count_limit; ++i) {

			CoarseLevel new_level;

			FillLevel(levels[i], levels[i].coarsed_graph, new_level, k);

			levels.push_back(new_level);
		}
		return std::move(levels);
	}

	void FillLevel(
		const CoarseLevel& level,
		const Graph&	   graph,
			  CoarseLevel& new_level,
		const int_t        k
	) {
		switch (ProgramConfig::coarsening_method) {
		case ProgramConfig::CoarseningMethod::RandomMatching:
			RandomMatching(level, graph, new_level, k);
			break;

		case ProgramConfig::CoarseningMethod::LightEdgeMatching:
			LightEdgeMatching(level, graph, new_level, k);
			break;

		case ProgramConfig::CoarseningMethod::HeavyEdgeMatching:
			HeavyEdgeMatching(level, graph, new_level, k);
			break;

		case ProgramConfig::CoarseningMethod::HeavyCliqueMatching:
			HeavyCliqueMatching(level, graph, new_level, k);
			break;

		default:
			throw std::runtime_error("Unknown coarsening method in ProgramConfig.");
		}
	}

	void RandomMatching(
		const CoarseLevel& level,
		const Graph&	   graph,
			  CoarseLevel& new_level,
		const int_t        k
	) {

		Vector<int_t> permutation = GetRandomPermutation(graph.n);

		Vector<int_t> matching(graph.n, -1);
		Vector<int_t> matching_edge_weights(graph.n, (int_t)(0));

		int_t max_allowed_size = graph.getSumOfVertexWeights();
		if (!ProgramConfig::coarsening_clusterization_prohibition) {
			max_allowed_size = (int_t)(((real_t)(max_allowed_size) / (real_t)(k)) * ProgramConfig::coarsening_clusterization_size_factor);
		}

		for (int_t curr_V : permutation) {
			if (matching[curr_V] != -1) continue;
			for (auto [next_V, w] : graph[curr_V]) {
				if (matching[next_V] == -1 && graph.vertex_weights[curr_V] + graph.vertex_weights[next_V] <= max_allowed_size) {
					matching[next_V] = curr_V;
					matching[curr_V] = next_V;
					matching_edge_weights[next_V] = w;
					matching_edge_weights[curr_V] = w;
					break;
				}
			}
		}

		ProcessMatching(level, graph, new_level, matching, matching_edge_weights);
	}

	void LightEdgeMatching(
		const CoarseLevel& level,
		const Graph&	   graph,
			  CoarseLevel& new_level,
		const int_t        k
	) {

		Vector<int_t> permutation = GetRandomPermutation(graph.n);

		Vector<int_t> matching(graph.n, -1);
		Vector<int_t> matching_edge_weights(graph.n, (int_t)(0));

		int_t max_allowed_size = graph.getSumOfVertexWeights();
		if (!ProgramConfig::coarsening_clusterization_prohibition) {
			max_allowed_size = (int_t)(((real_t)(max_allowed_size) / (real_t)(k)) * ProgramConfig::coarsening_clusterization_size_factor);
		}

		for (int_t curr_V : permutation) {
			if (matching[curr_V] != -1) {
				continue;
			}
			int_t best_V;
			int_t min_W;
			bool found = false;

			for (auto [next_V, w] : graph[curr_V]) {
				if (graph.vertex_weights[curr_V] + graph.vertex_weights[next_V] > max_allowed_size) continue;
				if (matching[next_V] == -1 && (!found || w < min_W)) {
					min_W = w;
					best_V = next_V;
					found = true;
				}
			}

			if (found) {
				matching[best_V] = curr_V;
				matching[curr_V] = best_V;
				matching_edge_weights[best_V] = min_W;
				matching_edge_weights[curr_V] = min_W;
			}
		}

		ProcessMatching(level, graph, new_level, matching, matching_edge_weights);
	}

	void HeavyEdgeMatching(
		const CoarseLevel& level,
		const Graph&	   graph,
		CoarseLevel&	   new_level,
		const int_t        k
	) {

		Vector<int_t> permutation = GetRandomPermutation(graph.n);

		Vector<int_t> matching(graph.n, -1);
		Vector<int_t> matching_edge_weights(graph.n, (int_t)(0));

		int_t max_allowed_size = graph.getSumOfVertexWeights();
		if (!ProgramConfig::coarsening_clusterization_prohibition) {
			max_allowed_size = (int_t)(((real_t)(max_allowed_size) / (real_t)(k)) * ProgramConfig::coarsening_clusterization_size_factor);
		}

		for (int_t curr_V : permutation) {
			if (matching[curr_V] != -1) {
				continue;
			}
			int_t best_V;
			int_t max_W;
			bool found = false;

			for (auto [next_V, w] : graph[curr_V]) {
				if (graph.vertex_weights[curr_V] + graph.vertex_weights[next_V] > max_allowed_size) continue;
				if (matching[next_V] == -1 && (!found || w > max_W)) {
					max_W = w;
					best_V = next_V;
					found = true;
				}
			}

			if (found) {
				matching[best_V] = curr_V;
				matching[curr_V] = best_V;
				matching_edge_weights[best_V] = max_W;
				matching_edge_weights[curr_V] = max_W;
			}
		}

		ProcessMatching(level, graph, new_level, matching, matching_edge_weights);
	}

	void HeavyCliqueMatching(
		const CoarseLevel& level,
		const Graph&	   graph,
			  CoarseLevel& new_level,
		const int_t        k
	) {

		Vector<int_t> permutation = GetRandomPermutation(graph.n);

		Vector<int_t> matching(graph.n, -1);
		Vector<int_t> matching_edge_weights(graph.n, (int_t)(0));

		int_t max_allowed_size = graph.getSumOfVertexWeights();
		if (ProgramConfig::coarsening_clusterization_prohibition) {
			max_allowed_size = (int_t)(((real_t)(max_allowed_size) / (real_t)(k)) * ProgramConfig::coarsening_clusterization_size_factor);
		}

		for (int_t curr_V : permutation) {
			if (matching[curr_V] != -1) {
				continue;
			}
			int_t best_V;
			int_t best_F;
			int_t edge_W;
			bool found = false;

			for (auto [next_V, w] : graph[curr_V]) {
				if (graph.vertex_weights[curr_V] + graph.vertex_weights[next_V] > max_allowed_size) continue;
				if (matching[next_V] == -1) {
					int_t total_W = level.coarsed_graph.vertex_weights[curr_V] + level.coarsed_graph.vertex_weights[next_V];
					int_t F = (w + level.vertexmportance[curr_V] + level.vertexmportance[next_V]) / (total_W * (total_W - (int_t)(1)));
					if (!found || F > best_F) {
						edge_W = w;
						best_V = next_V;
						best_F = F;
						found = true;
					}
				}
			}

			if (found) {
				matching[best_V] = curr_V;
				matching[curr_V] = best_V;
				matching_edge_weights[best_V] = edge_W;
				matching_edge_weights[curr_V] = edge_W;
			}
		}

		ProcessMatching(level, graph, new_level, matching, matching_edge_weights);
	}

	// This function builds the coarse level based on the found matching
	void ProcessMatching(
		const CoarseLevel&   level,
		const Graph&         graph,
			  CoarseLevel&   new_level,
		const Vector<int_t>& matching,
		const Vector<int_t>& matching_edge_weights
	) {
		// 1. Filling coarse vectors

		Vector<int_t> uncoarse_to_coarse(graph.n, -1);

		Vector<Vector<int_t>> coarse_to_uncoarse;
		coarse_to_uncoarse.reserve(graph.n / 2 + 1);

		int_t vertex_count = 0;

		for (int_t curr_V = 0; curr_V < graph.n; ++curr_V) {
			if (uncoarse_to_coarse[curr_V] != -1) continue;

			int_t next_V = matching[curr_V];
			Vector<int_t> component;

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

		Graph coarsed_graph;
		coarsed_graph.n = coarse_to_uncoarse.size();
		coarsed_graph.vertex_weights.resize(coarsed_graph.n, (int_t)(0));

		for (int_t curr_V = 0; curr_V < coarsed_graph.n; ++curr_V) {
			for (int_t next_V : coarse_to_uncoarse[curr_V]) {
				coarsed_graph.vertex_weights[curr_V] += graph.vertex_weights[next_V];
			}
		}

		// 3. Edges
		Vector<std::unordered_map<int_t, int_t>> tmp_edges(coarsed_graph.n);

		for (int_t c_curr_V = 0; c_curr_V < coarsed_graph.n; ++c_curr_V) {
			for (int_t u_curr_V : coarse_to_uncoarse[c_curr_V]) {
				for (auto [u_next_V, w] : graph[u_curr_V]) {
					int_t c_next_V = uncoarse_to_coarse[u_next_V];

					if (c_next_V != c_curr_V) {
						tmp_edges[c_curr_V][c_next_V] += w;
					}
				}
			}
		}

		int_t total_edges = 0;
		Vector<int_t> edge_count(coarsed_graph.n, 0);
		for (int_t i = 0; i < coarsed_graph.n; ++i) {
			edge_count[i] = tmp_edges[i].size();
			total_edges += edge_count[i];
		}

		coarsed_graph.m = total_edges;
		coarsed_graph.xadj.resize(coarsed_graph.n + 1);
		coarsed_graph.adjncy.resize(total_edges);
		coarsed_graph.edge_weights.resize(total_edges);

		int_t pos = 0;
		for (int_t curr_V = 0; curr_V < coarsed_graph.n; ++curr_V) {
			coarsed_graph.xadj[curr_V] = pos;
			for (auto& [next_V, weight] : tmp_edges[curr_V]) {
				coarsed_graph.adjncy[pos] = next_V;
				coarsed_graph.edge_weights[pos] = weight;
				pos++;
			}
		}
		coarsed_graph.xadj[coarsed_graph.n] = pos;

		// 4. Importance

		Vector<int_t> vertexmportance(coarsed_graph.n, (int_t)(0));
		for (int_t curr_V = 0; curr_V < coarsed_graph.n; ++curr_V) {
			for (int_t prev_V: coarse_to_uncoarse[curr_V]) {
				vertexmportance[curr_V] += level.vertexmportance[prev_V];
			}
			if (coarse_to_uncoarse[curr_V].size() == 2) {
				vertexmportance[curr_V] += matching_edge_weights[coarse_to_uncoarse[curr_V][0]];
			}
		}

		// 5. Results

		new_level.uncoarse_to_coarse = std::move(uncoarse_to_coarse);
		new_level.coarse_to_uncoarse = std::move(coarse_to_uncoarse);
		new_level.coarsed_graph = std::move(coarsed_graph);
		new_level.vertexmportance = std::move(vertexmportance);
	}
};