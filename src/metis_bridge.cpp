#include "metis_bridge.hpp"

#include "metis.h"

#include <vector>

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