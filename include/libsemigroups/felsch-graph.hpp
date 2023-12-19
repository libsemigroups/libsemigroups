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

// This file contains a declaration of a class called FelschGraph which is
// used by the classes Sims1 and ToddCoxeter.

// TODO:
// * iwyu
// * noexcept

// * test file
// * doc

#ifndef LIBSEMIGROUPS_FELSCH_GRAPH_HPP_
#define LIBSEMIGROUPS_FELSCH_GRAPH_HPP_

#include <cstddef>  // for size_t
#include <cstdint>  // for uint8_t
#include <utility>  // for pair
#include <vector>   // for vector

#include "adapters.hpp"                 // for Noop
#include "debug.hpp"                    // for LIBSEMIGROUPS_ASSERT
#include "presentation.hpp"             // for Presentation
#include "word-graph-with-sources.hpp"  // for WordGraphWithSources

#include "detail/felsch-tree.hpp"  // for FelschTree

namespace libsemigroups {

  // TODO move to detail namespace?
  // This struct exists to avoid having to write typename before
  // options::def_policy etc everywhere below
  struct FelschGraphEnums {
    struct options {
      enum class def_version : uint8_t {
        one,
        two,
      };
    };
  };

  bool constexpr DoNotRegisterDefs = false;
  bool constexpr RegisterDefs      = true;

  // This class exists so that both FelschGraph and ToddCoxeter can use the
  // same settings/options without code duplication
  // TODO move to detail namespace?
  template <typename Subclass>
  class FelschGraphSettings : public FelschGraphEnums {
   private:
    options::def_version _def_version;

   public:
    FelschGraphSettings() : _def_version() {
      init();
    }

    // TODO should it be Subclass&?
    FelschGraphSettings& init() {
      _def_version = options::def_version::two;
      return *this;
    }

    FelschGraphSettings(FelschGraphSettings const&)            = default;
    FelschGraphSettings(FelschGraphSettings&&)                 = default;
    FelschGraphSettings& operator=(FelschGraphSettings const&) = default;
    FelschGraphSettings& operator=(FelschGraphSettings&&)      = default;

    template <typename S>
    Subclass& settings(FelschGraphSettings<S>& that) {
      _def_version = that.def_version();
      return static_cast<Subclass&>(*this);
    }

    //
    [[nodiscard]] FelschGraphSettings const& settings() const noexcept {
      return *this;
    }

    // TODO doc for the purpose of copying the doc elsewhere
    Subclass& def_version(options::def_version val) {
      _def_version = val;
      return static_cast<Subclass&>(*this);
    }

    //! The current value of the definition policy setting.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns The current value of the setting, a value of type
    //! ``options::definitions``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::def_version def_version() const noexcept {
      return _def_version;
    }
  };

  // TODO move to detail namespace? If it's not user facing anywhere then yes.
  template <typename Word, typename Node, typename Definitions>
  class FelschGraph
      : public WordGraphWithSources<Node>,
        public FelschGraphSettings<FelschGraph<Word, Node, Definitions>> {
   private:
    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - aliases - private
    ////////////////////////////////////////////////////////////////////////
    using FelschGraph_          = FelschGraph<Word, Node, Definitions>;
    using FelschGraphSettings_  = FelschGraphSettings<FelschGraph_>;
    using WordGraphWithSources_ = WordGraphWithSources<Node>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - aliases + nested classes - public
    ////////////////////////////////////////////////////////////////////////

    using options = typename FelschGraphSettings_::options;

    using node_type     = Node;
    using word_type     = Word;
    using label_type    = typename WordGraphWithSources_::label_type;
    using size_type     = typename WordGraphWithSources_::size_type;
    using word_iterator = typename word_type::const_iterator;

    using NoPreferredDefs = Noop;
    using Definition      = std::pair<node_type, label_type>;

    struct StopIfIncompatible {
      template <typename... Args>
      [[nodiscard]] constexpr bool operator()(Args...) const noexcept {
        return false;
      }
    };

    static constexpr bool RegisterDefs      = true;
    static constexpr bool DoNotRegisterDefs = false;

    using WordGraph<Node>::out_degree;
    using WordGraph<Node>::number_of_nodes;

   private:
    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - data members - private
    ////////////////////////////////////////////////////////////////////////
    Definitions                _definitions;
    mutable detail::FelschTree _felsch_tree;
    mutable bool               _felsch_tree_initted;
    Presentation<word_type>    _presentation;

   public:
    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    FelschGraph() = default;
    FelschGraph& init();

    FelschGraph(FelschGraph const&)            = default;
    FelschGraph(FelschGraph&&)                 = default;
    FelschGraph& operator=(FelschGraph const&) = default;
    FelschGraph& operator=(FelschGraph&&)      = default;

    explicit FelschGraph(Presentation<Word> const& p);
    FelschGraph& init(Presentation<Word> const& p);

    explicit FelschGraph(Presentation<Word>&& p);
    FelschGraph& init(Presentation<Word>&& p);

    // TODO remove and replace with an init(Presentation, WordGraph)
    template <typename M>
    explicit FelschGraph(WordGraph<M> const& ad);

    // TODO remove and replace with an init(Presentation, WordGraph)
    template <typename M>
    FelschGraph& init(WordGraph<M> const& ad);

    // No point in having a general rvalue ref version since we can't actually
    // use a word graph containing another type of node to initialise this.
    // TODO remove and replace with an init(Presentation, WordGraph)
    explicit FelschGraph(WordGraph<Node>&& ad);

    // TODO remove and replace with an init(Presentation, WordGraph)
    FelschGraph& init(WordGraph<Node>&& ad);

    // This is *not* the same as init(p) since we only replace the presentation
    // but otherwise do not modify the graph, whereas init(p) returns this to
    // the same state as FelschGraph(p). This is required say after calling
    // init(WordGraph) so that the presentation is defined.
    // Seems safer that if these are only used after construction/init from a
    // word graph that these be one of the parameters of the constructor/init
    // function so that they are coupled and it's not possible to get into a
    // bad undefined position.
    // TODO remove and replace with an init(Presentation, WordGraph)
    FelschGraph& presentation(Presentation<Word> const& p);
    FelschGraph& presentation(Presentation<Word>&& p);

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - settings - public
    ////////////////////////////////////////////////////////////////////////

    using FelschGraphSettings_::def_version;
    using FelschGraphSettings_::settings;

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - operators - public
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] bool operator==(FelschGraph const& that) const;

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - accessors - public
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] Presentation<word_type>& presentation() noexcept;
    Presentation<word_type> const&         presentation() const noexcept;

    [[nodiscard]] detail::FelschTree const& felsch_tree() const {
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
    // FelschGraph - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    template <bool RegDefs = true>
    void set_target_no_checks(node_type c, label_type x, node_type d) noexcept;

    void reduce_number_of_edges_to(size_type n);

    // Returns false if the targets of the edges (x, a, xa) and (y, b, yb)
    // cannot be merged (i.e. if xa != yb and incompat(x, a, y, b) returns
    // false), otherwise returns true. Always modifies the graph if xa !=
    // UNDEFINED and yb = UNDEFINED, or vice versa, and pref_defs(x, a, y,
    // b) is called if xa = UNDEFINED and yb = UNDEFINED.
    // Not nodiscard because we don't care about the return value in
    // ToddCoxeter (return value indicates if we should stop early an
    // incompatibility is discovered, which we never do in TC)
    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    bool merge_targets_of_nodes_if_possible(node_type      x,
                                            label_type     a,
                                            node_type      y,
                                            label_type     b,
                                            Incompatible&  incompat,
                                            PreferredDefs& pref_defs);

    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    [[nodiscard]] bool
    merge_targets_of_paths_if_possible(node_type      u_node,
                                       word_iterator  u_first,
                                       word_iterator  u_last,
                                       node_type      v_node,
                                       word_iterator  v_first,
                                       word_iterator  v_last,
                                       Incompatible&  incompat,
                                       PreferredDefs& pref_defs) noexcept;

    [[nodiscard]] bool
    merge_targets_of_paths_if_possible(node_type     u_node,
                                       word_iterator u_first,
                                       word_iterator u_last,
                                       letter_type   a,
                                       node_type     v_node,
                                       word_iterator v_first,
                                       word_iterator v_last) noexcept;

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - process definitions - public
    ////////////////////////////////////////////////////////////////////////

    // This function should remain here, i.e. don't move to tpp file.
    // Not nodiscard because we don't care about the return value in
    // ToddCoxeter
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
    // FelschGraph - constructors + initializers - private
    ////////////////////////////////////////////////////////////////////////

    FelschGraph& private_init_from_presentation();
    FelschGraph& private_init_from_word_graph();

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - accessors - private
    ////////////////////////////////////////////////////////////////////////

    void init_felsch_tree() const;

    // Non-const version for private use only
    [[nodiscard]] detail::FelschTree& felsch_tree() {
      init_felsch_tree();
      return _felsch_tree;
    }

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - modifiers - private
    ////////////////////////////////////////////////////////////////////////

    // Follows the paths from node c labelled by the left and right handsides
    // of the i-th rule, and returns merge_targets on the last but one nodes
    // and letters.
    // Not nodiscard because we don't care about the return value in
    // ToddCoxeter
    template <bool RegDefs, typename Incompatible, typename PreferredDefs>
    bool merge_targets_of_paths_labelled_by_rules_if_possible(
        node_type const& c,
        size_t           i,
        Incompatible&    incompat,
        PreferredDefs&   pref_defs) noexcept;

    ////////////////////////////////////////////////////////////////////////
    // FelschGraph - process definitions - private
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
  };  // FelschGraph

  namespace felsch_graph {

    // Check that [first_node, last_node) is compatible with [first_rule,
    // last_rule) or if there are edges missing in paths labelled by rules,
    // then try to fill those in so that fd is compatible.
    // Not nodiscard because we don't care about the return value in
    // ToddCoxeter
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
        PrefDefs&& pref_defs) noexcept;

    // Not nodiscard because we don't care about the return value in
    // ToddCoxeter
    template <bool RegDefs, typename Word, typename Node, typename Definitions>
    bool make_compatible(
        FelschGraph<Word, Node, Definitions>&                    fd,
        typename FelschGraph<Word, Node, Definitions>::node_type first_node,
        typename FelschGraph<Word, Node, Definitions>::node_type last_node,
        typename std::vector<Word>::const_iterator               first_rule,
        typename std::vector<Word>::const_iterator               last_rule) {
      return make_compatible<RegDefs>(
          fd,
          first_node,
          last_node,
          first_rule,
          last_rule,
          typename FelschGraph<Word, Node, Definitions>::StopIfIncompatible(),
          typename FelschGraph<Word, Node, Definitions>::NoPreferredDefs());
    }

  }  // namespace felsch_graph
}  // namespace libsemigroups

#include "felsch-graph.tpp"

#endif  // LIBSEMIGROUPS_FELSCH_GRAPH_HPP_
