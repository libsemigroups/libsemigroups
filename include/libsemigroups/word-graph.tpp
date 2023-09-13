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

// TODO re-order the file as per hpp

namespace libsemigroups {
  //////////////////////////////////////////////////////////////////////////////
  // Helper namespace
  //////////////////////////////////////////////////////////////////////////////

  namespace word_graph {
    namespace detail {
      template <typename Graph>
      bool shortlex_standardize(Graph& wg, Forest& f) {
        LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type = typename Graph::node_type;

        f.add_nodes(1);

        node_type    t      = 0;
        size_t const n      = wg.out_degree();
        bool         result = false;

        // p : new -> old and q : old -> new
        std::vector<node_type> p(wg.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        for (node_type s = 0; s <= t; ++s) {
          for (letter_type x = 0; x < n; ++x) {
            node_type r = wg.target_no_checks(p[s], x);
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
        wg.permute_nodes_no_checks(p, q);
        return result;
      }

      template <typename Graph>
      bool lex_standardize(Graph& wg, Forest& f) {
        LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type  = typename Graph::node_type;
        using label_type = typename Graph::label_type;

        f.add_nodes(1);

        node_type  s = 0, t = 0;
        label_type x      = 0;
        auto const n      = wg.out_degree();
        bool       result = false;

        // p : new -> old and q : old -> new
        std::vector<node_type> p(wg.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        // Perform a DFS through wg
        while (s <= t) {
          node_type r = wg.target_no_checks(p[s], x);
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
        wg.permute_nodes_no_checks(p, q);
        return result;
      }

      template <typename Graph>
      bool recursive_standardize(Graph& wg, Forest& f) {
        LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
        LIBSEMIGROUPS_ASSERT(f.number_of_nodes() == 0);

        using node_type = typename Graph::node_type;

        f.add_nodes(1);

        std::vector<word_type> words;
        size_t const           n = wg.out_degree();
        letter_type            a = 0;
        node_type              s = 0, t = 0;

        std::vector<node_type> p(wg.number_of_nodes(), 0);
        std::iota(p.begin(), p.end(), 0);
        std::vector<node_type> q(p);

        size_t max_t = number_of_nodes_reachable_from(wg, 0) - 1;

        // TODO move this out of here and use it in the other standardize
        // functions
        auto swap_if_necessary = [&wg, &f, &p, &q](node_type const   s,
                                                   node_type&        t,
                                                   letter_type const x) {
          node_type r      = wg.target_no_checks(p[s], x);
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
              wg, 0, words[u].begin(), words[u].end());
          if (uu != UNDEFINED) {
            for (int v = 0; v < x; v++) {
              node_type const uuv = word_graph::follow_path_no_checks(
                  wg, uu, words[v].begin(), words[v].end() - 1);
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
                wg, 0, words[w].begin(), words[w].end());
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
        wg.permute_nodes_no_checks(p, q);
        return result;
      }

      // Helper function for the two versions of is_acyclic below.
      // Not noexcept because std::stack::emplace isn't
      // This function does not really need to exist any longer, since
      // topological_sort can be used for the same computation, but we retain
      // it because it was already written and uses less space than
      // topological_sort.
      template <typename Node>
      bool is_acyclic(WordGraph<Node> const& wg,
                      std::stack<Node>&      stck,
                      std::vector<Node>&     preorder,
                      Node&                  next_preorder_num,
                      std::vector<Node>&     postorder,
                      Node&                  next_postorder_num) {
        size_t const M = wg.out_degree();
        size_t const N = wg.number_of_nodes();
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
                auto w = wg.target_no_checks(v, label);
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
      using stack_type
          = std::stack<std::pair<Node, typename WordGraph<Node>::label_type>>;
      using lookup_type = std::vector<uint8_t>;

      // helper function for the public functions below
      template <typename Node>
      bool topological_sort(WordGraph<Node> const& wg,
                            stack_type<Node>&      stck,
                            lookup_type&           seen,
                            std::vector<Node>&     order) {
        using label_type = typename WordGraph<Node>::label_type;
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
          std::tie(n, e) = wg.next_target_no_checks(m, e);
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
        } while (e < wg.out_degree());
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

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      template <typename Node>
      void init_adjacency_matrix(
          WordGraph<Node> const&                                 wg,
          Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& mat) {
        size_t const N = wg.number_of_nodes();
        mat.resize(N, N);
        mat.fill(0);
      }

      static inline void
      identity(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& x) {
        x.fill(0);
        for (size_t i = 0; i < static_cast<size_t>(x.rows()); ++i) {
          x(i, i) = 1;
        }
      }

#else
      template <typename Node>
      void init_adjacency_matrix(WordGraph<Node> const& wg,
                                 IntMat<0, 0, int64_t>& mat) {
        size_t const N = wg.number_of_nodes();
        mat            = IntMat<0, 0, int64_t>(N, N);
        std::fill(mat.begin(), mat.end(), 0);
      }
#endif
    }  // namespace detail

    //! No doc
    // not noexcept because it throws an exception!
    template <typename Node>
    void validate_node(WordGraph<Node> const& wg, Node v) {
      if (v >= wg.number_of_nodes()) {
        LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value "
                                "in the range [0, {}), got {}",
                                wg.number_of_nodes(),
                                v);
      }
    }

    //! No doc
    // not noexcept because it throws an exception!
    template <typename Node>
    void validate_label(WordGraph<Node> const&               wg,
                        typename WordGraph<Node>::label_type lbl) {
      if (lbl >= wg.out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("label value out of bounds, expected value in "
                                "the range [0, {}), got {}",
                                wg.out_degree(),
                                lbl);
      }
    }

    template <typename Graph>
    std::pair<bool, Forest> standardize(Graph& wg, Order val) {
      static_assert(
          std::is_base_of<WordGraphBase, Graph>::value,
          "the template parameter Graph must be derived from WordGraphBase");
      Forest f;
      bool   result = standardize(wg, f, val);
      return std::make_pair(result, f);
    }

    template <typename Graph>
    bool standardize(Graph& wg, Forest& f, Order val) {
      static_assert(std::is_base_of_v<WordGraphBase, Graph>,
                    "the template parameter Graph must be "
                    "derived from WordGraphBase");
      if (!f.empty()) {
        f.clear();
      }
      if (wg.number_of_nodes() == 0) {
        return false;
      }

      switch (val) {
        case Order::none:
          return false;
        case Order::shortlex:
          return detail::shortlex_standardize(wg, f);
        case Order::lex:
          return detail::lex_standardize(wg, f);
        case Order::recursive:
          return detail::recursive_standardize(wg, f);
        default:
          return false;
      }
    }

    template <typename Node, typename Iterator>
    void add_cycle(WordGraph<Node>& wg, Iterator first, Iterator last) {
      for (auto it = first; it < last - 1; ++it) {
        wg.set_target(*it, 0, *(it + 1));
      }
      wg.set_target(*(last - 1), 0, *first);
    }

    template <typename Node,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3>
    bool is_compatible(WordGraph<Node> const& wg,
                       Iterator1              first_node,
                       Iterator2              last_node,
                       Iterator3              first_rule,
                       Iterator3              last_rule) {
      for (auto nit = first_node; nit != last_node; ++nit) {
        for (auto rit = first_rule; rit != last_rule; ++rit) {
          auto l = word_graph::follow_path_no_checks(
              wg, *nit, rit->cbegin(), rit->cend());
          ++rit;
          if (l == UNDEFINED) {
            continue;
          }
          auto r = word_graph::follow_path_no_checks(
              wg, *nit, rit->cbegin(), rit->cend());
          if (r == UNDEFINED) {
            continue;
          }
          if (l != r) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Node, typename Iterator1, typename Iterator2>
    bool is_complete(WordGraph<Node> const& wg,
                     Iterator1              first_node,
                     Iterator2              last_node) {
      size_t const n = wg.out_degree();
      for (auto it = first_node; it != last_node; ++it) {
        for (size_t a = 0; a < n; ++a) {
          if (wg.target_no_checks(*it, a) == UNDEFINED) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Node>
    bool is_strictly_cyclic(WordGraph<Node> const& wg) {
      using node_type = typename WordGraph<Node>::node_type;
      auto const N    = wg.number_of_nodes();

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
            for (auto t : wg.targets_no_checks(n)) {
              if (t != UNDEFINED) {
                stack.push(t);
              }
            }
          }
        }
        std::fill(seen.begin(), seen.end(), false);
      }
      return false;
    }

    template <typename Node>
    bool is_connected(WordGraph<Node> const& wg) {
      auto const N = wg.number_of_nodes();
      if (N == 0) {
        return true;
      }

      ::libsemigroups::detail::Duf<> uf(N);
      for (auto s : wg.nodes()) {
        for (auto t : wg.targets_no_checks(s)) {
          if (t != UNDEFINED) {
            uf.unite(s, t);
          }
        }
      }
      return uf.number_of_blocks() == 1;
    }

    template <typename Node1, typename Node2>
    bool is_reachable(WordGraph<Node1> const& wg, Node2 source, Node2 target) {
      using label_type = typename WordGraph<Node1>::label_type;
      validate_node(wg, static_cast<Node1>(source));
      validate_node(wg, static_cast<Node1>(target));
      if (source == target) {
        return true;
      }
      label_type             edge = 0;
      std::stack<Node1>      nodes;
      std::stack<label_type> edges;
      std::vector<bool>      seen(wg.number_of_nodes(), false);
      nodes.push(source);
      seen[source] = true;

      do {
        Node1 node;
        std::tie(node, edge) = wg.next_target_no_checks(nodes.top(), edge);
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
    bool is_acyclic(WordGraph<Node> const& wg) {
      if (word_graph::is_complete(wg)) {
        return false;
      }
      auto const        N = wg.number_of_nodes();
      std::stack<Node>  stck;
      std::vector<Node> preorder(N, N);
      Node              next_preorder_num = 0;
      std::vector<Node> postorder(N, N);
      Node              next_postorder_num = 0;

      for (auto m : wg.nodes()) {
        if (preorder[m] == N) {
          stck.push(m);
          if (!detail::is_acyclic(wg,
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
    bool is_acyclic(WordGraph<Node1> const& wg, Node2 source) {
      validate_node(wg, static_cast<Node1>(source));
      auto const        N = wg.number_of_nodes();
      std::stack<Node1> stck;
      stck.push(source);
      std::vector<Node1> preorder(N, N);
      Node1              next_preorder_num = 0;
      std::vector<Node1> postorder(N, N);
      Node1              next_postorder_num = 0;
      return detail::is_acyclic(
          wg, stck, preorder, next_preorder_num, postorder, next_postorder_num);
    }

    template <typename Node1, typename Node2>
    bool is_acyclic(WordGraph<Node1> const& wg, Node2 source, Node2 target) {
      validate_node(wg, static_cast<Node1>(source));
      validate_node(wg, static_cast<Node1>(target));
      if (!is_reachable(wg, source, target)) {
        return true;
      }
      auto const        N = wg.number_of_nodes();
      std::stack<Node1> stck;
      stck.push(source);
      std::vector<Node1> preorder(N, N);
      Node1              next_preorder_num = 0;
      std::vector<Node1> postorder(N, N);
      Node1              next_postorder_num = 0;
      // TODO(later) there should be a better way of doing this
      for (auto n : wg.nodes()) {
        if (!is_reachable(wg, n, static_cast<Node1>(target))) {
          preorder[n] = N + 1;
        }
      }
      return detail::is_acyclic(
          wg, stck, preorder, next_preorder_num, postorder, next_postorder_num);
    }

    template <typename Node>
    std::vector<Node> topological_sort(WordGraph<Node> const& wg) {
      std::vector<Node> order;
      if (word_graph::is_complete(wg)) {
        return order;
      }

      size_t const             N = wg.number_of_nodes();
      detail::stack_type<Node> stck;
      std::vector<uint8_t>     seen(N, 0);

      for (auto m : wg.nodes()) {
        if (seen[m] == 0) {
          stck.emplace(m, 0);
          if (!detail::topological_sort(wg, stck, seen, order)) {
            // digraph is not acyclic and so there's no topological order for
            // the nodes.
            LIBSEMIGROUPS_ASSERT(order.empty());
            return order;
          }
        }
      }
      LIBSEMIGROUPS_ASSERT(order.size() == wg.number_of_nodes());
      return order;
    }

    template <typename Node1, typename Node2>
    std::vector<Node1> topological_sort(WordGraph<Node1> const& wg,
                                        Node2                   source) {
      std::vector<Node1> order;
      if (word_graph::is_complete(wg)) {
        return order;
      }
      size_t const              N = wg.number_of_nodes();
      detail::stack_type<Node1> stck;
      std::vector<uint8_t>      seen(N, 0);

      stck.emplace(source, 0);
      detail::topological_sort(wg, stck, seen, order);
      return order;
    }

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    static inline Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
    pow(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const& x,
        size_t                                                       e) {
      using Mat = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
      return Eigen::MatrixPower<Mat>(x)(e);
    }
#endif

    template <typename Node>
    auto adjacency_matrix(WordGraph<Node> const& wg) {
      using Mat = typename WordGraph<Node>::adjacency_matrix_type;
      Mat mat;
      detail::init_adjacency_matrix(wg, mat);

      for (auto s : wg.nodes()) {
        for (auto t : wg.targets_no_checks(s)) {
          if (t != UNDEFINED) {
            mat(s, t) += 1;
          }
        }
      }
      return mat;
    }

    template <typename Node1, typename Node2>
    std::unordered_set<Node1> nodes_reachable_from(WordGraph<Node1> const& wg,
                                                   Node2 source) {
      validate_node(wg, static_cast<Node1>(source));

      std::unordered_set<Node1> seen;
      std::stack<Node1>         stack;
      stack.push(source);

      while (!stack.empty()) {
        Node1 n = stack.top();
        stack.pop();
        if (seen.insert(n).second) {
          for (auto t : wg.targets_no_checks(n)) {
            if (t != UNDEFINED) {
              stack.push(t);
            }
          }
        }
      }
      return seen;
    }

    template <typename Node1, typename Node2, typename Iterator>
    Node1 follow_path(WordGraph<Node1> const& wg,
                      Node2                   from,
                      Iterator                first,
                      Iterator                last) {
      if constexpr (::libsemigroups::detail::HasLessEqual<Iterator,
                                                          Iterator>::value) {
        if (last <= first) {
          return from;
        }
      }
      for (auto it = first; it != last && from != UNDEFINED; ++it) {
        from = wg.target(from, *it);
      }
      return from;
    }

    template <typename Node1, typename Node2, typename Iterator>
    Node1 follow_path_no_checks(WordGraph<Node1> const& wg,
                                Node2                   from,
                                Iterator                first,
                                Iterator                last) noexcept {
      if constexpr (::libsemigroups::detail::HasLessEqual<Iterator,
                                                          Iterator>::value) {
        if (last <= first) {
          return from;
        }
      }
      for (auto it = first; it != last && from != UNDEFINED; ++it) {
        from = wg.target_no_checks(from, *it);
      }
      return from;
    }

    template <typename Node1, typename Node2, typename Iterator>
    std::pair<Node1, Iterator>
    last_node_on_path_no_checks(WordGraph<Node1> const& wg,
                                Node2                   from,
                                Iterator                first,
                                Iterator                last) noexcept {
      auto  it   = first;
      Node1 prev = from, to = from;
      for (; it < last && to != UNDEFINED; ++it) {
        prev = to;
        to   = wg.target_no_checks(to, *it);
      }
      if (it != last || to == UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(prev != UNDEFINED);
        return {prev, it - 1};
      } else {
        return {to, it};
      }
    }

    template <typename Node1, typename Node2, typename Iterator>
    std::pair<Node1, Iterator> last_node_on_path(WordGraph<Node1> const& wg,
                                                 Node2                   from,
                                                 Iterator                first,
                                                 Iterator                last) {
      auto         it   = first;
      Node1        prev = from;
      Node1        to   = from;
      size_t const n    = wg.out_degree();
      for (; it < last && to != UNDEFINED; ++it) {
        prev = to;
        if (*it >= n) {
          to = UNDEFINED;
        } else {
          to = wg.target_no_checks(to, *it);
        }
      }
      if (it != last || to == UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(prev != UNDEFINED);
        return {prev, it - 1};
      } else {
        return {to, it};
      }
    }

  }  // namespace word_graph

  //////////////////////////////////////////////////////////////////////////////
  // Member functions
  //////////////////////////////////////////////////////////////////////////////

  template <typename Node>
  WordGraph<Node>::~WordGraph() = default;

  template <typename Node>
  WordGraph<Node>::WordGraph(size_type m, size_type n)
      : _degree(n),
        _nr_nodes(m),
        _num_active_nodes(),
        _dynamic_array_2(_degree, _nr_nodes, UNDEFINED) {}

  template <typename Node>
  WordGraph<Node>& WordGraph<Node>::init(size_type m, size_type n) {
    _degree           = n;
    _nr_nodes         = m;
    _num_active_nodes = 0;
    _dynamic_array_2.reshape(n, m);
    remove_all_targets();
    return *this;
  }

  template <typename Node>
  WordGraph<Node>::WordGraph(WordGraph const&) = default;

  template <typename Node>
  template <typename OtherNode>
  WordGraph<Node>::WordGraph(WordGraph<OtherNode> const& wg)
      : WordGraph(wg.number_of_nodes(), wg.out_degree()) {
    _dynamic_array_2 = wg._dynamic_array_2;
  }

  template <typename Node>
  template <typename OtherNode>
  WordGraph<Node>& WordGraph<Node>::init(WordGraph<OtherNode> const& wg) {
    init(wg.number_of_nodes(), wg.out_degree());
    _dynamic_array_2 = wg._dynamic_array_2;
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
  WordGraph<Node> WordGraph<Node>::random(size_type    number_of_nodes,
                                          size_type    out_degree,
                                          size_type    number_of_edges,
                                          std::mt19937 mt) {
    if (number_of_nodes < 2) {
      LIBSEMIGROUPS_EXCEPTION("the 1st parameter `number_of_nodes` must be "
                              "at least 2, found {}",
                              number_of_nodes);
    } else if (out_degree < 2) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd parameter `number_of_edges` must be "
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
  WordGraph<Node> WordGraph<Node>::random_acyclic(size_type    number_of_nodes,
                                                  size_type    out_degree,
                                                  size_type    number_of_edges,
                                                  std::mt19937 mt) {
    if (number_of_nodes < 2) {
      LIBSEMIGROUPS_EXCEPTION("the 1st parameter `number_of_nodes` must be "
                              "at least 2, found {}",
                              number_of_nodes);
    } else if (out_degree < 2) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd parameter `number_of_edges` must be "
                              "at least 2, found {}",
                              out_degree);
    }
    size_t max_edges = std::min(number_of_nodes * out_degree,
                                number_of_nodes * (number_of_nodes - 1) / 2);

    if (number_of_edges > max_edges) {
      LIBSEMIGROUPS_EXCEPTION(
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
  void WordGraph<Node>::induced_subgraph_no_checks(Node first, Node last) {
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
  template <typename Iterator, typename>
  void WordGraph<Node>::induced_subgraph_no_checks(Iterator first,
                                                   Iterator last) {
    size_t const N = std::distance(first, last);
    // if (*last == *first + N) {
    //   induced_subgraph_no_checks(*first, *last);
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
      for (auto [a, t] : rx::enumerate(targets_no_checks(*nit))) {
        if (t != UNDEFINED) {
          if (old_to_new[t] == UNDEFINED) {
            old_to_new[t] = next;
            next++;
          }
          copy.set_target_no_checks(old_to_new[*nit], a, old_to_new[t]);
        }
      }
    }
    std::swap(*this, copy);
  }

  template <typename Node>
  void WordGraph<Node>::set_target(node_type m, label_type lbl, node_type n) {
    word_graph::validate_node(*this, m);
    word_graph::validate_node(*this, n);
    word_graph::validate_label(*this, lbl);
    set_target_no_checks(m, lbl, n);
  }

  template <typename Node>
  std::pair<Node, typename WordGraph<Node>::label_type>
  WordGraph<Node>::next_target_no_checks(
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
  std::pair<Node, typename WordGraph<Node>::label_type>
  WordGraph<Node>::next_target(node_type v, label_type i) const {
    word_graph::validate_node(*this, v);
    return next_target_no_checks(v, i);
  }

  template <typename Node>
  size_t WordGraph<Node>::number_of_edges() const {
    return _dynamic_array_2.number_of_rows() * _dynamic_array_2.number_of_cols()
           - std::count(
               _dynamic_array_2.cbegin(), _dynamic_array_2.cend(), UNDEFINED);
  }

  template <typename Node>
  size_t WordGraph<Node>::number_of_edges(node_type n) const {
    word_graph::validate_node(*this, n);
    return out_degree()
           - std::count(_dynamic_array_2.cbegin_row(n),
                        _dynamic_array_2.cend_row(n),
                        UNDEFINED);
  }

  template <typename Node>
  void WordGraph<Node>::remove_label_no_checks(label_type lbl) {
    if (lbl == _degree - 1) {
      _degree--;
    } else {
      _dynamic_array_2.erase_column(lbl);
    }
  }

  template <typename Node>
  void WordGraph<Node>::remove_label(label_type lbl) {
    word_graph::validate_label(*this, lbl);
    remove_label_no_checks(lbl);
  }

  template <typename Node>
  void WordGraph<Node>::reserve(size_type m, size_type n) const {
    if (n > _dynamic_array_2.number_of_cols()) {
      _dynamic_array_2.add_cols(n - _dynamic_array_2.number_of_cols());
    }
    if (m > _dynamic_array_2.number_of_rows()) {
      _dynamic_array_2.add_rows(m - _dynamic_array_2.number_of_rows());
    }
  }

  template <typename Node>
  Node WordGraph<Node>::target(node_type v, label_type lbl) const {
    word_graph::validate_node(*this, v);
    word_graph::validate_label(*this, lbl);
    return _dynamic_array_2.get(v, lbl);
  }

  template <typename Node>
  typename WordGraph<Node>::const_iterator_targets
  WordGraph<Node>::cbegin_targets(node_type i) const {
    word_graph::validate_node(*this, i);
    return cbegin_targets_no_checks(i);
  }

  template <typename Node>
  typename WordGraph<Node>::const_iterator_targets
  WordGraph<Node>::cend_targets(node_type i) const {
    word_graph::validate_node(*this, i);
    return cend_targets_no_checks(i);
  }

  template <typename Node>
  rx::iterator_range<typename WordGraph<Node>::const_iterator_targets>
  WordGraph<Node>::targets(node_type n) const {
    word_graph::validate_node(*this, n);
    return targets_no_checks(n);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-member functions
  //////////////////////////////////////////////////////////////////////////////

  template <typename Node>
  std::ostream& operator<<(std::ostream& os, WordGraph<Node> const& wg) {
    os << "{";
    std::string sep_n;
    for (auto n : wg.nodes()) {
      std::string sep_e;
      os << sep_n << "{";
      for (auto e : wg.targets_no_checks(n)) {
        os << sep_e << (e == UNDEFINED ? "-" : std::to_string(e));
        sep_e = ", ";
      }
      os << "}";
      sep_n = ", ";
    }
    os << "}";
    return os;
  }

  // TODO: Refactor to use vectors
  template <typename Node>
  WordGraph<Node> to_word_graph(size_t                                num_nodes,
                                std::vector<std::vector<Node>> const& il) {
    WordGraph<Node> result(num_nodes, il.begin()->size());
    for (size_t i = 0; i < il.size(); ++i) {
      for (size_t j = 0; j < (il.begin() + i)->size(); ++j) {
        auto val = *((il.begin() + i)->begin() + j);
        if (val != UNDEFINED) {
          result.set_target(i, j, *((il.begin() + i)->begin() + j));
        }
      }
    }
    return result;
  }

  template <typename Node>
  WordGraph<Node> to_word_graph(size_t num_nodes,
                                std::initializer_list<std::vector<Node>> il) {
    return to_word_graph<Node>(num_nodes, std::vector<std::vector<Node>>(il));
  }

}  // namespace libsemigroups
