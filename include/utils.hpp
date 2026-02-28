#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <numeric>
#include <filesystem>
#include <map>
#include <unordered_map>
#include <functional>
#include <set>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <deque>
#include <bitset>
#include <random>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <cassert>
#include <cstring>
#include <list>
#include <climits>
#include <stack>

using namespace std;

using int_t = long long;
using real_t = double;

enum class Part {
    First,
    Second
};

inline constexpr real_t EPS = 1e-8;

template <typename T> 
using Vector = std::vector<T>;

using String = std::string;

// part: current part  | Part::Second
// returns: other part | Part::First
Part GetOtherPart(Part part);

// folder: path to folder         | "../data/"
// format: extension with dot     | ".mtx"
// returns: vector of full paths  | {"../data/add20.mtx", ...}
Vector<String> GetFileNames(const String& folder, const String& format);

// n: size of permutation (range [0, n-1])     | 5
// returns: shuffled vector of integers 0..n-1 | {4, 2, 3, 0, 1}
Vector<int_t> GetRandomPermutation(int_t n);

// n: upper bound (exclusive, returns in [0, n-1]) | 5
// returns: random integer in range [0, n-1]       | 0
int_t GetRandomInt(int_t n);