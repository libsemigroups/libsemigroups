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

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>::FelschGraph()
        : Graph(),
          FelschGraphSettings_(),
          // mutable
          _felsch_tree(0),
          _felsch_tree_initted(false),
          // non-mutable
          _definitions(),
          _presentation() {}

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>& FelschGraph<Graph, Definitions>::init() {
      FelschGraphSettings_::init();
      Graph::init();
      // mutable
      _felsch_tree.init(0);
      _felsch_tree_initted = false;

      // non-mutable
      _definitions.clear();
      _presentation.init();
      return *this;
    }

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>::FelschGraph(FelschGraph const&) = default;

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>::FelschGraph(FelschGraph&&) = default;

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>&
    FelschGraph<Graph, Definitions>::operator=(FelschGraph const&)
        = default;

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>&
    FelschGraph<Graph, Definitions>::operator=(FelschGraph&&)
        = default;

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>&
    FelschGraph<Graph, Definitions>::operator=(WordGraph<node_type> const& wg) {
      init();
      Graph::init(wg);

      for (auto s : Graph::nodes()) {
        for (auto a : Graph::labels()) {
          if (target_no_checks(s, a) != UNDEFINED) {
            _definitions.emplace_back(s, a);
          }
        }
      }
      return *this;
    }

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>::FelschGraph(
        Presentation<word_type> const& p)
        : FelschGraph() {
      init(p);
    }

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>&
    FelschGraph<Graph, Definitions>::init(Presentation<word_type> const& p) {
      init();
      WordGraphWithSources<node_type>::init(p.contains_empty_word() ? 0 : 1,
                                            p.alphabet().size());
      presentation_no_checks(p);
      return *this;
    }

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>::~FelschGraph() = default;

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - setters - public
    ////////////////////////////////////////////////////////////////////////

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>&
    FelschGraph<Graph, Definitions>::presentation_no_checks(
        Presentation<word_type> const& p) {
      LIBSEMIGROUPS_ASSERT(p.alphabet().size() == out_degree());
      _presentation = p;
      _felsch_tree.init(_presentation.alphabet().size());
      _felsch_tree_initted = false;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - operators - public
    ////////////////////////////////////////////////////////////////////////

    template <typename Graph, typename Definitions>
    bool
    FelschGraph<Graph, Definitions>::operator==(FelschGraph const& that) const {
      size_type const m = Graph::number_of_active_nodes();
      size_type const n = that.number_of_active_nodes();
      return (m == 0 && n == 0) || (m == n && Graph::operator==(that));
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - accessors - public
    ////////////////////////////////////////////////////////////////////////

    // TODO Move back to hpp
    template <typename Graph, typename Definitions>
    Presentation<word_type>&
    FelschGraph<Graph, Definitions>::presentation() noexcept {
      return _presentation;
    }

    // TODO Move back to hpp
    template <typename Graph, typename Definitions>
    Presentation<word_type> const&
    FelschGraph<Graph, Definitions>::presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    template <typename Graph, typename Definitions>
    template <bool RegDefs>
    FelschGraph<Graph, Definitions>&
    FelschGraph<Graph, Definitions>::possibly_register_target_no_checks(
        node_type  s,
        label_type a,
        node_type  t) noexcept {
      if constexpr (RegDefs) {
        return register_target_no_checks(s, a, t);
      } else {
        Graph::target_no_checks(s, a, t);
        return *this;
      }
    }

    template <typename Graph, typename Definitions>
    FelschGraph<Graph, Definitions>&
    FelschGraph<Graph, Definitions>::register_target_no_checks(
        node_type  s,
        label_type a,
        node_type  t) noexcept {
      LIBSEMIGROUPS_ASSERT(s < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(a < out_degree());
      LIBSEMIGROUPS_ASSERT(t < number_of_nodes());
      _definitions.emplace_back(s, a);
      Graph::target_no_checks(s, a, t);
      return *this;
    }

    template <typename Graph, typename Definitions>
    void
    FelschGraph<Graph, Definitions>::reduce_number_of_edges_to(size_type n) {
      LIBSEMIGROUPS_ASSERT(number_of_edges() == _definitions.size());

      while (_definitions.size() > n) {
        auto const& p = _definitions.back();
        Graph::remove_target_no_checks(p.first, p.second);
        _definitions.pop_back();
      }
    }

    template <typename Graph, typename Definitions>
    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::merge_targets_of_nodes_if_possible(
        node_type      x,
        label_type     a,
        node_type      y,
        label_type     b,
        Incompatible&  incompat,
        PreferredDefs& pref_def) {
      LIBSEMIGROUPS_ASSERT(x < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(y < number_of_nodes());

      node_type xa = (a == UNDEFINED ? x : target_no_checks(x, a));
      node_type yb = (b == UNDEFINED ? y : target_no_checks(y, b));

      if (xa == UNDEFINED && yb != UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(a < out_degree());
        possibly_register_target_no_checks<RegDefs>(x, a, yb);
      } else if (xa != UNDEFINED && yb == UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(b < out_degree());
        possibly_register_target_no_checks<RegDefs>(y, b, xa);
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

    template <typename Graph, typename Definitions>
    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::merge_targets_of_paths_if_possible(
        node_type                          u_node,
        typename word_type::const_iterator u_first,
        typename word_type::const_iterator u_last,
        node_type                          v_node,
        typename word_type::const_iterator v_first,
        typename word_type::const_iterator v_last,
        Incompatible&                      incompat,
        PreferredDefs&                     pref_defs) noexcept {
      LIBSEMIGROUPS_ASSERT(u_node < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(v_node < number_of_nodes());

      node_type  x;
      label_type a;
      if (u_first == u_last) {
        x = u_node;
        a = UNDEFINED;
      } else {
        LIBSEMIGROUPS_ASSERT(u_first < u_last);
        x = word_graph::follow_path_no_checks(
            *this, u_node, u_first, u_last - 1);
        if (x == UNDEFINED) {
          return true;
        }
        a = *(u_last - 1);
        LIBSEMIGROUPS_ASSERT(x < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(a < _presentation.alphabet().size());
      }
      // TODO reduce code dupl
      node_type  y;
      label_type b;
      if (v_first == v_last) {
        y = v_node;
        b = UNDEFINED;
      } else {
        LIBSEMIGROUPS_ASSERT(v_first < v_last);
        y = word_graph::follow_path_no_checks(
            *this, v_node, v_first, v_last - 1);
        if (y == UNDEFINED) {
          return true;
        }
        b = *(v_last - 1);
        LIBSEMIGROUPS_ASSERT(y < number_of_nodes());
        LIBSEMIGROUPS_ASSERT(b < _presentation.alphabet().size());
      }
      return merge_targets_of_nodes_if_possible<RegDefs>(
          x, a, y, b, incompat, pref_defs);
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - process definitions - public
    ////////////////////////////////////////////////////////////////////////

    template <typename Graph, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::process_definitions(
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

    template <typename Graph, typename Definitions>
    bool FelschGraph<Graph, Definitions>::process_definitions(size_t start) {
      StopIfIncompatible incompat;
      NoPreferredDefs    pref_defs;
      return process_definitions(start, incompat, pref_defs);
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - accessors - private
    ////////////////////////////////////////////////////////////////////////

    template <typename Graph, typename Definitions>
    void FelschGraph<Graph, Definitions>::init_felsch_tree() const {
      if (!_felsch_tree_initted) {
        _felsch_tree_initted = true;
        _felsch_tree.add_relations(_presentation.rules.cbegin(),
                                   _presentation.rules.cend());
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - modifiers - private
    ////////////////////////////////////////////////////////////////////////

    // Follows the paths from node c labelled by the left and right handsides
    // of the i-th rule, and returns merge_targets on the last but one nodes
    // and letters.
    template <typename Graph, typename Definitions>
    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    inline bool FelschGraph<Graph, Definitions>::
        merge_targets_of_paths_labelled_by_rules_if_possible(
            node_type const& c,
            size_t           i,
            Incompatible&    incompat,
            PreferredDefs&   pref_defs) noexcept {
      auto j = (i % 2 == 0 ? i + 1 : i - 1);
      return merge_targets_of_paths_if_possible<RegDefs>(
          c,
          _presentation.rules[i].cbegin(),
          _presentation.rules[i].cend(),
          c,
          _presentation.rules[j].cbegin(),
          _presentation.rules[j].cend(),
          incompat,
          pref_defs);
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - process definitions - private
    ////////////////////////////////////////////////////////////////////////

    template <typename Graph, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::process_definition_v2(
        Definition     d,
        Incompatible&  incompat,
        PreferredDefs& pref_defs) {
      felsch_tree().push_back(d.second);
      for (auto it = felsch_tree().cbegin(); it < felsch_tree().cend(); ++it) {
        // Using anything other than NoPreferredDefs here seems to be bad
        // in test case "ACE --- perf602p5 - Felsch", maybe this is a good
        // example where the fill factor would be useful?
        if (!merge_targets_of_paths_labelled_by_rules_if_possible<
                felsch_graph::do_register_defs>(
                d.first, *it, incompat, pref_defs)) {
          return false;
        }
      }
      if (!process_definitions_dfs_v2(d.first, d.first, incompat, pref_defs)) {
        return false;
      }
      return true;
    }

    template <typename Graph, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::process_definition_v1(
        Definition     d,
        Incompatible&  incompat,
        PreferredDefs& pref_defs) {
      felsch_tree().push_back(d.second);
      if (!process_definitions_dfs_v1(d.first, incompat, pref_defs)) {
        return false;
      }
      return true;
    }

    template <typename Graph, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::process_definitions_v2(
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

    // TODO(2) remove code dupl with process_definitions_v2
    template <typename Graph, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::process_definitions_v1(
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

    template <typename Graph, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::process_definitions_dfs_v2(
        node_type      root,
        node_type      c,
        Incompatible&  incompat,
        PreferredDefs& pref_defs) {
      size_t const n = Graph::out_degree();
      for (size_t x = 0; x < n; ++x) {
        node_type e = Graph::first_source_no_checks(c, x);
        if (e != UNDEFINED && felsch_tree().push_front(x)) {
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
          for (auto it = felsch_tree().cbegin(); it < felsch_tree().cend();
               ++it) {
            auto        i = *it;  // good
            auto        j = (i % 2 == 0 ? *it + 1 : *it - 1);
            auto const& u = _presentation.rules[i];
            auto const& v = _presentation.rules[j];
            // LIBSEMIGROUPS_ASSERT(word_graph::follow_path_no_checks(
            //                         *this,
            //                         first_source_no_checks(c, x),
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
            auto u_first = u.cbegin() + felsch_tree().length() - 1;
            auto u_last  = u.cend() - 1;

            if (u_first > u_last) {
              // TODO(2) this shouldn't really be necessary, i.e. I don't think
              // we should ever reach here if this is implemented correctly,
              // this is reached in i.e. Sims1 test case 034.
              // In more words, if felsch_tree().length() (maybe - 1) is longer
              // than u, then we shouldn't be here at all
              continue;
            }

            node_type y = word_graph::follow_path_no_checks(
                *this, root, u_first, u_last);
            LIBSEMIGROUPS_ASSERT(u_first <= u_last || y == root);
            if (y == UNDEFINED) {
              continue;
            }
            u_first = u.cend() - 1;
            u_last  = u.cend();
            e       = Graph::first_source_no_checks(c, x);
            while (e != UNDEFINED) {
              if (!merge_targets_of_paths_if_possible<
                      felsch_graph::do_register_defs>(y,
                                                      u_first,
                                                      u_last,
                                                      e,
                                                      v.cbegin(),
                                                      v.cend(),
                                                      incompat,
                                                      pref_defs)) {
                return false;
              }
              e = Graph::next_source_no_checks(e, x);
            }
          }
          e = Graph::first_source_no_checks(c, x);
          while (e != UNDEFINED) {
            if (!process_definitions_dfs_v2(root, e, incompat, pref_defs)) {
              return false;
            }
            e = Graph::next_source_no_checks(e, x);
          }
          felsch_tree().pop_front();
        }
      }
      return true;
    }

    template <typename Graph, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Graph, Definitions>::process_definitions_dfs_v1(
        node_type      c,
        Incompatible&  incompat,
        PreferredDefs& pref_defs) {
      for (auto it = felsch_tree().cbegin(); it < felsch_tree().cend(); ++it) {
        if (!merge_targets_of_paths_labelled_by_rules_if_possible<
                felsch_graph::do_register_defs>(c, *it, incompat, pref_defs)) {
          return false;
        }
      }

      size_t const n = _presentation.alphabet().size();
      for (size_t x = 0; x < n; ++x) {
        if (felsch_tree().push_front(x)) {
          node_type e = Graph::first_source_no_checks(c, x);
          while (e != UNDEFINED) {
            if (!process_definitions_dfs_v1(e, incompat, pref_defs)) {
              return false;
            }
            e = Graph::next_source_no_checks(e, x);
          }
          felsch_tree().pop_front();
        }
      }
      return true;
    }

    // Helper namespace
    namespace felsch_graph {
      template <bool RegDefs,
                typename Graph,
                typename Definitions,
                typename Incompatible,
                typename PrefDefs>
      bool make_compatible(
          FelschGraph<Graph, Definitions>&                    fd,
          typename FelschGraph<Graph, Definitions>::node_type first_node,
          typename FelschGraph<Graph, Definitions>::node_type last_node,
          typename std::vector<word_type>::const_iterator     first_rule,
          typename std::vector<word_type>::const_iterator     last_rule,
          Incompatible&&                                      incompat,
          PrefDefs&& pref_defs) noexcept {
        LIBSEMIGROUPS_ASSERT(first_node < fd.number_of_nodes());
        LIBSEMIGROUPS_ASSERT(last_node <= fd.number_of_nodes());
        LIBSEMIGROUPS_ASSERT(std::distance(first_rule, last_rule) % 2 == 0);
        using node_type = typename FelschGraph<Graph, Definitions>::node_type;

        for (node_type n = first_node; n < last_node; ++n) {
          for (auto it = first_rule; it != last_rule; it += 2) {
            if (!fd.template merge_targets_of_paths_if_possible<RegDefs,
                                                                Incompatible,
                                                                PrefDefs>(
                    n,
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
    }  // namespace felsch_graph
  }    // namespace detail
}  // namespace libsemigroups
