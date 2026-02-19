#include "utils.hpp"

static std::random_device rd;
static std::mt19937 rng(rd());

Vector<String> GetFileNames(const String& folder, const String& format) {
    Vector<String> file_names;
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_regular_file() && entry.path().extension() == format) {
            file_names.push_back(entry.path().string());
        }
    }
    return file_names;
}

Vector<int_t> GetRandomPermutation(int_t n) {
    Vector<int_t> permutation(n);
    for (int_t i = 0; i < n; i++) {
        permutation[i] = i;
    }

    std::shuffle(permutation.begin(), permutation.end(), rng);

    return permutation;
}

int_t GetRandomInt(int_t n) {
	std::uniform_int_distribution<int_t> dist(0, n - 1);
	return dist(rng);
}