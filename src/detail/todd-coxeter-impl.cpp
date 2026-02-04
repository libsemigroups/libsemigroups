//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 James D. Mitchell
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

#include <chrono>       // for high_resolution_clock
#include <iterator>     // for TODO
#include <string_view>  // for basic_string_view
#include <tuple>        // for tie

#include "libsemigroups/constants.hpp"  // for operator==, operator!=
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/detail/string.hpp"
#include "libsemigroups/exception.hpp"     // for LIBSEMIGROUPS_EXCEP...
#include "libsemigroups/forest.hpp"        // for Forest
#include "libsemigroups/obvinf.hpp"        // for is_obviously_infinite
#include "libsemigroups/order.hpp"         // for Order
#include "libsemigroups/presentation.hpp"  // for Presentation, length
#include "libsemigroups/runner.hpp"        // for Runner::run_until
#include "libsemigroups/types.hpp"         // for word_type, letter_type
#include "libsemigroups/word-graph.hpp"    // for follow_path_no_...

#include "libsemigroups/detail/cong-common-class.hpp"  // for CongruenceCommon
#include "libsemigroups/detail/felsch-graph.hpp"       // for DoNotRegisterDefs
#include "libsemigroups/detail/fmt.hpp"       // for format_decimal, copy_str
#include "libsemigroups/detail/guard.hpp"     // for Guard
#include "libsemigroups/detail/iterator.hpp"  // for operator+
#include "libsemigroups/detail/node-manager.hpp"  // for NodeManager
#include "libsemigroups/detail/report.hpp"        // for report_no_prefix

namespace libsemigroups::detail {

  namespace {
    // Constants
    constexpr bool do_not_register_defs
        = detail::felsch_graph::do_not_register_defs;

    constexpr bool do_register_defs = detail::felsch_graph::do_register_defs;

    constexpr bool ye_print_divider = true;

    constexpr bool stop_early        = true;
    constexpr bool do_not_stop_early = false;

    // Aliases
    using node_type = typename ToddCoxeterImpl::node_type;

    using DoRegisterDefs = detail::felsch_graph::DoRegisterDefs<
        detail::NodeManagedGraph<detail::ToddCoxeterImpl::Graph::node_type>,
        detail::ToddCoxeterImpl::Definitions>;

    using DoNotRegisterDefs = Noop;
  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl::Settings
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeterImpl::Settings& ToddCoxeterImpl::tc_settings() {
    LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
    return *_settings_stack.back();
  }

  ToddCoxeterImpl::Settings const& ToddCoxeterImpl::tc_settings() const {
    LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
    return *_settings_stack.back();
  }

  void ToddCoxeterImpl::reset_settings_stack() {
    if (_settings_stack.empty()) {
      _settings_stack.push_back(std::make_unique<Settings>());
    } else {
      _settings_stack.erase(_settings_stack.begin() + 1, _settings_stack.end());
      _settings_stack.back()->init();
    }
    LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl::Graph --- Constructors and initializers
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeterImpl::Graph::Graph()
      : _forest(), _forest_valid(false), _standardization_order(Order::none) {}

  ToddCoxeterImpl::Graph& ToddCoxeterImpl::Graph::init() {
    NodeManager<node_type>::clear();
    FelschGraph_::init();
    // TODO(1) shouldn't add nodes here because then there'll be more than
    // there should be (i.e. NodeManager and FelschGraph_ will have
    // different numbers of nodes
    FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
    // Don't need to _forest.init() because this will be done at first call of
    // current_spanning_tree()
    _forest_valid          = false;
    _standardization_order = Order::none;
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
    _forest_valid          = false;
    _standardization_order = Order::none;
    return *this;
  }

  ToddCoxeterImpl::Graph&
  ToddCoxeterImpl::Graph::init(Presentation<word_type> const& p,
                               WordGraph<node_type> const&    wg) {
    // Don't need to _forest.init() because this will be done at first
    // call of current_spanning_tree()
    _forest_valid          = false;
    _standardization_order = Order::none;
    FelschGraph_::operator=(wg);
    FelschGraph_::presentation_no_checks(p);
    return *this;
  }

  ToddCoxeterImpl::Graph&
  ToddCoxeterImpl::Graph::operator=(WordGraph<node_type> const& wg) {
    // Don't need to _forest.init() because this will be done at first
    // call of current_spanning_tree()
    _forest_valid          = false;
    _standardization_order = Order::none;
    FelschGraph_::operator=(wg);
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // FelschGraph_ mem fns overrides
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeterImpl::Graph&
  ToddCoxeterImpl::Graph::target_no_checks(node_type  s,
                                           label_type a,
                                           node_type  t) noexcept {
    _forest_valid          = false;
    _standardization_order = Order::none;
    FelschGraph_::target_no_checks(s, a, t);
    return *this;
  }

  ToddCoxeterImpl::Graph&
  ToddCoxeterImpl::Graph::register_target_no_checks(node_type  s,
                                                    label_type a,
                                                    node_type  t) noexcept {
    _forest_valid          = false;
    _standardization_order = Order::none;
    FelschGraph_::register_target_no_checks(s, a, t);
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl::Graph --- Other modifiers
  ////////////////////////////////////////////////////////////////////////

  template <typename RuleIterator>
  void ToddCoxeterImpl::Graph::make_compatible(ToddCoxeterImpl* tc,
                                               node_type&       current,
                                               RuleIterator     first,
                                               RuleIterator     last,
                                               bool should_stop_early) {
    auto last_stop_early_check      = std::chrono::high_resolution_clock::now();
    auto const old_number_of_killed = number_of_nodes_killed();
    auto       killed_at_prev_interval = old_number_of_killed;

    bool   old_ticker_running = tc->_ticker_running;
    auto   start_time         = std::chrono::high_resolution_clock::now();
    Ticker ticker;

    CollectCoincidences                    incompat(_coinc);
    typename FelschGraph_::NoPreferredDefs prefdefs;

    while (current != NodeManager<node_type>::first_free_node()
           && (!should_stop_early || !tc->stopped())) {
      // If should_stop_early and tc->stopped(), then we exit this loop.
      // O/w we continue, this is because _finished is sometimes set before we
      // are really finished, which is something that should be fixed at some
      // point (see for example CR_style).

      // TODO(1) when we have an RuleIterator into the active nodes, we
      // should remove the while loop, and use that in make_compatible
      // instead. At present there is a cbegin/cend_active_nodes in
      // NodeManager but the RuleIterators returned by them are invalidated
      // by any changes to the graph, such as those made by
      // felsch_graph::make_compatible.
      felsch_graph::make_compatible<do_not_register_defs>(
          *this, current, current + 1, first, last, incompat, prefdefs);
      // Using NoPreferredDefs is just a (more or less) arbitrary
      // choice, could allow the other choices here too (which works,
      // but didn't seem to be very useful).
      process_coincidences(DoNotRegisterDefs{});

      current = NodeManager<node_type>::next_active_node(current);
      tc->_stats.lookahead_or_behind_position++;
      if (tc->lookahead_stop_early(should_stop_early,
                                   last_stop_early_check,
                                   killed_at_prev_interval)) {
        break;
      }
      if (!tc->_ticker_running && reporting_enabled()
          && delta(start_time) > std::chrono::milliseconds(500)) {
        tc->_ticker_running = true;
        ticker([&tc]() { tc->report_progress_from_thread(ye_print_divider); });
      }
    }
    tc->_stats.lookahead_or_behind_nodes_killed
        += (number_of_nodes_killed() - killed_at_prev_interval);
    tc->_ticker_running = old_ticker_running;
  }

  ToddCoxeterImpl::Graph& ToddCoxeterImpl::Graph::presentation_no_checks(
      Presentation<word_type> const& p) {
    FelschGraph_::presentation_no_checks(p);
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
      process_coincidences(DoRegisterDefs(this));
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
            possibly_register_target_no_checks<RegDefs>(xx, aa, d);
            if (aa != bb || xx != yy) {
              possibly_register_target_no_checks<RegDefs>(yy, bb, d);
            }
          };

    FelschGraph_::merge_targets_of_nodes_if_possible<RegDefs>(
        x, a, y, b, incompat, pref_defs);
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl::Graph --- Spanning tree
  ////////////////////////////////////////////////////////////////////////

  Forest& ToddCoxeterImpl::Graph::current_spanning_tree() {
    if (!_forest_valid) {
      _standardization_order = Order::none;
      _forest.init();
      v4::word_graph::spanning_tree_no_checks(*this, initial_node(), _forest);
      _forest_valid = true;
    }
    LIBSEMIGROUPS_ASSERT(_forest.number_of_nodes() == max_active_node() + 1);
    return _forest;
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl::Graph --- Standardization
  ////////////////////////////////////////////////////////////////////////

  bool ToddCoxeterImpl::Graph::standardize(Order val) {
    if (val == Order::none) {
      // This is technically a breaking change, but the previous behaviour was
      // a bug, because this is used as if every call to standardize(Order)
      // correctly sets _forest, which it does not.
      LIBSEMIGROUPS_EXCEPTION("the argument (Order) must not be Order::none");
    } else if (is_standardized(val)) {
      return false;
    }
    time_point start_time;
    if (reporting_enabled()) {
      start_time = std::chrono::high_resolution_clock::now();
      report_no_prefix(report_divider());
      report_default("ToddCoxeter: {} standardizing the word graph, this might "
                     "take a few moments!\n",
                     val);
    }
    _forest.init();
    _forest.add_nodes(number_of_nodes_active());
    // NOTE: the cursor() does not survive the next line
    // but lookahead_cursor() should
    bool result            = v4::word_graph::standardize(*this, _forest, val);
    _forest_valid          = true;
    _standardization_order = val;
    report_default("ToddCoxeter: the word graph was {} standardized in {}\n",
                   val,
                   string_time(delta(start_time)));
    return result;
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
      : CongruenceCommon(),
        _finished(),
        _never_run(),
        _settings_stack(),
        _state(),
        _stats(),
        _ticker_running(),
        _word_graph() {
    init();
  }

  ToddCoxeterImpl& ToddCoxeterImpl::init() {
    CongruenceCommon::init();
    report_divider(fmt::format("{:+<32}\n", ""));
    report_prefix("ToddCoxeter");
    _finished  = false;
    _never_run = true;
    reset_settings_stack();
    _state = state::none;
    _stats.init();
    _ticker_running = false;
    _word_graph.init();
    copy_settings_into_graph();
    return *this;
  }

  ToddCoxeterImpl::ToddCoxeterImpl(ToddCoxeterImpl const& that)
      : ToddCoxeterImpl() {
    operator=(that);
    LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
  }

  ToddCoxeterImpl::ToddCoxeterImpl(ToddCoxeterImpl&& that) : ToddCoxeterImpl() {
    operator=(std::move(that));
  }

  ToddCoxeterImpl& ToddCoxeterImpl::operator=(ToddCoxeterImpl&& that) {
    LIBSEMIGROUPS_ASSERT(!that._settings_stack.empty());
    CongruenceCommon::operator=(std::move(that));
    _finished       = std::move(that._finished);
    _never_run      = std::move(that._never_run);
    _settings_stack = std::move(that._settings_stack);
    _state          = that._state.load();
    _stats          = std::move(that._stats);
    _ticker_running = std::move(that._ticker_running);
    _word_graph     = std::move(that._word_graph);
    // The next line is essential so that the _word_graph.definitions()._tc
    // points at <this>.
    _word_graph.definitions().init(this);
    LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
    return *this;
  }

  ToddCoxeterImpl& ToddCoxeterImpl::operator=(ToddCoxeterImpl const& that) {
    LIBSEMIGROUPS_ASSERT(!that._settings_stack.empty());
    CongruenceCommon::operator=(that);
    _finished  = that._finished;
    _never_run = that._never_run;
    _settings_stack.clear();
    for (auto const& uptr : that._settings_stack) {
      _settings_stack.push_back(std::make_unique<Settings>(*uptr));
    }
    _state          = that._state.load();
    _stats          = that._stats;
    _ticker_running = that._ticker_running;
    _word_graph     = that._word_graph;
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
    p.throw_if_bad_alphabet_or_rules();
    presentation::throw_if_not_normalized(p);
    init();
    kind(knd);
    _word_graph.init(std::move(p));
    copy_settings_into_graph();
    LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
    return *this;
  }

  ToddCoxeterImpl&
  ToddCoxeterImpl::presentation_no_checks(Presentation<word_type> const& p) {
    _word_graph.presentation_no_checks(p);
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
          "(onesided / onesided) and (two-sided / *) are valid",
          tc.kind(),
          knd);
    }
    // TODO(1) what about the word graph, if it's partially defined at least
    // shouldn't we copy it also?

    // if this and &tc are the same object, then we insert the generating
    // pairs into the presentation before reinitialising.
    auto& rules = _word_graph.presentation().rules;
    if (this == &tc) {
      rules.insert(rules.end(),
                   tc.internal_generating_pairs().cbegin(),
                   tc.internal_generating_pairs().cend());
    }
    init(knd, tc.internal_presentation());
    if (this != &tc) {
      // This looks weird yes, but the if-clause above is triggered, then
      // above we are adding the internal_generating_pairs of tc into the
      // presentation of tc (because it's the same as *this), then
      // initialising using it. If this != &tc, then we must copy the
      // internal_generating_pairs into this after initialisation.
      rules.insert(rules.end(),
                   tc.internal_generating_pairs().cbegin(),
                   tc.internal_generating_pairs().cend());
    }
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

  ToddCoxeterImpl& ToddCoxeterImpl::use_relations_in_extra(bool val) noexcept {
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

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl - modifiers - public
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeterImpl::shrink_to_fit() {
    _word_graph.standardize(Order::shortlex);
    _word_graph.erase_free_nodes();
    _word_graph.induced_subgraph_no_checks(
        0, _word_graph.number_of_nodes_active());
  }

  ////////////////////////////////////////////////////////////////////////
  // Runner - pure virtual member functions - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeterImpl::really_run_impl() {
    stats_run_start();
    report_before_run();
    before_run();
    if (strategy() == options::strategy::felsch) {
      Guard guard(_state, state::felsch);
      felsch();
    } else if (strategy() == options::strategy::hlt) {
      Guard guard(_state, state::hlt);
      hlt();
    } else if (strategy() == options::strategy::lookahead) {
      Guard guard(_state, state::lookahead);
      perform_lookahead_impl(stop_early);
    } else if (strategy() == options::strategy::lookbehind) {
      Guard guard(_state, state::lookbehind);
      perform_lookbehind_impl();
    } else if (strategy() == options::strategy::CR) {
      CR_style();
    } else if (strategy() == options::strategy::R_over_C) {
      R_over_C_style();
    } else if (strategy() == options::strategy::Cr) {
      Cr_style();
    } else if (strategy() == options::strategy::Rc) {
      Rc_style();
    }
    after_run();
  }

  void ToddCoxeterImpl::run_impl() {
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    if (!running_for() && !running_until() && is_obviously_infinite(*this)) {
      LIBSEMIGROUPS_EXCEPTION(
          "there are infinitely many classes in the congruence and "
          "Todd-Coxeter will never terminate");
    } else if (strategy() != options::strategy::felsch
               && strategy() != options::strategy::hlt
               && strategy() != options::strategy::lookahead
               && strategy() != options::strategy::lookbehind
               && running_until()) {
      LIBSEMIGROUPS_EXCEPTION("the strategy {} cannot be used with run_until !",
                              strategy());
    } else if (internal_presentation().rules.empty()
               && !internal_presentation().alphabet().empty()
               && (internal_generating_pairs().empty()
                   || kind() == congruence_kind::onesided)
               && strategy() == options::strategy::hlt
               && current_word_graph().number_of_nodes_active() == 1) {
      LIBSEMIGROUPS_EXCEPTION(
          "the HLT strategy cannot be used with a presentation with > 0 "
          "generators and 0 rules unless the word graph was defined at "
          "construction or re-initialisation");
    }

    // TODO the pattern below is so common we should incorporate it into
    // Runner or somewhere

    if (!_ticker_running && reporting_enabled()
        && (!running_for()
            || duration_cast<seconds>(running_for_how_long()) >= seconds(1))) {
      _ticker_running = true;
      Ticker t([this]() { report_progress_from_thread(ye_print_divider); });
      really_run_impl();
      _ticker_running = false;
    } else {
      really_run_impl();
    }
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

  [[nodiscard]] bool ToddCoxeterImpl::any_change() const {
    return _stats.run_nodes_active_at_start
           != current_word_graph().number_of_nodes_active();
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl - main strategies - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeterImpl::before_run() {
    _word_graph.settings(*this);
    if (!_never_run) {
      return;
    }
    _never_run       = false;
    auto       first = internal_generating_pairs().cbegin();
    auto       last  = internal_generating_pairs().cend();
    auto const id    = current_word_graph().initial_node();

    if (save() || strategy() == options::strategy::felsch) {
      for (auto it = first; it < last; it += 2) {
        _word_graph.push_definition_hlt<do_register_defs>(id, *it, *(it + 1));
        _word_graph.process_coincidences(DoRegisterDefs{_word_graph});
      }
    } else {
      for (auto it = first; it < last; it += 2) {
        _word_graph.push_definition_hlt<do_not_register_defs>(
            id, *it, *(it + 1));
        _word_graph.process_coincidences(DoNotRegisterDefs{});
      }
    }

    if (use_relations_in_extra() && strategy() == options::strategy::felsch) {
      first = internal_presentation().rules.cbegin();
      last  = internal_presentation().rules.cend();

      for (auto it = first; it < last; it += 2) {
        _word_graph.push_definition_hlt<do_register_defs>(id, *it, *(it + 1));
        _word_graph.process_coincidences(DoNotRegisterDefs{});
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
      _word_graph.presentation_no_checks(std::move(p));
    }

    if (save() || strategy() == options::strategy::felsch) {
      _word_graph.process_definitions();
    }
  }

  void ToddCoxeterImpl::after_run() {
    if (strategy() != options::strategy::lookahead
        && strategy() != options::strategy::lookbehind && !stopped()) {
      if (_word_graph.definitions().any_skipped()) {
        auto const& d = current_word_graph();
        if (d.number_of_nodes_active() != lower_bound()
            || !v4::word_graph::is_complete(
                d, d.cbegin_active_nodes(), d.cend_active_nodes())) {
          SettingsGuard guard(this);
          lookahead_extent(options::lookahead_extent::full);
          lookahead_style(options::lookahead_style::hlt);
          perform_lookahead_impl(do_not_stop_early);
        }
      }
      if (any_change()) {
        report_progress_from_thread(ye_print_divider);
      }
      if (!is_obviously_infinite(*this)) {
        // We require this clause because we might still be obviously
        // infinite and running_for a specific amount of time, in which case
        // we are never finished. This is an issue when the input
        // presentation has no rules for example.
        _finished = true;
      }
    }
    report_after_run();
    stats_run_stop();
  }

  void ToddCoxeterImpl::felsch() {
    stats_phase_start();
    report_before_phase();

    _word_graph.process_definitions();

    auto& current  = _word_graph.cursor();
    current        = _word_graph.initial_node();
    size_t const n = _word_graph.out_degree();
    while (current != _word_graph.first_free_node() && !stopped()) {
      for (letter_type a = 0; a < n; ++a) {
        if (_word_graph.target_no_checks(current, a) == UNDEFINED) {
          _word_graph.register_target_no_checks(
              current, a, _word_graph.new_node());
          _word_graph.process_definitions();
        }
      }
      current = _word_graph.next_active_node(current);
    }
    report_after_phase();
    stats_phase_stop();
  }

  void ToddCoxeterImpl::hlt() {
    stats_phase_start();
    report_before_phase();
    auto& current    = _word_graph.cursor();
    current          = _word_graph.initial_node();
    auto const first = internal_presentation().rules.cbegin();
    auto const last  = internal_presentation().rules.cend();
    while (current != _word_graph.first_free_node() && !stopped()) {
      if (!save()) {
        for (auto it = first; it < last; it += 2) {
          _word_graph.push_definition_hlt<do_not_register_defs>(
              current, *it, *(it + 1));
          _word_graph.process_coincidences(DoNotRegisterDefs{});
        }
      } else {
        for (auto it = first; it < last; it += 2) {
          _word_graph.push_definition_hlt<do_register_defs>(
              current, *it, *(it + 1));
          _word_graph.process_definitions();
        }
      }
      if ((!save() || _word_graph.definitions().any_skipped())
          && (_word_graph.number_of_nodes_active() > lookahead_next())) {
        // If save() == true and no deductions were skipped, then we have
        // already run process_definitions, and so there's no point in doing
        // a lookahead.
        report_after_phase();
        stats_phase_stop();
        perform_lookahead_impl(stop_early);
        stats_phase_start();
        report_before_phase();
      }
      current = _word_graph.next_active_node(current);
    }
    report_after_phase();
    stats_phase_stop();
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
    perform_lookahead_impl(do_not_stop_early);
  }

  void ToddCoxeterImpl::R_over_C_style() {
    SettingsGuard guard(this);

    strategy(options::strategy::hlt);
    run_until([this]() -> bool {
      return current_word_graph().number_of_nodes_active() >= lookahead_next();
    });
    lookahead_extent(options::lookahead_extent::full);
    perform_lookahead_impl(stop_early);
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
    perform_lookahead_impl(do_not_stop_early);
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
    perform_lookahead_impl(do_not_stop_early);
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeterImpl - lookahead - private
  ////////////////////////////////////////////////////////////////////////

  // There are two ways to run a lookahead:
  //
  // 1. from within HLT or Felsch
  // 2. via the mem fns perform_lookahead(_for/_until)
  //
  // If 1 holds, then strategy() will be options::strategy::hlt or
  // options::strategy::felsch. If 2 holds, then strategy() will be
  // options::strategy::lookahead. If 1 holds, then we do all of the wrangling
  // of the settings. If 2 holds, we do none of the wrangling of the settings,
  // since the stopping condition should be set from outside.
  ToddCoxeterImpl&
  ToddCoxeterImpl::perform_lookahead_impl(bool should_stop_early) {
    if (_word_graph.number_of_nodes_active() == 1) {
      // Can't collapse anything in this case
      return *this;
    }
    Guard guard(_state, state::lookahead);

    stats_phase_start();
    report_before_lookahead();

    auto& current = _word_graph.lookahead_cursor();
    if (lookahead_extent() == options::lookahead_extent::partial) {
      // Start lookahead from the node after _current
      current = _word_graph.next_active_node(_word_graph.cursor());
      _stats.lookahead_or_behind_position
          = _word_graph.position_of_node(current);
    } else {
      LIBSEMIGROUPS_ASSERT(lookahead_extent()
                           == options::lookahead_extent::full);
      current                             = _word_graph.initial_node();
      _stats.lookahead_or_behind_position = 0;
    }

    _stats.lookahead_or_behind_nodes_killed = 0;

    if (lookahead_style() == options::lookahead_style::hlt) {
      hlt_lookahead(should_stop_early);
    } else {
      LIBSEMIGROUPS_ASSERT(lookahead_style()
                           == options::lookahead_style::felsch);
      felsch_lookahead(should_stop_early);
    }

    report_after_phase();
    if (strategy() != options::strategy::lookahead) {
      auto const old_lookahead_next = lookahead_update_settings();
      report_lookahead_settings(old_lookahead_next);
    }
    stats_phase_stop();
    return *this;
  }

  size_t ToddCoxeterImpl::lookahead_update_settings() {
    size_t const num_nodes          = _word_graph.number_of_nodes_active();
    size_t const old_lookahead_next = lookahead_next();

    // NOTE: that lookahead_next() is ~= num_nodes + num_killed_by_me
    // unless triggered by skipped definitions.

    // The aim is that lookahead_next() should at most num_nodes *
    // growth_factor, or if num_killed_by_me is too small lookahead_next()
    // be should increased.
    if (num_nodes * lookahead_growth_factor() < lookahead_next()
        || num_nodes > lookahead_next()) {
      // In the first case,
      // num_killed_by_me ~= lookahead_next() - num_nodes
      //                   > num_nodes * lookahead_growth_factor() -
      //                   num_nodes = num_nodes * (lookahead_growth_factor
      //                   - 1).
      // I.e. num_killed_by_me is relatively big, and so we decrease
      // lookahead_next().

      // In the second case, if we don't change lookahead_next(), then we'd
      // immediately perform the same lookahead again without making any
      // further definitions, so we increase lookahead_next().
      lookahead_next(
          std::max(lookahead_min(),
                   static_cast<size_t>(lookahead_growth_factor() * num_nodes)));
    } else if (_stats.lookahead_or_behind_nodes_killed
               < ((num_nodes + _stats.lookahead_or_behind_nodes_killed)
                  / lookahead_growth_threshold())) {
      // In this case,
      // num_killed_by_me ~= lookahead_next() - num_nodes
      //                   < (num_nodes + num_killed_by_me) / lgt
      // => num_killed_by_me * lgt < num_nodes + num_killed_by_me
      // => num_killed_by_me (lgt - 1) < num_nodes
      // => num_killed_by_me < num_nodes / (lgt - 1)
      // i.e. num_killed_by_me is relatively small and so we increase
      // lookahead_next().
      lookahead_next(lookahead_next() * lookahead_growth_factor());
    }
    return old_lookahead_next;
  }

  ToddCoxeterImpl& ToddCoxeterImpl::perform_lookahead() {
    SettingsGuard sg(this);
    strategy(options::strategy::lookahead);
    run();
    return *this;
  }

  ToddCoxeterImpl&
  ToddCoxeterImpl::perform_lookahead_for(std::chrono::nanoseconds t) {
    SettingsGuard sg(this);
    strategy(options::strategy::lookahead);
    run_for(t);
    return *this;
  }

  ToddCoxeterImpl&
  ToddCoxeterImpl::perform_lookahead_until(std::function<bool()>&& pred) {
    SettingsGuard sg(this);
    strategy(options::strategy::lookahead);
    run_until(std::move(pred));
    return *this;
  }

  bool ToddCoxeterImpl::lookahead_stop_early(
      bool                                            should_stop_early,
      std::chrono::high_resolution_clock::time_point& last_stop_early_check,
      uint64_t&                                       killed_at_prev_interval) {
    // We don't use "stop_early" when running lookahead/behind from the
    // outside, which is indicated by the strategy.
    if (strategy() != options::strategy::lookahead
        && strategy() != options::strategy::lookbehind && should_stop_early
        && delta(last_stop_early_check) > lookahead_stop_early_interval()) {
      size_t killed_last_interval
          = current_word_graph().number_of_nodes_killed()
            - killed_at_prev_interval;
      killed_at_prev_interval = current_word_graph().number_of_nodes_killed();
      _stats.lookahead_or_behind_nodes_killed += killed_last_interval;

      auto expected         = static_cast<size_t>(number_of_nodes_active()
                                          * lookahead_stop_early_ratio());
      last_stop_early_check = std::chrono::high_resolution_clock::now();
      if (killed_last_interval < expected) {
        report_lookahead_stop_early(expected, killed_last_interval);
        return true;
      }
    }
    return false;
  }

  void ToddCoxeterImpl::hlt_lookahead(bool should_stop_early) {
    _word_graph.make_compatible(this,
                                _word_graph.lookahead_cursor(),
                                internal_presentation().rules.cbegin(),
                                internal_presentation().rules.cend(),
                                should_stop_early);
  }

  void ToddCoxeterImpl::felsch_lookahead(bool should_stop_early) {
    auto last_stop_early_check      = std::chrono::high_resolution_clock::now();
    auto const old_number_of_killed = _word_graph.number_of_nodes_killed();
    auto       killed_at_prev_interval = old_number_of_killed;

    bool       old_ticker_running = _ticker_running;
    time_point start_time         = std::chrono::high_resolution_clock::now();
    Ticker     ticker;

    node_type&   current = _word_graph.lookahead_cursor();
    size_t const n       = _word_graph.out_degree();

    while (current != _word_graph.first_free_node()
           && (!should_stop_early || !stopped())) {
      // See the comment in make_compatible about why we have
      // !should_stop_early here. This might never be used but is here for
      // consistency.
      _word_graph.definitions().clear();
      for (size_t a = 0; a < n; ++a) {
        _word_graph.definitions().emplace_back(current, a);
      }
      _word_graph.process_definitions();
      current = _word_graph.next_active_node(current);
      _stats.lookahead_or_behind_position++;
      if (lookahead_stop_early(should_stop_early,
                               last_stop_early_check,
                               killed_at_prev_interval)) {
        break;
      }
      if (!_ticker_running && reporting_enabled()
          && delta(start_time) >= std::chrono::milliseconds(500)) {
        _ticker_running = true;
        ticker([this]() { report_progress_from_thread(ye_print_divider); });
      }
    }
    _stats.lookahead_or_behind_nodes_killed
        += (_word_graph.number_of_nodes_killed() - killed_at_prev_interval);
    _ticker_running = old_ticker_running;
  }

  ////////////////////////////////////////////////////////////////////////
  // Lookbehind
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeterImpl& ToddCoxeterImpl::perform_lookbehind_impl() {
    if (kind() == congruence_kind::onesided
        && !internal_generating_pairs().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected a 2-sided ToddCoxeter instance, or a 1-sided ToddCoxeter "
          "instance with 0 generating pairs, found {} generating pairs",
          internal_generating_pairs().size());
    } else if (_word_graph.number_of_nodes_active() == 1) {
      // Can't collapse anything in this case
      return *this;
    }

    // TODO Rename Guard to ValueGuard
    Guard guard(_state, state::lookbehind);

    stats_phase_start();
    report_before_phase();
    auto const killed_before_lookbehind = _word_graph.number_of_nodes_killed();

    bool const       old_ticker_running = _ticker_running;
    time_point const start_time = std::chrono::high_resolution_clock::now();
    Ticker           ticker;

    node_type& current = _word_graph.lookahead_cursor();
    current            = _word_graph.initial_node();

    _stats.lookahead_or_behind_position     = 0;
    _stats.lookahead_or_behind_nodes_killed = 0;

    word_type w1, w2;

    while (current != _word_graph.first_free_node() && !stopped()) {
      w1.clear();
      w2.clear();
      // Repeatedly call current_spanning_tree() to ensure it is up-to-date
      // after any calls to process_coincidences below.
      current_spanning_tree().path_from_root_no_checks(std::back_inserter(w1),
                                                       current);
      _lookbehind_collapser(std::back_inserter(w2), w1.begin(), w1.end());
      if (!std::equal(w1.begin(), w1.end(), w2.begin(), w2.end())) {
        node_type other = v4::word_graph::follow_path_no_checks(
            _word_graph, _word_graph.initial_node(), w2.begin(), w2.end());
        if (other != UNDEFINED && other != current) {
          _word_graph.merge_nodes_no_checks(current, other);
          if (_word_graph.number_of_coincidences() > 32'768) {
            // TODO(0) make the number 32'768 above a setting
            _word_graph.process_coincidences();
          }
        }
      }
      current = _word_graph.next_active_node(current);
      _stats.lookahead_or_behind_position++;
      _stats.lookahead_or_behind_nodes_killed
          = current_word_graph().number_of_nodes_killed()
            - killed_before_lookbehind;
      if (!_ticker_running && reporting_enabled()
          && delta(start_time) >= std::chrono::milliseconds(500)) {
        _ticker_running = true;
        ticker([this]() { report_progress_from_thread(true); });
      }
    }
    _word_graph.process_coincidences();
    report_after_phase();
    stats_phase_stop();
    _stats.lookahead_or_behind_nodes_killed
        = current_word_graph().number_of_nodes_killed()
          - killed_before_lookbehind;
    _ticker_running = old_ticker_running;
    return *this;
  }

  ToddCoxeterImpl& ToddCoxeterImpl::perform_lookbehind() {
    return perform_lookbehind_no_checks(
        [this](auto d_first, auto first, auto last) {
          reduce_no_run_no_checks(d_first, first, last);
        });
  }

  ToddCoxeterImpl&
  ToddCoxeterImpl::perform_lookbehind_for(std::chrono::nanoseconds t) {
    return perform_lookbehind_for_no_checks(
        t, [this](auto d_first, auto first, auto last) {
          reduce_no_run_no_checks(d_first, first, last);
        });
  }

  ToddCoxeterImpl&
  ToddCoxeterImpl::perform_lookbehind_until(std::function<bool()>&& pred) {
    return perform_lookbehind_until_no_checks(
        std::move(pred), [this](auto d_first, auto first, auto last) {
          reduce_no_run_no_checks(d_first, first, last);
        });
  }

}  // namespace libsemigroups::detail
