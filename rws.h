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
  //
  // Type for letters for rewriting systems.
  typedef char rws_letter_t;

  //
  // Type for words for rewriting systems.
  typedef std::string rws_word_t;

  //
  // Type for rules in rewriting systems.
  typedef std::pair<rws_word_t, rws_word_t> rws_rule_t;

  // Reduction ordering
  // This class provides a call operator which can be used to compare
  // <rws_word_t>. A *reduction ordering* is a TODO
  class RO {
   public:
    // 1 parameter (function)
    // @func a binary function accepting <rws_word_t>s
    //
    // This constructs a reduction ordering object whose call operator uses the
    // function <func> to compare <rws_word_t>s. It is the responsibility of
    // the caller to verify that <func> specifies a reduction ordering.
    explicit RO(std::function<bool(rws_word_t const&, rws_word_t const&)> func)
        : _func(func) {}

    // const
    // @p an rws_word_t
    // @q an rws_word_t
    //
    // @return **true** if the word <p> is greater than the word <q> in the
    // reduction ordering.
    size_t operator()(rws_word_t const& p, rws_word_t const& q) const {
      return _func(p, q);
    }

   private:
    std::function<bool(rws_word_t const&, rws_word_t const&)> _func;
  };

  // Short-lex reduction ordering
  // This is a subclass of <RO> and implements the shortlex reduction ordering
  // derived from an ordering on letters.
  class SHORTLEX : public RO {
   public:
    // 0 parameter
    //
    // This constructs a short-lex reduction ordering object derived from the
    // order of on <rws_letter_t>s given by the operator &gt;.
    SHORTLEX()
        : RO([](rws_word_t const& p, rws_word_t const& q) {
            return (p.size() > q.size() || (p.size() == q.size() && p > q));
          }) {}

    // 1 parameter (function)
    // @letter_order a binary function taking <rws_letter_t>s defining a total
    // order on a set of letters.
    //
    // This constructs a short-lex reduction ordering object derived from the
    // order on <rws_letter_t>s given by <letter_order>.
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

  // Rewriting system
  //
  // This class is used to represent a
  // [string rewriting system](https://en.wikipedia.org/wiki/Semi-Thue_system)
  // defining a finitely presented monoid or semigroup.
  class RWS {
   public:
    // static
    //
    // This variable is used to indicate that no rule has been added by a call
    // to <add_rule>, which returns the rule which was added.
    static rws_rule_t const NONE;

    // 1 param (reduction ordering)
    // @order a pointer to a reduction ordering
    //
    // This constructs a rewriting system with no rules, and with the reduction
    // ordering <RO> specifed by <order>.
    explicit RWS(RO* order)
        : _confluence_known(false),
          _is_confluent(),
          _nr_active_rules(0),
          _order(order),
          _report_next(0),
          _report_interval(1000),
          _rules(),
          _stack() {}

    // 0 params
    //
    // This constructs a rewriting system with no rules, and the <SHORTLEX>
    // order.
    RWS() : RWS(new SHORTLEX()) {}

    // 1 param (relations)
    // @relations const reference to a vector of <relation_t>s.
    //
    // This constructs a rewriting system with rules derived from <relations>,
    // and with the reduction ordering equal to <SHORTLEX>.
    explicit RWS(std::vector<relation_t> const& relations) : RWS() {
      add_rules(relations);
    }

    // 1 param (rules)
    // @rules const reference to a vector of <rws_word_t>s.
    //
    // This constructs a rewriting system with rules derived from <rules>,
    // and with the reduction ordering equal to <SHORTLEX>.
    explicit RWS(std::vector<rws_rule_t> const& rules) : RWS() {
      add_rules(rules);
    }

    // 2 params (reduction ordering, relations)
    // @order a pointer to a reduction ordering
    // @relations const reference to a vector of <relation_t>s.
    //
    // This constructs a rewriting system with rules derived from <relations>,
    // and with the reduction ordering equal specified by <order>.
    RWS(RO* order, std::vector<relation_t> const& relations) : RWS(order) {
      add_rules(relations);
    }

    // 2 params (reduction ordering, rules)
    // @order a pointer to a reduction ordering
    // @rules const reference to a vector of <rws_word_t>s.
    //
    // This constructs a rewriting system with rules derived from <rules>,
    // and with the reduction ordering equal specified by <order>.
    RWS(RO* order, std::vector<rws_rule_t> const& rules) : RWS(order) {
      add_rules(rules);
    }

    // 1 param (congruence)
    // @cong reference to a <Congruence>.
    //
    // This constructs a rewriting system with rules corresponding to the
    // relations used to define <cong>, i.e. <Congruence::relations> and
    // <Congruence::extra>, and with the reduction ordering equal to
    // <SHORTLEX>.
    explicit RWS(Congruence& cong) : RWS() {
      add_rules(cong);
    }

    // A default destructor
    //
    // This deletes the reduction order used to construct the object.
    ~RWS() {
      delete _order;
    }

    bool is_confluent(std::atomic<bool>& killed) const;

    // const
    //
    // A rewriting system is *confluent* ... TODO
    // @return **true** if the rewriting system is
    // [confluent](https://en.wikipedia.org/wiki/Confluence_(abstract_rewriting))
    // and **false** if it is not.
    bool is_confluent() const {
      std::atomic<bool> killed(false);
      return is_confluent(killed);
    }

    // const
    //
    // This method returns the total number of active rules in the rewriting
    // system.
    //
    // @return the total number of active rules in the rewriting system.
    size_t nr_rules() const {
      return _nr_active_rules;
    }

    // const
    // @w   a <rws_word_t> to rewrite
    // @buf a buffer <rws_word_t> for use in the rewriting
    //
    // This method rewrites the first parameter <w> in-place according to the
    // current rules in the rewriting system.
    void rewrite(rws_word_t& w, rws_word_t& buf) const;

    // const
    // @w   a pointer to a <rws_word_t> to rewrite
    // @buf a buffer <rws_word_t> for use in the rewriting
    //
    // This method rewrites the <rws_word_t> pointed to by <w> in-place
    // according to the current rules in the rewriting system.
    void rewrite(rws_word_t* w, rws_word_t& buf) const {
      rewrite(*w, buf);
    }

    // const
    // @w   a <rws_word_t> to rewrite
    //
    // This method rewrites the <rws_word_t> by <w> in-place.
    //
    // @return the rewritten version of <w>.
    rws_word_t rewrite(rws_word_t w) const {
      rws_word_t buf;
      rewrite(w, buf);
      return w;
    }

    // non-const
    //
    // Run the [Knuth-Bendix
    // algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    // on the rewriting system.
    //
    // This will terminate when the rewriting system is confluent, which might
    // be never.
    void knuth_bendix() {
      std::atomic<bool> killed(false);
      knuth_bendix(killed);
    }

    // non-const
    // @killed an atomic boolean
    //
    // Run the [Knuth-Bendix
    // algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    // on the rewriting system until it terminates or <killed> is set to
    // **true** by another thread.
    //
    // This will terminate when the rewriting system is confluent, which might
    // be never.
    void knuth_bendix(std::atomic<bool>& killed);

    // non-const
    // @val a boolean value
    //
    // If you somehow magically know that the rewriting system is confluent,
    // then you can set this using this method.
    void set_confluent(bool val) {
      _is_confluent     = val;
      _confluence_known = true;
    }

    // non-const
    // @p the one side of the rule being added
    // @q the one side of the rule being added
    //
    // This method adds a rule to the rewriting system, where the left-hand
    // side of the rule is strictly greater than the right-hand side in the
    // reduction ordering used to defined **this**. If <p> and <q> are equal,
    // then the rewriting system is unchanged, and <NONE> is returned.
    //
    // @return the actual rule added (either <p> -> <q>, or <q> -> <p> or
    // <NONE>).
    rws_rule_t add_rule(rws_word_t const& p, rws_word_t const& q);

    // non-const
    // @rule an <rws_rule_t>
    //
    // See <add_rule>.
    //
    // @return the actual rule added, which may not equel <rule>.
    rws_rule_t add_rule(rws_rule_t const& rule);

    // non-const
    // @rules vector of <rws_rule_t>s to add to the rewriting system
    //
    // Adds rules derived from <rules> to the rewriting system;
    // see <add_rule>.

    void add_rules(std::vector<rws_rule_t> const& rules);

    // non-const
    // @relations vector of <relation_t>s
    //
    // Adds rules derived from <relations> to the rewriting system;
    // see <add_rule>.
    void add_rules(std::vector<relation_t> const& relations);

    // non-const
    // @cong a Congruence object
    //
    // Adds all the defining relations of the Congruence <cong>, i.e.
    // <Congruence::relations> and <Congruence::extra>.
    void add_rules(Congruence& cong) {
      add_rules(cong.relations());
      add_rules(cong.extra());
    }

    // static
    // @a a <letter_t>
    //
    // This is a helper function for converting a <letter_t> to
    // a <rws_letter_t>.
    //
    // @return an <rws_letter_t> corresponding to the <letter_t> <a>.
    static rws_letter_t letter_to_rws_letter(letter_t const& a);

    // static
    // @a a letter
    //
    // This is a helper function for converting a <letter_t> to
    // <rws_word_t>.
    //
    // @return an <rws_word_t> corresponding to the <letter_t> <a>.
    static rws_word_t letter_to_rws_word(letter_t const& a);

    // static
    // @w a word
    //
    // This is a helper function for converting a <word_t> to
    // <rws_word_t>.
    //
    // @return an <rws_word_t> corresponding to the <word_t> <w>.
    static rws_word_t word_to_rws_word(word_t const& w);

    // static
    // @rws_letter a letter
    //
    // This is a helper function for converting a <rws_letter_t> to
    // a <letter_t>.
    //
    // @return an <letter_t> corresponding to the <rws_letter_t> <rws_letter>.
    static letter_t rws_letter_to_letter(rws_letter_t const& rws_letter);

    // static
    // @rws_word a word
    //
    // This is a helper function for converting a <rws_word_t> to
    // a <word_t> pointer.
    //
    // @return an <word_t> pointer corresponding to the <rws_word_t>
    // <rws_word>.
    static word_t* rws_word_to_word(rws_word_t const* rws_word);

    // const
    // @val a boolean value
    //
    // If @val is true, then some methods for a RWS object may report
    // information about the progress of the computation.
    void set_report(bool val) const {
      glob_reporter.set_report(val);
    }

    // non-const
    //
    // This method removes all deactivated rules from the rewriting system, and
    // may reduce the amount of memory used.
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
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_RWS_H_
