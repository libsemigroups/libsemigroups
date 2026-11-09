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

// This file contains an implementation of a class for WordGraphs that also
// keep track of the edges pointing into any node (not only those point out).

namespace libsemigroups {

  namespace detail {
    ////////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////////

    template <typename Node>
    template <typename ThatNode>
    WordGraphWithSources<Node>::WordGraphWithSources(
        WordGraph<ThatNode> const& that)
        : WordGraph<Node>(that),
          _preim_init(that.out_degree(), that.number_of_nodes(), UNDEFINED),
          _preim_next(that.out_degree(), that.number_of_nodes(), UNDEFINED) {
      populate_sources_no_checks();
    }

    template <typename Node>
    template <typename ThatNode>
    WordGraphWithSources<Node>::WordGraphWithSources(WordGraph<ThatNode>&& that)
        : WordGraph<Node>(std::move(that)),
          _preim_init(out_degree(), number_of_nodes(), UNDEFINED),
          _preim_next(out_degree(), number_of_nodes(), UNDEFINED) {
      populate_sources_no_checks();
    }

    template <typename Node>
    template <typename ThatNode>
    void WordGraphWithSources<Node>::init(WordGraph<ThatNode> const& that) {
      WordGraph<Node>::init(that);
      _preim_init.init(that.out_degree(), that.number_of_nodes(), UNDEFINED);
      _preim_next.init(that.out_degree(), that.number_of_nodes(), UNDEFINED);
      populate_sources_no_checks();
    }

    template <typename Node>
    template <typename ThatNode>
    void WordGraphWithSources<Node>::init(WordGraph<ThatNode>&& that) {
      WordGraph<Node>::init(std::move(that));
      _preim_init.init(out_degree(), number_of_nodes(), UNDEFINED);
      _preim_next.init(out_degree(), number_of_nodes(), UNDEFINED);
      populate_sources_no_checks();
    }

    template <typename Node>
    WordGraphWithSources<Node>::~WordGraphWithSources() = default;

    ////////////////////////////////////////////////////////////////////////////
    // Public member functions
    ////////////////////////////////////////////////////////////////////////////

    template <typename Node>
    void
    WordGraphWithSources<Node>::induced_subgraph_no_checks(node_type first,
                                                           node_type last) {
      LIBSEMIGROUPS_ASSERT(first <= number_of_nodes());
      LIBSEMIGROUPS_ASSERT(last <= number_of_nodes());
      LIBSEMIGROUPS_ASSERT(first <= last);
      WordGraph<Node>::induced_subgraph_no_checks(first, last);
      _preim_init.shrink_rows_to_no_checks(first, last);
      _preim_next.shrink_rows_to_no_checks(first, last);
      if (first != 0) {
        std::for_each(_preim_init.begin(),
                      _preim_init.end(),
                      [&first](node_type& x) { x -= first; });
        std::for_each(_preim_next.begin(),
                      _preim_next.end(),
                      [&first](node_type& x) { x -= first; });
      }
    }

    // The vector q is not const because apply_row_permutation_no_copy_no_checks
    // modifies q in order to permute the array in-place without allocating
    // additional memory.
    template <typename Node>
    void WordGraphWithSources<Node>::permute_nodes_no_checks(
        std::vector<node_type> const& p,
        std::vector<node_type>&       q,
        size_t                        m) {
      LIBSEMIGROUPS_ASSERT(m <= p.size());
      // p : new -> old, q = p ^ -1
      // Permute all the values in the _table, and pre-images, that relate
      // to active nodes
      size_t const n = out_degree();
      for (node_type s = 0; s < m; ++s) {
        for (label_type a = 0; a < n; ++a) {
          node_type t = WordGraph<Node>::target_no_checks(p[s], a);
          WordGraph<Node>::target_no_checks(
              p[s], a, (t == UNDEFINED ? t : q[t]));
          t = _preim_init.get(p[s], a);
          _preim_init.set(p[s], a, (t == UNDEFINED ? t : q[t]));
          t = _preim_next.get(p[s], a);
          _preim_next.set(p[s], a, (t == UNDEFINED ? t : q[t]));
        }
      }
      // Permute the rows themselves
      detail::dynamic_array2::apply_row_permutation_no_copy_no_checks(
          q, this->_dynamic_array_2, _preim_init, _preim_next);
    }

    template <typename Node>
    template <typename NewEdgeFunc, typename IncompatibleFunc>
    uint64_t WordGraphWithSources<Node>::merge_nodes_no_checks(
        node_type          min,
        node_type          max,
        NewEdgeFunc&&      new_edge,
        IncompatibleFunc&& incompat) {
      LIBSEMIGROUPS_ASSERT(min < max);
      LIBSEMIGROUPS_ASSERT(min < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(max < number_of_nodes());
      uint64_t num_edges_removed = 0;
      for (auto i : WordGraph<Node>::labels()) {
        node_type v = first_source_no_checks(max, i);
        while (v != UNDEFINED) {
          auto w = next_source_no_checks(v, i);
          LIBSEMIGROUPS_ASSERT(WordGraph<Node>::target_no_checks(v, i) != min);
          LIBSEMIGROUPS_ASSERT(WordGraph<Node>::target_no_checks(v, i) == max);
          target_no_checks(v, i, min);
          new_edge(v, i);
          v = w;
        }

        v = WordGraph<Node>::target_no_checks(max, i);
        if (v != UNDEFINED) {
          num_edges_removed++;
          remove_source_no_checks(v, i, max);
          node_type const u = WordGraph<Node>::target_no_checks(min, i);
          if (u == UNDEFINED) {
            LIBSEMIGROUPS_ASSERT(u != min);
            target_no_checks(min, i, v);
            new_edge(min, i);
            num_edges_removed--;
          } else if (u != v) {
            incompat(u, v);
          }
        }
      }
      return num_edges_removed;
    }

    // Is s a source of t under a?
    template <typename Node>
    bool WordGraphWithSources<Node>::is_source_no_checks(node_type  t,
                                                         label_type a,
                                                         node_type  s) const {
      LIBSEMIGROUPS_ASSERT(t < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(a < out_degree());
      LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
      t = first_source_no_checks(t, a);
      while (t != s && t != UNDEFINED) {
        t = next_source_no_checks(t, a);
      }
      return t == s;
    }

    template <typename Node>
    void WordGraphWithSources<Node>::remove_all_sources_and_targets_no_checks(
        node_type n) {
      LIBSEMIGROUPS_ASSERT(n < number_of_nodes());
      for (auto e : WordGraph<Node>::labels()) {
        WordGraph<Node>::remove_target_no_checks(n, e);
        _preim_init.set(n, e, UNDEFINED);
      }
    }

    template <typename Node>
    void WordGraphWithSources<Node>::remove_all_sources_no_checks(node_type n) {
      LIBSEMIGROUPS_ASSERT(n < number_of_nodes());
      for (auto e : WordGraph<Node>::labels()) {
        _preim_init.set(n, e, UNDEFINED);
      }
    }

    // Add s to the list of preimages of t under a, i.e.
    // _word_graph.target(s, a) = t
    template <typename Node>
    void
    WordGraphWithSources<Node>::add_source_no_checks(node_type  t,
                                                     label_type a,
                                                     node_type  s) noexcept {
      LIBSEMIGROUPS_ASSERT(t < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(a < out_degree());
      LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
      // If s = _preim_init(t, a), then preim_next(s, a) = s, which means that
      // if we try to loop over preimages we'll enter an infinite loop.
      if (s != _preim_init.get(t, a)) {
        // t -> e -> ... -->  t -> s -> e -> ..
        _preim_next.set(s, a, _preim_init.get(t, a));
        _preim_init.set(t, a, s);
      }
    }

    ////////////////////////////////////////////////////////////////////////////
    // Private member functions
    ////////////////////////////////////////////////////////////////////////////

    template <typename Node>
    void WordGraphWithSources<Node>::populate_sources_no_checks() {
      for (auto s : WordGraph<Node>::nodes()) {
        for (auto a : WordGraph<Node>::labels()) {
          auto t = WordGraph<node_type>::target_no_checks(s, a);
          if (t != UNDEFINED) {
            add_source_no_checks(t, a, s);
          }
        }
      }
    }

    template <typename Node>
    void WordGraphWithSources<Node>::remove_source_no_checks(node_type  t,
                                                             label_type a,
                                                             node_type  s) {
      LIBSEMIGROUPS_ASSERT(t < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(a < out_degree());
      LIBSEMIGROUPS_ASSERT(s < number_of_nodes());

      node_type n = _preim_init.get(t, a);
      if (n == s) {
        _preim_init.set(t, a, _preim_next.get(s, a));
      } else {
        while (_preim_next.get(n, a) != s) {
          n = _preim_next.get(n, a);
        }
        LIBSEMIGROUPS_ASSERT(_preim_next.get(n, a) == s);
        _preim_next.set(n, a, _preim_next.get(s, a));
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
