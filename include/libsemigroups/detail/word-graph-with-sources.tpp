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
    // Constructors
    ////////////////////////////////////////////////////////////////////////////

    template <typename Node>
    template <typename ThatNode>
    WordGraphWithSources<Node>::WordGraphWithSources(
        WordGraph<ThatNode> const& that)
        : WordGraph<Node>(that),
          _preim_init(that.out_degree(), that.number_of_nodes(), UNDEFINED),
          _preim_next(that.out_degree(), that.number_of_nodes(), UNDEFINED) {
      rebuild_sources_no_checks(WordGraph<Node>::cbegin_nodes(),
                                WordGraph<Node>::cend_nodes());
    }

    template <typename Node>
    template <typename ThatNode>
    WordGraphWithSources<Node>::WordGraphWithSources(WordGraph<ThatNode>&& that)
        : WordGraph<Node>(std::move(that)),
          _preim_init(out_degree(), number_of_nodes(), UNDEFINED),
          _preim_next(out_degree(), number_of_nodes(), UNDEFINED) {
      rebuild_sources_no_checks(WordGraph<Node>::cbegin_nodes(),
                                WordGraph<Node>::cend_nodes());
    }

    ////////////////////////////////////////////////////////////////////////////
    // Initializers
    ////////////////////////////////////////////////////////////////////////////

    // number of nodes, out-degree
    template <typename Node>
    void WordGraphWithSources<Node>::init(size_type m, size_type n) {
      WordGraph<node_type>::init(m, n);
      _preim_init.reshape(n, m);
      _preim_next.reshape(n, m);
    }

    template <typename Node>
    template <typename ThatNode>
    void WordGraphWithSources<Node>::init(WordGraph<ThatNode> const& that) {
      WordGraph<Node>::init(that);
      _preim_init.init(that.out_degree(), that.number_of_nodes(), UNDEFINED);
      _preim_next.init(that.out_degree(), that.number_of_nodes(), UNDEFINED);
      rebuild_sources_no_checks(WordGraph<Node>::cbegin_nodes(),
                                WordGraph<Node>::cend_nodes());
    }

    template <typename Node>
    template <typename ThatNode>
    void WordGraphWithSources<Node>::init(WordGraph<ThatNode>&& that) {
      WordGraph<Node>::init(std::move(that));
      _preim_init.init(out_degree(), number_of_nodes(), UNDEFINED);
      _preim_next.init(out_degree(), number_of_nodes(), UNDEFINED);
      rebuild_sources_no_checks(WordGraph<Node>::cbegin_nodes(),
                                WordGraph<Node>::cend_nodes());
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

    template <typename Node>
    void WordGraphWithSources<Node>::permute_nodes_no_checks(
        std::vector<node_type> const& p,
        std::vector<node_type> const& q,
        size_t                        m) {
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
      detail::dynamic_array2::apply_row_permutation_no_checks(
          p, this->_dynamic_array_2, _preim_init, _preim_next);
    }

    template <typename Node>
    void WordGraphWithSources<Node>::swap_nodes_no_checks(node_type c,
                                                          node_type d) {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(d < number_of_nodes());
      for (auto x : WordGraph<Node>::labels()) {
        node_type cx = WordGraph<Node>::target_no_checks(c, x);
        node_type dx = WordGraph<Node>::target_no_checks(d, x);
        replace_target_no_checks(c, x, d);
        replace_target_no_checks(d, x, c);

        if (cx == dx && cx != UNDEFINED) {
          // Swap c <--> d in preimages of cx = dx
          size_t    found = 0;
          node_type e     = _preim_init.get(cx, x);
          if (e == c) {
            ++found;
            _preim_init.set(cx, x, d);
          } else if (e == d) {
            ++found;
            _preim_init.set(cx, x, c);
          }
          while (e != UNDEFINED && found < 2) {
            node_type f = _preim_next.get(e, x);
            if (f == c) {
              ++found;
              _preim_next.set(e, x, d);
            } else if (f == d) {
              ++found;
              _preim_next.set(e, x, c);
            }
            e = f;
          }
        } else {
          replace_source_no_checks(c, d, x, cx);
          replace_source_no_checks(d, c, x, dx);
        }
        WordGraph<Node>::swap_targets_no_checks(c, d, x);
        _preim_init.swap(c, x, d, x);
        _preim_next.swap(c, x, d, x);
      }
    }

    // c must be valid, d may not be
    template <typename Node>
    void WordGraphWithSources<Node>::rename_node_no_checks(node_type c,
                                                           node_type d) {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(d < number_of_nodes());
      for (auto x : WordGraph<Node>::labels()) {
        node_type cx = WordGraph<Node>::target_no_checks(c, x);
        replace_target_no_checks(c, x, d);
        replace_source_no_checks(c, d, x, cx);
        WordGraph<Node>::swap_targets_no_checks(c, d, x);
        _preim_init.swap(c, x, d, x);
        _preim_next.swap(c, x, d, x);
      }
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

    // Is d a source of c under x?
    template <typename Node>
    bool WordGraphWithSources<Node>::is_source_no_checks(node_type  c,
                                                         label_type x,
                                                         node_type  d) const {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(x < out_degree());
      LIBSEMIGROUPS_ASSERT(d < number_of_nodes());
      c = first_source_no_checks(c, x);
      while (c != d && c != UNDEFINED) {
        c = next_source_no_checks(c, x);
      }
      return c == d;
    }

    template <typename Node>
    void WordGraphWithSources<Node>::remove_all_sources_and_targets_no_checks(
        node_type c) {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      for (auto e : WordGraph<Node>::labels()) {
        WordGraph<Node>::remove_target_no_checks(c, e);
        _preim_init.set(c, e, UNDEFINED);
      }
    }

    template <typename Node>
    void WordGraphWithSources<Node>::remove_all_sources_no_checks(node_type c) {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      for (auto e : WordGraph<Node>::labels()) {
        _preim_init.set(c, e, UNDEFINED);
      }
    }

    // Add d to the list of preimages of c under x, i.e.
    // _word_graph.target(d, x) = c
    template <typename Node>
    void
    WordGraphWithSources<Node>::add_source_no_checks(node_type  c,
                                                     label_type x,
                                                     node_type  d) noexcept {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(x < out_degree());
      LIBSEMIGROUPS_ASSERT(d < number_of_nodes());
      // If d = _preim_init(c, x), then preim_next(d, x) = d, which means that
      // if we try to loop over preimages we'll enter an infinite loop.
      if (d != _preim_init.get(c, x)) {
        // c -> e -> ... -->  c -> d -> e -> ..
        _preim_next.set(d, x, _preim_init.get(c, x));
        _preim_init.set(c, x, d);
      }
    }

    template <typename Node>
    template <typename It>
    void WordGraphWithSources<Node>::rebuild_sources_no_checks(It first,
                                                               It last) {
      for (auto it = first; it != last; ++it) {
        LIBSEMIGROUPS_ASSERT(*it < number_of_nodes());
        remove_all_sources_no_checks(*it);
      }

      for (auto it = first; it != last; ++it) {
        node_type c = *it;
        LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
        for (auto x : WordGraph<Node>::labels()) {
          auto cx = WordGraph<node_type>::target_no_checks(c, x);
          if (cx != UNDEFINED) {
            target_no_checks(c, x, cx);
          }
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////////
    // Private member functions
    ////////////////////////////////////////////////////////////////////////////

    template <typename Node>
    void WordGraphWithSources<Node>::remove_source_no_checks(node_type  cx,
                                                             label_type x,
                                                             node_type  d) {
      LIBSEMIGROUPS_ASSERT(cx < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(x < out_degree());
      LIBSEMIGROUPS_ASSERT(d < number_of_nodes());

      node_type e = _preim_init.get(cx, x);
      if (e == d) {
        _preim_init.set(cx, x, _preim_next.get(d, x));
      } else {
        while (_preim_next.get(e, x) != d) {
          e = _preim_next.get(e, x);
        }
        LIBSEMIGROUPS_ASSERT(_preim_next.get(e, x) == d);
        _preim_next.set(e, x, _preim_next.get(d, x));
      }
    }

    // All edges of the form e - x -> c are replaced with e - x -> d,
    template <typename Node>
    void WordGraphWithSources<Node>::replace_target_no_checks(node_type  c,
                                                              label_type x,
                                                              node_type  d) {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(x < out_degree());
      LIBSEMIGROUPS_ASSERT(d < number_of_nodes());
      node_type e = _preim_init.get(c, x);
      while (e != UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(WordGraph<Node>::target_no_checks(e, x) == c);
        WordGraph<Node>::target_no_checks(e, x, d);
        e = _preim_next.get(e, x);
      }
    }

    template <typename Node>
    void WordGraphWithSources<Node>::replace_source_no_checks(node_type  c,
                                                              node_type  d,
                                                              label_type x,
                                                              node_type  cx) {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(x < out_degree());
      LIBSEMIGROUPS_ASSERT(d < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(cx < number_of_nodes());
      if (cx != UNDEFINED) {
        // Replace c <-- d in preimages of cx, and d is not a preimage of cx
        // under x
        node_type e = _preim_init.get(cx, x);
        if (e == c) {
          _preim_init.set(cx, x, d);
          return;
        }
        while (e != UNDEFINED) {
          node_type f = _preim_next.get(e, x);
          if (f == c) {
            _preim_next.set(e, x, d);
            return;
          }
          e = f;
        }
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
