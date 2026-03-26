#include "benchmark.hpp"

#include <fstream>

#include "libs_bridges.hpp"

namespace Benchmark {

	void RunSingle(const std::string& graph_path, int k, const std::string& algo, const std::string& output_dir) {
		String graph_name = std::filesystem::path(graph_path).filename().string();
		Graph g(graph_path, "mtx", true);

		auto run = [&](const std::string& name) -> std::tuple<long long, int_t, double, int_t> {
			Vector<int_t> partition;

			auto start = std::chrono::steady_clock::now();

			if (name == "yagkp") {
				partition = Partitioner::GetGraphKPartition(g, k);
			}
			else if (name == "kahip") {
				partition = RunKaHIP(g.n, k, std::vector<int>(g.xadj.begin(), g.xadj.end()),
				                     std::vector<int>(g.adjncy.begin(), g.adjncy.end()),
				                     std::vector<int>(g.vertex_weights.begin(), g.vertex_weights.end()));
			}
			else if (name == "metis") {
				auto v    = RunMETIS(g.n, k,
				                     std::vector<int>(g.xadj.begin(), g.xadj.end()),
				                     std::vector<int>(g.adjncy.begin(), g.adjncy.end()),
				                     std::vector<int>(g.vertex_weights.begin(), g.vertex_weights.end()));
				partition = Vector<int_t>(v.begin(), v.end());
			}
			else if (name == "scotch") {
				auto v    = RunSCOTCH(g.n, k,
				                      std::vector<int>(g.xadj.begin(), g.xadj.end()),
				                      std::vector<int>(g.adjncy.begin(), g.adjncy.end()),
				                      std::vector<int>(g.vertex_weights.begin(), g.vertex_weights.end()));
				partition = Vector<int_t>(v.begin(), v.end());
			}
			else {
				throw std::runtime_error("Unknown algo: " + name);
			}

			auto end          = std::chrono::steady_clock::now();
			long long time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			return {
			    time_ms,
			    PartitionMetrics::GetEdgeCut(g, partition),
			    PartitionMetrics::GetAccuracy(g, k, partition),
			    PartitionMetrics::GetMaxPartWeight(g, k, partition)};
		};

		auto [time_ms, edge_cut, imbalance, max_part] = run(algo);
		double opt                                    = static_cast<double>(g.getSumOfVertexWeights()) / static_cast<double>(k);

		std::string stem = graph_name + "_k" + std::to_string(k);

		// === CSV ===
		std::string csv_dir = output_dir + "/csv";
		std::filesystem::create_directories(csv_dir);
		std::string csv_path = csv_dir + "/" + stem + ".csv";
		bool csv_exists      = std::filesystem::exists(csv_path);

		std::ofstream csv(csv_path, std::ios::app);
		if (!csv_exists) {
			csv << "graph,n,m,k,algo,time_ms,edge_cut,imbalance,max_part_weight,opt_part_weight\n";
		}
		csv << graph_name << "," << g.n << "," << g.m << "," << k << ","
		    << algo << "," << time_ms << "," << edge_cut << ","
		    << imbalance << "," << max_part << "," << opt << "\n";

		// === TXT ===
		std::string txt_dir = output_dir + "/txt";
		std::filesystem::create_directories(txt_dir);
		std::string txt_path = txt_dir + "/" + stem + ".txt";
		bool txt_exists      = std::filesystem::exists(txt_path);

		std::ofstream txt(txt_path, std::ios::app);
		if (!txt_exists) {
			txt << "Graph: " << graph_name << "  n=" << g.n << "  m=" << g.m << "  k=" << k << " " << "imb=" << ProgramConfig::accuracy << "\n";
			txt << std::string(88, '-') << "\n";
			txt << std::left
			    << std::setw(16) << "algo"
			    << std::setw(12) << "time(ms)"
			    << std::setw(14) << "edge cut"
			    << std::setw(12) << "imbalance"
			    << std::setw(14) << "max part"
			    << std::setw(14) << "opt part"
			    << "\n";
			txt << std::string(88, '-') << "\n";
		}

		std::ostringstream pct;
		pct << std::fixed << std::setprecision(2) << imbalance * 100.0 << "%";

		txt << std::left << std::fixed << std::setprecision(2)
		    << std::setw(16) << algo
		    << std::setw(12) << time_ms
		    << std::setw(14) << edge_cut
		    << std::setw(12) << pct.str()
		    << std::setw(14) << max_part
		    << std::setw(14) << opt
		    << "\n";

		// Вывод в stdout
		std::cout << std::left << std::fixed << std::setprecision(2)
		          << std::setw(16) << algo
		          << std::setw(12) << time_ms
		          << std::setw(14) << edge_cut
		          << std::setw(12) << pct.str()
		          << std::setw(14) << max_part
		          << std::setw(14) << opt
		          << "\n";
	}
} // namespace Benchmark