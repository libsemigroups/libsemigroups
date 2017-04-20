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

// TODO add max word length etc, number of unique lhs, max stack length, max
// number of active rules

#ifndef LIBSEMIGROUPS_SRC_RWS_H_
#define LIBSEMIGROUPS_SRC_RWS_H_

#include <atomic>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "cong.h"
#include "semigroups.h"

namespace libsemigroups {
  //! Type for letters for rewriting systems.
  typedef char rws_letter_t;

  //! Type for words for rewriting systems.
  typedef std::string rws_word_t;

  //! Type for rules in rewriting systems.
  typedef std::pair<rws_word_t, rws_word_t> rws_rule_t;

  //!  This class provides a call operator which can be used to compare
  //! libsemigroups::rws_word_t.
  //!
  //! A *reduction ordering* is a TODO
  class RO {
   public:
    //! A constructor.
    //!
    //! This constructs a reduction ordering object whose call operator uses
    //! the function \p func to compare libsemigroups::rws_word_t's. It is the
    //! responsibility of the caller to verify that \p func specifies a
    //! reduction ordering.
    explicit RO(std::function<bool(rws_word_t const&, rws_word_t const&)> func)
        : _func(func) {}

    //! Returns \c true if the word \p p is greater than the word
    //! \p q in the reduction ordering.
    size_t operator()(rws_word_t const& p, rws_word_t const& q) const {
      return _func(p, q);
    }

    //! Returns \c true if the libsemigroups::rws_word_t pointed to by \p p is
    //! greater than the libsemigroups::rws_word_t pointed to by \p q in the
    //! reduction ordering.
    size_t operator()(rws_word_t const* p, rws_word_t const* q) const {
      return _func(*p, *q);
    }

   private:
    std::function<bool(rws_word_t const&, rws_word_t const&)> _func;
  };

  //! This class implements the shortlex reduction ordering derived from
  //! an ordering on libsemigroups::rws_letter_t's.

  class SHORTLEX : public RO {
   public:
    //! Constructs a short-lex reduction ordering object derived from the
    //! order of on libsemigroups::rws_letter_t's given by the operator <.
    SHORTLEX()
        : RO([](rws_word_t const& p, rws_word_t const& q) {
            return (p.size() > q.size() || (p.size() == q.size() && p > q));
          }) {}

    //! A constructor.
    //!
    //! This constructs a short-lex reduction ordering object derived from the
    //! order on libsemigroups::rws_letter_t's given by the parameter
    //! \p letter_order.
    explicit SHORTLEX(std::function<bool(rws_letter_t const&,
                                         rws_letter_t const&)> letter_order)
        : RO([this](rws_word_t const& p, rws_word_t const& q) {
            // FIXME This is unsafe since we don't check that p and q consist of
            // the correct letters
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

  //!  This class is used to represent a
  //! [string rewriting system](https://en.wikipedia.org/wiki/Semi-Thue_system)
  //! defining a finitely presented monoid or semigroup.
  class RWS {
   public:
    //! This data member is used to indicate that no rule has been added by a
    //! call to RWS::add_rule, which returns the rule which was added.
    static rws_rule_t const NONE;

    //! Constructs rewriting system with no rules and the reduction ordering
    //! \p order.
    //!
    //! This constructs a rewriting system with no rules, and with the reduction
    //! ordering RO specifed by the parameter \p order.
    explicit RWS(RO* order)
        : _confluence_known(false),
          _is_confluent(),
          _nr_active_rules(0),
          _order(order),
          _report_next(0),
          _report_interval(1000),
          _rules(),
          _stack() {}

    //! Constructs a rewriting system with no rules, and the SHORTLEX
    //! reduction ordering.
    RWS() : RWS(new SHORTLEX()) {}

    //! Constructs a rewriting system with rules derived from the
    //! parameter \p relations, and with the SHORTLEX reduction ordering.
    explicit RWS(std::vector<relation_t> const& relations) : RWS() {
      add_rules(relations);
    }

    //! Constructs a rewriting system with rules \p rules,
    //! and with the SHORTLEX reduction ordering.
    explicit RWS(std::vector<rws_rule_t> const& rules) : RWS() {
      add_rules(rules);
    }

    //! Constructs a rewriting system with rules derived from \p relations,
    //! and with the reduction ordering specified by \p order.
    //!
    //! The RWS instance constructed here owns the parameter \p order, and
    //! deletes it in its destructor.
    RWS(RO* order, std::vector<relation_t> const& relations) : RWS(order) {
      add_rules(relations);
    }

    //! Constructs a rewriting system with rules derived from \p rules,
    //! and with the reduction ordering specified by \p order.
    //!
    //! The RWS instance constructed here owns the parameter \p order, and
    //! deletes it in its destructor.
    RWS(RO* order, std::vector<rws_rule_t> const& rules) : RWS(order) {
      add_rules(rules);
    }

    //! Constructs a rewriting system from Congruence::relations and
    //! Congruence::extra applied to \p cong.
    //!
    //! Constructs a rewriting system with rules corresponding to the
    //! relations used to define \p cong, i.e. Congruence::relations and
    //! Congruence::extra, and with the SHORTLEX reduction ordering.
    explicit RWS(Congruence& cong) : RWS() {
      add_rules(cong);
    }

    //! A default destructor
    //!
    //! This deletes the reduction order used to construct the object.
    ~RWS() {
      delete _order;
    }

    //! Returns \c true if the rewriting system is
    //! [confluent](https://en.wikipedia.org/wiki/Confluence_(abstract_rewriting))
    //! and \c false if it is not.
    bool is_confluent() const {
      std::atomic<bool> killed(false);
      return is_confluent(killed);
    }

    //! Returns the current number of active rules in the rewriting system.
    size_t nr_rules() const {
      return _nr_active_rules;
    }

    //! Rewrites the parameter \p w in-place according to the current rules in
    //! the rewriting system.
    void rewrite(rws_word_t& w) const;

    //! Rewrites the libsemigroups::rws_word_t pointed to by \p w
    //! in-place according to the current rules in the rewriting system.
    void rewrite(rws_word_t* w) const {
      rewrite(*w);
    }

    //! Rewrites the parameter \p w  according to the current rules in
    //! the rewriting system.
    rws_word_t rewrite(rws_word_t const& w) const {
      rws_word_t ww(w);
      rewrite(ww);
      return ww;
    }

    //! Run the [Knuth-Bendix
    //! algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    //! on the rewriting system.
    //!
    //! \warning This will terminate when the rewriting system is confluent,
    //! which might be never.
    void knuth_bendix() {
      std::atomic<bool> killed(false);
      knuth_bendix(killed);
    }

    //! Run the [Knuth-Bendix
    //! algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    //! on the rewriting system until it terminates or \p killed is set to
    //! \c true.
    //!
    //! \warning This will terminate when the rewriting system is confluent,
    //! which might be never.
    void knuth_bendix(std::atomic<bool>& killed);

    //! If you somehow magically know that the rewriting system is confluent,
    //! or not, then you can set this using this method.
    void set_confluent(bool val) {
      _is_confluent     = val;
      _confluence_known = true;
    }

    //! Add a rule to the rewriting system.
    //!
    //! The parameters \p p and \p q correspond to the rule being added.
    //!
    //! If \p p and \p q are not equal, then this method adds a rule to the
    //! rewriting system, where the left-hand side of the rule is strictly
    //! greater than the right-hand side in the reduction ordering used to
    //! defined \c this. If \p p and \p q are equal, then the rewriting system
    //! is unchanged.
    //!
    //! \return The rule added (either libsemigroups::rws_rule_t(\p p, \p q),
    //! libsemigroups::rws_rule_t(\p q, \p p) or RWS::NONE).
    rws_rule_t add_rule(rws_word_t const& p, rws_word_t const& q);

    //! Add a rule to the rewriting system.
    //!
    //! See RWS::add_rule.
    //!
    //! \return The actual rule added, which may not equal \p rule.
    rws_rule_t add_rule(rws_rule_t const& rule);

    //! Add rules to the rewriting system.
    //!
    //! See RWS::add_rule.
    void add_rules(std::vector<rws_rule_t> const& rules);

    //! Adds rules derived from \p relations to the rewriting system.
    //!
    //! See RWS::add_rule.
    void add_rules(std::vector<relation_t> const& relations);

    //! Add rules derived from Congruence::relations and Congruence::extra
    //! applied to \p cong.
    void add_rules(Congruence& cong) {
      add_rules(cong.relations());
      add_rules(cong.extra());
    }

    //! Helper function for converting a libsemigroups::letter_t to a
    //! libsemigroups::rws_letter_t.
    //!
    //! See also RWS::rws_letter_to_letter.
    static rws_letter_t letter_to_rws_letter(letter_t const& a);

    //! Helper function for converting a libsemigroups::letter_t to a
    //! libsemigroups::rws_word_t.
    static rws_word_t letter_to_rws_word(letter_t const& a);

    //! Helper function for converting a libsemigroups::word_t to a
    //! libsemigroups::rws_word_t.
    //!
    //! See also RWS::rws_word_to_word.
    static rws_word_t word_to_rws_word(word_t const& w);

    //! Helper function for converting a libsemigroups::rws_letter_t to a
    //! libsemigroups::letter_t.
    //!
    //! See also RWS::letter_to_rws_letter.
    static letter_t rws_letter_to_letter(rws_letter_t const& rws_letter);

    //! Helper function for converting a libsemigroups::rws_word_t to a
    //! libsemigroups::word_t.
    //!
    //! See also RWS::word_to_rws_word.
    static word_t* rws_word_to_word(rws_word_t const* rws_word);

    //! Turn reporting on or off.
    //!
    //! If \p val is true, then some methods for a RWS object may report
    //! information about the progress of the computation.
    void set_report(bool val) const {
      glob_reporter.set_report(val);
    }

    //! Remove deactivated rules.
    //!
    //! This method removes all deactivated rules from the rewriting system, and
    //! may reduce the amount of memory used.
    void compress();

    //! Returns \c true if the reduced form of \p p is less than
    //! the reduced form of \p q, and \c false if not.
    //!
    //! This is done with respect to the reduced ordering used to defined \c
    //! this.
    bool test_less_than(rws_word_t const& p, rws_word_t const& q) {
      assert(_order != nullptr);
      if (!is_confluent()) {
        knuth_bendix();
      }
      return (*_order)(rewrite(q), rewrite(p));
    }

   private:
    bool is_confluent(std::atomic<bool>& killed) const;

    void deactivate_rule(size_t i) {
      assert(i < _rules.size());
      _nr_active_rules--;
      _rules[i].second = false;
    }

    void clear_stack(std::atomic<bool>& killed);
    void overlap(size_t i, size_t j, std::atomic<bool>& killed);

    mutable bool _confluence_known;
    mutable bool _is_confluent;
    size_t       _nr_active_rules;
    RO*          _order;
    size_t       _report_next;
    size_t       _report_interval;
    std::vector<std::pair<rws_rule_t, bool>> _rules;
    std::stack<rws_rule_t> _stack;
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_RWS_H_
