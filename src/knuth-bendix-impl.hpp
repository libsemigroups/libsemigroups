//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains the implementation of the class KnuthBendixImpl, which is
// the main implementation for the class KnuthBendix.

#ifndef LIBSEMIGROUPS_SRC_KNUTH_BENDIX_IMPL_HPP_
#define LIBSEMIGROUPS_SRC_KNUTH_BENDIX_IMPL_HPP_

#include <algorithm>    // for max, min
#include <atomic>       // for atomic
#include <cinttypes>    // for int64_t
#include <cstddef>      // for size_t
#include <limits>       // for numeric_limits
#include <list>         // for list, list<>::iterator
#include <ostream>      // for string
#include <set>          // for set
#include <stack>        // for stack
#include <string>       // for operator!=, basic_strin...
#include <type_traits>  // for swap
#include <utility>      // for pair
#include <vector>       // for vector

#include "libsemigroups/constants.hpp"     // for POSITIVE_INFINITY
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, KnuthBendi...
#include "libsemigroups/libsemigroups-config.hpp"  // for LIBSEMIGROUPS_DEBUG
#include "libsemigroups/libsemigroups-debug.hpp"   // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/order.hpp"                 // for shortlex_compare
#include "libsemigroups/report.hpp"                // for REPORT
#include "libsemigroups/string.hpp"  // for detail::is_suffix, maximum_comm...
#include "libsemigroups/timer.hpp"   // for detail::Timer
#include "libsemigroups/types.hpp"   // for word_type

namespace libsemigroups {
  namespace detail {
    class KBE;  // Forward declarations
  }
  namespace fpsemigroup {

    class KnuthBendix::KnuthBendixImpl {
      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - typedefs/aliases - private
      ////////////////////////////////////////////////////////////////////////

      using external_string_type = std::string;
      using internal_string_type = std::string;
      using external_char_type   = char;
      using internal_char_type   = char;

      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - nested subclasses - private
      ////////////////////////////////////////////////////////////////////////

      // Rule and RuleLookup classes
      class Rule {
       public:
        // Construct from KnuthBendix with new but empty internal_string_type's
        explicit Rule(KnuthBendixImpl const* kbimpl, int64_t id)
            : _kbimpl(kbimpl),
              _lhs(new internal_string_type()),
              _rhs(new internal_string_type()),
              _id(-1 * id) {
          LIBSEMIGROUPS_ASSERT(_id < 0);
        }

        // The Rule class does not support an assignment contructor to avoid
        // accidental copying.
        Rule& operator=(Rule const& copy) = delete;

        // The Rule class does not support a copy contructor to avoid
        // accidental copying.
        Rule(Rule const& copy) = delete;

        // Destructor, deletes pointers used to create the rule.
        ~Rule() {
          delete _lhs;
          delete _rhs;
        }

        // Returns the left hand side of the rule, which is guaranteed to be
        // greater than its right hand side according to the reduction ordering
        // of the KnuthBendix used to construct this.
        internal_string_type* lhs() const {
          return _lhs;
        }

        // Returns the right hand side of the rule, which is guaranteed to be
        // less than its left hand side according to the reduction ordering of
        // the KnuthBendix used to construct this.
        internal_string_type* rhs() const {
          return _rhs;
        }

        void rewrite() {
          LIBSEMIGROUPS_ASSERT(_id != 0);
          _kbimpl->internal_rewrite(_lhs);
          _kbimpl->internal_rewrite(_rhs);
          // reorder if necessary
          if (shortlex_compare(_lhs, _rhs)) {
            std::swap(_lhs, _rhs);
          }
        }

        void clear() {
          LIBSEMIGROUPS_ASSERT(_id != 0);
          _lhs->clear();
          _rhs->clear();
        }

        inline bool active() const {
          LIBSEMIGROUPS_ASSERT(_id != 0);
          return (_id > 0);
        }

        void deactivate() {
          LIBSEMIGROUPS_ASSERT(_id != 0);
          if (active()) {
            _id *= -1;
          }
        }

        void activate() {
          LIBSEMIGROUPS_ASSERT(_id != 0);
          if (!active()) {
            _id *= -1;
          }
        }

        void set_id(int64_t id) {
          LIBSEMIGROUPS_ASSERT(id > 0);
          LIBSEMIGROUPS_ASSERT(!active());
          _id = -1 * id;
        }

        int64_t id() const {
          LIBSEMIGROUPS_ASSERT(_id != 0);
          return _id;
        }

        KnuthBendixImpl const* _kbimpl;
        internal_string_type*  _lhs;
        internal_string_type*  _rhs;
        int64_t                _id;
      };  // struct Rule

      // Simple class wrapping a two iterators to an internal_string_type and a
      // Rule const*

      class RuleLookup {
       public:
        RuleLookup() : _rule(nullptr) {}

        explicit RuleLookup(Rule* rule)
            : _first(rule->lhs()->cbegin()),
              _last(rule->lhs()->cend()),
              _rule(rule) {}

        RuleLookup& operator()(internal_string_type::iterator const& first,
                               internal_string_type::iterator const& last) {
          _first = first;
          _last  = last;
          return *this;
        }

        Rule const* rule() const {
          return _rule;
        }

        // This implements reverse lex comparison of this and that, which
        // satisfies the requirement of std::set that equivalent items be
        // incomparable, so, for example bcbc and abcbc are considered
        // equivalent, but abcba and bcbc are not.
        bool operator<(RuleLookup const& that) const {
          auto it_this = _last - 1;
          auto it_that = that._last - 1;
          while (it_this > _first && it_that > that._first
                 && *it_this == *it_that) {
            --it_that;
            --it_this;
          }
          return *it_this < *it_that;
        }

       private:
        internal_string_type::const_iterator _first;
        internal_string_type::const_iterator _last;
        Rule const*                          _rule;
      };  // class RuleLookup

      // Overlap measures
      struct OverlapMeasure {
        virtual size_t operator()(Rule const*,
                                  Rule const*,
                                  internal_string_type::const_iterator const&)
            = 0;
        virtual ~OverlapMeasure() {}
      };

      struct ABC : OverlapMeasure {
        size_t
        operator()(Rule const*                                 AB,
                   Rule const*                                 BC,
                   internal_string_type::const_iterator const& it) override {
          LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
          LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
          LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
          // |A| + |BC|
          return (it - AB->lhs()->cbegin()) + BC->lhs()->size();
        }
      };

      struct AB_BC : OverlapMeasure {
        size_t
        operator()(Rule const*                                 AB,
                   Rule const*                                 BC,
                   internal_string_type::const_iterator const& it) override {
          LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
          LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
          LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
          (void) it;
          // |AB| + |BC|
          return AB->lhs()->size() + BC->lhs()->size();
        }
      };

      struct MAX_AB_BC : OverlapMeasure {
        size_t
        operator()(Rule const*                                 AB,
                   Rule const*                                 BC,
                   internal_string_type::const_iterator const& it) override {
          LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
          LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
          LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
          (void) it;
          // max(|AB|, |BC|)
          return std::max(AB->lhs()->size(), BC->lhs()->size());
        }
      };

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - friend declarations - private
      //////////////////////////////////////////////////////////////////////////

      friend class Rule;                          // defined in this file
      friend class ::libsemigroups::detail::KBE;  // defined in detail::kbe.hpp

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - constructors + destructor - public
      //////////////////////////////////////////////////////////////////////////

      explicit KnuthBendixImpl(KnuthBendix* kb)
          : _active_rules(),
            _confluent(false),
            _confluence_known(false),
            _inactive_rules(),
            _internal_is_same_as_external(false),
            _kb(kb),
            _min_length_lhs_rule(std::numeric_limits<size_t>::max()),
            _overlap_measure(nullptr),
            _stack(),
            _tmp_word1(new internal_string_type()),
            _tmp_word2(new internal_string_type()),
            _total_rules(0) {
        _next_rule_it1 = _active_rules.end();  // null
        _next_rule_it2 = _active_rules.end();  // null
        this->set_overlap_policy(policy::overlap::ABC);
#ifdef LIBSEMIGROUPS_VERBOSE
        _max_stack_depth        = 0;
        _max_word_length        = 0;
        _max_active_word_length = 0;
        _max_active_rules       = 0;
#endif
      }

      ~KnuthBendixImpl() {
        delete _overlap_measure;
        delete _tmp_word1;
        delete _tmp_word2;
        for (Rule const* rule : _active_rules) {
          delete const_cast<Rule*>(rule);
        }
        for (Rule* rule : _inactive_rules) {
          delete rule;
        }
        while (!_stack.empty()) {
          Rule* rule = _stack.top();
          _stack.pop();
          delete rule;
        }
      }

     private:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - converting ints <-> string/char - private
      //////////////////////////////////////////////////////////////////////////

      static size_t internal_char_to_uint(internal_char_type c) {
#ifdef LIBSEMIGROUPS_DEBUG
        LIBSEMIGROUPS_ASSERT(c >= 97);
        return static_cast<size_t>(c - 97);
#else
        return static_cast<size_t>(c - 1);
#endif
      }

      static internal_char_type uint_to_internal_char(size_t a) {
        LIBSEMIGROUPS_ASSERT(a
                             <= std::numeric_limits<internal_char_type>::max());
#ifdef LIBSEMIGROUPS_DEBUG
        LIBSEMIGROUPS_ASSERT(a <= std::numeric_limits<internal_char_type>::max()
                                      - 97);
        return static_cast<internal_char_type>(a + 97);
#else
        return static_cast<internal_char_type>(a + 1);
#endif
      }

      static internal_string_type uint_to_internal_string(size_t const i) {
        LIBSEMIGROUPS_ASSERT(i
                             <= std::numeric_limits<internal_char_type>::max());
        return internal_string_type({uint_to_internal_char(i)});
      }

      static word_type internal_string_to_word(internal_string_type const& s) {
        word_type w;
        w.reserve(s.size());
        for (internal_char_type const& c : s) {
          w.push_back(internal_char_to_uint(c));
        }
        return w;
      }

      static internal_string_type*
      word_to_internal_string(word_type const& w, internal_string_type* ww) {
        ww->clear();
        for (size_t const& a : w) {
          (*ww) += uint_to_internal_char(a);
        }
        return ww;
      }

      static internal_string_type word_to_internal_string(word_type const& u) {
        internal_string_type v;
        v.reserve(u.size());
        for (size_t const& l : u) {
          v += uint_to_internal_char(l);
        }
        return v;
      }

      internal_char_type external_to_internal_char(external_char_type c) const {
        LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
        return uint_to_internal_char(_kb->char_to_uint(c));
      }

      external_char_type internal_to_external_char(internal_char_type a) const {
        LIBSEMIGROUPS_ASSERT(!_internal_is_same_as_external);
        return _kb->uint_to_char(internal_char_to_uint(a));
      }

      void external_to_internal_string(external_string_type& w) const {
        if (_internal_is_same_as_external) {
          return;
        }
        for (auto& a : w) {
          a = external_to_internal_char(a);
        }
      }

      void internal_to_external_string(internal_string_type& w) const {
        if (_internal_is_same_as_external) {
          return;
        }
        for (auto& a : w) {
          a = internal_to_external_char(a);
        }
      }

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - methods for rules - public
      //////////////////////////////////////////////////////////////////////////

      void add_rule(std::string const& p, std::string const& q) {
        LIBSEMIGROUPS_ASSERT(p != q);
        auto pp = new external_string_type(p);
        auto qq = new external_string_type(q);
        external_to_internal_string(*pp);
        external_to_internal_string(*qq);
        add_rule(new_rule(pp, qq));
      }

      void add_rules(KnuthBendixImpl const* impl) {
        for (Rule const* rule : impl->_active_rules) {
          add_rule(new_rule(rule));
        }
      }

      std::vector<std::pair<std::string, std::string>> rules() const {
        std::vector<std::pair<external_string_type, external_string_type>>
            rules;
        rules.reserve(_active_rules.size());
        for (Rule const* rule : _active_rules) {
          internal_string_type lhs = internal_string_type(*rule->lhs());
          internal_string_type rhs = internal_string_type(*rule->rhs());
          internal_to_external_string(lhs);
          internal_to_external_string(rhs);
          rules.emplace_back(lhs, rhs);
        }
        std::sort(
            rules.begin(),
            rules.end(),
            [](std::pair<external_string_type, external_string_type> rule1,
               std::pair<external_string_type, external_string_type> rule2) {
              return shortlex_compare(rule1.first, rule2.first)
                     || (rule1.first == rule2.first
                         && shortlex_compare(rule1.second, rule2.second));
            });
        return rules;
      }

      size_t nr_rules() const {
        return _active_rules.size();
      }

     private:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - methods for rules - private
      //////////////////////////////////////////////////////////////////////////

      Rule* new_rule() const {
        ++_total_rules;
        Rule* rule;
        if (!_inactive_rules.empty()) {
          rule = _inactive_rules.front();
          rule->clear();
          rule->set_id(_total_rules);
          _inactive_rules.erase(_inactive_rules.begin());
        } else {
          rule = new Rule(this, _total_rules);
        }
        LIBSEMIGROUPS_ASSERT(!rule->active());
        return rule;
      }

      Rule* new_rule(internal_string_type* lhs,
                     internal_string_type* rhs) const {
        Rule* rule = new_rule();
        delete rule->_lhs;
        delete rule->_rhs;
        if (shortlex_compare(rhs, lhs)) {
          rule->_lhs = lhs;
          rule->_rhs = rhs;
        } else {
          rule->_lhs = rhs;
          rule->_rhs = lhs;
        }
        return rule;
      }

      Rule* new_rule(Rule const* rule1) const {
        Rule* rule2 = new_rule();
        rule2->_lhs->append(*rule1->lhs());  // copies lhs
        rule2->_rhs->append(*rule1->rhs());  // copies rhs
        return rule2;
      }

      Rule* new_rule(internal_string_type::const_iterator begin_lhs,
                     internal_string_type::const_iterator end_lhs,
                     internal_string_type::const_iterator begin_rhs,
                     internal_string_type::const_iterator end_rhs) const {
        Rule* rule = new_rule();
        rule->_lhs->append(begin_lhs, end_lhs);
        rule->_rhs->append(begin_rhs, end_rhs);
        return rule;
      }

      void add_rule(Rule* rule) {
        LIBSEMIGROUPS_ASSERT(*rule->lhs() != *rule->rhs());
#ifdef LIBSEMIGROUPS_VERBOSE
        _max_word_length  = std::max(_max_word_length, rule->lhs()->size());
        _max_active_rules = std::max(_max_active_rules, _active_rules.size());
        _unique_lhs_rules.insert(*rule->lhs());
#endif
        if (!_set_rules.emplace(RuleLookup(rule)).second) {
          // The rules are not reduced, this should only happen if we are
          // calling add_rule from outside the class (i.e. we are initialising
          // the KnuthBendix).
          push_stack(rule);
          return;  // Do not activate or actually add the rule at this point
        }
        rule->activate();
        _active_rules.push_back(rule);
        if (_next_rule_it1 == _active_rules.end()) {
          --_next_rule_it1;
        }
        if (_next_rule_it2 == _active_rules.end()) {
          --_next_rule_it2;
        }
        _confluence_known = false;
        if (rule->lhs()->size() < _min_length_lhs_rule) {
          // TODO(later) this is not valid when using non-length reducing
          // orderings (such as RECURSIVE)
          _min_length_lhs_rule = rule->lhs()->size();
        }
        LIBSEMIGROUPS_ASSERT(_set_rules.size() == _active_rules.size());
      }

      std::list<Rule const*>::iterator
      remove_rule(std::list<Rule const*>::iterator it) {
#ifdef LIBSEMIGROUPS_VERBOSE
        _unique_lhs_rules.erase(*((*it)->lhs()));
#endif
        Rule* rule = const_cast<Rule*>(*it);
        rule->deactivate();
        if (it != _next_rule_it1 && it != _next_rule_it2) {
          it = _active_rules.erase(it);
        } else if (it == _next_rule_it1 && it != _next_rule_it2) {
          _next_rule_it1 = _active_rules.erase(it);
          it             = _next_rule_it1;
        } else if (it != _next_rule_it1 && it == _next_rule_it2) {
          _next_rule_it2 = _active_rules.erase(it);
          it             = _next_rule_it2;
        } else {
          _next_rule_it1 = _active_rules.erase(it);
          _next_rule_it2 = _next_rule_it1;
          it             = _next_rule_it1;
        }
#ifdef LIBSEMIGROUPS_DEBUG
        LIBSEMIGROUPS_ASSERT(_set_rules.erase(RuleLookup(rule)));
#else
        _set_rules.erase(RuleLookup(rule));
#endif
        LIBSEMIGROUPS_ASSERT(_set_rules.size() == _active_rules.size());
        return it;
      }

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - other methods - public
      //////////////////////////////////////////////////////////////////////////

      external_string_type* rewrite(external_string_type* w) const {
        external_to_internal_string(*w);
        internal_rewrite(w);
        internal_to_external_string(*w);
        return w;
      }

      bool equal_to(external_string_type const& u,
                    external_string_type const& v) {
        if (u == v) {
          return true;
        }
        external_string_type uu = _kb->rewrite(u);
        external_string_type vv = _kb->rewrite(v);
        if (uu == vv) {
          return true;
        }
        knuth_bendix();
        external_to_internal_string(uu);
        external_to_internal_string(vv);
        internal_rewrite(&uu);
        internal_rewrite(&vv);
        return uu == vv;
      }

      void set_overlap_policy(policy::overlap p) {
        if (p == _kb->_settings._overlap_policy
            && _overlap_measure != nullptr) {
          return;
        }
        delete _overlap_measure;
        switch (p) {
          case policy::overlap::ABC:
            _overlap_measure = new ABC();
            break;
          case policy::overlap::AB_BC:
            _overlap_measure = new AB_BC();
            break;
          case policy::overlap::MAX_AB_BC:
            _overlap_measure = new MAX_AB_BC();
            break;
          default:
            LIBSEMIGROUPS_ASSERT(false);
        }
      }

      void set_internal_alphabet(std::string const& lphbt = "") {
        _internal_is_same_as_external = true;
        for (size_t i = 0; i < lphbt.size(); ++i) {
          if (uint_to_internal_char(i) != lphbt[i]) {
            _internal_is_same_as_external = false;
            return;
          }
        }
      }

     private:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - other methods - private
      //////////////////////////////////////////////////////////////////////////
      // REWRITE_FROM_LEFT from Sims, p67
      // Caution: this uses the assumption that rules are length reducing, if it
      // is not, then u might not have sufficient space!
      void internal_rewrite(internal_string_type* u) const {
        if (u->size() < _min_length_lhs_rule) {
          return;
        }
        internal_string_type::iterator const& v_begin = u->begin();
        internal_string_type::iterator        v_end
            = u->begin() + _min_length_lhs_rule - 1;
        internal_string_type::iterator        w_begin = v_end;
        internal_string_type::iterator const& w_end   = u->end();

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
                 && _min_length_lhs_rule - 1
                        > static_cast<size_t>((v_end - v_begin))) {
            *v_end = *w_begin;
            ++v_end;
            ++w_begin;
          }
        }
        u->erase(v_end - u->cbegin());
      }

      // TEST_2 from Sims, p76
      void clear_stack() {
        while (!_stack.empty() && !_kb->stopped()) {
#ifdef LIBSEMIGROUPS_VERBOSE
          _max_stack_depth = std::max(_max_stack_depth, _stack.size());
#endif

          Rule* rule1 = _stack.top();
          _stack.pop();
          LIBSEMIGROUPS_ASSERT(!rule1->active());
          LIBSEMIGROUPS_ASSERT(*rule1->lhs() != *rule1->rhs());
          // Rewrite both sides and reorder if necessary . . .
          rule1->rewrite();

          if (*rule1->lhs() != *rule1->rhs()) {
            internal_string_type const* lhs = rule1->lhs();
            for (auto it = _active_rules.begin(); it != _active_rules.end();) {
              Rule* rule2 = const_cast<Rule*>(*it);
              if (rule2->lhs()->find(*lhs) != external_string_type::npos) {
                it = remove_rule(it);
                LIBSEMIGROUPS_ASSERT(*rule2->lhs() != *rule2->rhs());
                // rule2 is added to _inactive_rules by clear_stack
                _stack.emplace(rule2);
              } else {
                if (rule2->rhs()->find(*lhs) != external_string_type::npos) {
                  internal_rewrite(rule2->rhs());
                }
                ++it;
              }
            }
            add_rule(rule1);
            // rule1 is activated, we do this after removing rules that rule1
            // makes redundant to avoid failing to insert rule1 in _set_rules
          } else {
            _inactive_rules.push_back(rule1);
          }
          if (_kb->report()) {
            REPORT_DEFAULT(
                "active rules = %d, inactive rules = %d, rules defined = "
                "%d\n",
                _active_rules.size(),
                _inactive_rules.size(),
                _total_rules);
            REPORT_VERBOSE_DEFAULT("max stack depth        = %d\n"
                                   "max word length        = %d\n"
                                   "max active word length = %d\n"
                                   "max active rules       = %d\n"
                                   "number of unique lhs   = %d\n",
                                   _max_stack_depth,
                                   _max_word_length,
                                   max_active_word_length(),
                                   _max_active_rules,
                                   _unique_lhs_rules.size());
          }
        }
      }
      // FIXME(later) there is a possibly infinite loop here clear_stack ->
      // push_stack -> clear_stack and so on
      void push_stack(Rule* rule) {
        LIBSEMIGROUPS_ASSERT(!rule->active());
        if (*rule->lhs() != *rule->rhs()) {
          _stack.emplace(rule);
          clear_stack();
        } else {
          _inactive_rules.push_back(rule);
        }
      }

      // OVERLAP_2 from Sims, p77
      void overlap(Rule const* u, Rule const* v) {
        LIBSEMIGROUPS_ASSERT(u->active() && v->active());
        auto limit
            = u->lhs()->cend() - std::min(u->lhs()->size(), v->lhs()->size());
        int64_t u_id = u->id();
        int64_t v_id = v->id();
        for (auto it = u->lhs()->cend() - 1;
             it > limit && u_id == u->id() && v_id == v->id() && !_kb->stopped()
             && (_kb->_settings._max_overlap == POSITIVE_INFINITY
                 || (*_overlap_measure)(u, v, it)
                        <= _kb->_settings._max_overlap);
             --it) {
          // Check if B = [it, u->lhs()->cend()) is a prefix of v->lhs()
          if (detail::is_prefix(
                  v->lhs()->cbegin(), v->lhs()->cend(), it, u->lhs()->cend())) {
            // u = P_i = AB -> Q_i and v = P_j = BC -> Q_j
            // This version of new_rule does not reorder
            Rule* rule = new_rule(u->lhs()->cbegin(),
                                  it,
                                  u->rhs()->cbegin(),
                                  u->rhs()->cend());  // rule = A -> Q_i
            rule->_lhs->append(*v->rhs());            // rule = AQ_j -> Q_i
            rule->_rhs->append(v->lhs()->cbegin() + (u->lhs()->cend() - it),
                               v->lhs()->cend());  // rule = AQ_j -> Q_iC
            // rule is reordered during rewriting in clear_stack
            push_stack(rule);
            // It can be that the iterator `it` is invalidated by the call to
            // push_stack (i.e. if `u` is deactivated, then rewritten, actually
            // changed, and reactivated) and that is the reason for the checks
            // in the for-loop above. If this is the case, then we should stop
            // considering the overlaps of u and v here, and note that they will
            // be considered later, because when the rule `u` is reactivated it
            // is added to the end of the active rules list.
          }
        }
      }

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - main methods - public
      //////////////////////////////////////////////////////////////////////////

      bool confluent() const {
        if (!_stack.empty()) {
          return false;
        }
        if (!_confluence_known && (!_kb->running() || !_kb->stopped())) {
          LIBSEMIGROUPS_ASSERT(_stack.empty());
          _confluent        = true;
          _confluence_known = true;
          internal_string_type word1;
          internal_string_type word2;
          size_t               seen = 0;

          for (auto it1 = _active_rules.cbegin();
               it1 != _active_rules.cend()
               && (!_kb->running() || !_kb->stopped());
               ++it1) {
            Rule const* rule1 = *it1;
            // Seems to be much faster to do this in reverse.
            for (auto it2 = _active_rules.crbegin();
                 it2 != _active_rules.crend()
                 && (!_kb->running() || !_kb->stopped());
                 ++it2) {
              seen++;
              Rule const* rule2 = *it2;
              for (auto it = rule1->lhs()->cend() - 1;
                   it >= rule1->lhs()->cbegin()
                   && (!_kb->running() || !_kb->stopped());
                   --it) {
                // Find longest common prefix of suffix B of rule1.lhs() defined
                // by it and R = rule2.lhs()
                auto prefix
                    = detail::maximum_common_prefix(it,
                                                    rule1->lhs()->cend(),
                                                    rule2->lhs()->cbegin(),
                                                    rule2->lhs()->cend());
                if (prefix.first == rule1->lhs()->cend()
                    || prefix.second == rule2->lhs()->cend()) {
                  word1.clear();
                  word1.append(rule1->lhs()->cbegin(), it);          // A
                  word1.append(*rule2->rhs());                       // S
                  word1.append(prefix.first, rule1->lhs()->cend());  // D

                  word2.clear();
                  word2.append(*rule1->rhs());                        // Q
                  word2.append(prefix.second, rule2->lhs()->cend());  // E

                  if (word1 != word2) {
                    internal_rewrite(&word1);
                    internal_rewrite(&word2);
                    if (word1 != word2) {
                      _confluent = false;
                      return _confluent;
                    }
                  }
                }
              }
            }
            if (_kb->report()) {
              REPORT_DEFAULT("checked %d pairs of overlaps out of %d\n",
                             seen,
                             _active_rules.size() * _active_rules.size());
            }
          }
          if (_kb->running() && _kb->stopped()) {
            _confluence_known = false;
          }
        }
        return _confluent;
      }

      // KBS_2 from Sims, p77-78
      bool knuth_bendix() {
        detail::Timer timer;
        if (_stack.empty() && confluent() && !_kb->stopped()) {
          // _stack can be non-empty if non-reduced rules were used to define
          // the KnuthBendix.  If _stack is non-empty, then it means that the
          // rules in _active_rules might not define the system.
          REPORT_DEFAULT("the system is confluent already\n");
          return true;
        } else if (_active_rules.size() >= _kb->_settings._max_rules) {
          REPORT_DEFAULT("too many rules\n");
          return false;
        }
        // Reduce the rules
        _next_rule_it1 = _active_rules.begin();
        while (_next_rule_it1 != _active_rules.end() && !_kb->stopped()) {
          // Copy *_next_rule_it1 and push_stack so that it is not modified by
          // the call to clear_stack.
          LIBSEMIGROUPS_ASSERT((*_next_rule_it1)->lhs()
                               != (*_next_rule_it1)->rhs());
          push_stack(new_rule(*_next_rule_it1));
          ++_next_rule_it1;
        }
        _next_rule_it1 = _active_rules.begin();
        size_t nr      = 0;
        while (_next_rule_it1 != _active_rules.cend()
               && _active_rules.size() < _kb->_settings._max_rules
               && !_kb->stopped()) {
          Rule const* rule1 = *_next_rule_it1;
          _next_rule_it2    = _next_rule_it1;
          ++_next_rule_it1;
          overlap(rule1, rule1);
          while (_next_rule_it2 != _active_rules.begin() && rule1->active()) {
            --_next_rule_it2;
            Rule const* rule2 = *_next_rule_it2;
            overlap(rule1, rule2);
            ++nr;
            if (rule1->active() && rule2->active()) {
              ++nr;
              overlap(rule2, rule1);
            }
          }
          if (nr > _kb->_settings._check_confluence_interval) {
            if (confluent()) {
              break;
            }
            nr = 0;
          }
          if (_next_rule_it1 == _active_rules.cend()) {
            clear_stack();
          }
        }
        // LIBSEMIGROUPS_ASSERT(_stack.empty());
        // Seems that the stack can be non-empty here in KnuthBendix 12, 14, 16
        // and maybe more
        bool ret;
        if (_kb->_settings._max_overlap == POSITIVE_INFINITY
            && _kb->_settings._max_rules == POSITIVE_INFINITY
            && !_kb->stopped()) {
          _confluence_known = true;
          _confluent        = true;
          for (Rule* rule : _inactive_rules) {
            delete rule;
          }
          _inactive_rules.clear();
          ret = true;
        } else {
          ret = false;
        }

        REPORT_DEFAULT("stopping with active rules = %d, inactive rules = %d, "
                       "rules defined = %d\n",
                       _active_rules.size(),
                       _inactive_rules.size(),
                       _total_rules);
        REPORT_VERBOSE_DEFAULT("max stack depth = %d", _max_stack_depth);
        REPORT_TIME(timer);
        return ret;
      }

      void knuth_bendix_by_overlap_length() {
        detail::Timer timer;
        size_t        max_overlap = _kb->_settings._max_overlap;
        size_t        check_confluence_interval
            = _kb->_settings._check_confluence_interval;
        _kb->_settings._max_overlap               = 1;
        _kb->_settings._check_confluence_interval = POSITIVE_INFINITY;
        while (!_kb->stopped() && !confluent()) {
          knuth_bendix();
          _kb->_settings._max_overlap++;
        }
        _kb->_settings._max_overlap               = max_overlap;
        _kb->_settings._check_confluence_interval = check_confluence_interval;
        REPORT_TIME(timer);
      }

     private:
      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - iterators - private
      ////////////////////////////////////////////////////////////////////////

      using external_rule_type
          = std::pair<external_string_type, external_string_type>;

      struct IteratorMethods {
        external_rule_type
        indirection(KnuthBendixImpl*                       kbi,
                    std::list<Rule const*>::const_iterator it) const {
          auto lhs = std::string(*(*it)->lhs());
          auto rhs = std::string(*(*it)->rhs());
          kbi->internal_to_external_string(lhs);
          kbi->internal_to_external_string(rhs);

          return std::make_pair(lhs, rhs);
        }

        // Not defined!
        external_rule_type const*
        addressof(KnuthBendixImpl*,
                  std::list<Rule const*>::const_iterator) const {
          return nullptr;
        }
      };

     public:
      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - iterators - public
      ////////////////////////////////////////////////////////////////////////

      /*using const_iterator = detail::ConstIteratorStateful<
          KnuthBendixImpl const*,                  // state
          IteratorMethods,                        // methods
          std::list<Rule const*>::const_iterator,  // wrapped iterator
          external_rule_type,                      // external value type
          external_rule_type,                      // external const pointer
          external_rule_type&&                     // external const reference
          >;

      const_iterator cbegin_active_rules() const {
        return const_iterator(this, _active_rules.cbegin());
      }

      const_iterator cend_active_rules() const {
        return const_iterator(this, _active_rules.cend());
      }*/

     private:
      ////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - data - private
      ////////////////////////////////////////////////////////////////////////

      std::list<Rule const*>           _active_rules;
      mutable std::atomic<bool>        _confluent;
      mutable std::atomic<bool>        _confluence_known;
      mutable std::list<Rule*>         _inactive_rules;
      bool                             _internal_is_same_as_external;
      KnuthBendix*                     _kb;
      size_t                           _min_length_lhs_rule;
      std::list<Rule const*>::iterator _next_rule_it1;
      std::list<Rule const*>::iterator _next_rule_it2;
      OverlapMeasure*                  _overlap_measure;
      std::set<RuleLookup>             _set_rules;
      std::stack<Rule*>                _stack;
      internal_string_type*            _tmp_word1;
      internal_string_type*            _tmp_word2;
      mutable size_t                   _total_rules;

#ifdef LIBSEMIGROUPS_VERBOSE
      //////////////////////////////////////////////////////////////////////////
      // ./configure --enable-verbose functions
      //////////////////////////////////////////////////////////////////////////

      size_t max_active_word_length() {
        auto comp = [](Rule const* p, Rule const* q) -> bool {
          return p->lhs()->size() < q->lhs()->size();
        };
        auto max = std::max_element(
            _active_rules.cbegin(), _active_rules.cend(), comp);
        if (max != _active_rules.cend()) {
          _max_active_word_length
              = std::max(_max_active_word_length, (*max)->lhs()->size());
        }
        return _max_active_word_length;
      }
      size_t                                   _max_stack_depth;
      size_t                                   _max_word_length;
      size_t                                   _max_active_word_length;
      size_t                                   _max_active_rules;
      std::unordered_set<internal_string_type> _unique_lhs_rules;
#endif
    };  // struct KnuthBendixImpl
  }     // namespace fpsemigroup
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_KNUTH_BENDIX_IMPL_HPP_
