//
// libsemigroups - C++ library for computing with semigroups and monoids
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

// TODO add max word length etc, number of unique lhs, max stack length, max
// number of active rules

#ifndef LIBSEMIGROUPS_RWS_H_
#define LIBSEMIGROUPS_RWS_H_

#include <atomic>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "cong.h"
#include "semigroups.h"

namespace libsemigroups {
  typedef char        rws_letter_t;
  typedef std::string rws_word_t;
  typedef std::pair<rws_word_t, rws_word_t> rws_rule_t;

  // Reduction order
  class RO {
   public:
    explicit RO(std::function<bool(rws_word_t const&, rws_word_t const&)> func)
        : _func(func) {}

    size_t operator()(rws_word_t const& p, rws_word_t const& q) const {
      return _func(p, q);
    }

   private:
    std::function<bool(rws_word_t const&, rws_word_t const&)> _func;
  };

  class SHORTLEX : public RO {
   public:
    SHORTLEX()
        : RO([](rws_word_t const& p, rws_word_t const& q) {
            return (p.size() > q.size() || (p.size() == q.size() && p > q));
          }) {}

    // Defined in terms of an initial ordering on rws_letter_t's
    // FIXME This is unsafe since we don't check that p and q consist of the
    // correct letters
    explicit SHORTLEX(std::function<bool(rws_letter_t const&,
                                         rws_letter_t const&)> letter_order)
        : RO([this](rws_word_t const& p, rws_word_t const& q) {
            if (p.size() > q.size()) {
              return true;
            } else if (p.size() < q.size()) {
              return false;
            }
            auto itp = p.cbegin();
            auto itq = q.cbegin();
            while (itp < p.cend() && *itp == *itq) {
              itp++;
              itq++;
            }
            return (itp != p.cend() && this->_letter_order(*itp, *itq));
          }),
          _letter_order(letter_order) {}

   private:
    std::function<bool(rws_letter_t const&, rws_letter_t const&)> _letter_order;
  };

  // TODO add more reduction orderings

  // Rewriting system
  class RWS {
   public:
    explicit RWS(RO* order)
        : _confluence_known(false),
          _is_confluent(),
          _nr_active_rules(0),
          _order(order),
          _report_next(0),
          _report_interval(1000),
          _rules(),
          _stack() {}

    RWS() : RWS(new SHORTLEX()) {}

    explicit RWS(std::vector<relation_t> const& relations) : RWS() {
      add_rules(relations);
    }

    explicit RWS(std::vector<rws_rule_t> const& rules) : RWS() {
      add_rules(rules);
    }

    RWS(RO* order, std::vector<relation_t> const& relations) : RWS(order) {
      add_rules(relations);
    }

    RWS(RO* order, std::vector<rws_rule_t> const& rules) : RWS(order) {
      add_rules(rules);
    }

    explicit RWS(Congruence& cong) : RWS() {
      add_rules(cong);
    }

    ~RWS() {
      delete _order;
    }

    bool is_confluent(std::atomic<bool>& killed) const;

    bool is_confluent() const {
      std::atomic<bool> killed(false);
      return is_confluent(killed);
    }

    size_t nr_rules() const {
      return _nr_active_rules;
    }

    void rewrite(rws_word_t& w, rws_word_t& buf) const;

    void rewrite(rws_word_t* w, rws_word_t& buf) const {
      rewrite(*w, buf);
    }

    rws_word_t rewrite(rws_word_t w) const {
      rws_word_t buf;
      rewrite(w, buf);
      return w;
    }

    void knuth_bendix() {
      std::atomic<bool> killed(false);
      knuth_bendix(killed);
    }

    void knuth_bendix(std::atomic<bool>& killed);

    void set_confluent(bool val) {
      _is_confluent     = val;
      _confluence_known = true;
    }

    rws_rule_t add_rule(rws_word_t const& p, rws_word_t const& q);
    rws_rule_t add_rule(rws_rule_t const& rule);

    void add_rules(std::vector<rws_rule_t> const& rules);
    void add_rules(std::vector<relation_t> const& relations);

    void add_rules(Congruence& cong) {
      add_rules(cong.relations());
      add_rules(cong.extra());
    }

    static rws_letter_t letter_to_rws_letter(letter_t const& a);
    static rws_word_t letter_to_rws_word(letter_t const& a);
    static rws_word_t word_to_rws_word(word_t const& w);

    static letter_t rws_letter_to_letter(rws_letter_t const& rws_letter);
    static word_t* rws_word_to_word(rws_word_t const* rws_word);

    void set_report(bool val) {
      glob_reporter.set_report(val);
    }

    void compress();

   private:
    void deactivate_rule(size_t i) {
      assert(i < _rules.size());
      _nr_active_rules--;
      _rules[i].second = false;
    }

    void clear_stack(std::atomic<bool>& killed, rws_word_t& buf);
    void
    overlap(size_t i, size_t j, std::atomic<bool>& killed, rws_word_t& buf);

    mutable bool _confluence_known;
    mutable bool _is_confluent;
    size_t       _nr_active_rules;
    RO*          _order;
    size_t       _report_next;
    size_t       _report_interval;
    std::vector<std::pair<rws_rule_t, bool>> _rules;
    std::stack<rws_rule_t> _stack;

    static rws_rule_t const NONE;
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_RWS_H_
