#include "libs_bridges.hpp"

#include <cstdio>
#include <vector>

#include "metis.h"
#include "scotch.h"

std::vector<int> RunMETIS_impl(
    int n, int nparts,
    const std::vector<int>& xadj,
    const std::vector<int>& adjncy,
    const std::vector<int>& vwgt) {
	idx_t _n       = n;
	idx_t _nparts  = nparts;
	idx_t _ncon    = 1;
	idx_t edge_cut = 0;

	std::vector<idx_t> _xadj(xadj.begin(), xadj.end());
	std::vector<idx_t> _adjncy(adjncy.begin(), adjncy.end());
	std::vector<idx_t> _vwgt(vwgt.begin(), vwgt.end());
	std::vector<idx_t> partition(n);

	METIS_PartGraphKway(
	    &_n, &_ncon,
	    _xadj.data(), _adjncy.data(), _vwgt.data(),
	    nullptr, nullptr,
	    &_nparts, nullptr, nullptr, nullptr,
	    &edge_cut, partition.data());

	return std::vector<int>(partition.begin(), partition.end());
}

std::vector<int> RunSCOTCH_impl(
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