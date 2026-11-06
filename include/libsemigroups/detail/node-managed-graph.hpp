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
// * code coverage

#ifndef LIBSEMIGROUPS_DETAIL_NODE_MANAGED_GRAPH_HPP_
#define LIBSEMIGROUPS_DETAIL_NODE_MANAGED_GRAPH_HPP_

#include <algorithm>  // for minmax
#include <atomic>     // for atomic_uint64_t
#include <cstddef>    // for size_t
#include <cstdint>    // for uint64_t
#include <random>     // for random_device, unifo...
#include <stack>      // for stack
#include <tuple>      // for tie
#include <utility>    // for pair
#include <vector>     // for vector

#include "libsemigroups/adapters.hpp"            // for Noop
#include "libsemigroups/constants.hpp"           // for UNDEFINED
#include "libsemigroups/debug.hpp"               // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/ranges.hpp"              // for advance_by
#include "libsemigroups/runner.hpp"              // for Reporter
#include "libsemigroups/types.hpp"               // for word_type, letter_type
#include "libsemigroups/word-graph-class.hpp"    // for WordGraph
#include "libsemigroups/word-graph-helpers.hpp"  // for last_node_on_path_no...

#include "node-manager.hpp"             // for NodeManager
#include "report.hpp"                   // for report_default
#include "string.hpp"                   // for group_digits
#include "word-graph-with-sources.hpp"  // for WordGraphWithSources

namespace libsemigroups::detail {

  template <typename Node>
  class NodeManagedGraph : public WordGraphWithSources<Node>,
                           public NodeManager<Node>,
                           public Reporter {
   public:
    ////////////////////////////////////////////////////////////////////////
    // Aliases - public
    ////////////////////////////////////////////////////////////////////////

    using node_type  = typename WordGraphWithSources<Node>::node_type;
    using label_type = typename WordGraphWithSources<Node>::label_type;

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
    // WordGraphWithSources<Node> mem fns
    ////////////////////////////////////////////////////////////////////////

    using WordGraphWithSources<Node>::out_degree;

    NodeManagedGraph& target_no_checks(node_type  s,
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
    using WordGraphWithSources<Node>::target_no_checks;

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

    // TODO needed?
    template <typename OtherNode>
    explicit NodeManagedGraph(WordGraph<OtherNode> const& wg)
        : WordGraphWithSources<Node>(wg), NodeManager<node_type>() {
      // NodeManager always has one node active
      NodeManager<node_type>::add_active_nodes(
          WordGraph<node_type>::number_of_nodes() - 1);
      _stats.num_edges_active = wg.number_of_edges();
    }

    template <typename OtherNode>
    NodeManagedGraph& init(WordGraph<OtherNode> const& wg) {
      init();
      WordGraphWithSources<Node>::init(wg);
      LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() > 0);
      // NodeManager always has one node active
      NodeManager<node_type>::add_active_nodes(
          WordGraph<node_type>::number_of_nodes() - 1);
      _stats.num_edges_active = wg.number_of_edges();
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

    [[nodiscard]] size_t number_of_coincidences() const noexcept {
      return _coinc.size();
    }

    ////////////////////////////////////////////////////////////////////////
    // Modifiers
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] node_type new_node();

    template <typename Register>
    [[nodiscard]] node_type complete_path(node_type                 s,
                                          word_type::const_iterator first,
                                          word_type::const_iterator last,
                                          Register const&) noexcept;

    void merge_nodes_no_checks(node_type x, node_type y) {
      _coinc.emplace(x, y);
    }

    // Returns true if any changes were made to the graph and false o/w.
    template <typename Register>
    bool process_coincidences(Register const&);

    // q is not not const here because permute_nodes_no_checks changes q in
    // place.
    void standardize_no_checks(std::vector<node_type> const& p,
                               std::vector<node_type>&       q);

    void permute_nodes_no_checks(std::vector<node_type> const& p,
                                 std::vector<node_type>&       q) {
      WordGraphWithSources<Node>::permute_nodes_no_checks(
          p, q, NodeManager<node_type>::number_of_nodes_active());
      NodeManager<node_type>::apply_permutation(p);
    }

    // Not currently used for anything, previously required for immediate
    // standardization. The implementation has been deleted but should be
    // available in the git history pre-v3.7.0
    // void swap_nodes_no_checks(node_type c, node_type d);
  };  // class NodeManagedGraph

  namespace node_managed_graph {
    template <typename Node>
    Node random_active_node(NodeManagedGraph<Node> const& nmg);

  }  // namespace node_managed_graph
}  // namespace libsemigroups::detail

#include "node-managed-graph.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_NODE_MANAGED_GRAPH_HPP_
