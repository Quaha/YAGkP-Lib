#pragma once

#include <vector>

#include "yagkp/types.hpp"

namespace yagkp {
	template <typename T>
	class Queue {
		std::vector<T> data;
		int_t head = 0;

	  public:
		void push(const T& value) {
			data.push_back(value);
		}
		void pop() {
			++head;
		}
		T front() const {
			return data[head];
		}
		bool empty() const {
			return head >= (int_t)data.size();
		}
		void reserve(int_t n) {
			data.reserve(n);
		}
		void clear() {
			data.clear();
			head = 0;
		}
	};
} // namespace yagkp