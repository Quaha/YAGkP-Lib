#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <climits>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using int_t = int;

enum class Part {
	First,
	Second
};

inline constexpr double EPS = 1e-8;

template <typename T>
using Vector = std::vector<T>;

using String = std::string;

template <typename T>
class Queue {
    Vector<T> data;
    int_t head = 0;

public:
    void push(const T& value) { data.push_back(value); }
    void pop() { ++head; }
    T front() const { return data[head]; }
    bool empty() const { return head >= (int_t)data.size(); }

    void reserve(int_t n) { data.reserve(n); }
    void clear() { data.clear(); head = 0; }
};

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