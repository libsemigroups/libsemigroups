//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

namespace libsemigroups {
  namespace detail {
    // Implemented in cpp file
    void prefixes_string(std::unordered_map<Rule::native_word_type, size_t>& st,
                         Rule::native_word_type const&                       x,
                         size_t&                                             n);

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix::Settings - constructor - public
    //////////////////////////////////////////////////////////////////////////

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::Settings::Settings() noexcept {
      init();
    }

    template <typename RewritingSystem>
    typename KnuthBendixImpl<RewritingSystem>::Settings&
    KnuthBendixImpl<RewritingSystem>::Settings::init() noexcept {
      max_overlap    = POSITIVE_INFINITY;
      max_rules      = POSITIVE_INFINITY;
      overlap_policy = options::overlap::ABC;
      return *this;
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::Stats::Stats() noexcept {
      init();
    }

    template <typename RewritingSystem>
    typename KnuthBendixImpl<RewritingSystem>::Stats&
    KnuthBendixImpl<RewritingSystem>::Stats::init() noexcept {
      prev_active_rules   = 0;
      prev_inactive_rules = 0;
      prev_total_rules    = 0;
      return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - setters for Settings - public
    //////////////////////////////////////////////////////////////////////////

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>&
    KnuthBendixImpl<RewritingSystem>::overlap_policy(
        typename options::overlap p) {
      if (p == _settings.overlap_policy && _overlap_measure != nullptr) {
        return *this;
      }
      switch (p) {
        case options::overlap::ABC:
          _overlap_measure.reset(new ABC());
          break;
        case options::overlap::AB_BC:
          _overlap_measure.reset(new AB_BC());
          break;
        case options::overlap::MAX_AB_BC:
          _overlap_measure.reset(new MAX_AB_BC());
          break;
        default:
          LIBSEMIGROUPS_ASSERT(false);
      }
      _settings.overlap_policy = p;
      return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - constructors and destructor - public
    //////////////////////////////////////////////////////////////////////////

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::KnuthBendixImpl()
        : CongruenceCommon(),
          _gen_pairs_initted(),
          _gilman_graph(),
          _gilman_graph_node_labels(),
          _overlap_measure(nullptr),
          _presentation(),
          _rewriting_system(),
          _settings(),
          _stats(),
          _ticker_running(false),
          _tmp_element1() {
      init();
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>& KnuthBendixImpl<RewritingSystem>::init() {
      CongruenceCommon::init();
      report_prefix("KnuthBendix");

      _gen_pairs_initted = false;
      _gilman_graph.init(0, 0);
      _gilman_graph_node_labels.clear();
      _overlap_measure = nullptr;
      _presentation.init();
      _rewriting_system.init();
      _settings.init();
      _stats.init();
      _ticker_running = false;

      // The next line sets _overlap_measure to be something sensible.
      overlap_policy(_settings.overlap_policy);
      return *this;
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>&
    KnuthBendixImpl<RewritingSystem>::operator=(KnuthBendixImpl const& that) {
      CongruenceCommon::operator=(that);
      _gen_pairs_initted        = that._gen_pairs_initted;
      _gilman_graph             = that._gilman_graph;
      _gilman_graph_node_labels = that._gilman_graph_node_labels;
      _overlap_measure          = nullptr;
      _presentation             = that._presentation;
      _rewriting_system         = that._rewriting_system;
      _settings                 = that._settings;
      _stats                    = that._stats;
      _ticker_running           = that._ticker_running;

      // The next line sets _overlap_measure to be something sensible.
      overlap_policy(_settings.overlap_policy);

      return *this;
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>&
    KnuthBendixImpl<RewritingSystem>::operator=(KnuthBendixImpl&& that) {
      CongruenceCommon::operator=(std::move(that));
      _gen_pairs_initted        = std::move(that._gen_pairs_initted);
      _gilman_graph             = std::move(that._gilman_graph);
      _gilman_graph_node_labels = std::move(that._gilman_graph_node_labels);
      _overlap_measure          = std::move(that._overlap_measure);
      _presentation             = std::move(that._presentation);
      _rewriting_system         = std::move(that._rewriting_system);
      _settings                 = std::move(that._settings);
      _stats                    = std::move(that._stats);
      _ticker_running           = std::move(that._ticker_running);
      return *this;
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::KnuthBendixImpl(KnuthBendixImpl&& that)
        : KnuthBendixImpl() {
      operator=(std::move(that));
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::KnuthBendixImpl(
        KnuthBendixImpl const& that)
        : KnuthBendixImpl() {
      operator=(that);
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::~KnuthBendixImpl() = default;

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::KnuthBendixImpl(
        congruence_kind                  knd,
        Presentation<native_word_type>&& p)
        : KnuthBendixImpl() {
      init(knd, std::move(p));
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>&
    KnuthBendixImpl<RewritingSystem>::init(congruence_kind                  knd,
                                           Presentation<native_word_type>&& p) {
      static_assert(
          !order::is_stateful_v<typename RewritingSystem::reduction_order>,
          "a KnuthBendix object with a stateful reduction_order must be "
          "initialised by specifying an instance of a reduction_order.");
      return init(knd, p, reduction_order());
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::KnuthBendixImpl(
        congruence_kind                       knd,
        Presentation<native_word_type> const& p)
        : KnuthBendixImpl() {
      init(knd, p);
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>& KnuthBendixImpl<RewritingSystem>::init(
        congruence_kind                       knd,
        Presentation<native_word_type> const& p) {
      // Call rvalue ref init
      return init(knd, Presentation(p));
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::KnuthBendixImpl(
        congruence_kind                       knd,
        Presentation<native_word_type> const& p,
        reduction_order const&                order)
        : KnuthBendixImpl() {
      init(knd, p, order);
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>& KnuthBendixImpl<RewritingSystem>::init(
        congruence_kind                       knd,
        Presentation<native_word_type> const& p,
        reduction_order const&                order) {
      // Call rvalue ref init
      return init(knd, Presentation(p), reduction_order(order));
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>::KnuthBendixImpl(
        congruence_kind                  knd,
        Presentation<native_word_type>&& p,
        reduction_order&&                order)
        : KnuthBendixImpl() {
      init(knd, std::move(p), std::move(order));
    }

    template <typename RewritingSystem>
    KnuthBendixImpl<RewritingSystem>&
    KnuthBendixImpl<RewritingSystem>::init(congruence_kind                  knd,
                                           Presentation<native_word_type>&& p,
                                           reduction_order&& order) {
      // TODO(1) assert that the alphabet + rules are good
      // p.throw_if_bad_alphabet_or_rules();
      LIBSEMIGROUPS_ASSERT(presentation::is_normalized(p));
      init();
      kind(knd);
      _rewriting_system.order() = std::move(order);
      _presentation             = std::move(p);
      init_from_internal_presentation();
      return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - attributes - public
    //////////////////////////////////////////////////////////////////////////

    template <typename RewritingSystem>
    uint64_t KnuthBendixImpl<RewritingSystem>::number_of_classes() {
      if (is_obviously_infinite(*this)) {
        return POSITIVE_INFINITY;
      }

      int const modifier
          = (internal_presentation().contains_empty_word() ? 0 : -1);
      if (internal_presentation().alphabet().empty()) {
        return 1 + modifier;
      } else {
        uint64_t result = v4::paths::count(gilman_graph(), 0);
        return result == POSITIVE_INFINITY ? result : result + modifier;
      }
    }

    template <typename RewritingSystem>
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril KnuthBendixImpl<RewritingSystem>::currently_contains_no_checks(
        Iterator1 first1,
        Iterator2 last1,
        Iterator3 first2,
        Iterator4 last2) const {
      if (std::equal(first1, last1, first2, last2)) {
        return tril::TRUE;
      }
      // TODO(1) remove allocations here, can't use _tmp_element1 here because
      // it's used in reduce
      native_word_type w1, w2;
      reduce_no_run_no_checks(std::back_inserter(w1), first1, last1);
      reduce_no_run_no_checks(std::back_inserter(w2), first2, last2);
      if (w1 == w2) {
        return tril::TRUE;
      } else if (finished()
                 || (internal_presentation().rules.empty()
                     && internal_generating_pairs().empty())) {
        return tril::FALSE;
      }
      return tril::unknown;
    }

    template <typename RewritingSystem>
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator KnuthBendixImpl<RewritingSystem>::reduce_no_run_no_checks(
        OutputIterator d_first,
        InputIterator1 first,
        InputIterator2 last) const {
      // TODO(1) improve this to not require _tmp_element1
      if constexpr (std::is_same_v<InputIterator1, char const*>) {
        static_assert(std::is_same_v<InputIterator2, char const*>);
        _tmp_element1.assign(first, std::distance(first, last));
      } else {
        _tmp_element1.assign(first, last);
      }
      const_cast<KnuthBendixImpl<RewritingSystem>&>(*this).rewrite_inplace(
          _tmp_element1);
      return std::copy(
          std::begin(_tmp_element1), std::end(_tmp_element1), d_first);
    }

    // TODO(1) export a version of this for use elsewhere
    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::report_presentation() const {
      // NOTE: this function does the same as presentation::to_report_string,
      // which we cannot use directly here because we don't have a presentation
      // object to pass to it (and possibly because of some cyclic dependency
      // that this would introduce).
      size_t min = POSITIVE_INFINITY, max = 0, len = 0;
      for (auto const& rule : _rewriting_system.rules()) {
        auto rule_len = rule.first.size() + rule.second.size();
        len += rule_len;
        min = (rule_len < min ? rule_len : min);
        max = (rule_len > max ? rule_len : max);
      }
      if (min == POSITIVE_INFINITY) {
        min = 0;
      }

      report_default("KnuthBendix: |A| = {}, |R| = {}, "
                     "|u| + |v| \u2208 [{}, {}], \u2211(|u| + |v|) = {}\n",
                     internal_presentation().alphabet().size(),
                     group_digits(_rewriting_system.number_of_rules()),
                     group_digits(min),
                     group_digits(max),
                     group_digits(len));
    }

    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::report_before_run() {
      if (reporting_enabled()) {
        report_no_prefix("{:+<95}\n", "");
        report_default("KnuthBendix: STARTING . . .\n");
        report_no_prefix("{:+<95}\n", "");
        report_presentation();
      }
    }

    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::report_progress_from_thread() {
      using std::chrono::duration_cast;
      using std::chrono::high_resolution_clock;

      auto active  = _rewriting_system.number_of_rules();
      auto defined = _rewriting_system.stats().total_rules;

      int64_t const active_diff  = active - _stats.prev_active_rules;
      int64_t const defined_diff = defined - _stats.prev_total_rules;

      auto run_time = duration_cast<nanoseconds>(high_resolution_clock::now()
                                                 - start_time());
      auto const mean_defined
          = group_digits(std::pow(10, 9) * static_cast<double>(defined)
                         / run_time.count())
            + "/s";

      ReportCell<4> rc;
      rc.min_width(12);
      rc("KnuthBendix: rules {} (active) | {} (defined)\n",
         group_digits(active),
         group_digits(defined));

      rc("KnuthBendix: diff  {} (active) | {} (defined)\n",
         signed_group_digits(active_diff),
         signed_group_digits(defined_diff));

      rc("KnuthBendix: time  {} (total)  | {} (defined)\n",
         string_time(run_time),
         mean_defined);

      stats_check_point();
    }

    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::report_after_run() {
      if (reporting_enabled()) {
        report_progress_from_thread();
        if (finished()) {
          ReportCell<2> rc;
          rc.min_width(12);
          rc("KnuthBendix: RUN STATISTICS\n");
          rc("KnuthBendix: max number of pending rules {}\n",
             group_digits(_rewriting_system.stats().max_pending_rules));
          rc("KnuthBendix: max length lhs rule         {}\n",
             group_digits(_rewriting_system.stats().max_length_lhs_rule));
        }

        report_no_prefix("{:-<95}\n", "");
        report_presentation();

        report_no_prefix("{:+<95}\n", "");
        report_default("KnuthBendix: STOPPING -- ");

        if (finished()) {
          report_no_prefix("finished!\n");
        } else if (dead()) {
          report_no_prefix("killed!\n");
        } else if (timed_out()) {
          report_no_prefix("timed out!\n");
        } else if (stopped_by_predicate()) {
          report_no_prefix("stopped by predicate!\n");
        } else {
          report_no_prefix("max. overlap length of {} reached!\n",
                           max_overlap());
        }
        report_no_prefix("{:+<95}\n", "");
      }
    }

    // report_no_prefix(msg);
    // REVIEW was it okay to remove const here? Needed to do so to maybe process
    // some rules.
    template <typename RewritingSystem>
    void
    KnuthBendixImpl<RewritingSystem>::rewrite_inplace(native_word_type& w) {
      add_octo(w);
      _rewriting_system.rewrite(w);
      rm_octo(w);
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - other methods - private
    //////////////////////////////////////////////////////////////////////////

    // TODO(1) move to Stats
    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::stats_check_point() {
      _stats.prev_active_rules = _rewriting_system.number_of_rules();
      _stats.prev_total_rules  = _rewriting_system.stats().total_rules;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - main methods - public
    //////////////////////////////////////////////////////////////////////////

    template <typename RewritingSystem>
    bool KnuthBendixImpl<RewritingSystem>::finished_impl() const {
      return _rewriting_system.confluent_known()
             && _rewriting_system.confluent();
    }

    template <typename RewritingSystem>
    bool KnuthBendixImpl<RewritingSystem>::stop_running() const {
      return stopped()
             || _rewriting_system.number_of_rules() > _settings.max_rules;
    }

    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::init_from_generating_pairs() {
      if (_gen_pairs_initted) {
        return;
      }
      _gen_pairs_initted = true;

      auto& p     = _presentation;
      auto& pairs = internal_generating_pairs();

      if (kind() == congruence_kind::onesided && !pairs.empty()) {
        LIBSEMIGROUPS_ASSERT(
            p.alphabet().size()
            < std::numeric_limits<typename native_word_type::value_type>::max()
                  - std::numeric_limits<
                      typename native_word_type::value_type>::min());
        p.alphabet(p.alphabet()
                   + static_cast<typename native_word_type::value_type>(
                       p.alphabet().size()));
        _rewriting_system.increase_alphabet_size_by(1);
      }

      for (auto it = pairs.cbegin(); it != pairs.cend(); ++it) {
        // it points at a word_type
        p.rules.emplace_back(it->cbegin(), it->cend());

        add_octo(p.rules.back());
        ++it;
        p.rules.emplace_back(it->cbegin(), it->cend());
        add_octo(p.rules.back());
        rewriting_system::add_rule(
            _rewriting_system, p.rules.cend()[-2], p.rules.cend()[-1]);
      }
    }

    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::run_impl() {
      using std::chrono::duration_cast;
      using std::chrono::seconds;

      stats_check_point();
      reset_start_time();

      init_from_generating_pairs();
      if (_rewriting_system.confluent() && !stop_running()) {
        // _rewriting_system._pending_rules can be non-empty if non-reduced
        // rules were used to define the KnuthBendixImpl.  If
        // _rewriting_system._pending_rules is non-empty, then it means that the
        // rules in _rewriting_system might not define the system.
        report_default("KnuthBendix: the system is confluent already!\n");
        return;
      } else if (_rewriting_system.number_of_rules() >= max_rules()) {
        report_default(
            "KnuthBendix: too many rules, found {}, max_rules() is {}\n",
            _rewriting_system.number_of_rules(),
            max_rules());
        return;
      }

      Ticker     ticker;
      ValueGuard tg(_ticker_running);
      if (!_ticker_running && reporting_enabled()
          && (!running_for()
              || duration_cast<seconds>(running_for_how_long())
                     >= seconds(1))) {
        _ticker_running = true;
        ticker([this]() { report_progress_from_thread(); });
      }

      report_before_run();

      // TODO(1) re-add overlap iterator stuff
      _rewriting_system.reduce();
      auto& first  = _rewriting_system.cursor(0);
      auto& second = _rewriting_system.cursor(1);
      first.it     = _rewriting_system.active_rules().begin();
      // TODO try sorting active_rules?

      // All rules in _rewriting_system are in 1 of 3 states:
      //
      // 1. active (current part of the system)
      // 2. pending (maybe part of the system in the future)
      // 3. inactive (currently unused)
      //
      // When an overlap is detected (by calls to "overlap" below), sometimes a
      // rule will be sent from the active list to the pending list. Its fate is
      // determined at a later point (in "reduce"), and it will either be
      // re-added to the active list (at the end of
      // _rewriting_system.active_rules()), or to the inactive list (position
      // unimportant).
      //
      // The aim of the loops below is to consider all overlaps between active
      // rules **while those active rules are changing**. The cursors "first"
      // and "second" always point at valid active rules, and have the
      // following behaviour with respect to removal of active rules (those
      // sent from the active to the pending list).
      //
      // Let "it" be an iterator pointing at the rule "r_{i}" being sent from
      // the active to the pending list and let "r_{i-1}" and "r_{i+1}" be
      // the active rules before and immediate after "r_{i}" in the active rule
      // list (if any):
      //
      //    ... -- [ r_{i - 1} ] -- [ r_{i} ] -- [ r_{i + 1} ] -- ...
      //                              ^
      //                              |
      //                              it
      //
      // Then after the "r_{i}" is sent from the active to the pending list:
      //
      //    ... -- [ r_{i - 1} ] -- [ r_{i + 1} ] -- [ r_{i + 2} ] -- ...
      //                              ^
      //                              |
      //                              new_it
      //
      // where "new_it = _active_rules.erase(it)" (this is std::list::erase
      // which returns an iterator pointing one beyond the erased iterator)
      //
      // The cursors "first" and "last" are updated as follows:
      //
      // A if neither cursor coincides with "it", then the cursors are left
      //   unaltered.
      //
      // B if "second" and "it" coincide, then "second" is set to "new_it"
      //
      //    ... -- [ r_{i - 1} ] -- [ r_{i + 1} ] -- [ r_{i + 2} ] -- ...
      //                              ^
      //                              |
      //                              second
      //
      // C if "first" and "it" coincide and "new_it" is _active_rules.begin(),
      //   then "first" is set to "new_it" == _active_rules.begin().
      //
      //   [ r_{i + 1} ] -- [ r_{i + 2} ] -- ...
      //     ^
      //     |
      //     first
      //
      //    NOTE this happens if r_{i} (the rule being sent from active to
      //    pending) is the first rule in _active_rules.
      //
      // D if "first" and "it" coincide and "new_it" is not
      //   _active_rules.begin(), then "first" is set to "--new_it". This is
      //   the same as the rule before "it" (and "first") before "it" was
      //   erased.
      //
      //    ... -- [ r_{i - 1} ] -- [ r_{i + 1} ] -- [ r_{i + 2} ] -- ...
      //             ^
      //             |
      //             first
      do {
        // Within this do-loop, active rules may become pending, but pending
        // rules can only become active/inactive at the call to
        // _rewriting_system.reduce() in the condition evaluated at the end of
        // the outer while-loop. In other words, the active rules list may
        // shrink but not grow when inside this do-loop.
        //
        // The cursor "first" never needs to be re-initialised, because
        // although active_rules() can grow and shrink. If it grows, then the
        // new rules are added at the end, and so "first" will equal any
        // surviving new rule eventually. If it shrinks, then we no longer care
        // about comparing the removed rules to anything.
        while (first.it != _rewriting_system.active_rules().end()
               && !stop_running()) {
          auto first_version = first.version;
          // We assume that we have overlapped between all rules strictly
          // preceding "first" in the active rules list. This is true at the
          // start because there are no such rules.
          overlap(*first.it, *first.it);
          if (first.version != first_version) {
            // In this case, either C or D above applies.
            //
            // If C applies, then after the "continue" below and the for-loop
            // where "second" is defined would be:
            //
            //   [ r_{i + 1} ] -- [ r_{i + 2} ] -- ...
            //     ^                ^
            //     |                |
            //     second           first
            //
            // if we incremented "first". In this case we would have missed
            // the possible overlap of r_{i + 1} with itself. This is the
            // reason we do not increment "first" if C applies.

            if (first.it != _rewriting_system.active_rules().begin()) {
              // In all of the KnuthBendix test cases this line is always
              // executed, i.e. the rule r_{i} being sent from active to
              // pending is never the first rule in these tests.
              ++first.it;
            }

            // If D applies, then after the "continue" below and the for-loop
            // where "second" is defined is entered:
            //
            //    ... -- [ r_{i - 1} ] -- [ r_{i + 1} ] -- ...
            //             ^                ^
            //             |                |
            //             second           first
            //
            // We have already overlapped r_{i - 1} and all previous rules in
            // the list, so no overlaps can be missed.

            continue;
          }

          for (second = first;
               second.it != _rewriting_system.active_rules().begin();) {
            --second.it;
            auto second_version = second.version;
            overlap(*first.it, *second.it);
            if (first.version != first_version) {
              // In this case, either C or D above applies. This being the
              // case, means that we end up in the same exact situation as
              // above, because we break out below we don't need to consider
              // what happens to "second". Note that C cannot apply because
              // "second" is strictly before "first" so "first" cannot be
              // pointing at the beginning of _active_rules.
              break;
            } else if (second.version != second_version) {
              // In this case B, applies and so after the "continue" below:
              //
              //    ... -- [ r_{i - 1} ] -- [ r_{i + 1} ] -- ... -- [ r_{j} ]
              //             ^                                        ^
              //             |                                        |
              //             second                                   first
              //
              // Note that in this case r_{j} is not being erased, and so
              // "first" is not modified, and "second" assumes the next value it
              // would have at the end of the loop. No overlaps are missed.
              continue;
            }
            overlap(*second.it, *first.it);
            if (first.version != first_version) {
              // In this case, either C or D above applies. This being the
              // case, means that we end up in the same exact situation as
              // above, because we break out below we don't need to consider
              // what happens to "second". Note that C cannot apply because
              // "second" is strictly before "first" so "first" cannot be
              // pointing at the beginning of _active_rules.
              break;
            }
            // Note that if second != second_orig here, then we'd want to
            // continue, but because we are now at the end of this for-loop,
            // this is the same as doing nothing.
          }
          ++first.it;
        }
      } while (!stop_running() && _rewriting_system.reduce());

      if (_settings.max_overlap == POSITIVE_INFINITY
          && _settings.max_rules == POSITIVE_INFINITY && !stop_running()) {
        _rewriting_system.set_cached_confluent(tril::TRUE);
      }

      report_after_run();
    }

    template <typename RewritingSystem>
    WordGraph<uint32_t> const&
    KnuthBendixImpl<RewritingSystem>::gilman_graph() {
      if (_gilman_graph.number_of_nodes() == 0
          && !internal_presentation().alphabet().empty()) {
        // TODO(1) should implement a SettingsGuard as in ToddCoxeterImpl
        // reset the settings so that we really run!
        max_rules(POSITIVE_INFINITY);
        run();
        LIBSEMIGROUPS_ASSERT(finished());
        LIBSEMIGROUPS_ASSERT(_rewriting_system.confluent());
        std::unordered_map<Rule::native_word_type, size_t> prefixes;
        prefixes.emplace(Rule::native_word_type(), 0);
        size_t n = 1;
        for (auto const& rule : _rewriting_system.rules()) {
          prefixes_string(prefixes, rule.first, n);
        }

        _gilman_graph_node_labels.resize(prefixes.size(),
                                         Rule::native_word_type());
        for (auto const& p : prefixes) {
          _gilman_graph_node_labels[p.second] = p.first;
        }

        _gilman_graph.add_nodes(prefixes.size());
        _gilman_graph.add_to_out_degree(
            internal_presentation().alphabet().size());

        for (auto& p : prefixes) {
          for (auto i : internal_presentation().alphabet()) {
            auto s  = p.first + native_word_type({i});
            auto it = prefixes.find(s);
            if (it != prefixes.end()) {
              _gilman_graph.target(p.second, i, it->second);
            } else {
              auto t = s;
              _rewriting_system.rewrite(t);
              if (t == s) {
                while (!s.empty()) {
                  s  = native_word_type(s.begin() + 1, s.end());
                  it = prefixes.find(s);
                  if (it != prefixes.end()) {
                    _gilman_graph.target(p.second, i, it->second);
                    break;
                  }
                }
              }
            }
          }
        }
        if (kind() != congruence_kind::twosided
            && !internal_generating_pairs().empty()) {
          auto const& p    = internal_presentation();
          auto        octo = p.index(p.alphabet().back());
          auto        src  = _gilman_graph.target_no_checks(0, octo);
          LIBSEMIGROUPS_ASSERT(src != UNDEFINED);
          _gilman_graph.remove_label_no_checks(octo);
          auto nodes = v4::word_graph::nodes_reachable_from(_gilman_graph, src);
          LIBSEMIGROUPS_ASSERT(std::find(nodes.cbegin(), nodes.cend(), src)
                               != nodes.cend());
          // This is a bit awkward, it exists to ensure
          // that node 0 in the induced subdigraph is src.
          std::vector<decltype(src)> sorted_nodes(nodes.cbegin(), nodes.cend());
          // The order which nodes come out of nodes_reachable_from is
          // non-deterministic and so we sort the nodes
          std::sort(sorted_nodes.begin(), sorted_nodes.end());
          if (sorted_nodes[0] != src) {
            std::iter_swap(
                sorted_nodes.begin(),
                std::find(sorted_nodes.begin(), sorted_nodes.end(), src));
          }

          _gilman_graph.induced_subgraph_no_checks(sorted_nodes.cbegin(),
                                                   sorted_nodes.cend());
        }
      }
      return _gilman_graph;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - converting ints <-> string/char - private
    //////////////////////////////////////////////////////////////////////////

    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::add_octo(native_word_type& w) const {
      if (kind() != congruence_kind::twosided
          && !internal_generating_pairs().empty()) {
        w = internal_presentation().alphabet().back() + w;
      }
    }

    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::rm_octo(native_word_type& w) const {
      if (kind() != congruence_kind::twosided
          && !internal_generating_pairs().empty()) {
        LIBSEMIGROUPS_ASSERT(w.front()
                             == internal_presentation().alphabet().back());
        w.erase(w.begin());
      }
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendixImpl - methods for rules - private
    //////////////////////////////////////////////////////////////////////////

    // TODO(1) move this to the single call site
    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::init_from_internal_presentation() {
      auto const& p = _presentation;

      _rewriting_system.increase_alphabet_size_by(p.alphabet().size());

      auto const first = p.rules.cbegin();
      auto const last  = p.rules.cend();
      for (auto it = first; it != last; it += 2) {
        auto lhs = *it, rhs = *(it + 1);
        _rewriting_system.add_rule(
            lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
      }
    }

    // OVERLAP_2 from Sims, p77
    // TODO(1) move to elsewhere
    template <typename RewritingSystem>
    void KnuthBendixImpl<RewritingSystem>::overlap(Rule const* u,
                                                   Rule const* v) {
      constexpr const auto active = Rule::State::active;

      LIBSEMIGROUPS_ASSERT(u->state() == active);
      LIBSEMIGROUPS_ASSERT(v->state() == active);

      native_word_type const& ulhs = u->lhs();
      native_word_type const& vlhs = v->lhs();
      native_word_type const& urhs = u->rhs();
      native_word_type const& vrhs = v->rhs();

      auto const lower_limit = ulhs.cend() - std::min(ulhs.size(), vlhs.size());

      for (auto it = ulhs.cend() - 1;
           it > lower_limit && u->state() == active && v->state() == active
           && !stop_running()
           && (_settings.max_overlap == POSITIVE_INFINITY
               || (*_overlap_measure)(u, v, it) <= _settings.max_overlap);
           --it) {
        // Check if B = [it, ulhs.cend()) is a prefix of v.first
        // TODO(1) use std::mismatch instead
        if (is_prefix(vlhs.cbegin(), vlhs.cend(), it, ulhs.cend())) {
          // u = P_i = AB -> Q_i and v = P_j = BC -> Q_j This version of
          // new_rule does not reorder _rewriting_system.add_rule(AQ_j,
          // Q_iC);
          MultiView<native_word_type> x(ulhs.cbegin(), it);
          x.append(vrhs.cbegin(), vrhs.cend());
          MultiView<native_word_type> y(urhs.cbegin(), urhs.cend());
          y.append(vlhs.cbegin() + (ulhs.cend() - it),
                   vlhs.cend());  // rule = AQ_j -> Q_iC
          _rewriting_system.add_rule(x.begin(), x.end(), y.begin(), y.end());
        }
      }
    }
  }  // namespace detail

  template <typename RewritingSystem>
  std::ostream& operator<<(std::ostream&                             os,
                           detail::KnuthBendixImpl<RewritingSystem>& kb) {
    os << kb.rewriting_system().rules();
    return os;
  }

  template <typename RewritingSystem>
  std::string
  to_human_readable_repr(detail::KnuthBendixImpl<RewritingSystem>& kb) {
    using detail::group_digits;
    std::string conf, genpairs;
    if (kb.rewriting_system().confluent_known()) {
      conf = "confluent ";
      if (!kb.rewriting_system().confluent()) {
        conf = "non-" + conf;
      }
    }

    return fmt::format(
        "<{}{} KnuthBendix over {} with {} gen. pair{}, {} rule{}>",
        conf,
        kb.kind() == congruence_kind::twosided ? "2-sided" : "1-sided",
        to_human_readable_repr(kb.internal_presentation()),
        group_digits(kb.number_of_generating_pairs()),
        kb.number_of_generating_pairs() == 1 ? "" : "s",
        group_digits(kb.rewriting_system().number_of_rules()),
        kb.rewriting_system().number_of_rules() == 1 ? "" : "s");
  }

}  // namespace libsemigroups
