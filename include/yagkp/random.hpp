#pragma once

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "yagkp/types.hpp"

namespace yagkp {
	/**
	 * @brief Get the random permutation
	 *
	 * @param n size of permutation (ex. 5)
	 * @return shuffled vector of integers 0..n-1 (ex. {4, 2, 3, 0, 1})
	 */
	inline std::vector<int_t> GetRandomPermutation(int_t n) {
		static std::mt19937 rng{std::random_device{}()};

		std::vector<int_t> p(n);
		std::iota(p.begin(), p.end(), 0);
		std::shuffle(p.begin(), p.end(), rng);
		return p;
	}
} // namespace yagkp