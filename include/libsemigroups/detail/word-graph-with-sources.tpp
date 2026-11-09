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

namespace libsemigroups::detail {

  ////////////////////////////////////////////////////////////////////////////
  // Constructors + initializers
  ////////////////////////////////////////////////////////////////////////////

  template <typename Node>
  WordGraphWithSources<Node>::WordGraphWithSources(WordGraph<Node> const& that)
      : WordGraphWithSources() {
    init(that);
  }

  template <typename Node>
  void WordGraphWithSources<Node>::init(WordGraph<Node> const& that) {
    WordGraph<Node>::init(that);
    _preim_init.init(that.out_degree(), that.number_of_nodes(), UNDEFINED);
    _preim_next.init(that.out_degree(), that.number_of_nodes(), UNDEFINED);
    populate_sources_no_checks();
  }

  template <typename Node>
  WordGraphWithSources<Node>::~WordGraphWithSources() = default;

  ////////////////////////////////////////////////////////////////////////
  // WordGraph mem fns with a specialization
  ////////////////////////////////////////////////////////////////////////

  template <typename Node>
  WordGraphWithSources<Node>&
  WordGraphWithSources<Node>::add_nodes(size_type m) {
    WordGraph<node_type>::add_nodes(m);
    _preim_init.add_rows(m);
    _preim_next.add_rows(m);
    return *this;
  }

  template <typename Node>
  WordGraphWithSources<Node>&
  WordGraphWithSources<Node>::induced_subgraph_no_checks(node_type first,
                                                         node_type last) {
    LIBSEMIGROUPS_ASSERT(first <= number_of_nodes());
    LIBSEMIGROUPS_ASSERT(last <= number_of_nodes());
    LIBSEMIGROUPS_ASSERT(first <= last);
    WordGraph<Node>::induced_subgraph_no_checks(first, last);
    _preim_init.shrink_rows_to_no_checks(first, last);
    _preim_next.shrink_rows_to_no_checks(first, last);
    _preim_init.fill(UNDEFINED);
    _preim_next.fill(UNDEFINED);
    populate_sources_no_checks();
    return *this;
  }

  // The vector q is not const because apply_row_permutation_no_copy_no_checks
  // modifies q in order to permute the array in-place without allocating
  // additional memory.
  template <typename Node>
  WordGraphWithSources<Node>&
  WordGraphWithSources<Node>::permute_nodes_no_checks(
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
        WordGraph<Node>::target_no_checks(p[s], a, (t == UNDEFINED ? t : q[t]));
        t = _preim_init.get(p[s], a);
        _preim_init.set(p[s], a, (t == UNDEFINED ? t : q[t]));
        t = _preim_next.get(p[s], a);
        _preim_next.set(p[s], a, (t == UNDEFINED ? t : q[t]));
      }
    }
    // Permute the rows themselves
    dynamic_array2::apply_row_permutation_no_copy_no_checks(
        q, this->_dynamic_array_2, _preim_init, _preim_next);
    return *this;
  }

  template <typename Node>
  WordGraphWithSources<Node>&
  WordGraphWithSources<Node>::remove_target_no_checks(node_type  s,
                                                      label_type a) noexcept {
    LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
    LIBSEMIGROUPS_ASSERT(a < out_degree());
    if (target_no_checks(s, a) != UNDEFINED) {
      remove_source_no_checks(target_no_checks(s, a), a, s);
      WordGraph<node_type>::remove_target_no_checks(s, a);
    }
    return *this;
  }

  template <typename Node>
  WordGraphWithSources<Node>&
  WordGraphWithSources<Node>::target_no_checks(node_type  s,
                                               label_type a,
                                               node_type  t) noexcept {
    LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
    LIBSEMIGROUPS_ASSERT(a < out_degree());
    LIBSEMIGROUPS_ASSERT(t < number_of_nodes());
    node_type const current = target_no_checks(s, a);
    if (t != current) {
      if (current != UNDEFINED) {
        remove_source_no_checks(current, a, s);
      }
      WordGraph<node_type>::target_no_checks(s, a, t);
      add_source_no_checks(t, a, s);
    }
    return *this;
  }

  template <typename Node>
  WordGraphWithSources<Node>& WordGraphWithSources<Node>::target(node_type  s,
                                                                 label_type a,
                                                                 node_type  t) {
    throw_if_not_less(s, number_of_nodes(), "node ");
    throw_if_not_less(a, out_degree(), "label ");
    throw_if_not_less(t, number_of_nodes(), "node ");
    return target_no_checks(s, a, t);
  }

  ////////////////////////////////////////////////////////////////////////
  // WordGraphWithSources specific mem fns
  ////////////////////////////////////////////////////////////////////////

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
    for (auto a : WordGraph<Node>::labels()) {
      node_type m = first_source_no_checks(max, a);
      while (m != UNDEFINED) {
        auto w = next_source_no_checks(m, a);
        LIBSEMIGROUPS_ASSERT(WordGraph<Node>::target_no_checks(m, a) == max);
        WordGraph<Node>::target_no_checks(m, a, min);
        add_source_no_checks(min, a, m);
        new_edge(m, a);
        m = w;
      }

      m = WordGraph<Node>::target_no_checks(max, a);
      if (m != UNDEFINED) {
        remove_source_no_checks(m, a, max);
        node_type const n = WordGraph<Node>::target_no_checks(min, a);
        if (n == UNDEFINED) {
          target_no_checks(min, a, m);
          new_edge(min, a);
        } else {
          num_edges_removed++;
          if (n != m) {
            incompat(n, m);
          }
        }
      }
    }
    return num_edges_removed;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Protected member functions
  ////////////////////////////////////////////////////////////////////////////

  // Add s to the list of preimages of t under a, i.e.
  // _word_graph.target(s, a) = t
  template <typename Node>
  void WordGraphWithSources<Node>::add_source_no_checks(node_type  t,
                                                        label_type a,
                                                        node_type  s) noexcept {
    LIBSEMIGROUPS_ASSERT(t < number_of_nodes());
    LIBSEMIGROUPS_ASSERT(a < out_degree());
    LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
    // If s = _preim_init(t, a), then preim_next(s, a) = s, which means that
    // if we try to loop over preimages we'll enter an infinite loop.
    LIBSEMIGROUPS_ASSERT(s != _preim_init.get(t, a));
    // t -> e -> ... -->  t -> s -> e -> ..
    _preim_next.set(s, a, _preim_init.get(t, a));
    _preim_init.set(t, a, s);
  }

  template <typename Node>
  void WordGraphWithSources<Node>::remove_all_sources_and_targets_no_checks(
      node_type n) {
    LIBSEMIGROUPS_ASSERT(n < number_of_nodes());
    for (auto a : WordGraph<Node>::labels()) {
      WordGraph<Node>::remove_target_no_checks(n, a);
      _preim_init.set(n, a, UNDEFINED);
    }
  }

  template <typename Node>
  void WordGraphWithSources<Node>::remove_all_sources_no_checks(node_type n) {
    LIBSEMIGROUPS_ASSERT(n < number_of_nodes());
    for (auto a : WordGraph<Node>::labels()) {
      _preim_init.set(n, a, UNDEFINED);
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
      auto m = _preim_next.get(n, a);
      while (m != s) {
        n = m;
        m = _preim_next.get(n, a);
      }
      _preim_next.set(n, a, _preim_next.get(s, a));
    }
  }
}  // namespace libsemigroups::detail
