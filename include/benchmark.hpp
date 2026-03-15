#pragma once

#include "graph.hpp"
#include "metrics.hpp"
#include "partitioner.hpp"
#include "utils.hpp"

namespace Benchmark {
	struct BenchmarkResult {
		String graph_name;
		int_t n;
		int_t m;
		int_t k;
		long long time_ms;
		real_t edge_cut;
		real_t imbalance;
		int_t max_part_weight;
		real_t optimal_part_weight;
	};

	Vector<BenchmarkResult> RunBenchmark(const Vector<String>& files, const Vector<int_t>& ks);

	void PrintResults(const Vector<BenchmarkResult>& results);

	void SaveResultsCSV(const Vector<BenchmarkResult>& results, const String& output_path, const String& run_name);

	void PrintBenchmark();
} // namespace Benchmark