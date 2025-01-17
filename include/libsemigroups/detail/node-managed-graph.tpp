//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2023 James D. Mitchell
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
// by Stephen and by ToddCoxeterBase.

namespace libsemigroups {

  namespace detail {
    ////////////////////////////////////////////////////////////////////////
    // Nested classes
    ////////////////////////////////////////////////////////////////////////

    template <typename BaseGraph>
    struct NodeManagedGraph<BaseGraph>::CollectCoincidences {
      explicit CollectCoincidences(Coincidences& c) : _coinc(c) {}

      bool operator()(node_type x, node_type y) {
        _coinc.emplace(x, y);
        return true;
      }

      Coincidences& _coinc;
    };

    template <typename BaseGraph>
    struct NodeManagedGraph<BaseGraph>::Settings {
      uint64_t large_collapse = 100'000;
    };

    template <typename BaseGraph>
    struct NodeManagedGraph<BaseGraph>::Stats {
      uint64_t prev_active_nodes;
      uint64_t prev_nodes_killed;
      uint64_t prev_nodes_defined;
    };

    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    template <typename BaseGraph>
    NodeManagedGraph<BaseGraph>& NodeManagedGraph<BaseGraph>::init() {
      _coinc    = decltype(_coinc)();
      _settings = Settings();
      _stats    = Stats();
      return *this;
    }

    template <typename BaseGraph>
    NodeManagedGraph<BaseGraph>&
    NodeManagedGraph<BaseGraph>::reserve(size_t n) {
      size_t m = NodeManager<node_type>::node_capacity();
      if (n > m) {
        m = n - m;
        BaseGraph::add_nodes(m);
        NodeManager<node_type>::add_free_nodes(m);
      }
      return *this;
    }

    template <typename BaseGraph>
    NodeManagedGraph<BaseGraph>::NodeManagedGraph(NodeManagedGraph const&)
        = default;

    template <typename BaseGraph>
    NodeManagedGraph<BaseGraph>::NodeManagedGraph(NodeManagedGraph&&) = default;

    template <typename BaseGraph>
    NodeManagedGraph<BaseGraph>&
    NodeManagedGraph<BaseGraph>::operator=(NodeManagedGraph const&)
        = default;

    template <typename BaseGraph>
    NodeManagedGraph<BaseGraph>&
    NodeManagedGraph<BaseGraph>::operator=(NodeManagedGraph&&)
        = default;

    template <typename BaseGraph>
    NodeManagedGraph<BaseGraph>::~NodeManagedGraph() = default;

    ////////////////////////////////////////////////////////////////////////
    // Stats
    ////////////////////////////////////////////////////////////////////////

    template <typename BaseGraph>
    void NodeManagedGraph<BaseGraph>::stats_check_point() const {
      _stats.prev_active_nodes
          = NodeManager<node_type>::number_of_nodes_active();
      _stats.prev_nodes_killed
          = NodeManager<node_type>::number_of_nodes_killed();
      _stats.prev_nodes_defined
          = NodeManager<node_type>::number_of_nodes_defined();
    }

    ////////////////////////////////////////////////////////////////////////
    // Modifiers
    ////////////////////////////////////////////////////////////////////////

    template <typename BaseGraph>
    typename NodeManagedGraph<BaseGraph>::node_type
    NodeManagedGraph<BaseGraph>::new_node() {
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

    template <typename BaseGraph>
    template <bool RegisterDefs>
    std::pair<bool, typename NodeManagedGraph<BaseGraph>::node_type>
    NodeManagedGraph<BaseGraph>::complete_path(
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
        BaseGraph::template target_no_checks<RegisterDefs>(c, *it, d);
        result = true;
        c      = d;
      }
      return std::make_pair(result, c);
    }

    template <typename BaseGraph>
    template <bool RegisterDefs>
    void NodeManagedGraph<BaseGraph>::process_coincidences() {
      if (_coinc.empty()) {
        return;
      }
      CollectCoincidences incompat_func(_coinc);

      // TODO re-enable
      // size_t        prev_num_nodes = this->number_of_nodes_active();
      // static size_t total_coinc   = 0;
      while (!_coinc.empty() && _coinc.size() < large_collapse()) {
        // total_coinc++;
        Coincidence c = _coinc.top();
        _coinc.pop();
        node_type min = NodeManager<node_type>::find_node(c.first);
        node_type max = NodeManager<node_type>::find_node(c.second);
        if (min != max) {
          std::tie(min, max) = std::minmax({min, max});
          NodeManager<node_type>::union_nodes(min, max);
          if constexpr (RegisterDefs) {
            BaseGraph::merge_nodes_no_checks(
                min,
                max,
                [this](node_type n, letter_type x) {
                  this->definitions().emplace_back(n, x);
                },
                incompat_func);
          } else {
            BaseGraph::merge_nodes_no_checks(min, max, Noop(), incompat_func);
          }
        }
        // if (_coinc.size() > large_collapse()) {
        //   size_t num_nodes = this->number_of_nodes_active();
        //   size_t cost_pairwise
        //       = 2 * out_degree() * out_degree() * (prev_num_nodes -
        //       num_nodes);
        //   size_t cost_bigcrush = 2 * num_nodes * out_degree() + num_nodes;
        //   if (cost_bigcrush < cost_pairwise) {
        //     report_default("ToddCoxeterBase: large collapse {} -> {}
        //     nodes\n",
        //                    fmt::group_digits(prev_num_nodes),
        //                    fmt::group_digits(num_nodes));
        //     report_default("ToddCoxeterBase: {} (coincidences)\n",
        //                    fmt::group_digits(_coinc.size()));
        //     break;
        //   }
        // }
      }

      if (_coinc.empty()) {
        // fmt::print("Position 1, total coincidences is {}\n", total_coinc);
        return;
      }

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
      // size_t m = 0;

      while (c != NodeManager<node_type>::first_free_node()) {
        // m++;
        for (letter_type x = 0; x < out_degree(); ++x) {
          auto cx = target_no_checks(c, x);
          if (cx != UNDEFINED) {
            auto d = NodeManager<node_type>::find_node(cx);
            if (cx != d) {
              if constexpr (RegisterDefs) {
                this->definitions().emplace_back(c, x);
              }
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
      // fmt::print("Position 2, total coincidences is {}\n", total_coinc);
    }

    template <typename BaseGraph>
    void NodeManagedGraph<BaseGraph>::swap_nodes_no_checks(node_type c,
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
    // Reporting - public
    ////////////////////////////////////////////////////////////////////////

    template <typename BaseGraph>
    void NodeManagedGraph<BaseGraph>::report_progress_from_thread() const {
      using detail::group_digits;
      using detail::signed_group_digits;

      auto run_time = delta(start_time());

      auto const active  = this->number_of_nodes_active();
      auto const killed  = this->number_of_nodes_killed();
      auto const defined = this->number_of_nodes_defined();

      auto const active_diff
          = signed_group_digits(active - _stats.prev_active_nodes);
      auto const killed_diff
          = signed_group_digits(killed - _stats.prev_nodes_killed);
      auto const defined_diff
          = signed_group_digits(defined - _stats.prev_nodes_defined);

      auto const mean_killed
          = group_digits(std::pow(10, 9) * static_cast<double>(killed)
                         / run_time.count())
            + "/s";
      auto const mean_defined
          = group_digits(std::pow(10, 9) * static_cast<double>(defined)
                         / run_time.count())
            + "/s";

      std::string_view const line1
          = "{}: nodes {} (active) | {} (killed) | {} (defined)\n";

      detail::ReportCell<4> rc;
      rc.min_width(11).min_width(0, report_prefix().size());

      rc(line1,
         report_prefix(),
         group_digits(active),
         group_digits(killed),
         group_digits(defined));
      rc("{}: diff  {} (active) | {} (killed) | {} (defined)\n",
         report_prefix(),
         active_diff,
         killed_diff,
         defined_diff);
      rc("{}: time  {} (total)  | {} (killed) | {} (defined)\n",
         report_prefix(),
         string_time(run_time),
         mean_killed,
         mean_defined);
      stats_check_point();
    }

    ////////////////////////////////////////////////////////////////////////
    // Helpers
    ////////////////////////////////////////////////////////////////////////

    namespace node_managed_graph {
      template <typename BaseGraph>
      typename BaseGraph::node_type
      random_active_node(NodeManagedGraph<BaseGraph> const& nmg) {
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
