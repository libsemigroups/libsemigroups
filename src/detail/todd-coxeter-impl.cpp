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
  namespace {
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
  }  // namespace

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
    size_t ToddCoxeterImpl::Graph::make_compatible(ToddCoxeterImpl* tc,
                                                   node_type&       current,
                                                   RuleIterator     first,
                                                   RuleIterator     last,
                                                   bool stop_early) {
      auto last_stop_early_check = std::chrono::high_resolution_clock::now();
      size_t const old_number_of_killed    = number_of_nodes_killed();
      size_t       killed_at_prev_interval = old_number_of_killed;

      CollectCoincidences                    incompat(_coinc);
      typename FelschGraph_::NoPreferredDefs prefdefs;

      bool           old_ticker_running = tc->_ticker_running;
      auto           start_time = std::chrono::high_resolution_clock::now();
      detail::Ticker ticker;

      while (current != NodeManager<node_type>::first_free_node()) {
        // TODO(1) when we have an RuleIterator into the active nodes, we
        // should remove the while loop, and use that in make_compatible
        // instead. At present there is a cbegin/cend_active_nodes in
        // NodeManager but the RuleIterators returned by them are invalidated
        // by any changes to the graph, such as those made by
        // felsch_graph::make_compatible.
        detail::felsch_graph::make_compatible<detail::DoNotRegisterDefs>(
            *this, current, current + 1, first, last, incompat, prefdefs);
        // Using NoPreferredDefs is just a (more or less) arbitrary
        // choice, could allow the other choices here too (which works,
        // but didn't seem to be very useful).
        process_coincidences<detail::DoNotRegisterDefs>();
        current = NodeManager<node_type>::next_active_node(current);
        if (stop_early
            && delta(last_stop_early_check)
                   > tc->lookahead_stop_early_interval()) {
          size_t killed_last_interval
              = number_of_nodes_killed() - killed_at_prev_interval;
          killed_at_prev_interval = number_of_nodes_killed();
          auto expected           = static_cast<size_t>(
              number_of_nodes_active() * tc->lookahead_stop_early_ratio());
          if (killed_last_interval < expected) {
            report_lookahead_stop_early(tc, expected, killed_last_interval);
            break;
          }
          last_stop_early_check = std::chrono::high_resolution_clock::now();
          if (!tc->_ticker_running && reporting_enabled()
              && delta(start_time) > std::chrono::seconds(1)) {
            tc->_ticker_running = true;
            ticker([this]() { report_progress_from_thread(); });
          }
        }
      }
      tc->_ticker_running = old_ticker_running;
      return NodeManager<node_type>::number_of_nodes_killed();
    }

    void ToddCoxeterImpl::Graph::report_lookahead_stop_early(
        ToddCoxeterImpl* tc,
        size_t           expected,
        size_t           killed_last_interval) {
      if (reporting_enabled()) {
        auto gd = detail::group_digits;
        auto interval
            = detail::string_time(tc->lookahead_stop_early_interval());
        report_no_prefix("{:-<90}\n", "");
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
                                 tc->lookahead_stop_early_ratio()),
                     fmt::format("{} = lookahead_stop_early_interval() = {}\n",
                                 italic("i"),
                                 interval),
                     fmt::format("{} = number_of_nodes_active()        = {}\n",
                                 italic("a"),
                                 gd(number_of_nodes_active()))});
      }
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
          _ticker_running(),
          _word_graph() {
      init();
    }

    ToddCoxeterImpl& ToddCoxeterImpl::init() {
      detail::CongruenceCommon::init();
      report_prefix("ToddCoxeter");
      _finished = false;
      _forest.init();
      reset_settings_stack();
      _standardized   = Order::none;
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
      detail::CongruenceCommon::operator=(std::move(that));
      _finished       = std::move(that._finished);
      _forest         = std::move(that._forest);
      _settings_stack = std::move(that._settings_stack);
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
      detail::CongruenceCommon::operator=(that);
      _finished = that._finished;
      _forest   = that._forest;
      _settings_stack.clear();
      for (auto const& uptr : that._settings_stack) {
        _settings_stack.push_back(std::make_unique<Settings>(*uptr));
      }
      _standardized   = that._standardized;
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
      if (!finished()) {
        return;
      }
      standardize(Order::shortlex);
      _word_graph.erase_free_nodes();
      _word_graph.induced_subgraph_no_checks(
          0, _word_graph.number_of_nodes_active());
    }

    bool ToddCoxeterImpl::standardize(Order val) {
      using time_point = std::chrono::high_resolution_clock::time_point;
      if (is_standardized(val)) {
        return false;
      }
      time_point start_time;
      if (reporting_enabled()) {
        start_time = std::chrono::high_resolution_clock::now();
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
                     detail::string_time(delta(start_time)));
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
      }

      init_run();

      if (reporting_enabled()
          && (!running_for()
              || duration_cast<seconds>(running_for_how_long())
                     >= seconds(1))) {
        report_before_run();
        _ticker_running = true;
        detail::Ticker t(
            [this]() { current_word_graph().report_progress_from_thread(); });
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
          // We require this clause because we might still be obviously
          // infinite and running_for a specific amount of time, in which case
          // we are never finished. This is an issue when the input
          // presentation has no rules for example.
          _finished = true;
        }
      }
      report_after_run();
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
          // already run process_definitions, and so there's no point in doing
          // a lookahead.
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

    void ToddCoxeterImpl::report_after_lookahead(
        size_t old_lookahead_next,
        size_t number_killed_in_lookahead,
        std::chrono::high_resolution_clock::time_point lookahead_start_time)
        const {
      auto gd = detail::group_digits;
      using detail::signed_group_digits;

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

        auto l      = number_killed_in_lookahead;
        auto l_name = italic("l");
        auto l_key  = fmt::format(
            "{} = nodes killed in lookahead    = {}\n", l_name, gd(l));

        auto m      = lookahead_min();
        auto m_name = italic("m");
        auto m_key  = fmt::format(
            "{} = lookahead_min()              = {}\n", m_name, gd(m));

        std::set<std::string> keys;

        std::string reason
            = fmt_default("ToddCoxeter: lookahead_next() is now ");

        if (a * lgf < oln || a > oln) {
          reason += fmt::format("max({} x {} = {}, {} = {})\n",
                                lgf_name,
                                a_name,
                                gd(lgf * a),
                                m_name,
                                gd(m));
          if (a * lgf < oln) {
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
          reason += fmt::format(
              "{} x {} = {}\n", oln_name, lgf_name, gd(oln * lgf));

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
          reason += fmt::format("{}\n", gd(ln));
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

        int64_t const diff = static_cast<int64_t>(ln) - oln;

        report_no_prefix("{:+<90}\n", "");
        report_default("ToddCoxeter: lookahead complete with    |{:>12} "
                       "(active) |{:>12} (diff)\n",
                       gd(a),
                       gd(-l));
        report_default("ToddCoxeter: after                      |{:>12} "
                       "(time)   |{:>12} (total)\n",
                       detail::string_time(delta(lookahead_start_time)),
                       detail::string_time(delta(start_time())));
        if (!finished()) {
          report_default(
              "ToddCoxeter: next lookahead at          |{:>12} (nodes)  "
              "|{:>12} (diff)\n",
              gd(ln),
              signed_group_digits(diff));
          report_no_prefix("{:+<90}\n", "");
          report_no_prefix(reason);
          report_keys(keys);
        }

        report_no_prefix("{:+<90}\n", "");
      }
    }

    void ToddCoxeterImpl::report_after_run() const {
      if (reporting_enabled()) {
        report_no_prefix("{:+<90}\n", "");
        report_default("ToddCoxeter: STOPPING ({}) --- ",
                       detail::string_time(delta(start_time())));
        if (finished()) {
          report_no_prefix("finished!\n");
        } else if (dead()) {
          report_no_prefix("killed!\n");
        } else if (timed_out()) {
          report_no_prefix("timed out!\n");
        } else if (stopped_by_predicate()) {
          report_no_prefix("stopped by predicate!\n");
        }
        report_no_prefix("{:+<90}\n", "");
        // TODO(1) report time spent doing lookaheads, definitions, etc.
      }
    }

    void ToddCoxeterImpl::report_before_lookahead() const {
      using detail::group_digits;
      if (reporting_enabled()) {
        report_no_prefix("{:+<90}\n", "");
        report_default("ToddCoxeter: performing {} {} lookahead, triggered at "
                       "{} . . .\n",
                       lookahead_extent(),
                       lookahead_style(),
                       detail::string_time(delta(start_time())));
        if (current_word_graph().number_of_nodes_active() > lookahead_next()) {
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

        } else if (current_word_graph().definitions().any_skipped()) {
          report_default(
              "ToddCoxeter: because of skipped definitions ({} active "
              "nodes)!\n",
              group_digits(current_word_graph().number_of_nodes_active()));
        }

        report_no_prefix("{:+<90}\n", "");
      }
    }

    void ToddCoxeterImpl::report_before_run() const {
      if (reporting_enabled()) {
        report_no_prefix("{:+<90}\n", "");
        report_default("ToddCoxeter: STARTING --- ");
        report_strategy();
        report_no_prefix("{:+<90}\n", "");
        report_presentation();
      }
    }

    void ToddCoxeterImpl::report_presentation() const {
      report_default("ToddCoxeter: {}",
                     presentation::to_report_string(internal_presentation()));
    }

    void ToddCoxeterImpl::report_strategy() const {
      // TODO(1) if using ACE style strategy include the value of the relevant
      // setting
      // TODO(1) add more nuance when not using hlt/felsch
      report_no_prefix("using {} strategy . . .\n", strategy());
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - lookahead - private
    ////////////////////////////////////////////////////////////////////////

    void ToddCoxeterImpl::perform_lookahead(bool stop_early) {
      report_before_lookahead();
      std::chrono::high_resolution_clock::time_point lookahead_start_time
          = std::chrono::high_resolution_clock::now();

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

      size_t const num_nodes = _word_graph.number_of_nodes_active();

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
      } else if (num_killed_by_me < ((num_nodes + num_killed_by_me)
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
      report_after_lookahead(
          old_lookahead_next, num_killed_by_me, lookahead_start_time);
    }

    size_t ToddCoxeterImpl::hlt_lookahead(bool stop_early) {
      size_t const old_number_of_killed = _word_graph.number_of_nodes_killed();
      _word_graph.make_compatible(this,
                                  _word_graph.lookahead_cursor(),
                                  internal_presentation().rules.cbegin(),
                                  internal_presentation().rules.cend(),
                                  stop_early);
      return _word_graph.number_of_nodes_killed() - old_number_of_killed;
    }

    size_t ToddCoxeterImpl::felsch_lookahead() {
      using time_point = std::chrono::high_resolution_clock::time_point;

      size_t const old_number_of_killed = _word_graph.number_of_nodes_killed();
      node_type&   current              = _word_graph.lookahead_cursor();
      size_t const n                    = _word_graph.out_degree();

      bool           old_ticker_running = _ticker_running;
      detail::Ticker ticker;
      time_point     lookahead_start_time
          = std::chrono::high_resolution_clock::now();

      while (current != _word_graph.first_free_node()) {
        _word_graph.definitions().clear();
        for (size_t a = 0; a < n; ++a) {
          _word_graph.definitions().emplace_back(current, a);
        }
        _word_graph.process_definitions();
        current = _word_graph.next_active_node(current);
        if (!_ticker_running && reporting_enabled()
            && delta(lookahead_start_time) >= std::chrono::seconds(1)) {
          _ticker_running = true;
          ticker(
              [this]() { current_word_graph().report_progress_from_thread(); });
        }
      }
      // TODO(1) stop early?
      _ticker_running = old_ticker_running;
      return _word_graph.number_of_nodes_killed() - old_number_of_killed;
    }

  }  // namespace detail
}  // namespace libsemigroups
