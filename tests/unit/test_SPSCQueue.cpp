#include <doctest/doctest.h>

#include "utils/SPSCQueue.h"

TEST_CASE("SPSCQueue single thread basic functionality") {
    size_t n = 10;
    SPSCQueue<size_t> q(n);

    CHECK(q.empty());

    for (size_t i = 0; i < n; ++i) {
        CHECK(q.push(i));
    }

    CHECK(q.full());

    CHECK(!q.push(n));

    size_t num;
    for (size_t i = 0; i < n/2; ++i) {
        CHECK(q.pop(num));
        CHECK(num == i);
    }
    CHECK(!q.empty());
    q.clear();
    CHECK(q.empty());
    CHECK(!q.pop(num));
    q.clear();
    CHECK(q.empty());
    CHECK(!q.full());
}

TEST_CASE("SPSCQueue single thread ring buffer functionality") {
    size_t n = 10;
    SPSCQueue<size_t> q(n);
    size_t i = 0;
    size_t j = 0;
    for (; i < 5; ++i) {
        q.push(i);
    }
    size_t num;
    for (; j < 3; ++j) {
        CHECK(q.pop(num));
        CHECK(num == j);
    }
    for (; i < 13; ++i) {
        q.push(i);
    }
    for (; j < 10; ++j) {
        CHECK(q.pop(num));
        CHECK(num == j);
    }
    for (; i < 20; ++i) {
        q.push(i);
    }
    for (; j < 20; ++j) {
        CHECK(q.pop(num));
        CHECK(num == j);
    }
}
