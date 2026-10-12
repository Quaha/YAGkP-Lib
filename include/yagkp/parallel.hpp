#pragma once

#ifdef _OPENMP
#include <omp.h>
#endif

#include "yagkp/types.hpp"

namespace yagkp::parallel {
#ifdef _OPENMP
	inline constexpr bool enabled = true;
#else
	inline constexpr bool enabled = false;
#endif

	inline int_t MaxThreads() {
#ifdef _OPENMP
		return omp_get_max_threads();
#else
		return 1;
#endif
	}

	inline int_t ThreadId() {
#ifdef _OPENMP
		return omp_get_thread_num();
#else
		return 0;
#endif
	}
} // namespace yagkp::parallel