//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2026 James D. Mitchell
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

// This file contains the implementation of the class WordGraph.

namespace libsemigroups {

  namespace detail {
    template <typename Node1, typename Node2>
    void throw_if_word_graph_node_out_of_bounds(WordGraph<Node1> const& wg,
                                                Node2                   v) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      if (static_cast<Node1>(v) >= wg.number_of_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value "
                                "in the range [0, {}), got {}",
                                wg.number_of_nodes(),
                                v);
      }
    }

    template <typename Node>
    void throw_if_word_graph_label_out_of_bounds(
        WordGraph<Node> const&               wg,
        typename WordGraph<Node>::label_type lbl) {
      if (lbl >= wg.out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("label value out of bounds, expected value in "
                                "the range [0, {}), got {}",
                                wg.out_degree(),
                                lbl);
      }
    }
  }  // namespace detail

  //////////////////////////////////////////////////////////////////////////////
  // Member functions
  //////////////////////////////////////////////////////////////////////////////

  template <typename Node>
  WordGraph<Node>::~WordGraph() = default;

  template <typename Node>
  WordGraph<Node>::WordGraph(size_type m, size_type n)
      : _dynamic_array_2(n, m, UNDEFINED),
        _degree(n),
        _nr_nodes(m),
        _num_active_nodes() {}

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::init(size_type m, size_type n) {
    // Protected
    _dynamic_array_2.reshape(n, m);
    // Private
    _degree           = n;
    _nr_nodes         = m;
    _num_active_nodes = 0;
    remove_all_targets();
    return *this;
  }

  template <typename Node>
  WordGraph<Node>::WordGraph(WordGraph const&) = default;

  template <typename Node>
  template <typename OtherNode>
  WordGraph<Node>::WordGraph(WordGraph<OtherNode> const& wg)
      : WordGraph(wg.number_of_nodes(), wg.out_degree()) {
    static_assert(sizeof(OtherNode) <= sizeof(Node));
    init(wg);
  }

  template <typename Node>
  template <typename OtherNode>
  WordGraph<Node>& WordGraph<Node>::init(WordGraph<OtherNode> const& wg) {
    static_assert(sizeof(OtherNode) <= sizeof(Node));
    if (wg._dynamic_array_2.default_value()
        == static_cast<OtherNode>(UNDEFINED)) {
      _dynamic_array_2.set_default_value(static_cast<Node>(UNDEFINED));
    }
    _degree           = wg.out_degree();
    _nr_nodes         = wg.number_of_nodes();
    _num_active_nodes = 0;
    _dynamic_array_2.reshape(wg.out_degree(), wg.number_of_nodes());
    std::transform(wg._dynamic_array_2.cbegin(),
                   wg._dynamic_array_2.cend(),
                   _dynamic_array_2.begin(),
                   [](auto const& val) {
                     if (val == static_cast<OtherNode>(UNDEFINED)) {
                       return static_cast<Node>(UNDEFINED);
                     }
                     return static_cast<Node>(val);
                   });
    return *this;
  }

  template <typename Node>
  WordGraph<Node>::WordGraph(WordGraph&&) = default;

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::operator=(WordGraph const&) = default;

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::operator=(WordGraph&&) = default;

  template <typename Node>
  WordGraph<Node> WordGraph<Node>::random(size_type    number_of_nodes,
                                          size_type    out_degree,
                                          std::mt19937 mt) {
    std::uniform_int_distribution<Node> dist(0, number_of_nodes - 1);
    WordGraph<Node>                     g(number_of_nodes, out_degree);
    LIBSEMIGROUPS_ASSERT(g._dynamic_array_2.number_of_rows()
                         == number_of_nodes);
    LIBSEMIGROUPS_ASSERT(g._dynamic_array_2.number_of_cols() == out_degree);
    std::generate(g._dynamic_array_2.begin(),
                  g._dynamic_array_2.end(),
                  [&dist, &mt]() { return dist(mt); });
    return g;
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::add_nodes(size_type nr) {
    if (nr > _dynamic_array_2.number_of_rows() - _nr_nodes) {
      _dynamic_array_2.add_rows(
          nr - (_dynamic_array_2.number_of_rows() - _nr_nodes));
    }
    _nr_nodes += nr;
    return *this;
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::add_to_out_degree(size_t nr) {
    if (nr > _dynamic_array_2.number_of_cols() - _degree) {
      _dynamic_array_2.add_cols(
          nr - (_dynamic_array_2.number_of_cols() - _degree));
    }
    _degree += nr;
    return *this;
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::induced_subgraph_no_checks(Node first,
                                                               Node last) {
    _nr_nodes = last - first;
    _dynamic_array_2.shrink_rows_to_no_checks(first, last);
    if (first != 0) {
      std::for_each(_dynamic_array_2.begin(),
                    _dynamic_array_2.end(),
                    [first](node_type& x) { x -= first; });
    }
    return *this;
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::induced_subgraph(Node first, Node last) {
    detail::throw_if_word_graph_node_out_of_bounds(*this, first);
    if (last != number_of_nodes()) {
      detail::throw_if_word_graph_node_out_of_bounds(*this, last);
    }

    for (node_type s = first; s < last; ++s) {
      for (auto [a, t] : labels_and_targets_no_checks(s)) {
        if ((t >= last || t < first) && t != UNDEFINED) {
          LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value "
                                  "in the range [{}, {}), got {}",
                                  first,
                                  last,
                                  t);
        }
      }
    }
    return induced_subgraph_no_checks(first, last);
  }

  template <typename Node>
  template <typename Iterator, typename>
  WordGraph<Node>& WordGraph<Node>::induced_subgraph_no_checks(Iterator first,
                                                               Iterator last) {
    size_t const N = std::distance(first, last);

    // TODO(1) do this without allocation
    WordGraph<Node>        copy(N, out_degree());
    std::vector<node_type> old_to_new(number_of_nodes(),
                                      static_cast<node_type>(UNDEFINED));
    node_type              next = 0;

    for (auto n = first; n != last; ++n) {
      if (old_to_new[*n] == UNDEFINED) {
        old_to_new[*n] = next;
        next++;
      }
      for (auto [a, t] : labels_and_targets_no_checks(*n)) {
        if (t != UNDEFINED) {
          if (old_to_new[t] == UNDEFINED) {
            old_to_new[t] = next;
            next++;
          }
          copy.target_no_checks(old_to_new[*n], a, old_to_new[t]);
        }
      }
    }
    std::swap(*this, copy);
    return *this;
  }

  template <typename Node>
  template <typename Iterator, typename>
  WordGraph<Node>& WordGraph<Node>::induced_subgraph(Iterator first,
                                                     Iterator last) {
    std::for_each(first, last, [this](auto n) {
      detail::throw_if_word_graph_node_out_of_bounds(*this, n);
    });
    for (auto it = first; it != last; ++it) {
      auto s = *it;
      for (auto [a, t] : labels_and_targets_no_checks(s)) {
        if (t != UNDEFINED && std::find(first, last, t) == last) {
          LIBSEMIGROUPS_EXCEPTION(
              "target out of bounds, the edge with source {} and label {} "
              "has target {}, which does not belong to the specified range",
              s,
              a,
              t);
        }
      }
    }
    return induced_subgraph_no_checks(first, last);
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::disjoint_union_inplace_no_checks(
      WordGraph<Node> const& that) {
    if (that.number_of_nodes() == 0) {
      return *this;
    }
    size_t old_num_nodes = number_of_nodes();
    _dynamic_array_2.append(that._dynamic_array_2);
    auto first = _dynamic_array_2.begin_row(old_num_nodes);
    auto last  = _dynamic_array_2.end();
    std::for_each(
        first, last, [old_num_nodes](node_type& n) { n += old_num_nodes; });
    return *this;
  }

  template <typename Node>
  WordGraph<Node>&
  WordGraph<Node>::disjoint_union_inplace(WordGraph<Node> const& that) {
    if (this == &that) {
      LIBSEMIGROUPS_EXCEPTION("the argument <that> (a word graph) must be a "
                              "distinct object from <*this>");
    }
    if (out_degree() != that.out_degree()) {
      LIBSEMIGROUPS_EXCEPTION("expected word graphs with equal out-degrees "
                              "but found {} != {}",
                              that.out_degree(),
                              out_degree());
    }
    return disjoint_union_inplace_no_checks(that);
  }

  template <typename Node>
  WordGraph<Node>&
  WordGraph<Node>::permute_nodes_no_checks(std::vector<node_type> const& p,
                                           std::vector<node_type> const& q,
                                           size_t                        m) {
    LIBSEMIGROUPS_ASSERT(m <= p.size());
    // p : new -> old, q = p ^ -1: old -> new
    for (node_type s = 0; s < m; ++s) {
      for (auto [a, t] : labels_and_targets_no_checks(p[s])) {
        target_no_checks(p[s], a, (t == UNDEFINED ? t : q[t]));
      }
    }
    // Permute the rows themselves
    detail::dynamic_array2::apply_row_permutation_no_checks(q,
                                                            _dynamic_array_2);
    return *this;
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::target(node_type  m,
                                           label_type lbl,
                                           node_type  n) {
    detail::throw_if_word_graph_node_out_of_bounds(*this, m);
    detail::throw_if_word_graph_label_out_of_bounds(*this, lbl);
    detail::throw_if_word_graph_node_out_of_bounds(*this, n);
    target_no_checks(m, lbl, n);
    return *this;
  }

  template <typename Node>
  std::pair<typename WordGraph<Node>::label_type, Node>
  WordGraph<Node>::next_label_and_target_no_checks(
      Node                                 s,
      typename WordGraph<Node>::label_type a) const {
    while (a < out_degree()) {
      node_type t = _dynamic_array_2.get(s, a);
      if (t != UNDEFINED) {
        return std::make_pair(a, t);
      }
      a++;
    }
    return std::make_pair(UNDEFINED, UNDEFINED);
  }

  template <typename Node>
  std::pair<Node, typename WordGraph<Node>::label_type>
  WordGraph<Node>::next_label_and_target(node_type s, label_type a) const {
    detail::throw_if_word_graph_node_out_of_bounds(*this, s);
    return next_label_and_target_no_checks(s, a);
  }

  template <typename Node>
  size_t WordGraph<Node>::number_of_edges() const {
    return _dynamic_array_2.number_of_rows() * _dynamic_array_2.number_of_cols()
           - std::count(_dynamic_array_2.cbegin(),
                        _dynamic_array_2.cend(),
                        static_cast<Node>(UNDEFINED));
  }

  template <typename Node>
  size_t WordGraph<Node>::number_of_edges(node_type n) const {
    detail::throw_if_word_graph_node_out_of_bounds(*this, n);
    return number_of_edges_no_checks(n);
  }

  template <typename Node>
  size_t WordGraph<Node>::number_of_edges_no_checks(node_type n) const {
    return out_degree()
           - std::count(_dynamic_array_2.cbegin_row(n),
                        _dynamic_array_2.cend_row(n),
                        UNDEFINED);
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::remove_target(node_type s, label_type a) {
    detail::throw_if_word_graph_node_out_of_bounds(*this, s);
    detail::throw_if_word_graph_label_out_of_bounds(*this, a);
    return remove_target_no_checks(s, a);
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::remove_label_no_checks(label_type lbl) {
    if (lbl == _degree - 1) {
      _degree--;
    } else {
      _dynamic_array_2.erase_column(lbl);
    }
    return *this;
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::remove_label(label_type lbl) {
    detail::throw_if_word_graph_label_out_of_bounds(*this, lbl);
    return remove_label_no_checks(lbl);
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::reserve(size_type m, size_type n) {
    if (n > _dynamic_array_2.number_of_cols()) {
      _dynamic_array_2.add_cols(n - _dynamic_array_2.number_of_cols());
    }
    if (m > _dynamic_array_2.number_of_rows()) {
      _dynamic_array_2.add_rows(m - _dynamic_array_2.number_of_rows());
    }
    return *this;
  }

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::swap_targets(node_type  u,
                                                 node_type  v,
                                                 label_type a) {
    detail::throw_if_word_graph_node_out_of_bounds(*this, u);
    detail::throw_if_word_graph_node_out_of_bounds(*this, v);
    detail::throw_if_word_graph_label_out_of_bounds(*this, a);
    return swap_targets_no_checks(u, v, a);
  }

  template <typename Node>
  Node WordGraph<Node>::target(node_type v, label_type lbl) const {
    detail::throw_if_word_graph_node_out_of_bounds(*this, v);
    detail::throw_if_word_graph_label_out_of_bounds(*this, lbl);
    return _dynamic_array_2.get(v, lbl);
  }

  template <typename Node>
  typename WordGraph<Node>::const_iterator_targets
  WordGraph<Node>::cbegin_targets(node_type i) const {
    detail::throw_if_word_graph_node_out_of_bounds(*this, i);
    return cbegin_targets_no_checks(i);
  }

  template <typename Node>
  typename WordGraph<Node>::const_iterator_targets
  WordGraph<Node>::cend_targets(node_type i) const {
    detail::throw_if_word_graph_node_out_of_bounds(*this, i);
    return cend_targets_no_checks(i);
  }

  template <typename Node>
  rx::iterator_range<typename WordGraph<Node>::const_iterator_targets>
  WordGraph<Node>::targets(node_type n) const {
    detail::throw_if_word_graph_node_out_of_bounds(*this, n);
    return targets_no_checks(n);
  }

  template <typename Node>
  auto WordGraph<Node>::labels_and_targets(node_type n) const {
    detail::throw_if_word_graph_node_out_of_bounds(*this, n);
    return labels_and_targets_no_checks(n);
  }

}  // namespace libsemigroups
