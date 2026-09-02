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

// This file contains helper functions for word graphs

namespace libsemigroups {
  namespace detail {
    // Forward decl
    template <typename Node>
    class NodeManagedGraph;

    template <typename Subclass>
    template <typename Node>
    void
    JoinerMeeterCommon<Subclass>::throw_if_bad_args(WordGraph<Node> const& x,
                                                    Node xroot,
                                                    WordGraph<Node> const& y,
                                                    Node yroot) {
      throw_if_not_less(xroot, x.number_of_nodes(), "node ");
      throw_if_not_less(yroot, y.number_of_nodes(), "node ");

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
      word_graph::throw_if_any_target_out_of_bounds(
          x, x.cbegin_nodes(), x.cend_nodes());
      word_graph::throw_if_any_target_out_of_bounds(
          y, y.cbegin_nodes(), y.cend_nodes());
    }

    template <typename Subclass>
    template <typename Node>
    void JoinerMeeterCommon<Subclass>::call_no_checks(WordGraph<Node>&       xy,
                                                      WordGraph<Node> const& x,
                                                      Node xroot,
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
    template <typename Node>
    bool JoinerMeeterCommon<Subclass>::is_subrelation_no_checks(
        WordGraph<Node> const& x,
        Node                   xroot,
        WordGraph<Node> const& y,
        Node                   yroot) {
      return static_cast<Subclass&>(*this).is_subrelation_no_checks(
          x,
          word_graph::number_of_nodes_reachable_from(x, xroot),
          xroot,
          y,
          word_graph::number_of_nodes_reachable_from(y, yroot),
          yroot);
    }

    template <typename Graph>
    class Standardizer {
      static_assert(std::is_same_v<std::decay_t<Graph>, Graph>);

      using node_type  = typename Graph::node_type;
      using label_type = typename Graph::label_type;

     public:
      Standardizer(Graph& wg, Forest& f)
          : _forest(f),
            _largest_used_node(0),
            _p(),
            _p_inverse(wg.number_of_nodes(), UNDEFINED),
            _is_non_trivial_permutation(false),
            _wg(wg) {
        _p.resize(max_node() + 1);
        _p[0]         = 0;
        _p_inverse[0] = 0;
      }

      // Follow the edge labelled <x> out of the node currently occupying
      // position <s>. If that edge leads to a node that is larger than
      // <_largest_used_node>, then <_largest_used_node> is incremented and
      // the newly discovered node is set to be the next smallest.
      //
      // Returns true if a previously-unseen node was discovered.
      bool try_set_next_smallest(node_type s, label_type x) {
        node_type const t = _wg.target_no_checks(_p[s], x);
        if (t == UNDEFINED || seen(t)) {
          return false;
        }
        set_next_smallest(t);
        LIBSEMIGROUPS_ASSERT(s != _largest_used_node);
        _forest.set_parent_and_label_no_checks(_largest_used_node, s, x);
        return true;
      }

      // Try to make <t> the next smallest node, with parent _p_inverse[s] and
      // label <x>.
      bool try_set_next_smallest_from_old(node_type  s,
                                          label_type x,
                                          node_type  t) {
        if (seen(t)) {
          return false;
        }
        set_next_smallest(t);
        _forest.set_parent_and_label_no_checks(
            _largest_used_node, _p_inverse[s], x);
        return true;
      }

      node_type largest_used_node() const noexcept {
        return _largest_used_node;
      }

      bool seen(node_type s) const {
        return _p_inverse[s] != UNDEFINED;
      }

      bool stop_early() const {
        return _largest_used_node >= max_node();
      }

      bool standardize() {
        LIBSEMIGROUPS_ASSERT(
            word_graph::number_of_nodes_reachable_from(_wg, node_type(0))
            == _largest_used_node + 1);
        if (_is_non_trivial_permutation) {
          _p.resize(_largest_used_node + 1);
          _wg.standardize_no_checks(_p, _p_inverse);
        }
        return _is_non_trivial_permutation;
      }

     private:
      inline node_type max_node() const noexcept {
        if constexpr (std::is_base_of_v<detail::NodeManagedGraph<node_type>,
                                        Graph>) {
          return _wg.number_of_nodes_active() - 1;
        } else {
          return _wg.number_of_nodes() - 1;
        }
      }

      void set_next_smallest(node_type t) {
        ++_largest_used_node;
        if (_largest_used_node >= _forest.number_of_nodes()) {
          _forest.add_nodes(1);
        }
        _p[_largest_used_node] = t;
        _p_inverse[t]          = _largest_used_node;
        if (t != _largest_used_node) {
          _is_non_trivial_permutation = true;
        }
      }

      Forest&                _forest;
      node_type              _largest_used_node;
      std::vector<node_type> _p;
      std::vector<node_type> _p_inverse;
      bool                   _is_non_trivial_permutation;
      Graph&                 _wg;
    };  // class Standardizer

    // For best performance ensure that <f> has the correct number of nodes
    // when calling this function.
    template <typename Graph>
    bool lenlex_standardize(Graph& wg, Forest& f) {
      LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
      LIBSEMIGROUPS_ASSERT(f.number_of_nodes() != 0);

      using node_type  = typename Graph::node_type;
      using label_type = typename Graph::label_type;

      size_t const        n = wg.out_degree();
      Standardizer<Graph> standardizer(wg, f);

      for (node_type s = 0; s <= standardizer.largest_used_node(); ++s) {
        for (label_type x = 0; x < n; ++x) {
          standardizer.try_set_next_smallest(s, x);
          if (standardizer.stop_early()) {
            return standardizer.standardize();
          }
        }
      }

      return standardizer.standardize();
    }

    template <typename Graph>
    bool lex_standardize(Graph& wg, Forest& f) {
      LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
      LIBSEMIGROUPS_ASSERT(f.number_of_nodes() != 0);

      using node_type  = typename Graph::node_type;
      using label_type = typename Graph::label_type;

      node_type    s = 0;
      label_type   x = 0;
      size_t const n = wg.out_degree();

      Standardizer<Graph> standardizer(wg, f);

      // Perform a DFS through wg
      while (s <= standardizer.largest_used_node()
             && !standardizer.stop_early()) {
        if (standardizer.try_set_next_smallest(s, x)) {
          s = standardizer.largest_used_node();
          x = 0;
          continue;
        }
        x++;
        if (x == n) {  // backtrack
          x = f.label(s);
          s = f.parent(s);
        }
      }
      return standardizer.standardize();
    }

    template <typename Graph>
    bool rpo_standardize(Graph& wg, Forest& f) {
      LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
      LIBSEMIGROUPS_ASSERT(f.number_of_nodes() != 0);

      using node_type  = typename Graph::node_type;
      using label_type = typename Graph::label_type;

      Standardizer<Graph>    standardizer(wg, f);
      size_t const           n = wg.out_degree();
      label_type             x = 0;
      std::vector<node_type> next_node(n, 0);

      while (x < n) {
        bool            changed           = false;
        node_type const largest_this_pass = standardizer.largest_used_node();
        while (next_node[x] <= largest_this_pass) {
          node_type const s = next_node[x];
          ++next_node[x];
          if (standardizer.try_set_next_smallest(s, x)) {
            if (standardizer.stop_early()) {
              return standardizer.standardize();
            }
            changed = true;
          }
        }
        if (changed) {
          x = 0;
        } else {
          ++x;
        }
      }

      return standardizer.standardize();
    }

    template <typename Graph>
    bool rev_rpo_standardize(Graph& wg, Forest& f) {
      LIBSEMIGROUPS_ASSERT(wg.number_of_nodes() != 0);
      LIBSEMIGROUPS_ASSERT(f.number_of_nodes() != 0);

      using node_type  = typename Graph::node_type;
      using label_type = typename Graph::label_type;

      Standardizer<Graph> standardizer(wg, f);
      size_t const        n = wg.out_degree();

      std::vector<node_type> next_node(n, 0);

    // Follow Sims' WREATH_STND literally: each letter keeps its own
    // cursor, and every discovery restarts the sweep from the first
    // letter so earlier frontier words are handled first.
    start_rev_rpo_standardize_search:
      for (label_type x = 0; x < n; ++x) {
        while (next_node[x] <= standardizer.largest_used_node()) {
          node_type const s = next_node[x];
          ++next_node[x];

          if (standardizer.try_set_next_smallest(s, x)) {
            if (standardizer.stop_early()) {
              return standardizer.standardize();
            }
            goto start_rev_rpo_standardize_search;
          }
        }
      }

      return standardizer.standardize();
    }
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////////
  // Helper namespace
  ////////////////////////////////////////////////////////////////////////////

  namespace word_graph {

    template <typename Node, typename Iterator>
    void add_cycle_no_checks(WordGraph<Node>& wg,
                             Iterator         first,
                             Iterator         last) {
      for (auto it = first; it < last - 1; ++it) {
        wg.target(*it, 0, *(it + 1));
      }
      wg.target(*(last - 1), 0, *first);
    }

    // not noexcept because it throws an exception!
    // TODO(v4): rm
    template <typename Node>
    void throw_if_node_out_of_bounds(WordGraph<Node> const& wg, Node v) {
      detail::throw_if_not_less(v, wg.number_of_nodes(), "node ");
    }

    // TODO(v4): rm
    template <typename Node, typename Iterator1, typename Iterator2>
    void throw_if_node_out_of_bounds(WordGraph<Node> const& wg,
                                     Iterator1              first,
                                     Iterator2              last) {
      detail::throw_if_any_not_less(first, last, wg.number_of_nodes(), "node ");
    }

    // TODO(v4): rm
    template <typename Node>
    void
    throw_if_label_out_of_bounds(WordGraph<Node> const&               wg,
                                 typename WordGraph<Node>::label_type lbl) {
      detail::throw_if_not_less(lbl, wg.out_degree(), "label ");
    }

    // TODO(v4): rm
    template <typename Node>
    void throw_if_label_out_of_bounds(WordGraph<Node> const& wg,
                                      word_type const&       word) {
      detail::throw_if_any_not_less(
          word.begin(), word.end(), wg.out_degree(), "label ");
    }

    // TODO(v4): rm
    template <typename Node, typename Iterator>
    void throw_if_label_out_of_bounds(WordGraph<Node> const& wg,
                                      Iterator               first,
                                      Iterator               last) {
      detail::throw_if_any_not_less(first, last, wg.out_degree(), "label ");
    }

    template <typename Graph, typename Cmp>
    bool standardize_no_checks(Graph& wg, Forest& f, Cmp&& cmp) {
      if (wg.number_of_nodes() == 0) {
        return false;
      }

      if (f.number_of_nodes() == 0) {
        f.add_nodes(1);
      }

      // TODO(1): improve this so that we can use the bespoke standardization
      // functions with non-standard alphabets
      // TODO(1): Also catch the functions like lenlex_cmp?
      if constexpr (std::is_same_v<std::decay_t<Cmp>, LenLexCmp<>>) {
        return detail::lenlex_standardize(wg, f);
      } else if (std::is_same_v<std::decay_t<Cmp>, LexCmp<>>) {
        return detail::lex_standardize(wg, f);
      } else if (std::is_same_v<std::decay_t<Cmp>, RPOCmp<>>) {
        return detail::rpo_standardize(wg, f);
      } else if (std::is_same_v<std::decay_t<Cmp>, RevRPOCmp<>>) {
        return detail::rev_rpo_standardize(wg, f);
      }

      using node_type          = typename Graph::node_type;
      using label_type         = typename Graph::label_type;
      using FrontierCandidate_ = detail::FrontierCandidate<node_type>;

      size_t const                    n = wg.out_degree();
      detail::Standardizer<Graph>     standardizer(wg, f);
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
        // NOLINTNEXTLINE(whitespace/braces)
        auto const [current_word, current_node, parent_node] = frontier.back();
        frontier.pop_back();

        if (!current_word.empty()) {
          if (!standardizer.try_set_next_smallest_from_old(
                  parent_node, current_word.back(), current_node)) {
            continue;
          }
        }

        for (label_type x = 0; x < n; ++x) {
          node_type new_node = wg.target_no_checks(current_node, x);
          if (new_node == UNDEFINED || standardizer.seen(new_node)) {
            continue;
          }
          word_type new_word(current_word);
          new_word.push_back(x);
          frontier.emplace_back(new_word, new_node, current_node);
          std::push_heap(
              frontier.begin(), frontier.end(), candidate_comparator);
        }
      }

      return standardizer.standardize();
    }

    template <typename Graph>
    std::pair<bool, Forest> standardize_no_checks(Graph& wg, Order val) {
      Forest f;
      bool   result = standardize_no_checks(wg, f, val);
      return std::make_pair(result, f);
    }

    template <typename Graph>
    bool standardize_no_checks(Graph& wg, Forest& f, Order val) {
      if (wg.number_of_nodes() == 0) {
        return false;
      }

      if (f.number_of_nodes() == 0) {
        f.add_nodes(1);
      }

      switch (val) {
        case Order::none:
          return false;
        case Order::lenlex:
          return detail::lenlex_standardize(wg, f);
        case Order::lex:
          return detail::lex_standardize(wg, f);
        case Order::rpo:
          return detail::rpo_standardize(wg, f);
        case Order::rev_rpo:
          return detail::rev_rpo_standardize(wg, f);
          // Intentional fall-through
        default:
          return false;
      }
    }

    // This must be implemented here because throw_if_any_target_out_of_bounds
    // is declared after standardize in the header.
    template <typename Graph, typename Cmp>
    bool standardize(Graph& wg, Forest& f, Cmp&& cmp) {
      throw_if_any_target_out_of_bounds(wg, wg.cbegin_nodes(), wg.cend_nodes());
      return standardize_no_checks(wg, f, std::forward<Cmp>(cmp));
    }

    // This must be implemented here because throw_if_any_target_out_of_bounds
    // is declared after standardize in the header.
    template <typename Graph, typename Cmp>
    std::pair<bool, Forest> standardize(Graph& wg, Cmp&& cmp) {
      throw_if_any_target_out_of_bounds(wg, wg.cbegin_nodes(), wg.cend_nodes());
      return standardize_no_checks(wg, std::forward<Cmp>(cmp));
    }

    // This must be implemented here because throw_if_any_target_out_of_bounds
    // is declared after standardize in the header.
    template <typename Graph>
    bool standardize(Graph& wg, Forest& f, Order val) {
      throw_if_any_target_out_of_bounds(wg, wg.cbegin_nodes(), wg.cend_nodes());
      return standardize_no_checks(wg, f, val);
    }

    // This must be implemented here because throw_if_any_target_out_of_bounds
    // is declared after standardize in the header.
    template <typename Graph>
    std::pair<bool, Forest> standardize(Graph& wg, Order val) {
      throw_if_any_target_out_of_bounds(wg, wg.cbegin_nodes(), wg.cend_nodes());
      return standardize_no_checks(wg, val);
    }

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
      static std::uniform_int_distribution<Node> source(0, number_of_nodes - 1);
      static std::uniform_int_distribution<Node> target(0, number_of_nodes - 2);

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
  make(size_t                                                         num_nodes,
       std::initializer_list<std::vector<typename Return::node_type>> il) {
    return make<Return>(
        num_nodes, std::vector<std::vector<typename Return::node_type>>(il));
  }

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

  template <typename Node>
  bool Joiner::is_subrelation_no_checks(WordGraph<Node> const& x,
                                        size_t xnum_nodes_reachable_from_root,
                                        Node   xroot,
                                        WordGraph<Node> const& y,
                                        size_t ynum_nodes_reachable_from_root,
                                        Node   yroot) {
    if (ynum_nodes_reachable_from_root > xnum_nodes_reachable_from_root) {
      return false;
    }

    run(x,
        xnum_nodes_reachable_from_root,
        xroot,
        y,
        ynum_nodes_reachable_from_root,
        yroot);
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

    xy.init(xnum_nodes_reachable_from_root * ynum_nodes_reachable_from_root, N);

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

  template <typename Node>
  bool Meeter::is_subrelation_no_checks(WordGraph<Node> const& x,
                                        size_t xnum_nodes_reachable_from_root,
                                        Node   xroot,
                                        WordGraph<Node> const& y,
                                        size_t ynum_nodes_reachable_from_root,
                                        Node   yroot) {
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
      out += fmt::format(
          "{}{}{}{}", sep, braces[0], fmt::join(first, last, ", "), braces[1]);
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
}  // namespace libsemigroups
