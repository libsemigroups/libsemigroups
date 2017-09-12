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

#ifndef LIBSEMIGROUPS_SRC_RWS_H_
#define LIBSEMIGROUPS_SRC_RWS_H_

#include <atomic>
#include <list>
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

  //!  This class provides a call operator which can be used to compare
  //! libsemigroups::rws_word_t.
  //!
  //! A *reduction ordering* is a linear order \f$\prec\f$ with no infinite
  //! descending chains and where \f$u \prec v\f$ for some
  //! libsemigroups::rws_word_t \f$u\f$ and \f$v\f$ implies that
  //! \f$ aub \prec avb\f$ for all  libsemigroups::rws_word_t \f$a\f$ and
  //! \f$b\f$.
  class ReductionOrdering {
   public:
    //! A constructor.
    //!
    //! This constructs a reduction ordering object whose call operator uses
    //! the function \p func to compare libsemigroups::rws_word_t's. It is the
    //! responsibility of the caller to verify that \p func specifies a
    //! reduction ordering.
    explicit ReductionOrdering(
        std::function<bool(rws_word_t const*, rws_word_t const*)> func)
        : _func(func) {}

    //! Returns \c true if the word pointed to by \p p is greater than the word
    //! pointed to by \p q in the reduction ordering.
    size_t operator()(rws_word_t const* p, rws_word_t const* q) const {
      return _func(p, q);
    }

    //! Returns \c true if the word \p p is greater than the word
    //! \p q in the reduction ordering.
    size_t operator()(rws_word_t const& p, rws_word_t const& q) const {
      return _func(&p, &q);
    }

   private:
    std::function<bool(rws_word_t const*, rws_word_t const*)> _func;
  };

  //! This class implements the shortlex reduction ordering derived from
  //! an ordering on libsemigroups::rws_letter_t's.
  class SHORTLEX : public ReductionOrdering {
   public:
    //! Constructs a short-lex reduction ordering object derived from the
    //! order of on libsemigroups::rws_letter_t's given by the operator <.
    SHORTLEX()
        : ReductionOrdering([](rws_word_t const* p, rws_word_t const* q) {
            return (p->size() > q->size()
                    || (p->size() == q->size() && *p > *q));
          }) {}

    //! A constructor.
    //!
    //! This constructs a short-lex reduction ordering object derived from the
    //! order on libsemigroups::rws_letter_t's given by the parameter
    //! \p letter_order.
    explicit SHORTLEX(std::function<bool(rws_letter_t const&,
                                         rws_letter_t const&)> letter_order)
        : ReductionOrdering([this](rws_word_t const* p, rws_word_t const* q) {
            // FIXME This is unsafe since we don't check that p and q consist of
            // the correct letters
            if (p->size() > q->size()) {
              return true;
            } else if (p->size() < q->size()) {
              return false;
            }
            auto itp = p->cbegin();
            auto itq = q->cbegin();
            while (itp < p->cend() && *itp == *itq) {
              itp++;
              itq++;
            }
            return (itp != p->cend() && this->_letter_order(*itp, *itq));
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
    // Forward declaration of Rule
    class Rule;

    //! Constructs rewriting system with no rules and the reduction ordering
    //! \p order.
    //!
    //! This constructs a rewriting system with no rules, and with the reduction
    //! ordering ReductionOrdering specifed by the parameter \p order.
    explicit RWS(ReductionOrdering* order)
        : _active_rules(),
          _confluence_known(false),
          _inactive_rules(),
          _is_confluent(),
          _order(order),
          _report_next(0),
          _report_interval(1000),
          _stack(),
          _total_rules(0) {
      _next_rule_it1 = _active_rules.end();  // null
      _next_rule_it2 = _active_rules.end();  // null
#ifdef LIBSEMIGROUPS_STATS
      _max_stack_depth        = 0;
      _max_word_length        = 0;
      _max_active_word_length = 0;
      _max_active_rules       = 0;
#endif
    }

    //! Constructs a rewriting system with no rules, and the SHORTLEX
    //! reduction ordering.
    RWS() : RWS(new SHORTLEX()) {}

    //! Constructs a rewriting system with rules derived from the
    //! parameter \p relations, and with the SHORTLEX reduction ordering.
    explicit RWS(std::vector<relation_t> const& relations) : RWS() {
      add_rules(relations);
    }

    //! Constructs a rewriting system with rules derived from \p relations,
    //! and with the reduction ordering specified by \p order.
    //!
    //! The RWS instance constructed here owns the parameter \p order, and
    //! deletes it in its destructor.
    RWS(ReductionOrdering* order, std::vector<relation_t> const& relations)
        : RWS(order) {
      add_rules(relations);
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
    //! This deletes the reduction order used to construct the object, and the
    //! rules in the system.
    ~RWS();

    //! Returns \c true if the rewriting system is
    //! [confluent](https://en.wikipedia.org/wiki/Confluence_(abstract_rewriting))
    //! and \c false if it is not.
    bool is_confluent() const {
      std::atomic<bool> killed(false);
      return is_confluent(killed);
    }

    //! Returns the current number of active rules in the rewriting system.
    size_t nr_rules() const {
      return _active_rules.size();
    }

    //! Rewrites the word pointed to by \p w in-place according to the current
    //! rules in the rewriting system.
    void rewrite(rws_word_t* w) const;

    //! Rewrites a copy of the word \p w according to the current rules in the
    //! rewriting system.
    rws_word_t rewrite(rws_word_t w) const {
      rewrite(&w);
      return w;
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
    //! is unchanged and \p p and \p q are deleted.
    //!
    //! The parameters \p p and \p q are not copied and will be deleted by the
    //! \c this.
    void add_rule(rws_word_t* p, rws_word_t* q);

    //! Add a rule to the rewriting system.
    //!
    //! The principal difference between this method and
    //! RWS::add_rule(rws_word_t* p, rws_word_t* q) is that the arguments are
    //! copied by this method.
    //!
    //! \sa RWS::add_rule.
    void add_rule(rws_word_t const& p, rws_word_t const& q);

    //! Adds rules derived from \p relations via RWS::word_to_rws_word to the
    //! rewriting system.
    //!
    //! \sa RWS::add_rule.
    void add_rules(std::vector<relation_t> const& relations) {
      for (relation_t const& rel : relations) {
        if (rel.first != rel.second) {
          add_rule(word_to_rws_word(rel.first), word_to_rws_word(rel.second));
        }
      }
    }

    //! Add rules derived from Congruence::relations and Congruence::extra
    //! applied to \p cong.
    void add_rules(Congruence& cong) {
      add_rules(cong.relations());
      add_rules(cong.extra());
    }

    //! Helper function for converting a libsemigroups::letter_t to a
    //! libsemigroups::rws_letter_t.
    //!
    //! \sa RWS::rws_letter_to_letter.
    static rws_letter_t letter_to_rws_letter(letter_t const& a);

    //! Helper function for converting a libsemigroups::letter_t to a
    //! libsemigroups::rws_word_t.
    static rws_word_t* letter_to_rws_word(letter_t const& a);

    //! Helper function for converting a libsemigroups::word_t to a
    //! libsemigroups::rws_word_t.
    //!
    //! \sa RWS::rws_word_to_word.
    static rws_word_t* word_to_rws_word(word_t const& w);

    //! Helper function for converting a libsemigroups::rws_letter_t to a
    //! libsemigroups::letter_t.
    //!
    //! \sa RWS::letter_to_rws_letter.
    static letter_t rws_letter_to_letter(rws_letter_t const& rws_letter);

    //! Helper function for converting a libsemigroups::rws_word_t to a
    //! libsemigroups::word_t.
    //!
    //! \sa RWS::word_to_rws_word.
    static word_t* rws_word_to_word(rws_word_t const* rws_word);

    //! Turn reporting on or off.
    //!
    //! If \p val is true, then some methods for a RWS object may report
    //! information about the progress of the computation.
    void set_report(bool val) const {
      glob_reporter.set_report(val);
    }

    //! Returns \c true if the reduced form of \c RWS::word_to_rws_word(p) is
    //! less than the reduced form of \c RWS::word_to_rws_word(q), with respect
    //! to the reduction ordering of \c this, and \c false if not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_less_than(rws_word_t const& p, rws_word_t const& q)
    bool test_less_than(word_t const& p, word_t const& q);

    //! Returns \c true if \c RWS::rewrite(p) is
    //! less than \c RWS::rewrite(q), with respect
    //! to the reduction ordering of \c this, and \c false if not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_less_than(word_t const& p, word_t const& q)
    bool test_less_than(rws_word_t const& p, rws_word_t const& q);

    //! Returns \c true if the reduced form of \c RWS::word_to_rws_word(p)
    //! equal the reduced form of \c RWS::word_to_rws_word(q), and \c false if
    //! not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_equals(rws_word_t const& p, rws_word_t const& q)
    bool test_equals(word_t const& p, word_t const& q);

    //! Returns \c true if \c RWS::rewrite(p) equals
    //! \c RWS::rewrite(q), and \c false if not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_equals(word_t const& p, word_t const& q)
    bool test_equals(rws_word_t const& p, rws_word_t const& q);

    //! Returns an iterator pointing at the first Rule of \c this.
    std::list<Rule const*>::const_iterator rules_cbegin() const {
      return _active_rules.cbegin();
    }

    //! Returns an iterator pointing past the last Rule of \c this.
    std::list<Rule const*>::const_iterator rules_cend() const {
      return _active_rules.cend();
    }

   private:
    // internal only, rewrites in-place and deletes p and q.
    bool test(rws_word_t* p,
              rws_word_t* q,
              std::function<bool(rws_word_t* p, rws_word_t* q)> func) {
      knuth_bendix();
      rewrite(p);
      rewrite(q);
      bool out = func(q, p);
      delete p;
      delete q;
      return out;
    }

    void add_rule(Rule* rule);
    std::list<Rule const*>::iterator
    remove_rule(std::list<Rule const*>::iterator it);

    Rule* new_rule() const;
    Rule* new_rule(rws_word_t const* lhs, rws_word_t const* rhs) const;
    Rule* new_rule(Rule const* rule) const;
    Rule* new_rule(rws_word_t::const_iterator begin_lhs,
                   rws_word_t::const_iterator end_lhs,
                   rws_word_t::const_iterator begin_rhs,
                   rws_word_t::const_iterator end_rhs) const;

    bool is_confluent(std::atomic<bool>& killed) const;
    void clear_stack(std::atomic<bool>& killed);
    void overlap(Rule const* u, Rule const* v, std::atomic<bool>& killed);

    std::list<Rule const*>           _active_rules;
    mutable bool                     _confluence_known;
    mutable std::list<Rule*>         _inactive_rules;
    mutable bool                     _is_confluent;
    std::list<Rule const*>::iterator _next_rule_it1;
    std::list<Rule const*>::iterator _next_rule_it2;
    ReductionOrdering const*         _order;
    size_t                           _report_next;
    size_t                           _report_interval;
    std::stack<Rule*>                _stack;
    mutable size_t                   _total_rules;

#ifdef LIBSEMIGROUPS_STATS
    size_t                         max_active_word_length();
    size_t                         _max_stack_depth;
    size_t                         _max_word_length;
    size_t                         _max_active_word_length;
    size_t                         _max_active_rules;
    std::unordered_set<rws_word_t> _unique_lhs_rules;
#endif
  };

  //! Class for rules in rewriting systems, which supports only two methods,
  //! Rule::lhs and Rule::rhs, which return the left and right hand sides of the
  //! rule.
  class RWS::Rule {
    friend RWS;

   public:
    //! Returns the left hand side of the rule, which is guaranteed to be
    //! greater than its right hand side according to the reduction ordering of
    //! the RWS used to construct this.
    rws_word_t const* lhs() const {
      return const_cast<rws_word_t const*>(_lhs);
    }

    //! Returns the right hand side of the rule, which is guaranteed to be
    //! less than its left hand side according to the reduction ordering of
    //! the RWS used to construct this.
    rws_word_t const* rhs() const {
      return const_cast<rws_word_t const*>(_rhs);
    }

   private:
    // The Rule class does not support an assignment contructor to avoid
    // accidental copying.
    Rule& operator=(Rule const& copy) = delete;

    // The Rule class does not support a copy contructor to avoid
    // accidental copying.
    Rule(Rule const& copy) = delete;

    // Construct from RWS, and pointers to libsemigroups::rws_word_t.
    //
    // A rule is guaranteed to have its left hand side greater than its right
    // hand side according to the reduction ordering of \p rws. The Rule
    // object constructed here takes ownership of \p p and \p q and deletes
    // them in its destructor.
    Rule(RWS const* rws, rws_word_t* p, rws_word_t* q)
        : _rws(rws), _lhs(p), _rhs(q), _active(false) {
      LIBSEMIGROUPS_ASSERT(*_lhs != *_rhs);
      reorder();
    }

    // Construct from RWS with new but empty rws_word_t's
    explicit Rule(RWS const* rws)
        : _rws(rws),
          _lhs(new rws_word_t()),
          _rhs(new rws_word_t()),
          _active(false) {}

    // Destructor, deletes pointers used to create the rule.
    ~Rule() {
      delete _lhs;
      delete _rhs;
    }

    void rewrite() {
      _rws->rewrite(_lhs);
      _rws->rewrite(_rhs);
      reorder();
    }

    void rewrite_rhs() {
      _rws->rewrite(_rhs);
    }

    void clear() {
      _lhs->clear();
      _rhs->clear();
    }

    inline bool is_active() const {
      return _active;
    }

    void deactivate() {
      _active = false;
    }

    void activate() {
      _active = true;
    }

    void reorder() {
      if ((*(_rws->_order))(_rhs, _lhs)) {
        std::swap(_lhs, _rhs);
      }
    }

    RWS const*  _rws;
    rws_word_t* _lhs;
    rws_word_t* _rhs;
    bool        _active;
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_RWS_H_
