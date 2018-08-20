//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#include "knuth-bendix.hpp"

#include <algorithm>
#include <set>
#include <string>

#include "internal/report.hpp"
#include "internal/stl.hpp"
#include "internal/timer.hpp"

#include "froidure-pin.hpp"
#include "kbe.hpp"

namespace libsemigroups {

  using external_string_type = std::string;
  using internal_string_type = std::string;
  using external_char_type   = char;
  using internal_char_type   = char;

  //////////////////////////////////////////////////////////////////////////
  // Internal functions for handling strings
  //////////////////////////////////////////////////////////////////////////

  // Replace [it1_begin .. it1_begin + (it2_end - it2_begin)] by
  // [it2_begin .. it2_end], no checks performed.
  static inline void
  string_replace(external_string_type::iterator       it1_begin,
                 external_string_type::const_iterator it2_begin,
                 external_string_type::const_iterator it2_end) {
    while (it2_begin < it2_end) {
      *it1_begin = *it2_begin;
      ++it1_begin;
      ++it2_begin;
    }
  }

  // Returns true if [first_prefix, last_prefix) is a prefix of [first_word,
  // last_word).
  static inline bool
  is_prefix(external_string_type::const_iterator const& first_word,
            external_string_type::const_iterator const& last_word,
            external_string_type::const_iterator const& first_prefix,
            external_string_type::const_iterator const& last_prefix) {
    LIBSEMIGROUPS_ASSERT(first_word <= last_word);
    // We don't care if first_prefix > last_prefix
    if (last_prefix - first_prefix > last_word - first_word) {
      return false;
    }
    // Check if [first_prefix, last_prefix) equals [first_word, first_word +
    // (last_suffix - first_suffix))
    return std::equal(first_prefix, last_prefix, first_word);
  }

  static inline std::pair<external_string_type::const_iterator,
                          external_string_type::const_iterator>
  maximum_common_prefix(
      external_string_type::const_iterator        first_word1,
      external_string_type::const_iterator const& last_word1,
      external_string_type::const_iterator        first_word2,
      external_string_type::const_iterator const& last_word2) {
    while (*first_word1 == *first_word2 && first_word1 < last_word1
           && first_word2 < last_word2) {
      ++first_word1;
      ++first_word2;
    }
    return std::make_pair(first_word1, first_word2);
  }

#ifdef LIBSEMIGROUPS_DEBUG
  // Returns true if [first_suffix, last_suffix) is a suffix of [first_word,
  // last_word).
  static inline bool
  is_suffix(external_string_type::const_iterator const& first_word,
            external_string_type::const_iterator const& last_word,
            external_string_type::const_iterator const& first_suffix,
            external_string_type::const_iterator const& last_suffix) {
    LIBSEMIGROUPS_ASSERT(first_word <= last_word);
    // We don't care if first_suffix > last_suffix
    if (last_suffix - first_suffix > last_word - first_word) {
      return false;
    }

    // Check if [first_suffix, last_suffix) equals [last_word - (last_suffix -
    // first_suffix), end_word).
    // The following seems faster than calling std::equal.
    auto it_suffix = last_suffix - 1;
    auto it_word   = last_word - 1;
    while ((it_suffix > first_suffix) && (*it_suffix == *it_word)) {
      --it_suffix;
      --it_word;
    }
    return *it_suffix == *it_word;
  }
#endif

  namespace fpsemigroup {
    //////////////////////////////////////////////////////////////////////////
    // Internal classes for use by KnuthBendixImpl
    //////////////////////////////////////////////////////////////////////////

    struct KnuthBendix::Rule {
      // Returns the left hand side of the rule, which is guaranteed to be
      // greater than its right hand side according to the reduction ordering of
      // the KnuthBendix used to construct this.
      internal_string_type* lhs() const {
        return _lhs;
      }

      // Returns the right hand side of the rule, which is guaranteed to be
      // less than its left hand side according to the reduction ordering of
      // the KnuthBendix used to construct this.
      internal_string_type* rhs() const {
        return _rhs;
      }

      // The Rule class does not support an assignment contructor to avoid
      // accidental copying.
      Rule& operator=(Rule const& copy) = delete;

      // The Rule class does not support a copy contructor to avoid
      // accidental copying.
      Rule(Rule const& copy) = delete;

      // Construct from KnuthBendix with new but empty internal_string_type's
      explicit Rule(KnuthBendixImpl const* kbimpl, int64_t id)
          : _kbimpl(kbimpl),
            _lhs(new internal_string_type()),
            _rhs(new internal_string_type()),
            _id(-1 * id) {
        LIBSEMIGROUPS_ASSERT(_id < 0);
      }

      // Destructor, deletes pointers used to create the rule.
      ~Rule() {
        delete _lhs;
        delete _rhs;
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

    class KnuthBendix::RuleLookup {
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
      // incomparable, so, for example bcbc and abcbc are considered equivalent,
      // but abcba and bcbc are not.
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

    struct KnuthBendix::OverlapMeasure {
      virtual size_t operator()(Rule const*                                 AB,
                                Rule const*                                 BC,
                                internal_string_type::const_iterator const& it)
          = 0;
      virtual ~OverlapMeasure() {}
      // Forward declarations
      struct ABC;
      struct AB_BC;
      struct MAX_AB_BC;
    };

    struct KnuthBendix::OverlapMeasure::ABC : public OverlapMeasure {
      size_t operator()(Rule const*                                 AB,
                        Rule const*                                 BC,
                        internal_string_type::const_iterator const& it) final {
        LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
        LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
        LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
        // |A| + |BC|
        return (it - AB->lhs()->cbegin()) + BC->lhs()->size();
      }
    };

    struct KnuthBendix::OverlapMeasure::AB_BC : public OverlapMeasure {
      size_t operator()(Rule const*                                 AB,
                        Rule const*                                 BC,
                        internal_string_type::const_iterator const& it) final {
        LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
        LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
        LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
        (void) it;
        // |AB| + |BC|
        return AB->lhs()->size() + BC->lhs()->size();
      }
    };

    struct KnuthBendix::OverlapMeasure::MAX_AB_BC : public OverlapMeasure {
      size_t operator()(Rule const*                                 AB,
                        Rule const*                                 BC,
                        internal_string_type::const_iterator const& it) final {
        LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
        LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
        LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
        (void) it;
        // max(|AB|, |BC|)
        return std::max(AB->lhs()->size(), BC->lhs()->size());
      }
    };

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - nested subclasses - private
    ////////////////////////////////////////////////////////////////////////

    class KnuthBendix::KnuthBendixImpl {
     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - forward declarations - public
      //////////////////////////////////////////////////////////////////////////

      friend struct Rule;                 // defined in knuth-bendix.cpp
      friend class ::libsemigroups::KBE;  // defined in kbe.hpp

      KnuthBendixImpl() : KnuthBendixImpl(new SHORTLEX()) {}

      explicit KnuthBendixImpl(ReductionOrdering* order)
          : _active_rules(),
            _check_confluence_interval(4096),
            _confluent(false),
            _confluence_known(false),
            _inactive_rules(),
            _internal_is_same_as_external(false),
            _max_overlap(POSITIVE_INFINITY),
            _max_rules(POSITIVE_INFINITY),
            _min_length_lhs_rule(std::numeric_limits<size_t>::max()),
            _order(order),
            _overlap_measure(nullptr),
            _stack(),
            _tmp_word1(new internal_string_type()),
            _tmp_word2(new internal_string_type()),
            _total_rules(0) {
        _next_rule_it1 = _active_rules.end();  // null
        _next_rule_it2 = _active_rules.end();  // null
        this->set_overlap_policy(overlap_policy::ABC);
#ifdef LIBSEMIGROUPS_STATS
        _max_stack_depth        = 0;
        _max_word_length        = 0;
        _max_active_word_length = 0;
        _max_active_rules       = 0;
#endif
      }

      ~KnuthBendixImpl() {
        delete _order;
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

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - methods for converting ints <-> string/char -
      // public
      //////////////////////////////////////////////////////////////////////////

      static size_t internal_char_to_uint(internal_char_type c) {
#ifdef LIBSEMIGROUPS_DEBUG
        return static_cast<size_t>(c - 97);
#else
        return static_cast<size_t>(c - 1);
#endif
      }

      static internal_char_type uint_to_internal_char(size_t a) {
        // TODO check a is not too big
#ifdef LIBSEMIGROUPS_DEBUG
        return static_cast<internal_char_type>(a + 97);
#else
        return static_cast<internal_char_type>(a + 1);
#endif
      }

      static internal_string_type uint_to_internal_string(size_t const i) {
        // TODO check i is not too big
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

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - methods for rules - public
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
        if ((*_order)(lhs, rhs)) {
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
#ifdef LIBSEMIGROUPS_STATS
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
          // FIXME this is not valid when using non-length reducing orderings
          // (such as RECURSIVE)
          _min_length_lhs_rule = rule->lhs()->size();
        }
        LIBSEMIGROUPS_ASSERT(_set_rules.size() == _active_rules.size());
      }

      std::list<Rule const*>::iterator
      remove_rule(std::list<Rule const*>::iterator it) {
#ifdef LIBSEMIGROUPS_STATS
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

      void add_rule(std::string const& p, std::string const& q) {
        LIBSEMIGROUPS_ASSERT(p != q);
        auto pp = new external_string_type(p);
        auto qq = new external_string_type(q);
        external_to_internal_string(*pp);
        external_to_internal_string(*qq);
        add_rule(new_rule(pp, qq));
      }

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

      void set_overlap_policy(overlap_policy p) {
        delete _overlap_measure;
        switch (p) {
          case overlap_policy::ABC:
            _overlap_measure = new OverlapMeasure::ABC();
            break;
          case overlap_policy::AB_BC:
            _overlap_measure = new OverlapMeasure::AB_BC();
            break;
          case overlap_policy::MAX_AB_BC:
            _overlap_measure = new OverlapMeasure::MAX_AB_BC();
            break;
          default:
            LIBSEMIGROUPS_ASSERT(false);
        }
      }

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
              LIBSEMIGROUPS_ASSERT(is_suffix(
                  v_begin, v_end, rule->lhs()->cbegin(), rule->lhs()->cend()));
              v_end -= rule->lhs()->size();
              w_begin -= rule->rhs()->size();
              string_replace(
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
        while (!_stack.empty() && !_kb->dead() && !_kb->timed_out()) {
#ifdef LIBSEMIGROUPS_STATS
          _max_stack_depth = std::max(_max_stack_depth, _stack.size());
#endif

          Rule* rule1 = _stack.top();
          _stack.pop();
          LIBSEMIGROUPS_ASSERT(!rule1->active());
          LIBSEMIGROUPS_ASSERT(*rule1->lhs() != *rule1->rhs());
          // Rewrite both sides and reorder if necessary . . .
          internal_rewrite(rule1->lhs());
          internal_rewrite(rule1->rhs());

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
                  rewrite(rule2->rhs());
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
            REPORT("active rules = ",
                   _active_rules.size(),
                   ", inactive rules = ",
                   _inactive_rules.size(),
                   ", rules defined = ",
                   _total_rules);
#ifdef LIBSEMIGROUPS_STATS
            REPORT("max stack depth        = ", _max_stack_depth);
            REPORT("max word length        = ", _max_word_length);
            REPORT("max active word length = ", max_active_word_length());
            REPORT("max active rules       = ", _max_active_rules);
            REPORT("number of unique lhs   = ", _unique_lhs_rules.size());
#endif
          }
        }
      }
      // FIXME there is a possibly infinite loop here clear_stack -> push_stack
      // -> clear_stack and so on
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
             it > limit && !_kb->dead() && u_id == u->id() && v_id == v->id()
             && !_kb->timed_out()
             && (_max_overlap == POSITIVE_INFINITY
                 || (*_overlap_measure)(u, v, it) <= _max_overlap);
             --it) {
          // Check if B = [it, u->lhs()->cend()) is a prefix of v->lhs()
          if (is_prefix(
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
            [this](
                std::pair<external_string_type, external_string_type> rule1,
                std::pair<external_string_type, external_string_type> rule2) {
              return (*_order)(rule2.first, rule1.first)
                     || (rule1.first == rule2.first
                         && (*_order)(rule2.second, rule1.second));
            });
        return rules;
      }

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - main methods - public
      //////////////////////////////////////////////////////////////////////////

      bool confluent() const {
        if (!_stack.empty()) {
          return false;
        }
        if (!_confluence_known && !_kb->dead() && !_kb->timed_out()) {
          LIBSEMIGROUPS_ASSERT(_stack.empty());
          _confluent        = true;
          _confluence_known = true;
          internal_string_type word1;
          internal_string_type word2;
          size_t               seen = 0;

          for (auto it1 = _active_rules.cbegin();
               it1 != _active_rules.cend() && !_kb->dead() && !_kb->timed_out();
               ++it1) {
            Rule const* rule1 = *it1;
            // Seems to be much faster to do this in reverse.
            for (auto it2 = _active_rules.crbegin();
                 it2 != _active_rules.crend() && !_kb->dead()
                 && !_kb->timed_out();
                 ++it2) {
              seen++;
              Rule const* rule2 = *it2;
              for (auto it = rule1->lhs()->cend() - 1;
                   it >= rule1->lhs()->cbegin() && !_kb->dead()
                   && !_kb->timed_out();
                   --it) {
                // Find longest common prefix of suffix B of rule1.lhs() defined
                // by it and R = rule2.lhs()
                auto prefix = maximum_common_prefix(it,
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
              REPORT("checked ",
                     seen,
                     " pairs of overlaps out of ",
                     _active_rules.size() * _active_rules.size());
            }
          }
          if (_kb->dead() || _kb->timed_out()) {
            _confluence_known = false;
          }
        }
        return _confluent;
      }

      // KBS_2 from Sims, p77-78
      void knuth_bendix() {
        if (_kb->finished() || _kb->dead()) {
          return;
        }
        Timer timer;
        if (_stack.empty() && confluent() && !_kb->dead()) {
          // _stack can be non-empty if non-reduced rules were used to define
          // the KnuthBendix.  If _stack is non-empty, then it means that the
          // rules in _active_rules might not define the system.
          REPORT("the system is confluent already");
          _kb->set_finished(true);
          return;
        } else if (_active_rules.size() >= _max_rules) {
          REPORT("too many rules");
          return;
        }
        // Reduce the rules
        _next_rule_it1 = _active_rules.begin();
        while (_next_rule_it1 != _active_rules.end() && !_kb->dead()
               && !_kb->timed_out()) {
          // Copy *_next_rule_it1 and push_stack so that it is not modified by
          // the call to clear_stack.
          LIBSEMIGROUPS_ASSERT((*_next_rule_it1)->lhs()
                               != (*_next_rule_it1)->rhs());
          push_stack(new_rule(*_next_rule_it1));
          ++_next_rule_it1;
        }
        _next_rule_it1 = _active_rules.begin();
        size_t nr      = 0;
        while (_next_rule_it1 != _active_rules.cend() && !_kb->dead()
               && _active_rules.size() < _max_rules && !_kb->timed_out()) {
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
          if (nr > _check_confluence_interval) {
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
        if (_max_overlap == POSITIVE_INFINITY && _max_rules == POSITIVE_INFINITY
            && !_kb->dead() && !_kb->timed_out()) {
          _confluence_known = true;
          _confluent        = true;
          for (Rule* rule : _inactive_rules) {
            delete rule;
          }
          _inactive_rules.clear();
          _kb->set_finished(true);
        }
        REPORT("stopping with active rules = ",
               _active_rules.size(),
               ", inactive rules = ",
               _inactive_rules.size(),
               ", rules defined = ",
               _total_rules);
#ifdef LIBSEMIGROUPS_STATS
        REPORT("max stack depth = ", _max_stack_depth);
#endif
        REPORT("elapsed time = ", timer);
      }

      void knuth_bendix_by_overlap_length() {
        Timer  timer;
        size_t max_overlap               = _max_overlap;
        size_t check_confluence_interval = _check_confluence_interval;
        _max_overlap                     = 1;
        _check_confluence_interval       = POSITIVE_INFINITY;
        while (!_kb->dead() && !_kb->timed_out() && !confluent()) {
          knuth_bendix();
          _max_overlap++;
        }
        _max_overlap               = max_overlap;
        _check_confluence_interval = check_confluence_interval;
        REPORT("elapsed time = ", timer);
      }

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendixImpl - data - public
      //////////////////////////////////////////////////////////////////////////

      std::list<Rule const*>           _active_rules;
      size_t                           _check_confluence_interval;
      mutable std::atomic<bool>        _confluent;
      mutable std::atomic<bool>        _confluence_known;
      mutable std::list<Rule*>         _inactive_rules;
      bool                             _internal_is_same_as_external;
      KnuthBendix*                     _kb;
      size_t                           _max_overlap;
      size_t                           _max_rules;
      size_t                           _min_length_lhs_rule;
      std::list<Rule const*>::iterator _next_rule_it1;
      std::list<Rule const*>::iterator _next_rule_it2;
      ReductionOrdering const*         _order;
      OverlapMeasure*                  _overlap_measure;
      overlap_policy                   _overlap_policy;
      std::set<RuleLookup>             _set_rules;
      std::stack<Rule*>                _stack;
      internal_string_type*            _tmp_word1;
      internal_string_type*            _tmp_word2;
      mutable size_t                   _total_rules;

#ifdef LIBSEMIGROUPS_STATS
      //////////////////////////////////////////////////////////////////////////
      // ./configure --enable-stats functions
      //////////////////////////////////////////////////////////////////////////

      size_t max_active_word_length() {
        auto comp = [](libsemigroups::KnuthBendix::Rule const* p,
                       libsemigroups::KnuthBendix::Rule const* q) -> bool {
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

#ifdef LIBSEMIGROUPS_STATS

#endif

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - constructors and destructor - public
    //////////////////////////////////////////////////////////////////////////

    KnuthBendix::KnuthBendix() : FpSemiBase(), _impl(new KnuthBendixImpl()) {}

    KnuthBendix::KnuthBendix(ReductionOrdering* order)
        : FpSemiBase(), _impl(new KnuthBendixImpl(order)) {}

    KnuthBendix::KnuthBendix(FroidurePinBase* S) : KnuthBendix() {
      // TODO move the call to add_rules to elsewhere, so that it's done in
      // knuth_bendix so that this is done in a thread, and not when KnuthBendix
      // is constructed. If it is moved, then we will have to do
      // add_rules(S) to add_rule() so that we don't lose the relations
      // from S.
      // TODO what if S is the return value of isomorphic_non_fp_semigroup? Then
      // shouldn't we use the same alphabet as S?
      set_alphabet(S->nr_generators());
      set_isomorphic_non_fp_semigroup(S, false);
      // false is for "do not delete"
      add_rules(S);
    }

    KnuthBendix::KnuthBendix(FroidurePinBase& S) : KnuthBendix(&S) {}

    KnuthBendix::KnuthBendix(KnuthBendix const* kb)
        : KnuthBendix(new ReductionOrdering(kb->_impl->_order)) {
      // TODO _active_rules.reserve(kb->nr_rules());
      // TODO set confluence if known?
      set_alphabet(kb->alphabet());

      _impl->set_overlap_policy(kb->_impl->_overlap_policy);
      for (Rule const* rule : kb->_impl->_active_rules) {
        _impl->add_rule(_impl->new_rule(rule));
      }
    }

    KnuthBendix::~KnuthBendix() {
      delete _impl;
    }

    //////////////////////////////////////////////////////////////////////////
    // Setters for KnuthBendix optional parameters
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::set_check_confluence_interval(size_t interval) {
      _impl->_check_confluence_interval = interval;
    }

    void KnuthBendix::set_max_overlap(size_t val) {
      _impl->_max_overlap = val;
    }

    void KnuthBendix::set_max_rules(size_t val) {
      _impl->_max_rules = val;
    }

    void KnuthBendix::set_overlap_policy(overlap_policy p) {
      _impl->set_overlap_policy(p);
    }

    //////////////////////////////////////////////////////////////////////////
    // FpSemiBase - overridden non-pure virtual methods - public
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::set_alphabet(std::string const& lphbt) {
      FpSemiBase::set_alphabet(lphbt);
      _impl->_internal_is_same_as_external = true;
      for (size_t i = 0; i < lphbt.size(); ++i) {
        if (_impl->uint_to_internal_char(i) != lphbt[i]) {
          _impl->_internal_is_same_as_external = false;
          return;
        }
      }
    }

    void KnuthBendix::set_alphabet(size_t n) {
      FpSemiBase::set_alphabet(n);
      _impl->_internal_is_same_as_external = true;
    }

    void KnuthBendix::add_rule(std::string const& p, std::string const& q) {
      if (!is_alphabet_defined()) {
        throw LIBSEMIGROUPS_EXCEPTION("KnuthBendix::add_rule: cannot add rules "
                                      "before an alphabet is defined");
      }
      validate_word(p);
      validate_word(q);
      if (p != q) {
        _impl->add_rule(p, q);
        reset_isomorphic_non_fp_semigroup();
      }
    }

    bool KnuthBendix::is_obviously_finite() {
      return has_isomorphic_non_fp_semigroup()
             && isomorphic_non_fp_semigroup()->is_done();
    }

    bool KnuthBendix::is_obviously_infinite() {
      if (is_obviously_finite()) {
        // In this case the semigroup defined by the KnuthBendix is finite.
        return false;
      } else if (alphabet().size() > _impl->_active_rules.size()) {
        return true;
      }

      // TODO:
      // - check that every generator i occurs in the lhs of some rule (if not,
      //   then if two words have different numbers of i in them, then they are
      //   not the same.
      //
      // - check that for every generator there is a lhs consisting solely of
      //   that generator (otherwise the generators has infinite order).

      return false;
    }

    size_t KnuthBendix::size() {
      if (is_obviously_infinite()) {
        return POSITIVE_INFINITY;
      } else {
        return isomorphic_non_fp_semigroup()->size();
      }
    }

    size_t KnuthBendix::nr_rules() const noexcept {
      return _impl->_active_rules.size();
    }

    FroidurePinBase* KnuthBendix::isomorphic_non_fp_semigroup() {
      LIBSEMIGROUPS_ASSERT(is_alphabet_defined());
      // TODO check that no generators/rules can be added after this has been
      // called, or if they are that _isomorphic_non_fp_semigroup is reset again
      if (!has_isomorphic_non_fp_semigroup()) {
        run();
        // TODO use 0-param FroidurePin constructor
        auto T = new FroidurePin<KBE>({KBE(*this, 0)});
        for (size_t i = 1; i < alphabet().size(); ++i) {
          T->add_generator(KBE(*this, i));
        }
        set_isomorphic_non_fp_semigroup(T, true);
      }
      return get_isomorphic_non_fp_semigroup();
    }

    bool KnuthBendix::equal_to(std::string const& u, std::string const& v) {
      validate_word(u);
      validate_word(v);
      return _impl->equal_to(u, v);
    }

    std::string KnuthBendix::normal_form(std::string const& w) {
      validate_word(w);
      run();
      return rewrite(w);
    }

    //////////////////////////////////////////////////////////////////////////
    // Overridden virtual methods from Runner
    //////////////////////////////////////////////////////////////////////////

    void KnuthBendix::run() {
      knuth_bendix();
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix public methods for rules and rewriting
    //////////////////////////////////////////////////////////////////////////

    std::vector<std::pair<std::string, std::string>>
    KnuthBendix::rules() const {
      return _impl->rules();
    }

    std::string* KnuthBendix::rewrite(std::string* w) const {
      return _impl->rewrite(w);
    }

    std::string KnuthBendix::rewrite(std::string w) const {
      rewrite(&w);
      return w;
    }

    std::ostream& operator<<(std::ostream& os, KnuthBendix const& kb) {
      os << to_string(kb.rules()) << "\n";
      return os;
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix - main methods - public
    //////////////////////////////////////////////////////////////////////////

    bool KnuthBendix::confluent() const {
      return _impl->confluent();
    }

    void KnuthBendix::knuth_bendix() {
      _impl->knuth_bendix();
      report_why_we_stopped();
    }

    void KnuthBendix::knuth_bendix_by_overlap_length() {
      _impl->knuth_bendix_by_overlap_length();
    }

    //////////////////////////////////////////////////////////////////////////
    // KnuthBendix private methods
    //////////////////////////////////////////////////////////////////////////

  }  // namespace fpsemigroup

  namespace congruence {
    using class_index_type = CongBase::class_index_type;

    ////////////////////////////////////////////////////////////////////////////
    // KnuthBendix - constructors - public
    ////////////////////////////////////////////////////////////////////////////

    KnuthBendix::KnuthBendix()
        : CongBase(congruence_type::TWOSIDED),
          _kb(make_unique<fpsemigroup::KnuthBendix>()) {}

    KnuthBendix::KnuthBendix(fpsemigroup::KnuthBendix const* kb)
        // FIXME don't repeat the code here from the 0-param constructor
        : CongBase(congruence_type::TWOSIDED),
          _kb(make_unique<fpsemigroup::KnuthBendix>(kb)) {}

    KnuthBendix::KnuthBendix(FroidurePinBase& S)
        // FIXME don't repeat the code here from the 0-param constructor
        : CongBase(congruence_type::TWOSIDED),
          _kb(make_unique<fpsemigroup::KnuthBendix>(S)) {
      CongBase::set_nr_generators(S.nr_generators());
      set_parent(&S);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Runner - overridden pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    void KnuthBendix::run() {
      if (stopped()) {
        return;
      }
      auto stppd = [this]() -> bool { return _kb->dead() || _kb->timed_out(); };
      _kb->run_until(stppd);
      // It is essential that we call _kb->run() first and then
      // _kb->isomorphic_non_fp_semigroup(), since this might get killed
      // during _kb->run().
      if (!_kb->dead() && !_kb->timed_out()) {
        auto S = _kb->isomorphic_non_fp_semigroup();
        while (!S->is_done() && !_kb->dead() && !_kb->timed_out()) {
          S->run_until(stppd);
        }
      }
      report_why_we_stopped();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Runner - overridden non-pure virtual method - protected
    ////////////////////////////////////////////////////////////////////////////

    bool KnuthBendix::finished_impl() const {
      return _kb->has_isomorphic_non_fp_semigroup()
             && _kb->isomorphic_non_fp_semigroup()->is_done();
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - overridden pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    void KnuthBendix::add_pair(word_type const& lhs, word_type const& rhs) {
      _nr_generating_pairs++;  // defined in CongBase
      reset_quotient();
      _kb->add_rule(lhs, rhs);
    }

    word_type KnuthBendix::class_index_to_word(class_index_type i) {
      // i is checked in minimal_factorisation
      return _kb->isomorphic_non_fp_semigroup()->minimal_factorisation(i);
    }

    size_t KnuthBendix::nr_classes() {
      return _kb->size();
    }

    FroidurePinBase* KnuthBendix::quotient_semigroup() {
      if (!has_quotient()) {
        set_quotient(_kb->isomorphic_non_fp_semigroup(), false);
      }
      return get_quotient();
    }

    class_index_type KnuthBendix::word_to_class_index(word_type const& word) {
      // TODO check arg
      auto S
          = static_cast<FroidurePin<KBE>*>(_kb->isomorphic_non_fp_semigroup());
      size_t pos = S->position(
          KBE(_kb.get(), _kb->_impl->word_to_internal_string(word)));
      LIBSEMIGROUPS_ASSERT(pos != UNDEFINED);
      return pos;
    }

    ////////////////////////////////////////////////////////////////////////////
    // CongBase - overridden non-pure virtual methods - public
    ////////////////////////////////////////////////////////////////////////////

    CongBase::result_type
    KnuthBendix::const_contains(word_type const& lhs,
                                word_type const& rhs) const {
      if (_kb->rewrite(_kb->word_to_string(lhs))
          == _kb->rewrite(_kb->word_to_string(rhs))) {
        return result_type::TRUE;
      } else if (_kb->confluent()) {
        return result_type::FALSE;
      } else {
        return result_type::UNKNOWN;
      }
    }

    bool KnuthBendix::contains(word_type const& lhs, word_type const& rhs) {
      _kb->run();
      return const_contains(lhs, rhs) == result_type::TRUE;
    }

    void KnuthBendix::set_nr_generators(size_t n) {
      CongBase::set_nr_generators(n);
      _kb->set_alphabet(n);
    }

  }  // namespace congruence

  //////////////////////////////////////////////////////////////////////////////
  // KBE - constructors - private
  //////////////////////////////////////////////////////////////////////////////

  KBE::KBE(KnuthBendix* kb, internal_string_type w, bool reduce)
      : Element(), _kb(kb), _kb_word(w) {
    if (reduce) {
      _kb->_impl->internal_rewrite(&_kb_word);
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  // KBE - constructors - public
  //////////////////////////////////////////////////////////////////////////////

  KBE::KBE(KnuthBendix* kb) : KBE(kb, "", false) {}

  KBE::KBE(KnuthBendix* kb, internal_string_type const& w) : KBE(kb, w, true) {}

  KBE::KBE(KnuthBendix* kb, letter_type const& a)
      : KBE(kb, KnuthBendix::KnuthBendixImpl::uint_to_internal_string(a)) {}

  KBE::KBE(KnuthBendix& kb, letter_type const& a) : KBE(&kb, a) {}

  KBE::KBE(KnuthBendix* kb, word_type const& w)
      : KBE(kb, KnuthBendix::KnuthBendixImpl::word_to_internal_string(w)) {}

  KBE::KBE(KnuthBendix& kb, word_type const& w) : KBE(&kb, w) {}

  //////////////////////////////////////////////////////////////////////////////
  // Element - overriden methods - public
  //////////////////////////////////////////////////////////////////////////////

  bool KBE::operator==(Element const& that) const {
    return static_cast<KBE const&>(that)._kb_word == this->_kb_word;
  }

  bool KBE::operator<(Element const& that) const {
    internal_string_type const& u = this->_kb_word;
    internal_string_type const& v = static_cast<KBE const&>(that)._kb_word;
    if (u != v && (u.size() < v.size() || (u.size() == v.size() && u < v))) {
      // TODO allow other reduction orders here
      return true;
    } else {
      return false;
    }
  }

  void KBE::swap(Element& x) {
    auto& xx = static_cast<KBE&>(x);
    _kb_word.swap(xx._kb_word);
    std::swap(_kb, xx._kb);
    std::swap(this->_hash_value, xx._hash_value);
  }

  size_t KBE::complexity() const {
    return LIMIT_MAX;
  }

  size_t KBE::degree() const {
    return 0;
  }

  KBE KBE::identity() const {
    return KBE(_kb);
  }

  KBE* KBE::heap_copy() const {
    return new KBE(*this);
  }

  KBE* KBE::heap_identity() const {
    return this->identity().heap_copy();
  }

  void KBE::cache_hash_value() const {
    this->_hash_value = hash<internal_string_type>{}(_kb_word);
  }

  void KBE::redefine(Element const& x, Element const& y, size_t) {
    auto const& xx = static_cast<KBE const&>(x);
    auto const& yy = static_cast<KBE const&>(y);
    LIBSEMIGROUPS_ASSERT(xx._kb == yy._kb);
    _kb_word.clear();
    _kb_word.append(xx._kb_word);
    _kb_word.append(yy._kb_word);
    _kb->_impl->internal_rewrite(&_kb_word);
    this->reset_hash_value();
  }

#ifdef LIBSEMIGROUPS_DENSEHASHMAP
  Element* KBE::empty_key() const {
    return new KBE(_kb, "supercalifragilisticexpialidocious");
  }
#endif

  KBE::operator word_type() const {
    return _kb->_impl->internal_string_to_word(_kb_word);
  }

  KBE::operator std::string() const {
    std::string out(_kb_word);
    _kb->_impl->internal_to_external_string(out);  // changes out in-place
    return out;
  }

  template <> word_type FroidurePin<KBE>::factorisation(KBE const& x) {
    return x;
  }
}  // namespace libsemigroups
