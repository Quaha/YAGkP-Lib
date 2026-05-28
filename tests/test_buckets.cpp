#include <gtest/gtest.h>

#include <algorithm>
#include <queue>
#include <random>
#include <vector>

#include "buckets.hpp"

// =============================================================================
// BucketPriorityQueue — executable documentation
//
// BucketPriorityQueue is a max-priority queue specialized for integer gains
// in a known, bounded range. It stores at most `n` distinct elements,
// identified by integer ids in [0, n).
//
// Each test below demonstrates one specific guarantee of the contract.
// =============================================================================


// -----------------------------------------------------------------------------
// 1. Construction
// -----------------------------------------------------------------------------

// The constructor takes:
//   - n:     maximum number of distinct ids the queue can hold;
//            valid ids are in [0, n).
//   - min_g: smallest gain that may ever be stored.
//   - max_g: largest gain that may ever be stored.
// A freshly constructed queue is empty.
TEST(BucketPriorityQueue, IsEmptyAfterConstruction) {
    BucketPriorityQueue pq(/*n=*/5, /*min_g=*/-10, /*max_g=*/10);

    EXPECT_TRUE(pq.empty());
}


// -----------------------------------------------------------------------------
// 2. insert + extract: the basic round-trip
// -----------------------------------------------------------------------------

// `insert(gain, id)` adds the element (id, gain) to the queue.
// `extract()` removes and returns the element with the maximum gain
// as a pair {gain, id}.
TEST(BucketPriorityQueue, InsertThenExtractReturnsSameElement) {
    BucketPriorityQueue pq(5, -10, 10);

    pq.insert(/*gain=*/3, /*id=*/2);

    auto [gain, id] = pq.extract();
    EXPECT_EQ(gain, 3);
    EXPECT_EQ(id, 2);
}

// After extracting the only element, the queue becomes empty again.
TEST(BucketPriorityQueue, BecomesEmptyAfterLastExtract) {
    BucketPriorityQueue pq(5, -10, 10);
    pq.insert(3, 2);

    pq.extract();

    EXPECT_TRUE(pq.empty());
}


// -----------------------------------------------------------------------------
// 3. Priority order: this is a max-heap
// -----------------------------------------------------------------------------

// Given two elements, extract() returns the one with the larger gain first.
TEST(BucketPriorityQueue, ExtractReturnsLargerGainFirst) {
    BucketPriorityQueue pq(5, -10, 10);

    pq.insert(/*gain=*/2, /*id=*/0);
    pq.insert(/*gain=*/7, /*id=*/1);

    EXPECT_EQ(pq.extract().second, 1);   // gain=7 comes out first
    EXPECT_EQ(pq.extract().second, 0);   // then gain=2
}

// The order in which elements were inserted does not matter —
// only the gain does.
TEST(BucketPriorityQueue, InsertionOrderDoesNotAffectExtractionOrder) {
    BucketPriorityQueue pq(5, -10, 10);

    // Insert in increasing order of gain.
    pq.insert(1, 0);
    pq.insert(5, 1);
    pq.insert(9, 2);

    EXPECT_EQ(pq.extract().first, 9);
    EXPECT_EQ(pq.extract().first, 5);
    EXPECT_EQ(pq.extract().first, 1);
}


// -----------------------------------------------------------------------------
// 4. top(): peek without removing
// -----------------------------------------------------------------------------

// `top()` returns the maximum-gain element but does not modify the queue.
TEST(BucketPriorityQueue, TopReturnsMaxButDoesNotRemove) {
    BucketPriorityQueue pq(5, -10, 10);
    pq.insert(4, 0);
    pq.insert(1, 1);

    auto peeked = pq.top();
    EXPECT_EQ(peeked.first, 4);
    EXPECT_EQ(peeked.second, 0);

    // The element is still there; extract returns the same pair.
    auto extracted = pq.extract();
    EXPECT_EQ(extracted.first, 4);
    EXPECT_EQ(extracted.second, 0);
}


// -----------------------------------------------------------------------------
// 5. Boundary gains
// -----------------------------------------------------------------------------

// Gains exactly equal to `min_g` and `max_g` are valid.
TEST(BucketPriorityQueue, AcceptsGainsAtBothBoundaries) {
    BucketPriorityQueue pq(/*n=*/3, /*min_g=*/-5, /*max_g=*/5);

    pq.insert(-5, 0);   // lower boundary
    pq.insert( 5, 1);   // upper boundary
    pq.insert( 0, 2);   // middle

    EXPECT_EQ(pq.extract().second, 1);   // +5
    EXPECT_EQ(pq.extract().second, 2);   //  0
    EXPECT_EQ(pq.extract().second, 0);   // -5
}

// A degenerate range [g, g] is allowed: every element has the same gain.
TEST(BucketPriorityQueue, AcceptsDegenerateSingleValueRange) {
    BucketPriorityQueue pq(/*n=*/3, /*min_g=*/7, /*max_g=*/7);

    pq.insert(7, 0);
    pq.insert(7, 1);
    pq.insert(7, 2);

    int count = 0;
    while (!pq.empty()) {
        EXPECT_EQ(pq.extract().first, 7);
        ++count;
    }
    EXPECT_EQ(count, 3);
}


// -----------------------------------------------------------------------------
// 6. add(): incremental gain updates
// -----------------------------------------------------------------------------

// `add(delta, id)` adjusts the stored gain of `id` by `delta`,
// without removing it from the queue.
TEST(BucketPriorityQueue, AddShiftsGainOfStoredElement) {
    BucketPriorityQueue pq(5, -10, 10);
    pq.insert(2, 0);

    pq.add(/*delta=*/3, /*id=*/0);   // gain: 2 -> 5

    auto [gain, id] = pq.extract();
    EXPECT_EQ(gain, 5);
    EXPECT_EQ(id, 0);
}

// Increasing the gain of a non-top element may make it the new top.
TEST(BucketPriorityQueue, AddCanPromoteElementToTop) {
    BucketPriorityQueue pq(5, -10, 10);
    pq.insert(1, 0);
    pq.insert(2, 1);
    ASSERT_EQ(pq.top().second, 1);   // gain=2 wins initially

    pq.add(+5, 0);                   // id=0: gain 1 -> 6

    EXPECT_EQ(pq.top().second, 0);
    EXPECT_EQ(pq.top().first, 6);
}

// Decreasing the gain of the current top may demote it,
// and the queue must correctly track the new maximum.
TEST(BucketPriorityQueue, AddCanDemoteTopAndMaxSlidesDown) {
    BucketPriorityQueue pq(5, -10, 10);
    pq.insert(5, 0);                 // current top
    pq.insert(3, 1);

    pq.add(-4, 0);                   // id=0: gain 5 -> 1

    auto [gain, id] = pq.top();
    EXPECT_EQ(id, 1);
    EXPECT_EQ(gain, 3);
}

// Multiple consecutive add() calls accumulate.
TEST(BucketPriorityQueue, ConsecutiveAddsAccumulate) {
    BucketPriorityQueue pq(5, -10, 10);
    pq.insert(0, 0);

    pq.add(+3, 0);   // 0 -> 3
    pq.add(-1, 0);   // 3 -> 2
    pq.add(+4, 0);   // 2 -> 6

    EXPECT_EQ(pq.extract().first, 6);
}


// -----------------------------------------------------------------------------
// 7. add() on absent ids: silently ignored
// -----------------------------------------------------------------------------

// After an id is extracted, it is no longer in the queue.
// Calling add() on it is a no-op — the element is NOT re-inserted.
// This is the contract that the matching algorithms (KL, GGGA) rely on:
// once a vertex is "consumed", further gain updates from its neighbors
// can be issued without checking whether it still lives in the queue.
TEST(BucketPriorityQueue, AddIsNoopAfterElementWasExtracted) {
    BucketPriorityQueue pq(5, -10, 10);
    pq.insert(3, 0);
    pq.extract();
    ASSERT_TRUE(pq.empty());

    pq.add(+100, 0);   // must NOT bring id=0 back

    EXPECT_TRUE(pq.empty());
}

// Same contract for ids that were never inserted at all.
TEST(BucketPriorityQueue, AddIsNoopForNeverInsertedId) {
    BucketPriorityQueue pq(5, -10, 10);
    pq.insert(3, 0);

    pq.add(+5, /*id=*/4);   // id=4 was never inserted

    auto [gain, id] = pq.extract();
    EXPECT_EQ(id, 0);
    EXPECT_EQ(gain, 3);
    EXPECT_TRUE(pq.empty());
}


// -----------------------------------------------------------------------------
// 8. Stress tests: behavior matches std::priority_queue on the gain sequence
// -----------------------------------------------------------------------------

// On a randomized workload of inserts, the sequence of extracted gains
// is identical to that of std::priority_queue (which is a max-heap by default).
// Note: ties between equal gains may be broken differently — we only compare
// the gain values, not the ids.
TEST(BucketPriorityQueue, StressMatchesStdPriorityQueueOnInsertOnly) {
    constexpr int N = 500;
    constexpr int MIN_G = -50, MAX_G = 50;

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> gain_dist(MIN_G, MAX_G);

    BucketPriorityQueue pq(N, MIN_G, MAX_G);
    std::priority_queue<std::pair<int, int>> reference;

    for (int id = 0; id < N; ++id) {
        int g = gain_dist(rng);
        pq.insert(g, id);
        reference.emplace(g, id);
    }

    while (!reference.empty()) {
        ASSERT_FALSE(pq.empty());
        EXPECT_EQ(pq.extract().first, reference.top().first);
        reference.pop();
    }
    EXPECT_TRUE(pq.empty());
}

// A more realistic workload: interleaved inserts and add()s.
// We maintain our own ground-truth array of "current gain for each id"
// and verify, when draining the queue, that:
//   (a) extracted gains are non-increasing,
//   (b) the gain reported for an id matches our ground truth.
TEST(BucketPriorityQueue, StressInsertAddAndDrainPreservesInvariants) {
    constexpr int N = 200;
    constexpr int MIN_G = -100, MAX_G = 100;

    std::mt19937 rng(123);
    std::uniform_int_distribution<int> gain_dist(MIN_G / 2, MAX_G / 2);
    std::uniform_int_distribution<int> delta_dist(-5, 5);

    BucketPriorityQueue pq(N, MIN_G, MAX_G);

    constexpr int ABSENT = MIN_G - 1;
    std::vector<int> truth(N, ABSENT);

    auto in_queue = [&](int id) { return truth[id] != ABSENT; };

    // Phase 1: insert the first half.
    for (int id = 0; id < N / 2; ++id) {
        int g = gain_dist(rng);
        pq.insert(g, id);
        truth[id] = g;
    }

    // Phase 2: 1000 random add() operations, staying within the gain range.
    for (int step = 0; step < 1000; ++step) {
        int id = rng() % N;
        if (!in_queue(id)) continue;

        int d = delta_dist(rng);
        if (truth[id] + d < MIN_G || truth[id] + d > MAX_G) continue;

        pq.add(d, id);
        truth[id] += d;
    }

    // Phase 3: drain and verify the two invariants above.
    int prev = MAX_G + 1;
    while (!pq.empty()) {
        auto [gain, id] = pq.extract();

        EXPECT_LE(gain, prev) << "extracted gains must be non-increasing";
        EXPECT_EQ(gain, truth[id]) << "extracted gain must match ground truth";

        prev = gain;
        truth[id] = ABSENT;
    }
}