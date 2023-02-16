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

// This file contains a declaration of a class called FelschDigraph which is
// used by the classes Sims1 and ToddCoxeter.

#ifndef LIBSEMIGROUPS_FELSCH_DIGRAPH_HPP_
#define LIBSEMIGROUPS_FELSCH_DIGRAPH_HPP_

#include <cstddef>  // for size_t
#include <cstdint>  // for uint8_t
#include <utility>  // for pair
#include <vector>   // for vector

#include "digraph-with-sources.hpp"  // for DigraphWithSources
#include "felsch-tree.hpp"           // for FelschTree
#include "libsemigroups/debug.hpp"   // for LIBSEMIGROUPS_ASSERT
#include "present.hpp"               // for Presentation

namespace libsemigroups {
  // TODO move this somewhere else
  struct ReturnFalse {
    template <typename... Args>
    constexpr bool operator()(Args...) const noexcept {
      return false;
    }
  };

  // TODO move this somewhere else
  struct Noop {
    template <typename... Args>
    constexpr void operator()(Args...) const noexcept {}
  };

  // This struct exists to avoid having to write typename before
  // options::def_policy etc everywhere below
  struct FelschDigraphEnums {
    struct options {
      enum class def_version : uint8_t {
        one,
        two,
      };
    };
  };

  // This class exists so that both FelschDigraph and ToddCoxeter can use the
  // same settings/options without code duplication
  template <typename Subclass>
  class FelschDigraphSettings : public FelschDigraphEnums {
   private:
    options::def_version _def_version;

   public:
    FelschDigraphSettings() : _def_version() {
      init();
    }
    FelschDigraphSettings& init();

    FelschDigraphSettings(FelschDigraphSettings const&)            = default;
    FelschDigraphSettings(FelschDigraphSettings&&)                 = default;
    FelschDigraphSettings& operator=(FelschDigraphSettings const&) = default;
    FelschDigraphSettings& operator=(FelschDigraphSettings&&)      = default;

    template <typename S>
    Subclass& settings(FelschDigraphSettings<S>& that) {
      _def_version = that.def_version();
      return static_cast<Subclass&>(*this);
    }

    FelschDigraphSettings const& settings() const noexcept {
      return *this;
    }

    // TODO doc
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
    options::def_version def_version() const noexcept {
      return _def_version;
    }
  };

  template <typename Word, typename Node, typename Definitions>
  class FelschDigraph
      : public DigraphWithSources<Node>,
        public FelschDigraphSettings<FelschDigraph<Word, Node, Definitions>> {
   private:
    using FelschDigraph_         = FelschDigraph<Word, Node, Definitions>;
    using FelschDigraphSettings_ = FelschDigraphSettings<FelschDigraph_>;
    using DigraphWithSources_    = DigraphWithSources<Node>;

   public:
    using options = typename FelschDigraphSettings_::options;

    using node_type  = Node;
    using word_type  = Word;
    using label_type = typename DigraphWithSources_::label_type;
    using size_type  = typename DigraphWithSources_::size_type;

    using NoPreferredDefs    = Noop;
    using StopIfIncompatible = ReturnFalse;

    static constexpr bool RegisterDefs      = true;
    static constexpr bool DoNotRegisterDefs = false;

   private:
    using Definition = std::pair<node_type, label_type>;

    Definitions             _definitions;
    detail::FelschTree      _felsch_tree;
    Presentation<word_type> _presentation;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    FelschDigraph() = default;
    FelschDigraph& init();

    FelschDigraph(FelschDigraph const&)            = default;
    FelschDigraph(FelschDigraph&&)                 = default;
    FelschDigraph& operator=(FelschDigraph const&) = default;
    FelschDigraph& operator=(FelschDigraph&&)      = default;

    FelschDigraph(Presentation<Word> const& p);
    FelschDigraph& init(Presentation<Word> const& p);

    FelschDigraph(Presentation<Word>&& p);
    FelschDigraph& init(Presentation<Word>&& p);

    // TODO rvalue reference version + init
    template <typename M>
    FelschDigraph(ActionDigraph<M> const& ad);

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    using FelschDigraphSettings_::def_version;
    using FelschDigraphSettings_::settings;

    ////////////////////////////////////////////////////////////////////////
    // Operators
    ////////////////////////////////////////////////////////////////////////

    bool operator==(FelschDigraph const& that) const;

    ////////////////////////////////////////////////////////////////////////
    // Accessors
    ////////////////////////////////////////////////////////////////////////

    Presentation<word_type>&       presentation() noexcept;
    Presentation<word_type> const& presentation() const noexcept;

    detail::FelschTree const& felsch_tree() const {
      return _felsch_tree;
    }

    [[nodiscard]] auto& definitions() noexcept {
      return _definitions;
    }

    [[nodiscard]] auto const& definitions() const noexcept {
      return _definitions;
    }

    template <typename T>
    void init_definitions(T const& thing) {
      // For example this calls ToddCoxeter::init(Definitions&) so that
      // the settings in the _definitions object is set to be the same as that
      // of the ToddCoxeter instance.
      thing.init_definitions(_definitions);
    }

    ////////////////////////////////////////////////////////////////////////
    // Modifiers
    ////////////////////////////////////////////////////////////////////////

    template <bool RegisterDefs = true>
    bool try_def_edge_nc(node_type c, label_type x, node_type d) noexcept;

    template <bool RegisterDefs = true>
    void def_edge_nc(node_type c, label_type x, node_type d) noexcept;

    void reduce_number_of_edges_to(size_type n);

    // This is *not* the same as init(p) since we only replace the presentation
    // but otherwise do not modify the graph, whereas init(p) returns this to
    // the same state as FelschDigraph(p);
    FelschDigraph& presentation(Presentation<Word> const& p);
    FelschDigraph& presentation(Presentation<Word>&& p);

    ////////////////////////////////////////////////////////////////////////
    // Process definitions
    ////////////////////////////////////////////////////////////////////////

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
    bool process_definitions(size_t         start,
                             Incompatible&  incompat,
                             PreferredDefs& pref_defs);

    bool process_definitions(size_t start = 0);

    // Returns false if the targets of the edges (x, a, xa) and (y, b, yb)
    // cannot be merged (i.e. if xa != yb and incompat(x, a, y, b) returns
    // false), otherwise returns true. Always modifies the graph if xa !=
    // UNDEFINED and yb = UNDEFINED, or vice versa, and pref_defs(x, a, y,
    // b) is called if xa = UNDEFINED and yb = UNDEFINED.
    template <typename Incompatible, typename PreferredDefs>
    bool merge_targets_of_nodes_if_possible(node_type      x,
                                            label_type     a,
                                            node_type      y,
                                            label_type     b,
                                            Incompatible&  incompat,
                                            PreferredDefs& pref_defs);

    using word_iterator = typename word_type::const_iterator;

    template <typename Incompatible, typename PreferredDefs>
    bool merge_targets_of_paths_if_possible(node_type      u_node,
                                            word_iterator  u_first,
                                            word_iterator  u_last,
                                            node_type      v_node,
                                            word_iterator  v_first,
                                            word_iterator  v_last,
                                            Incompatible&  incompat,
                                            PreferredDefs& pref_defs) noexcept;

   private:
    // TODO can we use a reference here?
    template <typename Incompatible, typename PreferredDefs>
    bool process_definition_v2(Definition     d,
                               Incompatible&  incompat,
                               PreferredDefs& pref_defs);
    // TODO can we use a reference here?
    template <typename Incompatible, typename PreferredDefs>
    bool process_definition_v1(Definition     d,
                               Incompatible&  incompat,
                               PreferredDefs& pref_defs);

    // Follows the paths from node c labelled by the left and right handsides
    // of the i-th rule, and returns merge_targets on the last but one nodes
    // and letters.
    template <typename Incompatible, typename PreferredDefs>
    inline bool merge_targets_of_paths_labelled_by_rules_if_possible(
        node_type const& c,
        size_t           i,
        Incompatible&    incompat,
        PreferredDefs&   pref_defs) noexcept;

    // Returns true if no contradictions are found.
    template <typename Incompatible, typename PreferredDefs>
    bool process_definitions_v1(size_t start, Incompatible&, PreferredDefs&);

    template <typename Incompatible, typename PreferredDefs>
    bool process_definitions_v2(size_t start, Incompatible&, PreferredDefs&);

    // Returns true if no contradictions are found.
    template <typename Incompatible, typename PreferredDefs>
    bool process_definitions_dfs_v1(node_type c, Incompatible&, PreferredDefs&);

    template <typename Incompatible, typename PreferredDefs>
    bool process_definitions_dfs_v2(node_type root,
                                    node_type c,
                                    Incompatible&,
                                    PreferredDefs&);
  };

  namespace felsch_digraph {
    // TODO compatible function for ActionDigraph

    // Check that [first_node, last_node) is compatible with [first_rule,
    // last_rule) or if there are edges missing in paths labelled by rules,
    // then try to fill those in so that fd is compatible.
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
        PrefDefs&& pref_defs) noexcept;

    template <typename Word, typename Node, typename Definitions>
    bool make_compatible(
        FelschDigraph<Word, Node, Definitions>&                    fd,
        typename FelschDigraph<Word, Node, Definitions>::node_type first_node,
        typename FelschDigraph<Word, Node, Definitions>::node_type last_node,
        typename std::vector<Word>::const_iterator                 first_rule,
        typename std::vector<Word>::const_iterator                 last_rule) {
      return make_compatible(
          fd,
          first_node,
          last_node,
          first_rule,
          last_rule,
          typename FelschDigraph<Word, Node, Definitions>::StopIfIncompatible(),
          typename FelschDigraph<Word, Node, Definitions>::NoPreferredDefs());
    }

  }  // namespace felsch_digraph
}  // namespace libsemigroups

#include "felsch-digraph.tpp"

#endif  // LIBSEMIGROUPS_FELSCH_DIGRAPH_HPP_
