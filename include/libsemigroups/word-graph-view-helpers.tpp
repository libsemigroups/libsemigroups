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

  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // Private helper classes
    ////////////////////////////////////////////////////////////////////////

    template <typename Node>
    struct FrontierCandidate {
      word_type word;
      Node      node;
      Node      parent;

      FrontierCandidate(word_type w, Node n, Node p)
          : word{w}, node{n}, parent{p} {};
    };

    ////////////////////////////////////////////////////////////////////////
    // Private helper functions
    ////////////////////////////////////////////////////////////////////////

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED

    template <typename Node>
    void init_adjacency_matrix(
        WordGraphView<Node> const&                             wgv,
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& mat) {
      size_t const N = wgv.number_of_nodes_no_checks();
      mat.resize(N, N);
      mat.fill(0);
    }
#else
    template <typename Node>
    void init_adjacency_matrix(WordGraphView<Node> const& wgv,
                               IntMat<0, 0, int64_t>&     mat) {
      size_t const N = wgv.number_of_nodes_no_checks();
      mat            = IntMat<0, 0, int64_t>(N, N);
      std::fill(mat.begin(), mat.end(), 0);
    }
#endif

    // Helper function for the two versions of is_acyclic below.
    // Not noexcept because std::stack::emplace isn't
    // This function does not really need to exist any longer, since
    // topological_sort can be used for the same computation, but we
    // retain it because it was already written and uses less space than
    // topological_sort.
    template <typename Node>
    bool is_acyclic(WordGraphView<Node> const& wgv,
                    std::stack<Node>&          stck,
                    std::vector<Node>&         preorder,
                    Node&                      next_preorder_num,
                    std::vector<Node>&         postorder,
                    Node&                      next_postorder_num) {
      size_t const M = wgv.out_degree_no_checks();
      size_t const N = wgv.number_of_nodes_no_checks();
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
              auto w = wgv.target_no_checks(v, label);
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
    bool is_lenlex_standardized(WordGraphView<Node> const& wgv) {
      LIBSEMIGROUPS_ASSERT(wgv.number_of_nodes_no_checks() != 0);
      Node current_max_node = 0;

      for (Node s = 0; s <= current_max_node; s++) {
        for (auto t : wgv.targets_no_checks(s)) {
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
    bool is_lex_standardized(WordGraphView<Node> const& wg) {
      LIBSEMIGROUPS_ASSERT(wg.number_of_nodes_no_checks() != 0);

      using node_type  = typename WordGraphView<Node>::node_type;
      using label_type = typename WordGraphView<Node>::label_type;

      node_type    s                 = 0;
      label_type   x                 = 0;
      size_t const n                 = wg.out_degree();
      node_type    largest_used_node = 0;

      // TODO(1): replace this with a forest?
      std::vector<node_type>  parents(wg.number_of_nodes());
      std::vector<label_type> labels(wg.number_of_nodes());
      parents[0] = UNDEFINED;
      labels[0]  = 0;

      // Perform a DFS through wg
      while (s <= largest_used_node) {
        node_type const t = wg.target_no_checks(s, x);
        if (t != UNDEFINED && t > largest_used_node) {
          if (t == largest_used_node + 1) {
            ++largest_used_node;
            parents[t] = s;
            labels[t]  = x;
            s          = largest_used_node;
            x          = 0;
            continue;
          } else {
            return false;
          }
        }
        x++;
        if (x == n) {  // backtrack
          x = labels[s];
          s = parents[s];
        }
      }
      return true;
    }

    template <typename Node>
    bool is_rev_rpo_standardized(WordGraphView<Node> const& wg) {
      LIBSEMIGROUPS_ASSERT(wg.number_of_nodes_no_checks() != 0);

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

    template <typename Node>
    bool is_rpo_standardized(WordGraphView<Node> const& wg) {
      LIBSEMIGROUPS_ASSERT(wg.number_of_nodes_no_checks() != 0);

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
    using stack_type
        = std::stack<std::pair<Node, typename WordGraph<Node>::label_type>>;
    using lookup_type = std::vector<uint8_t>;

    // helper function for the public functions below
    template <typename Node>
    bool topological_sort(WordGraphView<Node> const& wgv,
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
        std::tie(e, n) = wgv.next_label_and_target_no_checks(m, e);
        if (n != UNDEFINED) {
          if (seen[n] == 0) {
            // never saw this node before, so dive
            stck.emplace(n, 0);
            goto dive;
          } else if (seen[n] == 1) {
            // => all descendants of n prev. explored and no cycles
            // found
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
      } while (e < wgv.out_degree_no_checks());
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

  namespace word_graph {

    template <typename Node>
    auto adjacency_matrix(WordGraphView<Node> const& wgv) {
      using Mat = typename WordGraph<Node>::adjacency_matrix_type;
      Mat mat;
      detail::init_adjacency_matrix(wgv, mat);

      size_t const N = wgv.number_of_nodes_no_checks();

      for (auto s : wgv.nodes_no_checks()) {
        for (auto t : wgv.targets_no_checks(s)) {
          if (t < N) {
            mat(s, t) += 1;
          }
        }
      }
      return mat;
    }

    template <typename Node>
    typename WordGraph<Node>::adjacency_matrix_type
    adjacency_matrix(WordGraphView<Node> const& wgv) {
      validate(wgv);
      return adjacency_matrix_no_checks(wgv);
    }

    template <typename Node>
    std::unordered_set<Node>
    ancestors_of_no_checks(WordGraphView<Node> const& wgv, Node target) {
      using label_type = typename WordGraph<Node>::label_type;

      size_t const N = wgv.number_of_nodes_no_checks();
      size_t const M = wgv.out_degree_no_checks();

      // Reverse the WordGraph and then just find the nodes reachable from
      // target in the reversed graph. Since the reverse of a WordGraph is no
      // longer a WordGraph we use a vector of vectors here. Alternatively, we
      // could use the technique used in WordGraphWithSources (the sources are
      // essentially the reversed graph) to create the reversed graph (or just
      // use it if we know it already, like in ToddCoxeter).
      std::vector<std::vector<Node>> in_neighbours(N, std::vector<Node>({}));
      for (Node s = 0; s < N; ++s) {
        for (label_type a = 0; a < M; ++a) {
          auto t = wgv.target_no_checks(s, a);
          if (t < N) {
            in_neighbours[t].push_back(s);
          }
        }
      }

      std::unordered_set<Node> seen;
      std::stack<Node>         stack;
      stack.push(target);

      while (!stack.empty()) {
        Node s = stack.top();
        stack.pop();
        if (seen.insert(s).second) {
          for (auto t : in_neighbours[s]) {
            stack.push(t);
          }
        }
      }
      return seen;
    }

    template <typename Node>
    std::unordered_set<Node> ancestors_of(WordGraphView<Node> const& wgv,
                                          Node                       target) {
      detail::throw_if_not_less(
          target, wgv.number_of_nodes_no_checks(), "node ");
      return ancestors_of_no_checks(wgv, target);
    }

    template <typename Node>
    Dot dot_no_checks(WordGraphView<Node> const& wgv) {
      Dot result;
      result.name("WordGraph").kind(Dot::Kind::digraph);
      for (auto n : wgv.nodes_no_checks()) {
        result.add_node(n).add_attr("shape", "box");
      }
      for (auto n : wgv.nodes_no_checks()) {
        for (auto [a, m] : wgv.labels_and_targets_no_checks(n)) {
          if (m != UNDEFINED) {
            result.add_edge(n, m).add_attr("color", result.colors[a]);
          }
        }
      }
      return result;
    }

    template <typename Node>
    Dot dot(WordGraphView<Node> const&      wgv,
            std::vector<std::string> const& node_labels,
            std::vector<std::string> const& edge_labels) {
      if (node_labels.size() != wgv.number_of_nodes_no_checks()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected the 2nd argument (node labels) to have size {}, the "
            "number of nodes of the 1st argument (word graph), but found {}",
            wgv.number_of_nodes_no_checks(),
            node_labels.size());
      } else if (edge_labels.size() != wgv.out_degree()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected the 3rd argument (edge labels) to have size {}, the "
            "out-degree of the 1st argument (word graph), but found {}",
            wgv.out_degree(),
            edge_labels.size());
      } else if (Dot::colors.size() < wgv.out_degree()) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument (word graph) must have out "
                                "degree at most {}, found {}",
                                Dot::colors.size(),
                                wgv.out_degree());
      }

      Dot result = dot(wgv);

      auto const out_degree = wgv.out_degree();
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

    template <typename Node, typename Iterator>
    bool equal_to_no_checks(WordGraphView<Node> const& x,
                            WordGraphView<Node> const& y,
                            Iterator                   first,
                            Iterator                   last) {
      if (x.word_graph() == y.word_graph() && x.start_node() == y.start_node()
          && x.end_node() == y.end_node()) {
        return true;
      } else if (x.number_of_nodes_no_checks() != y.number_of_nodes_no_checks()
                 || x.out_degree_no_checks() != y.out_degree_no_checks()) {
        return false;
      }

      for (auto it = first; it != last; ++it) {
        auto s = *it;
        for (auto const& a : x.labels_no_checks()) {
          if (x.target_no_checks(s, a) != y.target_no_checks(s, a)) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Node, typename Iterator>
    bool equal_to(WordGraphView<Node> const& x,
                  WordGraphView<Node> const& y,
                  Iterator                   first,
                  Iterator                   last) {
      validate(x);
      validate(y);
      detail::throw_if_any_not_less(
          first, last, x.number_of_nodes_no_checks(), "node ");
      return equal_to_no_checks(x, y, first, last);
    }

    template <typename Node, typename Iterator>
    Node follow_path_no_checks(WordGraphView<Node> const& wgv,
                               Node                       source,
                               Iterator                   first,
                               Iterator                   last) noexcept {
      static_assert(sizeof(Node) <= sizeof(size_t));
      if constexpr (detail::HasLessEqual<Iterator, Iterator>::value) {
        if (last <= first) {
          return source;
        }
      }
      size_t const N = wgv.number_of_nodes_no_checks();
      for (auto it = first; it != last && static_cast<size_t>(source) < N;
           ++it) {
        source = wgv.target_no_checks(source, *it);
      }
      if (static_cast<size_t>(source) >= N) {
        return UNDEFINED;
      }
      return source;
    }

    template <typename Node, typename Iterator>
    Node follow_path(WordGraphView<Node> const& wgv,
                     Node                       source,
                     Iterator                   first,
                     Iterator                   last) {
      static_assert(sizeof(Node) <= sizeof(size_t));

      detail::throw_if_not_less(
          source, wgv.number_of_nodes_no_checks(), "node ");
      detail::throw_if_any_not_less(first, last, wgv.out_degree(), "label ");
      return follow_path_no_checks(wgv, source, first, last);
    }

    template <typename Node, typename Iterator>
    std::pair<Node, Iterator>
    last_node_on_path_no_checks(WordGraphView<Node> const& wgv,
                                Node                       source,
                                Iterator                   first,
                                Iterator                   last) noexcept {
      auto it   = first;
      Node prev = source, to = source;
      for (; it < last && to != UNDEFINED; ++it) {
        prev = to;
        to   = wgv.target_no_checks(to, *it);
      }
      if (it != last || to == UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(prev != UNDEFINED);
        return {prev, it - 1};
      } else {
        return {to, it};
      }
    }

    template <typename Node>
    bool is_acyclic(WordGraphView<Node> const& wgv) {
      if (word_graph::is_complete(wgv)) {
        return false;
      }
      auto const        N = wgv.number_of_nodes_no_checks();
      std::stack<Node>  stck;
      std::vector<Node> preorder(N, N);
      Node              next_preorder_num = 0;
      std::vector<Node> postorder(N, N);
      Node              next_postorder_num = 0;

      for (auto m : wgv.nodes_no_checks()) {
        if (preorder[m] == N) {
          stck.push(m);
          if (!detail::is_acyclic(wgv,
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

    template <typename Node>
    bool is_acyclic(WordGraphView<Node> const& wgv, Node source) {
      detail::throw_if_not_less(
          source, wgv.number_of_nodes_no_checks(), "node ");
      auto const       N = wgv.number_of_nodes_no_checks();
      std::stack<Node> stck;
      stck.push(source);
      std::vector<Node> preorder(N, N);
      Node              next_preorder_num = 0;
      std::vector<Node> postorder(N, N);
      Node              next_postorder_num = 0;
      return detail::is_acyclic(wgv,
                                stck,
                                preorder,
                                next_preorder_num,
                                postorder,
                                next_postorder_num);
    }

    template <typename Node>
    bool is_acyclic(WordGraphView<Node> const& wgv, Node source, Node target) {
      detail::throw_if_not_less(
          source, wgv.number_of_nodes_no_checks(), "node ");
      detail::throw_if_not_less(
          target, wgv.number_of_nodes_no_checks(), "node ");
      if (!is_reachable(wgv, source, target)) {
        return true;
      }
      auto const       N = wgv.number_of_nodes_no_checks();
      std::stack<Node> stck;
      stck.push(source);
      std::vector<Node> preorder(N, N);
      Node              next_preorder_num = 0;
      std::vector<Node> postorder(N, N);
      Node              next_postorder_num = 0;

      auto ancestors = ancestors_of_no_checks(wgv, target);
      for (auto n : wgv.nodes_no_checks()) {
        if (ancestors.count(n) == 0) {
          preorder[n] = N + 1;
        }
      }
      return detail::is_acyclic(wgv,
                                stck,
                                preorder,
                                next_preorder_num,
                                postorder,
                                next_postorder_num);
    }

    template <typename Node,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3>
    bool is_compatible_no_checks(WordGraphView<Node> const& wgv,
                                 Iterator1                  first_node,
                                 Iterator2                  last_node,
                                 Iterator3                  first_rule,
                                 Iterator3                  last_rule) {
      for (auto rit = first_rule; rit < last_rule; rit += 2) {
        if (!is_compatible_no_checks(
                wgv, first_node, last_node, *rit, *(rit + 1))) {
          return false;
        }
      }
      return true;
    }

    // TODO(v4) rm
    template <typename Node, typename Iterator1, typename Iterator2>
    bool is_compatible_no_checks(WordGraphView<Node> const& wgv,
                                 Iterator1                  first_node,
                                 Iterator2                  last_node,
                                 word_type const&           lhs,
                                 word_type const&           rhs) {
      for (auto nit = first_node; nit != last_node; ++nit) {
        auto l = word_graph::follow_path_no_checks(
            wgv, *nit, lhs.cbegin(), lhs.cend());
        if (l == UNDEFINED) {
          continue;
        }
        auto r = word_graph::follow_path_no_checks(
            wgv, *nit, rhs.cbegin(), rhs.cend());
        if (r == UNDEFINED) {
          continue;
        }
        if (l != r) {
          return false;
        }
      }
      return true;
    }

    // TODO(v4) rm
    template <typename Node, typename Iterator1, typename Iterator2>
    bool is_compatible(WordGraphView<Node> const& wgv,
                       Iterator1                  first_node,
                       Iterator2                  last_node,
                       word_type const&           lhs,
                       word_type const&           rhs) {
      using detail::throw_if_any_not_less;

      detail::throw_if_any_not_less(
          first_node, last_node, wgv.number_of_nodes_no_checks(), "node ");
      throw_if_any_not_less(lhs.begin(), lhs.end(), wgv.out_degree(), "label ");
      throw_if_any_not_less(rhs.begin(), rhs.end(), wgv.out_degree(), "label ");
      return is_compatible_no_checks(wgv, first_node, last_node, lhs, rhs);
    }

    template <typename Node,
              typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename>
    bool is_compatible(WordGraphView<Node> const& wgv,
                       Iterator1                  first_node,
                       Iterator2                  last_node,
                       Iterator3                  first_rule,
                       Iterator3                  last_rule) {
      using detail::throw_if_any_not_less;

      detail::throw_if_any_not_less(
          first_node, last_node, wgv.number_of_nodes_no_checks(), "node ");
      for (auto rit = first_rule; rit < last_rule; rit += 2) {
        throw_if_any_not_less(
            rit->begin(), rit->end(), wgv.out_degree(), "label ");
        throw_if_any_not_less(
            (rit + 1)->begin(), (rit + 1)->end(), wgv.out_degree(), "label ");
        if (!is_compatible_no_checks(
                wgv, first_node, last_node, *rit, *(rit + 1))) {
          return false;
        }
      }
      return true;
    }

    template <typename Node, typename Iterator1, typename Iterator2>
    bool is_complete_no_checks(WordGraphView<Node> const& wgv,
                               Iterator1                  first_node,
                               Iterator2                  last_node) {
      using label_type = typename WordGraph<Node>::label_type;
      size_t const n   = wgv.out_degree_no_checks();
      for (auto it = first_node; it != last_node; ++it) {
        for (label_type a = 0; a < n; ++a) {
          if (wgv.target_no_checks(*it, a) == UNDEFINED) {
            return false;
          }
        }
      }
      return true;
    }

    template <typename Node, typename Iterator1, typename Iterator2>
    bool is_complete(WordGraphView<Node> const& wgv,
                     Iterator1                  first_node,
                     Iterator2                  last_node) {
      detail::throw_if_any_not_less(
          first_node, last_node, wgv.number_of_nodes_no_checks(), "node ");
      return is_complete_no_checks(wgv, first_node, last_node);
    }

    template <typename Node>
    bool is_connected(WordGraphView<Node> const& wgv) {
      auto const N = wgv.number_of_nodes_no_checks();
      if (N == 0) {
        return true;
      }

      detail::Duf<> uf(N);
      for (auto s : wgv.nodes_no_checks()) {
        for (auto t : wgv.targets_no_checks(s)) {
          if (t < N) {
            uf.unite(s, t);
          }
        }
      }
      return uf.number_of_blocks() == 1;
    }

    template <typename Node>
    bool is_reachable_no_checks(WordGraphView<Node> const& wgv,
                                Node                       source,
                                Node                       target) {
      using label_type = typename WordGraph<Node>::label_type;
      if (source == target) {
        return true;
      }
      label_type             edge = 0;
      std::stack<Node>       nodes;
      std::stack<label_type> edges;
      std::vector<bool>      seen(wgv.number_of_nodes_no_checks(), false);
      nodes.push(source);
      seen[source] = true;

      size_t const N = wgv.number_of_nodes_no_checks();

      do {
        Node node;
        std::tie(edge, node)
            = wgv.next_label_and_target_no_checks(nodes.top(), edge);
        if (node == target) {
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

    template <typename Node>
    bool is_reachable(WordGraphView<Node> const& wgv,
                      Node                       source,
                      Node                       target) {
      detail::throw_if_not_less(
          source, wgv.number_of_nodes_no_checks(), "node ");
      detail::throw_if_not_less(
          target, wgv.number_of_nodes_no_checks(), "node ");
      return is_reachable_no_checks(wgv, source, target);
    }

    // TODO(1) reduce duplication with standardized(Graph&, Cmp cmp)
    template <typename Node, typename Cmp>
    bool is_standardized(WordGraphView<Node> const& wg, Cmp&& cmp) {
      if (wg.number_of_nodes_no_checks() <= 1) {
        return true;
      }

      // TODO(1): improve this so that we can use the bespoke
      // standardization functions with non-standard alphabets
      // TODO(1): Also catch the functions like lenlex_cmp?
      if constexpr (std::is_same_v<std::decay_t<Cmp>, LenLexCmp<>>) {
        return detail::is_lenlex_standardized(wg);
      } else if (std::is_same_v<std::decay_t<Cmp>, LexCmp<>>) {
        return detail::is_lex_standardized(wg);
      } else if (std::is_same_v<std::decay_t<Cmp>, RPOCmp<>>) {
        return detail::is_rpo_standardized(wg);
      } else if (std::is_same_v<std::decay_t<Cmp>, RevRPOCmp<>>) {
        return detail::is_rev_rpo_standardized(wg);
      }

      using node_type          = typename WordGraphView<Node>::node_type;
      using label_type         = typename WordGraphView<Node>::label_type;
      using FrontierCandidate_ = detail::FrontierCandidate<node_type>;

      size_t const                    n                 = wg.out_degree();
      node_type                       largest_used_node = 0;
      std::vector<FrontierCandidate_> frontier{{{}, 0, 0}};
      auto const& candidate_comparator = [&cmp](FrontierCandidate_ const& lhs,
                                                FrontierCandidate_ const& rhs) {
        // We want a min-heap, so we need to return true if lhs >
        // rhs
        if (lhs.word != rhs.word) {
          return cmp(rhs.word, lhs.word);
        } else {
          return lhs.node > rhs.node;
        }
      };
      std::make_heap(frontier.begin(), frontier.end(), candidate_comparator);

      // BFS through wg using a heap, so that the next node that is
      // considered always has the shortest word.
      while (!frontier.empty()) {
        std::pop_heap(frontier.begin(), frontier.end(), candidate_comparator);
        // We don't actually need parent_node at all in this function, but
        // it appears so that FrontierCandidate can be used in both
        // standardize and is_standardized.
        // NOLINTNEXTLINE(whitespace/braces)
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
    // TODO rename to _no_checks and add a checks version?
    bool is_strictly_cyclic(WordGraphView<Node> const& wgv) {
      using node_type = typename WordGraphView<Node>::node_type;
      auto const N    = wgv.number_of_nodes_no_checks();

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
            for (auto t : wgv.targets_no_checks(n)) {
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

    template <typename Node, typename Iterator>
    std::pair<Node, Iterator> last_node_on_path(WordGraphView<Node> const& wgv,
                                                Node     source,
                                                Iterator first,
                                                Iterator last) {
      detail::throw_if_not_less(
          source, wgv.number_of_nodes_no_checks(), "node ");
      detail::throw_if_any_not_less(first, last, wgv.out_degree(), "label ");
      return last_node_on_path_no_checks(wgv, source, first, last);
    }

    // TODO(1) tests
    // TODO(1) version where std::unordered_set is passed by reference, or
    // make this a class that stores its stack and unordered_set, not clear
    // why we'd single out the unordered_set to be passed by reference.
    // TODO(2) version which is an iterator i.e. returns an iterator or range
    // object that allows use to step through the nodes reachable from a given
    // node
    template <typename Node>
    std::unordered_set<Node>
    nodes_reachable_from_no_checks(WordGraphView<Node> const& wgv,
                                   Node                       source,
                                   size_t                     max_depth) {
      std::unordered_set<Node>            seen;
      std::stack<std::pair<Node, size_t>> stack;
      stack.emplace(source, 0);

      size_t const N = wgv.number_of_nodes_no_checks();

      while (!stack.empty()) {
        auto [s, depth] = stack.top();
        stack.pop();
        if (seen.insert(s).second && depth < max_depth) {
          for (auto t : wgv.targets_no_checks(s)) {
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

    template <typename Node>
    void spanning_tree_no_checks(WordGraphView<Node> const& wgv,
                                 Node                       root,
                                 Forest&                    f,
                                 size_t                     max_depth) {
      using node_type = typename WordGraphView<Node>::node_type;
      f.init(1);
      size_t const N = wgv.number_of_nodes_no_checks();

      std::queue<std::pair<node_type, size_t>> queue;
      queue.emplace(static_cast<node_type>(root), 0);

      do {
        auto [s, depth] = queue.front();
        if (depth < max_depth) {
          for (auto [a, t] : wgv.labels_and_targets_no_checks(s)) {
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

    template <typename Node>
    Forest spanning_tree_no_checks(WordGraphView<Node> const& wgv,
                                   Node                       root,
                                   size_t                     max_depth) {
      Forest f;
      spanning_tree_no_checks(wgv, root, f, max_depth);
      return f;
    }

    template <typename Node>
    void spanning_tree(WordGraphView<Node> const& wgv,
                       Node                       root,
                       Forest&                    f,
                       size_t                     max_depth) {
      detail::throw_if_not_less(root, wgv.number_of_nodes_no_checks(), "node ");
      return spanning_tree_no_checks(wgv, root, f, max_depth);
    }

    template <typename Node>
    Forest spanning_tree(WordGraphView<Node> const& wgv,
                         Node                       root,
                         size_t                     max_depth) {
      Forest f;
      spanning_tree(wgv, root, f, max_depth);
      return f;
    }

    template <typename Node, typename Iterator>
    void throw_if_any_target_out_of_bounds(WordGraphView<Node> const& wgv,
                                           Iterator                   first,
                                           Iterator                   last) {
      for (auto it = first; it != last; ++it) {
        auto s = *it;
        for (auto [a, t] : wgv.labels_and_targets_no_checks(s)) {
          if (t != UNDEFINED && t >= wgv.number_of_nodes_no_checks()) {
            LIBSEMIGROUPS_EXCEPTION(
                "target out of bounds, the edge with source {} and label {} "
                "has target {}, but expected value in the range [0, {})",
                s,
                a,
                t,
                wgv.number_of_nodes_no_checks());
          }
        }
      }
    }

    template <typename Node>
    std::vector<Node> topological_sort(WordGraphView<Node> const& wgv) {
      std::vector<Node> order;
      if (word_graph::is_complete(wgv)) {
        return order;
      }

      size_t const             N = wgv.number_of_nodes_no_checks();
      detail::stack_type<Node> stck;
      std::vector<uint8_t>     seen(N, 0);

      for (auto m : wgv.nodes_no_checks()) {
        if (seen[m] == 0) {
          stck.emplace(m, 0);
          if (!detail::topological_sort(wgv, stck, seen, order)) {
            // graph is not acyclic and so there's no topological order for
            // the nodes.
            LIBSEMIGROUPS_ASSERT(order.empty());
            return order;
          }
        }
      }
      LIBSEMIGROUPS_ASSERT(order.size() == wgv.number_of_nodes_no_checks());
      return order;
    }

    // TODO should be _no_checks
    template <typename Node>
    std::vector<Node> topological_sort(WordGraphView<Node> const& wgv,
                                       Node                       source) {
      std::vector<Node> order;
      if (word_graph::is_complete(wgv)) {
        return order;
      }
      size_t const             N = wgv.number_of_nodes_no_checks();
      detail::stack_type<Node> stck;
      std::vector<uint8_t>     seen(N, 0);

      stck.emplace(source, 0);
      detail::topological_sort(wgv, stck, seen, order);
      return order;
    }
  }  // namespace word_graph

  // This function is implemented here because it is declared by for
  // to_input_string in the header.
  template <typename Node>
  std::ostream& operator<<(std::ostream& os, WordGraphView<Node> const& wgv) {
    os << to_input_string(wgv, "{", "{}", "}");
    return os;
  }

  template <typename Node>
  std::string to_input_string(WordGraphView<Node> const& wgv,
                              std::string const&         prefix,
                              std::string const&         braces,
                              std::string const&         suffix) {
    if (braces.size() != 2) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 3rd argument (braces) must have length 2, but found {}",
          braces.size());
    }
    std::string out, sep;

    for (auto s : wgv.nodes()) {
      // TODO(v4) if we start using C++20, maybe we can reuse:
      // auto first = wgv.cbegin_targets(s);
      // auto last  = wgv.cend_targets(s);
      // and then fmt::join(first, last, ", ") instead of
      // fmt::join(targets, ", ") below
      auto targets = wgv.targets(s) | rx::to_vector();
      // This is probably not terrible since hopefully targets is small.
      out += fmt::format(
          "{}{}{}{}", sep, braces[0], fmt::join(targets, ", "), braces[1]);
      sep = ", ";
    }

    return fmt::format("{}{}, {}{}{}{}",
                       prefix,
                       wgv.number_of_nodes(),
                       braces[0],
                       out,
                       braces[1],
                       suffix);
  }

  template <typename Node>
  std::string to_human_readable_repr(WordGraphView<Node> const& wgv) {
    // TODO(2) could be more elaborate, include complete, etc
    // TODO(2) number_of_edges can be a bit slow
    return fmt::format(
        "<WordGraphView with {} nodes, {} edges, & out-degree {}>",
        detail::group_digits(wgv.number_of_nodes()),
        detail::group_digits(wgv.number_of_edges()),
        wgv.out_degree());
  }
}  // namespace libsemigroups
