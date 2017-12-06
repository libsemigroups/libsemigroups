//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

#include "rws.h"

#include <algorithm>
#include <set>
#include <string>

namespace libsemigroups {

  using rws_word_t = RWS::rws_word_t;

  // Internal functions for handling strings

  // Replace [it1_begin .. it1_begin + (it2_end - it2_begin)] by
  // [it2_begin .. it2_end], no checks performed.
  static inline void string_replace(std::string::iterator       it1_begin,
                                    std::string::const_iterator it2_begin,
                                    std::string::const_iterator it2_end) {
    while (it2_begin < it2_end) {
      *it1_begin = *it2_begin;
      ++it1_begin;
      ++it2_begin;
    }
  }

  // Returns true if [first_prefix, last_prefix) is a prefix of [first_word,
  // last_word).
  static inline bool is_prefix(std::string::const_iterator const& first_word,
                               std::string::const_iterator const& last_word,
                               std::string::const_iterator const& first_prefix,
                               std::string::const_iterator const& last_prefix) {
    LIBSEMIGROUPS_ASSERT(first_word <= last_word);
    // We don't care if first_prefix > last_prefix
    if (last_prefix - first_prefix > last_word - first_word) {
      return false;
    }
    // Check if [first_prefix, last_prefix) equals [first_word, first_word +
    // (last_suffix - first_suffix))
    return std::equal(first_prefix, last_prefix, first_word);
  }

#ifdef LIBSEMIGROUPS_DEBUG
  // Returns true if [first_suffix, last_suffix) is a suffix of [first_word,
  // last_word).
  static inline bool is_suffix(std::string::const_iterator const& first_word,
                               std::string::const_iterator const& last_word,
                               std::string::const_iterator const& first_suffix,
                               std::string::const_iterator const& last_suffix) {
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

  static inline std::pair<std::string::const_iterator,
                          std::string::const_iterator>
  maximum_common_prefix(std::string::const_iterator        first_word1,
                        std::string::const_iterator const& last_word1,
                        std::string::const_iterator        first_word2,
                        std::string::const_iterator const& last_word2) {
    while (*first_word1 == *first_word2 && first_word1 < last_word1
           && first_word2 < last_word2) {
      ++first_word1;
      ++first_word2;
    }
    return std::make_pair(first_word1, first_word2);
  }

  // Initialise static data members
  std::string const RWS::STANDARD_ALPHABET = "";

// Functions for when stats mode is enabled
#ifdef LIBSEMIGROUPS_STATS
  size_t RWS::max_active_word_length() {
    auto comp = [](libsemigroups::RWS::Rule const* p,
                   libsemigroups::RWS::Rule const* q) -> bool {
      return p->lhs()->size() < q->lhs()->size();
    };
    auto max
        = std::max_element(_active_rules.cbegin(), _active_rules.cend(), comp);
    if (max != _active_rules.cend()) {
      _max_active_word_length
          = std::max(_max_active_word_length, (*max)->lhs()->size());
    }
    return _max_active_word_length;
  }
#endif

  // OverlapMeasure and substructs

  struct RWS::OverlapMeasure {
    virtual size_t operator()(Rule const*                       AB,
                              Rule const*                       BC,
                              rws_word_t::const_iterator const& it)
        = 0;
    virtual ~OverlapMeasure() {}
    // Forward declarations
    struct ABC;
    struct AB_BC;
    struct max_AB_BC;
  };

  struct RWS::OverlapMeasure::ABC : public RWS::OverlapMeasure {
    size_t operator()(Rule const*                       AB,
                      Rule const*                       BC,
                      rws_word_t::const_iterator const& it) final {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      // |A| + |BC|
      return (it - AB->lhs()->cbegin()) + BC->lhs()->size();
    }
  };

  struct RWS::OverlapMeasure::AB_BC : public RWS::OverlapMeasure {
    size_t operator()(Rule const*                       AB,
                      Rule const*                       BC,
                      rws_word_t::const_iterator const& it) final {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      (void) it;
      // |AB| + |BC|
      return AB->lhs()->size() + BC->lhs()->size();
    }
  };

  struct RWS::OverlapMeasure::max_AB_BC : public RWS::OverlapMeasure {
    size_t operator()(Rule const*                       AB,
                      Rule const*                       BC,
                      rws_word_t::const_iterator const& it) final {
      LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
      LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
      LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
      (void) it;
      // max(|AB|, |BC|)
      return std::max(AB->lhs()->size(), BC->lhs()->size());
    }
  };

  void RWS::set_overlap_measure(overlap_measure measure) {
    delete _overlap_measure;

    switch (measure) {
      case ABC:
        _overlap_measure = new OverlapMeasure::ABC();
        break;
      case AB_BC:
        _overlap_measure = new OverlapMeasure::AB_BC();
        break;
      case max_AB_BC:
        _overlap_measure = new OverlapMeasure::max_AB_BC();
        break;
      default:
        LIBSEMIGROUPS_ASSERT(false);
    }
  }

  // RWS class non-static methods
  RWS::~RWS() {
    delete _order;
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

  //! This method allows a RWS object to be left shifted into a std::ostream,
  //! such as std::cout. The currently active rules of the system are
  //! represented in the output.
  std::ostream& operator<<(std::ostream& os, RWS const& rws) {
    for (auto rule : rws._active_rules) {
      os << *rule << std::endl;
    }
    return os;
  }

  // Public
  void RWS::add_rules(std::vector<relation_t> const& relations) {
    for (relation_t const& rel : relations) {
      if (rel.first != rel.second) {
        add_rule(new_rule(word_to_rws_word(rel.first),
                          word_to_rws_word(rel.second)));
      }
    }
  }

  void RWS::add_rule(std::string const& p, std::string const& q) {
    if (p != q) {
      auto pp = new rws_word_t(p);
      auto qq = new rws_word_t(q);
      string_to_rws_word(*pp);
      string_to_rws_word(*qq);
      add_rule(new_rule(pp, qq));
    }
  }

  // Private
  void RWS::add_rule(Rule* rule) {
#ifdef LIBSEMIGROUPS_STATS
    _max_word_length  = std::max(_max_word_length, rule->lhs()->size());
    _max_active_rules = std::max(_max_active_rules, _active_rules.size());
    _unique_lhs_rules.insert(*rule->lhs());
#endif
    LIBSEMIGROUPS_ASSERT(*rule->lhs() != *rule->rhs());
    if (!_set_rules.emplace(RuleLookup(rule)).second) {
      // The rules are not reduced, this should only happen if we are calling
      // add_rule from outside the class (i.e. we are initialising the RWS).
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
      // FIXME this is not valid when using non-length reducing orderings (such
      // as RECURSIVE)
      _min_length_lhs_rule = rule->lhs()->size();
    }

    LIBSEMIGROUPS_ASSERT(_set_rules.size() == _active_rules.size());
  }

  std::list<RWS::Rule const*>::iterator
  RWS::remove_rule(std::list<RWS::Rule const*>::iterator it) {
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
    auto ret = _set_rules.erase(RuleLookup(rule));
    LIBSEMIGROUPS_ASSERT(ret == 1);
#else
    _set_rules.erase(RuleLookup(rule));
#endif
    LIBSEMIGROUPS_ASSERT(_set_rules.size() == _active_rules.size());

    return it;
  }

  RWS::Rule* RWS::new_rule() const {
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

  RWS::Rule* RWS::new_rule(rws_word_t* lhs, rws_word_t* rhs) const {
    Rule* rule = new_rule();
    delete rule->_lhs;
    delete rule->_rhs;
    rule->_lhs = lhs;
    rule->_rhs = rhs;
    rule->reorder();
    return rule;
  }

  RWS::Rule* RWS::new_rule(Rule const* rule1) const {
    Rule* rule2 = new_rule();
    rule2->_lhs->append(*rule1->lhs());  // copies lhs
    rule2->_rhs->append(*rule1->rhs());  // copies rhs
    return rule2;
  }

  RWS::Rule* RWS::new_rule(rws_word_t::const_iterator begin_lhs,
                           rws_word_t::const_iterator end_lhs,
                           rws_word_t::const_iterator begin_rhs,
                           rws_word_t::const_iterator end_rhs) const {
    Rule* rule = new_rule();
    rule->_lhs->append(begin_lhs, end_lhs);
    rule->_rhs->append(begin_rhs, end_rhs);
    return rule;
  }

  std::vector<std::pair<std::string, std::string>> RWS::rules() const {
    std::vector<std::pair<std::string, std::string>> rules;
    rules.reserve(_active_rules.size());
    for (Rule const* rule : _active_rules) {
      rws_word_t lhs = rws_word_t(*rule->lhs());
      rws_word_t rhs = rws_word_t(*rule->rhs());
      rws_word_to_string(lhs);
      rws_word_to_string(rhs);
      rules.push_back(std::make_pair(lhs, rhs));
    }
    std::sort(rules.begin(),
              rules.end(),
              [this](std::pair<std::string, std::string> rule1,
                     std::pair<std::string, std::string> rule2) {
                return (*_order)(rule2.first, rule1.first)
                       || (rule1.first == rule2.first
                           && (*_order)(rule2.second, rule1.second));
              });
    return rules;
  }

  bool RWS::rule(std::string p, std::string q) const {
    // TODO Could use a RuleLookup and instead of this!
    // take references and then copy into _tmp_word1/2
    string_to_rws_word(p);
    string_to_rws_word(q);
    for (Rule const* rule : _active_rules) {
      if (*rule->lhs() == p && *rule->rhs() == q) {
        return true;
      } else if (*rule->lhs() == q && *rule->rhs() == p) {
        return true;
      }
    }
    return false;
  }

  // REWRITE_FROM_LEFT from Sims, p67
  // Caution: this uses the assumption that rules are length reducing, if it is
  // not, then u might not have sufficient space!
  void RWS::internal_rewrite(rws_word_t* u) const {
    if (u->size() < _min_length_lhs_rule) {
      return;
    }
    rws_word_t::iterator const& v_begin = u->begin();
    rws_word_t::iterator        v_end   = u->begin() + _min_length_lhs_rule - 1;
    rws_word_t::iterator        w_begin = v_end;
    rws_word_t::iterator const& w_end   = u->end();

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
          string_replace(w_begin, rule->rhs()->cbegin(), rule->rhs()->cend());
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

  // CONFLUENT from Sims, p62
  bool RWS::confluent(std::atomic<bool>& killed) const {
    // FIXME if the stack is not empty then what happens here?
    LIBSEMIGROUPS_ASSERT(_stack.empty());
    if (!_confluence_known) {
      _confluent        = true;
      _confluence_known = true;
      rws_word_t word1;
      rws_word_t word2;

      for (auto it1 = _active_rules.cbegin();
           it1 != _active_rules.cend() && !killed;
           ++it1) {
        Rule const* rule1 = *it1;
        // Seems to be much faster to do this in reverse.
        for (auto it2 = _active_rules.crbegin();
             it2 != _active_rules.crend() && !killed;
             ++it2) {
          Rule const* rule2 = *it2;
          for (auto it = rule1->lhs()->cend() - 1;
               it >= rule1->lhs()->cbegin() && !killed;
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
      }
      if (killed) {
        _confluence_known = false;
      }
    }
    return _confluent;
  }

  // TEST_2 from Sims, p76
  void RWS::clear_stack(std::atomic<bool>& killed) {
    while (!_stack.empty() && !killed) {
#ifdef LIBSEMIGROUPS_STATS
      _max_stack_depth = std::max(_max_stack_depth, _stack.size());
#endif

      Rule* rule1 = _stack.top();
      _stack.pop();
      LIBSEMIGROUPS_ASSERT(!rule1->active());
      LIBSEMIGROUPS_ASSERT(*rule1->lhs() != *rule1->rhs());
      // Rewrite both sides and reorder if necessary . . .
      rule1->rewrite();
      if (*rule1->lhs() != *rule1->rhs()) {
        rws_word_t const* lhs = rule1->lhs();
        for (auto it = _active_rules.begin(); it != _active_rules.end();) {
          Rule* rule2 = const_cast<Rule*>(*it);
          if (rule2->lhs()->find(*lhs) != std::string::npos) {
            it = remove_rule(it);
            LIBSEMIGROUPS_ASSERT(*rule2->lhs() != *rule2->rhs());
            // rule2 is added to _inactive_rules by clear_stack
            _stack.emplace(rule2);
          } else {
            if (rule2->rhs()->find(*lhs) != std::string::npos) {
              rule2->rewrite_rhs();
            }
            ++it;
          }
        }
        add_rule(rule1);
        // rule1 is activated, we do this after removing rules that rule1 makes
        // redundant to avoid failing to insert rule1 in _set_rules
      } else {
        _inactive_rules.push_back(rule1);
      }
      if (_report_next++ > _report_interval) {
        REPORT("active rules = " << _active_rules.size()
                                 << ", inactive rules = "
                                 << _inactive_rules.size()
                                 << ", rules defined = "
                                 << _total_rules);
#ifdef LIBSEMIGROUPS_STATS
        REPORT("max stack depth        = " << _max_stack_depth);
        REPORT("max word length        = " << _max_word_length);
        REPORT("max active word length = " << max_active_word_length());
        REPORT("max active rules       = " << _max_active_rules);
        REPORT("number of unique lhs   = " << _unique_lhs_rules.size());
#endif
        _report_next = 0;
      }
    }
  }

  void RWS::push_stack(Rule* rule) {
    std::atomic<bool> killed(false);
    push_stack(rule, killed);
  }

  void RWS::push_stack(Rule* rule, std::atomic<bool>& killed) {
    LIBSEMIGROUPS_ASSERT(!rule->active());
    if (*rule->lhs() != *rule->rhs()) {
      _stack.emplace(rule);
      clear_stack(killed);
    } else {
      _inactive_rules.push_back(rule);
    }
  }

  // OVERLAP_2 from Sims, p77
  void RWS::overlap(Rule const* u, Rule const* v, std::atomic<bool>& killed) {
    LIBSEMIGROUPS_ASSERT(u->active() && v->active());
    auto limit
        = u->lhs()->cend() - std::min(u->lhs()->size(), v->lhs()->size());
    int64_t u_id = u->id();
    int64_t v_id = v->id();
    for (auto it = u->lhs()->cend() - 1;
         it > limit && !killed && u_id == u->id() && v_id == v->id()
         && (_max_overlap == UNBOUNDED
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
        push_stack(rule, killed);
        // It can be that the iterator `it` is invalidated by the call to
        // push_stack (i.e. if `u` is deactivated, then rewritten, actually
        // changed, and reactivated) and that is the reason for the checks in
        // the for-loop above. If this is the case, then we should stop
        // considering the overlaps of u and v here, and note that they will be
        // considered later, because when the rule `u` is reactivated it is
        // added to the end of the active rules list.
      }
    }
  }

  // KBS_2 from Sims, p77-78
  void RWS::knuth_bendix(std::atomic<bool>& killed) {
    Timer timer;
    if (_stack.empty() && confluent(killed) && !killed) {
      // _stack can be non-empty if non-reduced rules were used to define the
      // RWS.  If _stack is non-empty, then it means that the rules in
      // _active_rules might not define the system.
      REPORT("the system is confluent already");
      return;
    } else if (_active_rules.size() >= _max_rules) {
      return;
    }
    // Reduce the rules
    _next_rule_it1 = _active_rules.begin();
    while (_next_rule_it1 != _active_rules.end() && !killed) {
      // Copy *_next_rule_it1 and push_stack so that it is not modified by
      // the call to clear_stack.
      LIBSEMIGROUPS_ASSERT((*_next_rule_it1)->lhs()
                           != (*_next_rule_it1)->rhs());
      push_stack(new_rule(*_next_rule_it1), killed);
      ++_next_rule_it1;
    }
    _next_rule_it1 = _active_rules.begin();
    size_t nr      = 0;
    while (_next_rule_it1 != _active_rules.cend() && !killed
           && _active_rules.size() < _max_rules) {
      Rule const* rule1 = *_next_rule_it1;
      _next_rule_it2    = _next_rule_it1;
      ++_next_rule_it1;
      overlap(rule1, rule1, killed);
      while (_next_rule_it2 != _active_rules.begin() && rule1->active()) {
        --_next_rule_it2;
        Rule const* rule2 = *_next_rule_it2;
        overlap(rule1, rule2, killed);
        ++nr;
        if (rule1->active() && rule2->active()) {
          ++nr;
          overlap(rule2, rule1, killed);
        }
      }
      if (nr > _check_confluence_interval) {
        if (confluent(killed)) {
          break;
        }
        nr = 0;
      }
      if (_next_rule_it1 == _active_rules.cend()) {
        clear_stack(killed);
      }
    }
    if (killed) {
      REPORT("killed");
    } else {
      // LIBSEMIGROUPS_ASSERT(_stack.empty());
      // Seems that the stack can be non-empty here in RWS 12, 14, 16 and maybe
      // more
      if (_max_overlap == UNBOUNDED && _max_rules == UNBOUNDED) {
        _confluence_known = true;
        _confluent        = true;
        for (Rule* rule : _inactive_rules) {
          delete rule;
        }
        _inactive_rules.clear();
      }
      REPORT("finished, active rules = " << _active_rules.size()
                                         << ", inactive rules = "
                                         << _inactive_rules.size()
                                         << ", rules defined = "
                                         << _total_rules);
#ifdef LIBSEMIGROUPS_STATS
      REPORT("max stack depth = " << _max_stack_depth);
#endif
      REPORT("elapsed time = " << timer);
    }
  }

  // Main method
  bool RWS::test_equals(std::string* p, std::string* q) {
    if (*p == *q) {
      return true;
    }
    if (p->size() < _min_length_lhs_rule && q->size() < _min_length_lhs_rule) {
      return p == q;
    }
    knuth_bendix();
    string_to_rws_word(*p);
    internal_rewrite(p);
    string_to_rws_word(*q);
    internal_rewrite(q);
    return *p == *q;
  }

  bool RWS::test_equals(word_t const& p, word_t const& q) {
    return test_equals(word_to_rws_word(p, _tmp_word1),
                       word_to_rws_word(q, _tmp_word2));
  }

  bool RWS::test_equals(std::initializer_list<size_t> const& p,
                        std::initializer_list<size_t> const& q) {
    return test_equals(word_t(p), word_t(q));
  }

  bool RWS::test_equals(std::string const& p, std::string const& q) {
    _tmp_word1->replace(
        _tmp_word1->begin(), _tmp_word1->end(), p.cbegin(), p.cend());
    _tmp_word2->replace(
        _tmp_word2->begin(), _tmp_word2->end(), q.cbegin(), q.cend());
    return test_equals(_tmp_word1, _tmp_word2);
  }

  // Main method
  bool RWS::test_less_than(std::string* p, std::string* q) {
    if (*p == *q) {
      return false;
    }
    knuth_bendix();
    string_to_rws_word(*p);
    internal_rewrite(p);
    string_to_rws_word(*q);
    internal_rewrite(q);
    // _order is defined in terms of rws_word_t's, i.e. the internal
    // representation of words.
    return (*_order)(q, p);
  }

  bool RWS::test_less_than(word_t const& p, word_t const& q) {
    return test_less_than(word_to_rws_word(p, _tmp_word1),
                          word_to_rws_word(q, _tmp_word2));
  }

  bool RWS::test_less_than(std::string const& p, std::string const& q) {
    _tmp_word1->replace(
        _tmp_word1->begin(), _tmp_word1->end(), p.cbegin(), p.cend());
    _tmp_word2->replace(
        _tmp_word2->begin(), _tmp_word2->end(), q.cbegin(), q.cend());
    return test_less_than(_tmp_word1, _tmp_word2);
  }

}  // namespace libsemigroups
