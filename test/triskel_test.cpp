
#include <triskel/triskel.hpp>
#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

// NOLINTNEXTLINE(google-build-using-namespace)
using namespace triskel;

TEST(Triskel, Smoke1) {
    auto builder = make_layout_builder();

    const auto a = builder->make_node(100, 100);
    const auto b = builder->make_node(100, 100);
    const auto c = builder->make_node(100, 100);
    const auto d = builder->make_node(100, 100);
    const auto e = builder->make_node(100, 100);
    const auto f = builder->make_node(100, 100);
    const auto g = builder->make_node(100, 100);

    builder->make_edge(a, b);
    builder->make_edge(a, e);
    builder->make_edge(b, c);
    builder->make_edge(b, d);
    builder->make_edge(e, f);
    builder->make_edge(f, e);
    builder->make_edge(c, g);
    builder->make_edge(d, g);
    builder->make_edge(e, g);
    builder->make_edge(g, a);

    ASSERT_NO_THROW(const auto layout = builder->build());
}

// A deep, densely back-connected graph: a long chain (forcing many layers) plus a
// full-height back edge from every node to the root. Each back edge spans most of the
// layer range, so long-edge splitting would insert a ghost node per spanned layer per
// edge -- O(nodes^2) ghosts -- which made the Sugiyama layout effectively never finish.
// The ghost-count preflight in remove_long_edges skips splitting past a size-relative
// budget, so this must build in bounded time rather than hang.
TEST(Triskel, DeepBackEdgesBounded) {
    auto builder = make_layout_builder();

    constexpr size_t N = 400;
    std::vector<size_t> ns;
    ns.reserve(N);
    for (size_t i = 0; i < N; ++i) ns.push_back(builder->make_node(100, 100));

    // A long forward chain (forces ~N layers) ...
    for (size_t i = 0; i + 1 < N; ++i) builder->make_edge(ns[i], ns[i + 1]);
    // ... plus a full-height back edge from every node to the root: each spans many
    // layers, so unbounded splitting would create ~N^2/2 ghost nodes.
    for (size_t i = 2; i < N; ++i) builder->make_edge(ns[i], ns[0]);

    ASSERT_NO_THROW(const auto layout = builder->build());
}
