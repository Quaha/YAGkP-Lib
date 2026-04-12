#pragma once

#include <string>

#include "config.hpp"
#include "graph.hpp"
#include "metrics.hpp"
#include "partitioner.hpp"
#include "utils.hpp"

namespace Benchmark {

	void RunSingle(const std::string& graph_path, int k, const std::string& algo, const std::string& output_dir);

} // namespace Benchmark