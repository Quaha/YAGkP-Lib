#pragma once

#include "matrix.hpp"
#include "utils.hpp"

// Graph stored in Compressed Row Storage (CRS/CSR) format.
// Vertices are numbered starting from 0.
struct Graph {

	int_t n = 0; // Number of vertices
	int_t m = 0; // Number of edges

	// Adjacency list:
	//   adjncy[xadj[u] .. xadj[u+1]-1] contains neighbors of vertex u.
	Vector<int_t> adjncy;

	// Row pointer array:
	//   xadj[u] = index in `adjncy` where adjacency of vertex u begins.
	//   Size = n + 1, with xadj[n] = m.
	Vector<int_t> xadj;

	// Vertex weights (size = n).
	Vector<int_t> vertex_weights;

	// Edge weights (size = m).
	Vector<int_t> edge_weights;

	struct AdjacentIterator {
		const Graph& g;
		int_t v;

		struct Iterator {
			const Graph& g;
			int_t pos;

			Iterator(const Graph& g, int_t pos) : g(g), pos(pos) {}

			bool operator!=(const Iterator& other) const {
				return pos != other.pos;
			}

			void operator++() {
				++pos;
			}

			std::pair<int_t, int_t> operator*() const {
				return std::make_pair(g.adjncy[pos], g.edge_weights[pos]);
			}
		};

		Iterator begin() const {
			return Iterator(g, g.xadj[v]);
		}

		Iterator end() const {
			return Iterator(g, g.xadj[v + 1]);
		}
	};

	AdjacentIterator operator[](int_t v) const {
		return AdjacentIterator{ *this, v };
	}

	void buildGraph(const spMtx<int_t>& matrix, bool ignore_eweights) {
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
			vertex_weights[i] = (int_t)(1);
		}

		edge_weights.resize(m);
		if (matrix.Val != nullptr && !ignore_eweights) {
			for (int_t i = 0; i < m; ++i) {
				edge_weights[i] = matrix.Val[i];
			}
		}
		else {
			for (int_t i = 0; i < m; ++i) {
				edge_weights[i] = (int_t)(1);
			}
		}
	}

	Graph() {

	}

	// Requires a matrix corresponding to an undirected graph
	Graph(const spMtx<int_t>& matrix, bool ignore_eweights = false) {
		buildGraph(matrix, ignore_eweights);
	}

	// Requires a matrix corresponding to an undirected graph
	Graph(const String& file_name, const String& format, bool ignore_eweights = false) {
		spMtx<int_t> matrix(file_name.c_str(), format);
		buildGraph(matrix, ignore_eweights);
	}

	Graph(
		const Vector<int_t>& vertex_weights,
		const Vector<std::tuple<int_t, int_t, int_t>>& edges
	) {
		n = static_cast<int_t>(vertex_weights.size());
		this->vertex_weights = vertex_weights;

		Vector<int_t> degree(n, 0);
		for (auto& [u, v, w] : edges) {
			degree[u]++;
			degree[v]++;
		}

		xadj.resize(n + 1, 0);
		for (int_t i = 1; i <= n; ++i) {
			xadj[i] = xadj[i - 1] + degree[i - 1];
		}

		m = xadj[n];

		adjncy.resize(m);
		edge_weights.resize(m);

		Vector<int_t> offset = xadj;

		for (auto& [u, v, w] : edges) {
			adjncy[offset[u]] = v;
			edge_weights[offset[u]] = w;
			++offset[u];

			adjncy[offset[v]] = u;
			edge_weights[offset[v]] = w;
			++offset[v];
		}
	}

	int_t getVerticesCount() const noexcept {
		return n;
	}

	int_t getEdgesCount() const noexcept {
		return m;
	}

	// This function returns a subgraph of the current graph, where
	// the vertices were mapped according to the order in vertices.
	Graph selectSubgraph(const Vector<int_t>& vertices) const {
		Graph subgraph;

		std::unordered_map<int_t, int_t> original_to_sub;
		for (int_t i = 0; i < vertices.size(); ++i) {
			original_to_sub[vertices[i]] = i;
		}

		subgraph.n = vertices.size();

		subgraph.vertex_weights.resize(subgraph.n);
		for (int_t i = 0; i < vertices.size(); ++i) {
			subgraph.vertex_weights[i] = vertex_weights[vertices[i]];
		}

		Vector<bool> is_exist(n, false);

		for (int_t curr_V : vertices) {
			is_exist[curr_V] = true;
		}

		for (int_t curr_V = 0; curr_V < n; ++curr_V) {
			for (int_t i = xadj[curr_V]; i < xadj[curr_V + 1]; ++i) {
				int_t next_V = adjncy[i];

				if (is_exist[curr_V] && is_exist[next_V]) {
					subgraph.m++;
				}
			}
		}

		subgraph.adjncy.resize(subgraph.m);
		subgraph.edge_weights.resize(subgraph.m);

		subgraph.xadj.resize(subgraph.n + 1);
		subgraph.xadj[0] = 0;

		int_t edge_pos = 0;

		for (int_t i = 0; i < vertices.size(); ++i) {
			int_t curr_V = vertices[i];
			subgraph.xadj[i + 1] = subgraph.xadj[i];
			for (int_t k = xadj[curr_V]; k < xadj[curr_V + 1]; ++k) {
				int_t next_V = adjncy[k];

				if (is_exist[curr_V] && is_exist[next_V]) {

					int_t j = original_to_sub[next_V];

					int_t weight = edge_weights[k];

					++subgraph.xadj[i + 1];
					subgraph.adjncy[edge_pos] = j;
					subgraph.edge_weights[edge_pos] = weight;

					++edge_pos;
				}
			}
		}

		return subgraph;
	}

	void printEdges() const {
		for (int_t curr_V = 0; curr_V < n; ++curr_V) {
			for (int_t i = xadj[curr_V]; i < xadj[curr_V + 1]; ++i) {
				int_t next_V = adjncy[i];
				int_t w = edge_weights[i];
				std::cout << curr_V << " " << next_V << " " << w << "\n";
			}
		}
	}

	bool operator==(const Graph& other) const {
		if (n != other.n || m != other.m) {
			return false;
		}
		if (vertex_weights.size() != other.vertex_weights.size()) {
			return false;
		}
		if (xadj.size() != other.xadj.size()) {
			return false;
		}
		if (edge_weights.size() != other.edge_weights.size()) {
			return false;
		}

		for (int_t i = 0; i < n; ++i) {
			if ((std::abs(vertex_weights[i] - other.vertex_weights[i]) > EPS)) {
				return false;
			}
		}

		std::map<std::pair<int_t, int_t>, int_t> edges;

		for (int_t curr_V = 0; curr_V < n; ++curr_V) {
			for (int_t i = xadj[curr_V]; i < xadj[curr_V + 1]; ++i) {
				int_t next_V = adjncy[i];
				if (curr_V <= next_V) {
					std::pair<int_t, int_t> key = std::make_pair(curr_V, next_V);
					edges[key] += edge_weights[i];
				}
			}
		}

		for (int_t curr_V = 0; curr_V < other.n; ++curr_V) {
			for (int_t i = other.xadj[curr_V]; i < other.xadj[curr_V + 1]; ++i) {
				int_t next_V = other.adjncy[i];
				if (curr_V <= next_V) {
					std::pair<int_t, int_t> key = { curr_V, next_V };
					auto it = edges.find(key);
					if (it == edges.end()) {
						return false;
					}
					it->second -= other.edge_weights[i];
					if (std::abs(it->second) < EPS) {
						edges.erase(it);
					}
				}
			}
		}

		return edges.empty();
	}

	bool operator!=(const Graph& other) const {
		return !(*this == other);
	}

	int_t getSumOfVertexWeights() const {
		int_t result = (int_t)(0);
		for (int_t i = 0; i < n; ++i) {
			result += vertex_weights[i];
		}
		return result;
	}

	int_t getVertexWeight(int_t v) const {
		return vertex_weights[v];
	}
};