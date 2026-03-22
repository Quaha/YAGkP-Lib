#include "config.hpp"
#include "graph.hpp"
#include "metrics.hpp"
#include "partitioner.hpp"
#include "utils.hpp"

namespace Benchmark {

	static Vector<int_t> RunMETIS(const Graph& g, int_t k);
	static Vector<int_t> RunKaHIP(const Graph& g, int_t k);

	void PrintBenchmark();

} // namespace Benchmark