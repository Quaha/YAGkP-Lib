#include "libs_bridges.hpp"

#include <cstdio>
#include <vector>

#include "config.hpp"
#include "kaHIP_interface.h"
#include "metis.h"
#include "scotch.h"

std::vector<int> RunMETIS(
    int n, int nparts,
    const std::vector<int>& xadj,
    const std::vector<int>& adjncy,
    const std::vector<int>& vwgt,
    const std::string& mode) {

	idx_t _n       = n;
	idx_t _nparts  = nparts;
	idx_t _ncon    = 1;
	idx_t edge_cut = 0;

	std::vector<idx_t> _xadj(xadj.begin(), xadj.end());
	std::vector<idx_t> _adjncy(adjncy.begin(), adjncy.end());
	std::vector<idx_t> _vwgt(vwgt.begin(), vwgt.end());
	std::vector<idx_t> partition(n);

	if (mode == "kway") {
		METIS_PartGraphKway(
		    &_n, &_ncon,
		    _xadj.data(), _adjncy.data(), _vwgt.data(),
		    nullptr, nullptr,
		    &_nparts, nullptr, nullptr, nullptr,
		    &edge_cut, partition.data());
	}
	else if (mode == "recursive") {
		METIS_PartGraphRecursive(
		    &_n, &_ncon,
		    _xadj.data(), _adjncy.data(), _vwgt.data(),
		    nullptr, nullptr,
		    &_nparts, nullptr, nullptr, nullptr,
		    &edge_cut, partition.data());
	}
	else {
		throw std::logic_error("Incorrect mode");
	}

	return std::vector<int>(partition.begin(), partition.end());
}

std::vector<int> RunKaHIP(
    int n, int nparts,
    const std::vector<int>& xadj,
    const std::vector<int>& adjncy,
    const std::vector<int>& vwgt,
    const std::string& mode) {

	int edge_cut     = 0;
	double imbalance = ProgramConfig::imbalance;

	std::vector<int> _xadj(xadj.begin(), xadj.end());
	std::vector<int> _adjncy(adjncy.begin(), adjncy.end());
	std::vector<int> _vwgt(vwgt.begin(), vwgt.end());

	std::vector<int> partition(n);

	int kahip_mode;
	if (mode == "fast") {
		kahip_mode = FAST;
	}
	else if (mode == "eco") {
		kahip_mode = ECO;
	}
	else if (mode == "strong") {
		kahip_mode = STRONG;
	}
	else {
		throw std::logic_error("Incorrect mode");
	}

	kaffpa(
	    &n,
	    _vwgt.data(),
	    _xadj.data(),
	    nullptr,
	    _adjncy.data(),
	    &nparts,
	    &imbalance,
	    true,
	    0,
	    kahip_mode,
	    &edge_cut,
	    partition.data());

	return partition;
}

std::vector<int> RunSCOTCH(
    int n, int nparts,
    const std::vector<int>& xadj,
    const std::vector<int>& adjncy,
    const std::vector<int>& vwgt) {
	SCOTCH_Graph graph;
	SCOTCH_graphInit(&graph);

	SCOTCH_graphBuild(
	    &graph,
	    0,
	    (SCOTCH_Num)n,
	    (SCOTCH_Num*)xadj.data(),
	    (SCOTCH_Num*)(xadj.data() + 1),
	    (SCOTCH_Num*)vwgt.data(),
	    nullptr,
	    (SCOTCH_Num)adjncy.size(),
	    (SCOTCH_Num*)adjncy.data(),
	    nullptr);

	SCOTCH_Strat strat;
	SCOTCH_stratInit(&strat);

	std::vector<int> partition(n);
	SCOTCH_graphPart(&graph, (SCOTCH_Num)nparts, &strat, (SCOTCH_Num*)partition.data());

	SCOTCH_stratExit(&strat);
	SCOTCH_graphExit(&graph);

	return partition;
}