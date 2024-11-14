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

#include "libsemigroups/todd-coxeter.hpp"

#include "libsemigroups/debug.hpp"
#include "libsemigroups/obvinf.hpp"

#include "libsemigroups/detail/report.hpp"

// TODO(0) rename _setting_stack -> _settings_stack

namespace libsemigroups {

  using node_type = typename ToddCoxeter::node_type;

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter::Settings
  ////////////////////////////////////////////////////////////////////////

  struct ToddCoxeter::Settings {
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
      def_max                       = 2'000;
      def_policy                    = options::def_policy::no_stack_if_no_space;
      hlt_defs                      = 200'000;
      f_defs                        = 100'000;
      lookahead_extent              = options::lookahead_extent::partial;
      lookahead_growth_factor       = 2.0;
      lookahead_growth_threshold    = 4;
      lower_bound                   = UNDEFINED;
      lookahead_min                 = 10'000;
      lookahead_next                = 5'000'000;
      lookahead_stop_early_interval = std::chrono::seconds(1);
      lookahead_stop_early_ratio    = 0.01;
      lookahead_style               = options::lookahead_style::hlt;

      save                   = false;
      strategy               = options::strategy::hlt;
      use_relations_in_extra = false;
      return *this;
    }
  };  // class ToddCoxeter::Settings

  ToddCoxeter::Settings& ToddCoxeter::tc_settings() {
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
    return *_setting_stack.back();
  }

  ToddCoxeter::Settings const& ToddCoxeter::tc_settings() const {
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
    return *_setting_stack.back();
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter::SettingsGuard
  ////////////////////////////////////////////////////////////////////////

  class ToddCoxeter::SettingsGuard {
    ToddCoxeter* _tc;

   public:
    SettingsGuard(ToddCoxeter* tc) : _tc(tc) {
      _tc->_setting_stack.push_back(std::make_unique<Settings>());
    }
    ~SettingsGuard() {
      _tc->_setting_stack.pop_back();
      LIBSEMIGROUPS_ASSERT(!_tc->_setting_stack.empty());
    }
  };  // class ToddCoxeter::SettingsGuard

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter::Graph
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeter::Graph&
  ToddCoxeter::Graph::init(Presentation<word_type> const& p) {
    NodeManager<node_type>::clear();
    FelschGraph_::init(p);
    // FIXME(0) shouldn't add nodes here because then there'll be more than
    // there should be (i.e. NodeManager and FelschGraph_ will have
    // different numbers of nodes
    FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
    return *this;
  }

  ToddCoxeter::Graph& ToddCoxeter::Graph::init(Presentation<word_type>&& p) {
    NodeManager<node_type>::clear();
    FelschGraph_::init(std::move(p));
    // FIXME(0) shouldn't add nodes here because then there'll be more than
    // there should be (i.e. NodeManager and FelschGraph_ will have
    // different numbers of nodes
    FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
    return *this;
  }

  void ToddCoxeter::Graph::process_definitions() {
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
  void ToddCoxeter::Graph::push_definition_hlt(node_type const& c,
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
  size_t ToddCoxeter::Graph::make_compatible(
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
              stop_early_interval,
              static_cast<size_t>(number_of_nodes_active() * stop_early_ratio),
              killed_last_interval);
          report_no_prefix("{:-<90}\n", "");
          break;
        }
      }
    }
    return NodeManager<node_type>::number_of_nodes_killed()
           - old_number_of_killed;
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter::Definitions
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::Definitions::emplace_back(node_type c, label_type x) {
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
  // ToddCoxeter - constructors + initializers - public
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeter::ToddCoxeter()
      : CongruenceInterface(),
        _input_presentation(),
        _finished(),
        _forest(),
        _setting_stack(),
        _standardized(),
        _word_graph() {
    init();
  }

  ToddCoxeter& ToddCoxeter::init() {
    CongruenceInterface::init();
    _finished = false;
    _forest.init();
    if (_setting_stack.empty()) {
      _setting_stack.push_back(std::make_unique<Settings>());
    } else {
      _setting_stack.erase(_setting_stack.begin() + 1, _setting_stack.end());
      _setting_stack.back()->init();
    }
    _input_presentation.init();
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
    _standardized = Order::none;
    copy_settings_into_graph();
    return *this;
  }

  ToddCoxeter::ToddCoxeter(ToddCoxeter const& that) : ToddCoxeter() {
    operator=(that);
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
  }

  ToddCoxeter::ToddCoxeter(ToddCoxeter&& that) : ToddCoxeter() {
    operator=(std::move(that));
  }

  ToddCoxeter& ToddCoxeter::operator=(ToddCoxeter&& that) {
    LIBSEMIGROUPS_ASSERT(!that._setting_stack.empty());
    CongruenceInterface::operator=(std::move(that));
    _finished           = std::move(that._finished);
    _forest             = std::move(that._forest);
    _setting_stack      = std::move(that._setting_stack);
    _standardized       = std::move(that._standardized);
    _input_presentation = std::move(that._input_presentation);
    _word_graph         = std::move(that._word_graph);
    // The next line is essential so that the _word_graph.definitions()._tc
    // points at <this>.
    _word_graph.definitions().init(this);
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
    return *this;
  }

  ToddCoxeter& ToddCoxeter::operator=(ToddCoxeter const& that) {
    LIBSEMIGROUPS_ASSERT(!that._setting_stack.empty());
    CongruenceInterface::operator=(that);
    _finished = that._finished;
    _forest   = that._forest;
    _setting_stack.clear();
    for (auto const& uptr : that._setting_stack) {
      _setting_stack.push_back(std::make_unique<Settings>(*uptr));
    }
    _standardized       = that._standardized;
    _input_presentation = that._input_presentation;
    _word_graph         = that._word_graph;
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind knd, Presentation<word_type>&& p)
      : ToddCoxeter() {
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
    init(knd, std::move(p));
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind           knd,
                                 Presentation<word_type>&& p) {
    init();
    CongruenceInterface::init(knd);
    if (knd == congruence_kind::left) {
      presentation::reverse(p);
    }
    _input_presentation = p;
    presentation::normalize_alphabet(p);
    _word_graph.init(std::move(p));
    copy_settings_into_graph();
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
    return *this;
  }

  ToddCoxeter::ToddCoxeter(congruence_kind                knd,
                           Presentation<word_type> const& p)
      // call the rval ref constructor
      : ToddCoxeter(knd, Presentation<word_type>(p)) {
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind                knd,
                                 Presentation<word_type> const& p) {
    // call the rval ref init
    return init(knd, Presentation<word_type>(p));
  }

  ToddCoxeter::ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc)
      : ToddCoxeter() {
    init(knd, tc);
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
  }

  ToddCoxeter& ToddCoxeter::init(congruence_kind knd, ToddCoxeter const& tc) {
    if (tc.kind() != congruence_kind::twosided && knd != tc.kind()) {
      LIBSEMIGROUPS_EXCEPTION(
          "incompatible types of congruence, found ({} / {}) but only (left "
          "/ left), (right / right), (two-sided / *) are valid",
          tc.kind(),
          knd);
    }
    CongruenceInterface::init(knd);
    _word_graph.init(tc.internal_presentation());
    _input_presentation = tc.presentation();
    copy_settings_into_graph();
    auto& rules = _word_graph.presentation().rules;
    rules.insert(rules.end(),
                 tc.generating_pairs().cbegin(),
                 tc.generating_pairs().cend());
    if (kind() == congruence_kind::left && tc.kind() != congruence_kind::left) {
      presentation::reverse(_word_graph.presentation());
    }
    LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
    // TODO(0) don't we need to reset the setting_stack here too?
    return *this;
  }

  ToddCoxeter::~ToddCoxeter() = default;

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - settings - public
  ////////////////////////////////////////////////////////////////////////

  ToddCoxeter& ToddCoxeter::def_max(size_t val) noexcept {
    tc_settings().def_max = val;
    return *this;
  }

  size_t ToddCoxeter::def_max() const noexcept {
    return tc_settings().def_max;
  }

  ToddCoxeter& ToddCoxeter::def_policy(options::def_policy val) {
    tc_settings().def_policy = val;
    return *this;
  }

  ToddCoxeter::options::def_policy ToddCoxeter::def_policy() const noexcept {
    return tc_settings().def_policy;
  }

  ToddCoxeter& ToddCoxeter::lookahead_next(size_t n) noexcept {
    tc_settings().lookahead_next = n;
    return *this;
  }

  size_t ToddCoxeter::lookahead_next() const noexcept {
    return tc_settings().lookahead_next;
  }

  ToddCoxeter& ToddCoxeter::lookahead_min(size_t n) noexcept {
    tc_settings().lookahead_min = n;
    return *this;
  }

  size_t ToddCoxeter::lookahead_min() const noexcept {
    return tc_settings().lookahead_min;
  }

  ToddCoxeter& ToddCoxeter::lookahead_growth_factor(float val) {
    if (val < 1.0) {
      LIBSEMIGROUPS_EXCEPTION("Expected a value >= 1.0, found {}", val);
    }
    tc_settings().lookahead_growth_factor = val;
    return *this;
  }

  float ToddCoxeter::lookahead_growth_factor() const noexcept {
    return tc_settings().lookahead_growth_factor;
  }

  ToddCoxeter& ToddCoxeter::lookahead_growth_threshold(size_t val) noexcept {
    tc_settings().lookahead_growth_threshold = val;
    return *this;
  }

  size_t ToddCoxeter::lookahead_growth_threshold() const noexcept {
    return tc_settings().lookahead_growth_threshold;
  }

  ToddCoxeter& ToddCoxeter::lookahead_stop_early_ratio(float val) {
    // TODO throw if val < 0 or >= 1.0
    tc_settings().lookahead_stop_early_ratio = val;
    return *this;
  }

  float ToddCoxeter::lookahead_stop_early_ratio() const noexcept {
    return tc_settings().lookahead_stop_early_ratio;
  }

  ToddCoxeter& ToddCoxeter::lookahead_stop_early_interval(
      std::chrono::nanoseconds val) noexcept {
    tc_settings().lookahead_stop_early_interval = val;
    return *this;
  }

  std::chrono::nanoseconds
  ToddCoxeter::lookahead_stop_early_interval() const noexcept {
    return tc_settings().lookahead_stop_early_interval;
  }

  ToddCoxeter&
  ToddCoxeter::lookahead_style(options::lookahead_style val) noexcept {
    tc_settings().lookahead_style = val;
    return *this;
  }

  ToddCoxeter&
  ToddCoxeter::lookahead_extent(options::lookahead_extent val) noexcept {
    tc_settings().lookahead_extent = val;
    return *this;
  }

  ToddCoxeter::options::lookahead_style
  ToddCoxeter::lookahead_style() const noexcept {
    return tc_settings().lookahead_style;
  }

  ToddCoxeter::options::lookahead_extent
  ToddCoxeter::lookahead_extent() const noexcept {
    return tc_settings().lookahead_extent;
  }

  ToddCoxeter& ToddCoxeter::save(bool x) {
    tc_settings().save = x;
    return *this;
  }

  bool ToddCoxeter::save() const noexcept {
    return tc_settings().save;
  }

  ToddCoxeter& ToddCoxeter::strategy(options::strategy x) {
    tc_settings().strategy = x;
    return *this;
  }

  ToddCoxeter::options::strategy ToddCoxeter::strategy() const noexcept {
    return tc_settings().strategy;
  }

  ToddCoxeter& ToddCoxeter::use_relations_in_extra(bool val) noexcept {
    tc_settings().use_relations_in_extra = val;
    return *this;
  }

  bool ToddCoxeter::use_relations_in_extra() const noexcept {
    return tc_settings().use_relations_in_extra;
  }

  ToddCoxeter& ToddCoxeter::lower_bound(size_t n) noexcept {
    tc_settings().lower_bound = n;
    return *this;
  }

  size_t ToddCoxeter::lower_bound() const noexcept {
    return tc_settings().lower_bound;
  }

  ToddCoxeter& ToddCoxeter::large_collapse(size_t n) noexcept {
    _word_graph.large_collapse(n);
    return *this;
  }

  size_t ToddCoxeter::large_collapse() const noexcept {
    return _word_graph.large_collapse();
  }

  ToddCoxeter& ToddCoxeter::hlt_defs(size_t val) {
    auto l = presentation::length(presentation());
    if (val == 0) {
      LIBSEMIGROUPS_EXCEPTION("Expected a value != 0!");
    } else if (val < l) {
      LIBSEMIGROUPS_EXCEPTION("Expected a value >= {}, found {}!", l, val);
    }
    tc_settings().hlt_defs = val;
    return *this;
  }

  size_t ToddCoxeter::hlt_defs() const noexcept {
    return tc_settings().hlt_defs;
  }

  ToddCoxeter& ToddCoxeter::f_defs(size_t val) {
    if (val == 0) {
      LIBSEMIGROUPS_EXCEPTION("Expected a value != 0!");
    }
    tc_settings().f_defs = val;
    return *this;
  }

  size_t ToddCoxeter::f_defs() const noexcept {
    return tc_settings().f_defs;
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - accessors - public
  ////////////////////////////////////////////////////////////////////////

  bool ToddCoxeter::is_standardized(Order val) const {
    // TODO(0) this is probably not always valid
    return val == _standardized
           && _forest.number_of_nodes()
                  == current_word_graph().number_of_nodes_active();
  }

  bool ToddCoxeter::is_standardized() const {
    // TODO(0) this is probably not always valid, i.e. if we are standardized,
    // then grow, then collapse, but end up with the same number of nodes
    // again.
    return _standardized != Order::none
           && _forest.number_of_nodes()
                  == current_word_graph().number_of_nodes_active();
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - modifiers - public
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::shrink_to_fit() {
    if (!finished()) {
      return;
    }
    standardize(Order::shortlex);
    _word_graph.erase_free_nodes();
    _word_graph.induced_subgraph_no_checks(
        0, _word_graph.number_of_nodes_active());
  }

  bool ToddCoxeter::standardize(Order val) {
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

  void ToddCoxeter::really_run_impl() {
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

  void ToddCoxeter::run_impl() {
    if (is_obviously_infinite(*this)) {
      LIBSEMIGROUPS_EXCEPTION(
          "there are infinitely many classes in the congruence and "
          "Todd-Coxeter will never terminate");
    } else if (strategy() != options::strategy::felsch
               && strategy() != options::strategy::hlt && running_until()) {
      LIBSEMIGROUPS_EXCEPTION("the strategy {} cannot be used with run_until!",
                              strategy());
    }

    init_run();

    if (reporting_enabled()
        && (!running_for()
            || report_every() >= std::chrono::milliseconds(1'500))) {
      // TODO(0) report_strategy
      auto msg = fmt::format("{:+<90}\n", "");
      msg += fmt_default("ToddCoxeter: Using {} strategy . . .\n", strategy());
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
  // ToddCoxeter - main strategies - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::init_run() {
    if (is_obviously_infinite(*this)) {
      LIBSEMIGROUPS_EXCEPTION(
          "there are infinitely many classes in the congruence and "
          "Todd-Coxeter will never terminate");
    }

    _word_graph.settings(*this);
    _word_graph.reset_start_time();
    _word_graph.stats_check_point();
    auto       first = generating_pairs().cbegin();
    auto       last  = generating_pairs().cend();
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

    if (kind() == congruence_kind::twosided && !generating_pairs().empty()) {
      // TODO(0) avoid copy of presentation here, if possible
      Presentation<word_type> p = internal_presentation();
      if (p.alphabet().size() != _word_graph.out_degree()) {
        LIBSEMIGROUPS_ASSERT(p.alphabet().size() == 0);
        p.alphabet(_word_graph.out_degree());
      }
      presentation::add_rules(
          p, generating_pairs().cbegin(), generating_pairs().cend());
      _word_graph.presentation(std::move(p));
    }

    if (save() || strategy() == options::strategy::felsch) {
      _word_graph.process_definitions();
    }
  }

  void ToddCoxeter::finalise_run() {
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
      _finished = true;
    }
  }

  void ToddCoxeter::felsch() {
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
    if (!stopped()) {
      _finished = true;
    }
  }

  void ToddCoxeter::hlt() {
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

  void ToddCoxeter::CR_style() {
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

  void ToddCoxeter::R_over_C_style() {
    SettingsGuard guard(this);

    strategy(options::strategy::hlt);
    run_until([this]() -> bool {
      return current_word_graph().number_of_nodes_active() >= lookahead_next();
    });
    lookahead_extent(options::lookahead_extent::full);
    perform_lookahead(StopEarly);
    CR_style();
  }

  void ToddCoxeter::Cr_style() {
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

  void ToddCoxeter::Rc_style() {
    SettingsGuard guard(this);

    strategy(options::strategy::hlt);
    auto   M = current_word_graph().number_of_nodes_active();
    size_t N = presentation::length(internal_presentation());
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
  // ToddCoxeter - reporting - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::report_next_lookahead(size_t old_value) const {
    static const std::string pad(8, ' ');
    int64_t diff = static_cast<int64_t>(lookahead_next()) - old_value;
    report_default("ToddCoxeter: next lookahead at {} | {:>12} (nodes)  "
                   "| {:>12} (diff)\n",
                   pad,
                   fmt::group_digits(lookahead_next()),
                   detail::signed_group_digits(diff));
    report_no_prefix("{:-<90}\n", "");
  }

  void ToddCoxeter::report_nodes_killed(int64_t N) const {
    report_no_prefix("{:-<90}\n", "");
    report_default(
        "ToddCoxeter: lookahead complete with    | {:>12} (killed) |\n",
        detail::group_digits(-1 * N));
  }

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter - lookahead - private
  ////////////////////////////////////////////////////////////////////////

  void ToddCoxeter::perform_lookahead(bool stop_early) {
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

  size_t ToddCoxeter::hlt_lookahead(bool stop_early) {
    size_t const old_number_of_killed = _word_graph.number_of_nodes_killed();
    _word_graph.make_compatible(_word_graph.lookahead_cursor(),
                                internal_presentation().rules.cbegin(),
                                internal_presentation().rules.cend(),
                                stop_early,
                                lookahead_stop_early_interval(),
                                lookahead_stop_early_ratio());
    return _word_graph.number_of_nodes_killed() - old_number_of_killed;
  }

  size_t ToddCoxeter::felsch_lookahead() {
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

  namespace todd_coxeter {
    tril is_non_trivial(ToddCoxeter&              tc,
                        size_t                    tries,
                        std::chrono::milliseconds try_for,
                        float                     threshold) {
      using detail::node_managed_graph::random_active_node;

      if (is_obviously_infinite(tc)) {
        return tril::TRUE;
      } else if (tc.finished()) {
        return tc.number_of_classes() == 1 ? tril::FALSE : tril::TRUE;
      }

      for (size_t try_ = 0; try_ < tries; ++try_) {
        report_default(
            "trying to show non-triviality: {} / {}\n", try_ + 1, tries);
        ToddCoxeter copy(tc);
        copy.save(true);
        while (!copy.finished()) {
          copy.run_for(try_for);
          size_t limit = copy.current_word_graph().number_of_nodes_active();
          while (copy.current_word_graph().number_of_nodes_active()
                     >= threshold * limit
                 && !copy.finished()) {
            node_type c1 = random_active_node(copy.current_word_graph());
            node_type c2 = random_active_node(copy.current_word_graph());
            auto&     wg = const_cast<ToddCoxeter::word_graph_type&>(
                copy.current_word_graph());
            wg.merge_nodes_no_checks(c1, c2);
            wg.process_coincidences<detail::RegisterDefs>();
            wg.process_definitions();
            copy.run_for(try_for);
          }
        }
        if (copy.number_of_classes() > 1) {
          report_default("successfully showed non-triviality!\n");
          return tril::TRUE;
        }
      }
      report_default("failed to show non-triviality!\n");
      return tril::unknown;
    }

#if !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

    uint64_t number_of_idempotents(ToddCoxeter& tc) {
      word_type tmp;
      size_t    i = 0;
      return normal_forms(tc) | rx::filter([&](auto const& w) {
               tmp.clear();
               tmp.insert(tmp.end(), w.cbegin(), w.cend());
               tmp.insert(tmp.end(), w.cbegin(), w.cend());
               return todd_coxeter::index_of(tc, tmp) == i++;
             })
             | rx::count();
    }
#if !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    std::vector<std::vector<word_type>> non_trivial_classes(ToddCoxeter& tc1,
                                                            ToddCoxeter& tc2) {
      return non_trivial_classes(tc1, normal_forms(tc2));
    }
  }  // namespace todd_coxeter
}  // namespace libsemigroups
