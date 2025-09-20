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

namespace libsemigroups {

  namespace detail {
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
      // FIXME this doesn't seem like enough is reset
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
        BaseGraph::add_nodes(m);
        NodeManager<node_type>::add_free_nodes(m);
      }
      return *this;
    }

    template <typename Node>
    NodeManagedGraph<Node>::NodeManagedGraph() = default;

    template <typename Node>
    NodeManagedGraph<Node>::NodeManagedGraph(NodeManagedGraph const&) = default;

    template <typename Node>
    NodeManagedGraph<Node>::NodeManagedGraph(NodeManagedGraph&&) = default;

    template <typename Node>
    NodeManagedGraph<Node>&
    NodeManagedGraph<Node>::operator=(NodeManagedGraph const&)
        = default;

    template <typename Node>
    NodeManagedGraph<Node>&
    NodeManagedGraph<Node>::operator=(NodeManagedGraph&&)
        = default;

    template <typename Node>
    NodeManagedGraph<Node>::~NodeManagedGraph() = default;

    template <typename Node>
    template <typename OtherNode>
    NodeManagedGraph<Node>&
    NodeManagedGraph<Node>::operator=(WordGraph<OtherNode> const& wg) {
      init();
      BaseGraph::operator=(wg);
      NodeManager<node_type>::add_active_nodes(
          WordGraph<node_type>::number_of_nodes() - 1);
      LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::number_of_nodes_active()
                           == BaseGraph::number_of_nodes());
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // Stats
    ////////////////////////////////////////////////////////////////////////

    template <typename Node>
    void NodeManagedGraph<Node>::stats_check_point() const {
      _stats.prev_active_nodes
          = NodeManager<node_type>::number_of_nodes_active();
      _stats.prev_nodes_killed
          = NodeManager<node_type>::number_of_nodes_killed();
      _stats.prev_nodes_defined
          = NodeManager<node_type>::number_of_nodes_defined();
    }

    ////////////////////////////////////////////////////////////////////////
    // Accessors
    ////////////////////////////////////////////////////////////////////////

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

    ////////////////////////////////////////////////////////////////////////
    // Modifiers
    ////////////////////////////////////////////////////////////////////////

    template <typename Node>
    typename NodeManagedGraph<Node>::node_type
    NodeManagedGraph<Node>::new_node() {
      if (NodeManager<node_type>::has_free_nodes()) {
        node_type const c = NodeManager<node_type>::new_active_node();
        // Clear the new node's row in each table
        BaseGraph::remove_all_sources_and_targets_no_checks(c);
        return c;
      } else {
        reserve(2 * NodeManager<node_type>::node_capacity());
        return NodeManager<node_type>::new_active_node();
      }
    }

    template <typename Node>
    template <bool RegisterDefs>
    std::pair<bool, typename NodeManagedGraph<Node>::node_type>
    NodeManagedGraph<Node>::complete_path(
        node_type                 c,
        word_type::const_iterator first,
        word_type::const_iterator last) noexcept {
      LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_active_node(c));
      LIBSEMIGROUPS_ASSERT(first <= last);

      word_type::const_iterator it;

      std::tie(c, it)
          = word_graph::last_node_on_path_no_checks(*this, c, first, last);

      bool result = false;
      for (; it < last; ++it) {
        LIBSEMIGROUPS_ASSERT(target_no_checks(c, *it) == UNDEFINED);
        node_type d = new_node();
        target_no_checks(c, *it, d);
        result = true;
        c      = d;
      }
      return std::make_pair(result, c);
    }

    template <typename Node>
    template <typename Functor>
    void NodeManagedGraph<Node>::process_coincidences(Functor&& new_def) {
      if (_coinc.empty()) {
        return;
      }

      CollectCoincidences incompat_func(_coinc);

      while (!_coinc.empty() && _coinc.size() < large_collapse()) {
        Coincidence c = _coinc.top();
        _coinc.pop();
        node_type min = NodeManager<node_type>::find_node(c.first);
        node_type max = NodeManager<node_type>::find_node(c.second);
        if (min != max) {
          std::tie(min, max) = std::minmax({min, max});
          NodeManager<node_type>::union_nodes(min, max);
          _stats.num_edges_active -= BaseGraph::merge_nodes_no_checks(
              min, max, new_def, incompat_func);
        }
      }

      if (_coinc.empty()) {
        // The next assert is likely very slow so don't do it routinely
        // LIBSEMIGROUPS_ASSERT(_stats.num_edges_active
        //                      == count_number_of_edges_active());
        return;
      }

      using detail::group_digits;
      _stats.num_large_collapses++;

      report_default("{}: large collapse, number of coincidences {} >= {} = "
                     "large_collapse()!\n",
                     report_prefix(),
                     group_digits(_coinc.size()),
                     group_digits(large_collapse()));
      // Setting this here means that during a large collapse the % of edges
      // active falls to zero until we start rebuilding the sources in the while
      // loop below, this currently seems unavoidable.
      _stats.num_edges_active = 0;

      while (!_coinc.empty()) {
        Coincidence c = _coinc.top();
        _coinc.pop();
        node_type min = NodeManager<node_type>::find_node(c.first);
        node_type max = NodeManager<node_type>::find_node(c.second);
        if (min != max) {
          std::tie(min, max) = std::minmax({min, max});
          NodeManager<node_type>::union_nodes(min, max);
          for (letter_type i = 0; i < out_degree(); ++i) {
            node_type const v = target_no_checks(max, i);
            if (v != UNDEFINED) {
              node_type const u = target_no_checks(min, i);
              if (u == UNDEFINED) {
                WordGraph<node_type>::target_no_checks(min, i, v);
              } else if (u != v) {
                _coinc.emplace(u, v);
              }
            }
          }
        }
      }

      // TODO(later) use rebuild_sources
      auto c = NodeManager<node_type>::initial_node();
      while (c != NodeManager<node_type>::first_free_node()) {
        BaseGraph::remove_all_sources_no_checks(c);
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
              new_def(c, x);
              WordGraph<node_type>::target_no_checks(c, x, d);
            }
            // Must re-add the source, even if we don't need to reset
            // the target or stack the deduction
            BaseGraph::add_source_no_checks(d, x, c);
            LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_active_node(d));
          }
        }
        c = NodeManager<node_type>::next_active_node(c);
      }
      // The next assertion is likely very slow so commented out
      // LIBSEMIGROUPS_ASSERT(_stats.num_edges_active
      //                      == count_number_of_edges_active());
    }

    template <typename Node>
    void NodeManagedGraph<Node>::swap_nodes_no_checks(node_type c,
                                                      node_type d) {
      LIBSEMIGROUPS_ASSERT(_coinc.empty());
      LIBSEMIGROUPS_ASSERT(c != NodeManager<node_type>::initial_node());
      LIBSEMIGROUPS_ASSERT(d != NodeManager<node_type>::initial_node());
      LIBSEMIGROUPS_ASSERT(c != d);
      LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_valid_node(c));
      LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_valid_node(d));

      if (NodeManager<node_type>::is_active_node(c)
          && NodeManager<node_type>::is_active_node(d)) {
        BaseGraph::swap_nodes_no_checks(c, d);
      } else if (NodeManager<node_type>::is_active_node(c)) {
        BaseGraph::rename_node_no_checks(c, d);
      } else {
        LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_active_node(d));
        BaseGraph::rename_node_no_checks(d, c);
      }
      NodeManager<node_type>::switch_nodes(c, d);
    }

    ////////////////////////////////////////////////////////////////////////
    // Helpers
    ////////////////////////////////////////////////////////////////////////

    namespace node_managed_graph {
      template <typename Node>
      Node random_active_node(NodeManagedGraph<Node> const& nmg) {
        static std::random_device rd;
        static std::mt19937       g(rd());

        std::uniform_int_distribution<> d(0, nmg.number_of_nodes_active() - 1);
        auto                            r = nmg.active_nodes();
        rx::advance_by(r, d(g));
        return r.get();
      }
    }  // namespace node_managed_graph
  }    // namespace detail
}  // namespace libsemigroups
