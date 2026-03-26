#include "config.hpp"
#include "graph.hpp"
#include "metrics.hpp"
#include "partitioner.hpp"
#include "utils.hpp"

#include <string>

namespace Benchmark {

	static Vector<int_t> RunKaHIP(const Graph& g, int_t k);

	void PrintBenchmark();
	void RunSingle(const std::string& graph_path, int k, const std::string& algo, const std::string& output_dir);

} // namespace Benchmark