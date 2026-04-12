#include "post_processing.hpp"

namespace PostProcessor {

	void FixPartitionImbalance(const Graph& graph, const int_t k, Vector<int_t>& partition) {

		Vector<int_t> comp_weight(k, 0);
		int_t n = graph.getVerticesCount();
		for (int_t v = 0; v < n; v++) {
			comp_weight[partition[v]] += graph.getVertexWeight(v);
		}

		int_t total_weight = graph.getSumOfVertexWeights();
		int_t max_allowed  = (int_t)((double)(total_weight) / (double)(k) * (1.0 + ProgramConfig::imbalance + EPS));

		while (max_allowed * k < total_weight) {
			max_allowed += 1;
		}

		Vector<Vector<int_t>> comp_vertices(k);
		for (int_t v = 0; v < n; v++) {
			comp_vertices[partition[v]].push_back(v);
		}

		IndexedHeap<int_t, std::greater<int_t>> heap(k);
		for (int_t c = 0; c < k; c++) {
			heap.push(comp_weight[c], c);
		}

		while (!heap.empty() && heap.top().first > max_allowed) {
			int_t cdx = heap.top().second;

			int_t target = -1;
			int_t min_w  = std::numeric_limits<int_t>::max();

			for (int_t t = 0; t < k; ++t) {
				if (t == cdx)
					continue;
				if (comp_weight[t] < min_w) {
					min_w  = comp_weight[t];
					target = t;
				}
			}

			if (target == -1) {
				break;
			}

			int_t size   = comp_vertices[cdx].size();
			int_t randdx = GetRandomInt(size);

			std::swap(comp_vertices[cdx][randdx], comp_vertices[cdx].back());

			int_t vertex_to_move = comp_vertices[cdx].back();
			comp_vertices[cdx].pop_back();
			comp_vertices[target].push_back(vertex_to_move);

			partition[vertex_to_move] = target;

			int_t vertex_weight = graph.getVertexWeight(vertex_to_move);
			comp_weight[cdx] -= vertex_weight;
			comp_weight[target] += vertex_weight;

			heap.changePriority(comp_weight[cdx], cdx);
			heap.changePriority(comp_weight[target], target);
		}
	}

	void ImproveFinalPartition(const Graph& graph, const int_t k, Vector<int_t>& partition) {
		int_t n = graph.getVerticesCount();

		Vector<int_t> comp_weight(k, 0);
		for (int_t v = 0; v < n; v++) {
			comp_weight[partition[v]] += graph.getVertexWeight(v);
		}

		int_t total_weight = graph.getSumOfVertexWeights();
		int_t max_allowed  = (int_t)(((double)(total_weight) / (double)(k)) * (1.0 + ProgramConfig::imbalance + EPS));

		while (max_allowed * k < total_weight) {
			max_allowed++;
		}

		Queue<int_t> vertices_queue; vertices_queue.reserve(n / 2);
		for (int_t i = 0; i < n; i++) {
			vertices_queue.push(i);
		}

		while (!vertices_queue.empty()) {
			int_t v = vertices_queue.front();
			vertices_queue.pop();
			int_t curr_comp = partition[v];
			int_t vertex_w  = graph.getVertexWeight(v);

			int_t best_target = curr_comp;
			int_t best_gain   = 0;

			for (int_t t = 0; t < k; t++) {
				if (t == curr_comp)
					continue;
				if (comp_weight[t] + vertex_w > max_allowed)
					continue;

				int_t cut_before = 0;
				int_t cut_after  = 0;

				for (auto [u, w]: graph[v]) {
					if (partition[u] == curr_comp)
						cut_before += w;
					if (partition[u] == t)
						cut_after += w;
				}

				int_t gain = cut_after - cut_before;
				if (gain > best_gain) {
					best_gain   = gain;
					best_target = t;
				}
			}

			if (best_target != curr_comp) {
				partition[v] = best_target;
				comp_weight[curr_comp] -= vertex_w;
				comp_weight[best_target] += vertex_w;
				vertices_queue.push(v);
			}
		}
	}
} // namespace PostProcessor