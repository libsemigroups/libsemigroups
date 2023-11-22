//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Joe Edwards + James D. Mitchell
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

#include "libsemigroups/rewriters.hpp"

namespace libsemigroups {
  // Construct from KnuthBendix with new but empty internal_string_type's
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
    while (it_this > _first && it_that > that._first && *it_this == *it_that) {
      --it_that;
      --it_this;
    }
    return *it_this < *it_that;
  }

  Rules::Stats::Stats() noexcept {
    init();
  }

  Rules::Stats& Rules::Stats::init() noexcept {
    max_stack_depth        = 0;
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
  Rewriter& Rewriter::init() {
    Rules::init();
    if (_requires_alphabet) {
      _alphabet.clear();
    }
    // Put all active rules and those rules in the stack into the
    // inactive_rules list
    while (!_stack.empty()) {
      Rules::add_inactive_rule(_stack.top());
      _stack.pop();
    }
    _confluent        = false;
    _confluence_known = false;
    return *this;
  }

  Rewriter::~Rewriter() {
    while (!_stack.empty()) {
      Rule* rule = _stack.top();
      _stack.pop();
      delete rule;
    }
  }

  bool Rewriter::push_stack(Rule* rule) {
    LIBSEMIGROUPS_ASSERT(!rule->active());
    if (*rule->lhs() != *rule->rhs()) {
      _stack.emplace(rule);
      return true;
    } else {
      Rules::add_inactive_rule(rule);
      return false;
    }
  }

  void Rewriter::clear_stack() {
    while (number_of_pending_rules() != 0) {
      // _stats.max_stack_depth = std::max(_stats.max_stack_depth,
      // _stack.size());

      Rule* rule1 = next_pending_rule();
      LIBSEMIGROUPS_ASSERT(!rule1->active());
      LIBSEMIGROUPS_ASSERT(*rule1->lhs() != *rule1->rhs());
      // Rewrite both sides and reorder if necessary . . .
      rewrite(rule1);

      if (*rule1->lhs() != *rule1->rhs()) {
        internal_string_type const* lhs = rule1->lhs();
        for (auto it = begin(); it != end();) {
          Rule* rule2 = const_cast<Rule*>(*it);
          // TODO Does this need to happen? Can we ensure rules are always
          // reduced wrt each other?
          if (rule2->lhs()->find(*lhs) != external_string_type::npos) {
            it = erase_from_active_rules(it);
            // rule2 is added to _inactive_rules or _active_rules by
            // clear_stack
          } else {
            if (rule2->rhs()->find(*lhs) != external_string_type::npos) {
              rewrite(*rule2->rhs());
            }
            ++it;
          }
        }
        add_rule(rule1);
        // rule1 is activated, we do this after removing rules that rule1
        // makes redundant to avoid failing to insert rule1 in _set_rules
      } else {
        add_inactive_rule(rule1);
      }
    }
  }

  RewriteFromLeft& RewriteFromLeft::init() {
    Rewriter::init();
    _set_rules.clear();
    return *this;
  }

  RewriteFromLeft& RewriteFromLeft::operator=(RewriteFromLeft const& that) {
    init();
    Rewriter::operator=(that);
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
  RewriteFromLeft::erase_from_active_rules(iterator it) {
    Rule* rule = const_cast<Rule*>(*it);
    rule->deactivate();
    push_stack(rule);
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
    confluent(tril::unknown);
  }

  // REWRITE_FROM_LEFT from Sims, p67
  // Caution: this uses the assumption that rules are length reducing, if they
  // are not, then u might not have sufficient space!
  void RewriteFromLeft::rewrite(internal_string_type& u) const {
    using iterator = internal_string_type::iterator;

    if (u.size() < stats().min_length_lhs_rule) {
      return;
    }

    iterator v_begin = u.begin();
    iterator v_end   = u.begin() + stats().min_length_lhs_rule - 1;
    iterator w_begin = v_end;
    iterator w_end   = u.end();

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

  void RewriteFromLeft::reduce() {
    for (Rule const* rule : *this) {
      // Copy rule and push_stack so that it is not modified by the
      // call to clear_stack.
      LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
      if (push_stack(copy_rule(rule))) {
        clear_stack();
      }
    }
  }

  bool RewriteFromLeft::confluent() const {
    if (number_of_pending_rules() != 0) {
      return false;
    } else if (confluence_known()) {
      return Rewriter::confluent();
    }
    // bool reported = false;
    confluent(tril::TRUE);
    internal_string_type word1;
    internal_string_type word2;
    // size_t               seen = 0;

    for (auto it1 = begin(); it1 != end(); ++it1) {
      Rule const* rule1 = *it1;
      // Seems to be much faster to do this in reverse.
      for (auto it2 = rbegin(); it2 != rend(); ++it2) {
        // seen++;
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
                confluent(tril::FALSE);
                return false;
              }
            }
          }
        }
      }
    }
    return confluent();
  }
}  // namespace libsemigroups
