//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 Joseph Edwards + James D. Mitchell
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

#include "libsemigroups/detail/rewriters.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>

#include "libsemigroups/runner.hpp"  // for Ticker

#include "libsemigroups/detail/guard.hpp"   // for Guard
#include "libsemigroups/detail/report.hpp"  // for report_default

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // Rule
    ////////////////////////////////////////////////////////////////////////

    Rule::Rule(int64_t id) : _lhs(), _rhs(), _id(-1 * id) {
      LIBSEMIGROUPS_ASSERT(_id < 0);
    }

    void Rule::activate_no_checks() noexcept {
      LIBSEMIGROUPS_ASSERT(_id != 0);
      LIBSEMIGROUPS_ASSERT(!active());
      _id *= -1;
    }

    void Rule::deactivate_no_checks() noexcept {
      LIBSEMIGROUPS_ASSERT(_id != 0);
      LIBSEMIGROUPS_ASSERT(active());
      _id *= -1;
    }

    ////////////////////////////////////////////////////////////////////////
    // RuleLookup
    ////////////////////////////////////////////////////////////////////////

    // Reverse lex order
    bool RuleLookup::operator<(RuleLookup const& that) const {
      auto it_this = _last - 1;
      auto it_that = that._last - 1;
      while (it_this > _first && it_that > that._first
             && *it_this == *it_that) {
        --it_that;
        --it_this;
      }
      return *it_this < *it_that;
    }

    ////////////////////////////////////////////////////////////////////////
    // Rules
    ////////////////////////////////////////////////////////////////////////

    Rules::Stats::Stats() noexcept {
      init();
    }

    Rules::Stats& Rules::Stats::init() noexcept {
      max_word_length        = 0;
      max_active_word_length = 0;
      max_active_rules       = 0;
      min_length_lhs_rule    = std::numeric_limits<size_t>::max();
      total_rules            = 0;
      return *this;
    }

    Rules& Rules::init() {
      // Put all active rules and those rules in the stack into the
      // inactive_rules list
      for (Rule* ptr : _active_rules) {
        ptr->deactivate_no_checks();
        _inactive_rules.insert(_inactive_rules.end(), ptr);
      }
      _active_rules.clear();
      for (auto& it : _cursors) {
        it = _active_rules.end();
      }
      return *this;
    }

    Rules& Rules::operator=(Rules const& that) {
      init();
      for (Rule const* rule : that) {
        add_rule(copy_rule(rule));
      }
      for (size_t i = 0; i < _cursors.size(); ++i) {
        _cursors[i] = _active_rules.begin();
        std::advance(
            _cursors[i],
            std::distance(that.begin(),
                          static_cast<const_iterator>(that._cursors[i])));
      }
      return *this;
    }

    Rules& Rules::operator=(Rules&& that) {
      // We swap to ensure that all rules are properly deleted
      std::swap(_active_rules, that._active_rules);
      std::swap(_inactive_rules, that._inactive_rules);
      _cursors = std::move(that._cursors);
      _stats   = std::move(that._stats);
      return *this;
    }

    Rules::~Rules() {
      for (Rule* rule : _active_rules) {
        delete rule;
      }
      for (Rule* rule : _inactive_rules) {
        delete rule;
      }
    }

    Rule* Rules::new_rule() {
      ++_stats.total_rules;
      Rule* rule;
      if (!_inactive_rules.empty()) {
        rule = _inactive_rules.front();
        rule->set_id_no_checks(_stats.total_rules);
        _inactive_rules.erase(_inactive_rules.begin());
      } else {
        rule = new Rule(_stats.total_rules);
      }
      LIBSEMIGROUPS_ASSERT(!rule->active());
      return rule;
    }

    Rule* Rules::copy_rule(Rule const* rule) {
      return new_rule(rule->lhs().cbegin(),
                      rule->lhs().cend(),
                      rule->rhs().cbegin(),
                      rule->rhs().cend());
    }

    Rules::iterator Rules::erase_from_active_rules(iterator it) {
      // _stats.unique_lhs_rules.erase(*((*it)->lhs()));
      LIBSEMIGROUPS_ASSERT(!(*it)->active());
      // TODO(1) calling the next two lines double deactivates some rules (those
      // coming from make_active_rule_pending), weirdly everything works when
      // this happens (tests pass, though some assertions fail in debug mode)
      // and test 139 is twice as fast for some reason!

      // Rule* rule = *it;
      // rule->deactivate_no_checks();

      if (it != _cursors[0] && it != _cursors[1]) {
        it = _active_rules.erase(it);
      } else if (it == _cursors[0] && it != _cursors[1]) {
        _cursors[0] = _active_rules.erase(it);
        it          = _cursors[0];
      } else if (it != _cursors[0] && it == _cursors[1]) {
        _cursors[1] = _active_rules.erase(it);
        it          = _cursors[1];
      } else {
        _cursors[0] = _active_rules.erase(it);
        _cursors[1] = _cursors[0];
        it          = _cursors[0];
      }
      return it;
    }

    void Rules::add_rule(Rule* rule) {
      LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
      _stats.max_word_length
          = std::max(_stats.max_word_length, rule->lhs().size());
      _stats.max_active_rules
          = std::max(_stats.max_active_rules, number_of_active_rules());
      // _stats.unique_lhs_rules.insert(*rule->lhs());
      rule->activate_no_checks();
      _active_rules.push_back(rule);
      for (auto& it : _cursors) {
        if (it == end()) {
          --it;
        }
      }
      if (rule->lhs().size() < _stats.min_length_lhs_rule) {
        // TODO(later) this is not valid when using non-length reducing
        // orderings (such as RECURSIVE)
        _stats.min_length_lhs_rule = rule->lhs().size();
      }
    }

    size_t Rules::max_active_word_length() const {
      auto comp = [](Rule const* p, Rule const* q) -> bool {
        return p->lhs().size() < q->lhs().size();
      };
      auto max = std::max_element(begin(), end(), comp);
      if (max != end()) {
        _stats.max_active_word_length
            = std::max(_stats.max_active_word_length, (*max)->lhs().size());
      }
      return _stats.max_active_word_length;
    }

    ////////////////////////////////////////////////////////////////////////
    // RewriteBase
    ////////////////////////////////////////////////////////////////////////

    RewriteBase::RewriteBase()
        : _cached_confluent(false),
          _confluence_known(false),
          _max_pending_rules(0),
          _pending_rules(),
          _state(State::none),
          _ticker_running(false) {}

    RewriteBase& RewriteBase::init() {
      Rules::init();
      // Put all active rules and those rules in the stack into the
      // inactive_rules list
      for (Rule* rule : _pending_rules) {
        Rules::add_inactive_rule(rule);
      }
      _pending_rules.clear();
      _max_pending_rules = 0;
      _cached_confluent  = false;
      _confluence_known  = false;
      _state             = State::none;
      _ticker_running    = false;
      return *this;
    }

    RewriteBase::RewriteBase(RewriteBase&& that)
        : _cached_confluent(that._cached_confluent.load()),
          _confluence_known(that._confluence_known.load()),
          _max_pending_rules(std::move(that._max_pending_rules)),
          _pending_rules(std::move(that._pending_rules)),
          _state(std::move(that._state)),
          _ticker_running(std::move(that._ticker_running)) {}

    RewriteBase& RewriteBase::operator=(RewriteBase const& that) {
      Rules::operator=(that);
      _cached_confluent = that._cached_confluent.load();
      _confluence_known = that._confluence_known.load();
      _pending_rules.clear();
      _ticker_running = that._ticker_running;
      _state          = that._state;

      for (auto const* rule : that._pending_rules) {
        _pending_rules.emplace_back(copy_rule(rule));
      }
      return *this;
    }

    RewriteBase& RewriteBase::operator=(RewriteBase&& that) {
      Rules::operator=(std::move(that));
      _cached_confluent = that._cached_confluent.load();
      _confluence_known = that._confluence_known.load();
      // Again we swap so that all rules are properly deleted
      std::swap(_pending_rules, that._pending_rules);
      _ticker_running = std::move(that._ticker_running);
      _state          = that._state;
      return *this;
    }

    RewriteBase::~RewriteBase() {
      for (Rule* rule : _pending_rules) {
        delete rule;
      }
      _pending_rules.clear();
    }

    void RewriteBase::set_cached_confluent(tril val) const {
      if (val == tril::TRUE) {
        _confluence_known = true;
        _cached_confluent = true;
      } else if (val == tril::FALSE) {
        _confluence_known = true;
        _cached_confluent = false;
      } else {
        _confluence_known = false;
      }
    }

    bool RewriteBase::add_pending_rule(Rule* rule) {
      LIBSEMIGROUPS_ASSERT(!rule->active());
      if (rule->lhs() != rule->rhs()) {
        rule->reorder();
        _pending_rules.push_back(rule);
        _max_pending_rules
            = std::max(_max_pending_rules, _pending_rules.size());
        return true;
      } else {
        Rules::add_inactive_rule(rule);
        return false;
      }
    }

    bool RewriteBase::confluent() {
      using std::chrono::high_resolution_clock;
      using std::chrono::time_point;

      if (number_of_pending_rules() != 0) {
        set_cached_confluent(tril::unknown);
        return false;
      } else if (confluence_known()) {
        return RewriteBase::cached_confluent();
      }

      std::atomic_uint64_t seen = 0;
      if (reporting_enabled() && !_ticker_running) {
        detail::Guard  state(_state, State::checking_confluence);
        detail::Guard  ticker(_ticker_running, true);
        time_point     start_time = high_resolution_clock::now();
        detail::Ticker t(
            [&]() { report_progress_from_thread(seen, start_time); });
        return confluent_impl(seen);
      } else {
        return confluent_impl(seen);
      }
    }

    void RewriteBase::report_progress_from_thread(
        std::atomic_uint64_t const&                           seen,
        std::chrono::high_resolution_clock::time_point const& start_time) {
      if (_state == State::none || _state == State::adding_pending_rules) {
        using detail::string_time;
        auto gd       = detail::group_digits;
        auto active   = gd(number_of_active_rules());
        auto inactive = gd(number_of_inactive_rules());
        auto pending  = gd(number_of_pending_rules());
        auto defined  = gd(stats().total_rules);

        report_default("KnuthBendix: rules {} (active) | {} (inactive) | {} "
                       "(pending) | {} "
                       "(defined) | {}\n",
                       active,
                       inactive,
                       pending,
                       defined,
                       string_time(delta(start_time)));
      } else if (_state == State::checking_confluence) {
        report_checking_confluence(seen, start_time);
      } else {
        LIBSEMIGROUPS_ASSERT(_state == State::reducing_pending_rules);
        report_reducing_rules(seen, start_time);
      }
    }

    Rule* RewriteBase::next_pending_rule() {
      LIBSEMIGROUPS_ASSERT(_pending_rules.size() != 0);
      Rule* rule = _pending_rules.back();
      _pending_rules.pop_back();
      return rule;
    }

    ////////////////////////////////////////////////////////////////////////
    // RewriteFromLeft
    ////////////////////////////////////////////////////////////////////////

    RewriteFromLeft::~RewriteFromLeft() = default;

    RewriteFromLeft& RewriteFromLeft::init() {
      RewriteBase::init();
      _set_rules.clear();
      return *this;
    }

    RewriteFromLeft& RewriteFromLeft::operator=(RewriteFromLeft const& that) {
      init();
      RewriteBase::operator=(that);
      for (auto* rule : *this) {
#ifdef LIBSEMIGROUPS_DEBUG
        LIBSEMIGROUPS_ASSERT(_set_rules.emplace(RuleLookup(rule)).second);
#else
        _set_rules.emplace(RuleLookup(rule));
#endif
      }
      return *this;
    }

    RewriteFromLeft::iterator
    RewriteFromLeft::make_active_rule_pending(iterator it) {
      Rule* rule = *it;
      rule->deactivate_no_checks();
      add_pending_rule(rule);
#ifdef LIBSEMIGROUPS_DEBUG
      LIBSEMIGROUPS_ASSERT(_set_rules.erase(RuleLookup(rule)));
#else
      _set_rules.erase(RuleLookup(rule));
#endif
      LIBSEMIGROUPS_ASSERT(_set_rules.size() == number_of_active_rules() - 1);
      return Rules::erase_from_active_rules(it);
    }

    void RewriteFromLeft::add_rule(Rule* rule) {
      Rules::add_rule(rule);
      // _stats.unique_lhs_rules.insert(*rule->lhs());
#ifdef LIBSEMIGROUPS_DEBUG
      LIBSEMIGROUPS_ASSERT(_set_rules.emplace(RuleLookup(rule)).second);
#else
      _set_rules.emplace(RuleLookup(rule));
#endif
      LIBSEMIGROUPS_ASSERT(_set_rules.size() == number_of_active_rules());
      set_cached_confluent(tril::unknown);
    }

    // REWRITE_FROM_LEFT from Sims, p67
    // Caution: this uses the assumption that rules are length reducing, if they
    // are not, then u might not have sufficient space!
    void RewriteFromLeft::rewrite(native_word_type& u) {
      if (u.size() < stats().min_length_lhs_rule) {
        return;
      }

      auto v_begin = u.begin();
      auto v_end   = u.begin() + stats().min_length_lhs_rule - 1;
      auto w_begin = v_end;
      auto w_end   = u.end();

      RuleLookup lookup;

      while (w_begin != w_end) {
        *v_end = *w_begin;
        ++v_end;
        ++w_begin;

        auto it = _set_rules.find(lookup(v_begin, v_end));
        if (it != _set_rules.end()) {
          Rule const* rule = (*it).rule();
          if (rule->lhs().size() <= static_cast<size_t>(v_end - v_begin)) {
            LIBSEMIGROUPS_ASSERT(detail::is_suffix(
                v_begin, v_end, rule->lhs().cbegin(), rule->lhs().cend()));
            v_end -= rule->lhs().size();
            w_begin -= rule->rhs().size();
            std::copy(rule->rhs().cbegin(), rule->rhs().cend(), w_begin);
          }
        }
        while (w_begin != w_end
               && stats().min_length_lhs_rule - 1
                      > static_cast<size_t>((v_end - v_begin))) {
          *v_end = *w_begin;
          ++v_end;
          ++w_begin;
        }
      }
      u.erase(v_end - u.cbegin());
    }

    void RewriteFromLeft::report_checking_confluence(
        std::atomic_uint64_t const&                           seen,
        std::chrono::high_resolution_clock::time_point const& start_time)
        const {
      if (reporting_enabled()) {
        auto total_pairs   = std::pow(Rules::number_of_active_rules(), 2);
        auto total_pairs_s = detail::group_digits(total_pairs);
        auto now           = std::chrono::high_resolution_clock::now();
        auto time          = std::chrono::duration_cast<std::chrono::seconds>(
            now - start_time);
        report_no_prefix("{:-<95}\n", "");
        report_default("KnuthBendix: locally confluent for: {0:>{width}} / "
                       "{1:>{width}} ({2:>4.1f}%) pairs of rules ({3}s)\n",
                       detail::group_digits(seen),
                       total_pairs_s,
                       (total_pairs != 0)
                           ? 100 * static_cast<double>(seen) / total_pairs
                           : 100,
                       time.count(),
                       fmt::arg("width", total_pairs_s.size()));
      }
    }

    bool RewriteFromLeft::confluent_impl(std::atomic_uint64_t& seen) {
      using std::chrono::time_point;
      time_point start_time = std::chrono::high_resolution_clock::now();

      set_cached_confluent(tril::TRUE);
      native_word_type word1;
      native_word_type word2;

      for (auto it1 = begin(); it1 != end(); ++it1) {
        Rule const* rule1 = *it1;
        // Seems to be much faster to do this in reverse.
        for (auto it2 = rbegin(); it2 != rend(); ++it2) {
          seen++;
          Rule const* rule2 = *it2;
          for (auto it = rule1->lhs().cend() - 1; it >= rule1->lhs().cbegin();
               --it) {
            // Find longest common prefix of suffix B of rule1.lhs() defined by
            // it and R = rule2.lhs()
            auto prefix = detail::maximum_common_prefix(it,
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
                rewrite(word1);
                rewrite(word2);
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

    bool RewriteFromLeft::process_pending_rules() {
      // TODO(1) try maintaining pending_rules as a heap
      std::sort(
          _pending_rules.begin(),
          _pending_rules.end(),
          [](Rule const* x, Rule const* y) { return x->lhs() > y->lhs(); });

      auto           start_time = std::chrono::high_resolution_clock::now();
      detail::Ticker ticker;
      bool           old_ticker_running = _ticker_running;

      bool rules_added = false;

      while (number_of_pending_rules() != 0) {
        Rule* rule1 = next_pending_rule();
        LIBSEMIGROUPS_ASSERT(!rule1->active());
        LIBSEMIGROUPS_ASSERT(rule1->lhs() != rule1->rhs());
        // Rewrite both sides and reorder if necessary . . .
        rewrite(rule1);

        // Check rule is non-trivial
        if (rule1->lhs() != rule1->rhs()) {
          native_word_type& lhs = rule1->lhs();

          for (auto it = begin(); it != end();) {
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
          add_rule(rule1);
          rules_added = true;
        } else {
          add_inactive_rule(rule1);
        }
        if (!_ticker_running && reporting_enabled()
            && delta(start_time) >= std::chrono::seconds(1)) {
          _ticker_running = true;
          ticker([this, start_time]() {
            report_progress_from_thread(start_time);
          });
        }
      }
      _ticker_running = old_ticker_running;
      return rules_added;
    }

    ////////////////////////////////////////////////////////////////////////
    // RewriteTrie
    ////////////////////////////////////////////////////////////////////////

    RewriteTrie::RewriteTrie()
        : RewriteBase(),
          _new_rule_map(),
          _new_rule_trie(),
          _rewrite_tmp_buf(),
          _rule_map(),
          _rule_trie(0),
          _ticker_running(false) {}

    RewriteTrie::~RewriteTrie() = default;

    RewriteTrie& RewriteTrie::init() {
      RewriteBase::init();
      _rule_map.clear();
      _rule_trie.init();
      // Do nothing to _rewrite_tmp_buf, _new_rule_map, or _new_rule_trie
      return *this;
    }

    RewriteTrie& RewriteTrie::operator=(RewriteTrie const& that) {
      init();
      RewriteBase::operator=(that);
      _rule_trie = that._rule_trie;
      for (Rule* rule : *this) {
        index_type node = _rule_trie.traverse_trie_no_checks(
            rule->lhs().cbegin(), rule->lhs().cend());
        LIBSEMIGROUPS_ASSERT(_rule_trie.terminal(node));
        _rule_map.emplace(node, rule);
      }

      return *this;
    }

    // As with RewriteFromLeft::rewrite, this assumes that all rules are length
    // reducing.
    void RewriteTrie::rewrite(native_word_type& u) {
      // Check if u is rewriteable
      if (u.size() < stats().min_length_lhs_rule) {
        return;
      }

      _rewrite_tmp_buf.clear();
      index_type current = _rule_trie.root;
      _rewrite_tmp_buf.push_back(current);

#ifdef LIBSEMIGROUPS_DEBUG
      iterator v_begin = u.begin();
#endif
      iterator v_end   = u.begin();
      iterator w_begin = v_end;
      iterator w_end   = u.end();

      while (w_begin != w_end) {
        // Read first letter of w and traverse trie
        auto x = *w_begin;
        ++w_begin;
        current = _rule_trie.traverse_no_checks(current,
                                                static_cast<letter_type>(x));

        auto rule_it = _rule_map.find(current);
        if (!_rule_trie.node_no_checks(current).terminal()) {
          _rewrite_tmp_buf.push_back(current);
          *v_end = x;
          ++v_end;
        } else {
          // Find rule that corresponds to terminal node
          Rule const* rule     = rule_it->second;
          auto        lhs_size = rule->lhs().size();
          LIBSEMIGROUPS_ASSERT(lhs_size != 0);

          // Check the lhs is smaller than the portion of the word that has
          // been read
          LIBSEMIGROUPS_ASSERT(lhs_size
                               <= static_cast<size_t>(v_end - v_begin) + 1);
          v_end -= lhs_size - 1;
          w_begin -= rule->rhs().size();
          // Replace lhs with rhs in-place
          std::copy(rule->rhs().cbegin(), rule->rhs().cend(), w_begin);
          _rewrite_tmp_buf.erase(_rewrite_tmp_buf.end() - lhs_size + 1,
                                 _rewrite_tmp_buf.end());
          current = _rewrite_tmp_buf.back();
        }
      }
      u.erase(v_end - u.cbegin());
    }

    bool RewriteTrie::process_pending_rules() {
      using detail::aho_corasick_impl::begin_search_no_checks;
      using detail::aho_corasick_impl::end_search_no_checks;

      auto           start_time = std::chrono::high_resolution_clock::now();
      detail::Ticker ticker;
      detail::Guard  guard(_ticker_running);
      std::atomic_uint64_t seen = 0;

      // TODO(1) use a heap for these maybe?
      std::sort(
          _pending_rules.begin(),
          _pending_rules.end(),
          [](Rule const* x, Rule const* y) { return x->lhs() > y->lhs(); });

      bool rules_added = false;
      // TODO(1) could make this a setting, or use a different condition (such
      // as number_of_active_rules / 2 or something)
      bool use_separate_trie
          = number_of_pending_rules() < number_of_active_rules();

      while (number_of_pending_rules() != 0) {
        if (use_separate_trie) {
          _new_rule_trie.init(_rule_trie.alphabet_size());
          _new_rule_map.clear();
        }
        bool rules_added_this_pass = false;
        while (number_of_pending_rules() != 0) {
          Guard sg(_state);
          _state     = State::adding_pending_rules;
          Rule* rule = next_pending_rule();
          LIBSEMIGROUPS_ASSERT(!rule->active());
          LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
          // Rewrite both sides and reorder if necessary . . .
          rewrite(rule);

          if (rule->lhs() != rule->rhs()) {
            add_rule(rule);
            if (use_separate_trie) {
              index_type node = _new_rule_trie.add_word_no_checks(
                  rule->lhs().cbegin(), rule->lhs().cend());
#ifdef LIBSEMIGROUPS_DEBUG
              auto [it, inserted] =
#endif
                  _new_rule_map.emplace(node, rule);
              // Shouldn't be possible for 2 rules with equal left-hand
              // sides to exist, since the later added one will be rewritten
              // using the first.
              LIBSEMIGROUPS_ASSERT(inserted);
            }
            rules_added           = true;
            rules_added_this_pass = true;
          } else {
            add_inactive_rule(rule);
          }
          if (!_ticker_running && reporting_enabled()
              && delta(start_time) >= std::chrono::seconds(1)) {
            _ticker_running = true;
            ticker([this, &start_time, &seen]() {
              report_progress_from_thread(seen, start_time);
            });
          }
        }
        if (_ticker_running) {
          report_progress_from_thread(seen, start_time);
        }

        if (rules_added_this_pass) {
          Guard sg(_state);
          _state = State::reducing_pending_rules;

          AhoCorasickImpl* new_rule_trie
              = use_separate_trie ? &_new_rule_trie : &_rule_trie;
          decltype(_rule_map)* rule_map
              = use_separate_trie ? &_new_rule_map : &_rule_map;
          seen = 0;

          for (auto it = begin(); it != end();) {
            ++seen;
            Rule* rule = *it;
            // Check whether any rule contains the left-hand-side of the "new"
            // rule
            bool increment = true;
            for (auto const& word : {rule->lhs(), rule->rhs()}) {
              auto first = begin_search_no_checks(*new_rule_trie, word);
              auto last  = end_search_no_checks(*new_rule_trie, word);

              if (std::any_of(first, last, [rule, rule_map](auto node_index) {
                    return (*rule_map)[node_index] != rule;
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
        if (_ticker_running) {
          report_progress_from_thread(seen, start_time);
        }
      }

      return rules_added;
    }

    bool RewriteTrie::confluent_impl(std::atomic_uint64_t& seen) {
      using std::chrono::time_point;
      time_point start_time = std::chrono::high_resolution_clock::now();

      index_type link;
      set_cached_confluent(tril::TRUE);

      // For each rule, check if any descendent of any suffix breaks
      // confluence
      for (auto node_it = _rule_map.begin(); node_it != _rule_map.end();
           ++node_it) {
        seen++;
        link = _rule_trie.suffix_link_no_checks(node_it->first);
        LIBSEMIGROUPS_ASSERT(node_it->first != _rule_trie.root);
        while (link != _rule_trie.root) {
          if (!descendants_confluent(
                  node_it->second, link, _rule_trie.height_no_checks(link))) {
            set_cached_confluent(tril::FALSE);
            report_checking_confluence(seen, start_time);
            return false;
          }
          link = _rule_trie.suffix_link_no_checks(link);
        }
      }

      report_checking_confluence(seen, start_time);
      return true;
    }

    [[nodiscard]] bool
    RewriteTrie::descendants_confluent(Rule const* rule1,
                                       index_type  current_node,
                                       size_t      overlap_length) const {
      LIBSEMIGROUPS_ASSERT(rule1->active());
      if (_rule_trie.node_no_checks(current_node).terminal()) {
        Rule const* rule2 = _rule_map.find(current_node)->second;
        // Process overlap
        // Word looks like ABC where the LHS of rule1 corresponds to AB,
        // the LHS of rule2 corresponds to BC, and |C|=nodes.size() - 1.
        // AB -> X, BC -> Y
        // ABC gets rewritten to XC and AY
        // TODO(1) remove allocation, use a MultiView, and check equality, then
        // copy inside the if-condition
        native_word_type word1;
        native_word_type word2;

        word1.assign(rule1->rhs());  // X
        word1.append(rule2->lhs().cbegin() + overlap_length,
                     rule2->lhs().cend());  // C

        word2.assign(rule1->lhs().cbegin(),
                     rule1->lhs().cend() - overlap_length);  // A
        word2.append(rule2->rhs());                          // Y

        if (word1 != word2) {
          rewrite(word1);
          rewrite(word2);
          if (word1 != word2) {
            set_cached_confluent(tril::FALSE);
            return false;
          }
        }
        return true;
      }

      // Read each possible letter and traverse down the trie
      for (letter_type x = 0; x != _rule_trie.alphabet_size(); ++x) {
        auto child = _rule_trie.child_no_checks(current_node, x);
        if (child != UNDEFINED) {
          if (!descendants_confluent(rule1, child, overlap_length)) {
            return false;
          }
        }
      }
      return true;
    }

    Rules::iterator RewriteTrie::make_active_rule_pending(Rules::iterator it) {
      Rule* rule = *it;
      rule->deactivate_no_checks();
      add_pending_rule(rule);
      index_type node = _rule_trie.rm_word_no_checks(rule->lhs().cbegin(),
                                                     rule->lhs().cend());
      _rule_map.erase(node);
      return Rules::erase_from_active_rules(it);
    }

    void RewriteTrie::report_checking_confluence(
        std::atomic_uint64_t const&                           seen,
        std::chrono::high_resolution_clock::time_point const& start_time)
        const {
      if (reporting_enabled()) {
        auto total_rules   = Rules::number_of_active_rules();
        auto total_rules_s = detail::group_digits(total_rules);
        auto now           = std::chrono::high_resolution_clock::now();
        auto time          = std::chrono::duration_cast<std::chrono::seconds>(
            now - start_time);
        report_no_prefix("{:-<95}\n", "");
        report_default("KnuthBendix: locally confluent for: {0:>{width}} / "
                       "{1:>{width}} ({2:>4.1f}%) rules ({3}s)\n",
                       detail::group_digits(seen),
                       total_rules_s,
                       (total_rules != 0)
                           ? 100 * static_cast<double>(seen) / total_rules
                           : 100,
                       time.count(),
                       fmt::arg("width", total_rules_s.size()));
      }
    }

    void RewriteTrie::report_reducing_rules(
        std::atomic_uint64_t const&                           seen,
        std::chrono::high_resolution_clock::time_point const& start_time)
        const {
      auto gd = detail::group_digits;
      using detail::string_time;
      if (reporting_enabled()) {
        // TODO(1) This could maybe be better, more like the formatting in
        // "report_progress_from_thread"
        auto total_rules
            = Rules::number_of_active_rules() + number_of_pending_rules();
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
  }  // namespace detail
}  // namespace libsemigroups
