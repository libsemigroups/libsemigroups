//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains a declaration of a class called Felsch which is
// used by the classes Sims1 and ToddCoxeterImpl.

// TODO(2):
// * iwyu
// * noexcept
// * test file
// * doc

#ifndef LIBSEMIGROUPS_DETAIL_FELSCH_GRAPH_HPP_
#define LIBSEMIGROUPS_DETAIL_FELSCH_GRAPH_HPP_

#include <cstddef>  // for size_t
#include <cstdint>  // for uint8_t
#include <utility>  // for pair
#include <vector>   // for vector

#include "libsemigroups/adapters.hpp"      // for Noop
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/presentation.hpp"  // for Presentation

#include "felsch-tree.hpp"              // for FelschTree
#include "word-graph-with-sources.hpp"  // for WordGraphWithSources

namespace libsemigroups {

  namespace detail {

    bool constexpr REGISTER_DEFS_YES = true;
    bool constexpr REGISTER_DEFS_NO  = false;

    // This struct exists to avoid having to write typename before
    // options::def_version etc everywhere below
    struct FelschEnums {
      struct options {
        // This is documented in todd-coxeter-class.hpp
        enum class def_version : uint8_t {
          one,
          two,
        };
      };
    };

    // This class exists so that both Felsch and ToddCoxeterImpl can use
    // the same settings/options without code duplication
    template <typename Subclass>
    class FelschSettings : public FelschEnums {
     private:
      options::def_version _def_version;

     public:
      FelschSettings() : _def_version() {
        init();
      }

      // TODO should it be Subclass&?
      FelschSettings& init() {
        _def_version = options::def_version::two;
        return *this;
      }

      FelschSettings(FelschSettings const&)            = default;
      FelschSettings(FelschSettings&&)                 = default;
      FelschSettings& operator=(FelschSettings const&) = default;
      FelschSettings& operator=(FelschSettings&&)      = default;

      template <typename S>
      Subclass& settings(FelschSettings<S>& that) {
        _def_version = that.def_version();
        return static_cast<Subclass&>(*this);
      }

      [[nodiscard]] FelschSettings const& settings() const noexcept {
        return *this;
      }

      // Doc in todd-coxeter.hpp
      Subclass& def_version(options::def_version val) {
        _def_version = val;
        return static_cast<Subclass&>(*this);
      }

      // Doc in todd-coxeter.hpp
      [[nodiscard]] options::def_version def_version() const noexcept {
        return _def_version;
      }
    };

    template <typename Graph, typename Definitions>
    class Felsch : public FelschSettings<Felsch<Graph, Definitions>> {
      // TODO static assert that graph is derived from WordGraphWithSources
     private:
      ////////////////////////////////////////////////////////////////////////
      // Felsch - aliases - private
      ////////////////////////////////////////////////////////////////////////

      using Felsch_         = Felsch<Graph, Definitions>;
      using FelschSettings_ = FelschSettings<Felsch_>;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Felsch - aliases + nested classes - public
      ////////////////////////////////////////////////////////////////////////

      using options = typename FelschSettings_::options;

      using word_graph_type = Graph;
      using node_type       = typename Graph::node_type;
      using label_type      = typename Graph::label_type;
      using size_type       = typename Graph::size_type;

      using word_iterator = typename word_type::const_iterator;

      using NoPreferredDefs    = Noop;
      using Definition         = std::pair<node_type, label_type>;
      using StopIfIncompatible = ReturnFalse;

     private:
      ////////////////////////////////////////////////////////////////////////
      // Felsch - data members - private
      ////////////////////////////////////////////////////////////////////////
      mutable FelschTree _felsch_tree;
      mutable bool       _felsch_tree_initted;

      Definitions                    _definitions;
      Presentation<word_type> const* _presentation;
      Graph*                         _word_graph;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Felsch - constructors + initializers - public
      ////////////////////////////////////////////////////////////////////////

      Felsch();
      Felsch& init();

      Felsch(Felsch const&);
      Felsch(Felsch&&);
      Felsch& operator=(Felsch const&);
      Felsch& operator=(Felsch&&);

      explicit Felsch(Graph const& wg, Presentation<word_type> const& p);
      Felsch& init(Graph const& wg, Presentation<word_type> const& p);

      ~Felsch();

      ////////////////////////////////////////////////////////////////////////
      // Felsch - settings - public
      ////////////////////////////////////////////////////////////////////////

      using FelschSettings_::def_version;
      using FelschSettings_::settings;

      ////////////////////////////////////////////////////////////////////////
      // Felsch - operators - public TODO remove this section
      ////////////////////////////////////////////////////////////////////////

      ////////////////////////////////////////////////////////////////////////
      // Felsch - accessors - public
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] FelschTree const& felsch_tree() const {
        init_felsch_tree();
        return _felsch_tree;
      }

      [[nodiscard]] Definitions& definitions() noexcept {
        return _definitions;
      }

      [[nodiscard]] Definitions const& definitions() const noexcept {
        return _definitions;
      }

      ////////////////////////////////////////////////////////////////////////
      // Felsch - modifiers - public
      ////////////////////////////////////////////////////////////////////////

      void add_definition(node_type s, label_type a) noexcept;

      template <bool RegisterDefs,
                typename Incompatible,
                typename PreferredDefs>
      [[nodiscard]] bool
      merge_targets_of_paths_if_possible(node_type      u_node,
                                         word_iterator  u_first,
                                         word_iterator  u_last,
                                         node_type      v_node,
                                         word_iterator  v_first,
                                         word_iterator  v_last,
                                         Incompatible&  incompat,
                                         PreferredDefs& pref_defs) noexcept;

      ////////////////////////////////////////////////////////////////////////
      // Felsch - process definitions - public
      ////////////////////////////////////////////////////////////////////////

      // This function should remain here, i.e. don't move to tpp file.
      // Not nodiscard because we don't care about the return value in
      // ToddCoxeterImpl
      template <typename Incompatible, typename PreferredDefs>
      inline bool process_definition(Definition const& d,
                                     Incompatible&     incompat,
                                     PreferredDefs&    pref_defs) {
        if (def_version() == options::def_version::two) {
          return process_definition_v2(d, incompat, pref_defs);
        } else {
          LIBSEMIGROUPS_ASSERT(def_version() == options::def_version::one);
          return process_definition_v1(d, incompat, pref_defs);
        }
      }

      template <typename Incompatible, typename PreferredDefs>
      [[nodiscard]] bool process_definitions(size_t         start,
                                             Incompatible&  incompat,
                                             PreferredDefs& pref_defs);

      [[nodiscard]] bool process_definitions(size_t start = 0);

     private:
      ////////////////////////////////////////////////////////////////////////
      // Felsch - constructors + initializers - private
      ////////////////////////////////////////////////////////////////////////

      Felsch& private_init_from_presentation();
      Felsch& private_init_from_word_graph();

      ////////////////////////////////////////////////////////////////////////
      // Felsch - accessors - private
      ////////////////////////////////////////////////////////////////////////

      void init_felsch_tree() const;

      // Non-const version for private use only
      [[nodiscard]] FelschTree& felsch_tree() {
        init_felsch_tree();
        return _felsch_tree;
      }

      ////////////////////////////////////////////////////////////////////////
      // Felsch - modifiers - private
      ////////////////////////////////////////////////////////////////////////

      // Follows the paths from node c labelled by the left and right handsides
      // of the i-th rule, and returns merge_targets on the last but one nodes
      // and letters. Not nodiscard because we don't care about the return value
      // in ToddCoxeterImpl
      template <bool RegisterDefs,
                typename Incompatible,
                typename PreferredDefs>
      bool merge_targets_of_paths_labelled_by_rules_if_possible(
          node_type const& c,
          size_t           i,
          Incompatible&    incompat,
          PreferredDefs&   pref_defs) noexcept;

      ////////////////////////////////////////////////////////////////////////
      // Felsch - process definitions - private
      ////////////////////////////////////////////////////////////////////////

      // TODO can we use a reference here?
      template <typename Incompatible, typename PreferredDefs>
      [[nodiscard]] bool process_definition_v2(Definition     d,
                                               Incompatible&  incompat,
                                               PreferredDefs& pref_defs);

      // TODO can we use a reference here?
      template <typename Incompatible, typename PreferredDefs>
      [[nodiscard]] bool process_definition_v1(Definition     d,
                                               Incompatible&  incompat,
                                               PreferredDefs& pref_defs);

      // Returns true if no contradictions are found.
      template <typename Incompatible, typename PreferredDefs>
      [[nodiscard]] bool process_definitions_v1(size_t start,
                                                Incompatible&,
                                                PreferredDefs&);

      template <typename Incompatible, typename PreferredDefs>
      [[nodiscard]] bool process_definitions_v2(size_t start,
                                                Incompatible&,
                                                PreferredDefs&);

      // Returns true if no contradictions are found.
      template <typename Incompatible, typename PreferredDefs>
      [[nodiscard]] bool process_definitions_dfs_v1(node_type c,
                                                    Incompatible&,
                                                    PreferredDefs&);

      template <typename Incompatible, typename PreferredDefs>
      [[nodiscard]] bool process_definitions_dfs_v2(node_type root,
                                                    node_type c,
                                                    Incompatible&,
                                                    PreferredDefs&);
    };  // Felsch

    namespace felsch {

      // Check that [first_node, last_node) is compatible with [first_rule,
      // last_rule) or if there are edges missing in paths labelled by rules,
      // then try to fill those in so that fd is compatible.
      // Not nodiscard because we don't care about the return value in
      // ToddCoxeterImpl
      template <bool RegisterDefs,
                typename Word,
                typename Node,
                typename Definitions,
                typename Incompatible,
                typename PrefDefs>
      bool make_compatible(
          Felsch<Word, Node, Definitions>&                    fd,
          typename Felsch<Word, Node, Definitions>::node_type first_node,
          typename Felsch<Word, Node, Definitions>::node_type last_node,
          typename std::vector<Word>::const_iterator          first_rule,
          typename std::vector<Word>::const_iterator          last_rule,
          Incompatible&&                                      incompat,
          PrefDefs&& pref_defs) noexcept;

      // Not nodiscard because we don't care about the return value in
      // ToddCoxeterImpl
      template <bool RegisterDefs,
                typename Word,
                typename Node,
                typename Definitions>
      bool make_compatible(
          Felsch<Word, Node, Definitions>&                    fd,
          typename Felsch<Word, Node, Definitions>::node_type first_node,
          typename Felsch<Word, Node, Definitions>::node_type last_node,
          typename std::vector<Word>::const_iterator          first_rule,
          typename std::vector<Word>::const_iterator          last_rule) {
        return make_compatible<RegisterDefs>(
            fd,
            first_node,
            last_node,
            first_rule,
            last_rule,
            typename Felsch<Word, Node, Definitions>::StopIfIncompatible(),
            typename Felsch<Word, Node, Definitions>::NoPreferredDefs());
      }

    }  // namespace felsch
  }  // namespace detail
}  // namespace libsemigroups

#include "felsch.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_FELSCH_GRAPH_HPP_
