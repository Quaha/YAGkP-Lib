#include "graph.hpp"

Graph::Graph() {
}

Graph::Graph(const spMtx<double>& matrix, bool ignore_eweights) {
	buildGraph(matrix, ignore_eweights);
}

Graph::Graph(const String& file_name, const String& format, bool ignore_eweights) {
	spMtx<double> matrix(file_name.c_str(), format);
	buildGraph(matrix, ignore_eweights);
}

void Graph::buildGraph(const spMtx<double>& matrix, bool ignore_eweights) {
	n = static_cast<int_t>(matrix.m);
	m = static_cast<int_t>(matrix.nz);

	adjncy.resize(m);
	for (int_t i = 0; i < m; ++i) {
		adjncy[i] = static_cast<int_t>(matrix.Col[i]);
	}

	xadj.resize(n + 1);
	for (int_t i = 0; i < n + 1; ++i) {
		xadj[i] = static_cast<int_t>(matrix.Rst[i]);
	}

	vertex_weights.resize(n);
	for (int_t i = 0; i < n; ++i) {
		vertex_weights[i] = 1;
	}

	total_vertex_weight = std::accumulate(vertex_weights.begin(), vertex_weights.end(), 0);

	edge_weights.resize(m);
	if (matrix.Val != nullptr && !ignore_eweights) {
		for (int_t i = 0; i < m; ++i) {
			edge_weights[i] = static_cast<int_t>(matrix.Val[i]);
		}
	}
	else {
		for (int_t i = 0; i < m; ++i) {
			edge_weights[i] = 1;
		}
	}
}

int_t Graph::getVerticesCount() const noexcept {
	return n;
}

int_t Graph::getEdgesCount() const noexcept {
	return m;
}

int_t Graph::getSumOfVertexWeights() const {
	return total_vertex_weight;
}

int_t Graph::getVertexWeight(int_t v) const {
	return vertex_weights[v];
}

Graph Graph::selectSubgraph(const Vector<int_t>& sub_vertices) const {
	Graph subgraph;

	Vector<int_t> original_to_sub(n, -1);
	for (int_t i = 0; i < (int_t)sub_vertices.size(); ++i) {
		original_to_sub[sub_vertices[i]] = i;
	}

	subgraph.n = sub_vertices.size();

	subgraph.vertex_weights.resize(subgraph.n);
	for (int_t i = 0; i < (int_t)sub_vertices.size(); ++i) {
		subgraph.vertex_weights[i] = vertex_weights[sub_vertices[i]];
	}

	subgraph.total_vertex_weight = std::accumulate(subgraph.vertex_weights.begin(), subgraph.vertex_weights.end(), 0);

	Vector<bool> is_exist(n, false);

	for (int_t curr_V: sub_vertices) {
		is_exist[curr_V] = true;
	}

	for (int_t curr_V: sub_vertices) {
		for (int_t i = xadj[curr_V]; i < xadj[curr_V + 1]; ++i) {
			if (is_exist[adjncy[i]]) {
				++subgraph.m;
			}
		}
	}

	subgraph.adjncy.resize(subgraph.m);
	subgraph.edge_weights.resize(subgraph.m);

	subgraph.xadj.resize(subgraph.n + 1);
	subgraph.xadj[0] = 0;

	int_t edge_pos = 0;

	for (int_t i = 0; i < (int_t)sub_vertices.size(); ++i) {
		int_t curr_V         = sub_vertices[i];
		subgraph.xadj[i + 1] = subgraph.xadj[i];
		for (int_t k = xadj[curr_V]; k < xadj[curr_V + 1]; ++k) {
			int_t next_V = adjncy[k];

			if (is_exist[next_V]) {

				int_t j = original_to_sub[next_V];

				int_t weight = edge_weights[k];

				++subgraph.xadj[i + 1];
				subgraph.adjncy[edge_pos]       = j;
				subgraph.edge_weights[edge_pos] = weight;

				++edge_pos;
			}
		}
	}

	return subgraph;
}