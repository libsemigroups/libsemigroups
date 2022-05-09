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

#include "digraph.hpp"  // for ActionDigraph

namespace libsemigroups {
  template <typename T>
  class DigraphWithSources : public ActionDigraph<T> {
   public:
    using node_type = T;
    using size_type = node_type;

    using Coincidence  = std::pair<node_type, node_type>;
    using Coincidences = std::stack<Coincidence>;

    class Deductions;  // Forward declaration

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

    node_type first_source(node_type c, letter_type x) const noexcept {
      return _preim_init.get(c, x);
    }

    node_type next_source(node_type c, letter_type x) const noexcept {
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

    // TODO(Sims) move to .tpp file + make the actions on Conincicdences and
    // Deductions less prescriptive.
    // I.e. pass a function to be called when ever a new edge is defined, or a
    // contradiction is obtained. Or even move coincidences in here so that we
    // can determinise the graph
    template <typename NewEdgeFunc, typename IncompatibleFunc>
    void merge_nodes(node_type          min,
                     node_type          max,
                     NewEdgeFunc&&      new_edge_func,
                     IncompatibleFunc&& incompat_func) {
      LIBSEMIGROUPS_ASSERT(min < max);
      for (letter_type i = 0; i < this->out_degree(); ++i) {
        // v -> max is an edge
        node_type v = first_source(max, i);
        while (v != UNDEFINED) {
          auto w = next_source(v, i);
          add_edge_nc(v, min, i);
          new_edge_func(v, i);
          v = w;
        }

        // Now let <v> be the IMAGE of <max>
        v = this->unsafe_neighbor(max, i);
        if (v != UNDEFINED) {
          remove_source(v, i, max);
          // Let <u> be the image of <min>, and ensure <u> = <v>
          node_type u = this->unsafe_neighbor(min, i);
          if (u == UNDEFINED) {
            add_edge_nc(min, v, i);
            new_edge_func(min, i);
          } else if (u != v) {
            incompat_func(u, v);
          }
        }
      }
    }

#ifdef LIBSEMIGROUPS_DEBUG
    // Is d a source of c under x?
    bool is_source(node_type c, node_type d, letter_type x) const {
      c = first_source(c, x);
      while (c != d && c != UNDEFINED) {
        c = next_source(c, x);
      }
      return c == d;
    }
#endif
    void clear_sources_and_targets(node_type c);
    void clear_sources(node_type c);

    // Add d to the list of preimages of c under x, i.e.
    // _word_graph.target(d, x) = c
    void add_source(node_type c, letter_type x, node_type d) noexcept {
      LIBSEMIGROUPS_ASSERT(x < this->out_degree());
      // c -> e -> ... -->  c -> d -> e -> ..
      _preim_next.set(d, x, _preim_init.get(c, x));
      _preim_init.set(c, x, d);
    }

   private:
    void remove_source(node_type cx, letter_type x, node_type d);
    void replace_target(node_type c, node_type d, size_t x);
    void replace_source(node_type c, node_type d, size_t x, node_type cx);
  };
}  // namespace libsemigroups

#include "digraph-with-sources.tpp"

#endif  // LIBSEMIGROUPS_DIGRAPH_WITH_SOURCES_HPP_
