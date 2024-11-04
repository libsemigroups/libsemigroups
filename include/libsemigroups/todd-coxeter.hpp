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
// * re-implement SettingsGuard (push_settings and pop_settings)
// * re-implement reserve
// * remove preferred_defs from FelschGraph etc (except where they are really
// needed)?
// * re-add report why stopped

#ifndef LIBSEMIGROUPS_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_HPP_

#include "cong-intf.hpp"
#include "exception.hpp"        // for LIBSEMIGROUPS_EXCEPTION
#include "obvinf.hpp"           // for is_obviously_infinite
#include "order.hpp"            // for order
#include "paths.hpp"            // for Paths
#include "presentation.hpp"     // for Presentation
#include "to-presentation.hpp"  // for to_presentation
#include "types.hpp"            // for word_type

#include "detail/felsch-graph.hpp"        // for FelschGraph
#include "detail/node-managed-graph.hpp"  // for NodeManagedGraph
#include "detail/report.hpp"              // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {
  // TODO(0) doc
  class ToddCoxeter : public CongruenceInterface,
                      public detail::FelschGraphSettings<ToddCoxeter> {
    using FelschGraphSettings_ = FelschGraphSettings<ToddCoxeter>;

   public:
    // TODO(0) doc
    using node_type = typename WordGraph<uint32_t>::node_type;
    // TODO(0) doc
    using label_type = typename WordGraph<uint32_t>::label_type;

    // TODO(0) doc
    template <typename T>
    void report_every(T val) {
      CongruenceInterface::report_every(val);
      _word_graph.report_every(val);
    }

    using Reporter::report_every;

    // TODO(0) doc
    struct options : public FelschGraphSettings_::options {
      // TODO(0) doc
      enum class strategy {
        // TODO(0) doc
        hlt,
        // TODO(0) doc
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

      // TODO(0) doc
      enum class lookahead_extent { full, partial };

      // TODO(0) doc
      enum class lookahead_style { hlt, felsch };

      // TODO(0) doc
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
      size_t                    def_max;
      options::def_policy       def_policy;
      size_t                    hlt_defs;
      size_t                    f_defs;
      options::lookahead_extent lookahead_extent;
      float                     lookahead_growth_factor;
      size_t                    lookahead_growth_threshold;
      size_t                    lookahead_min;
      size_t                    lookahead_next;
      options::lookahead_style  lookahead_style;
      size_t                    lower_bound;
      bool                      save;
      options::strategy         strategy;
      bool                      use_relations_in_extra;

      Settings()
          : def_max(),
            def_policy(),
            hlt_defs(),
            f_defs(),
            lookahead_extent(),
            lookahead_growth_factor(),
            lookahead_growth_threshold(),
            lookahead_min(),
            lookahead_next(),
            lookahead_style(),
            lower_bound(),
            save(),
            strategy(),
            use_relations_in_extra() {
        init();
      }

      Settings(Settings const&)            = default;
      Settings(Settings&&)                 = default;
      Settings& operator=(Settings const&) = default;
      Settings& operator=(Settings&&)      = default;

      Settings& init() {
        def_max                    = 2'000;
        def_policy                 = options::def_policy::no_stack_if_no_space;
        hlt_defs                   = 200'000;
        f_defs                     = 100'000;
        lookahead_extent           = options::lookahead_extent::partial;
        lookahead_growth_factor    = 2.0;
        lookahead_growth_threshold = 4;
        lower_bound                = UNDEFINED;
        lookahead_min              = 10'000;
        lookahead_next             = 5'000'000;
        lookahead_style            = options::lookahead_style::hlt;
        save                       = false;
        strategy                   = options::strategy::hlt;
        use_relations_in_extra     = false;
        return *this;
      }
    };

    Settings&       tc_settings();
    Settings const& tc_settings() const;

    class Definitions {
      using Definition = std::pair<node_type, label_type>;

     private:
      bool                    _any_skipped;
      std::vector<Definition> _definitions;
      ToddCoxeter const*      _tc;

     public:
      Definitions() : _any_skipped(false), _definitions(), _tc(nullptr) {}
      // TODO(0) init()

      Definitions(Definitions const&)                 = default;
      Definitions(Definitions&&)                      = default;
      Definitions& operator=(Definitions const& that) = default;
      Definitions& operator=(Definitions&&)           = default;

      // TODO(0) corresponding constructor
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

    class Graph : public detail::NodeManagedGraph<
                      detail::FelschGraph<word_type, uint32_t, Definitions>> {
      using FelschGraph_
          = detail::FelschGraph<word_type, uint32_t, Definitions>;
      using NodeManagedGraph_ = NodeManagedGraph<FelschGraph_>;

     public:
      using node_type = typename NodeManagedGraph_::node_type;

      Graph()                        = default;
      Graph(Graph const&)            = default;
      Graph(Graph&&)                 = default;
      Graph& operator=(Graph const&) = default;
      Graph& operator=(Graph&&)      = default;
      // TODO(0) init()

      Graph& operator=(WordGraph<node_type> const& wg) {
        NodeManagedGraph_::operator=(wg);
        return *this;
      }

      using FelschGraph_::target_no_checks;
      using NodeManagedGraph_::NodeManagedGraph;

      // TODO(0) corresponding constructors
      Graph& init(Presentation<word_type> const& p);
      Graph& init(Presentation<word_type>&& p);

      void process_definitions();

      // TODO(0) noexcept specification correct?
      template <bool RegDefs>
      void push_definition_hlt(node_type const& c,
                               word_type const& u,
                               word_type const& v) noexcept;

      // TODO(0) don't think this needs to be a member function at all so, we
      // can move the declaration in to the cpp file
      template <typename Iterator>
      size_t make_compatible(node_type& current,
                             Iterator   first,
                             Iterator   last,
                             bool       stop_early);
    };

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - data - private
    ////////////////////////////////////////////////////////////////////////

    bool                                   _finished;
    Forest                                 _forest;
    std::vector<std::unique_ptr<Settings>> _setting_stack;
    Order                                  _standardized;
    Graph                                  _word_graph;

   public:
    // TODO(0) use this everywhere
    using word_graph_type = Graph;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    ToddCoxeter();
    // TODO(0) doc
    ToddCoxeter& init();

    // TODO(0) doc
    ToddCoxeter(ToddCoxeter const& that);

    // TODO(0) doc
    ToddCoxeter(ToddCoxeter&&) = default;
    // TODO(0) doc
    ToddCoxeter& operator=(ToddCoxeter const&);
    // TODO(0) doc
    ToddCoxeter& operator=(ToddCoxeter&&) = default;

    ~ToddCoxeter();

    // TODO(0) doc
    explicit ToddCoxeter(congruence_kind knd);
    // TODO(0) doc
    ToddCoxeter& init(congruence_kind knd);

    // TODO(0) doc
    ToddCoxeter(congruence_kind knd, Presentation<word_type>&& p);
    // TODO(0) doc
    ToddCoxeter& init(congruence_kind knd, Presentation<word_type>&& p);

    // TODO(0) doc
    ToddCoxeter(congruence_kind knd, Presentation<word_type> const& p);
    // TODO(0) doc
    ToddCoxeter& init(congruence_kind knd, Presentation<word_type> const& p);

    // This is a constructor and not a helper so that everything that takes a
    // presentation has the same constructors, regardless of what they use
    // inside.
    // TODO(0) doc
    template <typename Word>
    ToddCoxeter(congruence_kind knd, Presentation<Word> const& p)
        : ToddCoxeter(knd, to_presentation<word_type>(p)) {}

    // TODO(0) doc
    template <typename Word>
    ToddCoxeter& init(congruence_kind knd, Presentation<Word> const& p) {
      init(knd, to_presentation<word_type>(p));
      return *this;
    }

    // TODO(0) doc
    template <typename Node>
    ToddCoxeter(congruence_kind knd, WordGraph<Node> const& ad)
        : ToddCoxeter(knd) {
      _word_graph = ad;
      _word_graph.presentation().alphabet(ad.out_degree());
      _word_graph.report_prefix("ToddCoxeter");
    }

    // TODO(0) doc
    template <typename Node>
    ToddCoxeter& init(congruence_kind knd, WordGraph<Node> const& ad) {
      init(knd);
      _word_graph = ad;
      _word_graph.presentation().alphabet(ad.out_degree());
      return *this;
    }

    // TODO(0) doc
    ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc);
    // TODO(0) doc
    ToddCoxeter& init(congruence_kind knd, ToddCoxeter const& tc);

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual - public
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    // TODO(0) change to iterators
    [[nodiscard]] bool contains(word_type const& lhs,
                                word_type const& rhs) override;

    // TODO(0)  remove since it falls right through
    [[nodiscard]] uint64_t number_of_classes() override {
      return number_of_classes_impl();
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - settings - public
    ////////////////////////////////////////////////////////////////////////

    using FelschGraphSettings_::def_version;
    using FelschGraphSettings_::settings;

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
    ToddCoxeter& def_max(size_t val) noexcept;

    //! The current value of the setting for the maximum number of
    //! definitions.
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t def_max() const noexcept;

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
    ToddCoxeter& def_policy(options::def_policy val);

    //! The current value of the definition policy setting.
    //!
    //! \returns The current value of the setting, a value of type
    //! ``options::definitions``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::def_policy def_policy() const noexcept;

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
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t hlt_defs() const noexcept;

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
    // TODO(0) remove?
    ToddCoxeter& large_collapse(size_t val) noexcept;

    //! The current value of the large collapse setting.
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    // TODO(0) remove?
    [[nodiscard]] size_t large_collapse() const noexcept;

    // TODO(0) doc
    ToddCoxeter& lookahead_extent(options::lookahead_extent val) noexcept;

    // TODO(0) doc
    [[nodiscard]] options::lookahead_extent lookahead_extent() const noexcept;

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
    ToddCoxeter& lookahead_style(options::lookahead_style val) noexcept;

    //! The current value of the setting for lookaheads.
    //!
    //! \returns A value of type options::lookahead.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::lookahead_style lookahead_style() const noexcept;

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
    //! \returns The current strategy, a value of type options::strategy.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::strategy strategy() const noexcept;

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
    //! \returns The current value of the setting, a value of type ``bool``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool use_relations_in_extra() const noexcept;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - accessors - public
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    Presentation<word_type> const& presentation() const noexcept {
      return _word_graph.presentation();
    }

    // TODO(0) doc
    Graph const& word_graph() const noexcept {
      return _word_graph;
    }

    // TODO(0) doc
    Forest const& spanning_tree() const noexcept {
      return _forest;
    }

    // TODO(0) doc
    inline Order standardization_order() const noexcept {
      return _standardized;
    }

    // TODO(0) doc
    bool is_standardized(Order val) const;
    // TODO(0) doc
    bool is_standardized() const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    void shrink_to_fit();

    // Returns true if anything changed
    // TODO:(0) The documentation for Order used to contain a description of
    // what each order means with respect to this function. However, the Order
    // enum is now used in other places, so those descriptions should be written
    // here instead. That documentation is placed here for reference:
    // No standardization has been done.
    // none = 0,
    // Normal forms are the short-lex least word belonging to a given
    // congruence class.
    // shortlex,
    // The congruence classes are ordered lexicographically by their
    // normal form. The normal forms themselves are essentially arbitrary
    // because there is not necessarily a lexicographically least word in
    // every class.
    // lex,
    // Normal forms are the recursive-path least word belonging to a given
    // congruence class.
    // recursive
    bool standardize(Order val);

    // TODO(0) doc
    // TODO(0) rename class_index
    // TODO(0) use iterators not word_type
    node_type word_to_class_index(word_type const& w) {
      validate_word(w);
      return word_to_class_index_impl(w);
    }

    // TODO(0) doc
    // TODO(0) rename to_word
    // TODO(0) use iterators not word_type
    // TODO(0) to cpp file
    word_type class_index_to_word(node_type i) {
      if (i >= number_of_classes()) {
        LIBSEMIGROUPS_EXCEPTION("invalid class index, expected a value in "
                                "the range [0, {}), found {}",
                                number_of_classes(),
                                i);
      }
      return class_index_to_word_impl(i);
    }

    // TODO(0) doc
    // TODO(0) rename current_contains or currently_contains
    // TODO(0) use iterators not word_type
    // TODO(0) to cpp file
    tril const_contains(word_type const& u, word_type const& v) const {
      if (u == v) {
        return tril::TRUE;
      }
      node_type uu, vv;
      try {
        uu = const_word_to_class_index(u);
        vv = const_word_to_class_index(v);
      } catch (LibsemigroupsException const& e) {
        report_default("ignoring exception:\n%s", e.what());
        return tril::unknown;
      }
      if (uu == UNDEFINED || vv == UNDEFINED) {
        return tril::unknown;
      } else if (uu == vv) {
        return tril::TRUE;
      } else if (finished()) {
        return tril::FALSE;
      } else {
        return tril::unknown;
      }
    }

    // stop_early indicates that if too few nodes are killed in 1 second, then
    // the lookahead aborts, this should not happen if we are doing a final
    // lookahead because we skipped some deductions
    // TODO(0) doc
    // TODO(0) change to taking a function
    void perform_lookahead(bool stop_early);

   private:
    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    void really_run_impl();

    void run_impl() override;

    bool finished_impl() const override {
      return _finished;
    }

    ////////////////////////////////////////////////////////////////////////
    // CongruenceInterface - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) all of these should be public and called _no_checks
    word_type class_index_to_word_impl(node_type i);

    uint64_t number_of_classes_impl();

    node_type word_to_class_index_impl(word_type const& w);

    node_type const_word_to_class_index(word_type const& w) const;

    void validate_word(word_type const& w) const override;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - main strategies - private
    ////////////////////////////////////////////////////////////////////////

    void init_run();
    void finalise_run();

    void felsch();
    void hlt();
    void CR_style();
    void R_over_C_style();
    void Cr_style();
    void Rc_style();

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - reporting - private
    ////////////////////////////////////////////////////////////////////////

    void report_next_lookahead(size_t old_value) const;
    void report_nodes_killed(int64_t number) const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - lookahead - private
    ////////////////////////////////////////////////////////////////////////

    static constexpr bool StopEarly      = true;
    static constexpr bool DoNotStopEarly = false;

    size_t hlt_lookahead(bool stop_early);
    size_t felsch_lookahead();
  };

  namespace todd_coxeter {
    using node_type = typename ToddCoxeter::node_type;

    // TODO(0) doc
    inline auto class_of(ToddCoxeter& tc, node_type n) {
      size_t const offset = (tc.presentation().contains_empty_word() ? 0 : 1);
      return Paths(tc.word_graph()).source(0).target(n + offset);
    }

    // TODO(0) doc
    // TODO(0) iterator version
    inline auto class_of(ToddCoxeter& tc, word_type const& w) {
      return class_of(tc, tc.word_to_class_index(w));
    }

    //! Returns a \ref normal_form_iterator pointing at the first normal
    //! form.
    //!
    //! Returns a const iterator pointing to the normal form of the first
    //! class of the congruence represented by an instance of ToddCoxeter.
    //! The order of the classes, and the normal form, that is returned are
    //! controlled by standardize(Order).
    //!
    //! \returns A value of type \ref normal_form_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(0): redo the doc
    inline auto normal_forms(ToddCoxeter& tc) {
      return rx::seq() | rx::take(tc.number_of_classes())
             | rx::transform(
                 [&tc](auto i) { return tc.class_index_to_word(i); });
    }

    // TODO(0) doc
    inline word_type normal_form(ToddCoxeter& tc, word_type const& w) {
      return tc.class_index_to_word(tc.word_to_class_index(w));
    }

    // TODO(0) doc
    std::vector<std::vector<word_type>> non_trivial_classes(ToddCoxeter& tc1,
                                                            ToddCoxeter& tc2);

    // TODO(0) doc
    uint64_t number_of_idempotents(ToddCoxeter& tc);

    // TODO(0) doc
    // TODO(1) range version
    template <typename Iterator>
    std::pair<Iterator, Iterator> first_equivalent_pair(ToddCoxeter& tc,
                                                        Iterator     first,
                                                        Iterator     last) {
      std::unordered_map<ToddCoxeter::node_type, Iterator> map;
      size_t                                               index = 0;
      for (auto it = first; it != last; ++it, ++index) {
        auto [map_it, inserted] = map.emplace(tc.word_to_class_index(*it), it);
        if (!inserted) {
          return std::pair(map_it->second, it);
        }
      }
      return std::pair(last, last);
    }

    // TODO(0) doc
    template <typename Iterator>
    bool is_traversal(ToddCoxeter& tc, Iterator first, Iterator last) {
      return first_equivalent_pair(tc, first, last) == std::pair(last, last);
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
    // TODO(0) redo the doc
    tril is_non_trivial(ToddCoxeter&              tc,
                        size_t                    tries = 10,
                        std::chrono::milliseconds try_for
                        = std::chrono::milliseconds(100),
                        float threshold = 0.99);

    // FIXME run_for seems to not function properly here.
    // TODO(0) doc
    // TODO(0) out of line this
    template <typename Word, typename Time>
    [[nodiscard]] auto redundant_rule(Presentation<Word> const& p, Time t) {
      constexpr static congruence_kind twosided = congruence_kind::twosided;

      p.validate();
      Presentation<Word> q;
      q.alphabet(p.alphabet());
      q.contains_empty_word(p.contains_empty_word());
      ToddCoxeter tc(twosided);
      ToWord      to_word(p.alphabet());

      for (auto omit = p.rules.crbegin(); omit != p.rules.crend(); omit += 2) {
        q.rules.clear();
        q.rules.insert(q.rules.end(), p.rules.crbegin(), omit);
        q.rules.insert(q.rules.end(), omit + 2, p.rules.crend());
        tc.init(twosided, q);
        tc.run_for(t);
        if (tc.word_to_class_index(to_word(*omit))
            == tc.word_to_class_index(to_word(*(omit + 1)))) {
          return (omit + 1).base() - 1;
        }
      }
      return p.rules.cend();
    }

  }  // namespace todd_coxeter

  // TODO(0) to_human_readable_repr

  // TODO(0) doc
  // TODO(0) out of line
  template <typename Range,
            typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
  std::vector<std::vector<word_type>> partition(ToddCoxeter& tc, Range r) {
    static_assert(
        std::is_same_v<std::decay_t<typename Range::output_type>, word_type>);
    using return_type = std::vector<std::vector<word_type>>;

    if (tc.number_of_classes() == POSITIVE_INFINITY) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 1st argument defines a congruence with infinitely many "
          "classes, the non-trivial classes cannot be determined!");
      // They really can't be determined because we cannot run ToddCoxeter at
      // all
    } else if (!r.is_finite) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument (a range) must be finite, "
                              "found an infinite range");
    }
    return_type         result;
    std::vector<size_t> lookup;
    size_t              next_index = 0;

    while (!r.at_end()) {
      auto       next  = r.get();
      auto const index = tc.word_to_class_index(next);
      if (index >= lookup.size()) {
        lookup.resize(index + 1, UNDEFINED);
      }
      if (lookup[index] == UNDEFINED) {
        lookup[index] = next_index++;
        result.emplace_back();
      }
      result[lookup[index]].push_back(std::move(next));
      r.next();
    }
    return result;
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TODD_COXETER_HPP_
