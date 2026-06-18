//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Joseph Edwards + James D. Mitchell
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

namespace libsemigroups::detail {

  ////////////////////////////////////////////////////////////////////////
  // RewritingSystemSet --- Constructors + initializers
  ////////////////////////////////////////////////////////////////////////

  template <typename ReductionOrder>
  RewritingSystemSet<ReductionOrder>::~RewritingSystemSet() = default;

  template <typename ReductionOrder>
  RewritingSystemSet<ReductionOrder>&
  RewritingSystemSet<ReductionOrder>::init() {
    RewritingSystemBase::init();
    _set_rules.clear();
    return *this;
  }

  template <typename ReductionOrder>
  RewritingSystemSet<ReductionOrder>&
  RewritingSystemSet<ReductionOrder>::operator=(
      RewritingSystemSet const& that) {
    init();
    RewritingSystemBase::operator=(that);
    for (auto* rule : Rules::active_rules()) {
#ifdef LIBSEMIGROUPS_DEBUG
      LIBSEMIGROUPS_ASSERT(_set_rules.emplace(RuleLookup(rule)).second);
#else
      _set_rules.emplace(RuleLookup(rule));
#endif
    }
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // RewritingSystemSet --- Public member functions
  ////////////////////////////////////////////////////////////////////////

  template <typename ReductionOrder>
  template <typename Iterator>
  RewritingSystemSet<ReductionOrder>&
  RewritingSystemSet<ReductionOrder>::add_rule(Iterator first1,
                                               Iterator last1,
                                               Iterator first2,
                                               Iterator last2) {
    if (!std::equal(first1, last1, first2, last2)) {
      set_cached_confluent(tril::unknown);
      Rule* rule = Rules::add_pending_rule(first1, last1, first2, last2);
      reorder<ReductionOrder>(rule);
      // The left-hand-side of a rule must not be empty; otherwise, bad things
      // happen.
      LIBSEMIGROUPS_ASSERT(!rule->lhs().empty());
      if (!active_rules().empty()
          && pending_rules().size() > settings().reduction_threshold) {
        reduce();
      }
    }
    return *this;
  }

  template <typename ReductionOrder>
  bool RewritingSystemSet<ReductionOrder>::reduce() {
    RewritingSystemBase::sort_pending_rules();

    auto   start_time = std::chrono::high_resolution_clock::now();
    Ticker ticker;
    bool   old_ticker_running = _ticker_running;

    bool rules_added = false;

    while (!Rules::pending_rules().empty()) {
      Rule* rule1 = Rules::pop_pending_rule();
      LIBSEMIGROUPS_ASSERT(rule1->state() == Rule::State::pending);
      LIBSEMIGROUPS_ASSERT(rule1->lhs() != rule1->rhs());

      rewrite_no_reduce(rule1->lhs());
      rewrite_no_reduce(rule1->rhs());

      // Check rule is non-trivial
      if (rule1->lhs() != rule1->rhs()) {
        reorder<ReductionOrder>(rule1);

        native_word_type& lhs = rule1->lhs();

        auto const first = Rules::active_rules().begin();
        auto const last  = Rules::active_rules().end();

        for (auto it = first; it != last;) {
          Rule* rule2 = *it;

          // Check if lhs is contained within either the lhs or rhs of rule2
          // TODO(1) investigate whether or not this can be improved?
          if (rule2->lhs().find(lhs) != native_word_type::npos
              || rule2->rhs().find(lhs) != native_word_type::npos) {
            // If it is, rule2 must be deactivated and re-processed
            it = make_active_rule_pending(it);
          } else {
            ++it;
          }
        }
        RewritingSystemSet::add_active_rule(rule1);
        rules_added = true;
      } else {
        Rules::add_inactive_rule(rule1);
      }

      if (!_ticker_running && reporting_enabled()
          && delta(start_time) >= std::chrono::seconds(1)) {
        _ticker_running = true;
        ticker(
            [this, start_time]() { report_progress_from_thread(start_time); });
      }
    }
    _ticker_running = old_ticker_running;
    return rules_added;
  }

  template <typename ReductionOrder>
  void RewritingSystemSet<ReductionOrder>::rewrite(native_word_type& v) {
    reduce();
    rewrite_no_reduce(v);
  }

  ////////////////////////////////////////////////////////////////////////
  // RewritingSystemSet --- Private member functions
  ////////////////////////////////////////////////////////////////////////

  template <typename ReductionOrder>
  void RewritingSystemSet<ReductionOrder>::add_active_rule(Rule* new_rule) {
    // NOTE: unlike add_active_rule in RewritingSystemTrie, we do not reorder
    // new_rule, but in reduce, because we need it ordered correctly
    // before we can add it here. This is because in RewritingSystemSet we
    // require the rules in _set_rules to be reduced. If we add a new_rule
    // e.g. aba -> a and there is a currently active rule of the form abab ->
    // aba, then we must make abab -> aba pending before adding aba -> a.
    LIBSEMIGROUPS_ASSERT(!ReductionOrder{}(new_rule->lhs(), new_rule->rhs()));

    Rules::add_active_rule(new_rule);
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(_set_rules.emplace(RuleLookup(new_rule)).second);
#else
    _set_rules.emplace(RuleLookup(new_rule));
#endif
    LIBSEMIGROUPS_ASSERT(_set_rules.size() == Rules::active_rules().size());
    set_cached_confluent(tril::unknown);
  }

  template <typename ReductionOrder>
  typename RewritingSystemSet<ReductionOrder>::iterator
  RewritingSystemSet<ReductionOrder>::make_active_rule_pending(iterator it) {
#ifdef LIBSEMIGROUPS_DEBUG
    LIBSEMIGROUPS_ASSERT(_set_rules.erase(RuleLookup(*it)));
#else
    _set_rules.erase(RuleLookup(*it));
#endif
    LIBSEMIGROUPS_ASSERT(_set_rules.size() == Rules::active_rules().size() - 1);
    return Rules::make_active_rule_pending(it);
  }

  // REWRITE_FROM_LEFT from Sims, p67
  template <typename ReductionOrder>
  void RewritingSystemSet<ReductionOrder>::rewrite_no_reduce(
      native_word_type& v) const {
    size_t const n = Rules::stats().min_length_lhs_rule;

    if (v.size() < n) {
      return;
    }

    // position of the start of the unread suffix of the input word
    size_t pos = n - 1;

    RuleLookup lookup;

    while (pos < v.size()) {
      LIBSEMIGROUPS_ASSERT(pos >= n - 1);
      ++pos;
      auto it = _set_rules.find(lookup(v.begin(), v.begin() + pos));
      if (it != _set_rules.end() && it->rule()->lhs().size() <= pos) {
        // Unlike in RewritingSystemTrie::rewrite just because we found the
        // lookup in _set_rules doesn't necessarily mean that we have found a
        // rule whose lhs is contained in [v.begin(), v.begin() + pos), that's
        // why we do the second check in the if-condition above.
        Rule const* rule = (*it).rule();
        LIBSEMIGROUPS_ASSERT(is_suffix(v.begin(),
                                       v.begin() + pos,
                                       rule->lhs().cbegin(),
                                       rule->lhs().cend()));
        size_t diff = rule->lhs().size();
        pos -= diff;
        v.erase(v.begin() + pos, v.begin() + pos + diff);
        v.insert(v.begin() + pos, rule->rhs().begin(), rule->rhs().end());
        pos = pos < n - 1 ? n - 1 : pos;
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Confluence
  ////////////////////////////////////////////////////////////////////////

  template <typename ReductionOrder>
  bool RewritingSystemSet<ReductionOrder>::confluent_impl(
      std::atomic_uint64_t& seen) {
    using std::chrono::time_point;
    time_point start_time = std::chrono::high_resolution_clock::now();

    reduce();
    set_cached_confluent(tril::TRUE);
    native_word_type word1;
    native_word_type word2;

    for (auto it1 = Rules::active_rules().begin();
         it1 != Rules::active_rules().end();
         ++it1) {
      Rule const* rule1 = *it1;
      // Seems to be much faster to do this in reverse.
      for (auto it2 = Rules::active_rules().rbegin();
           it2 != Rules::active_rules().rend();
           ++it2) {
        seen++;
        Rule const* rule2 = *it2;
        for (auto it = rule1->lhs().cend() - 1; it >= rule1->lhs().cbegin();
             --it) {
          if (rule1 == rule2 && it == rule1->lhs().cbegin()) {
            continue;
          }
          // TODO(1): Remove duplication between this and
          // RewritingSystemTrie::overlap_confluent
          // Find longest common prefix of suffix B of rule1.lhs() defined by it
          // and R = rule2.lhs()
          auto prefix = maximum_common_prefix(it,
                                              rule1->lhs().cend(),
                                              rule2->lhs().cbegin(),
                                              rule2->lhs().cend());
          if (prefix.first == rule1->lhs().cend()
              || prefix.second == rule2->lhs().cend()) {
            // Seems that this function isn't called enough to merit using
            // MSV's here.
            word1.assign(rule1->lhs().cbegin(),
                         it);            // A
            word1.append(rule2->rhs());  // S
            word1.append(prefix.first,
                         rule1->lhs().cend());  // D

            word2.assign(rule1->rhs());  // Q
            word2.append(prefix.second,
                         rule2->lhs().cend());  // E

            if (word1 != word2) {
              rewrite_no_reduce(word1);
              rewrite_no_reduce(word2);
              if (word1 != word2) {
                set_cached_confluent(tril::FALSE);
                if (reporting_enabled()) {
                  report_checking_confluence(seen, start_time);
                }
                return false;
              }
            }
          }
        }
      }
    }
    if (reporting_enabled()) {
      report_checking_confluence(seen, start_time);
    }
    return cached_confluent();
  }

  // TODO(1): Remove duplication between this function and confluent_impl
  template <typename ReductionOrder>
  std::pair<size_t, size_t>
  RewritingSystemSet<ReductionOrder>::confluence_ratio() {
    reduce();
    std::pair<size_t, size_t> confluence_ratio{0, 0};
    native_word_type          word1;
    native_word_type          word2;

    for (auto it1 = Rules::active_rules().begin();
         it1 != Rules::active_rules().end();
         ++it1) {
      Rule const* rule1 = *it1;
      // Seems to be much faster to do this in reverse.
      for (auto it2 = Rules::active_rules().rbegin();
           it2 != Rules::active_rules().rend();
           ++it2) {
        Rule const* rule2 = *it2;
        for (auto it = rule1->lhs().cend() - 1; it >= rule1->lhs().cbegin();
             --it) {
          if (rule1 == rule2 && it == rule1->lhs().cbegin()) {
            continue;
          }
          // Find longest common prefix of a suffix of rule1->lhs() and
          // rule2->rhs(). If this prefix is also a suffix of either
          // rule1->lhs() or rule2->lhs(), then there is an overlap that needs
          // to be checked.
          auto prefix = maximum_common_prefix(it,
                                              rule1->lhs().cend(),
                                              rule2->lhs().cbegin(),
                                              rule2->lhs().cend());
          if (prefix.first == rule1->lhs().cend()
              || prefix.second == rule2->lhs().cend()) {
            confluence_ratio.first++;
            confluence_ratio.second++;
            // Seems that this function isn't called enough to merit using
            // MSV's here.
            word1.assign(rule1->lhs().cbegin(),
                         it);            // A
            word1.append(rule2->rhs());  // S
            word1.append(prefix.first,
                         rule1->lhs().cend());  // D

            word2.assign(rule1->rhs());  // Q
            word2.append(prefix.second,
                         rule2->lhs().cend());  // E

            if (word1 != word2) {
              rewrite_no_reduce(word1);
              rewrite_no_reduce(word2);
              if (word1 != word2) {
                set_cached_confluent(tril::FALSE);
                confluence_ratio.first--;
              }
            }
          }
        }
      }
    }
    return confluence_ratio;
  }

  ////////////////////////////////////////////////////////////////////////
  // RewritingSystemSet --- Reporting
  ////////////////////////////////////////////////////////////////////////

  template <typename ReductionOrder>
  void RewritingSystemSet<ReductionOrder>::report_checking_confluence(
      std::atomic_uint64_t const&                           seen,
      std::chrono::high_resolution_clock::time_point const& start_time) const {
    if (reporting_enabled()) {
      auto total_pairs
          = Rules::active_rules().size() * Rules::active_rules().size();

      auto total_pairs_s = group_digits(total_pairs);
      auto now           = std::chrono::high_resolution_clock::now();
      auto time
          = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
      report_no_prefix("{:-<95}\n", "");
      report_default("KnuthBendix: locally confluent for: {0:>{width}} / "
                     "{1:>{width}} ({2:>4.1f}%) pairs of rules ({3}s)\n",
                     group_digits(seen),
                     total_pairs_s,
                     (total_pairs != 0)
                         ? 100 * static_cast<double>(seen) / total_pairs
                         : 100,
                     time.count(),
                     fmt::arg("width", total_pairs_s.size()));
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // RewritingSystemTrie - constructors + initializers
  ////////////////////////////////////////////////////////////////////////

  template <typename ReductionOrder>
  RewritingSystemTrie<ReductionOrder>::RewritingSystemTrie()
      : RewritingSystemBase(),
        _cached_terminating(false),
        _new_rule_trie(),
        _rule_trie(0),
        _terminating_known(false),
        _ticker_running(false),
        _trie_nodes_visited_indices() {}

  template <typename ReductionOrder>
  RewritingSystemTrie<ReductionOrder>&
  RewritingSystemTrie<ReductionOrder>::init() {
    // Do nothing to _trie_nodes_visited_indices, or _new_rule_trie
    RewritingSystemBase::init();
    _cached_terminating = false;
    _rule_trie.init();
    _terminating_known = false;
    _ticker_running    = false;
    return *this;
  }

  template <typename ReductionOrder>
  RewritingSystemTrie<ReductionOrder>&
  RewritingSystemTrie<ReductionOrder>::operator=(
      RewritingSystemTrie const& that) {
    init();
    RewritingSystemBase::operator=(that);
    // Cannot just copy the _rule_trie because the values in it are Rule* which
    // would then point at Rule objects in "that" not "this".
    _rule_trie.init().increase_alphabet_size_by(
        that._rule_trie.alphabet_size());
    for (Rule* rule : Rules::active_rules()) {
      _rule_trie.insert_no_checks(rule->lhs(), rule);
    }
    _cached_terminating = that._cached_terminating;
    _terminating_known  = that._terminating_known;
    return *this;
  }

  template <typename ReductionOrder>
  RewritingSystemTrie<ReductionOrder>::~RewritingSystemTrie() = default;

  ////////////////////////////////////////////////////////////////////////
  // Public member functions - alphabetical order
  ////////////////////////////////////////////////////////////////////////

  // TODO(1) this is identical in RewritingSystemTrie/Set maybe should be in
  // Base?
  template <typename ReductionOrder>
  template <typename Iterator>
  RewritingSystemTrie<ReductionOrder>&
  RewritingSystemTrie<ReductionOrder>::add_rule(Iterator first1,
                                                Iterator last1,
                                                Iterator first2,
                                                Iterator last2) {
    if (!std::equal(first1, last1, first2, last2)) {
      Rule* rule = Rules::add_pending_rule(first1, last1, first2, last2);
      reorder<ReductionOrder>(rule);
      set_cached_confluent(tril::unknown);
      set_cached_terminating(tril::unknown);
      if (!active_rules().empty()
          && pending_rules().size() > settings().reduction_threshold) {
        // If active_rules().empty(), then we don't process pending rules, to
        // allow construction of RewritingSystems to still be fast.
        reduce();
      }
    }
    return *this;
  }

  template <typename ReductionOrder>
  void RewritingSystemTrie<ReductionOrder>::rewrite(native_word_type& v) {
    reduce();
    rewrite_no_reduce(v);
  }

  template <typename ReductionOrder>
  tril RewritingSystemTrie<ReductionOrder>::is_length_non_increasing_no_reduce()
      const noexcept {
    if constexpr (order::is_length_non_increasing_v<ReductionOrder>) {
      set_cached_terminating(tril::TRUE);
      return tril::TRUE;
    }

    if (is_reduced() != tril::TRUE) {
      return tril::unknown;
    }

    for (auto const& rule : rules()) {
      if (rule.first.size() < rule.second.size()) {
        return tril::FALSE;
      }
    }

    return tril::TRUE;
  }

  template <typename ReductionOrder>
  bool
  RewritingSystemTrie<ReductionOrder>::is_length_non_increasing() noexcept {
    if constexpr (order::is_length_non_increasing_v<ReductionOrder>) {
      set_cached_terminating(tril::TRUE);
      return true;
    }

    reduce();
    return is_length_non_increasing_no_reduce() == tril::TRUE;
  }

  template <typename ReductionOrder>
  tril RewritingSystemTrie<ReductionOrder>::is_terminating_no_reduce()
      const noexcept {
    if (_terminating_known) {
      if (_cached_terminating == true) {
        return tril::TRUE;
      }
      return tril::FALSE;
    }

    tril result = tril::unknown;

    if constexpr (order::is_well_founded_v<ReductionOrder>) {
      result = tril::TRUE;
    } else if (is_length_non_increasing_no_reduce() == tril::TRUE) {
      result = tril::TRUE;
    } else {
      for (auto const& rule : rules()) {
        if (std::search(rule.second.begin(),
                        rule.second.end(),
                        rule.first.begin(),
                        rule.first.end())
            != rule.second.end()) {
          result = tril::FALSE;
          break;
        }
      }
    }

    set_cached_terminating(result);
    return result;
  }

  template <typename ReductionOrder>
  tril RewritingSystemTrie<ReductionOrder>::is_terminating() noexcept {
    if constexpr (order::is_well_founded_v<ReductionOrder>) {
      set_cached_terminating(tril::TRUE);
      return tril::TRUE;
    }
    reduce();
    return is_terminating_no_reduce();
  }

  template <typename ReductionOrder>
  bool RewritingSystemTrie<ReductionOrder>::reduce() {
    using aho_corasick_impl::begin_search_no_checks;
    using aho_corasick_impl::end_search_no_checks;

    // If a system is not terminating, then reduction might make it terminating,
    // provided it can actually finish!
    if (_terminating_known && !_cached_terminating) {
      set_cached_terminating(tril::unknown);
    }

    auto                 start_time = std::chrono::high_resolution_clock::now();
    Ticker               ticker;
    ValueGuard           guard(_ticker_running);
    std::atomic_uint64_t seen = 0;

    RewritingSystemBase::sort_pending_rules();

    bool rules_added       = false;
    bool use_separate_trie = false;

    if (_use_new_rule_trie != nullptr) {
      use_separate_trie = _use_new_rule_trie(*this);
    }

    while (!Rules::pending_rules().empty()) {
      if (use_separate_trie) {
        _new_rule_trie.init(_rule_trie.alphabet_size());
      }
      bool rules_added_this_pass = false;
      while (!Rules::pending_rules().empty()) {
        Rule* rule = Rules::pop_pending_rule();
        LIBSEMIGROUPS_ASSERT(rule->state() == Rule::State::pending);
        LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
        rewrite_no_reduce(rule->lhs());
        rewrite_no_reduce(rule->rhs());

        if (rule->lhs() != rule->rhs()) {
          reorder<ReductionOrder>(rule);
          add_active_rule(rule);
          if (use_separate_trie) {
#ifdef LIBSEMIGROUPS_DEBUG
            auto [_, inserted] =
#endif
                _new_rule_trie.emplace_no_checks(
                    rule->lhs().cbegin(), rule->lhs().cend(), rule);
            LIBSEMIGROUPS_ASSERT(inserted);

            // Shouldn't be possible for 2 rules with equal left-hand sides to
            // exist, since the later added one will be rewritten using the
            // first.
          }
          rules_added           = true;
          rules_added_this_pass = true;
        } else {
          Rules::add_inactive_rule(rule);
        }
        if (!_ticker_running && reporting_enabled()
            && delta(start_time) >= std::chrono::seconds(1)) {
          _ticker_running = true;
          ticker([this, &start_time, &seen]() {
            report_progress_from_thread(seen, start_time);
          });
        }
      }

      if (rules_added_this_pass) {
        ValueGuard sg(_state);
        _state = State::reducing_pending_rules;

        Trie* new_rule_trie = use_separate_trie ? &_new_rule_trie : &_rule_trie;

        seen = 0;

        for (auto it = Rules::active_rules().begin();
             it != Rules::active_rules().end();) {
          ++seen;
          Rule* rule = *it;
          // Check whether any rule contains the left-hand-side of the "new"
          // rule
          bool increment = true;
          for (auto const& word : {rule->lhs(), rule->rhs()}) {
            auto first = begin_search_no_checks(*new_rule_trie, word);
            auto last  = end_search_no_checks(*new_rule_trie, word);

            if (std::any_of(
                    first, last, [rule, new_rule_trie](auto node_index) {
                      return new_rule_trie->node_no_checks(node_index).value()
                             != rule;
                    })) {
              it        = make_active_rule_pending(it);
              increment = false;
              break;
            }
          }
          if (increment) {
            ++it;
          }
          if (!_ticker_running && reporting_enabled()
              && delta(start_time) >= std::chrono::seconds(1)) {
            _ticker_running = true;
            ticker([this, &start_time, &seen]() {
              report_progress_from_thread(seen, start_time);
            });
          }
        }
      }
    }

    return rules_added;
  }

  ////////////////////////////////////////////////////////////////////////
  // RewritingSystemTrie --- Private member functions
  ////////////////////////////////////////////////////////////////////////

  template <typename ReductionOrder>
  void RewritingSystemTrie<ReductionOrder>::add_active_rule(Rule* new_rule) {
    // Must check negation here so we can use ReturnFalse to mean "no order"
    LIBSEMIGROUPS_ASSERT(!ReductionOrder{}(new_rule->lhs(), new_rule->rhs()));
    Rules::add_active_rule(new_rule);
    _rule_trie.emplace_no_checks(
        new_rule->lhs().cbegin(), new_rule->lhs().cend(), new_rule);
    set_cached_confluent(tril::unknown);
    set_cached_terminating(tril::unknown);
  }

  template <typename ReductionOrder>
  typename RewritingSystemTrie<ReductionOrder>::iterator
  RewritingSystemTrie<ReductionOrder>::make_active_rule_pending(iterator it) {
    _rule_trie.erase_no_checks((*it)->lhs());
    return Rules::make_active_rule_pending(it);
    if (_terminating_known && !_cached_terminating) {
      set_cached_terminating(tril::unknown);
    }
  }

  template <typename ReductionOrder>
  void RewritingSystemTrie<ReductionOrder>::rewrite_no_reduce(
      native_word_type& v) const {
    // fmt::print("Rewriting {} -> ", to_printable(v));
    // Check if v is rewriteable
    if (v.size() < Rules::stats().min_length_lhs_rule) {
      // fmt::print("{}\n", to_printable(v));
      return;
    }
    // OLD VERSION; Assumes length reducing!!

    //     _trie_nodes_visited_indices.clear();
    //     index_type current = _rule_trie.root;
    //     _trie_nodes_visited_indices.push_back(current);
    //
    // #ifdef LIBSEMIGROUPS_DEBUG
    //     iterator v_begin = u.begin();
    // #endif
    //     auto v_end   = v.begin();
    //     auto w_begin = v_end;
    //     auto w_end   = v.end();
    //
    //     while (w_begin != w_end) {
    //       // Read first letter of w and traverse trie
    //       auto x = *w_begin;
    //       ++w_begin;
    //       current
    //           = _rule_trie.traverse_no_checks(current,
    //           static_cast<letter_type>(x));
    //
    //       if (!_rule_trie.node_no_checks(current).terminal()) {
    //         _trie_nodes_visited_indices.push_back(current);
    //         *v_end = x;
    //         ++v_end;
    //       } else {
    //         // Find rule that corresponds to terminal node
    //         Rule const* rule     =
    //         _rule_trie.node_no_checks(current).value(); auto lhs_size =
    //         rule->lhs().size(); LIBSEMIGROUPS_ASSERT(lhs_size != 0);
    //
    //         // Check the lhs is smaller than the portion of the word that
    //         has
    //         // been read
    //         LIBSEMIGROUPS_ASSERT(lhs_size
    //                              <= static_cast<size_t>(v_end - v_begin) +
    //                              1);
    //         v_end -= lhs_size - 1;
    //         w_begin -= rule->rhs().size();
    //         // Replace lhs with rhs in-place
    //         std::copy(rule->rhs().cbegin(), rule->rhs().cend(), w_begin);
    //         _trie_nodes_visited_indices.erase(_trie_nodes_visited_indices.end()
    //                                               - lhs_size + 1,
    //                                           _trie_nodes_visited_indices.end());
    //         current = _trie_nodes_visited_indices.back();
    //       }
    //     }
    //     v.erase(v_end - v.cbegin());

    // ALTERNATE NEW VERSION, NOT GOOD
    // NOTE: although this works, I think it's strictly worse than what was
    // here before, because here we repeatedly retraverse the trie, whereas
    // previously we just walked back to where we left off reading it before.

    //    auto match = _rule_trie.subword_no_checks(v.begin(), v.end());
    //    while (match) {
    //      // [v.begin(), v.end())
    //      //   = [v.begin(), match.first)
    //      //     + [match.first, match.last)
    //      //     + [match.last, v.end())
    //      // where [match.first, match.last) is a key into _rule_trie
    //      Rule const* rule = match.value().value();
    //
    //      size_t pos = std::distance(v.begin(), match.first);
    //      // lhs of a rule is key = [first, last), so we erase this from v
    //      v.erase(match.first, match.last);
    //      v.insert(v.begin() + pos, rule->rhs().begin(), rule->rhs().end());
    //      match = _rule_trie.subword_no_checks(v.begin(), v.end());
    //    }

    // NEW VERSION
    _trie_nodes_visited_indices.clear();
    index_type current = _rule_trie.root;
    _trie_nodes_visited_indices.push_back(current);

    // position of the start of the unread suffix of the input word
    size_t pos = 0;

    while (pos != v.size()) {
      // Read first letter of the unread suffix and traverse trie
      current = _rule_trie.traverse_no_checks(current,
                                              static_cast<letter_type>(v[pos]));

      if (!_rule_trie.node_no_checks(current).terminal()) {
        _trie_nodes_visited_indices.push_back(current);
        pos++;
      } else {
        // Everything here is off by one because we read everything up to and
        // including the pos-th character in "v"
        Rule const* rule = _rule_trie.node_no_checks(current).value();
        size_t      diff = rule->lhs().size() - 1;
        pos -= diff;
        v.erase(v.begin() + pos, v.begin() + pos + diff + 1);
        v.insert(v.begin() + pos, rule->rhs().begin(), rule->rhs().end());
        _trie_nodes_visited_indices.erase(_trie_nodes_visited_indices.end()
                                              - diff,
                                          _trie_nodes_visited_indices.end());
        current = _trie_nodes_visited_indices.back();
      }
    }
    // fmt::print("{}\n", to_printable(v));
  }

  template <typename ReductionOrder>
  void
  RewritingSystemTrie<ReductionOrder>::set_cached_terminating(tril val) const {
    if (val == tril::TRUE) {
      _terminating_known  = true;
      _cached_terminating = true;
    } else if (val == tril::FALSE) {
      _terminating_known  = true;
      _cached_terminating = false;
    } else {
      _terminating_known = false;
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Confluence
  ////////////////////////////////////////////////////////////////////////

  // TODO(1): Consider moving this into RewriterBase so that it can be used by
  // RewritingSystemSet. This would require making <rewrite_no_reduce> a virtual
  // function, so maybe we don't want to do this.
  template <typename ReductionOrder>
  bool RewritingSystemTrie<ReductionOrder>::overlap_confluent(
      Rule const*       rule1,
      Rule const*       rule2,
      size_t const      overlap_length,
      native_word_type& word1,
      native_word_type& word2) const {
    // Overlapped word looks like ABC where the LHS of rule1 corresponds to
    // AB, the LHS of rule2 corresponds to BC, and |B|= overlap_length.
    // If AB -> X, BC -> Y, then ABC gets rewritten to XC and AY
    MultiView<native_word_type> word1_view(rule1->rhs());  // X
    word1_view.append(rule2->lhs().cbegin() + overlap_length,
                      rule2->lhs().cend());  // C

    MultiView<native_word_type> word2_view(rule1->lhs().cbegin(),
                                           rule1->lhs().cend()
                                               - overlap_length);   // A
    word2_view.append(rule2->rhs().cbegin(), rule2->rhs().cend());  // Y

    if (word1_view == word2_view) {
      return true;
    }

    word1.assign(word1_view);
    word2.assign(word2_view);
    rewrite_no_reduce(word1);
    rewrite_no_reduce(word2);
    if (word1 == word2) {
      return true;
    }

    set_cached_confluent(tril::FALSE);
    return false;
  }

  template <typename ReductionOrder>
  bool RewritingSystemTrie<ReductionOrder>::descendants_confluent(
      Rule const* rule1,
      index_type  current_node,
      size_t      overlap_length) const {
    native_word_type                                word_1;
    native_word_type                                word_2;
    std::stack<index_type, std::vector<index_type>> stack;
    stack.push(current_node);

    while (!stack.empty()) {
      current_node = stack.top();
      stack.pop();
      LIBSEMIGROUPS_ASSERT(rule1->state() == Rule::State::active);
      if (_rule_trie.node_no_checks(current_node).terminal()) {
        Rule const* rule2 = _rule_trie.node_no_checks(current_node).value();
        if (!overlap_confluent(rule1, rule2, overlap_length, word_1, word_2)) {
          return false;
        }
      }

      // Read each possible letter and traverse down the trie
      for (letter_type x = 0; x != _rule_trie.alphabet_size(); ++x) {
        auto child = _rule_trie.child_no_checks(current_node, x);
        if (child != UNDEFINED) {
          stack.push(child);
        }
      }
    }
    return true;
  }

  // TODO(1): Remove duplication with descendants_confluent
  template <typename ReductionOrder>
  std::pair<size_t, size_t>
  RewritingSystemTrie<ReductionOrder>::number_descendants_confluent(
      Rule const* rule1,
      index_type  current_node,
      size_t      overlap_length) const {
    native_word_type                                word_1;
    native_word_type                                word_2;
    std::pair<size_t, size_t>                       confluence_fraction;
    std::stack<index_type, std::vector<index_type>> stack;
    stack.push(current_node);

    while (!stack.empty()) {
      current_node = stack.top();
      stack.pop();
      LIBSEMIGROUPS_ASSERT(rule1->state() == Rule::State::active);
      if (_rule_trie.node_no_checks(current_node).terminal()) {
        Rule const* rule2 = _rule_trie.node_no_checks(current_node).value();
        ++confluence_fraction.second;
        if (overlap_confluent(rule1, rule2, overlap_length, word_1, word_2)) {
          ++confluence_fraction.first;
        }
      }

      // Read each possible letter and traverse down the trie
      for (letter_type x = 0; x != _rule_trie.alphabet_size(); ++x) {
        auto child = _rule_trie.child_no_checks(current_node, x);
        if (child != UNDEFINED) {
          stack.push(child);
        }
      }
    }
    return confluence_fraction;
  }

  template <typename ReductionOrder>
  bool RewritingSystemTrie<ReductionOrder>::confluent_impl(
      std::atomic_uint64_t& seen) {
    using std::chrono::time_point;
    time_point start_time = std::chrono::high_resolution_clock::now();

    reduce();

    index_type link;
    set_cached_confluent(tril::TRUE);

    // For each rule, check if any descendent of any suffix breaks
    // confluence
    for (auto node_it = _rule_trie.cbegin_terminal_nodes();
         node_it != _rule_trie.cend_terminal_nodes();
         ++node_it) {
      seen++;
      link = _rule_trie.node_no_checks(*node_it).suffix_link();
      LIBSEMIGROUPS_ASSERT(*node_it != _rule_trie.root);
      while (link != _rule_trie.root) {
        if (!descendants_confluent(_rule_trie.node_no_checks(*node_it).value(),
                                   link,
                                   _rule_trie.node_no_checks(link).height())) {
          set_cached_confluent(tril::FALSE);
          report_checking_confluence(seen, start_time);
          return false;
        }
        link = _rule_trie.node_no_checks(link).suffix_link();
      }
    }

    report_checking_confluence(seen, start_time);
    return true;
  }

  template <typename ReductionOrder>
  std::pair<size_t, size_t>
  RewritingSystemTrie<ReductionOrder>::confluence_ratio() {
    std::pair<size_t, size_t> confluence_fraction{0, 0};
    reduce();
    index_type link;

    // For each rule, count how many descendents of any suffix breaks confluence
    for (auto node_it = _rule_trie.cbegin_terminal_nodes();
         node_it != _rule_trie.cend_terminal_nodes();
         ++node_it) {
      link = _rule_trie.node_no_checks(*node_it).suffix_link();
      LIBSEMIGROUPS_ASSERT(*node_it != _rule_trie.root);
      while (link != _rule_trie.root) {
        std::pair<size_t, size_t> const descendants_confluence_fraction
            = number_descendants_confluent(
                _rule_trie.node_no_checks(*node_it).value(),
                link,
                _rule_trie.node_no_checks(link).height());
        confluence_fraction.first += descendants_confluence_fraction.first;
        confluence_fraction.second += descendants_confluence_fraction.second;

        link = _rule_trie.node_no_checks(link).suffix_link();
      }
    }

    return confluence_fraction;
  }

  ////////////////////////////////////////////////////////////////////////
  // RewritingSystemTrie --- Reporting
  ////////////////////////////////////////////////////////////////////////

  template <typename ReductionOrder>
  void RewritingSystemTrie<ReductionOrder>::report_checking_confluence(
      std::atomic_uint64_t const&                           seen,
      std::chrono::high_resolution_clock::time_point const& start_time) const {
    if (reporting_enabled()) {
      auto total_rules   = Rules::active_rules().size();
      auto total_rules_s = group_digits(total_rules);
      auto now           = std::chrono::high_resolution_clock::now();
      auto time
          = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
      report_no_prefix("{:-<95}\n", "");
      report_default("KnuthBendix: locally confluent for: {0:>{width}} / "
                     "{1:>{width}} ({2:>4.1f}%) rules ({3}s)\n",
                     group_digits(seen),
                     total_rules_s,
                     (total_rules != 0)
                         ? 100 * static_cast<double>(seen) / total_rules
                         : 100,
                     time.count(),
                     fmt::arg("width", total_rules_s.size()));
    }
  }

  template <typename ReductionOrder>
  void RewritingSystemTrie<ReductionOrder>::report_reducing_rules(
      std::atomic_uint64_t const&                           seen,
      std::chrono::high_resolution_clock::time_point const& start_time) const {
    auto gd = group_digits;
    if (reporting_enabled()) {
      // TODO(1) This could maybe be better, more like the formatting in
      // "report_progress_from_thread"
      auto total_rules = Rules::active_rules().size();
      report_default("KnuthBendix: reducing rules: {0:>{width}} / "
                     "{1:>{width}} ({2:>4.1f}%) ({3})\n",
                     gd(seen),
                     gd(total_rules),
                     (total_rules != 0)
                         ? 100 * static_cast<double>(seen) / total_rules
                         : 100,
                     string_time(delta(start_time)),
                     fmt::arg("width", gd(total_rules).size()));
    }
  }

}  // namespace libsemigroups::detail
