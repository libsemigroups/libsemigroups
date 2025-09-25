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

#include <chrono>
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

#include "libsemigroups/detail/cong-common-class.hpp"  // for CongruenceCommon
#include "libsemigroups/detail/felsch-graph.hpp"       // for DoNotRegisterDefs
#include "libsemigroups/detail/fmt.hpp"       // for format_decimal, copy_str
#include "libsemigroups/detail/guard.hpp"     // for Guard
#include "libsemigroups/detail/iterator.hpp"  // for operator+
#include "libsemigroups/detail/node-manager.hpp"  // for NodeManager
#include "libsemigroups/detail/report.hpp"        // for report_no_prefix

namespace libsemigroups {

  constexpr bool do_not_register_defs
      = detail::felsch_graph::do_not_register_defs;

  constexpr bool do_register_defs = detail::felsch_graph::do_register_defs;

  constexpr bool ye_print_divider = true;
  constexpr bool no_print_divider = false;

  using DoRegisterDefs = detail::felsch_graph::DoRegisterDefs<
      detail::NodeManagedGraph<detail::ToddCoxeterImpl::Graph::node_type>,
      detail::ToddCoxeterImpl::Definitions>;

  using DoNotRegisterDefs = Noop;

  namespace {
    constexpr auto const run_color = fmt::bg(fmt::terminal_color::white)
                                     | fmt::fg(fmt::terminal_color::black);
    auto const phase_color
        = fmt::bg(fmt::rgb(96, 96, 96)) | fmt::fg(fmt::terminal_color::white);

    void report_keys(std::set<std::string> const& keys) {
      if (!keys.empty()) {
        report_default("ToddCoxeter: where:  ");
      }
      bool first = true;
      for (auto const& key : keys) {
        if (!first) {
          report_default("ToddCoxeter:         {}", key);
        } else {
          report_no_prefix("{}", key);
          first = false;
        }
      }
    }

    std::string italic(char const* var) {
      return fmt::format(fmt::emphasis::italic, "{}", var);
    }

    std::string underline(char const* var) {
      return fmt::format(fmt::emphasis::underline, "{}", var);
    }

    std::string underline(std::string const& var) {
      return fmt::format(fmt::emphasis::underline, "{}", var);
    }

    template <typename Thing>
    std::string toupper(Thing const& thing) {
      auto result = fmt::format("{}", thing);
      std::for_each(result.begin(), result.end(), [](auto& val) {
        val = std::toupper(val);
      });
      return result;
    }

    std::string to_percent(uint64_t num, uint64_t denom) {
      double val = static_cast<double>(num) * 100 / denom;
      return std::isnan(val) ? "-" : fmt::format("{:.0f}%", val);
    }

  }  // namespace

  namespace detail {
    namespace {
      ReportCell<5> report_cell() {
        ReportCell<5> rc;
        rc.min_width(12).min_width(0, 0).min_width(1, 23).align(1, Align::left);
        return rc;
      }
    }  // namespace

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

    void ToddCoxeterImpl::reset_settings_stack() {
      if (_settings_stack.empty()) {
        _settings_stack.push_back(std::make_unique<Settings>());
      } else {
        _settings_stack.erase(_settings_stack.begin() + 1,
                              _settings_stack.end());
        _settings_stack.back()->init();
      }
      LIBSEMIGROUPS_ASSERT(!_settings_stack.empty());
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
    ToddCoxeterImpl::Graph::init(Presentation<word_type>&& p) {
      NodeManager<node_type>::clear();
      FelschGraph_::init(std::move(p));
      // TODO(1) shouldn't add nodes here because then there'll be more than
      // there should be (i.e. NodeManager and FelschGraph_ will have
      // different numbers of nodes
      FelschGraph_::add_nodes(NodeManager<node_type>::node_capacity());
      return *this;
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

    template <typename RuleIterator>
    void ToddCoxeterImpl::Graph::make_compatible(ToddCoxeterImpl* tc,
                                                 node_type&       current,
                                                 RuleIterator     first,
                                                 RuleIterator     last,
                                                 bool             stop_early) {
      auto last_stop_early_check = std::chrono::high_resolution_clock::now();
      auto const old_number_of_killed    = number_of_nodes_killed();
      auto       killed_at_prev_interval = old_number_of_killed;

      bool   old_ticker_running = tc->_ticker_running;
      auto   start_time         = std::chrono::high_resolution_clock::now();
      Ticker ticker;

      CollectCoincidences                    incompat(_coinc);
      typename FelschGraph_::NoPreferredDefs prefdefs;

      while (current != NodeManager<node_type>::first_free_node()
             && (!stop_early || (tc->running() && !tc->stopped())
                 || !tc->running())) {
        // If stop_early and tc->stopped(), then we exit this loop.
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
        tc->_stats.lookahead_position++;
        if (tc->lookahead_stop_early(
                stop_early, last_stop_early_check, killed_at_prev_interval)) {
          break;
        }
        if (!tc->_ticker_running && reporting_enabled()
            && delta(start_time) > std::chrono::seconds(1)) {
          tc->_ticker_running = true;
          ticker(
              [&tc]() { tc->report_progress_from_thread(ye_print_divider); });
        }
      }
      tc->_stats.lookahead_nodes_killed
          += (number_of_nodes_killed() - killed_at_prev_interval);
      tc->_ticker_running = old_ticker_running;
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
    // ToddCoxeterImpl::NonAtomicStats
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeterImpl::NonAtomicStats& ToddCoxeterImpl::NonAtomicStats::init() {
      create_or_init_time = std::chrono::high_resolution_clock::now();
      run_index           = 0;

      all_num_hlt_phases       = 0;
      all_num_felsch_phases    = 0;
      all_num_lookahead_phases = 0;
      run_num_hlt_phases       = 0;
      run_num_felsch_phases    = 0;
      run_num_lookahead_phases = 0;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    ToddCoxeterImpl::ToddCoxeterImpl()
        : CongruenceCommon(),
          _finished(),
          _forest(),
          _settings_stack(),
          _standardized(),
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
      _finished = false;
      _forest.init();
      reset_settings_stack();
      _standardized = Order::none;
      _state        = state::none;
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

    ToddCoxeterImpl::ToddCoxeterImpl(ToddCoxeterImpl&& that)
        : ToddCoxeterImpl() {
      operator=(std::move(that));
    }

    ToddCoxeterImpl& ToddCoxeterImpl::operator=(ToddCoxeterImpl&& that) {
      LIBSEMIGROUPS_ASSERT(!that._settings_stack.empty());
      CongruenceCommon::operator=(std::move(that));
      _finished       = std::move(that._finished);
      _forest         = std::move(that._forest);
      _settings_stack = std::move(that._settings_stack);
      _state          = that._state.load();
      _stats          = std::move(that._stats);
      _standardized   = std::move(that._standardized);
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
      _finished = that._finished;
      _forest   = that._forest;
      _settings_stack.clear();
      for (auto const& uptr : that._settings_stack) {
        _settings_stack.push_back(std::make_unique<Settings>(*uptr));
      }
      _standardized   = that._standardized;
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
      // TODO(0) what about the word graph, if it's partially defined at least
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
      // TODO(1) this is probably not always valid, i.e. if we are
      // standardized, then grow, then collapse, but end up with the same
      // number of nodes again.
      return _standardized != Order::none
             && _forest.number_of_nodes()
                    == current_word_graph().number_of_nodes_active();
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::shrink_to_fit() {
      standardize(Order::shortlex);
      _word_graph.erase_free_nodes();
      _word_graph.induced_subgraph_no_checks(
          0, _word_graph.number_of_nodes_active());
    }

    bool ToddCoxeterImpl::standardize(Order val) {
      if (is_standardized(val)) {
        return false;
      }
      time_point start_time;
      if (reporting_enabled()) {
        start_time = std::chrono::high_resolution_clock::now();
        report_no_prefix(report_divider());
        report_default(
            "ToddCoxeter: {} standardizing the word graph, this might "
            "take a few moments!\n",
            val);
      }
      _forest.init();
      _forest.add_nodes(_word_graph.number_of_nodes_active());
      bool result   = word_graph::standardize(_word_graph, _forest, val);
      _standardized = val;
      report_default("ToddCoxeter: the word graph was {} standardized in {}\n",
                     val,
                     string_time(delta(start_time)));
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::really_run_impl() {
      if (strategy() == options::strategy::felsch) {
        Guard guard(_state, state::felsch);
        felsch();
      } else if (strategy() == options::strategy::hlt) {
        Guard guard(_state, state::hlt);
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

    void ToddCoxeterImpl::run_impl() {
      using std::chrono::duration_cast;
      using std::chrono::seconds;
      if (!running_for() && !running_until() && is_obviously_infinite(*this)) {
        LIBSEMIGROUPS_EXCEPTION(
            "there are infinitely many classes in the congruence and "
            "Todd-Coxeter will never terminate");
      } else if (strategy() != options::strategy::felsch
                 && strategy() != options::strategy::hlt && running_until()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the strategy {} cannot be used with run_until !", strategy());
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

      init_run();
      if (!_ticker_running && reporting_enabled()
          && (!running_for()
              || duration_cast<seconds>(running_for_how_long())
                     >= seconds(1))) {
        _ticker_running = true;
        Ticker t([this]() { report_progress_from_thread(ye_print_divider); });
        really_run_impl();
        _ticker_running = false;
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
      stats_run_start();
      report_before_run();

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
      if (strategy() == options::strategy::felsch && use_relations_in_extra()) {
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
          perform_lookahead(StopEarly);
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

    void ToddCoxeterImpl::report_after_phase() const {
      if (reporting_enabled()) {
        report_no_prefix(report_divider());
        report_default("ToddCoxeter: {}\n",
                       fmt::format(phase_color,
                                   "{} {}.{} STOP",
                                   toupper(_state.load()),
                                   _stats.run_index,
                                   _stats.phase_index));
        report_progress_from_thread(no_print_divider);
      }
    }

    void
    ToddCoxeterImpl::report_after_lookahead(size_t old_lookahead_next) const {
      auto gd  = group_digits;
      auto sgd = signed_group_digits;

      if (reporting_enabled()) {
        auto lgf      = lookahead_growth_factor();
        auto lgf_name = italic("f");
        auto lgf_key  = fmt::format(
            "{} = lookahead_growth_factor()    = {}\n", lgf_name, lgf);

        auto lgt      = lookahead_growth_threshold();
        auto lgt_name = italic("t");
        auto lgt_key  = fmt::format(
            "{} = lookahead_growth_threshold() = {}\n", lgt_name, lgt);

        auto oln      = old_lookahead_next;
        auto oln_name = italic("n");
        auto oln_key  = fmt::format(
            "{} = lookahead_next()             = {}\n", oln_name, gd(oln));

        auto ln = lookahead_next();

        auto a      = _word_graph.number_of_nodes_active();
        auto a_name = italic("a");
        auto a_key  = fmt::format(
            "{} = number_of_nodes_active()     = {}\n", a_name, gd(a));

        auto l      = _stats.lookahead_nodes_killed.load();
        auto l_name = italic("l");
        auto l_key  = fmt::format(
            "{} = nodes killed in lookahead    = {}\n", l_name, gd(l));

        auto m      = lookahead_min();
        auto m_name = italic("m");
        auto m_key  = fmt::format(
            "{} = lookahead_min()              = {}\n", m_name, gd(m));

        std::set<std::string> keys;

        int64_t const diff = static_cast<int64_t>(ln) - oln;
        std::string   reason
            = fmt_default("ToddCoxeter: lookahead_next() is now ");

        if (a * lgf < oln || a > oln) {
          reason += fmt::format("max({} x {} = {}, {} = {}) ({})\n",
                                lgf_name,
                                a_name,
                                gd(lgf * a),
                                m_name,
                                gd(m),
                                sgd(diff));
          if (a * lgf < oln) {
            // TODO(1) add different levels of reporting, and only print the
            // "because" stuff if the level is > 0 (where 0 would be the
            // default).
            reason += fmt_default("ToddCoxeter: because {} x {} < {}\n",
                                  lgf_name,
                                  a_name,
                                  oln_name);
          } else {
            reason += fmt_default(
                "ToddCoxeter: because {} > {}\n", a_name, oln_name);
          }
          keys.insert(a_key);
          keys.insert(lgf_key);
          keys.insert(oln_key);
          keys.insert(m_key);
        } else if (l < (l + a) / lgt) {
          reason += fmt::format("{} x {} = {} ({})\n",
                                oln_name,
                                lgf_name,
                                gd(oln * lgf),
                                sgd(diff));

          reason
              += fmt_default("ToddCoxeter: because: {} < ({} + {}) / {} = {}\n",
                             l_name,
                             l_name,
                             a_name,
                             lgt_name,
                             gd((l + a) / lgt));
          keys.insert(a_key);
          keys.insert(l_key);
          keys.insert(lgf_key);
          keys.insert(lgt_key);
          keys.insert(oln_key);
        } else {
          reason += fmt::format("{} ({})\n", gd(ln), sgd(diff));
          reason += fmt_default("ToddCoxeter: because:\n");
          reason += fmt_default("ToddCoxeter: 1. {} <= {} x {} = {}\n",
                                oln_name,
                                lgf_name,
                                a_name,
                                gd(lgf * a));
          reason += fmt_default("ToddCoxeter: 2. {} <= {}\n", a_name, oln_name);
          reason += fmt_default("ToddCoxeter: 3. {} >= ({} + {}) / {} = {}\n",
                                l_name,
                                l_name,
                                a_name,
                                lgt_name,
                                gd((l + a) / lgt));
          keys.insert(a_key);
          keys.insert(l_key);
          keys.insert(lgf_key);
          keys.insert(lgt_key);
          keys.insert(oln_key);
        }

        report_no_prefix(report_divider());
        report_default("ToddCoxeter: {}\n",
                       fmt::format(phase_color,
                                   "LOOKAHEAD {}.{} STOP",
                                   _stats.run_index,
                                   _stats.phase_index));
        report_progress_from_thread(no_print_divider);
        if (!finished()) {
          report_no_prefix(reason);
          report_keys(keys);
        }
      }
    }

    void ToddCoxeterImpl::report_after_run() const {
      if (reporting_enabled()) {
        std::string reason = finished() ? "finished" : string_why_we_stopped();

        // Often the end of a run coincides with the end of a lookahead, which
        // already prints out this info, so avoid duplication in case nothing
        // has changed.

        report_no_prefix(report_divider());
        report_default("{}: {} ({})\n",
                       report_prefix(),
                       fmt::format(run_color, "RUN {} STOP", _stats.run_index),
                       reason);
        auto rc = report_cell();
        rc("{}: {} | {} | {} | {}\n",
           report_prefix(),
           underline(fmt::format("run {}", _stats.run_index)),
           underline("lookahead"),
           underline("hlt"),
           underline("felsch"));
        rc("{}: {} | {} | {} | {}\n",
           report_prefix(),
           "num. phases",
           group_digits(_stats.run_num_lookahead_phases),
           group_digits(_stats.run_num_hlt_phases),
           group_digits(_stats.run_num_felsch_phases));

        auto this_run_time = delta(_stats.run_start_time);

        auto percent_run_time_lookahead = to_percent(
            _stats.run_lookahead_phases_time.count(), this_run_time.count());
        auto percent_run_time_hlt = to_percent(
            _stats.run_hlt_phases_time.count(), this_run_time.count());
        auto percent_run_time_felsch = to_percent(
            _stats.run_felsch_phases_time.count(), this_run_time.count());

        // When the times are very short (microseconds) the percentage spent in
        // each phase type won't add up to 100% (it will be less) because the
        // calling of the functions before hlt/felsch (init_run etc) take a
        // non-trivial % of the run time. Be good to fix this, but not sure how
        // exactly.
        rc("{}: {} | {} | {} | {}\n",
           report_prefix(),
           "time spent in phases",
           fmt::format("{} ({})",
                       string_time(_stats.run_lookahead_phases_time),
                       percent_run_time_lookahead),
           fmt::format("{} ({})",
                       string_time(_stats.run_hlt_phases_time),
                       percent_run_time_hlt),
           fmt::format("{} ({})",
                       string_time(_stats.run_felsch_phases_time),
                       percent_run_time_felsch));
        if (_stats.run_index > 0) {
          rc("{}: {} | {} | {} | {}\n",
             report_prefix(),
             underline("all runs"),
             underline("lookahead"),
             underline("hlt"),
             underline("felsch"));
          rc("{}: {} | {} | {} | {}\n",
             report_prefix(),
             "num. phases ",
             group_digits(_stats.all_num_lookahead_phases
                          + _stats.run_num_lookahead_phases),
             group_digits(_stats.all_num_hlt_phases
                          + _stats.run_num_hlt_phases),
             group_digits(_stats.all_num_felsch_phases
                          + _stats.run_num_felsch_phases));

          auto total_lookahead = _stats.all_lookahead_phases_time
                                 + _stats.run_lookahead_phases_time;
          auto total_hlt
              = _stats.all_hlt_phases_time + _stats.run_hlt_phases_time;
          auto total_felsch
              = _stats.all_felsch_phases_time + _stats.run_felsch_phases_time;
          auto total = (_stats.all_runs_time + this_run_time).count();

          auto percent_total_lookahead
              = to_percent(total_lookahead.count(), total);
          auto percent_total_hlt    = to_percent(total_hlt.count(), total);
          auto percent_total_felsch = to_percent(total_felsch.count(), total);

          rc("{}: {} | {} | {} | {}\n",
             report_prefix(),
             "time spent in",
             fmt::format("{} ({})",
                         string_time(total_lookahead),
                         percent_total_lookahead),
             fmt::format("{} ({})", string_time(total_hlt), percent_total_hlt),
             fmt::format(
                 "{} ({})", string_time(total_felsch), percent_total_felsch));
        }
        add_timing_row(rc);
        // TODO(1) time spent process_definitions, process_coincidences?
      }
    }

    void ToddCoxeterImpl::report_before_phase(std::string_view info) const {
      if (reporting_enabled()) {
        report_no_prefix(report_divider());
        report_default("ToddCoxeter: {}{}\n",
                       fmt::format(phase_color,
                                   "{} {}.{} START",
                                   toupper(_state.load()),
                                   _stats.run_index,
                                   _stats.phase_index),
                       info.empty() ? "" : fmt::format(" ({})", info));
        report_progress_from_thread(no_print_divider);
      }
    }

    void ToddCoxeterImpl::report_before_lookahead() const {
      if (reporting_enabled()) {
        report_before_phase(fmt::format("lookahead_extent() = {}, "
                                        "lookahead_style() = {}",
                                        lookahead_extent(),
                                        lookahead_style()));

        if (current_word_graph().definitions().any_skipped()) {
          report_default(
              "ToddCoxeter: triggered because there are skipped "
              "definitions ({} active nodes)!\n",
              group_digits(current_word_graph().number_of_nodes_active()));
        } else if (current_word_graph().number_of_nodes_active()
                   > lookahead_next()) {
          auto ln      = lookahead_next();
          auto ln_name = italic("n");
          auto ln_key  = fmt::format("{} = lookahead_next()         = {}\n",
                                    ln_name,
                                    group_digits(ln));

          auto a      = current_word_graph().number_of_nodes_active();
          auto a_name = italic("a");
          auto a_key  = fmt::format(
              "{} = number_of_nodes_active() = {}\n", a_name, group_digits(a));

          std::set<std::string> keys;
          report_default("ToddCoxeter: because {} >= {}\n", a_name, ln_name);
          keys.insert(a_key);
          keys.insert(ln_key);
          report_keys(keys);
        }
      }
    }

    void ToddCoxeterImpl::report_before_run() const {
      if (reporting_enabled()) {
        report_no_prefix(report_divider());
        report_default("ToddCoxeter: {} (strategy() = {})\n",
                       fmt::format(run_color, "RUN {} START", _stats.run_index),
                       // TODO(1) if using ACE style strategy include the value
                       // of the relevant setting
                       // TODO(1) add more nuance when not using hlt/felsch
                       strategy());
        if (_stats.run_index > 0) {
          report_times();
        }

        report_presentation();
      }
    }

    void
    ToddCoxeterImpl::report_lookahead_stop_early(size_t expected,
                                                 size_t killed_last_interval) {
      if (reporting_enabled()) {
        auto gd       = group_digits;
        auto interval = string_time(lookahead_stop_early_interval());
        report_no_prefix(report_divider());
        report_default("ToddCoxeter: too few nodes killed in last {} = "
                       "{}, stopping lookahead early!\n",
                       italic("i"),
                       interval);
        report_default("ToddCoxeter: expected at least {} x {} = {} but "
                       "found {}\n",
                       italic("r"),
                       italic("a"),
                       gd(expected),
                       gd(killed_last_interval));
        report_keys({fmt::format("{} = lookahead_stop_early_ratio()    = {}\n",
                                 italic("r"),
                                 lookahead_stop_early_ratio()),
                     fmt::format("{} = lookahead_stop_early_interval() = {}\n",
                                 italic("i"),
                                 interval),
                     fmt::format("{} = number_of_nodes_active()        = {}\n",
                                 italic("a"),
                                 gd(number_of_nodes_active()))});
      }
    }

    void ToddCoxeterImpl::report_presentation() const {
      report_default("ToddCoxeter: {}",
                     presentation::to_report_string(internal_presentation()));
    }

    void ToddCoxeterImpl::report_progress_from_thread(bool divider) const {
      if (reporting_enabled() && _state != state::none) {
        // Sometimes this gets called concurrently but slightly after the end
        // of a phase, which results in a weird NONE block with messed up
        // numbers being printed.
        auto rc = report_cell();

        // Set the value of _stats.report_nodes_active_prev to the current
        // number of active nodes when active_nodes is destructed,
        // active_nodes has the 2nd argument as its value
        DeferSet active_nodes(_stats.report_nodes_active_prev,
                              current_word_graph().number_of_nodes_active());
        DeferSet active_edges(_stats.report_edges_active_prev,
                              current_word_graph().number_of_edges_active());

        if (divider) {
          report_no_prefix(report_divider());
        }
        add_nodes_rows(rc, active_nodes);
        add_edges_rows(rc, active_nodes, active_edges);
        add_timing_row(rc);
        add_lookahead_row(rc);

        stats_report_stop();
      }
    }  // namespace detail

    void ToddCoxeterImpl::add_timing_row(ReportCell<5>& rc) const {
      auto this_run_time   = delta(_stats.run_start_time);
      auto this_phase_time = delta(_stats.phase_start_time);
      // We don't use start_time() in the next line because this gets reset in
      // Runner::run_for.
      auto elapsed = delta(_stats.create_or_init_time);

      LIBSEMIGROUPS_ASSERT(elapsed >= _stats.all_runs_time + this_run_time);
      std::string c1;
      if (_stats.report_index == 0 || _state == state::none) {
        c1 = underline("time");
      } else {
        c1 = fmt::format("{} {}.{} = {}",
                         toupper(_state.load()),
                         _stats.run_index,
                         _stats.phase_index,
                         string_time(this_phase_time));
      }

      rc("{}: {} | {} | {} | {}\n",
         report_prefix(),
         c1,
         fmt::format(
             "run {} = {}", _stats.run_index, string_time(this_run_time)),
         fmt::format("all runs = {}",
                     string_time(_stats.all_runs_time + this_run_time)),
         fmt::format("elapsed = {}", string_time(elapsed)));
    }
    void ToddCoxeterImpl::report_times() const {
      auto rc = report_cell();
      add_timing_row(rc);
    }

    // The 2nd argument for the next function is required
    // because we need the value at a fixed point in time (due to
    // multi-threaded reporting).
    void ToddCoxeterImpl::add_nodes_rows(ReportCell<5>& rc,
                                         uint64_t       active_nodes) const {
      auto const X = _stats.run_index, Y = _stats.phase_index,
                 Z = _stats.report_index;

      DeferSet defined(_stats.report_nodes_defined_prev,
                       current_word_graph().number_of_nodes_defined());
      DeferSet killed(_stats.report_nodes_killed_prev,
                      current_word_graph().number_of_nodes_killed());

      auto const active_diff1
          = signed_group_digits(active_nodes - _stats.report_nodes_active_prev);
      auto const killed_diff1
          = signed_group_digits(killed - _stats.report_nodes_killed_prev);
      auto const defined_diff1
          = signed_group_digits(defined - _stats.report_nodes_defined_prev);

      auto const active_diff2 = signed_group_digits(
          active_nodes - _stats.phase_nodes_active_at_start);
      auto const killed_diff2
          = signed_group_digits(killed - _stats.phase_nodes_killed_at_start);
      auto const defined_diff2
          = signed_group_digits(defined - _stats.phase_nodes_defined_at_start);

      rc("{}: {} | {} | {} | {}\n",
         report_prefix(),
         fmt::format(fmt::emphasis::underline,
                     "{} {}.{}.{}",
                     toupper(_state.load()),
                     X,
                     Y,
                     Z),
         underline("active"),
         underline("killed"),
         underline("defined"));
      rc("{}: {} | {} | {} | {}\n",
         report_prefix(),
         "nodes",
         group_digits(active_nodes),
         group_digits(killed),
         group_digits(defined));
      if (Z > 0) {
        rc("{}: {} | {} | {} | {}\n",
           report_prefix(),
           fmt::format("diff {}.{}.{}", X, Y, Z - 1),
           active_diff1,
           killed_diff1,
           defined_diff1);
        if (Z > 1) {
          rc("{}: {} | {} | {} | {}\n",
             report_prefix(),
             fmt::format("diff {}.{}.0", X, Y),
             active_diff2,
             killed_diff2,
             defined_diff2);
        }
      }
      // TODO(1) could add rows with max. overall/run/phase./min. values.
      // I (JDM) think this might be quite useful, but there's already a lot in
      // the reported info, so I'm skipping it for now.
    }

    // The 2nd and 3rd arguments for the next function are required
    // because we need the values at a fixed point in time (due to
    // multi-threaded reporting).
    void ToddCoxeterImpl::add_edges_rows(ReportCell<5>& rc,
                                         uint64_t       active_nodes,
                                         uint64_t       active_edges) const {
      auto const percent_complete = complete(active_nodes, active_edges);
      auto const X = _stats.run_index, Y = _stats.phase_index,
                 Z = _stats.report_index;
      auto const missing_edges
          = active_nodes * _word_graph.out_degree() - active_edges;

      rc("{}: {} | {} | {} | {}\n",
         report_prefix(),
         "",
         underline("active"),
         underline("missing"),
         underline("% complete"));
      rc("{}: {} | {} | {} | {}\n",
         report_prefix(),
         "edges",
         group_digits(active_edges),
         group_digits(missing_edges),
         fmt::format("{:.1f}%", 100 * percent_complete));
      if (Z > 0) {
        auto const active_diff1
            = active_edges - _stats.report_edges_active_prev;
        float const complete_diff1
            = 100
              * (percent_complete
                 - static_cast<float>(_stats.report_complete_prev));
        auto const missing_diff1
            = missing_edges
              - (_stats.report_nodes_active_prev * _word_graph.out_degree()
                 - _stats.report_edges_active_prev);

        rc("{}: {} | {} | {} | {}\n",
           report_prefix(),
           fmt::format("diff {}.{}.{}", X, Y, Z - 1),
           signed_group_digits(active_diff1),
           signed_group_digits(missing_diff1),
           fmt::format(
               "{}{:.1f}%", complete_diff1 >= 0 ? "+" : "", complete_diff1));
        if (Z > 1) {
          auto const active_diff2
              = active_edges - _stats.phase_edges_active_at_start;
          float const complete_diff2
              = 100
                * (percent_complete
                   - static_cast<float>(_stats.phase_complete_at_start));
          auto const missing_diff2
              = missing_edges
                - (_stats.phase_nodes_active_at_start * _word_graph.out_degree()
                   - _stats.phase_edges_active_at_start);
          rc("{}: {} | {} | {} | {}\n",
             report_prefix(),
             fmt::format("diff {}.{}.0", X, Y),
             signed_group_digits(active_diff2),
             signed_group_digits(missing_diff2),
             fmt::format(
                 "{}{:.1f}%", complete_diff2 >= 0 ? "+" : "", complete_diff2));
        }
      }
      _stats.report_complete_prev = percent_complete;
    }

    void ToddCoxeterImpl::add_lookahead_row(ReportCell<5>& rc) const {
      if (_state == state::lookahead && _stats.report_index != 0
          && this_threads_id() != 0) {
        // Don't call this in the main thread, because that's where we write
        // after a lookahead, where this percentage is often wrong and
        // superfluous.

        // It is difficult to get the exact value of the % complete due to
        // multi-threading issues, hence we don't try, we just assume that
        // nodes are uniformly randomly killed, leading to the following
        // approximate progress . . .
        auto const N = _stats.phase_nodes_active_at_start;
        auto const p = _stats.lookahead_position.load();
        auto const r = _stats.lookahead_nodes_killed.load();
        rc("{}: {} | {} \n",
           report_prefix(),
           "lookahead progress",
           fmt::format("~{:.1f}%",
                       (p - static_cast<double>(p * r) / N) * 100 / (N - r)));
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - lookahead - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::perform_lookahead(bool stop_early) {
      if (!running()) {
        stats_run_start();
        report_before_run();
      }
      stats_phase_start();
      Guard guard(_state, state::lookahead);

      report_before_lookahead();

      auto& current = _word_graph.lookahead_cursor();
      if (lookahead_extent() == options::lookahead_extent::partial) {
        // Start lookahead from the node after _current
        current = _word_graph.next_active_node(_word_graph.cursor());
        _stats.lookahead_position = _word_graph.position_of_node(current);
      } else {
        LIBSEMIGROUPS_ASSERT(lookahead_extent()
                             == options::lookahead_extent::full);
        current                   = _word_graph.initial_node();
        _stats.lookahead_position = 0;
      }

      _stats.lookahead_nodes_killed = 0;

      if (lookahead_style() == options::lookahead_style::hlt) {
        hlt_lookahead(stop_early);
      } else {
        LIBSEMIGROUPS_ASSERT(lookahead_style()
                             == options::lookahead_style::felsch);
        felsch_lookahead(stop_early);
      }

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
        lookahead_next(std::max(
            lookahead_min(),
            static_cast<size_t>(lookahead_growth_factor() * num_nodes)));
      } else if (_stats.lookahead_nodes_killed
                 < ((num_nodes + _stats.lookahead_nodes_killed)
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
      report_after_lookahead(old_lookahead_next);
      stats_phase_stop();
      if (!running()) {
        report_after_run();
        stats_run_stop();
      }
    }

    void ToddCoxeterImpl::hlt_lookahead(bool stop_early) {
      _word_graph.make_compatible(this,
                                  _word_graph.lookahead_cursor(),
                                  internal_presentation().rules.cbegin(),
                                  internal_presentation().rules.cend(),
                                  stop_early);
    }

    bool ToddCoxeterImpl::lookahead_stop_early(
        bool                                            stop_early,
        std::chrono::high_resolution_clock::time_point& last_stop_early_check,
        uint64_t& killed_at_prev_interval) {
      if (stop_early
          && delta(last_stop_early_check) > lookahead_stop_early_interval()) {
        size_t killed_last_interval
            = current_word_graph().number_of_nodes_killed()
              - killed_at_prev_interval;
        killed_at_prev_interval = current_word_graph().number_of_nodes_killed();
        _stats.lookahead_nodes_killed += killed_last_interval;

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

    void ToddCoxeterImpl::felsch_lookahead(bool stop_early) {
      auto last_stop_early_check = std::chrono::high_resolution_clock::now();
      auto const old_number_of_killed    = _word_graph.number_of_nodes_killed();
      auto       killed_at_prev_interval = old_number_of_killed;

      bool       old_ticker_running = _ticker_running;
      time_point start_time         = std::chrono::high_resolution_clock::now();
      Ticker     ticker;

      node_type&   current = _word_graph.lookahead_cursor();
      size_t const n       = _word_graph.out_degree();

      while (current != _word_graph.first_free_node()
             && (!stop_early || (running() && !stopped()) || !running())) {
        // See the comment in make_compatible about why we have !stop_early
        // here. This might never be used but is here for consistency.
        _word_graph.definitions().clear();
        for (size_t a = 0; a < n; ++a) {
          _word_graph.definitions().emplace_back(current, a);
        }
        _word_graph.process_definitions();
        current = _word_graph.next_active_node(current);
        _stats.lookahead_position++;
        if (lookahead_stop_early(
                stop_early, last_stop_early_check, killed_at_prev_interval)) {
          break;
        }
        if (!_ticker_running && reporting_enabled()
            && delta(start_time) >= std::chrono::seconds(1)) {
          _ticker_running = true;
          ticker([this]() { report_progress_from_thread(ye_print_divider); });
        }
      }
      _stats.lookahead_nodes_killed
          += (_word_graph.number_of_nodes_killed() - killed_at_prev_interval);
      _ticker_running = old_ticker_running;
    }
  }  // namespace detail
}  // namespace libsemigroups
