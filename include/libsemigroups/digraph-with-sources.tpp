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

// This file contains an implementation of a class for WordGraphs that also
// keep track of the edges pointing into any node (not only those point out).

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////////
  // Constructors
  ////////////////////////////////////////////////////////////////////////////

  template <typename NodeType>
  template <typename ThatNodeType>
  DigraphWithSources<NodeType>::DigraphWithSources(
      WordGraph<ThatNodeType> const& that)
      : WordGraph<NodeType>(that),
        _preim_init(that.out_degree(), that.number_of_nodes(), UNDEFINED),
        _preim_next(that.out_degree(), that.number_of_nodes(), UNDEFINED) {
    rebuild_sources(WordGraph<NodeType>::cbegin_nodes(),
                    WordGraph<NodeType>::cend_nodes());
  }

  template <typename NodeType>
  template <typename ThatNodeType>
  DigraphWithSources<NodeType>::DigraphWithSources(
      WordGraph<ThatNodeType>&& that)
      : WordGraph<NodeType>(std::move(that)),
        _preim_init(WordGraph<NodeType>::out_degree(),
                    WordGraph<NodeType>::number_of_nodes(),
                    UNDEFINED),
        _preim_next(WordGraph<NodeType>::out_degree(),
                    WordGraph<NodeType>::number_of_nodes(),
                    UNDEFINED) {
    rebuild_sources(WordGraph<NodeType>::cbegin_nodes(),
                    WordGraph<NodeType>::cend_nodes());
  }

  ////////////////////////////////////////////////////////////////////////////
  // Initializers
  ////////////////////////////////////////////////////////////////////////////

  // number of nodes, out-degree
  template <typename NodeType>
  void DigraphWithSources<NodeType>::init(size_type m, size_type n) {
    WordGraph<node_type>::init(m, n);
    _preim_init.reshape(n, m);
    _preim_next.reshape(n, m);
  }

  template <typename NodeType>
  template <typename ThatNodeType>
  void DigraphWithSources<NodeType>::init(WordGraph<ThatNodeType> const& that) {
    WordGraph<NodeType>::init(that);
    _preim_init.init(that.out_degree(), that.number_of_nodes(), UNDEFINED);
    _preim_next.init(that.out_degree(), that.number_of_nodes(), UNDEFINED);
    rebuild_sources(WordGraph<NodeType>::cbegin_nodes(),
                    WordGraph<NodeType>::cend_nodes());
  }

  template <typename NodeType>
  template <typename ThatNodeType>
  void DigraphWithSources<NodeType>::init(WordGraph<ThatNodeType>&& that) {
    WordGraph<NodeType>::init(std::move(that));
    _preim_init.init(WordGraph<NodeType>::out_degree(),
                     WordGraph<NodeType>::number_of_nodes(),
                     UNDEFINED);
    _preim_next.init(WordGraph<NodeType>::out_degree(),
                     WordGraph<NodeType>::number_of_nodes(),
                     UNDEFINED);
    rebuild_sources(WordGraph<NodeType>::cbegin_nodes(),
                    WordGraph<NodeType>::cend_nodes());
  }

  ////////////////////////////////////////////////////////////////////////////
  // Public member functions
  ////////////////////////////////////////////////////////////////////////////

  template <typename NodeType>
  void DigraphWithSources<NodeType>::permute_nodes_no_checks(
      std::vector<node_type> const& p,
      std::vector<node_type> const& q,
      size_t                        m) {
    // p : new -> old, q = p ^ -1
    node_type    c = 0;
    size_t const n = WordGraph<NodeType>::out_degree();
    // Permute all the values in the _table, and pre-images, that relate
    // to active cosets
    while (c < m) {
      for (label_type x = 0; x < n; ++x) {
        node_type i = WordGraph<NodeType>::target_no_checks(p[c], x);
        WordGraph<NodeType>::set_target_no_checks(
            p[c], x, (i == UNDEFINED ? i : q[i]));
        i = _preim_init.get(p[c], x);
        _preim_init.set(p[c], x, (i == UNDEFINED ? i : q[i]));
        i = _preim_next.get(p[c], x);
        _preim_next.set(p[c], x, (i == UNDEFINED ? i : q[i]));
      }
      c++;
    }
    // Permute the rows themselves
    WordGraph<NodeType>::apply_row_permutation(p);
    _preim_init.apply_row_permutation(p);
    _preim_next.apply_row_permutation(p);
  }

  template <typename NodeType>
  void DigraphWithSources<NodeType>::swap_nodes(node_type c, node_type d) {
    size_t const n = WordGraph<NodeType>::out_degree();
    for (label_type x = 0; x < n; ++x) {
      node_type cx = WordGraph<NodeType>::target_no_checks(c, x);
      node_type dx = WordGraph<NodeType>::target_no_checks(d, x);
      replace_target(c, d, x);
      replace_target(d, c, x);

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
        replace_source(c, d, x, cx);
        replace_source(d, c, x, dx);
      }
      WordGraph<NodeType>::swap_targets_no_checks(c, d, x);
      _preim_init.swap(c, x, d, x);
      _preim_next.swap(c, x, d, x);
    }
  }

  // c must be valid, d may not be
  template <typename NodeType>
  void DigraphWithSources<NodeType>::rename_node(node_type c, node_type d) {
    size_t const n = WordGraph<NodeType>::out_degree();

    for (label_type x = 0; x < n; ++x) {
      node_type cx = WordGraph<NodeType>::target_no_checks(c, x);
      replace_target(c, d, x);
      replace_source(c, d, x, cx);
      WordGraph<NodeType>::swap_targets_no_checks(c, d, x);
      _preim_init.swap(c, x, d, x);
      _preim_next.swap(c, x, d, x);
    }
  }

  template <typename NodeType>
  template <typename NewEdgeFunc, typename IncompatibleFunc>
  void DigraphWithSources<NodeType>::merge_nodes(node_type          min,
                                                 node_type          max,
                                                 NewEdgeFunc&&      new_edge,
                                                 IncompatibleFunc&& incompat) {
    LIBSEMIGROUPS_ASSERT(min < max);
    for (label_type i = 0; i < WordGraph<NodeType>::out_degree(); ++i) {
      // v -> max is an edge
      node_type v = first_source(max, i);
      while (v != UNDEFINED) {
        auto w = next_source(v, i);
        if (WordGraph<NodeType>::target_no_checks(v, i) != min) {
          set_target_no_checks(v, i, min);
          new_edge(v, i);
        }
        v = w;
      }

      // Now let <v> be the IMAGE of <max>
      v = WordGraph<NodeType>::target_no_checks(max, i);
      if (v != UNDEFINED) {
        remove_source(v, i, max);
        // Let <u> be the image of <min>, and ensure <u> = <v>
        node_type u = WordGraph<NodeType>::target_no_checks(min, i);
        if (u == UNDEFINED) {
          if (WordGraph<NodeType>::target_no_checks(min, i) != min) {
            set_target_no_checks(min, i, v);
            new_edge(min, i);
          }
        } else if (u != v) {
          incompat(u, v);
        }
      }
    }
  }

  // Is d a source of c under x?
  template <typename NodeType>
  bool DigraphWithSources<NodeType>::is_source(node_type  c,
                                               node_type  d,
                                               label_type x) const {
    c = first_source(c, x);
    while (c != d && c != UNDEFINED) {
      c = next_source(c, x);
    }
    return c == d;
  }

  template <typename NodeType>
  void DigraphWithSources<NodeType>::clear_sources_and_targets(node_type c) {
    for (label_type i = 0; i < WordGraph<NodeType>::out_degree(); i++) {
      // TODO replace with remove_edge
      WordGraph<NodeType>::set_target_no_checks(c, i, UNDEFINED);
      _preim_init.set(c, i, UNDEFINED);
    }
  }

  template <typename NodeType>
  void DigraphWithSources<NodeType>::clear_sources(node_type c) {
    for (label_type x = 0; x < WordGraph<NodeType>::out_degree(); x++) {
      _preim_init.set(c, x, UNDEFINED);
    }
  }

  // Add d to the list of preimages of c under x, i.e.
  // _word_graph.target(d, x) = c
  template <typename NodeType>
  void DigraphWithSources<NodeType>::add_source(node_type  c,
                                                label_type x,
                                                node_type  d) noexcept {
    LIBSEMIGROUPS_ASSERT(x < WordGraph<NodeType>::out_degree());
    // If d = _preim_init(c, x), then preim_next(d, x) = d, which means that
    // if we try to loop over preimages we'll enter an infinite loop.
    if (d != _preim_init.get(c, x)) {
      // c -> e -> ... -->  c -> d -> e -> ..
      _preim_next.set(d, x, _preim_init.get(c, x));
      _preim_init.set(c, x, d);
    }
  }

  template <typename NodeType>
  template <typename It>
  void DigraphWithSources<NodeType>::rebuild_sources(It first, It last) {
    for (auto it = first; it != last; ++it) {
      clear_sources(*it);
    }

    for (auto it = first; it != last; ++it) {
      node_type c = *it;
      for (label_type x = 0; x < WordGraph<NodeType>::out_degree(); ++x) {
        auto cx = WordGraph<node_type>::target_no_checks(c, x);
        if (cx != UNDEFINED) {
          set_target_no_checks(c, x, cx);
        }
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  // Private member functions
  ////////////////////////////////////////////////////////////////////////////

  template <typename NodeType>
  void DigraphWithSources<NodeType>::remove_source(node_type  cx,
                                                   label_type x,
                                                   node_type  d) {
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
  template <typename NodeType>
  void DigraphWithSources<NodeType>::replace_target(node_type c,
                                                    node_type d,
                                                    size_t    x) {
    node_type e = _preim_init.get(c, x);
    while (e != UNDEFINED) {
      LIBSEMIGROUPS_ASSERT(WordGraph<NodeType>::target_no_checks(e, x) == c);
      WordGraph<NodeType>::set_target_no_checks(e, x, d);
      e = _preim_next.get(e, x);
    }
  }

  template <typename NodeType>
  void DigraphWithSources<NodeType>::replace_source(node_type c,
                                                    node_type d,
                                                    size_t    x,
                                                    node_type cx) {
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
}  // namespace libsemigroups
