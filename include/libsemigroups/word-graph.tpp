//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains an implementation of word graphs (which are basically
// deterministic automata without initial or accept states).

namespace libsemigroups {
  namespace word_graph {
    //! No doc
    // not noexcept because it throws an exception!
    template <typename Node>
    void validate_node(WordGraph<Node> const& ad, node_type<Node> v) {
      if (v >= ad.number_of_nodes()) {
        LIBSEMIGROUPS_EXCEPTION_V3("node value out of bounds, expected value "
                                   "in the range [0, {}), got {}",
                                   ad.number_of_nodes(),
                                   v);
      }
    }

    //! No doc
    // not noexcept because it throws an exception!
    template <typename Node>
    void validate_label(WordGraph<Node> const& ad, label_type<Node> lbl) {
      if (lbl >= ad.out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("label value out of bounds, expected value in "
                                "the range [0, {}), got {}",
                                ad.out_degree(),
                                lbl);
      }
    }
  }  // namespace word_graph

  template <typename Node>
  WordGraph<Node>::~WordGraph() = default;

  template <typename Node>
  WordGraph<Node>::WordGraph(Node m, Node n)
      : _degree(n),
        _nr_nodes(m),
        _num_active_nodes(),
        _dynamic_array_2(_degree, _nr_nodes, UNDEFINED) {}

  template <typename Node>
  void WordGraph<Node>::init(Node m, Node n) {
    _degree           = n;
    _nr_nodes         = m;
    _num_active_nodes = 0;
    _dynamic_array_2.reshape(n, m);
    remove_all_edges();
  }

  template <typename Node>
  WordGraph<Node>::WordGraph(WordGraph const&) = default;

  template <typename Node>
  template <typename N>
  WordGraph<Node>::WordGraph(WordGraph<N> const& ad)
      : WordGraph(ad.number_of_nodes(), ad.out_degree()) {
    _dynamic_array_2 = ad._dynamic_array_2;
  }

  template <typename Node>
  WordGraph<Node>::WordGraph(WordGraph&&) = default;

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::operator=(WordGraph const&) = default;

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::operator=(WordGraph&&) = default;

  template <typename Node>
  WordGraph<Node> WordGraph<Node>::random(Node         number_of_nodes,
                                          Node         out_degree,
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
  WordGraph<Node> WordGraph<Node>::random(Node         number_of_nodes,
                                          Node         out_degree,
                                          Node         number_of_edges,
                                          std::mt19937 mt) {
    if (number_of_nodes < 2) {
      LIBSEMIGROUPS_EXCEPTION_V3("the 1st parameter `number_of_nodes` must be "
                                 "at least 2, found {}",
                                 number_of_nodes);
    } else if (out_degree < 2) {
      LIBSEMIGROUPS_EXCEPTION_V3("the 2nd parameter `number_of_edges` must be "
                                 "at least 2, found {}",
                                 out_degree);
    } else if (number_of_edges > number_of_nodes * out_degree) {
      LIBSEMIGROUPS_EXCEPTION("the 3rd parameter `number_of_edges` must be at "
                              "most {}, but found {}",
                              number_of_nodes * out_degree,
                              number_of_edges);
    }
    std::uniform_int_distribution<Node> source(0, number_of_nodes - 1);
    std::uniform_int_distribution<Node> target(0, number_of_nodes - 1);
    std::uniform_int_distribution<Node> label(0, out_degree - 1);

    WordGraph<Node> g(number_of_nodes, out_degree);
    size_t          edges_to_add = number_of_edges;
    size_t          old_nr_edges = 0;
    do {
      for (size_t i = 0; i < edges_to_add; ++i) {
        g._dynamic_array_2.set(source(mt), label(mt), target(mt));
      }
      size_t new_nr_edges = g.number_of_edges();
      edges_to_add -= (new_nr_edges - old_nr_edges);
      old_nr_edges = new_nr_edges;
    } while (edges_to_add != 0);
    return g;
  }

  template <typename Node>
  WordGraph<Node> WordGraph<Node>::random_acyclic(Node         number_of_nodes,
                                                  Node         out_degree,
                                                  Node         number_of_edges,
                                                  std::mt19937 mt) {
    if (number_of_nodes < 2) {
      LIBSEMIGROUPS_EXCEPTION_V3("the 1st parameter `number_of_nodes` must be "
                                 "at least 2, found {}",
                                 number_of_nodes);
    } else if (out_degree < 2) {
      LIBSEMIGROUPS_EXCEPTION_V3("the 2nd parameter `number_of_edges` must be "
                                 "at least 2, found {}",
                                 out_degree);
    }
    size_t max_edges = std::min(number_of_nodes * out_degree,
                                number_of_nodes * (number_of_nodes - 1) / 2);

    if (number_of_edges > max_edges) {
      LIBSEMIGROUPS_EXCEPTION_V3(
          "the 3rd parameter `number_of_edges` must be at most {}, but "
          "found {}",
          static_cast<uint64_t>(max_edges),
          static_cast<uint64_t>(number_of_edges));
    }
    std::uniform_int_distribution<Node> source(0, number_of_nodes - 1);
    std::uniform_int_distribution<Node> label(0, out_degree - 1);

    WordGraph<Node> g(number_of_nodes, out_degree);
    size_t          edges_to_add = number_of_edges;
    size_t          old_nr_edges = 0;
    do {
      for (size_t i = 0; i < edges_to_add; ++i) {
        auto v = source(mt);
        if (v != number_of_nodes - 1) {
          g._dynamic_array_2.set(v,
                                 label(mt),
                                 std::uniform_int_distribution<Node>(
                                     v + 1, number_of_nodes - 1)(mt));
        }
      }
      size_t new_nr_edges = g.number_of_edges();
      edges_to_add -= (new_nr_edges - old_nr_edges);
      old_nr_edges = new_nr_edges;
    } while (edges_to_add != 0);
    return g;
  }

  template <typename Node>
  void WordGraph<Node>::add_nodes(size_t nr) {
    if (nr > _dynamic_array_2.number_of_rows() - _nr_nodes) {
      _dynamic_array_2.add_rows(
          nr - (_dynamic_array_2.number_of_rows() - _nr_nodes));
    }
    _nr_nodes += nr;
  }

  template <typename Node>
  void WordGraph<Node>::add_to_out_degree(size_t nr) {
    if (nr > _dynamic_array_2.number_of_cols() - _degree) {
      _dynamic_array_2.add_cols(
          nr - (_dynamic_array_2.number_of_cols() - _degree));
    }
    _degree += nr;
  }

  template <typename Node>
  void WordGraph<Node>::induced_subdigraph(Node first, Node last) {
    _nr_nodes = last - first;
    _dynamic_array_2.shrink_rows_to(first, last);
    if (first != 0) {
      std::for_each(_dynamic_array_2.begin(),
                    _dynamic_array_2.end(),
                    [&first](node_type& x) { x -= first; });
    }
  }

  // TODO improve
  template <typename Node>
  template <typename Iterator>
  void WordGraph<Node>::induced_subdigraph(Iterator first, Iterator last) {
    size_t const N = std::distance(first, last);
    // if (*last == *first + N) {
    //   induced_subdigraph(*first, *last);
    //   return;
    // }

    // TODO do this without allocation
    WordGraph<Node>        copy(N, out_degree());
    std::vector<node_type> old_to_new(number_of_nodes(),
                                      static_cast<node_type>(UNDEFINED));
    node_type              next = 0;

    for (auto nit = first; nit != last; ++nit) {
      if (old_to_new[*nit] == UNDEFINED) {
        old_to_new[*nit] = next;
        next++;
      }
      for (auto eit = cbegin_edges(*nit); eit != cend_edges(*nit); ++eit) {
        if (*eit != UNDEFINED) {
          if (old_to_new[*eit] == UNDEFINED) {
            old_to_new[*eit] = next;
            next++;
          }
          copy.add_edge_no_checks(
              old_to_new[*nit], old_to_new[*eit], eit - cbegin_edges(*nit));
        }
      }
    }
    std::swap(*this, copy);
  }

  template <typename Node>
  std::pair<Node, typename WordGraph<Node>::label_type>
  WordGraph<Node>::next_neighbor_no_checks(
      Node                                 v,
      typename WordGraph<Node>::label_type i) const {
    while (i < out_degree()) {
      node_type u = _dynamic_array_2.get(v, i);
      if (u != UNDEFINED) {
        return std::make_pair(u, i);
      }
      i++;
    }
    return std::make_pair(UNDEFINED, UNDEFINED);
  }

  template <typename Node>
  size_t WordGraph<Node>::number_of_edges() const {
    return _dynamic_array_2.number_of_rows() * _dynamic_array_2.number_of_cols()
           - std::count(
               _dynamic_array_2.cbegin(), _dynamic_array_2.cend(), UNDEFINED);
  }

  template <typename Node>
  void WordGraph<Node>::remove_label_no_checks(label_type lbl) {
    if (lbl == _degree - 1) {
      _degree--;
    } else {
      _dynamic_array_2.erase_column(lbl);
    }
  }

}  // namespace libsemigroups
