//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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
    static inline void
    prefixes_string(std::unordered_map<std::string, size_t>& st,
                    std::string const&                       x,
                    size_t&                                  n) {
      for (auto it = x.cbegin() + 1; it < x.cend(); ++it) {
        auto w   = std::string(x.cbegin(), it);
        auto wit = st.find(w);
        if (wit == st.end()) {
          st.emplace(w, n);
          n++;
        }
      }
    }

  }  // namespace detail

  template <typename Rewriter, typename ReductionOrder>
  struct KnuthBendix<Rewriter, ReductionOrder>::ABC
      : KnuthBendix<Rewriter, ReductionOrder>::OverlapMeasure {
    size_t operator()(detail::Rule const*                                 AB,
                      detail::Rule const*                                 BC,
                      detail::internal_string_type::const_iterator const& it) {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      // |A| + |BC|
      return (it - AB->lhs()->cbegin()) + BC->lhs()->size();
    }
  };

  template <typename Rewriter, typename ReductionOrder>
  struct KnuthBendix<Rewriter, ReductionOrder>::AB_BC
      : KnuthBendix<Rewriter, ReductionOrder>::OverlapMeasure {
    size_t operator()(detail::Rule const*                                 AB,
                      detail::Rule const*                                 BC,
                      detail::internal_string_type::const_iterator const& it) {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      (void) it;
      // |AB| + |BC|
      return AB->lhs()->size() + BC->lhs()->size();
    }
  };

  template <typename Rewriter, typename ReductionOrder>
  struct KnuthBendix<Rewriter, ReductionOrder>::MAX_AB_BC
      : KnuthBendix<Rewriter, ReductionOrder>::OverlapMeasure {
    size_t operator()(detail::Rule const*                                 AB,
                      detail::Rule const*                                 BC,
                      detail::internal_string_type::const_iterator const& it) {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      (void) it;
      // max(|AB|, |BC|)
      return std::max(AB->lhs()->size(), BC->lhs()->size());
    }
  };

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix::Settings - constructor - public
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::Settings::Settings() noexcept {
    init();
  }

  template <typename Rewriter, typename ReductionOrder>
  typename KnuthBendix<Rewriter, ReductionOrder>::Settings&
  KnuthBendix<Rewriter, ReductionOrder>::Settings::init() noexcept {
    // TODO(1) experiment with starting size to optimise speed.
    max_pending_rules         = 128;
    check_confluence_interval = 4'096;
    max_overlap               = POSITIVE_INFINITY;
    max_rules                 = POSITIVE_INFINITY;
    overlap_policy            = options::overlap::ABC;
    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::Stats::Stats() noexcept {
    init();
  }

  template <typename Rewriter, typename ReductionOrder>
  typename KnuthBendix<Rewriter, ReductionOrder>::Stats&
  KnuthBendix<Rewriter, ReductionOrder>::Stats::init() noexcept {
    prev_active_rules   = 0;
    prev_inactive_rules = 0;
    prev_total_rules    = 0;
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - setters for Settings - public
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::overlap_policy(
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
  // KnuthBendix - constructors and destructor - public
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::KnuthBendix()
      : CongruenceInterface(),
        _gen_pairs_initted(),
        _gilman_graph(),
        _gilman_graph_node_labels(),
        _internal_is_same_as_external(),
        _overlap_measure(nullptr),
        _presentation(),
        _rewriter(),
        _settings(),
        _stats(),
        _tmp_element1() {
    init();
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::init() {
    CongruenceInterface::init();

    _gen_pairs_initted = false;
    _gilman_graph.init(0, 0);
    _gilman_graph_node_labels.clear();
    _internal_is_same_as_external = false;
    _overlap_measure              = nullptr;
    _presentation.init();
    _rewriter.init();
    _settings.init();
    _stats.init();

    // The next line sets _overlap_measure to be something sensible.
    overlap_policy(_settings.overlap_policy);
    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::operator=(KnuthBendix&& that) {
    CongruenceInterface::operator=(std::move(that));
    _gen_pairs_initted        = std::move(that._gen_pairs_initted);
    _gilman_graph             = std::move(that._gilman_graph);
    _gilman_graph_node_labels = std::move(that._gilman_graph_node_labels);
    _internal_is_same_as_external
        = std::move(that._internal_is_same_as_external);
    _overlap_measure = std::move(that._overlap_measure);
    _presentation    = std::move(that._presentation);
    _rewriter        = std::move(that._rewriter);
    _settings        = std::move(that._settings);
    _stats           = std::move(that._stats);
    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::operator=(KnuthBendix const& that) {
    CongruenceInterface::operator=(that);
    _gen_pairs_initted            = that._gen_pairs_initted;
    _gilman_graph                 = that._gilman_graph;
    _gilman_graph_node_labels     = that._gilman_graph_node_labels;
    _internal_is_same_as_external = that._internal_is_same_as_external;
    _overlap_measure              = nullptr;
    _presentation                 = that._presentation;
    _rewriter                     = that._rewriter;
    _settings                     = that._settings;
    _stats                        = that._stats;

    // The next line sets _overlap_measure to be something sensible.
    overlap_policy(_settings.overlap_policy);

    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::KnuthBendix(KnuthBendix&& that)
      : KnuthBendix() {
    operator=(std::move(that));
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::KnuthBendix(KnuthBendix const& that)
      : KnuthBendix() {
    operator=(that);
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::~KnuthBendix() = default;

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::KnuthBendix(
      congruence_kind             knd,
      Presentation<std::string>&& p)
      : KnuthBendix() {
    init(knd, std::move(p));
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::init(congruence_kind             knd,
                                              Presentation<std::string>&& p) {
    p.validate();
    init();
    CongruenceInterface::init(knd);
    _presentation = std::move(p);
    init_from_presentation();
    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::KnuthBendix(
      congruence_kind                  knd,
      Presentation<std::string> const& p)
      : KnuthBendix() {
    init(knd, p);
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::init(
      congruence_kind                  knd,
      Presentation<std::string> const& p) {
    // Call rvalue ref init
    return init(knd, Presentation(p));
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - attributes - public
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  uint64_t KnuthBendix<Rewriter, ReductionOrder>::number_of_classes() {
    if (is_obviously_infinite(*this)) {
      return POSITIVE_INFINITY;
    }

    int const modifier = (presentation().contains_empty_word() ? 0 : -1);
    if (presentation().alphabet().empty()) {
      return 1 + modifier;
    } else {
      uint64_t result = number_of_paths(gilman_graph(), 0);
      return result == POSITIVE_INFINITY ? result : result + modifier;
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  tril KnuthBendix<Rewriter, ReductionOrder>::currently_contains_no_checks(
      Iterator1 first1,
      Iterator2 last1,
      Iterator3 first2,
      Iterator4 last2) const {
    if (std::equal(first1, last1, first2, last2)) {
      return tril::TRUE;
    }
    // TODO(1) remove allocations here
    std::string w1, w2;
    reduce_no_run_no_checks(std::back_inserter(w1), first1, last1);
    reduce_no_run_no_checks(std::back_inserter(w2), first2, last2);
    if (w1 == w2) {
      return tril::TRUE;
    } else if (finished()) {
      return tril::FALSE;
    }
    return tril::unknown;
  }

  template <typename Rewriter, typename ReductionOrder>
  template <typename OutputIterator,
            typename InputIterator1,
            typename InputIterator2>
  OutputIterator KnuthBendix<Rewriter, ReductionOrder>::reduce_no_run_no_checks(
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
    const_cast<KnuthBendix<Rewriter, ReductionOrder>&>(*this).rewrite_inplace(
        _tmp_element1);
    return std::copy(
        std::begin(_tmp_element1), std::end(_tmp_element1), d_first);
  }

  template <typename Rewriter, typename ReductionOrder>
  auto KnuthBendix<Rewriter, ReductionOrder>::active_rules() {
    using rx::iterator_range;
    using rx::transform;
    if (_rewriter.number_of_active_rules() == 0
        && _rewriter.number_of_pending_rules() != 0) {
      _rewriter.process_pending_rules();
    }
    return iterator_range(_rewriter.begin(), _rewriter.end())
           | transform([this](auto const& rule) {
               // TODO(1) remove allocation
               detail::internal_string_type lhs
                   = detail::internal_string_type(*rule->lhs());
               detail::internal_string_type rhs
                   = detail::internal_string_type(*rule->rhs());
               internal_to_external_string(lhs);
               internal_to_external_string(rhs);
               return std::make_pair(lhs, rhs);
             });
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::report_presentation(
      Presentation<std::string> const& p) const {
    using detail::group_digits;
    auto shortest_short = presentation::shortest_rule_length(presentation());
    auto longest_short  = presentation::longest_rule_length(presentation());

    report_default("KnuthBendix: |A| = {}, |R| = {}, "
                   "|u| + |v| \u2208 [{}, {}], \u2211(|u| + |v|) = {}\n",
                   p.alphabet().size(),
                   group_digits(p.rules.size() / 2),
                   shortest_short,
                   longest_short,
                   group_digits(presentation::length(p)));
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::report_before_run() {
    if (reporting_enabled()) {
      report_no_prefix("{:+<95}\n", "");
      report_default("KnuthBendix: STARTING . . .\n");
      report_no_prefix("{:+<95}\n", "");
      // Required so that we can use the current presentation and not the
      // initial one.
      auto p = to_presentation<std::string>(*this);
      report_presentation(p);
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::report_progress_from_thread(
      std::atomic_bool const& pause) {
    using detail::group_digits;
    using detail::signed_group_digits;
    using std::chrono::duration_cast;

    using high_resolution_clock = std::chrono::high_resolution_clock;
    using nanoseconds           = std::chrono::nanoseconds;

    if (!pause) {
      auto active   = number_of_active_rules();
      auto inactive = number_of_inactive_rules();
      auto defined  = _rewriter.stats().total_rules;

      int64_t const active_diff   = active - _stats.prev_active_rules;
      int64_t const inactive_diff = inactive - _stats.prev_inactive_rules;
      int64_t const defined_diff  = defined - _stats.prev_total_rules;

      auto run_time = duration_cast<nanoseconds>(high_resolution_clock::now()
                                                 - start_time());
      auto const mean_defined
          = group_digits(std::pow(10, 9) * static_cast<double>(defined)
                         / run_time.count())
            + "/s";
      auto const mean_killed
          = group_digits(std::pow(10, 9) * static_cast<double>(inactive)
                         / run_time.count())
            + "/s";

      detail::ReportCell<4> rc;
      rc.min_width(12);  // .divider("{:-<95}\n");
      rc("KnuthBendix: rules {} (active) | {} (inactive) | {} (defined)\n",
         group_digits(active),
         group_digits(inactive),
         group_digits(defined));

      rc("KnuthBendix: diff  {} (active) | {} (inactive) | {} (defined)\n",
         signed_group_digits(active_diff),
         signed_group_digits(inactive_diff),
         signed_group_digits(defined_diff));

      rc("KnuthBendix: time  {} (total)  | {} (killed)   | {} (defined)\n",
         detail::string_time(run_time),
         mean_killed,
         mean_defined);

      stats_check_point();
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::report_after_run() {
    if (reporting_enabled()) {
      report_progress_from_thread(false);
      if (finished()) {
        using detail::group_digits;
        detail::ReportCell<2> rc;
        rc.min_width(12);  // .divider("{:-<95}\n");
        rc("KnuthBendix: RUN STATISTICS\n");
        // rc.divider();
        rc("KnuthBendix: max stack depth        {}\n",
           group_digits(_rewriter.max_stack_depth()));
        rc("KnuthBendix: max rule length        {}\n",
           group_digits(_rewriter.stats().max_word_length));
        rc("KnuthBendix: max active rule length {}\n",
           group_digits(max_active_word_length()));
        // rc("KnuthBendix: number of unique lhs   {}\n",
        //    group_digits(_stats.unique_lhs_rules.size()));
      }

      report_no_prefix("{:-<95}\n", "");
      auto p = to_presentation<std::string>(*this);
      report_presentation(p);

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
        report_no_prefix("max. overlap length of {} reached!\n", max_overlap());
      }
      report_no_prefix("{:+<95}\n", "");
    }
  }

  // report_no_prefix(msg);
  // REVIEW was it okay to remove const here? Needed to do so to maybe process
  // some rules.
  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::rewrite_inplace(
      detail::external_string_type& w) {
    if (_rewriter.number_of_active_rules() == 0
        && _rewriter.number_of_pending_rules() != 0) {
      _rewriter.process_pending_rules();
    }
    add_octo(w);
    external_to_internal_string(w);
    _rewriter.rewrite(w);
    internal_to_external_string(w);
    rm_octo(w);
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - other methods - private
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::stats_check_point() {
    _stats.prev_active_rules   = number_of_active_rules();
    _stats.prev_inactive_rules = number_of_inactive_rules();
    _stats.prev_total_rules    = total_rules();
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - main methods - public
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  bool KnuthBendix<Rewriter, ReductionOrder>::confluent_known() const noexcept {
    return _rewriter.confluence_known();
  }

  template <typename Rewriter, typename ReductionOrder>
  bool KnuthBendix<Rewriter, ReductionOrder>::confluent() const {
    if (_rewriter.number_of_active_rules() == 0
        && _rewriter.number_of_pending_rules() != 0) {
      const_cast<KnuthBendix<Rewriter, ReductionOrder>*>(this)
          ->_rewriter.process_pending_rules();
    }
    return _rewriter.confluent();
  }

  template <typename Rewriter, typename ReductionOrder>
  bool KnuthBendix<Rewriter, ReductionOrder>::finished_impl() const {
    return confluent_known() && confluent();
  }

  template <typename Rewriter, typename ReductionOrder>
  bool KnuthBendix<Rewriter, ReductionOrder>::stop_running() const {
    return stopped()
           || _rewriter.number_of_active_rules() > _settings.max_rules;
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::init_from_generating_pairs() {
    if (_gen_pairs_initted) {
      return;
    }
    _gen_pairs_initted = true;

    auto& p     = _presentation;
    auto& pairs = internal_generating_pairs();

    if (kind() != congruence_kind::twosided && !pairs.empty()) {
      p.alphabet(p.alphabet() + presentation::first_unused_letter(p));
    }

    for (auto it = pairs.cbegin(); it != pairs.cend(); ++it) {
      // it points at a word_type
      p.rules.emplace_back(it->cbegin(), it->cend());
      add_octo(p.rules.back());
      ++it;
      p.rules.emplace_back(it->cbegin(), it->cend());
      add_octo(p.rules.back());
      add_rule_impl(p.rules.cend()[-2], p.rules.cend()[-1]);
    }
  }

  // TODO(1) (When the rewriters have a pointer to the KB instance) move this
  // into the rewriter
  template <typename Rewriter, typename ReductionOrder>
  void
  KnuthBendix<Rewriter, ReductionOrder>::run_real(std::atomic_bool& pause) {
    bool add_overlaps = true;

    auto& first  = _rewriter.cursor(0);
    auto& second = _rewriter.cursor(1);
    first        = _rewriter.begin();

    size_t nr = 0;
    // Add overlaps that occur between rules. Repeat this process until no
    // non-trivial overlaps are added and there are a no pending rules.
    while (add_overlaps) {
      while (first != _rewriter.end() && !stop_running()) {
        detail::Rule const* rule1 = *first;
        // It is tempting to remove rule1 and rule2 here and use *first and
        // *second instead but this leads to some badness (which we didn't
        // understand, but it also didn't seem super important).
        second = first++;
        overlap(rule1, rule1);
        while (second != _rewriter.begin() && rule1->active()) {
          --second;
          detail::Rule const* rule2 = *second;
          overlap(rule1, rule2);
          ++nr;
          if (rule1->active() && rule2->active()) {
            ++nr;
            overlap(rule2, rule1);
          }
        }

        if (nr > _settings.check_confluence_interval) {
          pause = true;
          // Checking confluence requires there to be no pending rules which,
          // in general, isn't the case at this point in the loop (other than
          // when nr is a common multiple of max_pending_rules and
          // confluence_check_interval). Therefore, it might make sense to
          // process any remaining rules before checking confluence. However,
          // this seems to worsen performance on the test cases, so it remains
          // to see what the best option is for default behaviour.
          // TODO(1) should we process rules here too?
          // _rewriter.process_pending_rules();
          if (confluent()) {
            pause = false;
            goto confluence_achieved;
          }
          pause = false;
          nr    = 0;
        }
      }

      if (_rewriter.number_of_pending_rules() != 0) {
        _rewriter.process_pending_rules();
        // is_reduced is in the helper file now so can't check this here
        // anymore LIBSEMIGROUPS_ASSERT(knuth_bendix::is_reduced(*this));
      } else {
        add_overlaps = false;
      }
    }

  confluence_achieved:
    // _rewriter.reduce_rhs();
    LIBSEMIGROUPS_ASSERT(_rewriter.number_of_pending_rules() == 0);

    if (_settings.max_overlap == POSITIVE_INFINITY
        && _settings.max_rules == POSITIVE_INFINITY && !stop_running()) {
      _rewriter.set_cached_confluent(tril::TRUE);
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::run_impl() {
    stats_check_point();
    reset_start_time();

    init_from_generating_pairs();
    _rewriter.process_pending_rules();
    if (_rewriter.consistent() && confluent() && !stop_running()) {
      // _rewriter._pending_rules can be non-empty if non-reduced rules were
      // used to define the KnuthBendix.  If _rewriter._pending_rules is
      // non-empty, then it means that the rules in _rewriter might not define
      // the system.
      report_default("KnuthBendix: the system is confluent already!\n");
      return;
    } else if (_rewriter.number_of_active_rules() >= max_rules()) {
      report_default(
          "KnuthBendix: too many rules, found {}, max_rules() is {}\n",
          _rewriter.number_of_active_rules(),
          max_rules());
      return;
    }

    report_before_run();
    std::atomic_bool pause = false;
    if (reporting_enabled()) {
      detail::Ticker t([&]() { report_progress_from_thread(pause); },
                       report_every());
      run_real(pause);
    } else {
      run_real(pause);
    }

    report_after_run();
  }

  // REVIEW was it okay to remove const here?
  template <typename Rewriter, typename ReductionOrder>
  size_t
  KnuthBendix<Rewriter, ReductionOrder>::number_of_active_rules() noexcept {
    if (_rewriter.number_of_active_rules() == 0
        && _rewriter.number_of_pending_rules() != 0) {
      _rewriter.process_pending_rules();
    }
    return _rewriter.number_of_active_rules();
  }

  template <typename Rewriter, typename ReductionOrder>
  WordGraph<uint32_t> const&
  KnuthBendix<Rewriter, ReductionOrder>::gilman_graph() {
    if (_gilman_graph.number_of_nodes() == 0
        && !presentation().alphabet().empty()) {
      // TODO(1) should implement a SettingsGuard as in ToddCoxeterBase
      // reset the settings so that we really run!
      max_rules(POSITIVE_INFINITY);
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      LIBSEMIGROUPS_ASSERT(confluent());
      std::unordered_map<std::string, size_t> prefixes;
      prefixes.emplace("", 0);
      size_t n = 1;
      for (auto const* rule : _rewriter) {
        detail::prefixes_string(prefixes, *rule->lhs(), n);
      }

      _gilman_graph_node_labels.resize(prefixes.size(), "");
      for (auto const& p : prefixes) {
        _gilman_graph_node_labels[p.second] = p.first;
        internal_to_external_string(_gilman_graph_node_labels[p.second]);
      }

      _gilman_graph.add_nodes(prefixes.size());
      _gilman_graph.add_to_out_degree(presentation().alphabet().size());

      for (auto& p : prefixes) {
        for (size_t i = 0; i < presentation().alphabet().size(); ++i) {
          auto s  = p.first + uint_to_internal_string(i);
          auto it = prefixes.find(s);
          if (it != prefixes.end()) {
            _gilman_graph.target(p.second, i, it->second);
          } else {
            auto t = s;
            _rewriter.rewrite(t);
            if (t == s) {
              while (!s.empty()) {
                s  = std::string(s.begin() + 1, s.end());
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
        auto const& p    = presentation();
        auto        octo = p.index(p.alphabet().back());
        auto        src  = _gilman_graph.target_no_checks(0, octo);
        LIBSEMIGROUPS_ASSERT(src != UNDEFINED);
        _gilman_graph.remove_label_no_checks(octo);
        auto nodes = word_graph::nodes_reachable_from(_gilman_graph, src);
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
  // FpSemigroupInterface - pure virtual methods - private
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  void
  KnuthBendix<Rewriter, ReductionOrder>::add_rule_impl(std::string const& p,
                                                       std::string const& q) {
    if (p == q) {
      return;
    }
    if (_internal_is_same_as_external) {
      _rewriter.add_pending_rule(p, q);
    } else {
      auto pp(p), qq(q);
      // Can't see an alternative to the copy here
      external_to_internal_string(pp);
      external_to_internal_string(qq);
      _rewriter.add_pending_rule(pp, qq);
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendixImpl - converting ints <-> string/char - private
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  size_t KnuthBendix<Rewriter, ReductionOrder>::internal_char_to_uint(
      detail::internal_char_type c) {
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(c >= 97);
    return static_cast<size_t>(c - 97);
#else
    return static_cast<size_t>(c - 1);
#endif
  }

  template <typename Rewriter, typename ReductionOrder>
  typename detail::internal_char_type
  KnuthBendix<Rewriter, ReductionOrder>::uint_to_internal_char(size_t a) {
    // Ensure that the input value doesn't overflow the internal char type,
    // seems legit to me
    // TODO(1) should this be
    // std::numeric_limits<detail::internal_char_type>::max() -
    // std::numeric_limits<detail::internal_char_type>::min()?
    LIBSEMIGROUPS_ASSERT(
        a <= size_t(std::numeric_limits<detail::internal_char_type>::max()));
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(
        a
        <= size_t(std::numeric_limits<detail::internal_char_type>::max() - 97));
    return static_cast<detail::internal_char_type>(a + 97);
#else
    return static_cast<detail::internal_char_type>(a + 1);
#endif
  }

  template <typename Rewriter, typename ReductionOrder>
  typename detail::internal_string_type
  KnuthBendix<Rewriter, ReductionOrder>::uint_to_internal_string(size_t i) {
    // TODO(1) What is this check for?
    // TODO(1) should this be
    // std::numeric_limits<detail::internal_char_type>::max() -
    // std::numeric_limits<detail::internal_char_type>::min()?
    LIBSEMIGROUPS_ASSERT(
        i <= size_t(std::numeric_limits<detail::internal_char_type>::max()));
    return detail::internal_string_type({uint_to_internal_char(i)});
  }

  template <typename Rewriter, typename ReductionOrder>
  word_type KnuthBendix<Rewriter, ReductionOrder>::internal_string_to_word(
      detail::internal_string_type const& s) {
    word_type w;
    w.reserve(s.size());
    for (detail::internal_char_type const& c : s) {
      w.push_back(internal_char_to_uint(c));
    }
    return w;
  }

  template <typename Rewriter, typename ReductionOrder>
  typename detail::internal_char_type
  KnuthBendix<Rewriter, ReductionOrder>::external_to_internal_char(
      detail::external_char_type c) const {
    LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
    return uint_to_internal_char(presentation().index(c));
  }

  template <typename Rewriter, typename ReductionOrder>
  typename detail::external_char_type
  KnuthBendix<Rewriter, ReductionOrder>::internal_to_external_char(
      detail::internal_char_type a) const {
    LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
    return presentation().letter_no_checks(internal_char_to_uint(a));
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::external_to_internal_string(
      detail::external_string_type& w) const {
    if (_internal_is_same_as_external) {
      return;
    }
    for (auto& a : w) {
      a = external_to_internal_char(a);
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::internal_to_external_string(
      detail::internal_string_type& w) const {
    if (_internal_is_same_as_external) {
      return;
    }
    for (auto& a : w) {
      a = internal_to_external_char(a);
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::add_octo(
      detail::external_string_type& w) const {
    if (kind() != congruence_kind::twosided
        && !internal_generating_pairs().empty()) {
      w = presentation().alphabet().back() + w;
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::rm_octo(
      detail::external_string_type& w) const {
    if (kind() != congruence_kind::twosided
        && !internal_generating_pairs().empty()) {
      LIBSEMIGROUPS_ASSERT(w.front() == presentation().alphabet().back());
      w.erase(w.begin());
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendixImpl - methods for rules - private
  //////////////////////////////////////////////////////////////////////////

  // TODO(1) move this to the single call site
  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::init_from_presentation() {
    auto const& p                 = _presentation;
    _internal_is_same_as_external = true;
    for (size_t i = 0; i < p.alphabet().size(); ++i) {
      if (uint_to_internal_char(i) != p.letter_no_checks(i)) {
        _internal_is_same_as_external = false;
        break;
      }
    }

    if (_rewriter.requires_alphabet()) {
      if (_internal_is_same_as_external) {
        for (auto x = p.alphabet().begin(); x != p.alphabet().end(); ++x) {
          _rewriter.add_to_alphabet(*x);
        }
      } else {
        for (auto x = p.alphabet().begin(); x != p.alphabet().end(); ++x) {
          _rewriter.add_to_alphabet(external_to_internal_char(*x));
        }
      }
    }

    auto const first = p.rules.cbegin();
    auto const last  = p.rules.cend();
    for (auto it = first; it != last; it += 2) {
      auto lhs = *it, rhs = *(it + 1);
      add_rule_impl(lhs, rhs);
    }
  }

  // OVERLAP_2 from Sims, p77
  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::overlap(detail::Rule const* u,
                                                      detail::Rule const* v) {
    LIBSEMIGROUPS_ASSERT(u->active() && v->active());
    auto const &ulhs = *(u->lhs()), vlhs = *(v->lhs());
    auto const &urhs = *(u->rhs()), vrhs = *(v->rhs());
    auto const  lower_limit = ulhs.cend() - std::min(ulhs.size(), vlhs.size());

    int64_t const u_id = u->id(), v_id = v->id();
    for (auto it = ulhs.cend() - 1;
         it > lower_limit && u_id == u->id() && v_id == v->id()
         && it < ulhs.cend() && !stop_running()
         && (_settings.max_overlap == POSITIVE_INFINITY
             || (*_overlap_measure)(u, v, it) <= _settings.max_overlap);
         --it) {
      // Check if B = [it, ulhs.cend()) is a prefix of v->lhs()
      if (detail::is_prefix(vlhs.cbegin(), vlhs.cend(), it, ulhs.cend())) {
        // u = P_i = AB -> Q_i and v = P_j = BC -> Q_j This version of
        // new_rule does not reorder _rewriter.add_rule(AQ_j, Q_iC);
        detail::MultiStringView x(ulhs.cbegin(), it);
        x.append(vrhs.cbegin(), vrhs.cend());
        detail::MultiStringView y(urhs.cbegin(), urhs.cend());
        y.append(vlhs.cbegin() + (ulhs.cend() - it),
                 vlhs.cend());  // rule = AQ_j -> Q_iC
        _rewriter.add_pending_rule(x, y);

        if (_rewriter.number_of_pending_rules()
            >= _settings.max_pending_rules) {
          _rewriter.process_pending_rules();
        }
        // It can be that the iterator `it` is invalidated by the call to
        // proccess_pending_rule (i.e. if `u` is deactivated, then rewritten,
        // actually changed, and reactivated) and that is the reason for the
        // checks in the for-loop above. If this is the case, then we should
        // stop considering the overlaps of u and v here, and note that they
        // will be considered later, because when the rule `u` is reactivated it
        // is added to the end of the active rules list.

        // TODO(1) remove some of the above checks, since now rules don't get
        // processed after being added.
      }
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  std::ostream& operator<<(std::ostream&                          os,
                           KnuthBendix<Rewriter, ReductionOrder>& kb) {
    os << kb.active_rules();
    return os;
  }

  template <typename Rewriter, typename ReductionOrder>
  std::string
  to_human_readable_repr(KnuthBendix<Rewriter, ReductionOrder>& kb) {
    std::string conf, genpairs;
    if (kb.confluent_known()) {
      conf = "confluent";
      if (!kb.confluent()) {
        conf = "non-" + conf;
      }
    }
    if (kb.number_of_generating_pairs() != 0) {
      genpairs = fmt::format("{} generating pairs + ",
                             kb.number_of_generating_pairs());
    }

    return fmt::format(
        "<{}{} KnuthBendix over {} with {}{}/{} active/inactive rules>",
        conf,
        kb.kind() == congruence_kind::twosided ? " 2-sided" : " 1-sided",
        to_human_readable_repr(kb.presentation()),
        genpairs,
        kb.number_of_active_rules(),
        kb.number_of_inactive_rules());
  }

  template <typename Word, typename Rewriter, typename ReductionOrder>
  Presentation<Word>
  to_presentation(KnuthBendix<Rewriter, ReductionOrder>& kb) {
    if constexpr (std::is_same_v<Word, std::string>) {
      auto const&               p_orig = kb.presentation();
      Presentation<std::string> p;
      p.alphabet(p_orig.alphabet())
          .contains_empty_word(p_orig.contains_empty_word());

      for (auto const& rule : kb.active_rules()) {
        presentation::add_rule(p, rule.first, rule.second);
      }
      return p;
    } else {
      return to_presentation<Word>(to_presentation<std::string>(kb));
    }
  }
}  // namespace libsemigroups
