#include "benchmark.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

namespace Benchmark {
	Vector<BenchmarkResult> RunBenchmark(const Vector<String>& files, const Vector<int_t>& ks) {
		Vector<BenchmarkResult> results;

		for (const auto& path: files) {
			String graph_name = std::filesystem::path(path).filename().string();

			std::cout << "==============================================" << std::endl;
			std::cout << "Graph: " << graph_name << std::endl;

			Graph g(path, "mtx", true);

			std::cout << "n = " << g.getVerticesCount() << ", m = " << g.getEdgesCount() << std::endl;

			for (int_t k: ks) {
				auto start_t = std::chrono::steady_clock::now();

				Vector<int_t> partition = Partitioner::GetGraphKPartition(g, k);

				auto end_t        = std::chrono::steady_clock::now();
				long long time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count();

				BenchmarkResult result;
				result.graph_name          = graph_name;
				result.n                   = g.getVerticesCount();
				result.m                   = g.getEdgesCount();
				result.k                   = k;
				result.time_ms             = time_ms;
				result.edge_cut            = PartitionMetrics::GetEdgeCut(g, partition);
				result.imbalance           = PartitionMetrics::GetAccuracy(g, k, partition);
				result.max_part_weight     = PartitionMetrics::GetMaxPartWeight(g, k, partition);
				result.optimal_part_weight = static_cast<real_t>(g.getSumOfVertexWeights()) / static_cast<real_t>(k);

				results.push_back(result);

				std::cout << "  k = " << k
				          << " | time = " << time_ms << " ms"
				          << " | edge cut = " << result.edge_cut
				          << " | imbalance = " << std::fixed << std::setprecision(2) << result.imbalance * 100.0 << "%"
				          << std::endl;
			}
		}

		return results;
	}

	void PrintResults(const Vector<BenchmarkResult>& results) {
		std::cout << std::fixed << std::setprecision(4);
		std::cout << "\n===================== SUMMARY =====================" << std::endl;
		std::cout << std::left
		          << std::setw(24) << "Graph"
		          << std::setw(6) << "k"
		          << std::setw(12) << "Time (ms)"
		          << std::setw(14) << "Edge Cut"
		          << std::setw(14) << "Imbalance"
		          << std::setw(14) << "Max Part"
		          << std::setw(14) << "Opt Part"
		          << std::endl;

		std::cout << std::string(88, '-') << std::endl;

		for (const auto& r: results) {
			std::cout << std::left
			          << std::setw(24) << r.graph_name
			          << std::setw(6) << r.k
			          << std::setw(12) << r.time_ms
			          << std::setw(14) << r.edge_cut
			          << std::setw(13) << std::setprecision(2) << r.imbalance * 100.0 << "%"
			          << std::setw(14) << r.max_part_weight
			          << std::setw(14) << std::setprecision(2) << r.optimal_part_weight
			          << std::endl;
		}
	}

	void SaveResultsCSV(const Vector<BenchmarkResult>& results, const String& output_path, const String& run_name) {
		std::ofstream file(output_path);

		if (!file.is_open()) {
			std::cerr << "Failed to open output file: " << output_path << std::endl;
			return;
		}

		file << "run,graph,n,m,k,time_ms,edge_cut,imbalance,max_part_weight,optimal_part_weight\n";

		for (const auto& r: results) {
			file << run_name << ","
			     << r.graph_name << ","
			     << r.n << ","
			     << r.m << ","
			     << r.k << ","
			     << r.time_ms << ","
			     << r.edge_cut << ","
			     << r.imbalance << ","
			     << r.max_part_weight << ","
			     << r.optimal_part_weight
			     << "\n";
		}

		std::cout << "Results saved to: " << output_path << std::endl;
	}

	void PrintBenchmark() {
		std::cout << "accuracy (imbalance): " << ProgramConfig::accuracy * 100.0 << "%" << std::endl;

		const Vector<String> files = GetFileNames("../data", ".mtx");
		const Vector<int_t> ks     = {2, 4, 8, 16, 32, 64, 128, 256};

		auto results = RunBenchmark(files, ks);

		PrintResults(results);

		SaveResultsCSV(results, "../experiments/results_base.csv", "Base");
	}
} // namespace Benchmark