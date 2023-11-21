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

#include "libsemigroups/rewriter-base.hpp"

namespace libsemigroups {
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
}  // namespace libsemigroups
