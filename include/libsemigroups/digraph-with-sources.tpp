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

namespace libsemigroups {

  // number of nodes, out-degree
  template <typename T>
  void DigraphWithSources<T>::init(size_type m, size_type n) {
    ActionDigraph<node_type>::init(m, n);
    _preim_init.reshape(n, m);
    _preim_next.reshape(n, m);
  }

  template <typename T>
  void DigraphWithSources<T>::remove_source(node_type   cx,
                                            letter_type x,
                                            node_type   d) {
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

  template <typename T>
  void DigraphWithSources<T>::clear_sources_and_targets(node_type c) {
    for (letter_type i = 0; i < this->out_degree(); i++) {
      ActionDigraph<T>::add_edge_nc(c, UNDEFINED, i);
      _preim_init.set(c, i, UNDEFINED);
    }
  }

  template <typename T>
  void DigraphWithSources<T>::clear_sources(node_type c) {
    for (letter_type i = 0; i < this->out_degree(); i++) {
      _preim_init.set(c, i, UNDEFINED);
    }
  }

  // All edges of the form e - x -> c are replaced with e - x -> d,
  template <typename T>
  void DigraphWithSources<T>::replace_target(node_type c,
                                             node_type d,
                                             size_t    x) {
    node_type e = _preim_init.get(c, x);
    while (e != UNDEFINED) {
      LIBSEMIGROUPS_ASSERT(this->unsafe_neighbor(e, x) == c);
      ActionDigraph<T>::add_edge_nc(e, d, x);
      e = _preim_next.get(e, x);
    }
  }

  template <typename T>
  void DigraphWithSources<T>::replace_source(node_type c,
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

  template <typename T>
  void DigraphWithSources<T>::permute_nodes_nc(std::vector<node_type>& p,
                                               std::vector<node_type>& q,
                                               size_t                  m) {
    // p : new -> old, q = p ^ -1
    node_type    c = 0;
    size_t const n = this->out_degree();
    // Permute all the values in the _table, and pre-images, that relate
    // to active cosets
    while (c < m) {
      for (letter_type x = 0; x < n; ++x) {
        node_type i = this->unsafe_neighbor(p[c], x);
        ActionDigraph<T>::add_edge_nc(p[c], (i == UNDEFINED ? i : q[i]), x);
        i = _preim_init.get(p[c], x);
        _preim_init.set(p[c], x, (i == UNDEFINED ? i : q[i]));
        i = _preim_next.get(p[c], x);
        _preim_next.set(p[c], x, (i == UNDEFINED ? i : q[i]));
      }
      c++;
    }
    // Permute the rows themselves
    ActionDigraph<T>::apply_row_permutation(p);
    _preim_init.apply_row_permutation(p);
    _preim_next.apply_row_permutation(p);
  }

  template <typename T>
  void DigraphWithSources<T>::swap_nodes(node_type c, node_type d) {
    size_t const n = this->out_degree();
    for (letter_type x = 0; x < n; ++x) {
      node_type cx = this->unsafe_neighbor(c, x);
      node_type dx = this->unsafe_neighbor(d, x);
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
      this->swap_edges_nc(c, d, x);
      _preim_init.swap(c, x, d, x);
      _preim_next.swap(c, x, d, x);
    }
  }

  // c must be valid, d may not be
  template <typename T>
  void DigraphWithSources<T>::rename_node(node_type c, node_type d) {
    size_t const n = this->out_degree();

    for (letter_type x = 0; x < n; ++x) {
      node_type cx = this->unsafe_neighbor(c, x);
      replace_target(c, d, x);
      replace_source(c, d, x, cx);
      this->swap_edges_nc(c, d, x);
      _preim_init.swap(c, x, d, x);
      _preim_next.swap(c, x, d, x);
    }
  }

  template <typename T>
  template <typename NewEdgeFunc, typename IncompatibleFunc>
  void DigraphWithSources<T>::merge_nodes(node_type          min,
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

  template <typename T>
  void DigraphWithSources<T>::quotient_digraph(detail::Duf<> uf) {
    using node_type  = typename DigraphWithSources<T>::node_type;
    using label_type = typename DigraphWithSources<T>::label_type;
    auto N           = this->number_of_nodes();
    auto n_out       = this->out_degree();
    auto M           = uf.number_of_blocks();

    // Check that uf and d match
    if (N == 0) {
      if (uf.empty()) {
        return;
      } else {
        LIBSEMIGROUPS_EXCEPTION("Empty digraph but non-empty partition. "
                                "Expected empty partition.");
      }
    } else if (uf.size() != N) {
      LIBSEMIGROUPS_EXCEPTION("Union find and digraph have a different number "
                              "of nodes. Expected union find size and number "
                              "of digraph nodes to be equal.");
    }

    // Check if partition is trivial or discrete
    if (M == N) {
      return;
    } else if (M == 1) {
      this->init(1, n_out);
      for (label_type a = 0; a < n_out; ++a) {
        this->add_edge_nc(0, 0, a);
      }
      return;
    }

    std::stack<std::pair<node_type, node_type>> coincidences;

    // Make pairs of vertices that lie in the same part of a union find object
    for (node_type i = 0; i < uf.size(); ++i) {
      node_type j = uf.find(i);
      if (i != j) {
        coincidences.emplace(i, j);
      }
    }

    // For each coincidence (x, y), unite each out neighbour of x with the
    // corresponding out neighbour of y
    while (!coincidences.empty()) {
      node_type x, y, x_nb, y_nb, x_nb_rep, y_nb_rep;
      std::tie(x, y) = coincidences.top();
      coincidences.pop();
      for (label_type a = 0; a < n_out; ++a) {
        x_nb = this->unsafe_neighbor(x, a);
        y_nb = this->unsafe_neighbor(y, a);

        if (x_nb != UNDEFINED && y_nb != UNDEFINED) {
          x_nb_rep = uf.find(x_nb);
          y_nb_rep = uf.find(y_nb);
          if (x_nb_rep != y_nb_rep) {
            coincidences.emplace(x_nb_rep, y_nb_rep);
          }
        }
        // Handle missing edges
        else if (x_nb == UNDEFINED && y_nb == UNDEFINED) {
          continue;
        } else if (x_nb == UNDEFINED) {
          y_nb_rep = uf.find(y_nb);
          ActionDigraph<T>::add_edge_nc(x, y_nb_rep, a);
          continue;
        } else {
          x_nb_rep = uf.find(x_nb);
          ActionDigraph<T>::add_edge_nc(y, x_nb_rep, a);
          continue;
        }
      }
      uf.unite(x, y);
    }

    uf.normalize();

    // Populate representative row with representative out-neighbours
    for (auto it = uf.cbegin(); it < uf.cend(); ++it) {
      for (label_type a = 0; a != n_out; ++a) {
        node_type va = this->unsafe_neighbor(*it, a);
        if (va != UNDEFINED) {
          node_type va_rep = uf.find(va);
          if (va != va_rep) {
            this->add_edge_nc(*it, va_rep, a);
          }
        }
      }
    }

    // Identify the representative for each part in uf with a number in 0 ...
    // number_of_blocks
    node_type index = 0;
    for (auto it = uf.cbegin(); it < uf.cend(); ++it) {
      this->rename_node(*it, index);
      ++index;
    }

    // Alternatively, don't normalise the uf and uncomment the below.
    // for (node_type v = 0; v < N; ++v) {
    //   node_type rep = uf.find(v);
    //   if (v == rep) {
    //     this->rename_node(rep, index);
    //     ++index;
    //   }
    // }

    this->restrict(uf.number_of_blocks());
  }

  template <typename T>
  void
  DigraphWithSources<T>::hopcroft_karp_quotient(DigraphWithSources<T> const& d2,
                                                node_type                    p0,
                                                node_type q0) {
    auto uf = HopcroftKarp()(*this, p0, d2, q0);
    uf.restrict(this->ActionDigraph<T>::number_of_nodes());
    this->quotient_digraph(uf);
  }

  template <typename T>
  DigraphWithSources<T> DigraphWithSources<T>::get_quotient(detail::Duf<> uf) {
    auto d2 = *this;
    d2.quotient_digraph(uf);
    return d2;
  }

  template <typename T>
  bool DigraphWithSources<T>::is_valid() const {
    using node_type  = typename DigraphWithSources<T>::node_type;
    using label_type = typename DigraphWithSources<T>::label_type;
    auto num_nodes   = this->number_of_nodes();
    auto num_labels  = this->out_degree();
    for (node_type v = 0; v < num_nodes; ++v) {
      for (label_type a = 0; a < num_labels; ++a) {
        node_type va = this->unsafe_neighbor(v, a);
        if (va != UNDEFINED && not this->is_source(va, v, a)) {
          return false;
        }
      }
    }
    return true;
  }

  template <typename T>
  bool DigraphWithSources<T>::is_source(node_type   c,
                                        node_type   d,
                                        letter_type x) const {
    c = first_source(c, x);
    while (c != d && c != UNDEFINED) {
      c = next_source(c, x);
    }
    return c == d;
  }

  template <typename T>
  void DigraphWithSources<T>::add_source(node_type   c,
                                         letter_type x,
                                         node_type   d) noexcept {
    LIBSEMIGROUPS_ASSERT(x < this->out_degree());
    // If d = _preim_init(c, x), then preim_next(d, x) = d, which means that
    // if we try to loop over preimages we'll enter an infinite loop.
    if (d != _preim_init.get(c, x)) {
      // c -> e -> ... -->  c -> d -> e -> ..
      _preim_next.set(d, x, _preim_init.get(c, x));
      _preim_init.set(c, x, d);
    }
  }

  namespace digraph_with_sources {
    template <typename T>
    bool standardize(T& d, Forest& f) {
      // TODO(later): should be DigraphWithSourcesBase
      static_assert(
          std::is_base_of<ActionDigraphBase, T>::value,
          "the template parameter T must be derived from ActionDigraphBase");
      using node_type = typename T::node_type;
      if (!f.empty()) {
        f.clear();
      }
      if (d.number_of_nodes() == 0) {
        return false;
      }

      f.add_nodes(1);

      node_type    t      = 0;
      size_t const n      = d.out_degree();
      bool         result = false;

      for (node_type s = 0; s <= t; ++s) {
        for (letter_type x = 0; x < n; ++x) {
          node_type const r = d.unsafe_neighbor(s, x);
          if (r != UNDEFINED) {
            if (r > t) {
              t++;
              f.add_nodes(1);
              if (r > t) {
                d.swap_nodes(t, r);
                result = true;
              }
              f.set(t, (s == t ? r : s), x);
            }
          }
        }
      }
      return result;
    }

    template <typename T>
    std::pair<bool, Forest> standardize(T& d) {
      static_assert(
          std::is_base_of<ActionDigraphBase, T>::value,
          "the template parameter T must be derived from ActionDigraphBase");
      Forest f;
      bool   result = standardize(d, f);
      return std::make_pair(result, f);
    }
  }  // namespace digraph_with_sources
}  // namespace libsemigroups
