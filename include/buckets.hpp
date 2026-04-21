#pragma once

#include "utils.hpp"

class BucketPQ {
	struct Node {
		int_t prev = -1;
		int_t next = -1;
	};

	Vector<int_t> buckets;
	Vector<Node> nodes;
	Vector<int_t> gain_value;
	int_t min_gain;
	int_t max_gain;
	int_t n_nodes = 0;

  public:
	// n - amount of vertices in the graph, [min_g, max_g] - gains range
	BucketPQ(int_t n, int_t min_g, int_t max_g)
	    : buckets(max_g - min_g + 1, -1),
	      nodes(n),
	      gain_value(n, min_g - 1),
	      min_gain(min_g),
	      max_gain(min_g) {
	}

	void insert(int_t gain, int_t id) {
		int_t idx      = gain - min_gain;
		gain_value[id] = gain;

		nodes[id].next = buckets[idx];
		nodes[id].prev = -1;
		if (buckets[idx] != -1) {
			nodes[buckets[idx]].prev = id;
		}
		buckets[idx] = id;

		if (max_gain < gain) {
			max_gain = gain;
		}
		n_nodes++;
	}

	void add(int_t delta, int_t id) {
		if (gain_value[id] < min_gain) {
			return;
		}

		int_t old_gain = gain_value[id];
		int_t old_idx  = old_gain - min_gain;

		if (nodes[id].prev != -1) {
			nodes[nodes[id].prev].next = nodes[id].next;
		}
		else {
			buckets[old_idx] = nodes[id].next;
		}

		if (nodes[id].next != -1) {
			nodes[nodes[id].next].prev = nodes[id].prev;
		}

		int_t new_gain = old_gain + delta;
		int_t new_idx  = new_gain - min_gain;
		gain_value[id] = new_gain;

		nodes[id].next = buckets[new_idx];
		nodes[id].prev = -1;
		if (buckets[new_idx] != -1) {
			nodes[buckets[new_idx]].prev = id;
		}
		buckets[new_idx] = id;

		if (new_gain > max_gain) {
			max_gain = new_gain;
		}
		else if (old_gain == max_gain && buckets[old_idx] == -1) {
			while (buckets[max_gain - min_gain] == -1) {
				max_gain--;
			}
		}
	}

	std::pair<int_t, int_t> extract() {
		int_t idx  = max_gain - min_gain;
		int_t id   = buckets[idx];
		int_t gain = gain_value[id];

		buckets[idx] = nodes[id].next;
		if (nodes[id].next != -1) {
			nodes[nodes[id].next].prev = -1;
		}

		gain_value[id] = min_gain - 1;
		n_nodes--;

		if (n_nodes > 0) {
			while (buckets[max_gain - min_gain] == -1) {
				max_gain--;
			}
		}
		else {
			max_gain = min_gain;
		}

		return {gain, id};
	}

	std::pair<int_t, int_t> top() const {
		int_t idx = max_gain - min_gain;
		int_t id  = buckets[idx];
		return {gain_value[id], id};
	}

	bool empty() const {
		return n_nodes == 0;
	}
};