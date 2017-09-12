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
#include <string>

namespace libsemigroups {

#ifdef LIBSEMIGROUPS_STATS
  size_t RWS::max_active_word_length() {
    auto comp = [](libsemigroups::RWS::Rule const* p,
                   libsemigroups::RWS::Rule const* q) -> bool {
      return p->lhs()->size() < q->lhs()->size();
    };
    auto max = std::max_element(rules_cbegin(), rules_cend(), comp);
    if (max != rules_cend()) {
      _max_active_word_length
          = std::max(_max_active_word_length, (*max)->lhs()->size());
    }
    return _max_active_word_length;
  }
#endif

  RWS::~RWS() {
    delete _order;
    for (Rule const* rule : _active_rules) {
      delete const_cast<Rule*>(rule);
    }
    for (Rule* rule : _inactive_rules) {
      delete rule;
    }
  }

  rws_letter_t RWS::letter_to_rws_letter(letter_t const& a) {
    return static_cast<rws_letter_t>(a + 1);
    // return std::to_string(a).c_str()[0];
  }

  rws_word_t* RWS::letter_to_rws_word(letter_t const& a) {
    return new rws_word_t(1, letter_to_rws_letter(a));
    // return std::to_string(a);
  }

  // numbers to letters
  rws_word_t* RWS::word_to_rws_word(word_t const& w) {
    rws_word_t* ww = new rws_word_t();
    for (letter_t const& a : w) {
      (*ww) += letter_to_rws_letter(a);
    }
    return ww;
  }

  letter_t RWS::rws_letter_to_letter(rws_letter_t const& rws_letter) {
    return static_cast<letter_t>(rws_letter - 1);
  }

  word_t* RWS::rws_word_to_word(rws_word_t const* rws_word) {
    word_t* w = new word_t();
    w->reserve(rws_word->size());
    for (rws_letter_t const& rws_letter : *rws_word) {
      w->push_back(rws_letter_to_letter(rws_letter));
    }
    return w;
  }

  // Don't use new_rule here because this does not copy its args, and we don't
  // want it to, because it is used by add_rules, which creates the
  // rws_word_t's passed here.
  void RWS::add_rule(rws_word_t* p, rws_word_t* q) {
    if (*p != *q) {
      add_rule(new Rule(this, p, q));
    } else {
      // Since the RWS takes responsibility for deleting p and q, if we don't
      // actually add a rule we must delete p and q here, otherwise they are
      // never deleted.
      delete p;
      delete q;
    }
  }

  void RWS::add_rule(rws_word_t const& p, rws_word_t const& q) {
    if (p != q) {
      add_rule(new rws_word_t(p), new rws_word_t(q));
    }
  }

  void RWS::add_rule(Rule* rule) {
#ifdef LIBSEMIGROUPS_STATS
    _max_word_length  = std::max(_max_word_length, rule->lhs()->size());
    _max_active_rules = std::max(_max_active_rules, _active_rules.size());
    _unique_lhs_rules.insert(*rule->lhs());
#endif
    LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
    rule->activate();
    _active_rules.push_back(rule);
    // clear_stack relies on the fact that new rules are added to the end of
    // _active_rules
    if (_next_rule_it1 == _active_rules.end()) {
      _next_rule_it1--;
    }
    if (_next_rule_it2 == _active_rules.end()) {
      _next_rule_it2--;
    }
    _confluence_known = false;
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
    return it;
  }

  RWS::Rule* RWS::new_rule() const {
    _total_rules++;
    Rule* rule;
    if (!_inactive_rules.empty()) {
      rule = _inactive_rules.front();
      rule->clear();
      _inactive_rules.erase(_inactive_rules.begin());
    } else {
      rule = new Rule(this);
    }
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

  // Replace [it1_begin .. it1_begin + (it2_end - it2_begin)] by
  // [it2_begin .. it2_end], no checks performed.
  static inline void
  string_replace(typename std::string::iterator       it1_begin,
                 typename std::string::const_iterator it2_begin,
                 typename std::string::const_iterator it2_end) {
    while (it2_begin < it2_end) {
      *it1_begin = *it2_begin;
      it1_begin++;
      it2_begin++;
    }
  }

  // TODO collect minimum active rule length and just don't do anything if we
  // are trying to rewrite a shorter word

  // REWRITE_FROM_LEFT from Sims, p67
  // Caution: this contains the assumption that rules are length reducing!
  void RWS::rewrite(rws_word_t* u) const {
    auto v_begin = u->begin();
    auto v_end   = u->begin();
    auto w_begin = u->begin();
    auto w_end   = u->end();

    while (w_begin != w_end) {
      *v_end = *w_begin;
      v_end++;
      w_begin++;
      for (Rule const* rule : _active_rules) {
        // TODO use minimum active rule length here for v, if v is too short
        // then skip this entire loop
        if (rule->lhs()->size() <= static_cast<size_t>(v_end - v_begin)) {
          // x->first is an active rule, and isn't too long to be a suffix of v
          auto rule_cbegin = rule->lhs()->cbegin();
          auto rule_pos    = rule->lhs()->cend() - 1;
          auto v_pos       = v_end - 1;
          while ((rule_pos > rule_cbegin) && (*rule_pos == *v_pos)) {
            --rule_pos;
            --v_pos;
          }
          if (*rule_pos == *v_pos) {  // x->first.first is suffix of v
            v_end = v_pos;
            w_begin -= rule->rhs()->size();
            string_replace(w_begin, rule->rhs()->cbegin(), rule->rhs()->cend());
            break;
          }
        }
      }
    }
    u->erase(v_end - u->cbegin());
  }

  // CONFLUENT from Sims, p62
  bool RWS::is_confluent(std::atomic<bool>& killed) const {
    if (_confluence_known) {
      return _is_confluent;
    }
    rws_word_t* v = new rws_word_t();
    rws_word_t* w = new rws_word_t();
    for (Rule const* rule1 : _active_rules) {
      if (killed) {
        break;
      }
      for (Rule const* rule2 : _active_rules) {
        if (killed) {
          break;
        }
        for (auto it = rule1->lhs()->cend() - 1; it >= rule1->lhs()->cbegin();
             it--) {
          // Find longest common prefix of suffix B of rule1.first defined
          // by it and R = rule2.first
          // FIXME the following is not valid
          auto prefix
              = std::mismatch(it, rule1->lhs()->cend(), rule2->lhs()->cbegin());
          if (prefix.first != it || prefix.second != rule2->lhs()->cbegin()) {
            // FIXME this first if-condition redundant?
            // There was a match
            if (prefix.first == rule1->lhs()->cend()
                || prefix.second == rule2->lhs()->cend()) {
              v->append(rule1->lhs()->cbegin(), it);          // A
              v->append(*rule2->rhs());                       // S
              v->append(prefix.first, rule1->lhs()->cend());  // D
              rewrite(v);
              w->append(*rule1->rhs());                        // Q
              w->append(prefix.second, rule2->lhs()->cend());  // E
              rewrite(w);
              if (*v != *w) {
                _confluence_known = true;
                _is_confluent     = false;
                delete v;
                delete w;
                return false;
              }
              v->clear();
              w->clear();
            }
          }
        }
      }
    }
    delete v;
    delete w;
    if (!killed) {
      _confluence_known = true;
      _is_confluent     = true;
      return true;
    }
    return false;
  }

  // TEST_2 from Sims, p76
  void RWS::clear_stack(std::atomic<bool>& killed) {
    while (!_stack.empty() && !killed) {
#ifdef LIBSEMIGROUPS_STATS
      _max_stack_depth = std::max(_max_stack_depth, _stack.size());
#endif

      Rule* rule1 = _stack.top();
      _stack.pop();
      // Rewrite both sides and reorder if necessary . . .
      LIBSEMIGROUPS_ASSERT(!rule1->is_active());
      rule1->rewrite();
      if (*rule1->lhs() != *rule1->rhs()) {
        add_rule(rule1);  // rule1 is activated
        rws_word_t const* lhs = rule1->lhs();
        for (auto it = _active_rules.begin(); it != --_active_rules.end();) {
          Rule* rule2 = const_cast<Rule*>(*it);
          if (rule2->lhs()->find(*lhs) != std::string::npos) {
            it = remove_rule(it);
            _stack.push(rule2);
          } else {
            if (rule2->rhs()->find(*lhs) != std::string::npos) {
              rule2->rewrite_rhs();
            }
            it++;
          }
        }
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

  // OVERLAP_2 from Sims, p77
  void RWS::overlap(Rule const* u, Rule const* v, std::atomic<bool>& killed) {
    LIBSEMIGROUPS_ASSERT(u->is_active() && v->is_active());
    size_t m = std::min(u->lhs()->size(), v->lhs()->size()) - 1;

    for (size_t k = 1; k <= m && u->is_active() && v->is_active() && !killed;
         k++) {
      // Check if b = std::string(u->lhs()->cend() - k, u->lhs()->cend()) is a
      // prefix of v->lhs()
      auto first1 = u->lhs()->cend() - k;
      auto last1  = u->lhs()->cend();
      auto it     = v->lhs()->cbegin();
      while ((first1 < last1) && (*first1 == *it)) {
        ++first1;
        ++it;
      }
      if (first1 == last1) {  // b is a prefix of v.first
        Rule* rule = new_rule(u->lhs()->cbegin(),
                              u->lhs()->cend() - k,
                              u->rhs()->cbegin(),
                              u->rhs()->cend());
        rule->_lhs->append(*v->rhs());             // Q_j
        rule->_rhs->append(it, v->lhs()->cend());  // C
        LIBSEMIGROUPS_ASSERT(rule->lhs() != rule->rhs());
        _stack.emplace(rule);
        clear_stack(killed);
      }
    }
  }

  // KBS_2 from Sims, p77-78
  void RWS::knuth_bendix(std::atomic<bool>& killed) {
    if (is_confluent(killed) && !killed) {
      REPORT("the system is confluent already");
      return;
    }

    // Reduce the rules
    _next_rule_it1 = _active_rules.begin();
    while (_next_rule_it1 != _active_rules.end() && !killed) {
      // Copy *_next_rule_it1 and push into _stack so that it is not modified by
      // the call to clear_stack.
      _stack.push(new_rule(*_next_rule_it1));
      _next_rule_it1++;
      clear_stack(killed);
    }
    _next_rule_it1 = _active_rules.begin();
    size_t nr      = 0;
    while (_next_rule_it1 != _active_rules.end() && !killed) {
      Rule const* rule1 = *_next_rule_it1;
      _next_rule_it2    = _next_rule_it1;
      _next_rule_it1++;
      overlap(rule1, rule1, killed);
      while (_next_rule_it2 != _active_rules.begin() && rule1->is_active()) {
        _next_rule_it2--;
        Rule const* rule2 = *_next_rule_it2;
        overlap(rule1, rule2, killed);
        nr++;
        if (rule1->is_active() && rule2->is_active()) {
          nr++;
          overlap(rule2, rule1, killed);
        }
      }
      if (nr > 256) {
        nr = 0;
        if (is_confluent(killed)) {
          break;
        }
      }
    }
    if (killed) {
      REPORT("killed");
    } else {
      _confluence_known = true;
      _is_confluent     = true;
      REPORT("finished, active rules = " << _active_rules.size()
                                         << ", inactive rules = "
                                         << _inactive_rules.size()
                                         << ", rules defined = "
                                         << _total_rules);
#ifdef LIBSEMIGROUPS_STATS
      REPORT("max stack depth = " << _max_stack_depth);
#endif
    }
  }

  bool RWS::test_equals(word_t const& p, word_t const& q) {
    auto eq = [](rws_word_t* pp, rws_word_t* qq) -> bool { return *pp == *qq; };
    return test(word_to_rws_word(p), word_to_rws_word(q), eq);
  }

  bool RWS::test_equals(rws_word_t const& p, rws_word_t const& q) {
    auto eq = [](rws_word_t* pp, rws_word_t* qq) -> bool { return *pp == *qq; };
    return test(new rws_word_t(p), new rws_word_t(q), eq);
  }

  bool RWS::test_less_than(word_t const& p, word_t const& q) {
    return test(word_to_rws_word(p), word_to_rws_word(q), *_order);
  }

  bool RWS::test_less_than(rws_word_t const& p, rws_word_t const& q) {
    return test(new rws_word_t(p), new rws_word_t(q), *_order);
  }
}  // namespace libsemigroups
