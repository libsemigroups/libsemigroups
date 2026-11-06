//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This file contains the implementation for the NodeManagedGraph class, used
// by Stephen and by ToddCoxeterImpl.

#include "libsemigroups/detail/node-managed-graph.hpp"
namespace libsemigroups::detail {

  ////////////////////////////////////////////////////////////////////////
  // Nested classes
  ////////////////////////////////////////////////////////////////////////

  template <typename Node>
  struct NodeManagedGraph<Node>::CollectCoincidences {
    explicit CollectCoincidences(Coincidences& c) : _coinc(c) {}

    bool operator()(node_type x, node_type y) {
      _coinc.emplace(x, y);
      return true;
    }

    Coincidences& _coinc;
  };

  template <typename Node>
  struct NodeManagedGraph<Node>::Settings {
    uint64_t large_collapse = 100'000;
  };

  template <typename Node>
  struct NodeManagedGraph<Node>::Stats {
    std::atomic_uint64_t num_edges_active;
    uint64_t             num_large_collapses;
    std::atomic_uint64_t prev_active_nodes;
    std::atomic_uint64_t prev_nodes_defined;
    std::atomic_uint64_t prev_nodes_killed;

    Stats()
        : num_edges_active(0),
          num_large_collapses(0),
          prev_active_nodes(),
          prev_nodes_defined(),
          prev_nodes_killed() {}

    Stats(Stats const& that)
        : num_edges_active(that.num_edges_active.load()),
          num_large_collapses(that.num_large_collapses),
          prev_active_nodes(that.prev_active_nodes.load()),
          prev_nodes_defined(that.prev_nodes_defined.load()),
          prev_nodes_killed(that.prev_nodes_killed.load()) {}

    Stats& operator=(Stats const& that) {
      num_edges_active    = that.num_edges_active.load();
      num_large_collapses = that.num_large_collapses;
      prev_active_nodes   = that.prev_active_nodes.load();
      prev_nodes_killed   = that.prev_nodes_killed.load();
      prev_nodes_defined  = that.prev_nodes_defined.load();
      return *this;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Constructors + initializers
  ////////////////////////////////////////////////////////////////////////

  template <typename Node>
  NodeManagedGraph<Node>& NodeManagedGraph<Node>::init() {
    WordGraphWithSources<Node>::init();
    NodeManager<Node>::init();
    // NodeManagedGraph's always have 1 active node.
    WordGraphWithSources<Node>::add_nodes(1);
    LIBSEMIGROUPS_ASSERT(WordGraphWithSources<Node>::number_of_nodes() == 1);
    LIBSEMIGROUPS_ASSERT(WordGraphWithSources<Node>::number_of_nodes()
                         == NodeManager<Node>::number_of_nodes_active());
    _coinc    = decltype(_coinc)();
    _settings = Settings();
    _stats    = Stats();
    return *this;
  }

  template <typename Node>
  NodeManagedGraph<Node>& NodeManagedGraph<Node>::reserve(size_t n) {
    size_t m = NodeManager<node_type>::node_capacity();
    if (n > m) {
      m = n - m;
      WordGraphWithSources<Node>::add_nodes(m);
      NodeManager<node_type>::add_free_nodes(m);
    }
    return *this;
  }

  template <typename Node>
  NodeManagedGraph<Node>::NodeManagedGraph() {
    init();
  }

  template <typename Node>
  NodeManagedGraph<Node>::NodeManagedGraph(NodeManagedGraph const&) = default;

  template <typename Node>
  NodeManagedGraph<Node>::NodeManagedGraph(NodeManagedGraph&&) = default;

  template <typename Node>
  NodeManagedGraph<Node>&
  NodeManagedGraph<Node>::operator=(NodeManagedGraph const&)
      = default;

  template <typename Node>
  NodeManagedGraph<Node>& NodeManagedGraph<Node>::operator=(NodeManagedGraph&&)
      = default;

  template <typename Node>
  NodeManagedGraph<Node>::~NodeManagedGraph() = default;

  template <typename Node>
  NodeManagedGraph<Node>&
  NodeManagedGraph<Node>::init(WordGraph<Node> const& wg) {
    init();
    WordGraphWithSources<Node>::init(wg);
    LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() > 0);
    // NodeManager always has one node active
    NodeManager<node_type>::add_active_nodes(
        WordGraph<node_type>::number_of_nodes() - 1);
    _stats.num_edges_active = wg.number_of_edges();
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Other mem fns - alphabetical order !!!
  ////////////////////////////////////////////////////////////////////////

  template <typename Node>
  template <typename Register>
  typename NodeManagedGraph<Node>::node_type
  NodeManagedGraph<Node>::complete_path(node_type                 s,
                                        word_type::const_iterator first,
                                        word_type::const_iterator last,
                                        Register const& reg) noexcept {
    LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_active_node(s));
    LIBSEMIGROUPS_ASSERT(first <= last);

    word_type::const_iterator it;

    std::tie(s, it)
        = word_graph::last_node_on_path_no_checks(*this, s, first, last);

    for (; it < last; ++it) {
      LIBSEMIGROUPS_ASSERT(target_no_checks(s, *it) == UNDEFINED);
      node_type d = new_node();
      target_no_checks(s, *it, d);
      reg(s, *it);
      s = d;
    }
    return s;
  }

  template <typename Node>
  uint64_t
  NodeManagedGraph<Node>::count_number_of_edges_active() const noexcept {
    auto     current   = NodeManager<node_type>::initial_node();
    uint64_t num_edges = 0;
    while (current != NodeManager<node_type>::first_free_node()) {
      num_edges += WordGraph<node_type>::number_of_edges_no_checks(current);
      current = NodeManager<node_type>::next_active_node(current);
    }
    return num_edges;
  }

  template <typename Node>
  typename NodeManagedGraph<Node>::node_type
  NodeManagedGraph<Node>::new_node() {
    if (NodeManager<node_type>::has_free_nodes()) {
      node_type const c = NodeManager<node_type>::new_active_node();
      // Clear the new node's row in each table
      WordGraphWithSources<Node>::remove_all_sources_and_targets_no_checks(c);
      return c;
    } else {
      // There are no free nodes to recycle: make new ones.
      // It seems to be marginally faster to make lots like this, than to
      // just make 1, in some examples, notably ToddCoxeterImpl 040
      // (Walker 3).
      reserve(2 * NodeManager<node_type>::node_capacity());
      return NodeManager<node_type>::new_active_node();
    }
  }

  template <typename Node>
  template <typename Register>
  bool NodeManagedGraph<Node>::process_coincidences(Register const& reg) {
    bool any_collapse_before_large_collapse = false;
    if (_coinc.empty()) {
      return any_collapse_before_large_collapse;
    }

    CollectCoincidences incompat_func(_coinc);

    while (!_coinc.empty() && _coinc.size() < large_collapse()) {
      Coincidence c = _coinc.top();
      _coinc.pop();
      node_type min = NodeManager<node_type>::find_node(c.first);
      node_type max = NodeManager<node_type>::find_node(c.second);
      if (min != max) {
        std::tie(min, max)                 = std::minmax({min, max});
        any_collapse_before_large_collapse = true;
        NodeManager<node_type>::union_nodes(min, max);
        _stats.num_edges_active
            -= WordGraphWithSources<Node>::merge_nodes_no_checks(
                min, max, reg, incompat_func);
      }
    }

    if (_coinc.empty()) {
      // The next assert is likely very slow so don't do it routinely
      // LIBSEMIGROUPS_ASSERT(_stats.num_edges_active
      //                      == count_number_of_edges_active());
      return any_collapse_before_large_collapse;
    }

    using detail::group_digits;
    _stats.num_large_collapses++;

    report_default("{}: large collapse, number of coincidences {} >= {} = "
                   "large_collapse()!\n",
                   report_prefix(),
                   group_digits(_coinc.size()),
                   group_digits(large_collapse()));
    auto const old_number_of_edges_active = number_of_edges_active();
    // Setting this here means that during a large collapse the % of edges
    // active falls to zero until we start rebuilding the sources in the while
    // loop below, this currently seems unavoidable.
    _stats.num_edges_active                = 0;
    bool any_collapse_after_large_collapse = false;

    while (!_coinc.empty()) {
      Coincidence c = _coinc.top();
      _coinc.pop();
      node_type min = NodeManager<node_type>::find_node(c.first);
      node_type max = NodeManager<node_type>::find_node(c.second);
      if (min != max) {
        std::tie(min, max)                = std::minmax({min, max});
        any_collapse_after_large_collapse = true;
        NodeManager<node_type>::union_nodes(min, max);
        for (letter_type i = 0; i < out_degree(); ++i) {
          node_type const v = target_no_checks(max, i);
          if (v != UNDEFINED) {
            node_type const u = target_no_checks(min, i);
            if (u == UNDEFINED) {
              reg(min, i);
              WordGraph<node_type>::target_no_checks(min, i, v);
            } else if (u != v) {
              _coinc.emplace(u, v);
            }
          }
        }
      }
    }

    if (any_collapse_after_large_collapse) {
      // Rebuild the sources only required if any changes were made in the
      // loop above.
      auto c = NodeManager<node_type>::initial_node();
      while (c != NodeManager<node_type>::first_free_node()) {
        WordGraphWithSources<Node>::remove_all_sources_no_checks(c);
        c = NodeManager<node_type>::next_active_node(c);
      }

      c = NodeManager<node_type>::initial_node();

      while (c != NodeManager<node_type>::first_free_node()) {
        for (letter_type x = 0; x < out_degree(); ++x) {
          auto cx = target_no_checks(c, x);
          if (cx != UNDEFINED) {
            _stats.num_edges_active++;
            auto d = NodeManager<node_type>::find_node(cx);
            if (cx != d) {
              reg(c, x);
              WordGraph<node_type>::target_no_checks(c, x, d);
            }
            // Must re-add the source, even if we don't need to reset
            // the target or stack the deduction
            WordGraphWithSources<Node>::add_source_no_checks(d, x, c);
            LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_active_node(d));
          }
        }
        c = NodeManager<node_type>::next_active_node(c);
      }
    } else {
      _stats.num_edges_active = old_number_of_edges_active;
    }
    // The next assertion is likely very slow so commented out
    // LIBSEMIGROUPS_ASSERT(_stats.num_edges_active
    //                      == count_number_of_edges_active());
    return any_collapse_before_large_collapse
           || any_collapse_after_large_collapse;
  }

  template <typename Node>
  void
  NodeManagedGraph<Node>::standardize_no_checks(std::vector<node_type> const& p,
                                                std::vector<node_type>& q) {
    auto& c = lookahead_cursor();
    if (c < q.size()) {
      c = q[c];
    }
    WordGraphWithSources<Node>::permute_nodes_no_checks(
        p, q, NodeManager<node_type>::number_of_nodes_active());
    NodeManager<node_type>::compact();
  }

  template <typename Node>
  void NodeManagedGraph<Node>::stats_check_point() const {
    _stats.prev_active_nodes = NodeManager<node_type>::number_of_nodes_active();
    _stats.prev_nodes_killed = NodeManager<node_type>::number_of_nodes_killed();
    _stats.prev_nodes_defined
        = NodeManager<node_type>::number_of_nodes_defined();
  }

  template <typename Node>
  NodeManagedGraph<Node>&
  NodeManagedGraph<Node>::target_no_checks(node_type  s,
                                           label_type a,
                                           node_type  t) noexcept {
    LIBSEMIGROUPS_ASSERT(t != UNDEFINED);
    _stats.num_edges_active++;
    WordGraphWithSources<Node>::target_no_checks(s, a, t);
    // The next assertion is extremely slow so don't do it!
    // LIBSEMIGROUPS_ASSERT(_stats.num_edges_active
    //                      == count_number_of_edges_active());
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Helpers
  ////////////////////////////////////////////////////////////////////////

  namespace node_managed_graph {
    template <typename Node>
    Node random_active_node(NodeManagedGraph<Node> const& nmg) {
      static std::random_device rd;
      static std::mt19937       g(rd());

      std::uniform_int_distribution<uint64_t> d(
          0, nmg.number_of_nodes_active() - 1);
      auto r = nmg.active_nodes();
      rx::advance_by(r, d(g));
      return r.get();
    }
  }  // namespace node_managed_graph
}  // namespace libsemigroups::detail
