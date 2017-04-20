//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

// TODO add more reporting

#include "rws.h"

#include <algorithm>
#include <string>

namespace libsemigroups {
  rws_rule_t const RWS::NONE = std::make_pair("", "");

  void RWS::compress() {
    for (auto it = _rules.begin(); it < _rules.end(); it++) {
      while (!(*it).second) {
        it = _rules.erase(it);
      }
    }
  }

  rws_letter_t RWS::letter_to_rws_letter(letter_t const& a) {
    return static_cast<rws_letter_t>(a + 1);
    // return std::to_string(a).c_str()[0];
  }

  rws_word_t RWS::letter_to_rws_word(letter_t const& a) {
    return {letter_to_rws_letter(a)};
    // return std::to_string(a);
  }

  // numbers to letters
  rws_word_t RWS::word_to_rws_word(word_t const& w) {
    rws_word_t ww;
    for (letter_t const& a : w) {
      ww += letter_to_rws_letter(a);
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

  rws_rule_t RWS::add_rule(rws_word_t const& p, rws_word_t const& q) {
    if (p == q) {
      return NONE;
    }
    _nr_active_rules++;

    std::pair<rws_word_t, rws_word_t> pq;
    if ((*_order)(p, q)) {
      pq = std::make_pair(p, q);
    } else {
      pq = std::make_pair(q, p);
    }

    _rules.emplace_back(std::make_pair(pq, true));
    _confluence_known = false;
    return pq;
  }

  rws_rule_t RWS::add_rule(rws_rule_t const& rule) {
    return add_rule(rule.first, rule.second);
  }

  void RWS::add_rules(std::vector<rws_rule_t> const& rules) {
    for (rws_rule_t const& rule : rules) {
      add_rule(rule.first, rule.second);
    }
  }

  void RWS::add_rules(std::vector<relation_t> const& relations) {
    for (relation_t const& rel : relations) {
      add_rule(word_to_rws_word(rel.first), word_to_rws_word(rel.second));
    }
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
  //

  // REWRITE_FROM_LEFT from Sims, p67
  // Caution: this contains the assumption that rules are length reducing!
  void RWS::rewrite(rws_word_t& u) const {
    auto v_begin = u.begin();
    auto v_end   = u.begin();
    auto w_begin = u.begin();
    auto w_end   = u.end();

    while (w_begin != w_end) {
      *v_end = *w_begin;
      v_end++;
      w_begin++;
      for (std::pair<rws_rule_t, bool> const& x : _rules) {
        // TODO use minimum active rule length here for v, if v is too short
        // then skip this entire loop
        if (x.second
            && x.first.first.size() <= static_cast<size_t>(v_end - v_begin)) {
          // x.first is an active rule, and isn't too long to be a suffix of v
          auto rule_cbegin = x.first.first.cbegin();
          auto rule_pos    = x.first.first.cend() - 1;
          auto v_pos       = v_end - 1;
          while ((rule_pos > rule_cbegin) && (*rule_pos == *v_pos)) {
            --rule_pos;
            --v_pos;
          }
          if (*rule_pos == *v_pos) {  // x.first.first is suffix of v
            v_end = v_pos;
            w_begin -= x.first.second.size();
            string_replace(
                w_begin, x.first.second.cbegin(), x.first.second.cend());
            break;
          }
        }
      }
    }
    u.erase(v_end, u.cend());
  }

  // CONFLUENT from Sims, p62
  bool RWS::is_confluent(std::atomic<bool>& killed) const {
    if (_confluence_known) {
      return _is_confluent;
    }

    for (size_t i = 0; i < _rules.size() && !killed; i++) {
      if (_rules[i].second) {  // _rules[i] is active
        rws_rule_t rule1 = _rules[i].first;
        for (size_t j = 0; j < _rules.size() && !killed; j++) {
          if (_rules[j].second) {
            rws_rule_t rule2 = _rules[j].first;
            for (auto it = rule1.first.cend() - 1; it >= rule1.first.cbegin();
                 it--) {
              // Find longest common prefix of suffix B of rule1.first defined
              // by it and R = rule2.first
              // FIXME the following is not valid
              auto prefix
                  = std::mismatch(it, rule1.first.cend(), rule2.first.cbegin());
              if (prefix.first != it || prefix.second != rule2.first.cbegin()) {
                // FIXME this first if-condition redundant?
                // There was a match
                if (prefix.first == rule1.first.cend()
                    || prefix.second == rule2.first.cend()) {
                  rws_word_t v = rws_word_t(rule1.first.cbegin(), it);  // A
                  v.append(rule2.second);                               // S
                  v.append(prefix.first, rule1.first.cend());           // D
                  rewrite(v);

                  rws_word_t w = rule1.second;                  // Q
                  w.append(prefix.second, rule2.first.cend());  // E
                  rewrite(w);
                  if (v != w) {
                    _confluence_known = true;
                    _is_confluent     = false;
                    return false;
                  }
                }
              }
            }
          }
        }
      }
    }
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
      rws_rule_t uv = _stack.top();  // FIXME copy!
      _stack.pop();
      rewrite(uv.first);
      rewrite(uv.second);
      if (uv.first != uv.second) {
        rws_rule_t ab = add_rule(uv);
        // a -> b must be added at the end of _rules
        rws_word_t a = ab.first;
        rws_word_t b = ab.second;
        for (size_t i = 0; i < _rules.size() - 1; i++) {
          if (_rules[i].second) {  // _rules[i] is active
            rws_rule_t rule = _rules[i].first;
            size_t     pos  = rule.first.find(a);
            if (pos != std::string::npos) {
              _stack.push(rule);
              deactivate_rule(i);
            } else {
              pos = rule.second.find(a);
              if (pos != std::string::npos) {
                rewrite(rule.second);
              }
            }
          }
        }
      }
      if (_report_next++ > _report_interval) {
        REPORT("total rules = " << _rules.size() << ", active rules = "
                                << _nr_active_rules);
        _report_next = 0;
      }
    }
  }

  // OVERLAP_2 from Sims, p77
  void RWS::overlap(size_t i, size_t j, std::atomic<bool>& killed) {
    // Assert both rules are active
    assert(_rules[i].second && _rules[j].second);
    rws_rule_t u = _rules[i].first;
    rws_rule_t v = _rules[j].first;

    size_t m = std::min(u.first.size(), v.first.size()) - 1;

    for (size_t k = 1;
         k <= m && _rules[i].second && _rules[j].second && !killed;
         k++) {
      // Check if b = std::string(u.first.cend() - k, u.first.cend()) is a
      // prefix of v.first
      auto first1 = u.first.cend() - k;
      auto last1  = u.first.cend();
      auto it     = v.first.cbegin();
      while ((first1 < last1) && (*first1 == *it)) {
        ++first1;
        ++it;
      }
      if (first1 == last1) {  // b is a prefix of v.first
        rws_word_t a = rws_word_t(u.first.cbegin(), u.first.cend() - k);  // A
        a.append(v.second);                                               // Q_j
        rws_word_t b = u.second;                                          // Q_i
        b.append(it, v.first.cend());                                     // C
        _stack.emplace(rws_rule_t(a, b));
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
    for (size_t i = 0; i < _rules.size() && !killed; i++) {
      if (_rules[i].second) {
        _stack.push(_rules[i].first);
        clear_stack(killed);
      }
    }
    size_t nr = 0;
    for (size_t i = 0; i < _rules.size() && !killed; i++) {
      rws_rule_t rule1 = _rules[i].first;
      for (size_t j = 0; j <= i && _rules[i].second && !killed; j++) {
        if (_rules[j].second) {  // _rules[j] is active
          nr++;
          overlap(i, j, killed);
        }
        if (j < i && _rules[i].second && _rules[j].second) {
          nr++;
          overlap(j, i, killed);
        }
      }
      if (nr > 1024) {
        nr = 0;
        if (is_confluent(killed)) {
          break;
        }
      }
    }
    if (killed) {
      REPORT("killed");
    } else {
      REPORT("finished, total rules = " << _rules.size() << ", active rules = "
                                        << _nr_active_rules);
      compress();
    }
  }
}  // namespace libsemigroups
