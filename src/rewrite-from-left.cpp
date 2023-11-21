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

// This file contains the implementation of the RewriteFromLeft class.

#include "libsemigroups/rewrite-from-left.hpp"

namespace libsemigroups {
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
