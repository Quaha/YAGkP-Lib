#pragma once

#include "matrix.hpp"
#include "utils.hpp"

struct Graph {

	int_t n = 0;
	int_t m = 0;

	int_t total_vertex_weight = 0;

	Vector<int_t> adjncy;
	Vector<int_t> xadj;
	Vector<int_t> vertex_weights;
	Vector<int_t> edge_weights;

	struct AdjacentIterator {
		const Graph& g;
		int_t v;

		struct Iterator {
			const Graph& g;
			int_t pos;

			Iterator(const Graph& g, int_t pos);
			bool operator!=(const Iterator& other) const;
			void operator++();
			std::pair<int_t, int_t> operator*() const;
		};

		Iterator begin() const;
		Iterator end() const;
	};

	AdjacentIterator operator[](int_t v) const;

	Graph();
	Graph(const spMtx<double>& matrix, bool ignore_eweights = false);
	Graph(const String& file_name, const String& format, bool ignore_eweights = false);

	void buildGraph(const spMtx<double>& matrix, bool ignore_eweights);

	int_t getVerticesCount() const noexcept;
	int_t getEdgesCount() const noexcept;
	int_t getSumOfVertexWeights() const;
	int_t getVertexWeight(int_t v) const;

	Graph selectSubgraph(const Vector<int_t>& sub_vertices) const;
};