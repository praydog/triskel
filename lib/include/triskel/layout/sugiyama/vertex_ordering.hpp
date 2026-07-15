#pragma once

#include <cstddef>
#include <random>
#include <utility>
#include <vector>

#include "triskel/graph/graph_view.hpp"
#include "triskel/graph/igraph.hpp"
#include "triskel/utils/attribute.hpp"

namespace triskel {
struct VertexOrdering {
    using NodeView = GraphView::Node;
    using EdgeView = GraphView::Node;

    VertexOrdering(const IGraph& g,
                   const NodeAttribute<size_t>& layers,
                   size_t layer_count_);

    NodeAttribute<size_t> orders_;

   private:
    GraphView g_;

    const NodeAttribute<size_t>& layers_;

    std::vector<std::vector<const NodeView*>> node_layers_;

    std::default_random_engine rng_;

    void get_neighbor_orders(const NodeView& n,
                             std::vector<size_t>& orders_top,
                             std::vector<size_t>& orders_bottom) const;

    [[nodiscard]] auto count_crossings(const NodeView& node1,
                                       const NodeView& node2) const -> size_t;

    // Both orientations of an adjacent pair in one shot. transpose() compares
    // count_crossings(v,w) against count_crossings(w,v); the two share the SAME four
    // neighbor-order vectors (only the merge order differs), so building+sorting them
    // once and running merge_and_count both ways halves the per-pair work -- the
    // dominant transpose cost on large graphs (neighbor-order builds + attribute reads).
    // Returns {crossings(node1,node2), crossings(node2,node1)}.
    [[nodiscard]] auto count_crossings_both(const NodeView& node1,
                                            const NodeView& node2) const
        -> std::pair<size_t, size_t>;

    [[nodiscard]] auto count_crossings_with_layer(size_t l1,
                                                  size_t l2) -> size_t;

    // Save the vectors to avoid reallocating memory every time
    mutable std::vector<size_t> orders_top1;
    mutable std::vector<size_t> orders_top2;
    mutable std::vector<size_t> orders_bottom1;
    mutable std::vector<size_t> orders_bottom2;
    [[nodiscard]] auto count_crossings() -> size_t;

    /// @brief transform the order to the index in the layer
    void normalize_order();

    void median(size_t iter);
    void transpose();
};
}  // namespace triskel