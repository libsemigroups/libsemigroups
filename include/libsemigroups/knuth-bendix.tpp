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
    size_t operator()(Rule const*                                 AB,
                      Rule const*                                 BC,
                      internal_string_type::const_iterator const& it) {
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
    size_t operator()(Rule const*                                 AB,
                      Rule const*                                 BC,
                      internal_string_type::const_iterator const& it) {
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
    size_t operator()(Rule const*                                 AB,
                      Rule const*                                 BC,
                      internal_string_type::const_iterator const& it) {
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
    // TODO experiment with starting size to optimise speed.
    batch_size                = 128;
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
    delete _overlap_measure;
    switch (p) {
      case options::overlap::ABC:
        _overlap_measure = new ABC();
        break;
      case options::overlap::AB_BC:
        _overlap_measure = new AB_BC();
        break;
      case options::overlap::MAX_AB_BC:
        _overlap_measure = new MAX_AB_BC();
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
  KnuthBendix<Rewriter, ReductionOrder>::KnuthBendix(congruence_kind knd)
      : CongruenceInterface(knd),
        _settings(),
        _stats(),
        _rewriter(),
        _gen_pairs_initted(),
        _gilman_graph(),
        _internal_is_same_as_external(),
        _overlap_measure(),
        _presentation() {
    init(knd);
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::init(congruence_kind knd) {
    _rewriter.init();

    CongruenceInterface::init(knd);
    _settings.init();
    _stats.init();

    _gen_pairs_initted = false;
    _gilman_graph.init(0, 0);
    _internal_is_same_as_external = false;
    _presentation.init();
    overlap_policy(_settings.overlap_policy);
    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::KnuthBendix(KnuthBendix const& that)
      : KnuthBendix(that.kind()) {
    *this = that;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::KnuthBendix(KnuthBendix&& that)
      : KnuthBendix(static_cast<KnuthBendix const&>(that)) {}

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::operator=(KnuthBendix&& that) {
    *this = static_cast<KnuthBendix const&>(that);
    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::operator=(KnuthBendix const& that) {
    Runner::operator=(that);
    _rewriter = that._rewriter;

    _settings                     = that._settings;
    _gilman_graph                 = that._gilman_graph;
    _internal_is_same_as_external = that._internal_is_same_as_external;
    _presentation                 = that._presentation;

    overlap_policy(_settings.overlap_policy);

    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>::~KnuthBendix() {
    delete _overlap_measure;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::init(
      congruence_kind                  knd,
      Presentation<std::string> const& p) {
    return private_init(knd, p, true);
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::init(congruence_kind             knd,
                                              Presentation<std::string>&& p) {
    return private_init(knd, std::move(p), true);
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::private_init(
      congruence_kind                  knd,
      Presentation<std::string> const& p,
      bool                             call_init) {
    p.validate();
    if (call_init) {
      init(knd);
    }
    _presentation = p;
    init_from_presentation();
    return *this;
  }

  template <typename Rewriter, typename ReductionOrder>
  KnuthBendix<Rewriter, ReductionOrder>&
  KnuthBendix<Rewriter, ReductionOrder>::private_init(
      congruence_kind             knd,
      Presentation<std::string>&& p,
      bool                        call_init) {
    p.validate();
    if (call_init) {
      init(knd);
    }
    _presentation = std::move(p);
    init_from_presentation();
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - attributes - public
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  uint64_t KnuthBendix<Rewriter, ReductionOrder>::number_of_classes() {
    // TODO uncomment
    // if (is_obviously_infinite(*this)) {
    //   return POSITIVE_INFINITY;
    // }

    int const modifier = (presentation().contains_empty_word() ? 0 : -1);
    if (presentation().alphabet().empty()) {
      return 1 + modifier;
    } else {
      uint64_t const out = knuth_bendix::normal_forms(*this).count();
      return out;
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  bool KnuthBendix<Rewriter, ReductionOrder>::equal_to(std::string const& u,
                                                       std::string const& v) {
    presentation().validate_word(u.cbegin(), u.cend());
    presentation().validate_word(v.cbegin(), v.cend());

    if (u == v) {
      return true;
    }

    external_string_type uu = u;
    external_string_type vv = v;

    if (kind() == congruence_kind::left) {
      std::reverse(uu.begin(), uu.end());
      std::reverse(vv.begin(), vv.end());
    }

    add_octo(uu);
    add_octo(vv);

    rewrite_inplace(uu);
    rewrite_inplace(vv);

    if (uu == vv) {
      return true;
    }

    run();
    external_to_internal_string(uu);
    external_to_internal_string(vv);
    _rewriter.rewrite(uu);
    _rewriter.rewrite(vv);
    return uu == vv;
  }

  template <typename Rewriter, typename ReductionOrder>
  std::string
  KnuthBendix<Rewriter, ReductionOrder>::normal_form(std::string const& w) {
    presentation().validate_word(w.cbegin(), w.cend());
    run();
    return rewrite(w);
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
        // FIXME these are mostly 0, and should be obtained from the rewriter
        // probably
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
      external_string_type& w) {
    if (kind() == congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
    if (_rewriter.number_of_active_rules() == 0
        && _rewriter.number_of_pending_rules() != 0) {
      _rewriter.process_pending_rules();
    }
    add_octo(w);
    external_to_internal_string(w);
    _rewriter.rewrite(w);
    internal_to_external_string(w);
    rm_octo(w);
    if (kind() == congruence_kind::left) {
      std::reverse(w.begin(), w.end());
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendix - other methods - private
  //////////////////////////////////////////////////////////////////////////

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::throw_if_started() const {
    if (started()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the presentation cannot be changed after Knuth-Bendix has "
          "started, maybe try `init` instead?");
    }
  }

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

  // TODO should this check for 0 active rules?
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
  [[nodiscard]] bool
  KnuthBendix<Rewriter, ReductionOrder>::stop_running() const {
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
    auto  pairs = (generating_pairs() | rx::transform([&p](auto const& w) {
                    return to_string(p, w);
                  }))
                 | rx::in_groups_of_exactly(2);

    if (kind() != congruence_kind::twosided && (pairs | rx::count()) != 0) {
      p.alphabet(p.alphabet() + presentation::first_unused_letter(p));
    }
    for (auto&& x : pairs) {
      auto lhs = x.get();
      add_octo(lhs);
      x.next();
      auto rhs = x.get();
      add_octo(rhs);
      add_rule_impl(lhs, rhs);
      presentation::add_rule(p, lhs, rhs);
    }
  }

  // TODO (When the rewriters have a pointer to the KB instance) move this into
  // the rewriter
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
        Rule const* rule1 = *first;
        // It is tempting to remove rule1 and rule2 here and use *first and
        // *second instead but this leads to some badness (which we didn't
        // understand, but it also didn't seem super important).
        second = first++;
        overlap(rule1, rule1);
        while (second != _rewriter.begin() && rule1->active()) {
          --second;
          Rule const* rule2 = *second;
          overlap(rule1, rule2);
          ++nr;
          if (rule1->active() && rule2->active()) {
            ++nr;
            overlap(rule2, rule1);
          }
        }

        if (nr > _settings.check_confluence_interval) {
          pause = true;
          // Checking confluence requires there to be no pending rules which, in
          // general, isn't the case at this point in the loop (other than when
          // nr is a common multiple of batch_size and
          // confluence_check_interval). Therefore, it might make sense to
          // process any remaining rules before checking confluence. However,
          // this seems to worsen performance on the test cases, so it remains
          // to see what the best option is for default behaviour.
          // TODO should we process rules here too?
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
        LIBSEMIGROUPS_ASSERT(knuth_bendix::is_reduced(*this));
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
            _gilman_graph.set_target(p.second, i, it->second);
          } else {
            auto t = s;
            _rewriter.rewrite(t);
            if (t == s) {
              while (!s.empty()) {
                s  = std::string(s.begin() + 1, s.end());
                it = prefixes.find(s);
                if (it != prefixes.end()) {
                  _gilman_graph.set_target(p.second, i, it->second);
                  break;
                }
              }
            }
          }
        }
      }
      if (kind() != congruence_kind::twosided
          && (generating_pairs() | rx::count()) != 0) {
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
      internal_char_type c) {
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(c >= 97);
    return static_cast<size_t>(c - 97);
#else
    return static_cast<size_t>(c - 1);
#endif
  }

  template <typename Rewriter, typename ReductionOrder>
  typename KnuthBendix<Rewriter, ReductionOrder>::internal_char_type
  KnuthBendix<Rewriter, ReductionOrder>::uint_to_internal_char(size_t a) {
    LIBSEMIGROUPS_ASSERT(
        a <= size_t(std::numeric_limits<internal_char_type>::max()));
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(
        a <= size_t(std::numeric_limits<internal_char_type>::max() - 97));
    return static_cast<internal_char_type>(a + 97);
#else
    return static_cast<internal_char_type>(a + 1);
#endif
  }

  template <typename Rewriter, typename ReductionOrder>
  typename KnuthBendix<Rewriter, ReductionOrder>::internal_string_type
  KnuthBendix<Rewriter, ReductionOrder>::uint_to_internal_string(size_t i) {
    LIBSEMIGROUPS_ASSERT(
        i <= size_t(std::numeric_limits<internal_char_type>::max()));
    return internal_string_type({uint_to_internal_char(i)});
  }

  template <typename Rewriter, typename ReductionOrder>
  word_type KnuthBendix<Rewriter, ReductionOrder>::internal_string_to_word(
      internal_string_type const& s) {
    word_type w;
    w.reserve(s.size());
    for (internal_char_type const& c : s) {
      w.push_back(internal_char_to_uint(c));
    }
    return w;
  }

  template <typename Rewriter, typename ReductionOrder>
  typename KnuthBendix<Rewriter, ReductionOrder>::internal_char_type
  KnuthBendix<Rewriter, ReductionOrder>::external_to_internal_char(
      external_char_type c) const {
    LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
    return uint_to_internal_char(presentation().index(c));
  }

  template <typename Rewriter, typename ReductionOrder>
  typename KnuthBendix<Rewriter, ReductionOrder>::external_char_type
  KnuthBendix<Rewriter, ReductionOrder>::internal_to_external_char(
      internal_char_type a) const {
    LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
    return presentation().letter_no_checks(internal_char_to_uint(a));
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::external_to_internal_string(
      external_string_type& w) const {
    if (_internal_is_same_as_external) {
      return;
    }
    for (auto& a : w) {
      a = external_to_internal_char(a);
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::internal_to_external_string(
      internal_string_type& w) const {
    if (_internal_is_same_as_external) {
      return;
    }
    for (auto& a : w) {
      a = internal_to_external_char(a);
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::add_octo(
      external_string_type& w) const {
    if (kind() != congruence_kind::twosided
        && (generating_pairs() | rx::count()) != 0) {
      w = presentation().alphabet().back() + w;
    }
  }

  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::rm_octo(
      external_string_type& w) const {
    if (kind() != congruence_kind::twosided
        && (generating_pairs() | rx::count()) != 0) {
      LIBSEMIGROUPS_ASSERT(w.front() == presentation().alphabet().back());
      w.erase(w.begin());
    }
  }

  //////////////////////////////////////////////////////////////////////////
  // KnuthBendixImpl - methods for rules - private
  //////////////////////////////////////////////////////////////////////////

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
      if (kind() == congruence_kind::left) {
        std::reverse(lhs.begin(), lhs.end());
        std::reverse(rhs.begin(), rhs.end());
      }
      add_rule_impl(lhs, rhs);
    }
  }

  // OVERLAP_2 from Sims, p77
  template <typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Rewriter, ReductionOrder>::overlap(Rule const* u,
                                                      Rule const* v) {
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

        if (_rewriter.number_of_pending_rules() >= _settings.batch_size) {
          _rewriter.process_pending_rules();
        }
        // It can be that the iterator `it` is invalidated by the call to
        // proccess_pending_rule (i.e. if `u` is deactivated, then rewritten,
        // actually changed, and reactivated) and that is the reason for the
        // checks in the for-loop above. If this is the case, then we should
        // stop considering the overlaps of u and v here, and note that they
        // will be considered later, because when the rule `u` is reactivated it
        // is added to the end of the active rules list.

        // TODO remove some of the above checks, since now rules don't get
        // processed after being added.
      }
    }
  }

  namespace knuth_bendix {
    // We are computing non_trivial_classes with respect to kb2 (the greater
    // congruence, with fewer classes)
    //
    // This should work ok if kb1 and kb2 represent different kinds of
    // congruence.
    template <typename Rewriter, typename ReductionOrder>
    std::vector<std::vector<std::string>>
    non_trivial_classes(KnuthBendix<Rewriter, ReductionOrder>& kb1,
                        KnuthBendix<Rewriter, ReductionOrder>& kb2) {
      using rx::operator|;

      // It is intended that kb1 is defined using the same presentation as kb2
      // and some additional rules. The output might still be meaningful if
      // this is not the case.
      if (kb2.number_of_classes() == POSITIVE_INFINITY
          && kb1.number_of_classes() != POSITIVE_INFINITY) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument defines an infinite semigroup, and the 2nd "
            "argument defines a finite semigroup, so there is at least one "
            "infinite non-trivial class!");
      } else if (kb2.presentation().alphabet()
                 != kb1.presentation().alphabet()) {
        // It might be possible to handle this case too,
        // but doesn't seem worth it at present
        LIBSEMIGROUPS_EXCEPTION("the arguments must have presentations with "
                                "the same alphabets, found {} and {}",
                                kb2.presentation().alphabet(),
                                kb1.presentation().alphabet());
      }

      // We construct the WordGraph `ad` obtained by subtracting all of the
      // edges from the Gilman graph of kb1 from the Gilman graph of kb2. The
      // non-trivial classes are finite if and only if `ad` is acyclic. It
      // would be possible to do this without actually constructing `ad` but
      // constructing `ad` is simpler, and so we do that for now.

      auto g2 = kb2.gilman_graph();
      auto g1 = kb1.gilman_graph();

      LIBSEMIGROUPS_ASSERT(g2.number_of_nodes() > 0);
      LIBSEMIGROUPS_ASSERT(g1.number_of_nodes() > 0);

      if (g2.number_of_nodes() < g1.number_of_nodes()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the Gilman digraph of the 1st argument must have at least as "
            "many "
            "nodes as the Gilman digraph of the 2nd argument, found {} nodes "
            "and {} nodes",
            g2.number_of_nodes(),
            g1.number_of_nodes());
      }

      // We need to obtain a mappings from the nodes of
      // g2 to g1 and vice versa.

      using node_type = typename decltype(g2)::node_type;

      std::vector<node_type> to_g1(g2.number_of_nodes(),
                                   static_cast<node_type>(UNDEFINED));
      to_g1[0] = 0;
      std::vector<node_type> to_g2(g1.number_of_nodes(),
                                   static_cast<node_type>(UNDEFINED));
      to_g2[0] = 0;
      for (auto v : g2.nodes()) {
        for (auto e : g2.labels()) {
          auto ve2 = g2.target_no_checks(v, e);
          if (to_g1[v] != UNDEFINED && ve2 != UNDEFINED) {
            auto ve1 = g1.target_no_checks(to_g1[v], e);
            if (ve1 != UNDEFINED && to_g1[ve2] == UNDEFINED) {
              to_g1[ve2] = ve1;
              to_g2[ve1] = ve2;
            }
          }
        }
      }

      // We do a depth first search simultaneously for cycles, and edges E in
      // g2 not in g1. Pre order forcycle detection, post order for "can we
      // reach a node incident to an edge in E" and "number of paths through a
      // node is infinite"
      size_t const N = g2.number_of_nodes();
      // can_reach[v] == true if there is a path from v to a node incident to
      // an edge in g2 that's not in g1.
      std::vector<bool> can_reach(N, false);
      std::vector<bool> inf_paths(N, false);
      std::vector<bool> seen(N, false);

      std::stack<node_type> stck;
      stck.push(0);

      while (!stck.empty()) {
        auto v = stck.top();
        stck.pop();
        if (v >= N) {
          // post order
          v -= N;
          for (auto e : g2.labels()) {
            auto ve = g2.target_no_checks(v, e);
            if (ve != UNDEFINED) {
              can_reach[v] = (can_reach[v] || can_reach[ve]);
              if (can_reach[ve]) {
                inf_paths[v] = inf_paths[ve];
              }
              if (can_reach[v] && inf_paths[v]) {
                LIBSEMIGROUPS_EXCEPTION(
                    "there is an infinite non-trivial class!");
              }
            }
          }
        } else {
          seen[v] = true;
          // so we can tell when all of the descendants of v have been
          // processed out of the stack
          stck.push(v + N);
          if (to_g1[v] == UNDEFINED) {
            can_reach[v] = true;
          }
          for (auto e : g2.labels()) {
            auto ve2 = g2.target_no_checks(v, e);
            if (ve2 != UNDEFINED) {
              // Check if (v, e, ve2) corresponds to an edge in g1
              if (!can_reach[v]) {
                auto ve1 = g1.target_no_checks(to_g1[v], e);
                if (ve1 != UNDEFINED) {
                  // edges (v, e, ve2) and (to_g1[v], e, ve1) exist, so
                  // there's an edge in g2 not in g1 if the targets of these
                  // edges do not correspond to each other.
                  can_reach[v] = (ve2 != to_g2[ve1]);
                } else {
                  // There's no edge labelled by e incident to the node
                  // corresponding to v in g1, but there is such an edge in g2
                  // and so (v, e, ve2) is in g2 but not g1.
                  can_reach[v] = true;
                }
              }
              if (seen[ve2]) {
                // cycle detected
                inf_paths[v] = true;
              } else {
                stck.push(ve2);
              }
            }
          }
        }
      }

      // If we reach here, then the appropriate portion of g2 is acyclic, and
      // so all we do is enumerate the paths in that graph

      // Construct the "can_reach" subgraph of g2, could use a WordGraphView
      // here instead (but these don't yet exist) TODO(later)
      WordGraph<size_t> ad(g2.number_of_nodes(), g2.out_degree());

      for (auto v : ad.nodes()) {
        if (can_reach[v]) {
          for (auto e : ad.labels()) {
            auto ve = g2.target_no_checks(v, e);
            if (ve != UNDEFINED && can_reach[ve]) {
              ad.set_target_no_checks(v, e, ve);
            }
          }
        }
      }

      Paths paths(ad);
      // We only want those paths that pass through at least one of the edges
      // in g2 but not g1. Hence we require the `filter` in the next
      // expression.
      auto ntc
          = partition(kb1,
                      (paths.from(0) | rx::filter([&g1](word_type const& path) {
                         return word_graph::last_node_on_path(
                                    g1, 0, path.cbegin(), path.cend())
                                    .second
                                != path.cend();
                       })
                       | ToStrings(kb2.presentation().alphabet())));
      // The check in the next loop could be put into the lambda passed to
      // filter above, but then we'd have to convert `path` to a string, and
      // then discard the string, so better to do it here. Note that the
      // normal forms in `kb1` never contain an edge in g2 \ g1 and so we must
      // add in every normal form.
      for (auto& klass : ntc) {
        klass.push_back(kb1.normal_form(klass[0]));
      }
      return ntc;
    }

    template <typename Rewriter, typename ReductionOrder>
    void by_overlap_length(KnuthBendix<Rewriter, ReductionOrder>& kb) {
      size_t prev_max_overlap               = kb.max_overlap();
      size_t prev_check_confluence_interval = kb.check_confluence_interval();
      kb.max_overlap(1);
      kb.check_confluence_interval(POSITIVE_INFINITY);

      while (!kb.confluent()) {
        kb.run();
        kb.max_overlap(kb.max_overlap() + 1);
      }
      kb.max_overlap(prev_max_overlap);
      kb.check_confluence_interval(prev_check_confluence_interval);
    }

  }  // namespace knuth_bendix

  template <typename Rewriter, typename ReductionOrder>
  std::ostream& operator<<(std::ostream&                          os,
                           KnuthBendix<Rewriter, ReductionOrder>& kb) {
    os << kb.active_rules();
    return os;
  }
}  // namespace libsemigroups
