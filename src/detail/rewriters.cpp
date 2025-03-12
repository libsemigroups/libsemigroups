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
    // Construct from KnuthBendixImpl with new but empty internal_string_type's
    Rule::Rule(int64_t id)
        : _lhs(new internal_string_type()),
          _rhs(new internal_string_type()),
          _id(-1 * id) {
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
      if (_requires_alphabet) {
        _alphabet.clear();
      }
      // Put all active rules and those rules in the stack into the
      // inactive_rules list
      while (!_pending_rules.empty()) {
        Rules::add_inactive_rule(_pending_rules.top());
        _pending_rules.pop();
      }
      _max_stack_depth  = 0;
      _cached_confluent = false;
      _confluence_known = false;
      return *this;
    }

    RewriterBase::~RewriterBase() {
      while (!_pending_rules.empty()) {
        Rule* rule = _pending_rules.top();
        _pending_rules.pop();
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
        _pending_rules.emplace(rule);
        _max_stack_depth = std::max(_max_stack_depth, _pending_rules.size());
        return true;
      } else {
        Rules::add_inactive_rule(rule);
        return false;
      }
    }

    bool RewriterBase::process_pending_rules() {
      bool                        rules_added = false;
      Rule*                       rule1;
      internal_string_type const* lhs;
      while (number_of_pending_rules() != 0) {
        rule1 = next_pending_rule();
        LIBSEMIGROUPS_ASSERT(!rule1->active());
        LIBSEMIGROUPS_ASSERT(*rule1->lhs() != *rule1->rhs());
        // Rewrite both sides and reorder if necessary . . .
        rewrite(rule1);

        // Check rule is non-trivial
        if (*rule1->lhs() != *rule1->rhs()) {
          lhs = rule1->lhs();

          for (auto it = begin(); it != end();) {
            Rule* rule2 = const_cast<Rule*>(*it);

            // Check if lhs is contained within either the lhs or rhs of rule2
            if (rule2->lhs()->find(*lhs) != external_string_type::npos
                || rule2->rhs()->find(*lhs) != external_string_type::npos) {
              // If it is, rule2 must be deactivated and re-processed
              it = make_active_rule_pending(it);
            } else {
              ++it;
            }
          }
          add_rule(rule1);
          if (!rules_added) {
            rules_added = true;
          }
        } else {
          add_inactive_rule(rule1);
        }
      }
      // reduce_rhs();
      return rules_added;
    }

    void RewriterBase::reduce() {
      for (Rule const* rule : *this) {
        // Copy rule and add_pending_rule so that it is not modified by the
        // call to process_pending_rules.
        LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
        if (add_pending_rule(copy_rule(rule))) {
          process_pending_rules();
        }
      }
    }

    void RewriterBase::reduce_rhs() {
      for (Rule const* rule : *this) {
        rewrite(*rule->rhs());
      }
    }

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
    void RewriteFromLeft::rewrite(internal_string_type& u) const {
      // Add underscore because there's already an "iterator" alias.
      using iterator_ = internal_string_type::iterator;

      if (u.size() < stats().min_length_lhs_rule) {
        return;
      }

      iterator_ v_begin = u.begin();
      iterator_ v_end   = u.begin() + stats().min_length_lhs_rule - 1;
      iterator_ w_begin = v_end;
      iterator_ w_end   = u.end();

      RuleLookup lookup;

      while (w_begin != w_end) {
        *v_end = *w_begin;
        ++v_end;
        ++w_begin;

        auto it = _set_rules.find(lookup(v_begin, v_end));
        if (it != _set_rules.end()) {
          Rule const* rule = (*it).rule();
          if (rule->lhs()->size() <= static_cast<size_t>(v_end - v_begin)) {
            LIBSEMIGROUPS_ASSERT(detail::is_suffix(
                v_begin, v_end, rule->lhs()->cbegin(), rule->lhs()->cend()));
            v_end -= rule->lhs()->size();
            w_begin -= rule->rhs()->size();
            detail::string_replace(
                w_begin, rule->rhs()->cbegin(), rule->rhs()->cend());
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

    void RewriteFromLeft::rewrite(Rule* rule) const {
      // LIBSEMIGROUPS_ASSERT(_id != 0);
      rewrite(*rule->lhs());
      rewrite(*rule->rhs());
      rule->reorder();
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
      internal_string_type word1;
      internal_string_type word2;

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
      for (auto* crule : *this) {
        Rule* rule = const_cast<Rule*>(crule);
        add_rule_to_trie(rule);
      }
      return *this;
    }

    void RewriteTrie::all_overlaps() {
      // For each active rule, get the corresponding terminal node.
      for (auto node_it = _rules.begin(); node_it != _rules.end(); ++node_it) {
        index_type link = _trie.suffix_link_no_checks(node_it->first);
        while (link != _trie.root) {
          // For each suffix link, add an overlap between rule and every other
          // rule that corresponds to a terminal descendant of link
          add_overlaps(node_it->second, link, _trie.height_no_checks(link));
          link = _trie.suffix_link_no_checks(link);
        }
      }
    }

    void RewriteTrie::rule_overlaps(index_type node) {
      index_type link = _trie.suffix_link_no_checks(node);
      while (link != _trie.root) {
        // For each suffix link, add an overlap between rule and every other
        // rule that corresponds to a terminal descendant of link
        add_overlaps(_rules[node], link, _trie.height_no_checks(link));
        link = _trie.suffix_link_no_checks(link);
      }
    }

    void RewriteTrie::add_overlaps(Rule*      rule,
                                   index_type node,
                                   size_t     overlap_length) {
      // BFS find the terminal descendants of node and add overlaps with rule
      if (_trie.node_no_checks(node).is_terminal()) {
        Rule const*             rule2 = _rules.find(node)->second;
        detail::MultiStringView x(rule->lhs()->cbegin(),
                                  rule->lhs()->cend() - overlap_length);
        x.append(rule2->rhs()->cbegin(), rule2->rhs()->cend());
        detail::MultiStringView y(rule->rhs()->cbegin(), rule->rhs()->cend());
        y.append(rule2->lhs()->cbegin() + overlap_length,
                 rule2->lhs()->cend());  // rule = AQ_j -> Q_iC
        add_pending_rule(x, y);
      }
      for (auto a = alphabet_cbegin(); a != alphabet_cend(); ++a) {
        auto child = _trie.child_no_checks(node, static_cast<letter_type>(*a));
        if (child != UNDEFINED) {
          add_overlaps(rule, child, overlap_length);
        }
      }
    }

    // As with RewriteFromLeft::rewrite, this assumes that all rules are length
    // reducing.
    void RewriteTrie::rewrite(internal_string_type& u) const {
      // Check if u is rewriteable
      if (u.size() < stats().min_length_lhs_rule) {
        return;
      }

      std::vector<index_type> nodes;
      index_type              current = _trie.root;
      nodes.push_back(current);

#ifdef LIBSEMIGROUPS_DEBUG
      iterator v_begin = u.begin();
#endif
      iterator v_end   = u.begin();
      iterator w_begin = v_end;
      iterator w_end   = u.end();

      while (w_begin != w_end) {
        // Read first letter of W and traverse trie
        auto x = *w_begin;
        ++w_begin;
        current
            = _trie.traverse_no_checks(current, static_cast<letter_type>(x));

        if (!_trie.node_no_checks(current).is_terminal()) {
          nodes.push_back(current);
          *v_end = x;
          ++v_end;
        } else {
          // Find rule that corresponds to terminal node
          Rule const* rule     = _rules.find(current)->second;
          auto        lhs_size = rule->lhs()->size();
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
          for (size_t i = 0; i < lhs_size - 1; ++i) {
            nodes.pop_back();
          }
          current = nodes.back();
        }
      }
      u.erase(v_end - u.cbegin());
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

      // For each rule, check if any descendent of any suffix breaks confluence
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
      if (_trie.node_no_checks(current_node).is_terminal()) {
        Rule const* rule2 = _rules.find(current_node)->second;
        // Process overlap
        // Word looks like ABC where the LHS of rule1 corresponds to AB,
        // the LHS of rule2 corresponds to BC, and |C|=nodes.size() - 1.
        // AB -> X, BC -> Y
        // ABC gets rewritten to XC and AY

        internal_string_type word1;
        internal_string_type word2;

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
      for (auto x = alphabet_cbegin(); x != alphabet_cend(); ++x) {
        auto child
            = _trie.child_no_checks(current_node, static_cast<letter_type>(*x));
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
      LIBSEMIGROUPS_ASSERT(_trie.number_of_nodes() == number_of_active_rules());
    }
  }  // namespace detail
}  // namespace libsemigroups
