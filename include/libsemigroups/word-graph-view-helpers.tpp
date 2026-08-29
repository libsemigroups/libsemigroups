//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 Nadim Searight
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

// This file contains helper functions for word graph views

namespace libsemigroups {

  namespace word_graph {

    template <typename Node>
    // TODO rename to _no_checks and add a checks version?
    bool is_strictly_cyclic(WordGraphView<Node> const& wg) {
      using node_type = typename WordGraphView<Node>::node_type;
      auto const N    = wg.number_of_nodes_no_checks();

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
      // TODO rename to _no_checks and add a checks version?
      bool is_lenlex_standardized(WordGraphView<Node> const& wg) {
        Node current_max_node = 0;

        for (auto s : wg.nodes_no_checks()) {
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

      template <typename Node>
      bool is_rpo_standardized(WordGraphView<Node> const& wg) {
        if (wg.number_of_nodes_no_checks() == 0) {
          return true;
        }

        using node_type  = typename WordGraphView<Node>::node_type;
        using label_type = typename WordGraphView<Node>::label_type;

        size_t const           n                 = wg.out_degree();
        label_type             x                 = 0;
        node_type              largest_used_node = 0;
        std::vector<node_type> next_node(n, 0);

        while (x < n) {
          bool            changed           = false;
          node_type const largest_this_pass = largest_used_node;
          while (next_node[x] <= largest_this_pass) {
            node_type const s = next_node[x];
            ++next_node[x];
            node_type const t = wg.target_no_checks(s, x);
            if (t == UNDEFINED || t <= largest_used_node) {
              continue;
            }
            if (t == largest_used_node + 1) {
              changed = true;
              ++largest_used_node;
            } else {
              return false;
            }
          }
          if (changed) {
            x = 0;
          } else {
            ++x;
          }
        }

        return true;
      }

      template <typename Node>
      bool is_rev_rpo_standardized(WordGraphView<Node> const& wg) {
        if (wg.number_of_nodes_no_checks() == 0) {
          return true;
        }

        using node_type  = typename WordGraphView<Node>::node_type;
        using label_type = typename WordGraphView<Node>::label_type;

        size_t const           n                 = wg.out_degree();
        node_type              largest_used_node = 0;
        std::vector<node_type> next_node(n, 0);

      start_is_rev_rpo_standardize_search:
        for (label_type x = 0; x < n; ++x) {
          while (next_node[x] <= largest_used_node) {
            node_type const s = next_node[x];
            ++next_node[x];
            node_type const t = wg.target_no_checks(s, x);
            if (t == UNDEFINED || t <= largest_used_node) {
              continue;
            }
            if (t == largest_used_node + 1) {
              ++largest_used_node;
              goto start_is_rev_rpo_standardize_search;
            } else {
              return false;
            }
          }
        }

        return true;
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
        size_t const M = wg.out_degree_no_checks();
        size_t const N = wg.number_of_nodes_no_checks();
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
        } while (e < wg.out_degree_no_checks());
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
          WordGraphView<Node> const&                             wg,
          Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& mat) {
        size_t const N = wg.number_of_nodes_no_checks();
        mat.resize(N, N);
        mat.fill(0);
      }
#else
      template <typename Node>
      void init_adjacency_matrix(WordGraphView<Node> const& wg,
                                 IntMat<0, 0, int64_t>&     mat) {
        size_t const N = wg.number_of_nodes_no_checks();
        mat            = IntMat<0, 0, int64_t>(N, N);
        std::fill(mat.begin(), mat.end(), 0);
      }
#endif

      template <typename Node>
      struct FrontierCandidate {
        word_type word;
        Node      node;
        Node      parent;

        FrontierCandidate(word_type w, Node n, Node p)
            : word{w}, node{n}, parent{p} {};
      };

    }  // namespace detail

    // TODO(1) reduce duplication with standardized(Graph&, Cmp cmp)
    template <typename Node, typename Cmp>
    bool is_standardized(WordGraphView<Node> const& wg, Cmp cmp) {
      using node_type          = typename WordGraphView<Node>::node_type;
      using label_type         = typename WordGraphView<Node>::label_type;
      using FrontierCandidate_ = detail::FrontierCandidate<node_type>;

      size_t const                    n                 = wg.out_degree();
      node_type                       largest_used_node = 0;
      std::vector<FrontierCandidate_> frontier{{{}, 0, 0}};
      auto const& candidate_comparator = [&cmp](FrontierCandidate_ const& lhs,
                                                FrontierCandidate_ const& rhs) {
        // We want a min-heap, so we need to return true if lhs > rhs
        if (lhs.word != rhs.word) {
          return cmp(rhs.word, lhs.word);
        } else {
          return lhs.node > rhs.node;
        }
      };
      std::make_heap(frontier.begin(), frontier.end(), candidate_comparator);

      // BFS through wg using a heap, so that the next node that is considered
      // always has the shortest word.
      while (!frontier.empty()) {
        std::pop_heap(frontier.begin(), frontier.end(), candidate_comparator);
        // We don't actually need parent_node at all in this function, but it
        // appears so that FrontierCandidate can be used in both standardize and
        // is_standardized.
        auto const [current_word, current_node, parent_node] = frontier.back();
        frontier.pop_back();

        if (!current_word.empty()) {
          if (current_node <= largest_used_node) {
            continue;
          }
          if (current_node == largest_used_node + 1) {
            ++largest_used_node;
          } else {
            return false;
          }
        }

        for (label_type x = 0; x < n; ++x) {
          node_type new_node = wg.target_no_checks(current_node, x);
          if (new_node == UNDEFINED || new_node <= largest_used_node) {
            continue;
          }
          word_type new_word(current_word);
          new_word.push_back(x);
          frontier.emplace_back(new_word, new_node, current_node);
          std::push_heap(
              frontier.begin(), frontier.end(), candidate_comparator);
        }
      }

      return true;
    }

    template <typename Node>
    bool is_standardized(WordGraphView<Node> const& wg, Order val) {
      switch (val) {
        case Order::none:
          return true;
        case Order::lenlex:
          return detail::is_lenlex_standardized(wg);
        case Order::rpo:
          return detail::is_rpo_standardized(wg);
        case Order::rev_rpo:
          return detail::is_rev_rpo_standardized(wg);
        case Order::lex:
        default:
          LIBSEMIGROUPS_EXCEPTION("not yet implemented")
      }
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
      wg.throw_if_node_out_of_bounds(first_node, last_node);
      wg.throw_if_label_out_of_bounds(lhs);
      wg.throw_if_label_out_of_bounds(rhs);
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
      wg.throw_if_node_out_of_bounds(first_node, last_node);
      for (auto rit = first_rule; rit < last_rule; rit += 2) {
        wg.throw_if_label_out_of_bounds(*rit);
        wg.throw_if_label_out_of_bounds(*(rit + 1));
        if (!is_compatible_no_checks(
                wg, first_node, last_node, *rit, *(rit + 1))) {
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
      size_t const n   = wg.out_degree_no_checks();
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
      wg.throw_if_node_out_of_bounds(first_node, last_node);
      return is_complete_no_checks(wg, first_node, last_node);
    }

    template <typename Node>
    bool is_connected(WordGraphView<Node> const& wg) {
      auto const N = wg.number_of_nodes_no_checks();
      if (N == 0) {
        return true;
      }

      ::libsemigroups::detail::Duf<> uf(N);
      for (auto s : wg.nodes_no_checks()) {
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
      std::vector<bool>      seen(wg.number_of_nodes_no_checks(), false);
      nodes.push(source);
      seen[source] = true;

      size_t const N = wg.number_of_nodes_no_checks();

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
      wg.throw_if_node_out_of_bounds(static_cast<Node1>(source));
      wg.throw_if_node_out_of_bounds(static_cast<Node1>(target));
      return is_reachable_no_checks(wg, source, target);
    }

    template <typename Node>
    bool is_acyclic(WordGraphView<Node> const& wg) {
      if (word_graph::is_complete(wg)) {
        return false;
      }
      auto const        N = wg.number_of_nodes_no_checks();
      std::stack<Node>  stck;
      std::vector<Node> preorder(N, N);
      Node              next_preorder_num = 0;
      std::vector<Node> postorder(N, N);
      Node              next_postorder_num = 0;

      for (auto m : wg.nodes_no_checks()) {
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
      wg.throw_if_node_out_of_bounds(static_cast<Node1>(source));
      auto const        N = wg.number_of_nodes_no_checks();
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
    bool is_acyclic(WordGraphView<Node1> const& wg,
                    Node2                       source,
                    Node2                       target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      wg.throw_if_node_out_of_bounds(static_cast<Node1>(source));
      wg.throw_if_node_out_of_bounds(static_cast<Node1>(target));
      if (!is_reachable(wg, source, target)) {
        return true;
      }
      auto const        N = wg.number_of_nodes_no_checks();
      std::stack<Node1> stck;
      stck.push(source);
      std::vector<Node1> preorder(N, N);
      Node1              next_preorder_num = 0;
      std::vector<Node1> postorder(N, N);
      Node1              next_postorder_num = 0;

      auto ancestors = ancestors_of_no_checks(wg, target);
      for (auto n : wg.nodes_no_checks()) {
        if (ancestors.count(n) == 0) {
          preorder[n] = N + 1;
        }
      }
      return detail::is_acyclic(
          wg, stck, preorder, next_preorder_num, postorder, next_postorder_num);
    }

    template <typename Node>
    std::vector<Node> topological_sort(WordGraphView<Node> const& wg) {
      std::vector<Node> order;
      if (word_graph::is_complete(wg)) {
        return order;
      }

      size_t const             N = wg.number_of_nodes_no_checks();
      detail::stack_type<Node> stck;
      std::vector<uint8_t>     seen(N, 0);

      for (auto m : wg.nodes_no_checks()) {
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
      LIBSEMIGROUPS_ASSERT(order.size() == wg.number_of_nodes_no_checks());
      return order;
    }

    // TODO should be _no_checks
    template <typename Node1, typename Node2>
    std::vector<Node1> topological_sort(WordGraphView<Node1> const& wg,
                                        Node2                       source) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      std::vector<Node1> order;
      if (word_graph::is_complete(wg)) {
        return order;
      }
      size_t const              N = wg.number_of_nodes_no_checks();
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

      size_t const N = wg.number_of_nodes_no_checks();

      for (auto s : wg.nodes_no_checks()) {
        for (auto t : wg.targets_no_checks(s)) {
          if (t < N) {
            mat(s, t) += 1;
          }
        }
      }
      return mat;
    }

    template <typename Node1, typename Node2>
    std::unordered_set<Node1>
    nodes_reachable_from_no_checks(WordGraphView<Node1> const& wg,
                                   Node2                       source,
                                   size_t                      max_depth) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      std::unordered_set<Node1>            seen;
      std::stack<std::pair<Node1, size_t>> stack;
      stack.emplace(source, 0);

      size_t const N = wg.number_of_nodes_no_checks();

      while (!stack.empty()) {
        auto [s, depth] = stack.top();
        stack.pop();
        if (seen.insert(s).second && depth < max_depth) {
          for (auto t : wg.targets_no_checks(s)) {
            if (t < N) {
              // If we did see "t" before it "depth + 1" will never be
              // accessed again, so it doesn't matter that it is wrong. O/w if
              // "t" has not been seen before the "depth + 1" is its correct
              // depth.
              stack.emplace(t, depth + 1);
            }
          }
        }
      }
      return seen;
    }

    template <typename Node1, typename Node2>
    std::unordered_set<Node1>
    ancestors_of_no_checks(WordGraphView<Node1> const& wg, Node2 target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      using label_type = typename WordGraph<Node1>::label_type;

      size_t const N = wg.number_of_nodes_no_checks();
      size_t const M = wg.out_degree_no_checks();

      // Reverse the WordGraph and then just find the nodes reachable from
      // target in the reversed graph. Since the reverse of a WordGraph is no
      // longer a WordGraph we use a vector of vectors here. Alternatively, we
      // could use the technique used in WordGraphWithSources (the sources are
      // essentially the reversed graph) to create the reversed graph (or just
      // use it if we know it already, like in ToddCoxeter).
      std::vector<std::vector<Node1>> in_neighbours(N, std::vector<Node1>({}));
      for (Node1 s = 0; s < N; ++s) {
        for (label_type a = 0; a < M; ++a) {
          auto t = wg.target_no_checks(s, a);
          if (t < N) {
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
                                           Node2                       target) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      wg.throw_if_node_out_of_bounds(static_cast<Node1>(target));
      return ancestors_of_no_checks(wg, target);
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
      size_t const N = wg.number_of_nodes_no_checks();
      for (auto it = first; it != last && static_cast<size_t>(from) < N; ++it) {
        from = wg.target_no_checks(from, *it);
      }
      if (static_cast<size_t>(from) >= N) {
        return UNDEFINED;
      }
      return from;
    }

    template <typename Node1, typename Node2, typename Iterator>
    Node1 follow_path(WordGraphView<Node1> const& wg,
                      Node2                       from,
                      Iterator                    first,
                      Iterator                    last) {
      static_assert(sizeof(Node1) <= sizeof(size_t));
      static_assert(sizeof(Node2) <= sizeof(Node1));

      wg.throw_if_node_out_of_bounds(from);
      wg.throw_if_label_out_of_bounds(first, last);
      return follow_path_no_checks(wg, from, first, last);
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
    std::pair<Node1, Iterator> last_node_on_path(WordGraphView<Node1> const& wg,
                                                 Node2    from,
                                                 Iterator first,
                                                 Iterator last) {
      wg.throw_if_node_out_of_bounds(from);
      wg.throw_if_label_out_of_bounds(first, last);
      return last_node_on_path_no_checks(wg, from, first, last);
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
      for (auto n : wg.nodes_no_checks()) {
        result.add_node(n).add_attr("shape", "box");
      }
      for (auto n : wg.nodes_no_checks()) {
        for (auto [a, m] : wg.labels_and_targets_no_checks(n)) {
          if (m != UNDEFINED) {
            result.add_edge(n, m).add_attr("color", result.colors[a]);
          }
        }
      }
      return result;
    }

    template <typename Node>
    Dot dot(WordGraphView<Node> const&      wg,
            std::vector<std::string> const& node_labels,
            std::vector<std::string> const& edge_labels) {
      if (node_labels.size() != wg.number_of_nodes()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected the 2nd argument (node labels) to have size {}, the "
            "number of nodes of the 1st argument (word graph), but found {}",
            wg.number_of_nodes(),
            node_labels.size());
      } else if (edge_labels.size() != wg.out_degree()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected the 3rd argument (edge labels) to have size {}, the "
            "out-degree of the 1st argument (word graph), but found {}",
            wg.out_degree(),
            edge_labels.size());
      } else if (Dot::colors.size() < wg.out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument (word graph) must have out "
                                "degree at most {}, found {}",
                                Dot::colors.size(),
                                wg.out_degree());
      }

      Dot result = dot(wg);

      auto const out_degree = wg.out_degree();
      size_t     i          = 0;
      for (auto& node : result.nodes()) {
        node.add_attr("label", node_labels[i++]);
      }

      auto start_table = "<<table border=\"0\" cellpadding=\"2\" "
                         "cellspacing=\"0\" cellborder=\"0\">\n";
      auto end_table   = "</table>>\n";

      std::string label = start_table;
      for (size_t index = 0; index < out_degree; ++index) {
        label += fmt::format(
            "<tr><td align=\"right\" port=\"port{}\">{}&nbsp;</td></tr>\n",
            index,
            edge_labels[index]);
      }
      label += end_table;

      Dot legend;
      legend.name("legend").add_attr("node [shape=plaintext]");

      // HTML table for the head of the arrows in the legend
      legend.add_node("head").add_attr("label", label, Dot::Attr::html);

      label = start_table;
      for (size_t index = 0; index < out_degree; ++index) {
        label += fmt::format(
            "<tr><td align=\"right\" port=\"port{}\">&nbsp;</td></tr>\n",
            index);
      }
      label += end_table;

      // HTML table for the tail of the arrows in the legend
      legend.add_node("tail").add_attr("label", label, Dot::Attr::html);
      for (size_t index = 0; index < out_degree; ++index) {
        legend
            .add_edge(fmt::format("head:port{}:e", index),
                      fmt::format("tail:port{}:w", index))
            .add_attr("color", result.colors[index]);
      }
      result.add_subgraph(legend);
      return result;
    }

    template <typename Node1, typename Node2>
    void spanning_tree_no_checks(WordGraphView<Node1> const& wg,
                                 Node2                       root,
                                 Forest&                     f,
                                 size_t                      max_depth) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      using node_type = typename WordGraphView<Node1>::node_type;
      f.init(1);
      size_t const N = wg.number_of_nodes_no_checks();

      std::queue<std::pair<node_type, size_t>> queue;
      queue.emplace(static_cast<node_type>(root), 0);

      do {
        auto [s, depth] = queue.front();
        if (depth < max_depth) {
          for (auto [a, t] : wg.labels_and_targets_no_checks(s)) {
            if (t < N && t != static_cast<node_type>(root)) {
              if (t >= f.number_of_nodes()) {
                f.add_nodes(t - f.number_of_nodes() + 1);
              }
              if (f.parent_no_checks(t) == UNDEFINED) {
                f.set_parent_and_label_no_checks(t, s, a);
                queue.emplace(t, depth + 1);
              }
            }
          }
        }
        queue.pop();
      } while (!queue.empty());
    }

    template <typename Node1, typename Node2>
    void spanning_tree(WordGraphView<Node1> const& wg,
                       Node2                       root,
                       Forest&                     f,
                       size_t                      max_depth) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      wg.throw_if_node_out_of_bounds(root);
      return spanning_tree_no_checks(wg, root, f, max_depth);
    }

    template <typename Node1, typename Node2>
    Forest spanning_tree(WordGraphView<Node1> const& wg,
                         Node2                       root,
                         size_t                      max_depth) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      Forest f;
      spanning_tree(wg, root, f, max_depth);
      return f;
    }

    template <typename Node1, typename Node2>
    Forest spanning_tree_no_checks(WordGraphView<Node1> const& wg,
                                   Node2                       root,
                                   size_t                      max_depth) {
      static_assert(sizeof(Node2) <= sizeof(Node1));
      Forest f;
      spanning_tree_no_checks(wg, root, f, max_depth);
      return f;
    }

  }  // namespace word_graph

}  // namespace libsemigroups
