#pragma once
#include <vector>

std::vector<int> RunMETIS_impl(
    int n, int nparts,
    const std::vector<int>& xadj,
    const std::vector<int>& adjncy,
    const std::vector<int>& vwgt);