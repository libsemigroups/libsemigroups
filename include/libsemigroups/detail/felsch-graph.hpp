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

// This file contains a declaration of a class called FelschGraph which is
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

    // This struct exists to avoid having to write typename before
    // options::def_version etc everywhere below
    struct FelschGraphEnums {
      struct options {
        // This is documented in todd-coxeter-class.hpp
        enum class def_version : uint8_t {
          one,
          two,
        };
      };
    };

    // This class exists so that both FelschGraph and ToddCoxeterImpl can use
    // the same settings/options without code duplication
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

      [[nodiscard]] FelschGraphSettings const& settings() const noexcept {
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
    class FelschGraph
        : public Graph,
          public FelschGraphSettings<FelschGraph<Graph, Definitions>> {
     private:
      ////////////////////////////////////////////////////////////////////////
      // FelschGraph - aliases - private
      ////////////////////////////////////////////////////////////////////////

      using FelschGraph_         = FelschGraph<Graph, Definitions>;
      using FelschGraphSettings_ = FelschGraphSettings<FelschGraph_>;

     public:
      ////////////////////////////////////////////////////////////////////////
      // FelschGraph - aliases + nested classes - public
      ////////////////////////////////////////////////////////////////////////

      using options = typename FelschGraphSettings_::options;

      using word_graph_type = Graph;
      using node_type       = typename Graph::node_type;
      using label_type      = typename Graph::label_type;
      using size_type       = typename Graph::size_type;

      using Definition         = std::pair<node_type, label_type>;
      using NoPreferredDefs    = Noop;
      using StopIfIncompatible = ReturnFalse;

     private:
      ////////////////////////////////////////////////////////////////////////
      // FelschGraph - data members - private
      ////////////////////////////////////////////////////////////////////////
      mutable FelschTree _felsch_tree;
      mutable bool       _felsch_tree_initted;

      Definitions             _definitions;
      Presentation<word_type> _presentation;

     public:
      ////////////////////////////////////////////////////////////////////////
      // FelschGraph - constructors + initializers - public
      ////////////////////////////////////////////////////////////////////////

      FelschGraph();
      FelschGraph& init();

      FelschGraph(FelschGraph const&);
      FelschGraph(FelschGraph&&);
      FelschGraph& operator=(FelschGraph const&);
      FelschGraph& operator=(FelschGraph&&);

      FelschGraph& operator=(WordGraph<node_type> const& wg);

      explicit FelschGraph(Presentation<word_type> const& p);
      FelschGraph& init(Presentation<word_type> const& p);

      ~FelschGraph();

      // This is *not* the same as init(p) since we only replace the
      // presentation but otherwise do not modify the graph, whereas init(p)
      // returns this to the same state as FelschGraph(p).
      FelschGraph& presentation_no_checks(Presentation<word_type> const& p);

      ////////////////////////////////////////////////////////////////////////
      // WordGraph - mem. fns - public
      ////////////////////////////////////////////////////////////////////////

      using Graph::number_of_edges;
      using Graph::number_of_nodes;
      using Graph::out_degree;
      using Graph::target_no_checks;

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
      // FelschGraph - modifiers - public
      ////////////////////////////////////////////////////////////////////////

      template <bool RegDefs>
      FelschGraph& possibly_register_target_no_checks(node_type  s,
                                                      label_type a,
                                                      node_type  t) noexcept;

      FelschGraph& register_target_no_checks(node_type  s,
                                             label_type a,
                                             node_type  t) noexcept;

      void reduce_number_of_edges_to(size_type n);

      // Returns false if the targets of the edges (x, a, xa) and (y, b, yb)
      // cannot be merged (i.e. if xa != yb and incompat(x, a, y, b) returns
      // false), otherwise returns true. Always modifies the graph if xa !=
      // UNDEFINED and yb = UNDEFINED, or vice versa, and pref_defs(x, a, y,
      // b) is called if xa = UNDEFINED and yb = UNDEFINED.
      // Not nodiscard because we don't care about the return value in
      // ToddCoxeterImpl (return value indicates if we should stop early an
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
      merge_targets_of_paths_if_possible(node_type                 u_node,
                                         word_type::const_iterator u_first,
                                         word_type::const_iterator u_last,
                                         node_type                 v_node,
                                         word_type::const_iterator v_first,
                                         word_type::const_iterator v_last,
                                         Incompatible&             incompat,
                                         PreferredDefs& pref_defs) noexcept;

      ////////////////////////////////////////////////////////////////////////
      // FelschGraph - process definitions - public
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
      // FelschGraph - accessors - private
      ////////////////////////////////////////////////////////////////////////

      void init_felsch_tree() const;

      // Non-const version for private use only
      [[nodiscard]] FelschTree& felsch_tree() {
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
      // ToddCoxeterImpl
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
      constexpr static bool do_not_register_defs = false;
      constexpr static bool do_register_defs     = true;

      // Check that [first_node, last_node) is compatible with [first_rule,
      // last_rule) or if there are edges missing in paths labelled by rules,
      // then try to fill those in so that fd is compatible.
      // Not nodiscard because we don't care about the return value in
      // ToddCoxeterImpl
      template <bool RegDefs,
                typename Node,
                typename Definitions,
                typename Incompatible,
                typename PrefDefs>
      bool make_compatible(
          FelschGraph<Node, Definitions>&                    fd,
          typename FelschGraph<Node, Definitions>::node_type first_node,
          typename FelschGraph<Node, Definitions>::node_type last_node,
          typename std::vector<word_type>::const_iterator    first_rule,
          typename std::vector<word_type>::const_iterator    last_rule,
          Incompatible&&                                     incompat,
          PrefDefs&& pref_defs) noexcept;

      // Not nodiscard because we don't care about the return value in
      // ToddCoxeterImpl
      template <bool RegDefs, typename Node, typename Definitions>
      bool make_compatible(
          FelschGraph<Node, Definitions>&                    fd,
          typename FelschGraph<Node, Definitions>::node_type first_node,
          typename FelschGraph<Node, Definitions>::node_type last_node,
          typename std::vector<word_type>::const_iterator    first_rule,
          typename std::vector<word_type>::const_iterator    last_rule) {
        return make_compatible<RegDefs>(
            fd,
            first_node,
            last_node,
            first_rule,
            last_rule,
            typename FelschGraph<Node, Definitions>::StopIfIncompatible(),
            typename FelschGraph<Node, Definitions>::NoPreferredDefs());
      }

      using DoNotRegisterDefs = Noop;

      template <typename Graph, typename Definitions>
      class DoRegisterDefs {
        detail::FelschGraph<Graph, Definitions>* _felsch_graph;

       public:
        using node_type =
            typename detail::FelschGraph<Graph, Definitions>::node_type;
        using label_type =
            typename detail::FelschGraph<Graph, Definitions>::label_type;

        DoRegisterDefs()                                     = default;
        DoRegisterDefs(DoRegisterDefs const&)                = default;
        DoRegisterDefs& operator=(DoRegisterDefs const&)     = default;
        DoRegisterDefs(DoRegisterDefs&&) noexcept            = default;
        DoRegisterDefs& operator=(DoRegisterDefs&&) noexcept = default;

        ~DoRegisterDefs() = default;

        explicit DoRegisterDefs(detail::FelschGraph<Graph, Definitions>& graph)
            : _felsch_graph(&graph) {}

        explicit DoRegisterDefs(detail::FelschGraph<Graph, Definitions>* graph)
            : _felsch_graph(graph) {}

        void operator()(node_type s, label_type a) const {
          _felsch_graph->definitions().emplace_back(s, a);
        }
      };

    }  // namespace felsch_graph
  }    // namespace detail
}  // namespace libsemigroups

#include "felsch-graph.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_FELSCH_GRAPH_HPP_
