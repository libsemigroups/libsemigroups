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

// This file contains the declaration of the NodeManagedDigraph class, used
// by Stephen and (by ToddCoxeter in the future).

#ifndef LIBSEMIGROUPS_TODD_COXETER_DIGRAPH_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_DIGRAPH_HPP_

#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t
#include <vector>   // for vector

#include <cstddef>      // for size_t
#include <cstdint>      // for uint32_t
#include <stack>        // for stack
#include <type_traits>  // for is_base_of
#include <utility>      // for pair
#include <vector>       // for vector

#include "digraph-with-sources.hpp"  // for DigraphWithSources
#include "digraph.hpp"               // for ActionDigraph
#include "felsch-digraph.hpp"
#include "node-manager.hpp"  // for NodeManager
#include "present.hpp"       // for Presentation, Presentation<>:...
#include "report.hpp"        // for REPORT_DEFAULT
#include "types.hpp"         // for word_type

namespace libsemigroups {

  template <typename BaseDigraph>
  class NodeManagedDigraph
      : public BaseDigraph,
        public detail::NodeManager<typename BaseDigraph::node_type> {
   public:
    using node_type         = typename BaseDigraph::node_type;
    using base_digraph_type = BaseDigraph;

    static_assert(
        std::is_base_of<DigraphWithSources<node_type>, BaseDigraph>::value,
        "the template parameter BaseDigraph must be derived from "
        "DigraphWithSources<node_type>");

   private:
    using NodeManager_ = detail::NodeManager<node_type>;
    using Coincidence  = std::pair<node_type, node_type>;
    using Coincidences = std::stack<Coincidence>;

    struct Settings;  // forward decl
    struct Stats;     // forward decl

    Coincidences _coinc;
    Settings     _settings;
    Stats        _stats;

   public:
    using BaseDigraph::BaseDigraph;
    using BaseDigraph::out_degree;
    using BaseDigraph::unsafe_neighbor;

    NodeManagedDigraph()                                     = default;
    NodeManagedDigraph(NodeManagedDigraph const&)            = default;
    NodeManagedDigraph(NodeManagedDigraph&&)                 = default;
    NodeManagedDigraph& operator=(NodeManagedDigraph const&) = default;
    NodeManagedDigraph& operator=(NodeManagedDigraph&&)      = default;

    void init() {
      _coinc    = decltype(_coinc)();
      _settings = Settings();
      _stats    = Stats();
    }

    // TODO corresponding init + rvalue ref version
    template <typename N>
    NodeManagedDigraph(ActionDigraph<N> const& ad)
        : BaseDigraph(ad), NodeManager_() {
      // NodeManager always has one node active
      NodeManager_::add_active_nodes(ActionDigraph<node_type>::number_of_nodes()
                                     - 1);
    }

    // TODO this doesn't work when BaseDigraph is FelschDigraph
    // TODO split into StephenDigraph
    NodeManagedDigraph& init(Presentation<word_type> const& p) {
      NodeManager_::clear();
      BaseDigraph::init(NodeManager_::node_capacity(), p.alphabet().size());
      return *this;
    }

    // TODO fix this, only exists because previous function doesn't work for
    // FelschDigraph
    // TODO split into ToddCoxeterDigraph
    NodeManagedDigraph& init2(Presentation<word_type> const& p) {
      NodeManager_::clear();
      BaseDigraph::init(p);
      // FIXME shouldn't add nodes here because then there'll be more than
      // there should be (i.e. NodeManager and BaseDigraph will have different
      // numbers of nodes
      BaseDigraph::add_nodes(NodeManager_::node_capacity());
      return *this;
    }

    // TODO this doesn't work when BaseDigraph is FelschDigraph
    // TODO split into StephenDigraph
    NodeManagedDigraph init(Presentation<word_type>&& p) {
      NodeManager_::clear();
      BaseDigraph::init(NodeManager_::node_capacity(), p.alphabet().size());
      return *this;
    }

    // TODO fix this, only exists because previous function doesn't work for
    // FelschDigraph
    // TODO split into ToddCoxeterDigraph
    NodeManagedDigraph init2(Presentation<word_type>&& p) {
      NodeManager_::clear();
      BaseDigraph::init(std::move(p));
      // FIXME shouldn't add nodes here because then there'll be more than
      // there should be (i.e. NodeManager and BaseDigraph will have different
      // numbers of nodes
      BaseDigraph::add_nodes(NodeManager_::node_capacity());
      return *this;
    }

    bool operator==(ActionDigraph<node_type> const& that) const {
      return static_cast<ActionDigraph<node_type> const&>(*this) == that;
    }

    NodeManagedDigraph& large_collapse(size_t val) noexcept {
      _settings.large_collapse = val;
      return *this;
    }

    size_t large_collapse() const noexcept {
      return _settings.large_collapse;
    }

    node_type& cursor() {
      return NodeManager_::_current;
    }

    node_type& lookahead_cursor() {
      return NodeManager_::_current_la;
    }

    void reserve(size_t n);

    std::pair<bool, node_type>
    complete_path(node_type                 c,
                  word_type::const_iterator first,
                  word_type::const_iterator last) noexcept;

    // to StephenDigraph
    void coincide_nodes(node_type x, node_type y) {
      _coinc.emplace(x, y);
    }

    void report_coincidences() const {
      if (report::should_report()) {
        fmt::print(FORMAT("#0: ToddCoxeter: coincidences {}", _coinc.size()));
      }
    }

    template <bool RegisterDefs>
    void process_coincidences();

    // to ToddCoxeterDigraph
    void process_definitions() {
      CollectCoincidences incompat(_coinc);
      using NoPreferredDefs = typename BaseDigraph::NoPreferredDefs;
      NoPreferredDefs pref_defs;

      // debug_validate_word_graph();

      auto& defs = BaseDigraph::definitions();
      while (!defs.empty()) {
        for (size_t i = 0; i < defs.size(); ++i) {
          if (NodeManager_::is_active_node(defs[i].first)) {
            BaseDigraph::process_definition(defs[i], incompat, pref_defs);
          }
        }
        defs.clear();  // TODO doesn't this result in any_skipped always being
                       // true?
        process_coincidences<true>();
      }
    }

    void swap_nodes(node_type c, node_type d);

    Stats& stats() noexcept {
      return _stats;
    }

    void stats_check_point();

    // TODO should be private again
    node_type new_node();

    // to ToddCoxeterDigraph
    template <bool RegisterDefs = true>
    void push_definition_hlt(node_type const& c,
                             word_type const& u,
                             word_type const& v) noexcept {
      LIBSEMIGROUPS_ASSERT(NodeManager_::is_active_node(c));

      node_type   x, y;
      letter_type a, b;

      if (!u.empty()) {
        x = complete_path(c, u.begin(), u.cend() - 1).second;
        a = u.back();
      } else {
        x = c;
        a = UNDEFINED;
      }

      if (!v.empty()) {
        y = complete_path(c, v.begin(), v.cend() - 1).second;
        b = v.back();
      } else {
        y = c;
        b = UNDEFINED;
      }

      CollectCoincidences incompat(_coinc);
      auto                pref_defs
          = [this](node_type x, letter_type a, node_type y, letter_type b) {
              node_type d = new_node();
              this->template def_edge_nc<RegisterDefs>(x, a, d);
              if (a != b || x != y) {
                this->template def_edge_nc<RegisterDefs>(y, b, d);
              }
            };

      BaseDigraph::template merge_targets_of_nodes_if_possible<RegisterDefs>(
          x, a, y, b, incompat, pref_defs);
    }

    // to ToddCoxeterDigraph
    template <typename Iterator>
    size_t make_compatible(Iterator first, Iterator last) {
      // FIXME This relies on lookahead_cursor being in the right place, this is
      // bad _stats.hlt_lookahead_calls++; TODO re-enable

      size_t const old_number_of_killed
          = NodeManager_::number_of_nodes_killed();
      auto&                                 current = lookahead_cursor();
      CollectCoincidences                   incompat(_coinc);
      typename BaseDigraph::NoPreferredDefs prefdefs;
      while (current != NodeManager_::first_free_node()) {
        // TODO when we have an iterator into the active nodes, we should
        // remove the while loop, and use that in make_compatible instead
        felsch_digraph::make_compatible<DoNotRegisterDefs>(
            *this, current, current + 1, first, last, incompat, prefdefs);
        // Using NoPreferredDefs is just a (more or less) arbitrary
        // choice, could allow the other choices here too (which works,
        // but didn't seem to be very useful).
        current = NodeManager_::next_active_node(current);
      }

      return NodeManager_::number_of_nodes_killed() - old_number_of_killed;
    }

   private:
    // to ToddCoxeterDigraph
    struct CollectCoincidences {
      CollectCoincidences(Coincidences& c) : _coinc(c) {}

      bool operator()(node_type x, node_type y) {
        _coinc.emplace(x, y);
        return true;
      }

      Coincidences& _coinc;
    };
  };

  using StephenDigraph = NodeManagedDigraph<DigraphWithSources<size_t>>;

}  // namespace libsemigroups

#include "todd-coxeter-digraph.tpp"

#endif  // LIBSEMIGROUPS_TODD_COXETER_DIGRAPH_HPP_
