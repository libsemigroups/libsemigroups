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
}  // namespace libsemigroups
