#pragma once

#include <string>

namespace yagkp::benchmark {

	// Заголовок строк, которые RunSingle печатает с csv = true
	inline constexpr const char* kCsvHeader =
	    "graph,n,m,k,eps,solver,threads,time_ms,edge_cut,imbalance,max_part";

	// Один запуск алгоритма algo на графе. Результат дописывается в
	// output_dir/<граф>_k<k>.txt, если output_dir не пуст, и печатается в
	// stdout: таблицей для человека или, с csv = true, одной строкой CSV
	// (поля — kCsvHeader)
	void RunSingle(
	    const std::string& graph_path,
	    int k,
	    const std::string& algo,
	    const std::string& output_dir,
	    bool csv
	);

} // namespace yagkp::benchmark