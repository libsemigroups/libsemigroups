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

// This file contains a declaration of a class for performing the "low-index
// congruence" algorithm for semigroups and monoid.
//
// In the comments in this file we refer to "valid nodes", this means nodes in
// the graph where the values returned by first_source and next_source are
// valid (i.e. correspond to edges in the underlying ActionDigraph that point
// into the current node). Validity of nodes is not tracked by
// DigraphWithSources, and it is the responsibility of the caller to ensure
// that nodes are valid where required by the various member functions of
// DigraphWithSources.

#ifndef LIBSEMIGROUPS_DIGRAPH_WITH_SOURCES_HPP_
#define LIBSEMIGROUPS_DIGRAPH_WITH_SOURCES_HPP_

#include <cstddef>  // for size_t
#include <stack>    // for stack
#include <utility>  // for pair
#include <vector>   // for vector

#include "config.hpp"      // for LIBSEMIGROUPS_DEBUG
#include "constants.hpp"   // for UNDEFINED
#include "containers.hpp"  // for DynamicArray2
#include "digraph.hpp"     // for ActionDigraph
#include "types.hpp"       // for letter_type
                           //
namespace libsemigroups {
  class Forest;  // forward decl

  template <typename T>
  class DigraphWithSources : public ActionDigraph<T> {
   public:
    using node_type = T;
    using size_type = node_type;

   private:
    using table_type = detail::DynamicArray2<node_type>;
    table_type _preim_init;
    table_type _preim_next;

   public:
    using ActionDigraph<T>::ActionDigraph;

    explicit DigraphWithSources(size_type m = 0, size_type n = 0)
        : ActionDigraph<node_type>(m, n),
          _preim_init(n, m, UNDEFINED),
          _preim_next(n, m, UNDEFINED) {}

    DigraphWithSources(DigraphWithSources&&)      = default;
    DigraphWithSources(DigraphWithSources const&) = default;
    DigraphWithSources& operator=(DigraphWithSources const&) = default;
    DigraphWithSources& operator=(DigraphWithSources&&) = default;

    void init(size_type m, size_type n);

    void add_edge_nc(node_type c, node_type d, letter_type x) noexcept {
      ActionDigraph<node_type>::add_edge_nc(c, d, x);
      add_source(d, x, c);
    }

    void remove_edge_nc(node_type c, letter_type x) noexcept {
      remove_source(this->unsafe_neighbor(c, x), x, c);
      ActionDigraph<node_type>::remove_edge_nc(c, x);
    }

    void add_nodes(size_type m) {
      ActionDigraph<node_type>::add_nodes(m);
      _preim_init.add_rows(m);
      _preim_next.add_rows(m);
    }

    void add_to_out_degree(size_type m) {
      _preim_init.add_cols(m);
      _preim_next.add_cols(m);
      ActionDigraph<node_type>::add_to_out_degree(m);
    }

    void shrink_to_fit(size_type m) {
      this->restrict(m);
      _preim_init.shrink_rows_to(m);
      _preim_next.shrink_rows_to(m);
    }

    inline node_type first_source(node_type c, letter_type x) const noexcept {
      return _preim_init.get(c, x);
    }

    inline node_type next_source(node_type c, letter_type x) const noexcept {
      return _preim_next.get(c, x);
    }

    // The permutation q must map the valid nodes to the [0, .. , n), where n
    // is the number of valid nodes, and p = q ^ -1.
    void permute_nodes_nc(std::vector<node_type>& p,
                          std::vector<node_type>& q,
                          size_t                  n);

    // Swaps valid nodes c and d, if c or d is not valid, then this will fail
    // spectacularly (no checks are performed)
    void swap_nodes(node_type c, node_type d);

    // Rename c to d, i.e. node d has the exact same in- and out-neighbours as
    // c after this is called. Assumed that c is valid when this function is
    // called, and that d is valid after it is called. This is a one-sided
    // version of swap nodes.
    void rename_node(node_type c, node_type d);

    template <typename NewEdgeFunc, typename IncompatibleFunc>
    void merge_nodes(node_type          min,
                     node_type          max,
                     NewEdgeFunc&&      new_edge_func,
                     IncompatibleFunc&& incompat_func);

#ifdef LIBSEMIGROUPS_DEBUG
    // Is d a source of c under x?
    bool is_source(node_type c, node_type d, letter_type x) const;
#endif

    void clear_sources_and_targets(node_type c);
    void clear_sources(node_type c);
    void add_source(node_type c, letter_type x, node_type d) noexcept;

   private:
    void remove_source(node_type cx, letter_type x, node_type d);
    void replace_target(node_type c, node_type d, size_t x);
    void replace_source(node_type c, node_type d, size_t x, node_type cx);
  };

  namespace digraph_with_sources {
    // Return value indicates whether or not the graph was modified.
    template <typename T>
    bool standardize(T& d, Forest& f);

    template <typename T>
    std::pair<bool, Forest> standardize(T& d);

  }  // namespace digraph_with_sources
}  // namespace libsemigroups

#include "digraph-with-sources.tpp"

#endif  // LIBSEMIGROUPS_DIGRAPH_WITH_SOURCES_HPP_
