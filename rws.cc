//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

  // REWRITE_FROM_LEFT from Sims, p67
  void RWS::rewrite(rws_word_t& w, rws_word_t& buf) const {
    buf = w;
    w.clear();
    while (!buf.empty()) {
      w.push_back(buf[0]);
      buf.erase(buf.begin());
      for (std::pair<rws_rule_t, bool> const& x : _rules) {
        if (x.second) {
          rws_rule_t rule = x.first;
          if (rule.first.size() <= w.size()) {
            // Check if rule.first is a suffix of v
            auto it     = w.end() - 1;
            auto first1 = rule.first.cend() - 1;
            auto last1  = rule.first.cbegin();
            while ((first1 > last1) && (*first1 == *it)) {
              --first1;
              --it;
            }
            if (*first1 == *it) {  // rule.first is suffix of v
              w.erase(it, w.end());
              buf.insert(buf.begin(), rule.second.cbegin(), rule.second.cend());
              break;
            }
          }
        }
      }
    }
    // std::cout << v << std::endl;
  }

  // CONFLUENT from Sims, p62
  bool RWS::is_confluent(std::atomic<bool>& killed) const {
    if (_confluence_known) {
      return _is_confluent;
    }

    rws_word_t buf;

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
              auto prefix =
                  std::mismatch(it, rule1.first.cend(), rule2.first.cbegin());
              if (prefix.first != it || prefix.second != rule2.first.cbegin()) {
                // FIXME this first if-condition redundant?
                // There was a match
                if (prefix.first == rule1.first.cend()
                    || prefix.second == rule2.first.cend()) {
                  rws_word_t v = rws_word_t(rule1.first.cbegin(), it);  // A
                  v.append(rule2.second);                               // S
                  v.append(prefix.first, rule1.first.cend());           // D
                  rewrite(v, buf);

                  rws_word_t w = rule1.second;                  // Q
                  w.append(prefix.second, rule2.first.cend());  // E
                  rewrite(w, buf);
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
  void RWS::clear_stack(std::atomic<bool>& killed, rws_word_t& buf) {
    while (!_stack.empty() && !killed) {
      rws_rule_t uv = _stack.top();
      _stack.pop();
      rewrite(uv.first, buf);
      rewrite(uv.second, buf);
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
                rewrite(rule.second, buf);
              }
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

  // OVERLAP_2 from Sims, p77
  void
  RWS::overlap(size_t i, size_t j, std::atomic<bool>& killed, rws_word_t& buf) {
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
        clear_stack(killed, buf);
      }
    }
  }

  // KBS_2 from Sims, p77-78
  void RWS::knuth_bendix(std::atomic<bool>& killed) {
    if (is_confluent(killed) && !killed) {
      REPORT("the system is confluent already");
      return;
    }
    rws_word_t buf;
    // Reduce the rules
    for (size_t i = 0; i < _rules.size() && !killed; i++) {
      if (_rules[i].second) {
        _stack.push(_rules[i].first);
        clear_stack(killed, buf);
      }
    }
    size_t nr = 0;
    for (size_t i = 0; i < _rules.size() && !killed; i++) {
      rws_rule_t rule1 = _rules[i].first;
      for (size_t j = 0; j <= i && _rules[i].second && !killed; j++) {
        if (_rules[j].second) {  // _rules[j] is active
          nr++;
          overlap(i, j, killed, buf);
        }
        if (j < i && _rules[i].second && _rules[j].second) {
          nr++;
          overlap(j, i, killed, buf);
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
