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
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "cong.h"
#include "semigroups.h"

namespace libsemigroups {

  //! This class provides a call operator which can be used to compare
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
        std::function<bool(std::string const*, std::string const*)> func)
        : _func(func) {}

    //! Returns \c true if the word pointed to by \p p is greater than the word
    //! pointed to by \p q in the reduction ordering.
    size_t operator()(std::string const* p, std::string const* q) const {
      return _func(p, q);
    }

    //! Returns \c true if the word \p p is greater than the word
    //! \p q in the reduction ordering.
    size_t operator()(std::string const& p, std::string const& q) const {
      return _func(&p, &q);
    }

   private:
    std::function<bool(std::string const*, std::string const*)> _func;
  };

  //! This class implements the shortlex reduction ordering derived from
  //! an ordering on libsemigroups::rws_letter_t's.
  class SHORTLEX : public ReductionOrdering {
   public:
    //! Constructs a short-lex reduction ordering object derived from the
    //! order of on libsemigroups::rws_letter_t's given by the operator <.
    SHORTLEX()
        : ReductionOrdering([](std::string const* p, std::string const* q) {
            return (p->size() > q->size()
                    || (p->size() == q->size() && *p > *q));
          }) {}
  };

  // The ordering used here is recursive path ordering (based on
  // that described in the book "Confluent String Rewriting" by Matthias
  // Jantzen, Defn 1.2.14, page 24).
  //
  // The ordering is as follows:
  // let u, v be elements of X* u >= v iff one of the following conditions is
  // fulfilled;
  // 1) u = v
  // OR
  // u = u'a, v = v'b for some a,b elements of X, u',v' elements of X* and then:
  // 2) a = b and u' >= v'
  // OR
  // 3) a > b and u  > v'
  // OR
  // 4) b > a and u'> v
  //
  // 1 or 0 = false
  // 2      = true
  /*class RECURSIVE : public ReductionOrdering {
   public:
    RECURSIVE()
        : ReductionOrdering([](std::string const* Q, std::string const* P) {
            bool lastmoved = false;
            auto it_P      = P->crbegin();
            auto it_Q      = Q->crbegin();
            while (true) {
              if (it_P == P->crend()) {
                return (it_Q == Q->crend() ? lastmoved : true);
              } else if (it_Q == Q->crend()) {
                return false;
              }
              if (*it_P == *it_Q) {
                ++it_P;
                ++it_Q;
              } else if (*it_P < *it_Q) {
                ++it_P;
                lastmoved = false;
              } else {
                ++it_Q;
                lastmoved = true;
              }
            }
          }) {}
  };*/

  // TODO add more reduction orderings

  //! This class is used to represent a
  //! [string rewriting system](https://en.wikipedia.org/wiki/Semi-Thue_system)
  //! defining a finitely presented monoid or semigroup.

  class RWS {
    // Forward declarations
    struct Rule;
    class RuleLookup;
    struct OverlapMeasure;
    friend Rule;

   public:
    //! Type for letters for rewriting systems.
    typedef char rws_letter_t;

    //! Type for words for rewriting systems.
    typedef std::string rws_word_t;

    //! The values in this enum determine how a rewriting system measures the
    //! length \f$d(AB, BC)\f$ of the overlap of two words \f$AB\f$ and
    //! \f$BC\f$:
    //! * ***ABC***:        \f$d(AB, BC) = |A| + |B| + |C|\f$
    //! * ***AB_BC***:      \f$d(AB, BC) = |AB| + |BC|\f$
    //! * ***max_AB_BC***:  \f$d(AB, BC) = max(|AB|, |BC|)\f$
    //!
    //! \sa RWS::set_overlap_measure.
    enum overlap_measure { ABC = 0, AB_BC = 1, max_AB_BC = 2 };

    //! Constructs rewriting system with no rules and the reduction ordering
    //! \p order.
    //!
    //! This constructs a rewriting system with no rules, and with the
    //! reduction ordering ReductionOrdering specifed by the parameter \p
    //! order.
    explicit RWS(ReductionOrdering* order,
                 std::string        alphabet = STANDARD_ALPHABET)
        : _active_rules(),
          _alphabet(alphabet),
          _check_confluence_interval(4096),
          // _clear_stack_interval(0),
          _confluence_known(false),
          _inactive_rules(),
          _confluent(false),
          _max_overlap(UNBOUNDED),
          _max_rules(UNBOUNDED),
          _min_length_lhs_rule(std::numeric_limits<size_t>::max()),
          _order(order),
          _overlap_measure(nullptr),
          _report_next(0),
          _report_interval(1000),
          _stack(),
          _tmp_word1(new rws_word_t()),
          _tmp_word2(new rws_word_t()),
          _total_rules(0) {
      _next_rule_it1 = _active_rules.end();  // null
      _next_rule_it2 = _active_rules.end();  // null
      set_overlap_measure(overlap_measure::ABC);
      if (_alphabet != STANDARD_ALPHABET) {
        if (std::is_sorted(_alphabet.cbegin(), _alphabet.cend())) {
          _alphabet = STANDARD_ALPHABET;
        } else {
          for (size_t i = 0; i < _alphabet.size(); ++i) {
            _alphabet_map.emplace(
                std::make_pair(_alphabet[i], uint_to_rws_letter(i)));
          }
        }
      }
#ifdef LIBSEMIGROUPS_STATS
      _max_stack_depth        = 0;
      _max_word_length        = 0;
      _max_active_word_length = 0;
      _max_active_rules       = 0;
#endif
    }

    //! Constructs a rewriting system with no rules, and the SHORTLEX
    //! reduction ordering.
    explicit RWS(std::string alphabet = STANDARD_ALPHABET)
        : RWS(new SHORTLEX(), alphabet) {}

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

    //! This method allows a RWS object to be left shifted into a std::ostream,
    //! such as std::cout. The currently active rules of the system are
    //! represented in the output.
    friend std::ostream& operator<<(std::ostream& os, RWS const& rws);

    //! Returns \c true if the rewriting system is
    //! [confluent](https://en.wikipedia.org/wiki/Confluence_(abstract_rewriting))
    //! and \c false if it is not.
    bool confluent() const {
      std::atomic<bool> killed(false);
      return confluent(killed);
    }

    //! Returns the current number of active rules in the rewriting system.
    size_t nr_rules() const {  // FIXME should this be nrrules?
      return _active_rules.size();
    }

    //! Rewrites the word \p w in-place according to the current rules in the
    //! rewriting system.
    std::string* rewrite(std::string* w) const {
      string_to_rws_word(*w);
      internal_rewrite(w);
      rws_word_to_string(*w);
      return w;
    }

    //! Rewrites a copy of the word \p w according to the current rules in the
    //! rewriting system.
    std::string rewrite(std::string w) const {
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
    //!
    //! \sa knuth_bendix_by_overlap_length.
    void knuth_bendix(std::atomic<bool>& killed);

    //! This method runs the Knuth-Bendix algorithm on the rewriting system by
    //! considering all overlaps of a given length \f$n\f$ (according to the
    //! RWS::overlap_measure) before those overlaps of length \f$n + 1\f$.
    //!
    //! \warning This will terminate when the rewriting system is confluent,
    //! which might be never.
    //!
    //! \sa RWS::knuth_bendix.
    // TODO a version that also accepts killed.
    void knuth_bendix_by_overlap_length() {
      Timer t;
      t.start();
      size_t max_overlap               = _max_overlap;
      size_t check_confluence_interval = _check_confluence_interval;
      _max_overlap                     = 1;
      _check_confluence_interval       = UNBOUNDED;
      std::atomic<bool> killed(false);
      while (!confluent()) {
        knuth_bendix(killed);
        _max_overlap++;
      }
      _max_overlap               = max_overlap;
      _check_confluence_interval = check_confluence_interval;
      REPORT(t.string("elapsed time = "));
    }

    //! Add a rule to the rewriting system.
    //!
    //! The principal difference between this method and
    //! RWS::add_rule(std::string* p, std::string* q) is that the arguments are
    //! copied by this method.
    //!
    //! \sa RWS::add_rule.
    void add_rule(std::string const& p, std::string const& q);

    //! Adds rules derived from \p relations via RWS::word_to_rws_word to the
    //! rewriting system.
    //!
    //! \sa RWS::add_rule.
    void add_rules(std::vector<relation_t> const& relations);

    //! Add rules derived from Congruence::relations and Congruence::extra
    //! applied to \p cong.
    void add_rules(Congruence& cong) {
      add_rules(cong.relations());
      add_rules(cong.extra());
    }

    //! This method returns \c true if the strings \p p and \p q represent an
    //! active rule of the rewriting system, i.e. if either \f$ p \to q\f$ or
    //! \f$ q \to p \f$ is a currently active rule of the system.
    // TODO remove this
    bool rule(std::string p, std::string q) const;

    //! This method returns a vector consisting of the pairs of strings which
    //! represent the rules of the rewriting system. The \c first entry in every
    //! such pair is greater than the \c second according to the reduction
    //! ordering of the rewriting system. The rules are sorted
    //! according to the reduction ordering used by the rewriting system, on
    //! the first entry.
    std::vector<std::pair<std::string, std::string>> rules() const;

    //! Turn reporting on or off.
    //!
    //! If \p val is true, then some methods for a RWS object may report
    //! information about the progress of the computation.
    void set_report(bool val) const {
      glob_reporter.set_report(val);
    }

    //! Some information can be sent to std::cout during calls to
    //! RWS::knuth_bendix and RWS::knuth_bendix_by_overlap_length. This method
    //! can be used to determine the frequency with which this information is
    //! given, where a larger value means less frequently.
    //!
    //! The default value is 1000.
    void set_report_interval(size_t interval) {
      _report_interval = interval;
    }

    //! The method RWS::knuth_bendix periodically checks if the system is
    //! already confluent. This method can be used to set how frequently this
    //! happens, it is the number of new overlaps that should be considered
    //! before checking confluence. Setting this value too low can adversely
    //! affect the performance of RWS::knuth_bendix.
    //!
    //! The default value is 4096, and should be set to RWS::UNBOUNDED if
    //! RWS::knuth_bendix should never check if the system is already
    //! confluent.
    //!
    //! \sa RWS::knuth_bendix.
    void set_check_confluence_interval(size_t interval) {
      if (interval > UNBOUNDED) {
        interval = UNBOUNDED;
      }
      _check_confluence_interval = interval;
    }

    // This is temporarily disabled, and may be deleted altogether in the
    // future.
    // void set_clear_stack_interval(size_t interval) {
    //   _clear_stack_interval = interval;
    // }

    //! This method can be used to specify the maximum length of the overlap of
    //! two left hand sides of rules that should be considered in
    //! RWS::knuth_bendix.
    //!
    //! If this value is less than the longest left hand side of a rule, then
    //! RWS::knuth_bendix can terminate without the system being confluent.
    //!
    //! \sa RWS::knuth_bendix.
    void set_max_overlap(size_t val) {
      if (val > UNBOUNDED) {
        val = UNBOUNDED;
      }
      _max_overlap = val;
    }

    //! This method sets the (approximate) maximum number of rules that the
    //! system should contain. If this is number is exceeded in calls to
    //! RWS::knuth_bendix or RWS::knuth_bendix_by_overlap_length, then
    //! these methods will terminate and the system may not be confluent.
    //!
    //! \sa RWS::knuth_bendix and RWS::knuth_bendix.
    void set_max_rules(size_t val) {
      _max_rules = val;
    }

    //! This method can be used to determine the way that the length of an
    //! overlap of two words in the system is meaasured.
    //!
    //! \sa RWS::overlap_measure.
    void set_overlap_measure(overlap_measure measure);

    //! Returns \c true if the reduced form of \c RWS::word_to_rws_word(p) is
    //! less than the reduced form of \c RWS::word_to_rws_word(q), with respect
    //! to the reduction ordering of \c this, and \c false if not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_less_than(std::string const& p, std::string const& q)
    bool test_less_than(word_t const& p, word_t const& q);

    //! Returns \c true if \c RWS::rewrite(p) is
    //! less than \c RWS::rewrite(q), with respect
    //! to the reduction ordering of \c this, and \c false if not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_less_than(word_t const& p, word_t const& q)
    bool test_less_than(std::string const& p, std::string const& q);

    //! Returns \c true if \c RWS::rewrite(p) is
    //! less than \c RWS::rewrite(q), with respect
    //! to the reduction ordering of \c this, and \c false if not.
    //!
    //! The parameters \p p and \p q are rewritten in-place, and the caller is
    //! responsible for their deletion.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_less_than(word_t const& p, word_t const& q)
    bool test_less_than(std::string* p, std::string* q);

    //! Returns \c true if the reduced form of \c RWS::word_to_rws_word(p)
    //! equal the reduced form of \c RWS::word_to_rws_word(q), and \c false if
    //! not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_equals(std::string const& p, std::string const& q)
    bool test_equals(word_t const& p, word_t const& q);

    //! Returns \c true if the reduced form of \c RWS::word_to_rws_word(p)
    //! equal the reduced form of \c RWS::word_to_rws_word(q), and \c false if
    //! not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_equals(std::string const& p, std::string const& q)
    bool test_equals(std::initializer_list<size_t> const& p,
                     std::initializer_list<size_t> const& q);

    //! Returns \c true if \c RWS::rewrite(p) equals
    //! \c RWS::rewrite(q), and \c false if not.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_equals(word_t const& p, word_t const& q)
    bool test_equals(std::string const& p, std::string const& q);

    //! Returns \c true if \c RWS::rewrite(p) equals
    //! \c RWS::rewrite(q), and \c false if not.
    //!
    //! This method rewrites \p p and \p q in-place (unless they are already
    //! equal before rewriting). The caller responsible for deletion.
    //!
    //! \warning This method calls RWS::knuth_bendix and so it may never
    //! terminate.
    //!
    //! \sa RWS::test_equals(word_t const& p, word_t const& q)
    bool test_equals(std::string* p, std::string* q);

    //! The constant value represents an UNBOUNDED quantity.
    //!
    //! \sa RWS::set_check_confluence_interval, RWS::set_max_rules,
    //! RWS::set_max_overlap.
    static size_t const UNBOUNDED = static_cast<size_t>(-2);

    //! This method converts an unsigned integer to the corresponding
    //! rws_word_t. For example, the integer 0 is converted to the word with
    //! single letter which is the 1st ASCII character.
    static inline rws_word_t* uint_to_rws_word(size_t const& a) {
      return new rws_word_t(1, uint_to_rws_letter(a));
    }

    //! This method converts a string in the rewriting system into a vector of
    //! unsigned integers. This method is the inverse of RWS::uint_to_rws_word.
    static word_t* rws_word_to_word(rws_word_t const* rws_word) {
      word_t* w = new word_t();
      w->reserve(rws_word->size());
      for (rws_letter_t const& rws_letter : *rws_word) {
        w->push_back(rws_letter_to_uint(rws_letter));
      }
      return w;
    }

    //! This method converts a vector of unsigned integers to a string which
    //! represents a word in the rewriting system. The second parameter \p ww
    //! is modified in-place to contain the string version of the vector of
    //! unsigned integers \p w.
    //!
    //! This method returns its second parameter \p ww.
    static rws_word_t* word_to_rws_word(word_t const& w, rws_word_t* ww) {
      ww->clear();
      for (size_t const& a : w) {
        (*ww) += uint_to_rws_letter(a);
      }
      return ww;
    }

    //! This method converts a vector of unsigned integers to a string which
    //! represents a word in the rewriting system.
    //!
    //! This method returns a pointer to a new string, and it is the
    //! responsibility of the caller to delete it.
    static inline rws_word_t* word_to_rws_word(word_t const& w) {
      rws_word_t* ww = new rws_word_t();
      return word_to_rws_word(w, ww);
    }

   private:
    static inline size_t rws_letter_to_uint(rws_letter_t const& rws_letter) {
#ifdef LIBSEMIGROUPS_DEBUG
      return static_cast<size_t>(rws_letter - 97);
#else
      return static_cast<size_t>(rws_letter - 1);
#endif
    }

    static inline rws_letter_t uint_to_rws_letter(size_t const& a) {
#ifdef LIBSEMIGROUPS_DEBUG
      return static_cast<rws_letter_t>(a + 97);
#else
      return static_cast<rws_letter_t>(a + 1);
#endif
    }

    inline rws_letter_t char_to_rws_letter(char a) const {
      LIBSEMIGROUPS_ASSERT(_alphabet != STANDARD_ALPHABET);
      LIBSEMIGROUPS_ASSERT(_alphabet_map.find(a) != _alphabet_map.end());
      return (*_alphabet_map.find(a)).second;
    }

    inline char rws_letter_to_char(rws_letter_t a) const {
      LIBSEMIGROUPS_ASSERT(_alphabet != STANDARD_ALPHABET);
      LIBSEMIGROUPS_ASSERT(rws_letter_to_uint(a) < _alphabet.size());
      return _alphabet[rws_letter_to_uint(a)];
    }

    inline void string_to_rws_word(std::string& w) const {
      if (_alphabet == STANDARD_ALPHABET) {
        return;
      }
      for (auto& a : w) {
        a = char_to_rws_letter(a);
      }
    }

    inline void rws_word_to_string(rws_word_t& w) const {
      if (_alphabet == STANDARD_ALPHABET) {
        return;
      }
      for (auto& a : w) {
        a = rws_letter_to_char(a);
      }
    }

    static std::string const STANDARD_ALPHABET;

    void add_rule(Rule* rule);
    std::list<Rule const*>::iterator
    remove_rule(std::list<Rule const*>::iterator it);

    Rule* new_rule() const;
    Rule* new_rule(rws_word_t* lhs, rws_word_t* rhs) const;
    Rule* new_rule(Rule const* rule) const;
    Rule* new_rule(rws_word_t::const_iterator begin_lhs,
                   rws_word_t::const_iterator end_lhs,
                   rws_word_t::const_iterator begin_rhs,
                   rws_word_t::const_iterator end_rhs) const;

    // Rewrites the word pointed to by \p w in-place according to the current
    // rules in the rewriting system.
    void internal_rewrite(rws_word_t* w) const;

    bool confluent(std::atomic<bool>& killed) const;
    void clear_stack(std::atomic<bool>& killed);
    void push_stack(Rule* rule);
    void push_stack(Rule* rule, std::atomic<bool>& killed);
    void overlap(Rule const* u, Rule const* v, std::atomic<bool>& killed);
    std::list<Rule const*> _active_rules;
    std::string            _alphabet;
    std::unordered_map<char, rws_letter_t> _alphabet_map;
    size_t _check_confluence_interval;
    // size_t                           _clear_stack_interval;
    mutable std::atomic<bool>        _confluence_known;
    mutable std::list<Rule*>         _inactive_rules;
    mutable std::atomic<bool>        _confluent;
    size_t                           _max_overlap;
    size_t                           _max_rules;
    size_t                           _min_length_lhs_rule;
    std::list<Rule const*>::iterator _next_rule_it1;
    std::list<Rule const*>::iterator _next_rule_it2;
    ReductionOrdering const*         _order;
    OverlapMeasure*                  _overlap_measure;
    size_t                           _report_next;
    size_t                           _report_interval;
    std::set<RuleLookup>             _set_rules;
    std::stack<Rule*>                _stack;
    rws_word_t*                      _tmp_word1;
    rws_word_t*                      _tmp_word2;
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

  // Class for rules in rewriting systems, which supports only two methods,
  // Rule::lhs and Rule::rhs, which return the left and right hand sides of
  // the rule.
  struct RWS::Rule {
    friend std::ostream& operator<<(std::ostream& os, Rule const& rule) {
      os << rule.rws_word_to_string(rule.lhs()) << " -> "
         << rule.rws_word_to_string(rule.rhs());
      return os;
    }

    std::string rws_word_to_string(rws_word_t const* word) const {
      std::string str(*word);
      _rws->rws_word_to_string(str);
      return str;
    }

    // Returns the left hand side of the rule, which is guaranteed to be
    // greater than its right hand side according to the reduction ordering of
    // the RWS used to construct this.
    rws_word_t const* lhs() const {
      return const_cast<rws_word_t const*>(_lhs);
    }

    // Returns the right hand side of the rule, which is guaranteed to be
    // less than its left hand side according to the reduction ordering of
    // the RWS used to construct this.
    rws_word_t const* rhs() const {
      return const_cast<rws_word_t const*>(_rhs);
    }

    // The Rule class does not support an assignment contructor to avoid
    // accidental copying.
    Rule& operator=(Rule const& copy) = delete;

    // The Rule class does not support a copy contructor to avoid
    // accidental copying.
    Rule(Rule const& copy) = delete;

    // Construct from RWS with new but empty rws_word_t's
    explicit Rule(RWS const* rws, int64_t id)
        : _rws(rws),
          _lhs(new rws_word_t()),
          _rhs(new rws_word_t()),
          _id(-1 * id) {
      LIBSEMIGROUPS_ASSERT(_id < 0);
    }

    // Destructor, deletes pointers used to create the rule.
    ~Rule() {
      delete _lhs;
      delete _rhs;
    }

    void rewrite() {
      LIBSEMIGROUPS_ASSERT(_id != 0);
      _rws->internal_rewrite(_lhs);
      _rws->internal_rewrite(_rhs);
      reorder();
    }

    void rewrite_rhs() {
      LIBSEMIGROUPS_ASSERT(_id != 0);
      _rws->internal_rewrite(_rhs);
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

    void reorder() {
      if ((*(_rws->_order))(_rhs, _lhs)) {
        std::swap(_lhs, _rhs);
      }
    }

    RWS const*  _rws;
    rws_word_t* _lhs;
    rws_word_t* _rhs;
    int64_t     _id;
  };

  // Simple class wrapping a two iterators to an rws_word_t and a Rule const*
  class RWS::RuleLookup {
   public:
    RuleLookup() : _rule(nullptr) {}

    explicit RuleLookup(RWS::Rule* rule)
        : _first(rule->lhs()->cbegin()),
          _last(rule->lhs()->cend()),
          _rule(rule) {}

    RuleLookup& operator()(rws_word_t::iterator const& first,
                           rws_word_t::iterator const& last) {
      _first = first;
      _last  = last;
      return *this;
    }

    Rule const* rule() const {
      return _rule;
    }

    // This implements reverse lex comparison of this and that, which satisfies
    // the requirement of std::set that equivalent items be incomparable, so,
    // for example bcbc and abcbc are considered equivalent, but abcba and bcbc
    // are not.
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
    rws_word_t::const_iterator _first;
    rws_word_t::const_iterator _last;
    Rule const*                _rule;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_RWS_H_
