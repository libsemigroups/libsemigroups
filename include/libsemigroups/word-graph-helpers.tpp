//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Nadim Searight
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

// This file contains helper functions for word graphs and word graph views

namespace libsemigroups {
  namespace v4 {
    //////////////////////////////////////////////////////////////////////////////
    // Helper namespace
    //////////////////////////////////////////////////////////////////////////////

    namespace word_graph {

      template <typename Node>
      [[nodiscard]] bool is_strictly_cyclic(WordGraphView<Node> const& wg) {
        using node_type = typename WordGraphView<Node>::node_type;
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
                if (t < N) {
                  stack.push(t);
                }
              }
            }
          }
          std::fill(seen.begin(), seen.end(), false);
        }
        return false;
      }

      namespace detail {
        template <typename Node>
        bool is_shortlex_standardized(WordGraphView<Node> const& wg) {
          Node current_max_node = 0;

          for (auto s : wg.nodes()) {
            for (auto t : wg.targets_no_checks(s)) {
              if (t != UNDEFINED) {
                if (t > current_max_node) {
                  if (t == current_max_node + 1) {
                    current_max_node++;
                  } else {
                    return false;
                  }
                }
              }
            }
          }
          return true;
        }

        // For best performance ensure that <f> has the correct number of nodes
        // when calling this function.
        template <typename Graph>
        bool shortlex_standardize(Graph& wg, Forest& f) {
          LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
          LIBSEMIGROUPS_ASSERT(f.number_of_nodes() != 0);

          using node_type = typename Graph::node_type;

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
              if (r < wg.number_of_nodes()) {
                r = q[r];  // new
                if (r > t) {
                  t++;
                  if (r > t) {
                    std::swap(p[t], p[r]);
                    std::swap(q[p[t]], q[p[r]]);
                    result = true;
                  }
                  if (t >= f.number_of_nodes()) {
                    f.add_nodes(1);
                  }
                  f.set_parent_and_label_no_checks(t, (s == t ? r : s), x);
                }
              }
            }
          }
          if (result) {
            wg.standardize(p, q);
          }
          return result;
        }

        template <typename Graph>
        bool lex_standardize(Graph& wg, Forest& f) {
          LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
          LIBSEMIGROUPS_ASSERT(f.number_of_nodes() != 0);

          using node_type  = typename Graph::node_type;
          using label_type = typename Graph::label_type;

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
            if (r < wg.number_of_nodes()) {
              r = q[r];  // new
              if (r > t) {
                t++;
                if (t >= f.number_of_nodes()) {
                  f.add_nodes(1);
                }
                if (r > t) {
                  std::swap(p[t], p[r]);
                  std::swap(q[p[t]], q[p[r]]);
                  result = true;
                }
                f.set_parent_and_label_no_checks(t, (s == t ? r : s), x);
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
          if (result) {
            wg.standardize(p, q);
          }
          return result;
        }

        template <typename Graph>
        bool recursive_standardize(Graph& wg, Forest& f) {
          LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
          LIBSEMIGROUPS_ASSERT(f.number_of_nodes() != 0);

          using node_type = typename Graph::node_type;

          std::vector<word_type> words;
          size_t const           n = wg.out_degree();
          letter_type            a = 0;
          node_type              s = 0, t = 0;

          std::vector<node_type> p(wg.number_of_nodes(), 0);
          std::iota(p.begin(), p.end(), 0);
          std::vector<node_type> q(p);

          size_t max_t = number_of_nodes_reachable_from(wg, 0) - 1;

          // TODO(1) move this out of here and use it in the other standardize
          // functions
          auto swap_if_necessary = [&wg, &f, &p, &q](node_type const   ss,
                                                     node_type&        tt,
                                                     letter_type const x) {
            node_type r      = wg.target_no_checks(p[ss], x);
            bool      result = false;
            if (r < wg.number_of_nodes()) {
              r = q[r];  // new
              if (r > tt) {
                tt++;
                if (tt >= f.number_of_nodes()) {
                  f.add_nodes(1);
                }
                if (r > tt) {
                  std::swap(p[tt], p[r]);
                  std::swap(q[p[tt]], q[p[r]]);
                }
                result = true;
                f.set_parent_and_label_no_checks(tt, (ss == tt ? r : ss), x);
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
          if (result) {
            wg.standardize(p, q);
          }
          return result;
        }

        // Helper function for the two versions of is_acyclic below.
        // Not noexcept because std::stack::emplace isn't
        // This function does not really need to exist any longer, since
        // topological_sort can be used for the same computation, but we retain
        // it because it was already written and uses less space than
        // topological_sort.
        template <typename Node>
        bool is_acyclic(WordGraphView<Node> const& wg,
                        std::stack<Node>&          stck,
                        std::vector<Node>&         preorder,
                        Node&                      next_preorder_num,
                        std::vector<Node>&         postorder,
                        Node&                      next_postorder_num) {
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
        bool topological_sort(WordGraphView<Node> const& wg,
                              stack_type<Node>&          stck,
                              lookup_type&               seen,
                              std::vector<Node>&         order) {
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
            std::tie(e, n) = wg.next_label_and_target_no_checks(m, e);
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

        // helper function for the public functions below
        template <typename Node>
        bool topological_sort(WordGraph<Node> const& wg,
                              stack_type<Node>&      stck,
                              lookup_type&           seen,
                              std::vector<Node>&     order) {
          return topological_sort(WordGraphView<Node>(wg), stck, seen, order);
        }

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
        template <typename Node>
        void init_adjacency_matrix(
            WordGraph<Node> const&                                 wg,
            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& mat) {
          init_adjacency_matrix(WordGraphView<Node>(wg), mat);
        }

        template <typename Node>
        void init_adjacency_matrix(
            WordGraphView<Node> const&                             wg,
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
        void init_adjacency_matrix(WordGraphView<Node> const& wg,
                                   IntMat<0, 0, int64_t>&     mat) {
          size_t const N = wg.number_of_nodes();
          mat            = IntMat<0, 0, int64_t>(N, N);
          std::fill(mat.begin(), mat.end(), 0);
        }

        template <typename Node>
        void init_adjacency_matrix(WordGraph<Node> const& wg,
                                   IntMat<0, 0, int64_t>& mat) {
          init_adjacency_matrix(WordGraphView<Node>(wg), mat);
        }
#endif
      }  // namespace detail

      template <typename Graph>
      std::pair<bool, Forest> standardize(Graph& wg, Order val) {
        Forest f;
        bool   result = standardize(wg, f, val);
        return std::make_pair(result, f);
      }

      template <typename Graph>
      bool standardize(Graph& wg, Forest& f, Order val) {
        if (wg.number_of_nodes() == 0) {
          return false;
        }

        if (f.number_of_nodes() == 0) {
          f.add_nodes(1);
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

      template <typename Node>
      bool is_standardized(WordGraphView<Node> const& wg, Order val) {
        switch (val) {
          case Order::none:
            return true;
          case Order::shortlex:
            return detail::is_shortlex_standardized(wg);
          case Order::lex:
          case Order::recursive:
          default:
            LIBSEMIGROUPS_EXCEPTION("not yet implemented")
        }
      }

      template <typename Node, typename Iterator>
      void add_cycle_no_checks(WordGraph<Node>& wg,
                               Iterator         first,
                               Iterator         last) {
        for (auto it = first; it < last - 1; ++it) {
          wg.target(*it, 0, *(it + 1));
        }
        wg.target(*(last - 1), 0, *first);
      }

      template <typename Node,
                typename Iterator1,
                typename Iterator2,
                typename Iterator3>
      bool is_compatible_no_checks(WordGraphView<Node> const& wg,
                                   Iterator1                  first_node,
                                   Iterator2                  last_node,
                                   Iterator3                  first_rule,
                                   Iterator3                  last_rule) {
        for (auto rit = first_rule; rit < last_rule; rit += 2) {
          if (!is_compatible_no_checks(
                  wg, first_node, last_node, *rit, *(rit + 1))) {
            return false;
          }
        }
        return true;
      }

      template <typename Node, typename Iterator1, typename Iterator2>
      bool is_compatible_no_checks(WordGraphView<Node> const& wg,
                                   Iterator1                  first_node,
                                   Iterator2                  last_node,
                                   word_type const&           lhs,
                                   word_type const&           rhs) {
        for (auto nit = first_node; nit != last_node; ++nit) {
          auto l = word_graph::follow_path_no_checks(
              wg, *nit, lhs.cbegin(), lhs.cend());
          if (l == UNDEFINED) {
            continue;
          }
          auto r = word_graph::follow_path_no_checks(
              wg, *nit, rhs.cbegin(), rhs.cend());
          if (r == UNDEFINED) {
            continue;
          }
          if (l != r) {
            return false;
          }
        }
        return true;
      }

      template <typename Node, typename Iterator1, typename Iterator2>
      bool is_compatible(WordGraphView<Node> const& wg,
                         Iterator1                  first_node,
                         Iterator2                  last_node,
                         word_type const&           lhs,
                         word_type const&           rhs) {
        throw_if_node_out_of_bounds(wg, first_node, last_node);
        // TODO(1) be better to use follow_path in is_compatible_no_checks
        throw_if_label_out_of_bounds(wg, lhs);
        throw_if_label_out_of_bounds(wg, rhs);
        return is_compatible_no_checks(wg, first_node, last_node, lhs, rhs);
      }

      template <typename Node,
                typename Iterator1,
                typename Iterator2,
                typename Iterator3,
                typename>
      bool is_compatible(WordGraphView<Node> const& wg,
                         Iterator1                  first_node,
                         Iterator2                  last_node,
                         Iterator3                  first_rule,
                         Iterator3                  last_rule) {
        for (auto rit = first_rule; rit < last_rule; rit += 2) {
          if (!is_compatible(wg, first_node, last_node, *rit, *(rit + 1))) {
            return false;
          }
        }
        return true;
      }

      template <typename Node, typename Iterator1, typename Iterator2>
      bool is_complete_no_checks(WordGraphView<Node> const& wg,
                                 Iterator1                  first_node,
                                 Iterator2                  last_node) {
        using label_type = typename WordGraph<Node>::label_type;
        size_t const n   = wg.out_degree();
        for (auto it = first_node; it != last_node; ++it) {
          for (label_type a = 0; a < n; ++a) {
            if (wg.target_no_checks(*it, a) == UNDEFINED) {
              return false;
            }
          }
        }
        return true;
      }

      template <typename Node, typename Iterator1, typename Iterator2>
      bool is_complete(WordGraphView<Node> const& wg,
                       Iterator1                  first_node,
                       Iterator2                  last_node) {
        throw_if_node_out_of_bounds(wg, first_node, last_node);
        return is_complete_no_checks(wg, first_node, last_node);
      }

      template <typename Node>
      bool is_connected(WordGraphView<Node> const& wg) {
        auto const N = wg.number_of_nodes();
        if (N == 0) {
          return true;
        }

        ::libsemigroups::detail::Duf<> uf(N);
        for (auto s : wg.nodes()) {
          for (auto t : wg.targets_no_checks(s)) {
            if (t < N) {
              uf.unite(s, t);
            }
          }
        }
        return uf.number_of_blocks() == 1;
      }

      template <typename Node1, typename Node2>
      bool is_reachable_no_checks(WordGraphView<Node1> const& wg,
                                  Node2                       source,
                                  Node2                       target) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        using label_type = typename WordGraph<Node1>::label_type;
        if (source == target) {
          return true;
        }
        label_type             edge = 0;
        std::stack<Node1>      nodes;
        std::stack<label_type> edges;
        std::vector<bool>      seen(wg.number_of_nodes(), false);
        nodes.push(source);
        seen[source] = true;

        size_t const N = wg.number_of_nodes();

        do {
          Node1 node;
          std::tie(edge, node)
              = wg.next_label_and_target_no_checks(nodes.top(), edge);
          if (node == static_cast<Node1>(target)) {
            return true;
          } else if (node < N) {
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

      template <typename Node1, typename Node2>
      bool is_reachable(WordGraphView<Node1> const& wg,
                        Node2                       source,
                        Node2                       target) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        throw_if_node_out_of_bounds(wg, static_cast<Node1>(source));
        throw_if_node_out_of_bounds(wg, static_cast<Node1>(target));
        return is_reachable_no_checks(wg, source, target);
      }

      template <typename Node>
      bool is_acyclic(WordGraphView<Node> const& wg) {
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
      bool is_acyclic(WordGraphView<Node1> const& wg, Node2 source) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        throw_if_node_out_of_bounds(wg, static_cast<Node1>(source));
        auto const        N = wg.number_of_nodes();
        std::stack<Node1> stck;
        stck.push(source);
        std::vector<Node1> preorder(N, N);
        Node1              next_preorder_num = 0;
        std::vector<Node1> postorder(N, N);
        Node1              next_postorder_num = 0;
        return detail::is_acyclic(wg,
                                  stck,
                                  preorder,
                                  next_preorder_num,
                                  postorder,
                                  next_postorder_num);
      }

      template <typename Node1, typename Node2>
      bool is_acyclic(WordGraphView<Node1> const& wg,
                      Node2                       source,
                      Node2                       target) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        throw_if_node_out_of_bounds(wg, static_cast<Node1>(source));
        throw_if_node_out_of_bounds(wg, static_cast<Node1>(target));
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

        auto ancestors = ancestors_of_no_checks(wg, target);
        for (auto n : wg.nodes()) {
          if (ancestors.count(n) == 0) {
            preorder[n] = N + 1;
          }
        }
        return detail::is_acyclic(wg,
                                  stck,
                                  preorder,
                                  next_preorder_num,
                                  postorder,
                                  next_postorder_num);
      }

      template <typename Node>
      std::vector<Node> topological_sort(WordGraphView<Node> const& wg) {
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
              // graph is not acyclic and so there's no topological order for
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
      std::vector<Node1> topological_sort(WordGraphView<Node1> const& wg,
                                          Node2                       source) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
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

      template <typename Node>
      auto adjacency_matrix(WordGraphView<Node> const& wg) {
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
      std::unordered_set<Node1>
      nodes_reachable_from_no_checks(WordGraphView<Node1> const& wg,
                                     Node2                       source) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        std::unordered_set<Node1> seen;
        std::stack<Node1>         stack;
        stack.push(source);

        size_t const N = wg.number_of_nodes();

        while (!stack.empty()) {
          Node1 n = stack.top();
          stack.pop();
          if (seen.insert(n).second) {
            for (auto t : wg.targets_no_checks(n)) {
              if (t < N) {
                stack.push(t);
              }
            }
          }
        }
        return seen;
      }

      template <typename Node1, typename Node2>
      std::unordered_set<Node1>
      nodes_reachable_from(WordGraphView<Node1> const& wg, Node2 source) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        throw_if_node_out_of_bounds(wg, static_cast<Node1>(source));
        return nodes_reachable_from_no_checks(wg, source);
      }

      template <typename Node1, typename Node2>
      std::unordered_set<Node1>
      ancestors_of_no_checks(WordGraphView<Node1> const& wg, Node2 target) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        using label_type = typename WordGraph<Node1>::label_type;

        size_t const N = wg.number_of_nodes();
        size_t const M = wg.out_degree();

        // Reverse the WordGraph and then just find the nodes reachable from
        // target in the reversed graph. Since the reverse of a WordGraph is no
        // longer a WordGraph we use a vector of vectors here. Alternatively, we
        // could use the technique used in WordGraphWithSources (the sources are
        // essentially the reversed graph) to create the reversed graph (or just
        // use it if we know it already, like in ToddCoxeter).
        std::vector<std::vector<Node1>> in_neighbours(N,
                                                      std::vector<Node1>({}));
        for (Node1 s = 0; s < N; ++s) {
          for (label_type a = 0; a < M; ++a) {
            auto t = wg.target_no_checks(s, a);
            if (t != UNDEFINED) {
              in_neighbours[t].push_back(s);
            }
          }
        }

        std::unordered_set<Node1> seen;
        std::stack<Node1>         stack;
        stack.push(target);

        while (!stack.empty()) {
          Node1 s = stack.top();
          stack.pop();
          if (seen.insert(s).second) {
            for (auto t : in_neighbours[s]) {
              stack.push(t);
            }
          }
        }
        return seen;
      }

      template <typename Node1, typename Node2>
      std::unordered_set<Node1> ancestors_of(WordGraphView<Node1> const& wg,
                                             Node2 target) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        throw_if_node_out_of_bounds(wg, static_cast<Node1>(target));
        return ancestors_of_no_checks(wg, target);
      }

      template <typename Node1, typename Node2, typename Iterator>
      Node1 follow_path(WordGraphView<Node1> const& wg,
                        Node2                       from,
                        Iterator                    first,
                        Iterator                    last) {
        static_assert(sizeof(Node1) <= sizeof(size_t));
        static_assert(sizeof(Node2) <= sizeof(Node1));

        throw_if_node_out_of_bounds(wg, from);

        if constexpr (::libsemigroups::detail::HasLessEqual<Iterator,
                                                            Iterator>::value) {
          if (last <= first) {
            return from;
          }
        }
        size_t const N = wg.number_of_nodes();

        for (auto it = first; it != last && static_cast<size_t>(from) < N;
             ++it) {
          from = wg.target(from, *it);
        }
        if (static_cast<size_t>(from) >= N) {
          return UNDEFINED;
        }
        return from;
      }

      template <typename Node1, typename Node2, typename Iterator>
      Node1 follow_path_no_checks(WordGraphView<Node1> const& wg,
                                  Node2                       from,
                                  Iterator                    first,
                                  Iterator                    last) noexcept {
        static_assert(sizeof(Node1) <= sizeof(size_t));
        static_assert(sizeof(Node2) <= sizeof(Node1));
        if constexpr (::libsemigroups::detail::HasLessEqual<Iterator,
                                                            Iterator>::value) {
          if (last <= first) {
            return from;
          }
        }
        size_t const N = wg.number_of_nodes();
        for (auto it = first; it != last && static_cast<size_t>(from) < N;
             ++it) {
          from = wg.target_no_checks(from, *it);
        }
        if (static_cast<size_t>(from) >= N) {
          return UNDEFINED;
        }
        return from;
      }

      template <typename Node1, typename Node2, typename Iterator>
      std::pair<Node1, Iterator>
      last_node_on_path_no_checks(WordGraphView<Node1> const& wg,
                                  Node2                       from,
                                  Iterator                    first,
                                  Iterator                    last) noexcept {
        static_assert(sizeof(Node2) <= sizeof(Node1));
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
      std::pair<Node1, Iterator>
      last_node_on_path(WordGraphView<Node1> const& wg,
                        Node2                       from,
                        Iterator                    first,
                        Iterator                    last) {
        throw_if_node_out_of_bounds(wg, from);

        static_assert(sizeof(Node2) <= sizeof(Node1));
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

      template <typename Node1, typename Node2>
      std::pair<Node1, word_type::const_iterator>
      last_node_on_path(WordGraphView<Node1> const& wg,
                        Node2                       from,
                        word_type const&            w) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        return last_node_on_path(wg, from, w.cbegin(), w.cend());
      }

      template <typename Node1, typename Node2>
      std::pair<Node1, word_type::const_iterator>
      last_node_on_path_no_checks(WordGraphView<Node1> const& wg,
                                  Node2                       from,
                                  word_type const&            w) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        return last_node_on_path_no_checks(wg, from, w.cbegin(), w.cend());
      }

      template <typename Node>
      Dot dot(WordGraphView<Node> const& wg) {
        Dot result;
        result.name("WordGraph").kind(Dot::Kind::digraph);
        for (auto n : wg.nodes()) {
          result.add_node(n).add_attr("shape", "box");
        }
        for (auto n : wg.nodes()) {
          for (auto [a, m] : wg.labels_and_targets_no_checks(n)) {
            if (m != UNDEFINED) {
              result.add_edge(n, m).add_attr("color", result.colors[a]);
            }
          }
        }
        return result;
      }

      template <typename Node>
      bool equal_to(WordGraph<Node> const& x,
                    WordGraph<Node> const& y,
                    Node                   first,
                    Node                   last) {
        throw_if_node_out_of_bounds(x, first);
        throw_if_node_out_of_bounds(x, last - 1);
        throw_if_node_out_of_bounds(y, first);
        throw_if_node_out_of_bounds(y, last - 1);
        return equal_to_no_checks(x, y, first, last);
      }

    }  // namespace word_graph

    //////////////////////////////////////////////////////////////////////////////
    // Non-member functions
    //////////////////////////////////////////////////////////////////////////////

    template <typename Node>
    std::ostream& operator<<(std::ostream& os, WordGraph<Node> const& wg) {
      os << to_input_string(wg, "{", "{}", "}");
      return os;
    }

    // TODO(1) refactor to use vectors api, not initializer_list
    template <typename Return>
    std::enable_if_t<is_specialization_of_v<Return, WordGraph>, Return>
    make(size_t                                                      num_nodes,
         std::vector<std::vector<typename Return::node_type>> const& edges) {
      Return result(num_nodes, std::empty(edges) ? 0 : edges.begin()->size());
      for (size_t i = 0; i < edges.size(); ++i) {
        for (size_t j = 0; j < (edges.begin() + i)->size(); ++j) {
          auto val = *((edges.begin() + i)->begin() + j);
          if (val != UNDEFINED) {
            result.target(i, j, *((edges.begin() + i)->begin() + j));
          }
        }
      }
      return result;
    }

    template <typename Return>
    std::enable_if_t<is_specialization_of_v<Return, WordGraph>, Return>
    make(size_t num_nodes,
         std::initializer_list<std::vector<typename Return::node_type>> il) {
      return make<Return>(
          num_nodes, std::vector<std::vector<typename Return::node_type>>(il));
    }

    namespace detail {
      template <typename Subclass>
      template <typename Node1, typename Node2>
      void
      JoinerMeeterCommon<Subclass>::throw_if_bad_args(WordGraph<Node1> const& x,
                                                      Node2 xroot,
                                                      WordGraph<Node1> const& y,
                                                      Node2 yroot) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        libsemigroups::word_graph::throw_if_node_out_of_bounds(x, xroot);
        libsemigroups::word_graph::throw_if_node_out_of_bounds(y, yroot);
        if (x.out_degree() != y.out_degree()) {
          LIBSEMIGROUPS_EXCEPTION(
              "the 2nd and 4th arguments (word graphs) must have the same "
              "out-degree, found out-degrees {} and {}",
              x.out_degree(),
              y.out_degree());
        }
        // The following checks are really required because of the internal
        // implementational details. Basically if x has a target that's out of
        // bounds, then this might be confused for a node in y, and lead to
        // incorrect answers. So best just check this here.
        libsemigroups::word_graph::throw_if_any_target_out_of_bounds(x);
        libsemigroups::word_graph::throw_if_any_target_out_of_bounds(y);
      }

      template <typename Subclass>
      template <typename Node>
      void
      JoinerMeeterCommon<Subclass>::call_no_checks(WordGraph<Node>&       xy,
                                                   WordGraph<Node> const& x,
                                                   Node                   xroot,
                                                   WordGraph<Node> const& y,
                                                   Node yroot) {
        // TODO(1) could be improved by reusing the data used by
        // number_of_nodes_reachable_from.
        static_cast<Subclass&>(*this).call_no_checks(
            xy,
            x,
            word_graph::number_of_nodes_reachable_from(x, xroot),
            xroot,
            y,
            word_graph::number_of_nodes_reachable_from(y, yroot),
            yroot);
      }

      template <typename Subclass>
      template <typename Node1, typename Node2>
      bool JoinerMeeterCommon<Subclass>::is_subrelation_no_checks(
          WordGraph<Node1> const& x,
          Node2                   xroot,
          WordGraph<Node1> const& y,
          Node2                   yroot) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        return static_cast<Subclass&>(*this).is_subrelation_no_checks(
            x,
            word_graph::number_of_nodes_reachable_from(x, xroot),
            xroot,
            y,
            word_graph::number_of_nodes_reachable_from(y, yroot),
            yroot);
      }
    }  // namespace detail

    ////////////////////////////////////////////////////////////////////////////////
    // Joiner
    ////////////////////////////////////////////////////////////////////////////////

    template <typename Node>
    Node Joiner::find(WordGraph<Node> const& x,
                      size_t                 xnum_nodes_reachable_from_root,
                      WordGraph<Node> const& y,
                      uint64_t               n,
                      typename WordGraph<Node>::label_type a) const {
      // Check which word graph q1 and q2 belong to. nodes with labels
      // from 0 to Nx correspond to nodes in x; above Nx corresponds to
      // y.
      Node           na;
      constexpr Node undef = static_cast<Node>(UNDEFINED);
      if (n < xnum_nodes_reachable_from_root) {
        na = x.target_no_checks(n, a);
        if (na != undef) {
          na = _uf.find(na);
        }
      } else {
        na = y.target_no_checks(n - xnum_nodes_reachable_from_root, a);
        if (na != undef) {
          na = _uf.find(na + xnum_nodes_reachable_from_root);
        }
      }
      return na;
    }

    template <typename Node>
    void Joiner::run(WordGraph<Node> const& x,
                     size_t                 xnum_nodes_reachable_from_root,
                     Node                   xroot,
                     WordGraph<Node> const& y,
                     size_t                 ynum_nodes_reachable_from_root,
                     Node                   yroot) {
      using label_type     = typename WordGraph<Node>::label_type;
      constexpr Node undef = static_cast<Node>(UNDEFINED);
      auto const     M     = x.out_degree();
      _uf.init(xnum_nodes_reachable_from_root + ynum_nodes_reachable_from_root);
      _uf.unite(xroot, yroot + xnum_nodes_reachable_from_root);

      // The stack can't be empty if this function runs to the end so no need
      // to do anything.
      LIBSEMIGROUPS_ASSERT(_stck.empty());
      // 0 .. x.number_of_nodes() - 1, x.number_of_nodes()  ..
      //   x.number_of_nodes() + y.number_of_nodes() -1
      _stck.emplace(xroot, yroot + xnum_nodes_reachable_from_root);

      // Traverse x and y, uniting the target nodes at each stage
      while (!_stck.empty()) {
        auto [qx, qy] = _stck.top();
        _stck.pop();
        for (label_type a = 0; a < M; ++a) {
          Node rx = find(x, xnum_nodes_reachable_from_root, y, qx, a);
          Node ry = find(x, xnum_nodes_reachable_from_root, y, qy, a);
          if (rx != ry && rx != undef && ry != undef) {
            _uf.unite(rx, ry);
            _stck.emplace(rx, ry);
          }
        }
      }
    }

    template <typename Node>
    void Joiner::call_no_checks(WordGraph<Node>&       xy,
                                WordGraph<Node> const& x,
                                size_t xnum_nodes_reachable_from_root,
                                Node   xroot,
                                WordGraph<Node> const& y,
                                size_t ynum_nodes_reachable_from_root,
                                Node   yroot) {
      if (xnum_nodes_reachable_from_root > ynum_nodes_reachable_from_root) {
        call_no_checks(xy,
                       y,
                       ynum_nodes_reachable_from_root,
                       yroot,
                       x,
                       xnum_nodes_reachable_from_root,
                       xroot);
        return;
      }
      run(x,
          xnum_nodes_reachable_from_root,
          xroot,
          y,
          ynum_nodes_reachable_from_root,
          yroot);
      _uf.normalize();
      // It can be that _uf is equivalent to [0, 0, 2] at this point (and
      // there's no way for it to not be like this, because 2 doesn't belong
      // to the class of 0), and so we require the following lookup.
      _lookup.resize(xnum_nodes_reachable_from_root);
      LIBSEMIGROUPS_ASSERT(_lookup.size() == xnum_nodes_reachable_from_root);
      std::fill(_lookup.begin(), _lookup.end(), static_cast<Node>(UNDEFINED));
      size_t next_node = 0;

      for (Node s = 0; s < xnum_nodes_reachable_from_root; ++s) {
        auto ss = _uf.find(s);
        if (_lookup[ss] == static_cast<Node>(UNDEFINED)) {
          _lookup[ss] = next_node++;
        }
      }

      xy.init(_uf.number_of_blocks(), x.out_degree());
      for (Node s = 0; s < xnum_nodes_reachable_from_root; ++s) {
        for (auto [a, t] : x.labels_and_targets_no_checks(s)) {
          if (t != static_cast<Node>(UNDEFINED)) {
            xy.target_no_checks(_lookup[_uf.find(s)], a, _lookup[_uf.find(t)]);
          }
        }
      }
    }

    template <typename Node1, typename Node2>
    bool Joiner::is_subrelation_no_checks(WordGraph<Node1> const& x,
                                          size_t xnum_nodes_reachable_from_root,
                                          Node2  xroot,
                                          WordGraph<Node1> const& y,
                                          size_t ynum_nodes_reachable_from_root,
                                          Node2  yroot) {
      static_assert(sizeof(Node2) <= sizeof(Node1));

      if (ynum_nodes_reachable_from_root > xnum_nodes_reachable_from_root) {
        return false;
      }

      run(x,
          xnum_nodes_reachable_from_root,
          static_cast<Node1>(xroot),
          y,
          ynum_nodes_reachable_from_root,
          static_cast<Node1>(yroot));
      // if x is contained in y, then the join of x and y must be y, and
      // hence we just check that the number of nodes in the quotient equals
      // that of y.
      // TODO(2) We could just stop early in "run" if we find that
      // we are trying to merge two nodes of x also.
      return _uf.number_of_blocks() == ynum_nodes_reachable_from_root;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Meeter
    ////////////////////////////////////////////////////////////////////////////////

    template <typename Node>
    void Meeter::call_no_checks(WordGraph<Node>&       xy,
                                WordGraph<Node> const& x,
                                size_t xnum_nodes_reachable_from_root,
                                Node   xroot,
                                WordGraph<Node> const& y,
                                size_t ynum_nodes_reachable_from_root,
                                Node   yroot) {
      Node next = 0;

      _lookup.clear();
      _lookup.emplace(std::pair(xroot, yroot), next++);
      _todo.clear();
      _todo.emplace_back(xroot, yroot);

      size_t const N = x.out_degree();
      LIBSEMIGROUPS_ASSERT(N == y.out_degree());

      xy.init(xnum_nodes_reachable_from_root * ynum_nodes_reachable_from_root,
              N);

      node_type target;
      while (!_todo.empty()) {
        _todo_new.clear();
        for (auto const& source : _todo) {
          auto xy_source = _lookup[source];
          for (size_t a = 0; a < N; ++a) {
            auto xa = x.target_no_checks(source.first, a);
            auto ya = y.target_no_checks(source.second, a);
            if (xa != UNDEFINED && ya != UNDEFINED) {
              target = std::pair(x.target_no_checks(source.first, a),
                                 y.target_no_checks(source.second, a));
              auto [it, inserted] = _lookup.emplace(target, next);

              xy.target_no_checks(xy_source, a, it->second);
              if (inserted) {
                next++;
                _todo_new.push_back(std::move(target));
              }
            }
          }
        }
        std::swap(_todo, _todo_new);
      }
      xy.induced_subgraph_no_checks(0, next);
    }

    template <typename Node1, typename Node2>
    bool Meeter::is_subrelation_no_checks(WordGraph<Node1> const& x,
                                          size_t xnum_nodes_reachable_from_root,
                                          Node2  xroot,
                                          WordGraph<Node1> const& y,
                                          size_t ynum_nodes_reachable_from_root,
                                          Node2  yroot) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      // If x is a subrelation of y, then the meet of x and y must be x.
      if (ynum_nodes_reachable_from_root > xnum_nodes_reachable_from_root) {
        return false;
      }
      auto xy = call_no_checks(x,
                               xnum_nodes_reachable_from_root,
                               xroot,
                               y,
                               ynum_nodes_reachable_from_root,
                               yroot);
      return xy.number_of_nodes() == xnum_nodes_reachable_from_root;
    }

    namespace word_graph {
      template <typename Node>
      WordGraph<Node> random_acyclic(size_t       number_of_nodes,
                                     size_t       out_degree,
                                     std::mt19937 mt) {
        if (number_of_nodes < 2) {
          LIBSEMIGROUPS_EXCEPTION("the 1st parameter `number_of_nodes` must be "
                                  "at least 2, found {}",
                                  number_of_nodes);
        } else if (out_degree < 2) {
          LIBSEMIGROUPS_EXCEPTION("the 2nd parameter `out_degree` must be "
                                  "at least 2, found {}",
                                  out_degree);
        }

        using size_type  = typename WordGraph<Node>::size_type;
        using node_type  = typename WordGraph<Node>::node_type;
        using label_type = typename WordGraph<Node>::label_type;

        static std::uniform_int_distribution<Node> label(0, out_degree - 1);
        static std::uniform_int_distribution<Node> source(0,
                                                          number_of_nodes - 1);
        static std::uniform_int_distribution<Node> target(0,
                                                          number_of_nodes - 2);

        std::vector<size_type> in_degrees(number_of_nodes, 0);
        std::fill(in_degrees.begin() + 1, in_degrees.end(), 1);

        // Start with a word graph where every node is reachable from 0
        WordGraph<Node> wg(0, out_degree);
        add_cycle(wg, number_of_nodes);
        wg.remove_target(number_of_nodes - 1, 0);

        LIBSEMIGROUPS_ASSERT(is_acyclic(wg));
        LIBSEMIGROUPS_ASSERT(is_connected(wg));

        // Is this a good choice?
        size_type T = std::max((number_of_nodes * (number_of_nodes - 1)) / 2,
                               24 * number_of_nodes);

        // TODO(1) use the is_acyclic in the detail namespace to avoid
        // repeatedly allocating memory in the calls to is_acyclic

        for (size_type i = 0; i < T; ++i) {
          node_type const  p = source(mt);
          label_type const a = label(mt);
          node_type        q = target(mt);
          if (q >= p) {
            q++;
          }
          node_type r = wg.target_no_checks(p, a);
          if (r == UNDEFINED) {
            wg.target_no_checks(p, a, q);
            in_degrees[q]++;
            if (is_acyclic(wg)) {
              continue;
            }
            wg.remove_target_no_checks(p, a);
            in_degrees[q]--;
          } else if (r == q) {
            if (in_degrees[q] >= 2) {
              wg.remove_target_no_checks(p, a);
              in_degrees[q]--;
            }
          } else {
            if (in_degrees[r] >= 2) {
              wg.target_no_checks(p, a, q);
              if (!is_acyclic(wg)) {
                wg.target_no_checks(p, a, r);
              } else {
                in_degrees[q]++;
                in_degrees[r]--;
              }
            }
          }
        }
        LIBSEMIGROUPS_ASSERT(is_acyclic(wg));
        LIBSEMIGROUPS_ASSERT(is_connected(wg));
        return wg;
      }

      template <typename Node1, typename Node2>
      void spanning_tree_no_checks(WordGraphView<Node1> const& wg,
                                   Node2                       root,
                                   Forest&                     f) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        using node_type = typename WordGraphView<Node1>::node_type;
        f.init(1);
        size_t const N = wg.number_of_nodes();

        std::queue<node_type> queue;
        queue.push(static_cast<node_type>(root));
        do {
          node_type s = queue.front();
          for (auto [a, t] : wg.labels_and_targets_no_checks(s)) {
            if (t < N && t != static_cast<node_type>(root)) {
              if (t >= f.number_of_nodes()) {
                f.add_nodes(t - f.number_of_nodes() + 1);
              }
              if (f.parent_no_checks(t) == UNDEFINED) {
                f.set_parent_and_label_no_checks(t, s, a);
                queue.push(t);
              }
            }
          }
          queue.pop();
        } while (!queue.empty());
      }

      template <typename Node1, typename Node2>
      void spanning_tree(WordGraphView<Node1> const& wg,
                         Node2                       root,
                         Forest&                     f) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        throw_if_node_out_of_bounds(wg, root);
        return spanning_tree_no_checks(wg, root, f);
      }

      template <typename Node1, typename Node2>
      Forest spanning_tree(WordGraphView<Node1> const& wg, Node2 root) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        Forest f;
        spanning_tree(wg, root, f);
        return f;
      }

      template <typename Node1, typename Node2>
      Forest spanning_tree_no_checks(WordGraphView<Node1> const& wg,
                                     Node2                       root) {
        static_assert(sizeof(Node2) <= sizeof(Node1));
        Forest f;
        spanning_tree_no_checks(wg, root, f);
        return f;
      }
    }  // namespace word_graph

    template <typename Node>
    std::string to_human_readable_repr(WordGraph<Node> const& wg) {
      // TODO(2) could be more elaborate, include complete, etc
      // TODO(2) number_of_edges can be a bit slow
      return fmt::format("<WordGraph with {} nodes, {} edges, & out-degree {}>",
                         detail::group_digits(wg.number_of_nodes()),
                         detail::group_digits(wg.number_of_edges()),
                         wg.out_degree());
    }

    template <typename Node>
    std::string to_input_string(WordGraph<Node> const& wg,
                                std::string const&     prefix,
                                std::string const&     braces,
                                std::string const&     suffix) {
      if (braces.size() != 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 3rd argument (braces) must have length 2, but found {}",
            braces.size());
      }
      std::string out, sep;

      for (auto s : wg.nodes()) {
        auto first = wg.cbegin_targets(s), last = wg.cend_targets(s);
        out += fmt::format("{}{}{}{}",
                           sep,
                           braces[0],
                           fmt::join(first, last, ", "),
                           braces[1]);
        sep = ", ";
      }

      return fmt::format("{}{}, {}{}{}{}",
                         prefix,
                         wg.number_of_nodes(),
                         braces[0],
                         out,
                         braces[1],
                         suffix);
    }
  }  // namespace v4
}  // namespace libsemigroups
