//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards + James D. Mitchell
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

#include "libsemigroups/detail/rules.hpp"

#include <algorithm>  // for max, min, sort
#include <limits>     // for numeric_limits
#include <list>       // for list
#include <vector>     // for swap

namespace libsemigroups {
  namespace detail {
    ////////////////////////////////////////////////////////////////////////
    // OverlapMeasure
    ////////////////////////////////////////////////////////////////////////

    size_t ABC::operator()(Rule const*                        AB,
                           Rule const*                        BC,
                           std::string::const_iterator const& it) const {
      LIBSEMIGROUPS_ASSERT(AB->state() == Rule::State::active
                           && BC->state() == Rule::State::active);
      LIBSEMIGROUPS_ASSERT(AB->lhs().cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs().cend());
      // |A| + |BC|
      return (it - AB->lhs().cbegin()) + BC->lhs().size();
    }

    size_t AB_BC::operator()(Rule const*                        AB,
                             Rule const*                        BC,
                             std::string::const_iterator const& it) const {
      LIBSEMIGROUPS_ASSERT(AB->state() == Rule::State::active
                           && BC->state() == Rule::State::active);
      LIBSEMIGROUPS_ASSERT(AB->lhs().cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs().cend());
      (void) it;
      // |AB| + |BC|
      return AB->lhs().size() + BC->lhs().size();
    }

    size_t MAX_AB_BC::operator()(Rule const*                        AB,
                                 Rule const*                        BC,
                                 std::string::const_iterator const& it) const {
      LIBSEMIGROUPS_ASSERT(AB->state() == Rule::State::active
                           && BC->state() == Rule::State::active);
      LIBSEMIGROUPS_ASSERT(AB->lhs().cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs().cend());
      (void) it;
      // max(|AB|, |BC|)
      return std::max(AB->lhs().size(), BC->lhs().size());
    }

    ////////////////////////////////////////////////////////////////////////
    // Rules::Stats
    ////////////////////////////////////////////////////////////////////////

    Rules::Stats::Stats() noexcept {
      init();
    }

    Rules::Stats& Rules::Stats::init() noexcept {
      max_active_rules    = 0;
      max_length_lhs_rule = 0;
      max_pending_rules   = 0;
      min_length_lhs_rule = std::numeric_limits<size_t>::max();
      total_rules         = 0;
      return *this;
    }

    void Rules::Stats::update_after_active_rule_added(Rules const& rules) {
      Rule const* rule    = rules.active_rules().back();
      max_length_lhs_rule = std::max(max_length_lhs_rule, rule->lhs().size());
      min_length_lhs_rule = std::min(min_length_lhs_rule, rule->lhs().size());
      max_active_rules
          = std::max(max_active_rules, rules.active_rules().size());
    }

    ////////////////////////////////////////////////////////////////////////
    // Rules - private
    ////////////////////////////////////////////////////////////////////////

    void Rules::init_cursors() {
      for (auto& cursor : _cursors) {
        cursor.it      = _active_rules.end();
        cursor.version = 0;
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Rules - constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    Rules::Rules()
        : _active_rules(),
          _cursors(),
          _inactive_rules(),
          _pending_rules(),
          _stats() {
      init_cursors();
    }

    Rules& Rules::init() {
      // Leave _overlaps as is, will be reset when next required
      _stats.init();

      for (Rule* rule : _active_rules) {
        add_inactive_rule(rule);
      }
      _active_rules.clear();

      for (Rule* rule : _pending_rules) {
        add_inactive_rule(rule);
      }
      _pending_rules.clear();
      init_cursors();

      return *this;
    }

    Rules& Rules::operator=(Rules const& that) {
      init();
      for (Rule const* rule : that._active_rules) {
        add_active_rule(copy_rule(rule));
      }
      for (Rule const* rule : that._pending_rules) {
        add_pending_rule(copy_rule(rule));
      }
      // NOTE: copy the stats after calling add_active_rule and add_pending_rule
      // because they also set values in the stats, that we don't want to
      // retain. This does some unnecessary work, but we'll optimize that if it
      // is an issue later. A similar comment applies to _cursors.
      _stats = that._stats;
      // It seems to be too hard to keep the cursors alive across copy
      // construction, so we don't try.
      init_cursors();
      // Similarly, there's no point in doing anything for _overlaps
      return *this;
    }

    Rules& Rules::operator=(Rules&& that) {
      // We swap to ensure that all rules are properly deleted
      std::swap(_active_rules, that._active_rules);
      std::swap(_inactive_rules, that._inactive_rules);
      std::swap(_pending_rules, that._pending_rules);
      // It seems to be too hard to keep the cursors alive across move
      // construction, so we don't try.
      init_cursors();
      _stats = std::move(that._stats);
      return *this;
    }

    Rules::~Rules() {
      for (Rule* rule : _active_rules) {
        delete rule;
      }
      for (Rule* rule : _inactive_rules) {
        delete rule;
      }
      for (Rule* rule : _pending_rules) {
        delete rule;
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Rules - Adding/modifying rules - public
    ////////////////////////////////////////////////////////////////////////

    void Rules::add_active_rule(Rule* rule) {
      LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
      // Don't assert that rule isn't active, because it could be if we are
      // calling this in one of the copy constructors.
      rule->state(Rule::State::active);
      _active_rules.push_back(rule);
      _stats.update_after_active_rule_added(*this);
      // In a std::list like _active_rules, end() points at a dummy object,
      // which does not change when push_back is called. The cursors are like
      // "persistent iterators" meaning that they always point at a valid entry
      // of _active_rules. They are intended to be used in loops, if any of them
      // point at end(), then in any loop we'd consider the loop finished, but
      // now we have just added a new element at the end of _active_rules, and
      // so we should have our cursor point at that new element instead.
      for (auto& cursor : _cursors) {
        if (cursor.it == _active_rules.end()) {
          --cursor.it;
        }
      }
    }

    Rules::iterator Rules::make_active_rule_pending(iterator it) {
      LIBSEMIGROUPS_ASSERT((*it)->state() == Rule::State::active);
      LIBSEMIGROUPS_ASSERT((*it)->state() == Rule::State::active);
      LIBSEMIGROUPS_ASSERT(it != _active_rules.end());
      add_pending_rule(*it);
      bool const cursor0_was_it = (_cursors[0].it == it);
      bool const cursor1_was_it = (_cursors[1].it == it);

      auto next = _active_rules.erase(it);

      if (cursor1_was_it) {
        ++_cursors[1].version;
        _cursors[1].it = next;
      }

      if (cursor0_was_it) {
        ++_cursors[0].version;
        _cursors[0].it = next;
        if (next != _active_rules.begin()) {
          --_cursors[0].it;
        }
      }

      return next;
    }

    ////////////////////////////////////////////////////////////////////////
    // Rules - Getting rules - public
    ////////////////////////////////////////////////////////////////////////

    Rule* Rules::pop_pending_rule() {
      LIBSEMIGROUPS_ASSERT(_pending_rules.size() != 0);
      Rule* rule = _pending_rules.back();
      _pending_rules.pop_back();
      return rule;
    }

    ////////////////////////////////////////////////////////////////////////
    // Rules - mem fns - private
    ////////////////////////////////////////////////////////////////////////

    Rule* Rules::add_pending_rule(Rule* rule) {
      LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
      rule->state(Rule::State::pending);
      _pending_rules.push_back(rule);
      _stats.max_pending_rules
          = std::max(_stats.max_pending_rules, _pending_rules.size());
      return rule;
    }

    Rule* Rules::copy_rule(Rule const* rule) {
      return new_rule(rule->lhs().cbegin(),
                      rule->lhs().cend(),
                      rule->rhs().cbegin(),
                      rule->rhs().cend());
    }

    Rule* Rules::new_rule() {
      ++_stats.total_rules;
      Rule* rule;
      if (!_inactive_rules.empty()) {
        rule = _inactive_rules.front();
        _inactive_rules.erase(_inactive_rules.begin());
      } else {
        // NOTE: JDM tried adding more than one new inactive rule here,
        // by adding the sum of the number of active rules and the number of
        // pending rules, but found no examples where this was faster or slower,
        // so opted to keep this as is.
        rule = new Rule();
      }
      LIBSEMIGROUPS_ASSERT(rule->state() == Rule::State::inactive);
      return rule;
    }

  }  // namespace detail
}  // namespace libsemigroups
