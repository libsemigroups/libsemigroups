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
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>::FelschGraph(FelschGraph const&)
        = default;

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>::FelschGraph(FelschGraph&&) = default;

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::operator=(FelschGraph const&)
        = default;

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::operator=(FelschGraph&&)
        = default;

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::init() {
      WordGraphWithSources<Node>::init();
      FelschGraphSettings_::init();
      _definitions.clear();
      _felsch_tree.init(0);
      _felsch_tree_initted = false;
      _presentation.init();
      return *this;
    }

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>::FelschGraph(
        Presentation<Word> const& p)
        : FelschGraph() {
      init(p);
    }

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::init(Presentation<Word> const& p) {
      if (&_presentation != &p) {
        _presentation = p;
      }
      return private_init_from_presentation();
    }

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>::FelschGraph(Presentation<Word>&& p)
        : FelschGraph() {
      init(std::move(p));
    }

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::init(Presentation<Word>&& p) {
      _presentation = std::move(p);
      return private_init_from_presentation();
    }

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    template <typename M>
    FelschGraph<Word, Node, Definitions>::FelschGraph(WordGraph<M> const& ad)
        : FelschGraph() {
      init(ad);
    }

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    template <typename M>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::init(WordGraph<M> const& ad) {
      WordGraphWithSources<M>::init(ad);
      return private_init_from_word_graph();
    }

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>::FelschGraph(WordGraph<Node>&& ad)
        : FelschGraph() {
      init(std::move(ad));
    }

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::init(WordGraph<Node>&& ad) {
      WordGraphWithSources<Node>::init(std::move(ad));
      return private_init_from_word_graph();
    }

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>::~FelschGraph() = default;

    // TODO this should be removed
    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::presentation(
        Presentation<Word> const& p) {
      _presentation = p;
      size_t c      = _presentation.alphabet().size();
      if (c > out_degree()) {
        // Not sure this is required
        WordGraph<Node>::add_to_out_degree(c - out_degree());
      }
      _felsch_tree.init(c);
      _felsch_tree_initted = false;
      return *this;
    }

    // TODO this should be removed
    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::presentation(Presentation<Word>&& p) {
      // TODO avoid code dupl in constructors and init
      _presentation = std::move(p);
      size_t c      = _presentation.alphabet().size();
      if (c > out_degree()) {
        WordGraph<Node>::add_to_out_degree(c - out_degree());
      }
      _felsch_tree.init(c);
      _felsch_tree_initted = false;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - operators - public
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename Node, typename Definitions>
    bool FelschGraph<Word, Node, Definitions>::operator==(
        FelschGraph const& that) const {
      size_type const m = WordGraph<Node>::number_of_active_nodes();
      size_type const n = that.number_of_active_nodes();
      return (m == 0 && n == 0)
             || (m == n && WordGraphWithSources<node_type>::operator==(that));
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - accessors - public
    ////////////////////////////////////////////////////////////////////////

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    Presentation<Word>&
    FelschGraph<Word, Node, Definitions>::presentation() noexcept {
      return _presentation;
    }

    // TODO Move back to hpp
    template <typename Word, typename Node, typename Definitions>
    Presentation<Word> const&
    FelschGraph<Word, Node, Definitions>::presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename Node, typename Definitions>
    template <bool RegDefs>
    void FelschGraph<Word, Node, Definitions>::target_no_checks(
        node_type  c,
        label_type x,
        node_type  d) noexcept {
      LIBSEMIGROUPS_ASSERT(c < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(x < out_degree());
      LIBSEMIGROUPS_ASSERT(d < number_of_nodes());
      if constexpr (RegDefs) {
        _definitions.emplace_back(c, x);
      }
      WordGraphWithSources<Node>::target_no_checks(c, x, d);
    }

    template <typename Word, typename Node, typename Definitions>
    void FelschGraph<Word, Node, Definitions>::reduce_number_of_edges_to(
        size_type n) {
      LIBSEMIGROUPS_ASSERT(WordGraph<Node>::number_of_edges()
                           == _definitions.size());

      while (_definitions.size() > n) {
        auto const& p = _definitions.back();
        WordGraphWithSources<Node>::remove_target_no_checks(p.first, p.second);
        _definitions.pop_back();
      }
    }

    template <typename Word, typename Node, typename Definitions>
    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    bool
    FelschGraph<Word, Node, Definitions>::merge_targets_of_nodes_if_possible(
        node_type      x,
        label_type     a,
        node_type      y,
        label_type     b,
        Incompatible&  incompat,
        PreferredDefs& pref_def) {
      LIBSEMIGROUPS_ASSERT(x < number_of_nodes());
      LIBSEMIGROUPS_ASSERT(y < number_of_nodes());

      node_type xa
          = (a == UNDEFINED ? x : WordGraph<Node>::target_no_checks(x, a));
      node_type yb
          = (b == UNDEFINED ? y : WordGraph<Node>::target_no_checks(y, b));

      if (xa == UNDEFINED && yb != UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(a < out_degree());
        target_no_checks<RegDefs>(x, a, yb);
      } else if (xa != UNDEFINED && yb == UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(b < out_degree());
        target_no_checks<RegDefs>(y, b, xa);
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
    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    bool
    FelschGraph<Word, Node, Definitions>::merge_targets_of_paths_if_possible(
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

    template <typename Word, typename Node, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Word, Node, Definitions>::process_definitions(
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
    bool
    FelschGraph<Word, Node, Definitions>::process_definitions(size_t start) {
      StopIfIncompatible incompat;
      NoPreferredDefs    pref_defs;
      return process_definitions(start, incompat, pref_defs);
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - constructors + initializers - private
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::private_init_from_presentation() {
#ifdef LIBSEMIGROUPS_DEBUG
      _presentation.validate();
#endif
      size_t r = (_presentation.contains_empty_word() ? 0 : 1);
      size_t c = _presentation.alphabet().size();

      WordGraphWithSources<Node>::init(r, c);
      FelschGraphSettings_::init();
      _definitions.clear();
      _felsch_tree.init(c);
      _felsch_tree_initted = false;
      return *this;
    }

    template <typename Word, typename Node, typename Definitions>
    FelschGraph<Word, Node, Definitions>&
    FelschGraph<Word, Node, Definitions>::private_init_from_word_graph() {
      // Don't call init because we don't want to re-init WordGraphWithSources
      FelschGraphSettings_::init();
      _definitions.clear();
      _felsch_tree.init(0);
      _felsch_tree_initted = false;
      _presentation.init();  // TODO should we set the alphabet of the
                             // presentation?

      for (auto n : WordGraph<Node>::nodes()) {
        for (auto e : WordGraph<Node>::labels()) {
          if (WordGraph<Node>::target_no_checks(n, e) != UNDEFINED) {
            _definitions.emplace_back(n, e);
          }
        }
      }
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - accessors - private
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename Node, typename Definitions>
    void FelschGraph<Word, Node, Definitions>::init_felsch_tree() const {
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
    template <typename Word, typename Node, typename Definitions>
    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    inline bool FelschGraph<Word, Node, Definitions>::
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

    template <typename Word, typename Node, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Word, Node, Definitions>::process_definition_v2(
        Definition     d,
        Incompatible&  incompat,
        PreferredDefs& pref_defs) {
      felsch_tree().push_back(d.second);
      for (auto it = felsch_tree().cbegin(); it < felsch_tree().cend(); ++it) {
        // Using anything other than NoPreferredDefs here seems to be bad
        // in test case "ACE --- perf602p5 - Felsch", maybe this is a good
        // example where the fill factor would be useful?
        if (!merge_targets_of_paths_labelled_by_rules_if_possible<RegisterDefs>(
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
    bool FelschGraph<Word, Node, Definitions>::process_definition_v1(
        Definition     d,
        Incompatible&  incompat,
        PreferredDefs& pref_defs) {
      felsch_tree().push_back(d.second);
      if (!process_definitions_dfs_v1(d.first, incompat, pref_defs)) {
        return false;
      }
      return true;
    }

    template <typename Word, typename Node, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Word, Node, Definitions>::process_definitions_v2(
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

    // TODO remove code dupl with process_definitions_v2
    template <typename Word, typename Node, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Word, Node, Definitions>::process_definitions_v1(
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
    bool FelschGraph<Word, Node, Definitions>::process_definitions_dfs_v2(
        node_type      root,
        node_type      c,
        Incompatible&  incompat,
        PreferredDefs& pref_defs) {
      size_t const n = out_degree();
      for (size_t x = 0; x < n; ++x) {
        node_type e = WordGraphWithSources_::first_source_no_checks(c, x);
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
              // TODO this shouldn't really be necessary, i.e. I don't think we
              // should ever reach here if this is implemented correctly,
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
            e       = WordGraphWithSources_::first_source_no_checks(c, x);
            while (e != UNDEFINED) {
              if (!merge_targets_of_paths_if_possible<RegisterDefs>(
                      y,
                      u_first,
                      u_last,
                      e,
                      v.cbegin(),
                      v.cend(),
                      incompat,
                      pref_defs)) {
                return false;
              }
              e = WordGraphWithSources_::next_source_no_checks(e, x);
            }
          }
          e = WordGraphWithSources_::first_source_no_checks(c, x);
          while (e != UNDEFINED) {
            if (!process_definitions_dfs_v2(root, e, incompat, pref_defs)) {
              return false;
            }
            e = WordGraphWithSources_::next_source_no_checks(e, x);
          }
          felsch_tree().pop_front();
        }
      }
      return true;
    }

    template <typename Word, typename Node, typename Definitions>
    template <typename Incompatible, typename PreferredDefs>
    bool FelschGraph<Word, Node, Definitions>::process_definitions_dfs_v1(
        node_type      c,
        Incompatible&  incompat,
        PreferredDefs& pref_defs) {
      for (auto it = felsch_tree().cbegin(); it < felsch_tree().cend(); ++it) {
        if (!merge_targets_of_paths_labelled_by_rules_if_possible<RegisterDefs>(
                c, *it, incompat, pref_defs)) {
          return false;
        }
      }

      size_t const n = _presentation.alphabet().size();
      for (size_t x = 0; x < n; ++x) {
        if (felsch_tree().push_front(x)) {
          node_type e
              = WordGraphWithSources<Node>::first_source_no_checks(c, x);
          while (e != UNDEFINED) {
            if (!process_definitions_dfs_v1(e, incompat, pref_defs)) {
              return false;
            }
            e = WordGraphWithSources<Node>::next_source_no_checks(e, x);
          }
          felsch_tree().pop_front();
        }
      }
      return true;
    }

    // Helper namespace
    namespace felsch_graph {
      template <bool RegDefs,
                typename Word,
                typename Node,
                typename Definitions,
                typename Incompatible,
                typename PrefDefs>
      bool make_compatible(
          FelschGraph<Word, Node, Definitions>&                    fd,
          typename FelschGraph<Word, Node, Definitions>::node_type first_node,
          typename FelschGraph<Word, Node, Definitions>::node_type last_node,
          typename std::vector<Word>::const_iterator               first_rule,
          typename std::vector<Word>::const_iterator               last_rule,
          Incompatible&&                                           incompat,
          PrefDefs&& pref_defs) noexcept {
        LIBSEMIGROUPS_ASSERT(first_node < fd.number_of_nodes());
        LIBSEMIGROUPS_ASSERT(last_node <= fd.number_of_nodes());
        LIBSEMIGROUPS_ASSERT(std::distance(first_rule, last_rule) % 2 == 0);

        for (Node n = first_node; n < last_node; ++n) {
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
