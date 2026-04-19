#pragma once

#include <string>
#include <vector>

std::vector<int> RunMETIS(
    int n, int nparts,
    const std::vector<int>& xadj,
    const std::vector<int>& adjncy,
    const std::vector<int>& vwgt,
    const std::string& mode);

std::vector<int> RunKaHIP(
    int n, int nparts,
    const std::vector<int>& xadj,
    const std::vector<int>& adjncy,
    const std::vector<int>& vwgt,
    const std::string& mode);

std::vector<int> RunSCOTCH(
    int n, int nparts,
    const std::vector<int>& xadj,
    const std::vector<int>& adjncy,
    const std::vector<int>& vwgt);