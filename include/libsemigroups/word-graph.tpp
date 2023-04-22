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
  //////////////////////////////////////////////////////////////////////////////
  // Helper namespace
  //////////////////////////////////////////////////////////////////////////////

  namespace word_graph {
    namespace detail {
      template <typename Node>
      bool shortlex_standardize(Node& d, Forest& f) {
        LIBSEMIGROUPS_ASSERT(d.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type = typename Node::node_type;
        f.add_nodes(1);

        node_type    t      = 0;
        size_t const n      = d.out_degree();
        bool         result = false;

        // p : new -> old and q : old -> new
        std::vector<node_type> p(d.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        for (node_type s = 0; s <= t; ++s) {
          for (letter_type x = 0; x < n; ++x) {
            node_type r = d.neighbor_no_checks(p[s], x);
            if (r != UNDEFINED) {
              r = q[r];  // new
              if (r > t) {
                t++;
                f.add_nodes(1);
                if (r > t) {
                  std::swap(p[t], p[r]);
                  std::swap(q[p[t]], q[p[r]]);
                  result = true;
                }
                f.set(t, (s == t ? r : s), x);
              }
            }
          }
        }
        d.permute_nodes_no_checks(p, q);
        return result;
      }

      template <typename Node>
      bool lex_standardize(Node& d, Forest& f) {
        LIBSEMIGROUPS_ASSERT(d.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type  = typename Node::node_type;
        using label_type = typename Node::label_type;

        f.add_nodes(1);

        node_type  s = 0, t = 0;
        label_type x      = 0;
        auto const n      = d.out_degree();
        bool       result = false;

        // p : new -> old and q : old -> new
        std::vector<node_type> p(d.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        // Perform a DFS through d
        while (s <= t) {
          node_type r = d.neighbor_no_checks(p[s], x);
          if (r != UNDEFINED) {
            r = q[r];  // new
            if (r > t) {
              t++;
              f.add_nodes(1);
              if (r > t) {
                std::swap(p[t], p[r]);
                std::swap(q[p[t]], q[p[r]]);
                result = true;
              }
              f.set(t, (s == t ? r : s), x);
              s = t;
              x = 0;
              continue;
            }
          }
          x++;
          if (x == n) {  // backtrack
            x = f.label(s);
            s = f.parent(s);
          }
        }
        d.permute_nodes_no_checks(p, q);
        return result;
      }

      template <typename Node>
      bool recursive_standardize(Node& d, Forest& f) {
        LIBSEMIGROUPS_ASSERT(d.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type = typename Node::node_type;

        f.add_nodes(1);

        std::vector<word_type> words;
        size_t const           n = d.out_degree();
        letter_type            a = 0;
        node_type              s = 0, t = 0;

        std::vector<node_type> p(d.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        size_t max_t = number_of_nodes_reachable_from(d, 0) - 1;

        // TODO move this out of here and use it in the other standardize
        // functions
        auto swap_if_necessary = [&d, &f, &p, &q](node_type const   s,
                                                  node_type&        t,
                                                  letter_type const x) {
          node_type r      = d.neighbor_no_checks(p[s], x);
          bool      result = false;
          if (r != UNDEFINED) {
            r = q[r];  // new
            if (r > t) {
              t++;
              f.add_nodes(1);
              if (r > t) {
                std::swap(p[t], p[r]);
                std::swap(q[p[t]], q[p[r]]);
              }
              result = true;
              f.set(t, (s == t ? r : s), x);
            }
          }
          return result;
        };

        bool result = false;

        while (s <= t) {
          if (swap_if_necessary(s, t, 0)) {
            words.push_back(word_type(t, a));
            result = true;
          }
          s++;
        }
        a++;
        bool new_generator = true;
        int  x, u, w;
        while (a < n && t < max_t) {
          if (new_generator) {
            w = -1;  // -1 is the empty word
            if (swap_if_necessary(0, t, a)) {
              result = true;
              words.push_back({a});
            }
            x             = words.size() - 1;
            u             = words.size() - 1;
            new_generator = false;
          }

          node_type const uu = word_graph::follow_path_no_checks(
              d, 0, words[u].begin(), words[u].end());
          if (uu != UNDEFINED) {
            for (int v = 0; v < x; v++) {
              node_type const uuv = word_graph::follow_path_no_checks(
                  d, uu, words[v].begin(), words[v].end() - 1);
              if (uuv != UNDEFINED) {
                s = q[uuv];
                if (swap_if_necessary(s, t, words[v].back())) {
                  result        = true;
                  word_type nxt = words[u];
                  nxt.insert(nxt.end(), words[v].begin(), words[v].end());
                  words.push_back(std::move(nxt));
                }
              }
            }
          }
          w++;
          if (static_cast<size_t>(w) < words.size()) {
            node_type const ww = word_graph::follow_path_no_checks(
                d, 0, words[w].begin(), words[w].end());
            if (ww != UNDEFINED) {
              s = q[ww];
              if (swap_if_necessary(s, t, a)) {
                result        = true;
                u             = words.size();
                word_type nxt = words[w];
                nxt.push_back(a);
                words.push_back(std::move(nxt));
              }
            }
          } else {
            a++;
            new_generator = true;
          }
        }
        d.permute_nodes_no_checks(p, q);
        return result;
      }

      // Helper function for the two versions of is_acyclic below.
      // Not noexcept because std::stack::emplace isn't
      // This function does not really need to exist any longer, since
      // topological_sort can be used for the same computation, but we retain
      // it because it was already written and uses less space than
      // topological_sort.
      template <typename Node>
      bool is_acyclic(WordGraph<Node> const& ad,
                      std::stack<Node>&      stck,
                      std::vector<Node>&     preorder,
                      Node&                  next_preorder_num,
                      std::vector<Node>&     postorder,
                      Node&                  next_postorder_num) {
        size_t const M = ad.out_degree();
        size_t const N = ad.number_of_nodes();
        Node         v;
        while (!stck.empty()) {
          v = stck.top();
          stck.pop();
          if (v >= N) {
            postorder[v - N] = next_postorder_num++;
          } else {
            if (preorder[v] < next_preorder_num && postorder[v] == N) {
              // v is an ancestor of some vertex later in the search
              return false;
            } else if (preorder[v] == N) {
              // not seen v before
              preorder[v] = next_preorder_num++;
              // acts as a divider, so that we know when we've stopped
              // processing the out-neighbours of v
              stck.push(N + v);
              for (size_t label = 0; label < M; ++label) {
                auto w = ad.neighbor_no_checks(v, label);
                if (w != UNDEFINED) {
                  stck.push(w);
                }
              }
            }
          }
        }
        return true;
      }
      template <typename Node>
      using stack_type = std::stack<std::pair<Node, label_type<Node>>>;

      using lookup_type = std::vector<uint8_t>;

      // helper function for the public functions below
      template <typename Node>
      bool topological_sort(WordGraph<Node> const& ad,
                            stack_type<Node>&      stck,
                            lookup_type&           seen,
                            std::vector<Node>&     order) {
        using label_type = label_type<Node>;
        Node       m;
        Node       n;
        label_type e;
      dive:
        LIBSEMIGROUPS_ASSERT(!stck.empty());
        LIBSEMIGROUPS_ASSERT(seen[stck.top().first] == 0);
        m       = stck.top().first;
        seen[m] = 2;
        e       = 0;
        do {
        rise:
          std::tie(n, e) = ad.next_neighbor_no_checks(m, e);
          if (n != UNDEFINED) {
            if (seen[n] == 0) {
              // never saw this node before, so dive
              stck.emplace(n, 0);
              goto dive;
            } else if (seen[n] == 1) {
              // => all descendants of n prev. explored and no cycles found
              // => try the next neighbour of m.
              ++e;
            } else {
              LIBSEMIGROUPS_ASSERT(seen[n] == 2);
              // => n is an ancestor and a descendant of m
              // => there's a cycle
              order.clear();
              return false;
            }
          }
        } while (e < ad.out_degree());
        // => all descendants of m were explored, and no cycles were found
        // => backtrack
        seen[m] = 1;
        order.push_back(m);
        stck.pop();
        if (stck.size() == 0) {
          return true;
        } else {
          m = stck.top().first;
          e = stck.top().second;
          goto rise;
        }
      }
    }  // namespace detail

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

    template <typename Graph>
    std::pair<bool, Forest> standardize(Graph& d, Order val) {
      static_assert(
          std::is_base_of<WordGraphBase, Graph>::value,
          "the template parameter Graph must be derived from WordGraphBase");
      Forest f;
      bool   result = standardize(d, f, val);
      return std::make_pair(result, f);
    }

    template <typename Graph>
    bool standardize(Graph& d, Forest& f, Order val) {
      static_assert(std::is_base_of_v<WordGraphBase, Graph>,
                    "the template parameter Graph must be "
                    "derived from WordGraphBase");
      if (!f.empty()) {
        f.clear();
      }
      if (d.number_of_nodes() == 0) {
        return false;
      }

      switch (val) {
        case Order::none:
          return false;
        case Order::shortlex:
          return detail::shortlex_standardize(d, f);
        case Order::lex:
          return detail::lex_standardize(d, f);
        case Order::recursive:
          return detail::recursive_standardize(d, f);
        default:
          return false;
      }
    }

    template <typename Node, typename Iterator>
    void add_cycle(WordGraph<Node>& ad, Iterator first, Iterator last) {
      for (auto it = first; it < last - 1; ++it) {
        ad.add_edge(*it, *(it + 1), 0);
      }
      ad.add_edge(*(last - 1), *first, 0);
    }

    template <typename Node,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3>
    bool is_compatible(WordGraph<Node> const& d,
                       Iterator1              first_node,
                       Iterator2              last_node,
                       Iterator3              first_rule,
                       Iterator3              last_rule) {
      for (auto nit = first_node; nit != last_node; ++nit) {
        for (auto rit = first_rule; rit != last_rule; ++rit) {
          auto l = word_graph::follow_path_no_checks(
              d, *nit, rit->cbegin(), rit->cend());
          if (l == UNDEFINED) {
            return true;
          }
          ++rit;
          auto r = word_graph::follow_path_no_checks(
              d, *nit, rit->cbegin(), rit->cend());
          if (r == UNDEFINED) {
            return true;
          }
          if (l != r) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Node, typename Iterator1, typename Iterator2>
    bool is_complete(WordGraph<Node> const& d,
                     Iterator1              first_node,
                     Iterator2              last_node) {
      size_t const n = d.out_degree();
      for (auto it = first_node; it != last_node; ++it) {
        for (size_t a = 0; a < n; ++a) {
          if (d.neighbor_no_checks(*it, a) == UNDEFINED) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Node>
    bool is_strictly_cyclic(WordGraph<Node> const& ad) {
      using node_type = typename WordGraph<Node>::node_type;
      auto const N    = ad.number_of_nodes();

      if (N == 0) {
        return true;
      }

      std::vector<bool> seen(N, false);
      std::stack<Node>  stack;

      for (node_type m = 0; m < N; ++m) {
        stack.push(m);
        size_t count = 0;
        while (!stack.empty()) {
          auto n = stack.top();
          stack.pop();
          if (!seen[n]) {
            seen[n] = true;
            if (++count == N) {
              return true;
            }
            for (auto it = ad.cbegin_edges(n); it != ad.cend_edges(n); ++it) {
              if (*it != UNDEFINED) {
                stack.push(*it);
              }
            }
          }
        }
        std::fill(seen.begin(), seen.end(), false);
      }
      return false;
    }

    template <typename Node>
    bool is_connected(WordGraph<Node> const& ad) {
      using node_type = typename WordGraph<Node>::node_type;

      auto const N = ad.number_of_nodes();
      if (N == 0) {
        return true;
      }

      ::libsemigroups::detail::Duf<> uf(N);
      for (node_type n = 0; n < N; ++n) {
        for (auto it = ad.cbegin_edges(n); it != ad.cend_edges(n); ++it) {
          if (*it != UNDEFINED) {
            uf.unite(n, *it);
          }
        }
      }
      return uf.number_of_blocks() == 1;
    }

    template <typename Node1, typename Node2>
    bool is_reachable(WordGraph<Node1> const& ad, Node2 source, Node2 target) {
      using label_type = typename WordGraph<Node1>::label_type;
      validate_node(ad, source);
      validate_node(ad, target);
      if (source == target) {
        return true;
      }
      label_type             edge = 0;
      std::stack<Node1>      nodes;
      std::stack<label_type> edges;
      std::vector<bool>      seen(ad.number_of_nodes(), false);
      nodes.push(source);
      seen[source] = true;

      do {
        Node1 node;
        std::tie(node, edge) = ad.next_neighbor_no_checks(nodes.top(), edge);
        if (node == static_cast<Node1>(target)) {
          return true;
        } else if (node != UNDEFINED) {
          if (!seen[node]) {
            // dive, dive, dive!!
            seen[node] = true;
            nodes.push(node);
            edges.push(edge);
            edge = 0;
          } else {
            ++edge;
          }
        } else {
          // backtrack
          nodes.pop();
          if (!edges.empty()) {
            edge = edges.top();
            edges.pop();
          }
        }
      } while (!nodes.empty());
      return false;
    }

    template <typename Node>
    bool is_acyclic(WordGraph<Node> const& ad) {
      if (ad.validate()) {
        return false;
      }
      auto const        N = ad.number_of_nodes();
      std::stack<Node>  stck;
      std::vector<Node> preorder(N, N);
      Node              next_preorder_num = 0;
      std::vector<Node> postorder(N, N);
      Node              next_postorder_num = 0;

      for (auto m : ad.nodes()) {
        if (preorder[m] == N) {
          stck.push(m);
          if (!detail::is_acyclic(ad,
                                  stck,
                                  preorder,
                                  next_preorder_num,
                                  postorder,
                                  next_postorder_num)) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Node1, typename Node2>
    bool is_acyclic(WordGraph<Node1> const& ad, Node2 source) {
      validate_node(ad, source);
      auto const        N = ad.number_of_nodes();
      std::stack<Node1> stck;
      stck.push(source);
      std::vector<Node1> preorder(N, N);
      Node1              next_preorder_num = 0;
      std::vector<Node1> postorder(N, N);
      Node1              next_postorder_num = 0;
      return detail::is_acyclic(
          ad, stck, preorder, next_preorder_num, postorder, next_postorder_num);
    }

    template <typename Node1, typename Node2>
    bool is_acyclic(WordGraph<Node1> const& ad, Node2 source, Node2 target) {
      validate_node(ad, source);
      validate_node(ad, target);
      if (!is_reachable(ad, source, target)) {
        return true;
      }
      auto const        N = ad.number_of_nodes();
      std::stack<Node1> stck;
      stck.push(source);
      std::vector<Node1> preorder(N, N);
      Node1              next_preorder_num = 0;
      std::vector<Node1> postorder(N, N);
      Node1              next_postorder_num = 0;
      // TODO(later) there should be a better way of doing this
      for (auto it = ad.cbegin_nodes(); it != ad.cend_nodes(); ++it) {
        if (!is_reachable(ad, *it, static_cast<Node1>(target))) {
          preorder[*it] = N + 1;
        }
      }
      return detail::is_acyclic(
          ad, stck, preorder, next_preorder_num, postorder, next_postorder_num);
    }

    template <typename Node>
    std::vector<Node> topological_sort(WordGraph<Node> const& ad) {
      std::vector<Node> order;
      if (ad.validate()) {
        return order;
      }

      size_t const             N = ad.number_of_nodes();
      detail::stack_type<Node> stck;
      std::vector<uint8_t>     seen(N, 0);

      for (auto m : ad.nodes()) {
        if (seen[m] == 0) {
          stck.emplace(m, 0);
          if (!detail::topological_sort(ad, stck, seen, order)) {
            // digraph is not acyclic and so there's no topological order for
            // the nodes.
            LIBSEMIGROUPS_ASSERT(order.empty());
            return order;
          }
        }
      }
      LIBSEMIGROUPS_ASSERT(order.size() == ad.number_of_nodes());
      return order;
    }

    template <typename Node1, typename Node2>
    std::vector<Node1> topological_sort(WordGraph<Node1> const& ad,
                                        Node2                   source) {
      std::vector<Node1> order;
      if (ad.validate()) {
        return order;
      }
      size_t const              N = ad.number_of_nodes();
      detail::stack_type<Node1> stck;
      std::vector<uint8_t>      seen(N, 0);

      stck.emplace(source, 0);
      detail::topological_sort(ad, stck, seen, order);
      return order;
    }

  }  // namespace word_graph

  //////////////////////////////////////////////////////////////////////////////
  // Member functions
  //////////////////////////////////////////////////////////////////////////////

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

  //////////////////////////////////////////////////////////////////////////////
  // Non-member functions
  //////////////////////////////////////////////////////////////////////////////

  template <typename Node>
  std::ostream& operator<<(std::ostream& os, WordGraph<Node> const& ad) {
    os << "{";
    std::string sep_n;
    for (auto n = ad.cbegin_nodes(); n != ad.cend_nodes(); ++n) {
      std::string sep_e;
      os << sep_n << "{";
      for (auto e = ad.cbegin_edges(*n); e != ad.cend_edges(*n); ++e) {
        os << sep_e << (*e == UNDEFINED ? "-" : std::to_string(*e));
        sep_e = ", ";
      }
      os << "}";
      sep_n = ", ";
    }
    os << "}";
    return os;
  }

  template <typename Node>
  WordGraph<Node>
  to_word_graph(size_t                                             num_nodes,
                std::initializer_list<std::initializer_list<Node>> il) {
    WordGraph<Node> result(num_nodes, il.begin()->size());
    for (size_t i = 0; i < il.size(); ++i) {
      for (size_t j = 0; j < (il.begin() + i)->size(); ++j) {
        auto val = *((il.begin() + i)->begin() + j);
        if (val != UNDEFINED) {
          result.add_edge(i, *((il.begin() + i)->begin() + j), j);
        }
      }
    }
    return result;
  }

}  // namespace libsemigroups
