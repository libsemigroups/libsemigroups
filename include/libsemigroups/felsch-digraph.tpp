//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-23 James D. Mitchell
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

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////////
  // FelschDigraphSettings
  ////////////////////////////////////////////////////////////////////////////

  template <typename Subclass>
  FelschDigraphSettings<Subclass>& FelschDigraphSettings<Subclass>::init() {
    _def_version = options::def_version::two;
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // FelschDigraph - constructors + initializers
  ////////////////////////////////////////////////////////////////////////

  template <typename Word, typename Node, typename Definitions>
  FelschDigraph<Word, Node, Definitions>&
  FelschDigraph<Word, Node, Definitions>::init() {
    size_t r = (_presentation.contains_empty_word() ? 0 : 1);
    size_t c = _presentation.alphabet().size();

    DigraphWithSources<Node>::init(r, c);
    FelschDigraphSettings_::init();
    _felsch_tree.init(c);
    _felsch_tree.add_relations(_presentation.rules.cbegin(),
                               _presentation.rules.cend());
    return *this;
  }

  template <typename Word, typename Node, typename Definitions>
  FelschDigraph<Word, Node, Definitions>::FelschDigraph(
      Presentation<Word> const& p)
      : DigraphWithSources<node_type>(p.contains_empty_word() ? 0 : 1,
                                      p.alphabet().size()),
        FelschDigraphSettings<FelschDigraph<Word, Node, Definitions>>(),
        _felsch_tree(p.alphabet().size()),
        _presentation(p) {
    _felsch_tree.add_relations(_presentation.rules.cbegin(),
                               _presentation.rules.cend());
  }

  template <typename Word, typename Node, typename Definitions>
  FelschDigraph<Word, Node, Definitions>&
  FelschDigraph<Word, Node, Definitions>::init(Presentation<Word> const& p) {
    if (&_presentation != &p) {
      _presentation = p;
    }
    return init();
  }

  template <typename Word, typename Node, typename Definitions>
  FelschDigraph<Word, Node, Definitions>::FelschDigraph(Presentation<Word>&& p)
      : DigraphWithSources<node_type>(p.contains_empty_word() ? 0 : 1,
                                      p.alphabet().size()),
        FelschDigraphSettings<FelschDigraph<Word, Node, Definitions>>(),
        _felsch_tree(p.alphabet().size()),
        _presentation(std::move(p)) {
    _felsch_tree.add_relations(_presentation.rules.cbegin(),
                               _presentation.rules.cend());
  }

  template <typename Word, typename Node, typename Definitions>
  FelschDigraph<Word, Node, Definitions>&
  FelschDigraph<Word, Node, Definitions>::init(Presentation<Word>&& p) {
    _presentation = std::move(p);
    return init();
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename M>
  FelschDigraph<Word, Node, Definitions>::FelschDigraph(
      ActionDigraph<M> const& ad)
      : DigraphWithSources<node_type>(ad),
        FelschDigraphSettings<FelschDigraph<Word, Node, Definitions>>(),
        _felsch_tree(0),
        _presentation() {
    for (node_type n = 0; n < ad.number_of_nodes(); ++n) {
      for (label_type a = 0; a != ad.out_degree(); ++a) {
        if (ad.unsafe_neighbor(n, a) != UNDEFINED) {
          _definitions.emplace_back(n, a);
        }
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // FelschDigraph - operators
  ////////////////////////////////////////////////////////////////////////

  template <typename Word, typename Node, typename Definitions>
  bool FelschDigraph<Word, Node, Definitions>::operator==(
      FelschDigraph const& that) const {
    size_type const m = this->number_of_active_nodes();
    size_type const n = that.number_of_active_nodes();
    return (m == 0 && n == 0)
           || (m == n && this->ActionDigraph<node_type>::operator==(that));
  }

  ////////////////////////////////////////////////////////////////////////
  // FelschDigraph - accessors
  ////////////////////////////////////////////////////////////////////////

  template <typename Word, typename Node, typename Definitions>
  Presentation<Word>&
  FelschDigraph<Word, Node, Definitions>::presentation() noexcept {
    return _presentation;
  }

  template <typename Word, typename Node, typename Definitions>
  Presentation<Word> const&
  FelschDigraph<Word, Node, Definitions>::presentation() const noexcept {
    return _presentation;
  }

  ////////////////////////////////////////////////////////////////////////
  // FelschDigraph - modifiers
  ////////////////////////////////////////////////////////////////////////

  template <typename Word, typename Node, typename Definitions>
  template <bool RegisterDefs>
  bool FelschDigraph<Word, Node, Definitions>::try_def_edge_nc(
      node_type  c,
      label_type x,
      node_type  d) noexcept {
    LIBSEMIGROUPS_ASSERT(c < ActionDigraph<Node>::number_of_nodes());
    LIBSEMIGROUPS_ASSERT(x < ActionDigraph<Node>::out_degree());
    LIBSEMIGROUPS_ASSERT(d < ActionDigraph<Node>::number_of_nodes());
    node_type cx = ActionDigraph<Node>::unsafe_neighbor(c, x);
    if (cx == UNDEFINED) {
      def_edge_nc<RegisterDefs>(c, x, d);
      return true;
    } else {
      return cx == d;
    }
  }

  template <typename Word, typename Node, typename Definitions>
  template <bool RegisterDefs>
  void
  FelschDigraph<Word, Node, Definitions>::def_edge_nc(node_type  c,
                                                      label_type x,
                                                      node_type  d) noexcept {
    LIBSEMIGROUPS_ASSERT(c < ActionDigraph<Node>::number_of_nodes());
    LIBSEMIGROUPS_ASSERT(x < ActionDigraph<Node>::out_degree());
    LIBSEMIGROUPS_ASSERT(d < ActionDigraph<Node>::number_of_nodes());
    if constexpr (RegisterDefs) {
      _definitions.emplace_back(c, x);
    }
    DigraphWithSources<Node>::add_edge_nc(c, d, x);
  }

  template <typename Word, typename Node, typename Definitions>
  void FelschDigraph<Word, Node, Definitions>::reduce_number_of_edges_to(
      size_type n) {
    LIBSEMIGROUPS_ASSERT(ActionDigraph<Node>::number_of_edges()
                         == _definitions.size());

    while (_definitions.size() > n) {
      auto const& p = _definitions.back();
      this->remove_edge_nc(p.first, p.second);
      _definitions.pop_back();
    }
  }

  template <typename Word, typename Node, typename Definitions>
  FelschDigraph<Word, Node, Definitions>&
  FelschDigraph<Word, Node, Definitions>::presentation(
      Presentation<Word> const& p) {
    _presentation = p;
    size_t c      = _presentation.alphabet().size();
    if (c > ActionDigraph<Node>::out_degree()) {
      // Not sure this is required
      ActionDigraph<Node>::add_to_out_degree(
          c - ActionDigraph<Node>::out_degree());
    }
    _felsch_tree.init(c);
    _felsch_tree.add_relations(_presentation.rules.cbegin(),
                               _presentation.rules.cend());
    return *this;
  }

  template <typename Word, typename Node, typename Definitions>
  FelschDigraph<Word, Node, Definitions>&
  FelschDigraph<Word, Node, Definitions>::presentation(Presentation<Word>&& p) {
    // TODO avoid code dupl with previous fn
    _presentation = std::move(p);
    size_t c      = _presentation.alphabet().size();
    if (c > ActionDigraph<Node>::out_degree()) {
      ActionDigraph<Node>::add_to_out_degree(
          c - ActionDigraph<Node>::out_degree());
    }
    _felsch_tree.init(c);
    _felsch_tree.add_relations(_presentation.rules.cbegin(),
                               _presentation.rules.cend());
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // FelschDigraph - process definitions
  ////////////////////////////////////////////////////////////////////////

  template <typename Word, typename Node, typename Definitions>
  bool
  FelschDigraph<Word, Node, Definitions>::process_definitions(size_t start) {
    StopIfIncompatible incompat;
    NoPreferredDefs    pref_defs;
    return process_definitions(start, incompat, pref_defs);
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool FelschDigraph<Word, Node, Definitions>::process_definitions_v1(
      size_t         start,
      Incompatible&  incompat,
      PreferredDefs& pref_defs) {
    for (size_t i = start; i < _definitions.size(); ++i) {
      if (!process_definition_v1(_definitions[i], incompat, pref_defs)) {
        return false;
      }
    }
    return true;
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool FelschDigraph<Word, Node, Definitions>::process_definitions_dfs_v1(
      node_type      c,
      Incompatible&  incompat,
      PreferredDefs& pref_defs) {
    for (auto it = _felsch_tree.cbegin(); it < _felsch_tree.cend(); ++it) {
      if (!merge_targets_of_paths_labelled_by_rules_if_possible(
              c, *it, incompat, pref_defs)) {
        return false;
      }
    }

    size_t const n = _presentation.alphabet().size();
    for (size_t x = 0; x < n; ++x) {
      if (_felsch_tree.push_front(x)) {
        node_type e = this->first_source(c, x);
        while (e != UNDEFINED) {
          if (!process_definitions_dfs_v1(e, incompat, pref_defs)) {
            return false;
          }
          e = this->next_source(e, x);
        }
        _felsch_tree.pop_front();
      }
    }
    return true;
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool FelschDigraph<Word, Node, Definitions>::process_definitions_v2(
      size_t         start,
      Incompatible&  incompat,
      PreferredDefs& pref_defs) {
    for (size_t i = start; i < _definitions.size(); ++i) {
      if (!process_definition_v2(_definitions[i], incompat, pref_defs)) {
        return false;
      }
    }
    return true;
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool FelschDigraph<Word, Node, Definitions>::process_definitions_dfs_v2(
      node_type      root,
      node_type      c,
      Incompatible&  incompat,
      PreferredDefs& pref_defs) {
    size_t const n = this->out_degree();
    for (size_t x = 0; x < n; ++x) {
      node_type e = DigraphWithSources_::first_source(c, x);
      if (e != UNDEFINED && _felsch_tree.push_front(x)) {
        // We only need to push the side (the good side) of the relation
        // that corresponds to the prefix in the tree through one preimage,
        // because pushing it through any preimage leads to the same place
        // (this is how the preimages/tree works!). If that place is more
        // than one away from the end of the relation, then we don't have to
        // do anything further, i.e. no more pushes of any other preimage or
        // any pushes involving the other side of the relation. Do the
        // pushing of the good side once here and pass the result to the dfs
        // function. Update the pushing above to only do the other (bad)
        // side of the relations.
        for (auto it = _felsch_tree.cbegin(); it < _felsch_tree.cend(); ++it) {
          auto        i = *it;  // good
          auto        j = (i % 2 == 0 ? *it + 1 : *it - 1);
          auto const& u = _presentation.rules[i];
          auto const& v = _presentation.rules[j];
          // LIBSEMIGROUPS_ASSERT(action_digraph_helper::follow_path_nc(
          //                         *this,
          //                         first_source(c, x),
          //                         u.cbegin(),
          //                         u.cbegin() + _felsch_tree.length() - 1)
          //                     == root);
          // Start the push through not at the preimage, but at the original
          // node definition we are processing; again because we know that
          // all paths lead to this node (again by the definition of the
          // search).
          if (u.empty()) {
            continue;
          }
          auto u_first = u.cbegin() + _felsch_tree.length() - 1;
          auto u_last  = u.cend() - 1;

          node_type y = action_digraph_helper::follow_path_nc(
              *this, root, u_first, u_last);
          if (y == UNDEFINED) {
            continue;
          }
          u_first = u.cend() - 1;
          u_last  = u.cend();
          e       = DigraphWithSources_::first_source(c, x);
          while (e != UNDEFINED) {
            if (!merge_targets_of_paths_if_possible(y,
                                                    u_first,
                                                    u_last,
                                                    e,
                                                    v.cbegin(),
                                                    v.cend(),
                                                    incompat,
                                                    pref_defs)) {
              return false;
            }
            e = DigraphWithSources_::next_source(e, x);
          }
        }
        e = DigraphWithSources_::first_source(c, x);
        while (e != UNDEFINED) {
          if (!process_definitions_dfs_v2(root, e, incompat, pref_defs)) {
            return false;
          }
          e = DigraphWithSources_::next_source(e, x);
        }
        _felsch_tree.pop_front();
      }
    }
    return true;
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool
  FelschDigraph<Word, Node, Definitions>::merge_targets_of_paths_if_possible(
      node_type                          u_node,
      typename word_type::const_iterator u_first,
      typename word_type::const_iterator u_last,
      node_type                          v_node,
      typename word_type::const_iterator v_first,
      typename word_type::const_iterator v_last,
      Incompatible&                      incompat,
      PreferredDefs&                     pref_defs) noexcept {
    LIBSEMIGROUPS_ASSERT(u_node < this->number_of_nodes());
    LIBSEMIGROUPS_ASSERT(v_node < this->number_of_nodes());

    node_type  x;
    label_type a;
    if (u_first == u_last) {
      x = u_node;
      a = UNDEFINED;
    } else {
      x = action_digraph_helper::follow_path_nc(
          *this, u_node, u_first, u_last - 1);
      if (x == UNDEFINED) {
        return true;
      }
      a = *(u_last - 1);
      LIBSEMIGROUPS_ASSERT(x < this->number_of_nodes());
      LIBSEMIGROUPS_ASSERT(a < _presentation.alphabet().size());
    }
    // TODO reduce code dupl
    node_type  y;
    label_type b;
    if (v_first == v_last) {
      y = v_node;
      b = UNDEFINED;
    } else {
      y = action_digraph_helper::follow_path_nc(
          *this, v_node, v_first, v_last - 1);
      if (y == UNDEFINED) {
        return true;
      }
      b = *(v_last - 1);
      LIBSEMIGROUPS_ASSERT(y < this->number_of_nodes());
      LIBSEMIGROUPS_ASSERT(b < _presentation.alphabet().size());
    }
    return merge_targets_of_nodes_if_possible(x, a, y, b, incompat, pref_defs);
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool
  FelschDigraph<Word, Node, Definitions>::merge_targets_of_nodes_if_possible(
      node_type      x,
      label_type     a,
      node_type      y,
      label_type     b,
      Incompatible&  incompat,
      PreferredDefs& pref_def) {
    LIBSEMIGROUPS_ASSERT(x < ActionDigraph<Node>::number_of_nodes());
    LIBSEMIGROUPS_ASSERT(y < ActionDigraph<Node>::number_of_nodes());

    node_type xa
        = (a == UNDEFINED ? x : ActionDigraph<Node>::unsafe_neighbor(x, a));
    node_type yb
        = (b == UNDEFINED ? y : ActionDigraph<Node>::unsafe_neighbor(y, b));

    if (xa == UNDEFINED && yb != UNDEFINED) {
      LIBSEMIGROUPS_ASSERT(a < ActionDigraph<Node>::out_degree());
      def_edge_nc(x, a, yb);
    } else if (xa != UNDEFINED && yb == UNDEFINED) {
      LIBSEMIGROUPS_ASSERT(b < ActionDigraph<Node>::out_degree());
      def_edge_nc(y, b, xa);
    } else if (xa != UNDEFINED && yb != UNDEFINED && xa != yb) {
      return incompat(xa, yb);
    } else if (xa == UNDEFINED && yb == UNDEFINED) {
      // We discover that we are one letter away from being able to follow
      // the paths labelled u and v from some node. I.e. u = u_1a and v =
      // v_1b and u_1 and v_1 label (c, x)- and (c, y)-paths but u and v
      // don't label any paths starting at c (i.e there are no edges
      // labelled a incident to x nor labelled b incident to y). This would
      // make the (x, a) and (y, b) "preferred" definitions, or make an
      // immediate definition or do nothing.
      pref_def(x, a, y, b);
    }
    return true;
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool FelschDigraph<Word, Node, Definitions>::process_definitions(
      size_t         start,
      Incompatible&  incompat,
      PreferredDefs& pref_defs) {
    if (def_version() == options::def_version::two) {
      return process_definitions_v2(start, incompat, pref_defs);
    } else {
      LIBSEMIGROUPS_ASSERT(def_version() == options::def_version::one);
      return process_definitions_v1(start, incompat, pref_defs);
    }
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool FelschDigraph<Word, Node, Definitions>::process_definition_v2(
      Definition     d,
      Incompatible&  incompat,
      PreferredDefs& pref_defs) {
    _felsch_tree.push_back(d.second);
    for (auto it = _felsch_tree.cbegin(); it < _felsch_tree.cend(); ++it) {
      // Using anything other than NoPreferredDefs here seems to be bad
      // in test case "ACE --- perf602p5 - Felsch", maybe this is a good
      // example where the fill factor would be useful?
      if (!merge_targets_of_paths_labelled_by_rules_if_possible(
              d.first, *it, incompat, pref_defs)) {
        return false;
      }
    }
    if (!process_definitions_dfs_v2(d.first, d.first, incompat, pref_defs)) {
      return false;
    }
    return true;
  }

  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  bool FelschDigraph<Word, Node, Definitions>::process_definition_v1(
      Definition     d,
      Incompatible&  incompat,
      PreferredDefs& pref_defs) {
    _felsch_tree.push_back(d.second);
    if (!process_definitions_dfs_v1(d.first, incompat, pref_defs)) {
      return false;
    }
    return true;
  }

  // Follows the paths from node c labelled by the left and right handsides
  // of the i-th rule, and returns merge_targets on the last but one nodes
  // and letters.
  template <typename Word, typename Node, typename Definitions>
  template <typename Incompatible, typename PreferredDefs>
  inline bool FelschDigraph<Word, Node, Definitions>::
      merge_targets_of_paths_labelled_by_rules_if_possible(
          node_type const& c,
          size_t           i,
          Incompatible&    incompat,
          PreferredDefs&   pref_defs) noexcept {
    auto j = (i % 2 == 0 ? i + 1 : i - 1);
    return merge_targets_of_paths_if_possible(c,
                                              _presentation.rules[i].cbegin(),
                                              _presentation.rules[i].cend(),
                                              c,
                                              _presentation.rules[j].cbegin(),
                                              _presentation.rules[j].cend(),
                                              incompat,
                                              pref_defs);
  }

  // Helper namespace
  namespace felsch_digraph {
    template <typename Word,
              typename Node,
              typename Definitions,
              typename Incompatible,
              typename PrefDefs>
    bool make_compatible(
        FelschDigraph<Word, Node, Definitions>&                    fd,
        typename FelschDigraph<Word, Node, Definitions>::node_type first_node,
        typename FelschDigraph<Word, Node, Definitions>::node_type last_node,
        typename std::vector<Word>::const_iterator                 first_rule,
        typename std::vector<Word>::const_iterator                 last_rule,
        Incompatible&&                                             incompat,
        PrefDefs&& pref_defs) noexcept {
      LIBSEMIGROUPS_ASSERT(first_node < fd.number_of_nodes());
      LIBSEMIGROUPS_ASSERT(last_node <= fd.number_of_nodes());
      LIBSEMIGROUPS_ASSERT(std::distance(first_rule, last_rule) % 2 == 0);

      for (Node n = first_node; n < last_node; ++n) {
        for (auto it = first_rule; it != last_rule; it += 2) {
          if (!fd.merge_targets_of_paths_if_possible(n,
                                                     it->cbegin(),
                                                     it->cend(),
                                                     n,
                                                     (it + 1)->cbegin(),
                                                     (it + 1)->cend(),
                                                     incompat,
                                                     pref_defs)) {
            return false;
          }
        }
      }
      return true;
    }
  }  // namespace felsch_digraph
}  // namespace libsemigroups
