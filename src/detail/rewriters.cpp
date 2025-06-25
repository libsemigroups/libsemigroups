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

#include "libsemigroups/detail/report.hpp"  // for report_default
#include "libsemigroups/runner.hpp"         // for Ticker
#include <algorithm>
#include <chrono>

namespace libsemigroups {
  namespace detail {
    // Construct from KnuthBendixImpl with new but empty std::string's
    Rule::Rule(int64_t id)
        : _lhs(new std::string()), _rhs(new std::string()), _id(-1 * id) {
      LIBSEMIGROUPS_ASSERT(_id < 0);
    }

    void Rule::deactivate() noexcept {
      LIBSEMIGROUPS_ASSERT(_id != 0);
      if (active()) {
        _id *= -1;
      }
    }

    void Rule::activate() noexcept {
      LIBSEMIGROUPS_ASSERT(_id != 0);
      if (!active()) {
        _id *= -1;
      }
    }

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
      for (Rule const* cptr : _active_rules) {
        Rule* ptr = const_cast<Rule*>(cptr);
        ptr->deactivate();
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

    Rules::~Rules() {
      for (Rule const* rule : _active_rules) {
        delete const_cast<Rule*>(rule);
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
        rule->set_id(_stats.total_rules);
        _inactive_rules.erase(_inactive_rules.begin());
      } else {
        rule = new Rule(_stats.total_rules);
      }
      LIBSEMIGROUPS_ASSERT(!rule->active());
      return rule;
    }

    Rule* Rules::copy_rule(Rule const* rule) {
      return new_rule(rule->lhs()->cbegin(),
                      rule->lhs()->cend(),
                      rule->rhs()->cbegin(),
                      rule->rhs()->cend());
    }

    Rules::iterator Rules::erase_from_active_rules(iterator it) {
      // _stats.unique_lhs_rules.erase(*((*it)->lhs()));
      Rule* rule = const_cast<Rule*>(*it);
      rule->deactivate();

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
      LIBSEMIGROUPS_ASSERT(*rule->lhs() != *rule->rhs());
      _stats.max_word_length
          = std::max(_stats.max_word_length, rule->lhs()->size());
      _stats.max_active_rules
          = std::max(_stats.max_active_rules, number_of_active_rules());
      // _stats.unique_lhs_rules.insert(*rule->lhs());
      rule->activate();
      _active_rules.push_back(rule);
      for (auto& it : _cursors) {
        if (it == end()) {
          --it;
        }
      }
      if (rule->lhs()->size() < _stats.min_length_lhs_rule) {
        // TODO(later) this is not valid when using non-length reducing
        // orderings (such as RECURSIVE)
        _stats.min_length_lhs_rule = rule->lhs()->size();
      }
    }

    size_t Rules::max_active_word_length() const {
      auto comp = [](Rule const* p, Rule const* q) -> bool {
        return p->lhs()->size() < q->lhs()->size();
      };
      auto max = std::max_element(begin(), end(), comp);
      if (max != end()) {
        _stats.max_active_word_length
            = std::max(_stats.max_active_word_length, (*max)->lhs()->size());
      }
      return _stats.max_active_word_length;
    }

    RewriterBase& RewriterBase::init() {
      Rules::init();
      // Put all active rules and those rules in the stack into the
      // inactive_rules list
      while (!_pending_rules.empty()) {
        Rules::add_inactive_rule(_pending_rules.back());
        _pending_rules.pop_back();
      }
      _max_stack_depth  = 0;
      _cached_confluent = false;
      _confluence_known = false;
      return *this;
    }

    RewriterBase::~RewriterBase() {
      while (!_pending_rules.empty()) {
        Rule* rule = _pending_rules.back();
        _pending_rules.pop_back();
        delete rule;
      }
    }

    void RewriterBase::set_cached_confluent(tril val) const {
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

    bool RewriterBase::add_pending_rule(Rule* rule) {
      LIBSEMIGROUPS_ASSERT(!rule->active());
      if (*rule->lhs() != *rule->rhs()) {
        _pending_rules.emplace_back(rule);
        _max_stack_depth = std::max(_max_stack_depth, _pending_rules.size());
        return true;
      } else {
        Rules::add_inactive_rule(rule);
        return false;
      }
    }

    void RewriterBase::report_progress_from_thread(
        std::chrono::high_resolution_clock::time_point start_time) {
      using detail::group_digits;
      using detail::signed_group_digits;
      using std::chrono::duration_cast;
      using std::chrono::nanoseconds;

      using high_resolution_clock = std::chrono::high_resolution_clock;

      auto active   = number_of_active_rules();
      auto inactive = number_of_inactive_rules();
      auto pending  = number_of_pending_rules();
      auto defined  = stats().total_rules;

      auto run_time = duration_cast<nanoseconds>(high_resolution_clock::now()
                                                 - start_time);
      report_default(
          "KnuthBendix: rules {} (active) | {} (inactive) | {} (pending) | {} "
          "(defined) | {}\n",
          group_digits(active),
          group_digits(inactive),
          group_digits(pending),
          group_digits(defined),
          detail::string_time(run_time));
    }

    bool RewriterBase::process_pending_rules() {
      std::sort(
          _pending_rules.begin(),
          _pending_rules.end(),
          [](Rule const* x, Rule const* y) { return *x->lhs() > *y->lhs(); });

      auto           start_time = std::chrono::high_resolution_clock::now();
      detail::Ticker ticker;
      bool           ticker_running = false;  // TODO(0) do this properly with a
                                              // data member
      bool rules_added = false;

      while (number_of_pending_rules() != 0) {
        Rule* rule1 = next_pending_rule();
        LIBSEMIGROUPS_ASSERT(!rule1->active());
        LIBSEMIGROUPS_ASSERT(*rule1->lhs() != *rule1->rhs());
        // Rewrite both sides and reorder if necessary . . .
        rewrite(rule1);

        // Check rule is non-trivial
        if (*rule1->lhs() != *rule1->rhs()) {
          std::string* lhs = rule1->lhs();

          for (auto it = begin(); it != end();) {
            Rule* rule2 = const_cast<Rule*>(*it);

            // Check if lhs is contained within either the lhs or rhs of rule2
            // TODO(0) investigate whether or not this can be improved?
            // Removed?
            if (rule2->lhs()->find(*lhs) != std::string::npos
                || rule2->rhs()->find(*lhs) != std::string::npos) {
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
        if (!ticker_running && reporting_enabled()
            && delta(start_time) >= std::chrono::seconds(1)) {
          ticker_running = true;
          ticker([this, start_time]() {
            report_progress_from_thread(start_time);
          });
        }
      }
      return rules_added;
    }

    void RewriterBase::reduce() {
      // TODO required?
      for (Rule const* rule : *this) {
        // Copy rule and add_pending_rule so that it is not modified by the
        // call to process_pending_rules.
        LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
        if (add_pending_rule(copy_rule(rule))) {
          process_pending_rules();
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // RewriteFromLeft
    ////////////////////////////////////////////////////////////////////////

    RewriteFromLeft::~RewriteFromLeft() = default;

    RewriteFromLeft& RewriteFromLeft::init() {
      RewriterBase::init();
      _set_rules.clear();
      return *this;
    }

    RewriteFromLeft& RewriteFromLeft::operator=(RewriteFromLeft const& that) {
      init();
      RewriterBase::operator=(that);
      for (auto* crule : that) {
        Rule* rule = const_cast<Rule*>(crule);
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
      Rule* rule = const_cast<Rule*>(*it);
      rule->deactivate();
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
    bool
    RewriteFromLeft::rewrite_with_disabled_rule(std::string& u,
                                                Rule const*  disabled_rule) {
      bool did_rewrite = false;
      if (u.size() < stats().min_length_lhs_rule) {
        return did_rewrite;
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
          if (rule != disabled_rule) {
            if (rule->lhs()->size() <= static_cast<size_t>(v_end - v_begin)) {
              LIBSEMIGROUPS_ASSERT(detail::is_suffix(
                  v_begin, v_end, rule->lhs()->cbegin(), rule->lhs()->cend()));
              v_end -= rule->lhs()->size();
              w_begin -= rule->rhs()->size();
              detail::string_replace(
                  w_begin, rule->rhs()->cbegin(), rule->rhs()->cend());
              did_rewrite = true;
            }
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
      return did_rewrite;
    }

    void RewriteFromLeft::report_from_confluent(
        std::atomic_uint64_t const&                           seen,
        std::chrono::high_resolution_clock::time_point const& start_time)
        const {
      auto total_pairs   = std::pow(Rules::number_of_active_rules(), 2);
      auto total_pairs_s = detail::group_digits(total_pairs);
      auto now           = std::chrono::high_resolution_clock::now();
      auto time
          = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
      if (reporting_enabled()) {
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

    bool RewriteFromLeft::confluent_impl(std::atomic_uint64_t& seen) const {
      using std::chrono::time_point;
      time_point start_time = std::chrono::high_resolution_clock::now();

      set_cached_confluent(tril::TRUE);
      std::string word1;
      std::string word2;

      for (auto it1 = begin(); it1 != end(); ++it1) {
        Rule const* rule1 = *it1;
        // Seems to be much faster to do this in reverse.
        for (auto it2 = rbegin(); it2 != rend(); ++it2) {
          seen++;
          Rule const* rule2 = *it2;
          for (auto it = rule1->lhs()->cend() - 1; it >= rule1->lhs()->cbegin();
               --it) {
            // Find longest common prefix of suffix B of rule1.lhs() defined by
            // it and R = rule2.lhs()
            auto prefix = detail::maximum_common_prefix(it,
                                                        rule1->lhs()->cend(),
                                                        rule2->lhs()->cbegin(),
                                                        rule2->lhs()->cend());
            if (prefix.first == rule1->lhs()->cend()
                || prefix.second == rule2->lhs()->cend()) {
              // Seems that this function isn't called enough to merit using
              // MSV's here.
              word1.assign(rule1->lhs()->cbegin(),
                           it);             // A
              word1.append(*rule2->rhs());  // S
              word1.append(prefix.first,
                           rule1->lhs()->cend());  // D

              word2.assign(*rule1->rhs());  // Q
              word2.append(prefix.second,
                           rule2->lhs()->cend());  // E

              if (word1 != word2) {
                rewrite(word1);
                rewrite(word2);
                if (word1 != word2) {
                  set_cached_confluent(tril::FALSE);
                  if (reporting_enabled()) {
                    report_from_confluent(seen, start_time);
                  }
                  return false;
                }
              }
            }
          }
        }
      }
      if (reporting_enabled()) {
        report_from_confluent(seen, start_time);
      }
      return cached_confluent();
    }

    bool RewriteFromLeft::confluent() const {
      if (number_of_pending_rules() != 0) {
        set_cached_confluent(tril::unknown);
        return false;
      } else if (confluence_known()) {
        return RewriterBase::cached_confluent();
      }
      std::atomic_uint64_t seen = 0;
      if (reporting_enabled()) {
        using std::chrono::time_point;
        time_point     start_time = std::chrono::high_resolution_clock::now();
        detail::Ticker t([&]() { report_from_confluent(seen, start_time); });
        return confluent_impl(seen);
      } else {
        return confluent_impl(seen);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // RewriteTrie
    ////////////////////////////////////////////////////////////////////////

    RewriteTrie::~RewriteTrie() = default;

    RewriteTrie& RewriteTrie::init() {
      RewriterBase::init();
      _trie.init();
      _rules.clear();
      return *this;
    }

    RewriteTrie& RewriteTrie::operator=(RewriteTrie const& that) {
      init();
      RewriterBase::operator=(that);
      _trie = that._trie;
      for (auto* crule : *this) {
        Rule*      rule = const_cast<Rule*>(crule);
        index_type node = _trie.traverse_trie_no_checks(rule->lhs()->cbegin(),
                                                        rule->lhs()->cend());
        // TODO check that node is correct
        // LIBSEMIGROUPS_ASSERT(_trie.terminal(node));
        _rules.emplace(node, rule);
      }

      return *this;
    }

    // As with RewriteFromLeft::rewrite, this assumes that all rules are length
    // reducing.
    bool RewriteTrie::rewrite_with_disabled_rule(std::string& u,
                                                 Rule const*  disabled_rule) {
      bool did_rewrite = false;
      // Check if u is rewriteable
      if (u.size() < stats().min_length_lhs_rule) {
        return did_rewrite;
      }

      _nodes.clear();
      index_type current = _trie.root;
      _nodes.push_back(current);

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
        current
            = _trie.traverse_no_checks(current, static_cast<letter_type>(x));

        auto rule_it = _rules.find(current);
        if (!_trie.node_no_checks(current).terminal()
            || rule_it->second == disabled_rule) {
          _nodes.push_back(current);
          *v_end = x;
          ++v_end;
        } else {
          // Find rule that corresponds to terminal node
          Rule const* rule = rule_it->second;
          did_rewrite      = true;
          auto lhs_size    = rule->lhs()->size();
          LIBSEMIGROUPS_ASSERT(lhs_size != 0);

          // Check the lhs is smaller than the portion of the word that has
          // been read
          LIBSEMIGROUPS_ASSERT(lhs_size
                               <= static_cast<size_t>(v_end - v_begin) + 1);
          v_end -= lhs_size - 1;
          w_begin -= rule->rhs()->size();
          // Replace lhs with rhs in-place
          detail::string_replace(
              w_begin, rule->rhs()->cbegin(), rule->rhs()->cend());
          _nodes.erase(_nodes.end() - lhs_size + 1, _nodes.end());
          current = _nodes.back();
        }
      }
      u.erase(v_end - u.cbegin());
      return did_rewrite;
    }

    bool RewriteTrie::process_pending_rules() {
      using detail::aho_corasick_impl::begin_search_no_checks;
      using detail::aho_corasick_impl::end_search_no_checks;
      auto           start_time = std::chrono::high_resolution_clock::now();
      detail::Ticker ticker;
      bool           ticker_running = false;  // TODO(0) do this properly with a
      // TODO use a heap for these maybe?
      std::sort(
          _pending_rules.begin(),
          _pending_rules.end(),
          [](Rule const* x, Rule const* y) { return *x->lhs() > *y->lhs(); });

      bool                    rules_added = false;
      detail::AhoCorasickImpl _new_rule_trie;
      std::unordered_map<AhoCorasickImpl::index_type, Rule*> _new_rule_map;

      while (number_of_pending_rules() != 0) {
        bool rules_added_this_pass = false;
        _new_rule_trie.init(_trie.alphabet_size());
        _new_rule_map.clear();
        while (number_of_pending_rules() != 0) {
          Rule* rule = next_pending_rule();
          LIBSEMIGROUPS_ASSERT(!rule->active());
          LIBSEMIGROUPS_ASSERT(*rule->lhs() != *rule->rhs());
          // Rewrite both sides and reorder if necessary . . .
          rewrite(rule);

          if (*rule->lhs() != *rule->rhs()) {
            add_rule(rule);
            auto node_index = detail::aho_corasick_impl::add_word_no_checks(
                _new_rule_trie, *rule->lhs());
#ifdef LIBSEMIGROUPS_DEBUG
            auto [it, inserted] =
#endif
                _new_rule_map.emplace(node_index, rule);
            // Shouldn't be possible for 2 rules with equal left-hand sides to
            // exist, since the later added one will be rewritten using the
            // first.
            LIBSEMIGROUPS_ASSERT(inserted);
            rules_added           = true;
            rules_added_this_pass = true;
          } else {
            add_inactive_rule(rule);
          }
          if (!ticker_running && reporting_enabled()
              && delta(start_time) >= std::chrono::seconds(1)) {
            ticker_running = true;
            ticker([this, start_time]() {
              report_progress_from_thread(start_time);
            });
          }
        }

        if (rules_added_this_pass) {
          for (auto it = begin(); it != end();) {
            Rule* rule = *it;
            // Check whether any rule contains the left-hand-side of the "new"
            // rule
            {
              auto first = begin_search_no_checks(_new_rule_trie, *rule->lhs());
              auto last  = end_search_no_checks(_new_rule_trie, *rule->lhs());

              if (std::any_of(
                      first, last, [&rule, &_new_rule_map](auto node_index) {
                        return _new_rule_map[node_index] != rule;
                      })) {
                it = make_active_rule_pending(it);
                continue;
              }
            }
            {
              auto first = begin_search_no_checks(_new_rule_trie, *rule->rhs());
              auto last  = end_search_no_checks(_new_rule_trie, *rule->rhs());

              if (std::any_of(
                      first, last, [&rule, &_new_rule_map](auto node_index) {
                        return _new_rule_map[node_index] != rule;
                      })) {
                it = make_active_rule_pending(it);
              } else {
                ++it;
              }
            }
          }
        }
      }
      return rules_added;
    }

    bool RewriteTrie::confluent() const {
      if (number_of_pending_rules() != 0) {
        set_cached_confluent(tril::unknown);
        return false;
      } else if (confluence_known()) {
        return RewriterBase::cached_confluent();
      }

      std::atomic_uint64_t seen = 0;
      if (reporting_enabled()) {
        using std::chrono::time_point;
        time_point     start_time = std::chrono::high_resolution_clock::now();
        detail::Ticker t([&]() { report_from_confluent(seen, start_time); });
        return confluent_impl(seen);
      } else {
        return confluent_impl(seen);
      }
    }

    void RewriteTrie::report_from_confluent(
        std::atomic_uint64_t const&                           seen,
        std::chrono::high_resolution_clock::time_point const& start_time)
        const {
      auto total_rules   = Rules::number_of_active_rules();
      auto total_rules_s = detail::group_digits(total_rules);
      auto now           = std::chrono::high_resolution_clock::now();
      auto time
          = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
      if (reporting_enabled()) {
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

    bool RewriteTrie::confluent_impl(std::atomic_uint64_t& seen) const {
      using std::chrono::time_point;
      time_point start_time = std::chrono::high_resolution_clock::now();

      index_type link;
      set_cached_confluent(tril::TRUE);

      // For each rule, check if any descendent of any suffix breaks
      // confluence
      for (auto node_it = _rules.begin(); node_it != _rules.end(); ++node_it) {
        seen++;
        link = _trie.suffix_link_no_checks(node_it->first);
        LIBSEMIGROUPS_ASSERT(node_it->first != _trie.root);
        while (link != _trie.root) {
          if (!descendants_confluent(
                  node_it->second, link, _trie.height_no_checks(link))) {
            set_cached_confluent(tril::FALSE);
            if (reporting_enabled()) {
              report_from_confluent(seen, start_time);
            }
            return false;
          }
          link = _trie.suffix_link_no_checks(link);
        }
      }

      if (reporting_enabled()) {
        report_from_confluent(seen, start_time);
      }
      // Set cached value
      // set_cached_confluent(tril::TRUE);
      return true;
    }

    [[nodiscard]] bool
    RewriteTrie::descendants_confluent(Rule const* rule1,
                                       index_type  current_node,
                                       size_t      overlap_length) const {
      LIBSEMIGROUPS_ASSERT(rule1->active());
      if (_trie.node_no_checks(current_node).terminal()) {
        Rule const* rule2 = _rules.find(current_node)->second;
        // Process overlap
        // Word looks like ABC where the LHS of rule1 corresponds to AB,
        // the LHS of rule2 corresponds to BC, and |C|=nodes.size() - 1.
        // AB -> X, BC -> Y
        // ABC gets rewritten to XC and AY

        std::string word1;
        std::string word2;

        word1.assign(*rule1->rhs());  // X
        word1.append(rule2->lhs()->cbegin() + overlap_length,
                     rule2->lhs()->cend());  // C

        word2.assign(rule1->lhs()->cbegin(),
                     rule1->lhs()->cend() - overlap_length);  // A
        word2.append(*rule2->rhs());                          // Y

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
      for (letter_type x = 0; x != _trie.alphabet_size(); ++x) {
        auto child = _trie.child_no_checks(current_node, x);
        if (child != UNDEFINED) {
          if (!descendants_confluent(rule1, child, overlap_length)) {
            return false;
          }
        }
      }
      return true;
    }

    Rules::iterator RewriteTrie::make_active_rule_pending(Rules::iterator it) {
      Rule* rule = const_cast<Rule*>(*it);
      rule->deactivate();  // Done in Rules::erase_from
      add_pending_rule(rule);
      index_type node
          = _trie.rm_word_no_checks(rule->lhs()->cbegin(), rule->lhs()->cend());
      _rules.erase(node);
      return Rules::erase_from_active_rules(it);
    }
  }  // namespace detail
}  // namespace libsemigroups
