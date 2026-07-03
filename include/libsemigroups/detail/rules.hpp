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

// This file contains the implementation of a Rule object containers for Rule
// objects. It also includes rewriter classes that can be used to rewrite
// strings relative to a collection of rules.

#ifndef LIBSEMIGROUPS_DETAIL_RULES_HPP_
#define LIBSEMIGROUPS_DETAIL_RULES_HPP_

#include <algorithm>  // for std::sort
#include <array>      // for array
#include <cstddef>    // for size_t
#include <cstdint>    // for uint64_t
#include <list>       // for list
#include <string>     // for std::string
#include <utility>    // for move
#include <vector>     // for vector

#include "libsemigroups/debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/order.hpp"

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // Rule
    ////////////////////////////////////////////////////////////////////////

    class Rule {
     public:
      enum class State : uint8_t { active = 0, inactive = 1, pending = 2 };

      using native_word_type = std::string;

     private:
      native_word_type _lhs;
      native_word_type _rhs;
      State            _state;

     public:
      Rule() : _lhs(), _rhs(), _state(Rule::State::inactive) {}

      Rule& operator=(Rule const& copy) = delete;
      Rule(Rule const& copy)            = delete;
      Rule(Rule&& copy)                 = delete;
      Rule& operator=(Rule&& copy)      = delete;

      ~Rule() = default;

      [[nodiscard]] native_word_type const& lhs() const noexcept {
        return _lhs;
      }

      [[nodiscard]] native_word_type const& rhs() const noexcept {
        return _rhs;
      }

      [[nodiscard]] native_word_type& lhs() noexcept {
        return _lhs;
      }

      [[nodiscard]] native_word_type& rhs() noexcept {
        return _rhs;
      }

      [[nodiscard]] State state() const noexcept {
        return _state;
      }

      Rule& state(State val) {
        _state = val;
        return *this;
      }
    };  // class Rule

    template <typename ReductionOrder>
    void reorder(Rule* rule, ReductionOrder const& order) {
      if (order(rule->lhs(), rule->rhs())) {
        std::swap(rule->lhs(), rule->rhs());
      }
    }

    template <typename ReductionOrder>
    void reorder(Rule* rule) {
      reorder(rule, ReductionOrder{});
    }

    [[nodiscard]] static inline size_t length(Rule const* rule) {
      return rule->lhs().size() + rule->rhs().size();
    }

    ////////////////////////////////////////////////////////////////////////
    // Rule comparison functions
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] static inline bool lhs_lex_cmp(Rule const* rule1,
                                                 Rule const* rule2) {
      return rule1->lhs() < rule2->lhs();
    }

    [[nodiscard]] static inline bool lhs_rev_lex_cmp(Rule const* rule1,
                                                     Rule const* rule2) {
      return rule1->lhs() > rule2->lhs();
    }

    [[nodiscard]] static inline bool lex_cmp(Rule const* rule1,
                                             Rule const* rule2) {
      return rule1->lhs() < rule2->lhs()
             || (rule1->lhs() == rule2->lhs() && rule1->rhs() < rule2->rhs());
    }

    [[nodiscard]] static inline bool rpo_cmp(Rule const* rule1,
                                             Rule const* rule2) {
      return ::libsemigroups::rpo_cmp(rule1->lhs(), rule2->lhs())
             || (rule1->lhs() == rule2->lhs()
                 && ::libsemigroups::rpo_cmp(rule1->rhs(), rule2->rhs()));
    }

    [[nodiscard]] static inline bool rev_rpo_cmp(Rule const* rule1,
                                                 Rule const* rule2) {
      return ::libsemigroups::rev_rpo_cmp(rule1->lhs(), rule2->lhs())
             || (rule1->lhs() == rule2->lhs()
                 && ::libsemigroups::rev_rpo_cmp(rule1->rhs(), rule2->rhs()));
    }

    ////////////////////////////////////////////////////////////////////////
    // OverlapMeasure
    ////////////////////////////////////////////////////////////////////////

    struct OverlapMeasure {
      [[nodiscard]] virtual size_t
      operator()(Rule const*,
                 Rule const*,
                 std::string::const_iterator const&) const
          = 0;
      virtual ~OverlapMeasure() {}
    };

    struct ABC : OverlapMeasure {
      [[nodiscard]] size_t
      operator()(Rule const*                        AB,
                 Rule const*                        BC,
                 std::string::const_iterator const& it) const override;
    };

    struct AB_BC : OverlapMeasure {
      [[nodiscard]] size_t
      operator()(Rule const*                        AB,
                 Rule const*                        BC,
                 std::string::const_iterator const& it) const override;
    };

    struct MAX_AB_BC : OverlapMeasure {
      [[nodiscard]] size_t
      operator()(Rule const*                        AB,
                 Rule const*                        BC,
                 std::string::const_iterator const& it) const override;
    };

    ////////////////////////////////////////////////////////////////////////
    // Rules
    ////////////////////////////////////////////////////////////////////////

    class Rules {
     public:
      using iterator               = std::list<Rule*>::iterator;
      using const_iterator         = std::list<Rule*>::const_iterator;
      using const_reverse_iterator = std::list<Rule*>::const_reverse_iterator;

      struct Cursor {
        iterator it;
        uint64_t version;
      };

     private:
      struct Stats {
        Stats() noexcept;
        Stats& init() noexcept;

        Stats(Stats const&) noexcept            = default;
        Stats(Stats&&) noexcept                 = default;
        Stats& operator=(Stats const&) noexcept = default;
        Stats& operator=(Stats&&) noexcept      = default;

        size_t   max_active_rules;
        size_t   max_length_lhs_rule;
        size_t   max_pending_rules;
        size_t   min_length_lhs_rule;
        uint64_t total_rules;

        void update_after_active_rule_added(Rules const&);
      };

      std::list<Rule*>      _active_rules;
      std::array<Cursor, 2> _cursors;
      std::vector<Rule*>    _inactive_rules;
      std::vector<Rule*>    _pending_rules;
      mutable Stats         _stats;

      // TODO(1) try maintaining pending_rules as a heap?

      void init_cursors();

     public:
      ////////////////////////////////////////////////////////////////////////
      // Constructors and initializers
      ////////////////////////////////////////////////////////////////////////

      Rules();
      Rules& init();

      // This is currently not used anywhere, because we go through the
      // copy/move assignment operators (so no RewritingSystem calls these
      // functions in their copy/move constructors).
      Rules(Rules const& that) : Rules() {
        *this = that;
      }

      // This is currently not used anywhere, because we go through the
      // copy/move assignment operators (so no RewritingSystem calls these
      // functions in their copy/move constructors).
      Rules(Rules&& that) : Rules() {
        *this = std::move(that);
      }

      Rules& operator=(Rules const&);
      Rules& operator=(Rules&& that);

      ~Rules();

      ////////////////////////////////////////////////////////////////////////
      // Adding/modifying rules
      ////////////////////////////////////////////////////////////////////////

      template <typename Iterator>
      Rule* add_pending_rule(Iterator first1,
                             Iterator last1,
                             Iterator first2,
                             Iterator last2) {
        return add_pending_rule(new_rule(first1, last1, first2, last2));
      }

      void add_active_rule(Rule* rule);

      void add_inactive_rule(Rule* rule) {
#ifdef LIBSEMIGROUPS_DEBUG
        rule->state(Rule::State::inactive);
#endif
        _inactive_rules.push_back(rule);
      }

      template <typename Compare>
      void sort_pending_rules(Compare&& cmp) {
        std::sort(_pending_rules.begin(), _pending_rules.end(), cmp);
      }

      [[nodiscard]] iterator make_active_rule_pending(iterator it);

      ////////////////////////////////////////////////////////////////////////
      // Getting rules
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] std::list<Rule*> const& active_rules() const noexcept {
        return _active_rules;
      }

      [[nodiscard]] std::list<Rule*>& active_rules() noexcept {
        return _active_rules;
      }

      [[nodiscard]] std::vector<Rule*> const& pending_rules() const noexcept {
        return _pending_rules;
      }

      // The next fn only exists for backward compat, and should be removed.
      [[deprecated]] [[nodiscard]] std::vector<Rule*> const&
      inactive_rules() const noexcept {
        return _inactive_rules;
      }

      [[nodiscard]] std::vector<Rule*>& pending_rules() noexcept {
        return _pending_rules;
      }

      [[nodiscard]] Rule* pop_pending_rule();

      [[nodiscard]] Cursor& cursor(size_t index) {
        LIBSEMIGROUPS_ASSERT(index < _cursors.size());
        _cursors[index].version = 0;
        return _cursors[index];
      }

      ////////////////////////////////////////////////////////////////////////
      // Numbers of rules
      ////////////////////////////////////////////////////////////////////////

      // TODO rm
      [[nodiscard]] size_t number_of_inactive_rules() const noexcept {
        return _inactive_rules.size();
      }

      [[nodiscard]] Stats const& stats() const {
        return _stats;
      }

     private:
      Rule* add_pending_rule(Rule* rule);

      [[nodiscard]] Rule* copy_rule(Rule const* rule);
      [[nodiscard]] Rule* new_rule();

      template <typename Iterator>
      [[nodiscard]] Rule* new_rule(Iterator first1,
                                   Iterator last1,
                                   Iterator first2,
                                   Iterator last2);
    };  // class Rules

    template <typename Iterator>
    [[nodiscard]] Rule* Rules::new_rule(Iterator first1,
                                        Iterator last1,
                                        Iterator first2,
                                        Iterator last2) {
      Rule* rule = new_rule();
      rule->lhs().assign(first1, last1);
      rule->rhs().assign(first2, last2);
      return rule;
    }

  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_RULES_HPP_
