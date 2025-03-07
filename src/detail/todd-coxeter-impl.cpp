//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains an implementation of the Todd-Coxeter algorithm for
// semigroups and monoids.

#include "libsemigroups/detail/todd-coxeter-impl.hpp"

#include <string_view>  // for basic_string_view
#include <tuple>        // for tie

#include "libsemigroups/constants.hpp"     // for operator==, operator!=
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"     // for LIBSEMIGROUPS_EXCEP...
#include "libsemigroups/forest.hpp"        // for Forest
#include "libsemigroups/obvinf.hpp"        // for is_obviously_infinite
#include "libsemigroups/order.hpp"         // for Order
#include "libsemigroups/presentation.hpp"  // for Presentation, length
#include "libsemigroups/runner.hpp"        // for Runner::run_until
#include "libsemigroups/types.hpp"         // for word_type, letter_type

#include "libsemigroups/detail/cong-common-class.hpp"  // for detail::CongruenceCommon
#include "libsemigroups/detail/felsch-graph.hpp"  // for DoNotRegisterDefs
#include "libsemigroups/detail/fmt.hpp"       // for format_decimal, copy_str
#include "libsemigroups/detail/iterator.hpp"  // for operator+
#include "libsemigroups/detail/node-manager.hpp"  // for NodeManager
#include "libsemigroups/detail/report.hpp"        // for report_no_prefix

namespace libsemigroups {
  namespace detail {
    using node_type = typename ToddCoxeterImpl::node_type;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl::Settings
    ////////////////////////////////////////////////////////////////////////

    struct ToddCoxeterImpl::Settings {
      size_t                    def_max;
      options::def_policy       def_policy;
      size_t                    hlt_defs;
      size_t                    f_defs;
      options::lookahead_extent lookahead_extent;
      float                     lookahead_growth_factor;
      size_t                    lookahead_growth_threshold;
      size_t                    lookahead_min;
      size_t                    lookahead_next;
      std::chrono::nanoseconds  lookahead_stop_early_interval;
      float                     lookahead_stop_early_ratio;
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
            lookahead_stop_early_interval(),
            lookahead_stop_early_ratio(),
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
        lookahead_stop_early_interval = std::chrono::seconds(1);
        lookahead_stop_early_ratio    = 0.01;
        lookahead_style               = options::lookahead_style::hlt;

        save                   = false;
        strategy               = options::strategy::hlt;
        use_relations_in_extra = false;
        return *this;
      }
    };  // class ToddCoxeterImpl::Settings

    ToddCoxeterImpl::Settings& ToddCoxeterImpl::tc_settings() {
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      return *_settings_stack.back();
    }

    ToddCoxeterImpl::Settings const& ToddCoxeterImpl::tc_settings() const {
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      return *_settings_stack.back();
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl::SettingsGuard
    ////////////////////////////////////////////////////////////////////////

    class ToddCoxeterImpl::SettingsGuard {
      ToddCoxeterImpl* _tc;

     public:
      explicit SettingsGuard(ToddCoxeterImpl* tc) : _tc(tc) {
        _tc->_settings_stack.push_back(std::make_unique<Settings>());
      }

      ~SettingsGuard() {
        _tc->_settings_stack.pop_back();
        LIBSEMIGROUPS_ASSERT(!_tc->_settings_stack.empty());
      }
    };  // class ToddCoxeterImpl::SettingsGuard

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl::Graph
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeterImpl::Graph& ToddCoxeterImpl::Graph::init() {
      NodeManager<node_type>::clear();
      FelschGraph_::init();
      // TODO(1) shouldn't add nodes here because then there'll be more than
      // there should be (i.e. NodeManager and FelschGraph_ will have
      // different numbers of nodes
      FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
      return *this;
    }

    ToddCoxeterImpl::Graph&
    ToddCoxeterImpl::Graph::init(Presentation<word_type> const& p) {
      NodeManager<node_type>::clear();
      FelschGraph_::init(p);
      // TODO(1) shouldn't add nodes here because then there'll be more than
      // there should be (i.e. NodeManager and FelschGraph_ will have
      // different numbers of nodes
      FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
      return *this;
    }

    ToddCoxeterImpl::Graph&
    ToddCoxeterImpl::Graph::init(Presentation<word_type>&& p) {
      NodeManager<node_type>::clear();
      FelschGraph_::init(std::move(p));
      // TODO(1) shouldn't add nodes here because then there'll be more than
      // there should be (i.e. NodeManager and FelschGraph_ will have
      // different numbers of nodes
      FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
      return *this;
    }

    void ToddCoxeterImpl::Graph::process_definitions() {
      if (presentation().rules.empty()) {
        return;
      }
      CollectCoincidences incompat(_coinc);
      NoPreferredDefs     pref_defs;

      auto&      defs = FelschGraph_::definitions();
      Definition d;
      while (!defs.empty()) {
        while (!defs.empty()) {
          defs.pop(d);
          if (NodeManager<node_type>::is_active_node(d.first)) {
            FelschGraph_::process_definition(d, incompat, pref_defs);
          }
        }
        process_coincidences<RegisterDefs>();
      }
    }

    template <bool RegDefs>
    void ToddCoxeterImpl::Graph::push_definition_hlt(node_type const& c,
                                                     word_type const& u,
                                                     word_type const& v) {
      LIBSEMIGROUPS_ASSERT(NodeManager<node_type>::is_active_node(c));

      node_type   x, y;
      letter_type a, b;

      if (!u.empty()) {
        x = complete_path<RegDefs>(c, u.begin(), u.cend() - 1).second;
        a = u.back();
      } else {
        x = c;
        a = UNDEFINED;
      }

      if (!v.empty()) {
        y = complete_path<RegDefs>(c, v.begin(), v.cend() - 1).second;
        b = v.back();
      } else {
        y = c;
        b = UNDEFINED;
      }

      CollectCoincidences incompat(_coinc);
      auto                pref_defs
          = [this](node_type xx, letter_type aa, node_type yy, letter_type bb) {
              node_type d = new_node();
              target_no_checks<RegDefs>(xx, aa, d);
              if (aa != bb || xx != yy) {
                target_no_checks<RegDefs>(yy, bb, d);
              }
            };

      FelschGraph_::merge_targets_of_nodes_if_possible<RegDefs>(
          x, a, y, b, incompat, pref_defs);
    }

    template <typename RuleIterator>
    size_t ToddCoxeterImpl::Graph::make_compatible(
        node_type&               current,
        RuleIterator             first,
        RuleIterator             last,
        bool                     stop_early,
        std::chrono::nanoseconds stop_early_interval,
        float                    stop_early_ratio) {
      detail::Timer t;
      size_t const  old_number_of_killed
          = NodeManager<node_type>::number_of_nodes_killed();
      CollectCoincidences                    incompat(_coinc);
      typename FelschGraph_::NoPreferredDefs prefdefs;

      size_t killed_at_prev_interval = old_number_of_killed;
      while (current != NodeManager<node_type>::first_free_node()) {
        // TODO(1) when we have an RuleIterator into the active nodes, we
        // should remove the while loop, and use that in make_compatible
        // instead. At present there is a cbegin/cend_active_nodes in
        // NodeManager but the RuleIterators returned by them are invalidated by
        // any changes to the graph, such as those made by
        // felsch_graph::make_compatible.
        detail::felsch_graph::make_compatible<detail::DoNotRegisterDefs>(
            *this, current, current + 1, first, last, incompat, prefdefs);
        // Using NoPreferredDefs is just a (more or less) arbitrary
        // choice, could allow the other choices here too (which works,
        // but didn't seem to be very useful).
        process_coincidences<detail::DoNotRegisterDefs>();
        current = NodeManager<node_type>::next_active_node(current);
        if (stop_early && t > stop_early_interval) {
          size_t killed_last_interval
              = number_of_nodes_killed() - killed_at_prev_interval;
          killed_at_prev_interval = number_of_nodes_killed();
          if (killed_last_interval
              < number_of_nodes_active() * stop_early_ratio) {
            report_no_prefix("{:-<90}\n", "");
            report_default(
                "ToddCoxeter: too few nodes killed in last {}, expected >= "
                "{}, found {}, aborting lookahead . . .\n",
                detail::string_time(stop_early_interval),
                detail::group_digits(static_cast<size_t>(
                    number_of_nodes_active() * stop_early_ratio)),
                detail::group_digits(killed_last_interval));
            report_no_prefix("{:-<90}\n", "");
            break;
          }
        }
      }
      return NodeManager<node_type>::number_of_nodes_killed()
             - old_number_of_killed;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl::Definitions
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::Definitions::emplace_back(node_type c, label_type x) {
      using def_policy = typename options::def_policy;

      if (_tc == nullptr  // this can be the case if for example we're
                          // in the FelschGraph constructor from
                          // WordGraph, in that case we always want
                          // all of the definitions
          || _tc->def_policy() == def_policy::unlimited
          || _definitions.size() < _tc->def_max()) {
        _definitions.emplace_back(c, x);
        return;
      }

      // We will skip the input definition (c, x)!
      _any_skipped = true;
      switch (_tc->def_policy()) {
        case def_policy::purge_from_top: {
          while (!_definitions.empty()
                 && !is_active_node(_definitions.back().first)) {
            _definitions.pop_back();
          }
          break;
        }
        case def_policy::purge_all: {
          _definitions.erase(std::remove_if(_definitions.begin(),
                                            _definitions.end(),
                                            [this](Definition const& d) {
                                              return !is_active_node(d.first);
                                            }),
                             _definitions.end());
          break;
        }
        case def_policy::discard_all_if_no_space: {
          clear();
          break;
        }
        case def_policy::no_stack_if_no_space:
        case def_policy::unlimited:
        default: {
          break;
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeterImpl::ToddCoxeterImpl()
        : detail::CongruenceCommon(),
          _finished(),
          _forest(),
          _settings_stack(),
          _standardized(),
          _word_graph() {
      init();
    }

    ToddCoxeterImpl& ToddCoxeterImpl::init() {
      detail::CongruenceCommon::init();
      _finished = false;
      _forest.init();
      if (_settings_stack.empty()) {
        _settings_stack.push_back(std::make_unique<Settings>());
      } else {
        _settings_stack.erase(_settings_stack.begin() + 1,
                              _settings_stack.end());
        _settings_stack.back()->init();
      }
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      _standardized = Order::none;
      _word_graph.init();
      copy_settings_into_graph();
      return *this;
    }

    ToddCoxeterImpl::ToddCoxeterImpl(ToddCoxeterImpl const& that)
        : ToddCoxeterImpl() {
      operator=(that);
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
    }

    ToddCoxeterImpl::ToddCoxeterImpl(ToddCoxeterImpl&& that)
        : ToddCoxeterImpl() {
      operator=(std::move(that));
    }

    ToddCoxeterImpl& ToddCoxeterImpl::operator=(ToddCoxeterImpl&& that) {
      LIBSEMIGROUPS_ASSERT(!that._settings_stack.empty());
      detail::CongruenceCommon::operator=(std::move(that));
      _finished       = std::move(that._finished);
      _forest         = std::move(that._forest);
      _settings_stack = std::move(that._settings_stack);
      _standardized   = std::move(that._standardized);
      _word_graph     = std::move(that._word_graph);
      // The next line is essential so that the _word_graph.definitions()._tc
      // points at <this>.
      _word_graph.definitions().init(this);
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      return *this;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::operator=(ToddCoxeterImpl const& that) {
      LIBSEMIGROUPS_ASSERT(!that._settings_stack.empty());
      detail::CongruenceCommon::operator=(that);
      _finished = that._finished;
      _forest   = that._forest;
      _settings_stack.clear();
      for (auto const& uptr : that._settings_stack) {
        _settings_stack.push_back(std::make_unique<Settings>(*uptr));
      }
      _standardized = that._standardized;
      _word_graph   = that._word_graph;
      return *this;
    }

    ToddCoxeterImpl::ToddCoxeterImpl(congruence_kind           knd,
                                     Presentation<word_type>&& p)
        : ToddCoxeterImpl() {
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      init(knd, std::move(p));
    }

    ToddCoxeterImpl& ToddCoxeterImpl::init(congruence_kind           knd,
                                           Presentation<word_type>&& p) {
      p.validate();
      presentation::throw_if_not_normalized(p);
      init();
      detail::CongruenceCommon::init(knd);
      _word_graph.init(std::move(p));
      copy_settings_into_graph();
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      return *this;
    }

    ToddCoxeterImpl::ToddCoxeterImpl(congruence_kind                knd,
                                     Presentation<word_type> const& p)
        // call the rval ref constructor
        : ToddCoxeterImpl(knd, Presentation<word_type>(p)) {
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
    }

    ToddCoxeterImpl& ToddCoxeterImpl::init(congruence_kind                knd,
                                           Presentation<word_type> const& p) {
      // call the rval ref init
      return init(knd, Presentation<word_type>(p));
    }

    ToddCoxeterImpl::ToddCoxeterImpl(congruence_kind        knd,
                                     ToddCoxeterImpl const& tc)
        : ToddCoxeterImpl() {
      init(knd, tc);
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
    }

    ToddCoxeterImpl& ToddCoxeterImpl::init(congruence_kind        knd,
                                           ToddCoxeterImpl const& tc) {
      if (tc.kind() != congruence_kind::twosided && knd != tc.kind()) {
        LIBSEMIGROUPS_EXCEPTION(
            "incompatible types of congruence, found ({} / {}) but only "
            "(right / right) and (two-sided / *) are valid",
            tc.kind(),
            knd);
      }
      init();
      detail::CongruenceCommon::init(knd);
      _word_graph.init(tc.internal_presentation());
      copy_settings_into_graph();
      auto& rules = _word_graph.presentation().rules;
      rules.insert(rules.end(),
                   tc.internal_generating_pairs().cbegin(),
                   tc.internal_generating_pairs().cend());
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
      // TODO(1) don't we need to reset the setting_stack here too?
      return *this;
    }

    ToddCoxeterImpl::~ToddCoxeterImpl() = default;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - Interface requirements
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] uint64_t ToddCoxeterImpl::number_of_classes() {
      if (is_obviously_infinite(*this)) {
        return POSITIVE_INFINITY;
      }
      run();
      size_t const offset
          = (internal_presentation().contains_empty_word() ? 0 : 1);
      return current_word_graph().number_of_nodes_active() - offset;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - settings - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeterImpl& ToddCoxeterImpl::def_max(size_t val) noexcept {
      tc_settings().def_max = val;
      return *this;
    }

    size_t ToddCoxeterImpl::def_max() const noexcept {
      return tc_settings().def_max;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::def_policy(options::def_policy val) {
      tc_settings().def_policy = val;
      return *this;
    }

    ToddCoxeterImpl::options::def_policy
    ToddCoxeterImpl::def_policy() const noexcept {
      return tc_settings().def_policy;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::lookahead_next(size_t n) noexcept {
      tc_settings().lookahead_next = n;
      return *this;
    }

    size_t ToddCoxeterImpl::lookahead_next() const noexcept {
      return tc_settings().lookahead_next;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::lookahead_min(size_t n) noexcept {
      tc_settings().lookahead_min = n;
      return *this;
    }

    size_t ToddCoxeterImpl::lookahead_min() const noexcept {
      return tc_settings().lookahead_min;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::lookahead_growth_factor(float val) {
      if (val < 1.0) {
        LIBSEMIGROUPS_EXCEPTION("Expected a value >= 1.0, found {}", val);
      }
      tc_settings().lookahead_growth_factor = val;
      return *this;
    }

    float ToddCoxeterImpl::lookahead_growth_factor() const noexcept {
      return tc_settings().lookahead_growth_factor;
    }

    ToddCoxeterImpl&
    ToddCoxeterImpl::lookahead_growth_threshold(size_t val) noexcept {
      tc_settings().lookahead_growth_threshold = val;
      return *this;
    }

    size_t ToddCoxeterImpl::lookahead_growth_threshold() const noexcept {
      return tc_settings().lookahead_growth_threshold;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::lookahead_stop_early_ratio(float val) {
      if (val < 0.0 || val >= 1.0) {
        LIBSEMIGROUPS_EXCEPTION(
            "Expected a float in the interval [0, 1), found {}", val);
      }
      tc_settings().lookahead_stop_early_ratio = val;
      return *this;
    }

    float ToddCoxeterImpl::lookahead_stop_early_ratio() const noexcept {
      return tc_settings().lookahead_stop_early_ratio;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::lookahead_stop_early_interval(
        std::chrono::nanoseconds val) noexcept {
      tc_settings().lookahead_stop_early_interval = val;
      return *this;
    }

    std::chrono::nanoseconds
    ToddCoxeterImpl::lookahead_stop_early_interval() const noexcept {
      return tc_settings().lookahead_stop_early_interval;
    }

    ToddCoxeterImpl&
    ToddCoxeterImpl::lookahead_style(options::lookahead_style val) noexcept {
      tc_settings().lookahead_style = val;
      return *this;
    }

    ToddCoxeterImpl&
    ToddCoxeterImpl::lookahead_extent(options::lookahead_extent val) noexcept {
      tc_settings().lookahead_extent = val;
      return *this;
    }

    ToddCoxeterImpl::options::lookahead_style
    ToddCoxeterImpl::lookahead_style() const noexcept {
      return tc_settings().lookahead_style;
    }

    ToddCoxeterImpl::options::lookahead_extent
    ToddCoxeterImpl::lookahead_extent() const noexcept {
      return tc_settings().lookahead_extent;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::save(bool x) noexcept {
      tc_settings().save = x;
      return *this;
    }

    bool ToddCoxeterImpl::save() const noexcept {
      return tc_settings().save;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::strategy(options::strategy x) noexcept {
      tc_settings().strategy = x;
      return *this;
    }

    ToddCoxeterImpl::options::strategy
    ToddCoxeterImpl::strategy() const noexcept {
      return tc_settings().strategy;
    }

    ToddCoxeterImpl&
    ToddCoxeterImpl::use_relations_in_extra(bool val) noexcept {
      tc_settings().use_relations_in_extra = val;
      return *this;
    }

    bool ToddCoxeterImpl::use_relations_in_extra() const noexcept {
      return tc_settings().use_relations_in_extra;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::lower_bound(size_t n) noexcept {
      tc_settings().lower_bound = n;
      return *this;
    }

    size_t ToddCoxeterImpl::lower_bound() const noexcept {
      return tc_settings().lower_bound;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::large_collapse(size_t n) noexcept {
      _word_graph.large_collapse(n);
      return *this;
    }

    size_t ToddCoxeterImpl::large_collapse() const noexcept {
      return _word_graph.large_collapse();
    }

    ToddCoxeterImpl& ToddCoxeterImpl::hlt_defs(size_t val) {
      auto l = presentation::length(internal_presentation());
      if (val == 0) {
        LIBSEMIGROUPS_EXCEPTION("Expected a value != 0!");
      } else if (val < l) {
        LIBSEMIGROUPS_EXCEPTION("Expected a value >= {}, found {}!", l, val);
      }
      tc_settings().hlt_defs = val;
      return *this;
    }

    size_t ToddCoxeterImpl::hlt_defs() const noexcept {
      return tc_settings().hlt_defs;
    }

    ToddCoxeterImpl& ToddCoxeterImpl::f_defs(size_t val) {
      if (val == 0) {
        LIBSEMIGROUPS_EXCEPTION("Expected a value != 0!");
      }
      tc_settings().f_defs = val;
      return *this;
    }

    size_t ToddCoxeterImpl::f_defs() const noexcept {
      return tc_settings().f_defs;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - accessors - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeterImpl::word_graph_type const& ToddCoxeterImpl::word_graph() {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      shrink_to_fit();
      return current_word_graph();
    }

    Forest const& ToddCoxeterImpl::spanning_tree() {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      shrink_to_fit();
      return current_spanning_tree();
    }

    bool ToddCoxeterImpl::is_standardized(Order val) const {
      // TODO(1) this is probably not always valid,
      // the easiest fix for this would just be to call standardize(val) and
      // check if the return value is false, but this wouldn't be const, so
      // maybe not.
      return val == _standardized
             && _forest.number_of_nodes()
                    == current_word_graph().number_of_nodes_active();
    }

    bool ToddCoxeterImpl::is_standardized() const {
      // TODO(1) this is probably not always valid, i.e. if we are standardized,
      // then grow, then collapse, but end up with the same number of nodes
      // again.
      return _standardized != Order::none
             && _forest.number_of_nodes()
                    == current_word_graph().number_of_nodes_active();
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::shrink_to_fit() {
      if (!finished()) {
        return;
      }
      standardize(Order::shortlex);
      _word_graph.erase_free_nodes();
      _word_graph.induced_subgraph_no_checks(
          0, _word_graph.number_of_nodes_active());
    }

    bool ToddCoxeterImpl::standardize(Order val) {
      if (is_standardized(val)) {
        return false;
      }
      bool result   = word_graph::standardize(_word_graph, _forest, val);
      _standardized = val;
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::really_run_impl() {
      if (strategy() == options::strategy::felsch) {
        felsch();
      } else if (strategy() == options::strategy::hlt) {
        hlt();
      } else if (strategy() == options::strategy::CR) {
        CR_style();
      } else if (strategy() == options::strategy::R_over_C) {
        R_over_C_style();
      } else if (strategy() == options::strategy::Cr) {
        Cr_style();
      } else if (strategy() == options::strategy::Rc) {
        Rc_style();
      }
    }

    // TODO(1) add !running_until to check below?
    void ToddCoxeterImpl::run_impl() {
      if (!running_for() && is_obviously_infinite(*this)) {
        LIBSEMIGROUPS_EXCEPTION(
            "there are infinitely many classes in the congruence and "
            "Todd-Coxeter will never terminate");
      } else if (strategy() != options::strategy::felsch
                 && strategy() != options::strategy::hlt && running_until()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the strategy {} cannot be used with run_until !", strategy());
      }

      init_run();

      if (reporting_enabled()
          && (!running_for()
              || report_every() >= std::chrono::milliseconds(1'500))) {
        // TODO(1) report_strategy
        // TODO(1) if using ACE style strategy include the value of the relevant
        // setting
        auto msg = fmt::format("{:+<90}\n", "");
        msg += fmt_default("ToddCoxeter: Using {} strategy . . .\n",
                           strategy());
        msg += fmt::format("{:+<90}\n", "");
        report_no_prefix(msg);

        detail::Ticker t(
            [this]() { current_word_graph().report_progress_from_thread(); });
        really_run_impl();
      } else {
        really_run_impl();
      }

      finalise_run();
    }

    ////////////////////////////////////////////////////////////////////////
    // 14. ToddCoxeterImpl - member functions - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::copy_settings_into_graph() {
      // This is where we pass through from settings to the
      // _word_graph.definitions
      _word_graph.definitions().init(this);
      _word_graph.report_prefix("ToddCoxeter");
    }
    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - main strategies - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::init_run() {
      _word_graph.settings(*this);
      _word_graph.reset_start_time();
      _word_graph.stats_check_point();
      auto       first = internal_generating_pairs().cbegin();
      auto       last  = internal_generating_pairs().cend();
      auto const id    = current_word_graph().initial_node();
      if (save() || strategy() == options::strategy::felsch) {
        for (auto it = first; it < last; it += 2) {
          _word_graph.push_definition_hlt<detail::RegisterDefs>(
              id, *it, *(it + 1));
          _word_graph.process_coincidences<detail::RegisterDefs>();
        }
      } else {
        for (auto it = first; it < last; it += 2) {
          _word_graph.push_definition_hlt<detail::DoNotRegisterDefs>(
              id, *it, *(it + 1));
          _word_graph.process_coincidences<detail::DoNotRegisterDefs>();
        }
      }
      if (strategy() == options::strategy::felsch && use_relations_in_extra()) {
        first = internal_presentation().rules.cbegin();
        last  = internal_presentation().rules.cend();

        for (auto it = first; it < last; it += 2) {
          _word_graph.push_definition_hlt<detail::RegisterDefs>(
              id, *it, *(it + 1));
          _word_graph.process_coincidences<detail::RegisterDefs>();
        }
      }

      if (kind() == congruence_kind::twosided
          && !internal_generating_pairs().empty()) {
        // TODO(1) avoid copy of presentation here, if possible
        Presentation<word_type> p = internal_presentation();
        if (p.alphabet().size() != _word_graph.out_degree()) {
          LIBSEMIGROUPS_ASSERT(p.alphabet().size() == 0);
          p.alphabet(_word_graph.out_degree());
        }
        presentation::add_rules(p,
                                internal_generating_pairs().cbegin(),
                                internal_generating_pairs().cend());
        _word_graph.presentation(std::move(p));
      }

      if (save() || strategy() == options::strategy::felsch) {
        _word_graph.process_definitions();
      }
    }

    void ToddCoxeterImpl::finalise_run() {
      if (!stopped()) {
        if (_word_graph.definitions().any_skipped()) {
          auto const& d = current_word_graph();
          if (d.number_of_nodes_active() != lower_bound()
              || !word_graph::is_complete(
                  d, d.cbegin_active_nodes(), d.cend_active_nodes())) {
            SettingsGuard guard(this);
            lookahead_extent(options::lookahead_extent::full);
            lookahead_style(options::lookahead_style::hlt);
            perform_lookahead(DoNotStopEarly);
          }
        }
        _word_graph.report_progress_from_thread();
        report_no_prefix("{:-<90}\n", "");
        if (!is_obviously_infinite(*this)) {
          // We require this clause because we might still be obviously infinite
          // and running_for a specific amount of time, in which case we are
          // never finished. This is an issue when the input presentation has no
          // rules for example.
          _finished = true;
        }
      }
    }

    void ToddCoxeterImpl::felsch() {
      _word_graph.process_definitions();

      auto& current  = _word_graph.cursor();
      current        = _word_graph.initial_node();
      size_t const n = _word_graph.out_degree();
      while (current != _word_graph.first_free_node() && !stopped()) {
        for (letter_type a = 0; a < n; ++a) {
          if (_word_graph.target_no_checks(current, a) == UNDEFINED) {
            _word_graph.target_no_checks<detail::RegisterDefs>(
                current, a, _word_graph.new_node());
            _word_graph.process_definitions();
          }
        }
        current = _word_graph.next_active_node(current);
      }
    }

    void ToddCoxeterImpl::hlt() {
      auto& current    = _word_graph.cursor();
      current          = _word_graph.initial_node();
      auto const first = internal_presentation().rules.cbegin();
      auto const last  = internal_presentation().rules.cend();
      while (current != _word_graph.first_free_node() && !stopped()) {
        if (!save()) {
          for (auto it = first; it < last; it += 2) {
            _word_graph.push_definition_hlt<detail::DoNotRegisterDefs>(
                current, *it, *(it + 1));
            _word_graph.process_coincidences<detail::DoNotRegisterDefs>();
          }
        } else {
          for (auto it = first; it < last; it += 2) {
            _word_graph.push_definition_hlt<detail::RegisterDefs>(
                current, *it, *(it + 1));
            _word_graph.process_definitions();
          }
        }
        if ((!save() || _word_graph.definitions().any_skipped())
            && (_word_graph.number_of_nodes_active() > lookahead_next())) {
          // If save() == true and no deductions were skipped, then we have
          // already run process_definitions, and so there's no point in doing a
          // lookahead.
          perform_lookahead(StopEarly);
        }
        current = _word_graph.next_active_node(current);
      }
    }

    void ToddCoxeterImpl::CR_style() {
      SettingsGuard guard(this);
      size_t        N = presentation::length(internal_presentation());
      while (!finished()) {
        strategy(options::strategy::felsch);
        auto M = _word_graph.number_of_nodes_active();
        run_until([this, &M]() -> bool {
          return current_word_graph().number_of_nodes_active() >= M + f_defs();
        });
        if (finished()) {
          break;
        }
        strategy(options::strategy::hlt);
        M = _word_graph.number_of_nodes_active();
        run_until([this, &M, &N]() -> bool {
          return current_word_graph().number_of_nodes_active()
                 >= M + (hlt_defs() / N);
        });
      }
      lookahead_extent(options::lookahead_extent::full);
      lookahead_style(options::lookahead_style::hlt);
      perform_lookahead(DoNotStopEarly);
    }

    void ToddCoxeterImpl::R_over_C_style() {
      SettingsGuard guard(this);

      strategy(options::strategy::hlt);
      run_until([this]() -> bool {
        return current_word_graph().number_of_nodes_active()
               >= lookahead_next();
      });
      lookahead_extent(options::lookahead_extent::full);
      perform_lookahead(StopEarly);
      CR_style();
    }

    void ToddCoxeterImpl::Cr_style() {
      SettingsGuard guard(this);

      strategy(options::strategy::felsch);
      auto M = current_word_graph().number_of_nodes_active();
      run_until([this, &M]() -> bool {
        return current_word_graph().number_of_nodes_active() >= M + f_defs();
      });
      strategy(options::strategy::hlt);
      M        = current_word_graph().number_of_nodes_active();
      size_t N = presentation::length(internal_presentation());
      run_until([this, &M, &N]() -> bool {
        return current_word_graph().number_of_nodes_active()
               >= (hlt_defs() / N) + M;
      });
      strategy(options::strategy::felsch);
      run();
      lookahead_extent(options::lookahead_extent::full);
      lookahead_style(options::lookahead_style::hlt);
      perform_lookahead(DoNotStopEarly);
    }

    void ToddCoxeterImpl::Rc_style() {
      SettingsGuard guard(this);

      strategy(options::strategy::hlt);
      auto M = current_word_graph().number_of_nodes_active();
      // The +1 is here to avoid division by 0, in the lambda below.
      size_t N = presentation::length(internal_presentation()) + 1;
      run_until([this, &M, &N]() -> bool {
        return current_word_graph().number_of_nodes_active()
               >= (hlt_defs() / N) + M;
      });
      strategy(options::strategy::felsch);
      M = current_word_graph().number_of_nodes_active();
      run_until([this, &M]() -> bool {
        return current_word_graph().number_of_nodes_active() >= f_defs() + M;
      });
      strategy(options::strategy::hlt);
      run();
      lookahead_extent(options::lookahead_extent::full);
      lookahead_style(options::lookahead_style::hlt);
      perform_lookahead(DoNotStopEarly);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - reporting - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::report_next_lookahead(size_t old_value) const {
      static const std::string pad(8, ' ');
      int64_t diff = static_cast<int64_t>(lookahead_next()) - old_value;
      report_default("ToddCoxeter: next lookahead at {} | {:>12} (nodes)  "
                     "| {:>12} (diff)\n",
                     pad,
                     fmt::group_digits(lookahead_next()),
                     detail::signed_group_digits(diff));
      report_no_prefix("{:-<90}\n", "");
    }

    void ToddCoxeterImpl::report_nodes_killed(int64_t N) const {
      report_no_prefix("{:-<90}\n", "");
      report_default(
          "ToddCoxeter: lookahead complete with    | {:>12} (killed) |\n",
          detail::group_digits(-1 * N));
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - lookahead - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::perform_lookahead(bool stop_early) {
      report_no_prefix("{:-<90}\n", "");
      report_default("ToddCoxeter: performing {} {} lookahead . . .\n",
                     lookahead_extent(),
                     lookahead_style());
      report_no_prefix("{:-<90}\n", "");
      auto& current = _word_graph.lookahead_cursor();

      if (lookahead_extent() == options::lookahead_extent::partial) {
        // Start lookahead from the coset after _current
        current = _word_graph.next_active_node(_word_graph.cursor());
      } else {
        LIBSEMIGROUPS_ASSERT(lookahead_extent()
                             == options::lookahead_extent::full);
        current = _word_graph.initial_node();
      }
      size_t num_killed_by_me = 0;
      if (lookahead_style() == options::lookahead_style::hlt) {
        num_killed_by_me = hlt_lookahead(stop_early);
      } else {
        LIBSEMIGROUPS_ASSERT(lookahead_style()
                             == options::lookahead_style::felsch);
        num_killed_by_me = felsch_lookahead();
      }

      report_nodes_killed(num_killed_by_me);

      size_t const num_nodes = _word_graph.number_of_nodes_active();

      size_t const old_lookahead_next = lookahead_next();

      if (num_nodes < (lookahead_next() / lookahead_growth_factor())) {
        // If the lookahead_next is much bigger than the current number of
        // nodes, then reduce the next lookahead.
        if (lookahead_growth_factor() * num_nodes > lookahead_min()) {
          lookahead_next(lookahead_growth_factor() * num_nodes);
        } else if (lookahead_next() / lookahead_growth_factor()
                   > lookahead_min()) {
          lookahead_next(lookahead_next() / lookahead_growth_factor());
        } else {
          lookahead_next(lookahead_min());
        }
      } else if (num_nodes > lookahead_next()
                 || num_killed_by_me
                        < (num_nodes / lookahead_growth_threshold())) {
        // Otherwise, if we already exceed the lookahead_next or too few
        // nodes were killed, then increase the next lookahead.
        tc_settings().lookahead_next *= lookahead_growth_factor();
      }
      report_next_lookahead(old_lookahead_next);
    }

    size_t ToddCoxeterImpl::hlt_lookahead(bool stop_early) {
      size_t const old_number_of_killed = _word_graph.number_of_nodes_killed();
      _word_graph.make_compatible(_word_graph.lookahead_cursor(),
                                  internal_presentation().rules.cbegin(),
                                  internal_presentation().rules.cend(),
                                  stop_early,
                                  lookahead_stop_early_interval(),
                                  lookahead_stop_early_ratio());
      return _word_graph.number_of_nodes_killed() - old_number_of_killed;
    }

    size_t ToddCoxeterImpl::felsch_lookahead() {
      size_t const old_number_of_killed = _word_graph.number_of_nodes_killed();
      node_type&   current              = _word_graph.lookahead_cursor();
      size_t const n                    = _word_graph.out_degree();

      while (current != _word_graph.first_free_node()) {
        _word_graph.definitions().clear();
        for (size_t a = 0; a < n; ++a) {
          _word_graph.definitions().emplace_back(current, a);
        }
        _word_graph.process_definitions();
        current = _word_graph.next_active_node(current);
      }
      return _word_graph.number_of_nodes_killed() - old_number_of_killed;
    }

  }  // namespace detail
}  // namespace libsemigroups
