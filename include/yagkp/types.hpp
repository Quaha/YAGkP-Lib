#pragma once

#include <cstdint>

namespace yagkp {

	// Signed integer type
	using int_t = std::int64_t;

	// Floating-point type
	using fp_t = double;

	enum class Part {
		First,
		Second
	};

	template <typename T>
	void setmax(T& a, T b) {
		if (b > a) {
			a = b;
		}
	}

	template <typename T>
	void setmin(T& a, T b) {
		if (b < a) {
			a = b;
		}
	}

	inline Part GetOtherPart(Part part) {
		if (part == Part::First) {
			return Part::Second;
		}
		return Part::First;
	}
} // namespace yagkp