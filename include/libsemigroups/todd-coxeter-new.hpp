//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-23 James D. Mitchell
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

// This file contains a declaration of a class for performing the Todd-Coxeter
// algorithm for semigroups and monoids.
//
// TODO:
// *  ensure that normal_forms etc work properly for monoid presentations (i.e.
//    when p.contains_empty_word() is true (see test case 101 for example)
// * implement reserve

#ifndef LIBSEMIGROUPS_TODD_COXETER_NEW_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_NEW_HPP_

#include "cong-intf-new.hpp"
#include "felsch-digraph.hpp"        // for FelschDigraph
#include "make-present.hpp"          // for make
#include "obvinf.hpp"                // for is_obviously_infinite
#include "order.hpp"                 // for order
#include "present.hpp"               // for Presentation
#include "todd-coxeter-digraph.hpp"  // for Digraph
#include "types.hpp"                 // for word_type

namespace libsemigroups {
  class ToddCoxeter : public v3::CongruenceInterface,
                      public FelschDigraphSettings<ToddCoxeter> {
    using FelschDigraphSettings_ = FelschDigraphSettings<ToddCoxeter>;

   public:
    using node_type  = typename ActionDigraph<uint32_t>::node_type;
    using label_type = typename ActionDigraph<uint32_t>::label_type;

    struct options : public FelschDigraphSettings_::options {
      enum class strategy {
        hlt,
        felsch,
        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy of the same
        //! name. The Felsch is run until at least f_defs() nodes are
        //! defined, then the HLT strategy is run until at least hlt_defs()
        //! divided by length_of_generating_pairs() nodes have been defined.
        //! These steps are repeated until the enumeration terminates.
        CR,
        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy R/C. The HLT
        //! strategy is run until the first lookahead is triggered (when
        //! number_of_cosets_active() is at least next_lookhead()). A full
        //! lookahead is then performed, and then the CR strategy is used.
        R_over_C,
        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Cr. The Felsch
        //! strategy is run until at least f_defs() new nodes have been
        //! defined, the HLT strategy is then run until at least hlt_defs()
        //! divided by length_of_generating_pairs() new nodes are defined,
        //! and then the Felsch strategy is run.
        Cr,
        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Rc. The HLT
        //! strategy is run until at least hlt_defs() divided by
        //! length_of_generating_pairs() new nodes have been
        //! defined, the Felsch strategy is then run until at least f_defs()
        //! new nodes are defined, and then the HLT strategy is run.
        Rc
      };
      enum class lookahead_extent { full, partial };
      enum class lookahead_style { hlt, felsch };
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
    };

   private:
    struct Settings {
      bool                      use_relations_in_extra = false;
      options::lookahead_style  lookahead_style = options::lookahead_style::hlt;
      options::lookahead_extent lookahead_extent
          = options::lookahead_extent::partial;
      float               lookahead_growth_factor    = 2.0;
      size_t              lookahead_growth_threshold = 4;
      size_t              lower_bound                = UNDEFINED;
      size_t              lookahead_min              = 10'000;
      size_t              lookahead_next             = 5'000'000;
      bool                save                       = false;
      options::strategy   strategy                   = options::strategy::hlt;
      size_t              def_max                    = 2'000;
      options::def_policy def_policy
          = options::def_policy::no_stack_if_no_space;
      size_t hlt_defs = 200'000;
      size_t f_defs   = 100'000;
    };

    class Definitions {
      using Definition = std::pair<node_type, label_type>;

     private:
      bool                    _any_skipped;
      std::vector<Definition> _definitions;
      ToddCoxeter const*      _tc;

     public:
      Definitions() : _any_skipped(false), _definitions(), _tc(nullptr) {}

      Definitions(Definitions const&)                 = default;
      Definitions(Definitions&&)                      = default;
      Definitions& operator=(Definitions const& that) = default;
      Definitions& operator=(Definitions&&)           = default;

      void init(ToddCoxeter const* tc) {
        _any_skipped = false;
        _definitions.clear();
        _tc = tc;
      }

      void emplace_back(node_type c, label_type x);

      [[nodiscard]] bool any_skipped() const noexcept {
        return _any_skipped;
      }

      [[nodiscard]] bool empty() const noexcept {
        return _definitions.empty();
      }

      void pop(Definition& d) {
        d = std::move(_definitions.back());
        _definitions.pop_back();
      }

      void clear() {
        _definitions.clear();
      }

     private:
      bool is_active_node(node_type n) const noexcept {
        return _tc->word_graph().is_active_node(n);
      }
    };  // Definitions

    class Digraph : public NodeManagedDigraph<
                        FelschDigraph<word_type, uint32_t, Definitions>> {
      using FelschDigraph_ = FelschDigraph<word_type, uint32_t, Definitions>;
      using NodeManagedDigraph_ = NodeManagedDigraph<FelschDigraph_>;
      using NodeManager_        = typename NodeManagedDigraph_::NodeManager_;

     public:
      using FelschDigraph_::def_edge_nc;
      using NodeManagedDigraph_::NodeManagedDigraph;

      Digraph& init(Presentation<word_type> const& p);
      Digraph& init(Presentation<word_type>&& p);

      void process_definitions();

      // TODO noexcept specification correct?
      template <bool RegDefs>
      void push_definition_hlt(node_type const& c,
                               word_type const& u,
                               word_type const& v) noexcept;

      // TODO don't think this needs to be a member function at all so, we can
      // move the declaration in to the cpp file
      template <typename Iterator>
      size_t make_compatible(node_type& current, Iterator first, Iterator last);
    };

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - data - private
    ////////////////////////////////////////////////////////////////////////

    // std::stack<Settings*>                _setting_stack;
    bool     _finished;
    Forest   _forest;
    Settings _settings;
    order    _standardized;
    Digraph  _word_graph;

   public:
    using digraph_type = Digraph;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeter();
    ToddCoxeter& init();

    ToddCoxeter(ToddCoxeter const& that)       = default;
    ToddCoxeter(ToddCoxeter&&)                 = default;
    ToddCoxeter& operator=(ToddCoxeter const&) = default;
    ToddCoxeter& operator=(ToddCoxeter&&)      = default;

    ~ToddCoxeter() = default;

    ToddCoxeter(congruence_kind knd);
    ToddCoxeter& init(congruence_kind knd);

    ToddCoxeter(congruence_kind knd, Presentation<word_type>&& p);
    ToddCoxeter& init(congruence_kind knd, Presentation<word_type>&& p);

    ToddCoxeter(congruence_kind knd, Presentation<word_type> const& p);
    ToddCoxeter& init(congruence_kind knd, Presentation<word_type> const& p);

    // This is a constructor and not a helper so that everything that takes a
    // presentation has the same constructors, regardless of what they use
    // inside.
    template <typename Word>
    ToddCoxeter(congruence_kind knd, Presentation<Word> const& p)
        : ToddCoxeter(knd, make<Presentation<word_type>>(p)) {}

    template <typename Word>
    ToddCoxeter& init(congruence_kind knd, Presentation<Word> const& p) {
      init(knd, make<Presentation<word_type>>(p));
      return *this;
    }

    template <typename Node>
    ToddCoxeter(congruence_kind knd, ActionDigraph<Node> const& ad)
        : ToddCoxeter(knd) {
      _word_graph = ad;
      _word_graph.presentation().alphabet(ad.out_degree());
    }

    template <typename Node>
    ToddCoxeter& init(congruence_kind knd, ActionDigraph<Node> const& ad) {
      init(knd);
      _word_graph = ad;
      _word_graph.presentation().alphabet(ad.out_degree());
      return *this;
    }

    ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc);
    ToddCoxeter& init(congruence_kind knd, ToddCoxeter const& tc);

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - settings - public
    ////////////////////////////////////////////////////////////////////////

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
    ToddCoxeter& def_policy(options::def_policy val) {
      _settings.def_policy = val;
      return *this;
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
    [[nodiscard]] options::def_policy def_policy() const noexcept {
      return _settings.def_policy;
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
    ToddCoxeter& def_max(size_t val) noexcept {
      _settings.def_max = val;
      return *this;
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
    [[nodiscard]] size_t def_max() const noexcept {
      return _settings.def_max;
    }

    using FelschDigraphSettings_::def_version;
    using FelschDigraphSettings_::settings;

    //! Specify the strategy.
    //!
    //! The strategy used during the enumeration can be specified using
    //! this function.
    //!
    //! The default value is options::strategy::hlt.
    //!
    //! \param val value indicating which strategy to use
    //!
    //! \returns A reference to `*this`.
    ToddCoxeter& strategy(options::strategy val);

    //! The current strategy for enumeration.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns The current strategy, a value of type options::strategy.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::strategy strategy() const noexcept;

    //! Set the style of lookahead to use in HLT.
    //!
    //! If the strategy is not HLT, then the value of this setting is
    //! ignored.
    //!
    //! The default value is options::lookahead::partial |
    //! options::lookahead::hlt. The other
    //! possible value are documented in options::lookahead.
    //!
    //! \param val value indicating whether to perform a full or partial
    //! lookahead in HLT or Felsch style.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_style(options::lookahead_style val) noexcept {
      _settings.lookahead_style = val;
      return *this;
    }

    // TODO doc
    ToddCoxeter& lookahead_extent(options::lookahead_extent val) noexcept {
      _settings.lookahead_extent = val;
      return *this;
    }

    //! The current value of the setting for lookaheads.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type options::lookahead.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::lookahead_style lookahead_style() const noexcept {
      return _settings.lookahead_style;
    }

    // TODO doc
    [[nodiscard]] options::lookahead_extent lookahead_extent() const noexcept {
      return _settings.lookahead_extent;
    }

    //! Set the threshold that will trigger a lookahead in HLT.
    //!
    //! If the number of cosets active exceeds the value set by this
    //! function, then a lookahead, of the type set using the function
    //! \ref lookahead, is triggered. This only applies when using
    //! the HLT strategy.
    //!
    //! The default value is 5 million.
    //!
    //! \param val value indicating the initial threshold.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_next(size_t val) noexcept;

    //! The current value of the next lookahead setting.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_next(size_t)
    [[nodiscard]] size_t lookahead_next() const noexcept;

    //! Set the minimum value of lookahead_next().
    //!
    //! After a lookahead is performed the value of lookahead_next() is
    //! modified depending on the outcome of the current lookahead. If the
    //! return value of lookahead_next() is too small or too large, then the
    //! value is adjusted according to lookahead_growth_factor() and
    //! lookahead_growth_threshold(). This setting specified the minimum
    //! possible value for lookahead_next().
    //!
    //! The default value is \c 10'000.
    //!
    //! \param val value indicating the minimum value of lookahead_next().
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_min(size_t val) noexcept;

    //! The current value of the minimum lookahead setting.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_min(size_t)
    [[nodiscard]] size_t lookahead_min() const noexcept;

    //! Set the lookahead growth factor.
    //!
    //! This setting determines by what factor the number of nodes required
    //! to trigger a lookahead grows. More specifically, at the end of any
    //! lookahead if the number of active nodes already exceeds the value of
    //! lookahead_next() or the number of nodes killed during the lookahead
    //! is less than the number of active nodes divided by
    //! lookahead_growth_threshold(), then the value of
    //! ToddCoxeter::next_lookhead is increased by a multiple of the \p value.
    //!
    //! \param val the value indicating the lookahead growth factor.
    //! The default value is ``2.0``.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is less than ``1.0``.
    ToddCoxeter& lookahead_growth_factor(float val);

    //! The current value of the lookahead growth factor.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `float`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_growth_factor(float)
    [[nodiscard]] float lookahead_growth_factor() const noexcept;

    //! Set the lookahead growth threshold.
    //!
    //! This setting determines the threshold for the number of nodes required
    //! to trigger a lookahead. More specifically, at the end of any lookahead
    //! if the number of active nodes already exceeds the value of
    //! lookahead_next() or the number of nodes killed during the lookahead is
    //! less than the number of active nodes divided by \ref
    //! lookahead_growth_threshold, then the value of
    //! ToddCoxeter::next_lookhead() is increased.
    //!
    //! The default value is ``4``.
    //!
    //! \param val the value indicating the lookahead growth threshold.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_growth_threshold(size_t val) noexcept;

    //! The current value of the lookahead growth threshold.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_growth_threshold()
    [[nodiscard]] size_t lookahead_growth_threshold() const noexcept;

    //! Process deductions during HLT.
    //!
    //! If the argument of this function is \c true and the HLT strategy is
    //! being used, then deductions are processed during the enumeration.
    //!
    //! The default value is \c false.
    //!
    //! \param val value indicating whether or not to process deductions.
    //!
    //! \returns A reference to `*this`.
    ToddCoxeter& save(bool val);

    //! The current value of save setting.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa save(bool)
    [[nodiscard]] bool save() const noexcept;

    //! Perform an HLT-style push of the defining relations at the identity.
    //!
    //! If a ToddCoxeter instance is defined over a finitely presented
    //! semigroup and the Felsch strategy is being used, it can be useful
    //! to follow all the paths from the identity labelled by the underlying
    //! relations of the semigroup (if any). The setting specifies whether or
    //! not to do this.
    //!
    //! The default value of this setting is \c false.
    //!
    //! \param val the boolean value.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& use_relations_in_extra(bool val) noexcept;

    //! The current value of the setting for using relations.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns The current value of the setting, a value of type ``bool``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool use_relations_in_extra() const noexcept;

    //! Specify minimum number of classes that may trigger early stop.
    //!
    //! Set a lower bound for the number of classes of the congruence
    //! represented by a ToddCoxeter instance. If the number of active cosets
    //! becomes at least the value of the argument, and the table is complete
    //! (\ref complete returns \c true), then the enumeration is terminated.
    //! When the given bound is equal to the number of classes, this may save
    //! tracing relations at many cosets when there is no possibility of
    //! finding coincidences.
    //!
    //! The default value is \ref UNDEFINED.
    //!
    //! \param val value indicating the lower bound.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lower_bound(size_t val) noexcept;

    //! The current value of the lower bound setting.
    //!
    //! \parameters
    //! None
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lower_bound(size_t)
    [[nodiscard]] size_t lower_bound() const noexcept;

    //! Specify what should be considered a large collapse.
    //!
    //! By default when processing coincidences nodes are merged in the word
    //! graph one pair at a time, and the in-neighbours of the surviving node
    //! are updated at the same time. If the number of coincidences is
    //! large, then it might be that a pair of nodes are merged at one step,
    //! then the surviving node is merged with another node at a future step,
    //! and this may happen many many times. This results in the
    //! in-neighbours of the surviving nodes being repeatedly traversed,
    //! which can result in a significant performance penalty.
    //! It can be beneficial to stop updating the in-neighbours as nodes are
    //! merged, and to just rebuild the entire in-neighbours data structure
    //! by traversing the entire word graph after all coincidences have been
    //! processed. This is beneficial if the number of surviving nodes is
    //! relatively small in comparison to the number of nodes merged.
    //! The purpose of this setting is to specify what should be
    //! considered a \"large\" collapse, or more precisely, what number of
    //! coincidences in the stack will trigger a change from updating the
    //! in-neighbours one-by-one to traversing the entire graph once after
    //! all coincidences have been processed.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& large_collapse(size_t val) noexcept;

    //! The current value of the large collapse setting.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t large_collapse() const noexcept;

    //! The approx number of Felsch style definitions in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
    //!
    //! If the strategy being used is any of those mimicking
    //! [ACE](https://staff.itee.uq.edu.au/havas/), then the value of this
    //! setting is used to determine the number of nodes defined in any Felsch
    //! phase of the strategy.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is \c 0.
    ToddCoxeter& f_defs(size_t val);

    //! The current value of the f_defs setting.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t f_defs() const noexcept;

    //! The approx number of HLT style definitions in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
    //!
    //! If the strategy being used is any of those mimicking
    //! [ACE](https://staff.itee.uq.edu.au/havas/), then the value of this
    //! setting is used to determine the number of nodes defined in any HLT
    //! phase of the strategy.
    //!
    //! The default value of this setting is \c 200'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is less than
    //! length_of_generating_pairs().
    ToddCoxeter& hlt_defs(size_t val);

    //! The current value of the hlt_defs setting.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t hlt_defs() const noexcept;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - accessors - public
    ////////////////////////////////////////////////////////////////////////

    Presentation<word_type> const& presentation() const noexcept {
      return _word_graph.presentation();
    }

    Digraph const& word_graph() const noexcept {
      return _word_graph;
    }

    Forest const& spanning_tree() const noexcept {
      return _forest;
    }

    inline order standardization_order() const noexcept {
      return _standardized;
    }

    bool contains(word_type const& lhs, word_type const& rhs) override;

    bool is_standardized(order val) const;
    bool is_standardized() const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    void shrink_to_fit();

    // Returns true if anything changed
    bool standardize(order val);

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    void run_impl() override;

    bool finished_impl() const override {
      return _finished;
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    word_type class_index_to_word_impl(class_index_type i) override;

    size_t number_of_classes_impl() override;

    class_index_type word_to_class_index_impl(word_type const& w) override;

    class_index_type
    const_word_to_class_index(word_type const& w) const override;

    void validate_word(word_type const& w) const override;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - main strategies - private
    ////////////////////////////////////////////////////////////////////////

    void init_run();
    void finalise_run();

    void felsch();
    void hlt();
    void CR_style();

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - reporting - private
    ////////////////////////////////////////////////////////////////////////

    void report_next_lookahead(size_t old_value) const;
    void report_nodes_killed(int64_t number) const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - lookahead - private
    ////////////////////////////////////////////////////////////////////////

    void   perform_lookahead();
    size_t hlt_lookahead();
    size_t felsch_lookahead();
  };

  namespace detail {
    using node_type = typename ToddCoxeter::node_type;
    struct NormalFormIteratorTraits
        : ConstIteratorTraits<IntegralRange<node_type>> {
      using value_type      = word_type;
      using const_reference = value_type const;
      using reference       = value_type;
      using const_pointer   = value_type const*;
      using pointer         = value_type*;

      using state_type = ToddCoxeter*;

      struct Deref {
        value_type operator()(state_type                               tc,
                              IntegralRange<node_type>::const_iterator it) {
          // It might seem better to just use forest::cbegin_paths, but we
          // can't because tc.kind() (runtime) determines whether or not the
          // paths should be reversed.
          return tc->class_index_to_word(*it);
        }
      };

      struct AddressOf {
        pointer operator()(state_type,
                           IntegralRange<node_type>::const_iterator) {
          LIBSEMIGROUPS_ASSERT(false);
          return nullptr;
        }
      };
    };
  }  // namespace detail

  namespace todd_coxeter {
    using node_type = typename ToddCoxeter::node_type;

    inline auto cbegin_class(ToddCoxeter& tc,
                             node_type    n,
                             size_t       min = 0,
                             size_t       max = POSITIVE_INFINITY) {
      // TODO don't add 1 if tc contains empty word?
      return tc.word_graph().cbegin_pstislo(0, n + 1, min, max);
    }

    inline auto cbegin_class(ToddCoxeter&     tc,
                             word_type const& w,
                             size_t           min = 0,
                             size_t           max = POSITIVE_INFINITY) {
      // TODO don't add 1 if tc contains empty word
      return tc.word_graph().cbegin_pstislo(
          0, tc.word_to_class_index(w) + 1, min, max);
    }

    inline auto cend_class(ToddCoxeter& tc) {
      return tc.word_graph().cend_pstislo();
    }

    inline auto number_of_words_in_class(ToddCoxeter const& tc, node_type i) {
      return tc.word_graph().number_of_paths(0, i + 1, 0, POSITIVE_INFINITY);
    }

    //! The type of a const iterator pointing to a normal form.
    //!
    //! Iterators of this type point to a \ref word_type.
    //!
    //! \sa cbegin_normal_forms, cend_normal_forms.
    // TODO(refactor): redo the doc
    using normal_form_iterator
        = detail::ConstIteratorStateful<detail::NormalFormIteratorTraits>;

    //! Returns a \ref normal_form_iterator pointing at the first normal
    //! form.
    //!
    //! Returns a const iterator pointing to the normal form of the first
    //! class of the congruence represented by an instance of ToddCoxeter.
    //! The order of the classes, and the normal form, that is returned are
    //! controlled by standardize(order).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref normal_form_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(refactor): redo the doc
    inline normal_form_iterator cbegin_normal_forms(ToddCoxeter& tc) {
      auto range = IntegralRange<node_type>(0, tc.number_of_classes());
      // TODO use rx::range instead
      return normal_form_iterator(&tc, range.cbegin());
    }

    //! Returns a \ref normal_form_iterator pointing one past the last normal
    //! form.
    //!
    //! Returns a const iterator one past the normal form of the last class
    //! of the congruence represented by an instance of ToddCoxeter. The
    //! order of the classes, and the normal form, that is returned are
    //! controlled by standardize(order).
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns A value of type \ref normal_form_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(refactor): redo the doc
    inline normal_form_iterator cend_normal_forms(ToddCoxeter& tc) {
      auto range = IntegralRange<node_type>(0, tc.number_of_classes());
      return normal_form_iterator(&tc, range.cend());
    }

    inline word_type normal_form(ToddCoxeter& tc, word_type const& w) {
      return tc.class_index_to_word(tc.word_to_class_index(w));
    }

    template <typename It>
    std::vector<std::vector<word_type>> partition(ToddCoxeter& tc,
                                                  It           first,
                                                  It           last) {
      using return_type = std::vector<std::vector<word_type>>;

      return_type result(tc.number_of_classes(), std::vector<word_type>());

      for (auto it = first; it != last; ++it) {
        LIBSEMIGROUPS_ASSERT(tc.word_to_class_index(*it) < result.size());
        result[tc.word_to_class_index(*it)].push_back(*it);
      }
      return result;
    }

    template <typename It>
    std::vector<std::vector<word_type>> non_trivial_classes(ToddCoxeter& tc,
                                                            It           first,
                                                            It           last) {
      auto result = partition(tc, first, last);

      result.erase(
          std::remove_if(result.begin(),
                         result.end(),
                         [](auto const& x) -> bool { return x.size() <= 1; }),
          result.end());
      return result;
    }

    //! Check if the congruence has more than one class.
    //!
    //! Returns tril::TRUE if it is possible to show that the congruence is
    //! non-trivial; tril::FALSE if the congruence is already known to be
    //! trivial; and tril::unknown if it is not possible to show that the
    //! congruence is non-trivial.
    //!
    //! This function attempts to find a non-trivial congruence containing
    //! the congruence represented by a ToddCoxeter instance by repeating the
    //! following steps on a copy until the enumeration concludes:
    //! 1. running the enumeration for the specified amount of time
    //! 2. repeatedly choosing a random pair of cosets and identifying them,
    //!    until the number of cosets left in the quotient is smaller than
    //!    \p threshold times the initial number of cosets for this step.
    //! If at the end of this process, the ToddCoxeter instance is
    //! non-trivial, then the original ToddCoxeter is also non-trivial.
    //! Otherwise, the entire process is repeated again up to a total of \p
    //! tries times.
    //!
    //! \param tries the number of attempts to find non-trivial
    //! super-congruence.
    //! \param try_for the amount of time in millisecond to enumerate the
    //! congruence after choosing a random pair of representatives and
    //! identifying them.
    //! \param threshold the threshold (see description).
    //!
    //! \returns A value of type \ref tril
    // TODO redo the doc
    tril is_non_trivial(ToddCoxeter&              tc,
                        size_t                    tries = 10,
                        std::chrono::milliseconds try_for
                        = std::chrono::milliseconds(100),
                        float threshold = 0.99);
  }  // namespace todd_coxeter

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TODD_COXETER_HPP_
