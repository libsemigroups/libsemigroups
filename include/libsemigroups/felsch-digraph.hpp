//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

// TODO
// * iwyu
// * use definition and not definition everywhere here!

#include <cstddef>

#include "digraph-helper.hpp"
#include "digraph-with-sources.hpp"
#include "felsch-tree.hpp"
#include "present.hpp"

namespace libsemigroups {

  // This struct exists to avoid having to write typename before
  // options::def_policy etc everywhere below
  struct FelschDigraphSettingsEnums {
    struct options {
      enum class def_policy : uint8_t {
        //! Do not put newly generated definitions in the stack if the stack
        //! already has size max_definitions().
        no_stack_if_no_space,
        //! If the definition stack has size max_definitions() and a new
        //! definition is generated, then definitions with dead source node are
        //! are popped from the top of the stack (if any).
        purge_from_top,
        //! If the definition stack has size max_definitions() and a new
        //! definition is generated, then definitions with dead source node are
        //! are popped from the entire of the stack (if any).
        purge_all,
        //! If the definition stack has size max_definitions() and a new
        //! definition is generated, then all definitions in the stack are
        //! discarded.
        discard_all_if_no_space,
        //! There is no limit to the number of definitions that can be put in
        //! the stack.
        unlimited
      };

      enum class def_version : uint8_t {
        one,
        two,
      };
    };
  };

  template <typename T>  // TODO better tparam name
  class FelschDigraphSettings {
   public:
    using options = FelschDigraphSettingsEnums::options;

   private:
    size_t               _def_max;
    options::def_policy  _def_policy;
    options::def_version _def_version;

   public:
    FelschDigraphSettings() : _def_max(), _def_policy(), _def_version() {
      init();
    }

    FelschDigraphSettings(FelschDigraphSettings const&)            = default;
    FelschDigraphSettings(FelschDigraphSettings&&)                 = default;
    FelschDigraphSettings& operator=(FelschDigraphSettings const&) = default;
    FelschDigraphSettings& operator=(FelschDigraphSettings&&)      = default;

    // TODO to tpp file
    FelschDigraphSettings& init() {
      _def_max    = 2'000;
      _def_policy = options::def_policy::unlimited;
      // TODO change back to def_policy::no_stack_if_no_space, can't at present
      // because we haven't yet reimplemented lookaheads

      _def_version = options::def_version::two;
      return *this;
    }

    // TODO this should really be a constructor or assignment operator
    template <typename S>
    T& settings(FelschDigraphSettings<S>& that) {
      _def_max     = that.def_max();
      _def_policy  = that.def_policy();
      _def_version = that.def_version();
      return static_cast<T&>(*this);
    }

    FelschDigraphSettings const& settings() const noexcept {
      return *this;
    }

    //! Specify how to handle definitions.
    //!
    //! This function can be used to specify how to handle definitions. For
    //! details see options::definitions.
    //!
    //! The default value of this setting is
    //! ``options::definitions::no_stack_if_no_space |
    //! options::definitions::v2``.
    //!
    //! \param val the policy to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is not valid (i.e. if for
    //! example ``options::definitions::v1 & options::definitions::v2`` returns
    //! ``true``).
    T& def_policy(options::def_policy val) {
      _def_policy = val;
      return static_cast<T&>(*this);
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
    options::def_policy def_policy() const noexcept {
      return _def_policy;
    }

    //! The maximum number of definitions in the stack.
    //!
    //! This setting specifies the maximum number of definitions that can be
    //! in the stack at any given time. What happens if there are the maximum
    //! number of definitions in the stack and a new definition is generated is
    //! governed by definition_policy().
    //!
    //! The default value of this setting is \c 2'000.
    //!
    //! \param val the maximum size of the definition stack.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    T& def_max(size_t val) noexcept {
      _def_max = val;
      return static_cast<T&>(*this);
    }

    //! The current value of the setting for the maximum number of
    //! definitions.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    size_t def_max() const noexcept {
      return _def_max;
    }

    T& def_version(options::def_version val) {
      _def_version = val;
      return static_cast<T&>(*this);
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

  template <typename W, typename N>
  class FelschDigraph : public DigraphWithSources<N>,
                        public FelschDigraphSettings<FelschDigraph<W, N>> {
   private:
    using FelschDigraph_         = FelschDigraph<W, N>;
    using FelschDigraphSettings_ = FelschDigraphSettings<FelschDigraph_>;

    struct Noop;

   public:
    using options      = typename FelschDigraphSettings_::options;
    using node_type    = N;
    using word_type    = W;
    using letter_type  = typename W::value_type;
    using digraph_type = DigraphWithSources<node_type>;
    using size_type    = typename digraph_type::size_type;

   private:
    class Definitions {
     public:
      using node_type  = N;
      using Definition = std::pair<node_type, letter_type>;

     private:
      bool                    _any_skipped;
      std::vector<Definition> _definitions;
      FelschDigraphSettings_* _settings;

     public:
      template <typename T>
      explicit Definitions(FelschDigraphSettings<T>* settings) : Definitions() {
        _settings = settings;
      }

      Definitions() : _any_skipped(false), _definitions(), _settings(nullptr) {}
      Definitions(Definitions const&)                 = default;
      Definitions(Definitions&&)                      = default;
      Definitions& operator=(Definitions const& that) = default;
      Definitions& operator=(Definitions&&)           = default;

      template <typename T>
      void init(FelschDigraphSettings<T>* settings) {
        _any_skipped = false;
        _definitions.clear();
        _settings = settings;
      }

      Definition pop();

      void emplace(node_type c, typename FelschDigraph_::label_type x);

      Definition const& operator[](size_t i) {
        return _definitions[i];
      }

      bool any_skipped() const noexcept {
        return _any_skipped;
      }

      // TODO noexcept correct?
      bool empty() const noexcept {
        return _definitions.empty();
      }

      size_t size() const noexcept {
        return _definitions.size();
      }

      void clear() {
        _any_skipped |= !_definitions.empty();
        _definitions.clear();
      }

      Definition const& back() {
        LIBSEMIGROUPS_ASSERT(!empty());
        return _definitions.back();
      }

      void pop_back() {
        LIBSEMIGROUPS_ASSERT(!empty());
        _definitions.pop_back();
      }

      using iterator = decltype(_definitions.begin());

      iterator begin() {
        return _definitions.begin();
      }

      iterator end() {
        return _definitions.end();
      }

      void erase(iterator first, iterator last) {
        _definitions.erase(first, last);
      }
    };  // Definitions

   public:
    using Definition = typename Definitions::Definition;

   private:
    Definitions             _definitions;
    detail::FelschTree      _felsch_tree;
    Presentation<word_type> _presentation;

   public:
    using NoPreferredDefs = Noop;
    struct ReturnFalse;

    // TODO remove second arg
    FelschDigraph(Presentation<word_type> const& p, size_type n);
    FelschDigraph(Presentation<word_type>&& p, size_type n);

    // TODO rvalue reference version + init
    template <typename M>
    FelschDigraph(ActionDigraph<M> const& ad);

    FelschDigraph()                                = default;
    FelschDigraph(FelschDigraph const&)            = default;
    FelschDigraph(FelschDigraph&&)                 = default;
    FelschDigraph& operator=(FelschDigraph const&) = default;
    FelschDigraph& operator=(FelschDigraph&&)      = default;

    using FelschDigraphSettings_::def_max;
    using FelschDigraphSettings_::def_policy;
    using FelschDigraphSettings_::def_version;
    using FelschDigraphSettings_::settings;

    FelschDigraph& init(Presentation<word_type> const& p, size_type n);
    FelschDigraph& init(Presentation<word_type>&& p, size_type n);

    // TODO shouldn't this be called init?
    FelschDigraph& presentation(Presentation<word_type> const& p);

    Presentation<word_type>&       presentation() noexcept;
    Presentation<word_type> const& presentation() const noexcept;

    detail::FelschTree const& felsch_tree() const {
      return _felsch_tree;
    }

    auto& definitions() noexcept {
      return _definitions;
    }

    auto const& definitions() const noexcept {
      return _definitions;
    }

    // Other stuff
    template <bool RegisterDefs = true>
    bool def_edge(node_type c, letter_type x, node_type d) noexcept;

    void reduce_number_of_edges_to(size_type n);

    bool operator==(FelschDigraph const& that) const;

    template <typename IncompatibleFunc, typename PreferredDefs>
    bool process_definitions(size_t            start,
                             IncompatibleFunc& incompat,
                             PreferredDefs&    pref_defs);

    template <typename IncompatibleFunc, typename PreferredDefs>
    bool process_definition(Definition const& d,
                            IncompatibleFunc& incompat,
                            PreferredDefs&    pref_defs) {
      if (def_version() == options::def_version::two) {
        return process_definition_v2(d, incompat, pref_defs);
      } else {
        LIBSEMIGROUPS_ASSERT(def_version() == options::def_version::one);
        return process_definition_v1(d, incompat, pref_defs);
      }
    }

    bool process_definitions(size_t start = 0) {
      ReturnFalse     incompat;
      NoPreferredDefs pref_defs;
      return process_definitions(start, incompat, pref_defs);
    }

    // Returns false if the targets of the edges (x, a, xa) and (y, b, yb)
    // cannot be merged (i.e. if xa != yb and incompat(x, a, y, b) returns
    // false), otherwise returns true. Always modifies the graph if xa !=
    // UNDEFINED and yb = UNDEFINED, or vice versa, and pref_defs(x, a, y,
    // b) is called if xa = UNDEFINED and yb = UNDEFINED.
    template <typename IncompatibleFunc, typename PreferredDefs>
    bool merge_targets_of_nodes_if_possible(node_type         x,
                                            letter_type       a,
                                            node_type         y,
                                            letter_type       b,
                                            IncompatibleFunc& incompat,
                                            PreferredDefs&    pref_defs);

    using word_iterator = typename word_type::const_iterator;

    template <typename IncompatibleFunc, typename PreferredDefs>
    bool merge_targets_of_paths_if_possible(node_type         u_node,
                                            word_iterator     u_first,
                                            word_iterator     u_last,
                                            node_type         v_node,
                                            word_iterator     v_first,
                                            word_iterator     v_last,
                                            IncompatibleFunc& incompat,
                                            PreferredDefs& pref_defs) noexcept;

   private:
    void init(size_type n);

    // TODO to tpp
    // TODO can we use a reference here?
    template <typename IncompatibleFunc, typename PreferredDefs>
    bool process_definition_v2(Definition        d,
                               IncompatibleFunc& incompat,
                               PreferredDefs&    pref_defs) {
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

    // TODO to tpp
    // TODO can we use a reference here?
    template <typename IncompatibleFunc, typename PreferredDefs>
    bool process_definition_v1(Definition        d,
                               IncompatibleFunc& incompat,
                               PreferredDefs&    pref_defs) {
      _felsch_tree.push_back(d.second);
      if (!process_definitions_dfs_v1(d.first, incompat, pref_defs)) {
        return false;
      }
      return true;
    }

    // Follows the paths from node c labelled by the left and right handsides
    // of the i-th rule, and returns merge_targets on the last but one nodes
    // and letters.
    template <typename IncompatibleFunc, typename PreferredDefs>
    inline bool merge_targets_of_paths_labelled_by_rules_if_possible(
        node_type const&  c,
        size_t            i,
        IncompatibleFunc& incompat,
        PreferredDefs&    pref_defs) noexcept {
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

    // Returns true if no contradictions are found.
    template <typename IncompatibleFunc, typename PreferredDefs>
    bool process_definitions_v1(size_t start,
                                IncompatibleFunc&,
                                PreferredDefs&);

    template <typename IncompatibleFunc, typename PreferredDefs>
    bool process_definitions_v2(size_t start,
                                IncompatibleFunc&,
                                PreferredDefs&);

    // Returns true if no contradictions are found.
    template <typename IncompatibleFunc, typename PreferredDefs>
    bool process_definitions_dfs_v1(node_type c,
                                    IncompatibleFunc&,
                                    PreferredDefs&);

    template <typename IncompatibleFunc, typename PreferredDefs>
    bool process_definitions_dfs_v2(node_type root,
                                    node_type c,
                                    IncompatibleFunc&,
                                    PreferredDefs&);
  };

  namespace felsch_digraph {
    // TODO compatible function for ActionDigraph

    // Check that [first_node, last_node) is compatible with [first_rule,
    // last_rule) or if there are edges missing in paths labelled by rules,
    // then try to fill those in so that fd is compatible.
    template <typename Word,
              typename Node,
              typename Incompatible,
              typename PrefDefs>
    bool
    make_compatible(FelschDigraph<Word, Node>&                    fd,
                    typename FelschDigraph<Word, Node>::node_type first_node,
                    typename FelschDigraph<Word, Node>::node_type last_node,
                    typename std::vector<Word>::const_iterator    first_rule,
                    typename std::vector<Word>::const_iterator    last_rule,
                    Incompatible&&                                incompat,
                    PrefDefs&& pref_defs) noexcept;

    template <typename Word, typename Node>
    bool
    make_compatible(FelschDigraph<Word, Node>&                    fd,
                    typename FelschDigraph<Word, Node>::node_type first_node,
                    typename FelschDigraph<Word, Node>::node_type last_node,
                    typename std::vector<Word>::const_iterator    first_rule,
                    typename std::vector<Word>::const_iterator    last_rule) {
      return make_compatible(
          fd,
          first_node,
          last_node,
          first_rule,
          last_rule,
          typename FelschDigraph<Word, Node>::ReturnFalse(),
          typename FelschDigraph<Word, Node>::NoPreferredDefs());
    }

  }  // namespace felsch_digraph
}  // namespace libsemigroups

#include "felsch-digraph.tpp"

#endif  // LIBSEMIGROUPS_FELSCH_DIGRAPH_HPP_
