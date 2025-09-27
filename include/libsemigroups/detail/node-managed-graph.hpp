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

// This file contains the declaration of the NodeManagedGraph class, used
// by Stephen and by ToddCoxeterImpl.

// TODO:
// * iwyu
// * code coverage

#ifndef LIBSEMIGROUPS_DETAIL_NODE_MANAGED_GRAPH_HPP_
#define LIBSEMIGROUPS_DETAIL_NODE_MANAGED_GRAPH_HPP_

#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t
#include <vector>   // for vector

#include <cstddef>      // for size_t
#include <cstdint>      // for uint32_t
#include <stack>        // for stack
#include <type_traits>  // for is_base_of
#include <utility>      // for pair
#include <vector>       // for vector

#include "libsemigroups/presentation.hpp"  // for Presentation, Presentation<>:...
#include "libsemigroups/runner.hpp"
#include "libsemigroups/types.hpp"       // for word_type
#include "libsemigroups/word-graph.hpp"  // for WordGraph

#include "felsch-graph.hpp"
#include "node-manager.hpp"             // for NodeManager
#include "report.hpp"                   // for REPORT_DEFAULT
#include "timer.hpp"                    // for Timer
#include "word-graph-with-sources.hpp"  // for WordGraphWithSources

namespace libsemigroups {
  namespace detail {

    template <typename Node>
    class NodeManagedGraph : public WordGraphWithSources<Node>,
                             public NodeManager<Node>,
                             public Reporter {
     public:
      ////////////////////////////////////////////////////////////////////////
      // Aliases - public
      ////////////////////////////////////////////////////////////////////////

      using BaseGraph  = WordGraphWithSources<Node>;
      using node_type  = typename BaseGraph::node_type;
      using label_type = typename BaseGraph::label_type;

      static_assert(
          std::is_base_of<WordGraphWithSources<node_type>, BaseGraph>::value,
          "the template parameter BaseGraph must be derived from "
          "WordGraphWithSources<node_type>");

     protected:
      ////////////////////////////////////////////////////////////////////////
      // Data - protected
      ////////////////////////////////////////////////////////////////////////

      using Coincidence  = std::pair<node_type, node_type>;
      using Coincidences = std::stack<Coincidence>;
      struct CollectCoincidences;  // forward decl

      Coincidences _coinc;

     private:
      ////////////////////////////////////////////////////////////////////////
      // Data - private
      ////////////////////////////////////////////////////////////////////////

      struct Settings;  // forward decl
      struct Stats;     // forward decl

      Settings      _settings;
      mutable Stats _stats;

     public:
      ////////////////////////////////////////////////////////////////////////
      // BaseGraph mem fns
      ////////////////////////////////////////////////////////////////////////

      using BaseGraph::out_degree;

      NodeManagedGraph& target_no_checks(node_type  s,
                                         label_type a,
                                         node_type  t) noexcept {
        _stats.num_edges_active += (t != UNDEFINED);
        WordGraphWithSources<Node>::target_no_checks(s, a, t);
        // The next assertion is extremely slow so don't do it!
        // LIBSEMIGROUPS_ASSERT(_stats.num_edges_active
        //                      == count_number_of_edges_active());
        return *this;
      }
      using BaseGraph::target_no_checks;

      using NodeManager<node_type>::cursor;
      using NodeManager<node_type>::lookahead_cursor;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + initializers
      ////////////////////////////////////////////////////////////////////////

      NodeManagedGraph();
      NodeManagedGraph& init();

      NodeManagedGraph(NodeManagedGraph const&);
      NodeManagedGraph(NodeManagedGraph&&);
      NodeManagedGraph& operator=(NodeManagedGraph const&);
      NodeManagedGraph& operator=(NodeManagedGraph&&);
      ~NodeManagedGraph();

      template <typename OtherNode>
      explicit NodeManagedGraph(WordGraph<OtherNode> const& wg)
          : BaseGraph(wg), NodeManager<node_type>() {
        // NodeManager always has one node active
        NodeManager<node_type>::add_active_nodes(
            WordGraph<node_type>::number_of_nodes() - 1);
        _stats.num_edges_active += wg.number_of_edges();
      }

      template <typename OtherNode>
      NodeManagedGraph& init(WordGraph<OtherNode> const& wg) {
        init();
        BaseGraph::init(wg);
        // NodeManager always has one node active
        NodeManager<node_type>::add_active_nodes(
            WordGraph<node_type>::number_of_nodes() - 1);
        _stats.num_edges_active += wg.number_of_edges();
        return *this;
      }

      template <typename OtherNode>
      NodeManagedGraph& operator=(WordGraph<OtherNode> const& wg);

      NodeManagedGraph& reserve(size_t n);

      ////////////////////////////////////////////////////////////////////////
      // Operators
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] bool operator==(WordGraph<node_type> const& that) const {
        return static_cast<WordGraph<node_type> const&>(*this) == that;
      }

      ////////////////////////////////////////////////////////////////////////
      // Settings
      ////////////////////////////////////////////////////////////////////////

      NodeManagedGraph& large_collapse(size_t val) noexcept {
        _settings.large_collapse = val;
        return *this;
      }

      [[nodiscard]] size_t large_collapse() const noexcept {
        return _settings.large_collapse;
      }

      ////////////////////////////////////////////////////////////////////////
      // Stats
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] Stats& stats() noexcept {
        return _stats;
      }

      [[nodiscard]] Stats& stats() const noexcept {
        return _stats;
      }

      void stats_check_point() const;

      ////////////////////////////////////////////////////////////////////////
      // Accessors
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] uint64_t number_of_edges_active() const noexcept {
        return _stats.num_edges_active;
      }

      // 100% not thread safe
      [[nodiscard]] uint64_t count_number_of_edges_active() const noexcept;

      ////////////////////////////////////////////////////////////////////////
      // Modifiers
      ////////////////////////////////////////////////////////////////////////

      node_type new_node();

      template <bool RegisterDefs = false>
      [[nodiscard]] std::pair<bool, node_type>
      complete_path(node_type                 c,
                    word_type::const_iterator first,
                    word_type::const_iterator last) noexcept;

      void merge_nodes_no_checks(node_type x, node_type y) {
        _coinc.emplace(x, y);
      }

      template <typename Functor = Noop>
      void process_coincidences(Functor&& = Noop{});

      void standardize(std::vector<node_type> const& p,
                       std::vector<node_type> const& q) {
        BaseGraph::permute_nodes_no_checks(
            p, q, NodeManager<node_type>::number_of_nodes_active());
        NodeManager<node_type>::compact();
      }

      void permute_nodes_no_checks(std::vector<node_type> const& p,
                                   std::vector<node_type> const& q) {
        BaseGraph::permute_nodes_no_checks(
            p, q, NodeManager<node_type>::number_of_nodes_active());
        NodeManager<node_type>::apply_permutation(p);
      }

      // Not currently used for anything, previously required for immediate
      // standardization
      void swap_nodes_no_checks(node_type c, node_type d);
    };

    namespace node_managed_graph {
      template <typename BaseGraph>
      typename BaseGraph::node_type
      random_active_node(NodeManagedGraph<BaseGraph> const& nmg);

    }  // namespace node_managed_graph

  }  // namespace detail
}  // namespace libsemigroups

#include "node-managed-graph.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_NODE_MANAGED_GRAPH_HPP_
