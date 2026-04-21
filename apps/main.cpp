#include <iostream>
#include <string>

#include "benchmark.hpp"

// ./YAGkP_app --graph ../data/add20.mtx --k 4 --algo kahip_strong --output ../benchmark/results/

int main(int argc, char* argv[]) {

	std::string graph_path;
	std::string algo;
	std::string output_dir;
	int k = 0;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "--graph" && i + 1 < argc) {
			graph_path = argv[++i];
		}
		else if (arg == "--k" && i + 1 < argc) {
			k = std::stoi(argv[++i]);
		}
		else if (arg == "--algo" && i + 1 < argc) {
			algo = argv[++i];
		}
		else if (arg == "--output" && i + 1 < argc) {
			output_dir = argv[++i];
		}
	}

	if (graph_path.empty() || k == 0 || algo.empty() || output_dir.empty()) {
		std::cerr << "Usage: YAGkP_app --graph <path> --k <k> --algo <algo> --output <dir>" << std::endl;
		std::cerr << "Algos: yagkp, kahip_strong, kahip_eco, kahip_fast, metis_kway, metis_recursive, scotch" << std::endl;
		return 1;
	}

	Benchmark::RunSingle(graph_path, k, algo, output_dir);

	return 0;
}