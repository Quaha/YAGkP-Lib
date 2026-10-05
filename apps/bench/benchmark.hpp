#pragma once

#include <string>

namespace yagkp::benchmark {

	void RunSingle(const std::string& graph_path, int k, const std::string& algo, const std::string& output_dir);

} // namespace yagkp::bench