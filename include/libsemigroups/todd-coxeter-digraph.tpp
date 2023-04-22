//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

// This file contains the implementation for the NodeManagedDigraph class, used
// by Stephen and (by ToddCoxeter in the future).

namespace libsemigroups {

  template <typename BaseDigraph>
  struct NodeManagedDigraph<BaseDigraph>::Settings {
    uint64_t large_collapse = 100'000;
  };

  template <typename BaseDigraph>
  struct NodeManagedDigraph<BaseDigraph>::Stats {
    using time_point = std::chrono::high_resolution_clock::time_point;
    uint64_t   prev_active_nodes;
    uint64_t   prev_nodes_killed;
    uint64_t   prev_nodes_defined;
    time_point start_time = std::chrono::high_resolution_clock::now();
  };

  template <typename BaseDigraph>
  void NodeManagedDigraph<BaseDigraph>::reserve(size_t n) {
    size_t m = NodeManager_::node_capacity();
    if (n > m) {
      m = n - m;
      BaseDigraph::add_nodes(m);
      NodeManager_::add_free_nodes(m);
    }
  }

  template <typename BaseDigraph>
  template <bool RegisterDefs>
  std::pair<bool, typename NodeManagedDigraph<BaseDigraph>::node_type>
  NodeManagedDigraph<BaseDigraph>::complete_path(
      node_type                 c,
      word_type::const_iterator first,
      word_type::const_iterator last) noexcept {
    LIBSEMIGROUPS_ASSERT(NodeManager_::is_active_node(c));
    LIBSEMIGROUPS_ASSERT(first <= last);

    word_type::const_iterator it;

    std::tie(c, it)
        = word_graph::last_node_on_path_nc(*this, c, first, last);
    bool result = false;
    for (; it < last; ++it) {
      LIBSEMIGROUPS_ASSERT(unsafe_neighbor(c, *it) == UNDEFINED);
      node_type d = new_node();
      BaseDigraph::template add_edge_nc<RegisterDefs>(c, d, *it);
      result = true;
      c      = d;
    }
    return std::make_pair(result, c);
  }

  template <typename BaseDigraph>
  void NodeManagedDigraph<BaseDigraph>::swap_nodes(node_type c, node_type d) {
    LIBSEMIGROUPS_ASSERT(_coinc.empty());
    LIBSEMIGROUPS_ASSERT(c != NodeManager_::_id_node);
    LIBSEMIGROUPS_ASSERT(d != NodeManager_::_id_node);
    LIBSEMIGROUPS_ASSERT(c != d);
    LIBSEMIGROUPS_ASSERT(NodeManager_::is_valid_node(c));
    LIBSEMIGROUPS_ASSERT(NodeManager_::is_valid_node(d));

    if (NodeManager_::is_active_node(c) && NodeManager_::is_active_node(d)) {
      BaseDigraph::swap_nodes(c, d);
    } else if (NodeManager_::is_active_node(c)) {
      BaseDigraph::rename_node(c, d);
    } else {
      LIBSEMIGROUPS_ASSERT(NodeManager_::is_active_node(d));
      BaseDigraph::rename_node(d, c);
    }
    NodeManager_::switch_nodes(c, d);
  }

  template <typename BaseDigraph>
  void NodeManagedDigraph<BaseDigraph>::stats_check_point() const {
    _stats.prev_active_nodes  = NodeManager_::number_of_nodes_active();
    _stats.prev_nodes_killed  = NodeManager_::number_of_nodes_killed();
    _stats.prev_nodes_defined = NodeManager_::number_of_nodes_defined();
  }

  template <typename BaseDigraph>
  typename NodeManagedDigraph<BaseDigraph>::node_type
  NodeManagedDigraph<BaseDigraph>::new_node() {
    if (NodeManager_::has_free_nodes()) {
      node_type const c = NodeManager_::new_active_node();
      // Clear the new node's row in each table
      BaseDigraph::clear_sources_and_targets(c);
      return c;
    } else {
      reserve(2 * NodeManager_::node_capacity());
      return NodeManager_::new_active_node();
    }
  }

  template <typename BaseDigraph>
  void NodeManagedDigraph<BaseDigraph>::report_active_nodes() const {
    using detail::group_digits;
    using detail::signed_group_digits;
    using std::chrono::duration_cast;
    using high_resolution_clock = std::chrono::high_resolution_clock;
    using nanoseconds           = std::chrono::nanoseconds;

    auto run_time = duration_cast<nanoseconds>(high_resolution_clock::now()
                                               - this->_stats.start_time);

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

    std::array<size_t, 3> col1_widths
        = {12, group_digits(active).size(), active_diff.size()};
    std::array<size_t, 4> col2_widths = {12,
                                         group_digits(killed).size(),
                                         killed_diff.size(),
                                         mean_killed.size()};
    std::array<size_t, 4> col3_widths = {12,
                                         group_digits(defined).size(),
                                         defined_diff.size(),
                                         mean_defined.size()};
    size_t c1 = *std::max_element(col1_widths.begin(), col1_widths.end());
    size_t c2 = *std::max_element(col2_widths.begin(), col2_widths.end());
    size_t c3 = *std::max_element(col3_widths.begin(), col3_widths.end());

    auto msg = fmt_default("{}: nodes {:>{c1}} (active) | {:>{c2}} (killed) | "
                           "{:>{c3}} (defined)\n",
                           _prefix,
                           group_digits(active),
                           group_digits(killed),
                           group_digits(defined),
                           fmt::arg("c1", c1),
                           fmt::arg("c2", c2),
                           fmt::arg("c3", c3));
    msg += fmt_default("{}: diff  {:>{c1}} (active) | {:>{c2}} (killed) | "
                       "{:>{c3}} (defined)\n",
                       _prefix,
                       active_diff,
                       killed_diff,
                       defined_diff,
                       fmt::arg("c1", c1),
                       fmt::arg("c2", c2),
                       fmt::arg("c3", c3));
    msg += fmt_default(
        "{}: time  {:>{c1}} (total)  | {:>{c2}} (killed) | {:>{c3}} "
        "(defined)\n",
        _prefix,
        string_time(run_time),
        mean_killed,
        mean_defined,
        fmt::arg("c1", c1),
        fmt::arg("c2", c2),
        fmt::arg("c3", c3));
    msg += fmt::format("{:-<93}\n", "");
    report_no_prefix(msg);
    stats_check_point();
  }

  template <typename BaseDigraph>
  template <bool RegisterDefs>
  void NodeManagedDigraph<BaseDigraph>::process_coincidences() {
    if (_coinc.empty()) {
      return;
    }
    CollectCoincidences incompat_func(_coinc);

    // TODO re-enable
    // size_t        prev_num_nodes = this->number_of_nodes_active();
    static size_t total_coinc   = 0;
    bool const    should_report = report::should_report();
    size_t        report_tick   = 0;
    while (!_coinc.empty() && _coinc.size() < large_collapse()) {
      total_coinc++;
      Coincidence c = _coinc.top();
      _coinc.pop();
      node_type min = NodeManager_::find_node(c.first);
      node_type max = NodeManager_::find_node(c.second);
      if (min != max) {
        report_tick++;
        std::tie(min, max) = std::minmax({min, max});
        NodeManager_::union_nodes(min, max);
        if constexpr (RegisterDefs) {
          BaseDigraph::merge_nodes(
              min,
              max,
              [this](node_type n, letter_type x) {
                this->definitions().emplace_back(n, x);
              },
              incompat_func);
        } else {
          BaseDigraph::merge_nodes(min, max, Noop(), incompat_func);
        }
        // TODO checking report here can be rather time consuming
        if (should_report && report_tick > 10'000 && report()) {
          report_tick = 0;
          report_active_nodes();
        }
      }
      // if (_coinc.size() > large_collapse()) {
      //   size_t num_nodes = this->number_of_nodes_active();
      //   size_t cost_pairwise
      //       = 2 * out_degree() * out_degree() * (prev_num_nodes -
      //       num_nodes);
      //   size_t cost_bigcrush = 2 * num_nodes * out_degree() + num_nodes;
      //   if (cost_bigcrush < cost_pairwise) {
      //     report_default("ToddCoxeter: large collapse {} -> {} nodes\n",
      //                    fmt::group_digits(prev_num_nodes),
      //                    fmt::group_digits(num_nodes));
      //     report_default("ToddCoxeter: {} (coincidences)\n",
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
      node_type min = NodeManager_::find_node(c.first);
      node_type max = NodeManager_::find_node(c.second);
      if (min != max) {
        report_tick++;
        std::tie(min, max) = std::minmax({min, max});
        NodeManager_::union_nodes(min, max);
        for (letter_type i = 0; i < out_degree(); ++i) {
          node_type const v = unsafe_neighbor(max, i);
          if (v != UNDEFINED) {
            node_type const u = unsafe_neighbor(min, i);
            if (u == UNDEFINED) {
              WordGraph<node_type>::add_edge_nc(min, v, i);
            } else if (u != v) {
              _coinc.emplace(u, v);
            }
          }
        }
        // TODO checking report here can be rather time consuming
        if (should_report && report_tick > 10'000 && report()) {
          report_tick = 0;
          report_active_nodes();
        }
      }
    }

    // Remove all sources of all remaining active cosets
    auto c = NodeManager_::_id_node;
    while (c != NodeManager_::first_free_node()) {
      BaseDigraph::clear_sources(c);
      c = NodeManager_::next_active_node(c);
    }

    // TODO use rebuild_sources, when I've implemented NodeManager_::cbegin(),
    // NodeManager_::cend()
    c        = NodeManager_::_id_node;
    size_t m = 0;

    while (c != NodeManager_::first_free_node()) {
      m++;
      for (letter_type x = 0; x < out_degree(); ++x) {
        auto cx = unsafe_neighbor(c, x);
        if (cx != UNDEFINED) {
          auto d = NodeManager_::find_node(cx);
          if (cx != d) {
            if constexpr (RegisterDefs) {
              this->definitions().emplace_back(c, x);
            }
            WordGraph<node_type>::add_edge_nc(c, d, x);
          }
          // Must re-add the source, even if we don't need to reset
          // the target or stack the deduction
          BaseDigraph::add_source(d, x, c);
          LIBSEMIGROUPS_ASSERT(NodeManager_::is_active_node(d));
        }
      }
      c = NodeManager_::next_active_node(c);
    }
    // fmt::print("Position 2, total coincidences is {}\n", total_coinc);
  }

}  // namespace libsemigroups
