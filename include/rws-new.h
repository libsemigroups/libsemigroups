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

#include <algorithm>
#include <atomic>
#include <list>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "fpsemi-intf.h"
#include "libsemigroups-config.h"
#include "libsemigroups-debug.h"
#include "race.h"
#include "report.h"
#include "rws-order.h"
#include "semigroups-base.h"
#include "timer.h"

// TODO: - move implementation to .cc file
//       - don't use the typedef external_string_t in public methods, use
//       std::string instead.

namespace libsemigroups {

  namespace fpsemigroup {
    //! This class is used to represent a
    //! [string rewriting
    //! system](https://en.wikipedia.org/wiki/Semi-Thue_system)
    //! defining a finitely presented monoid or semigroup.

    class RWS : public Interface {
      // Forward declarations
      class RuleLookup;       // defined in rws.h
      struct OverlapMeasure;  // defined in rws.cc
      struct Rule;
      friend Rule;        // defined in rws.h
      friend class RWSE;  // defined in rwse.h

      typedef char internal_char_t;
      typedef char external_char_t;

      typedef std::string internal_string_t;
      typedef std::string external_string_t;

     public:
      //////////////////////////////////////////////////////////////////////////
      // Static data members and enums
      //////////////////////////////////////////////////////////////////////////

      //! The values in this enum determine how a rewriting system measures the
      //! length \f$d(AB, BC)\f$ of the overlap of two words \f$AB\f$ and
      //! \f$BC\f$:
      //! * ***ABC***:        \f$d(AB, BC) = |A| + |B| + |C|\f$
      //! * ***AB_BC***:      \f$d(AB, BC) = |AB| + |BC|\f$
      //! * ***max_AB_BC***:  \f$d(AB, BC) = max(|AB|, |BC|)\f$
      //!
      //! \sa RWS::set_overlap_policy.
      enum overlap_policy { ABC = 0, AB_BC = 1, max_AB_BC = 2 };

      static external_string_t const STANDARD_ALPHABET;

      //! The constant value represents an UNBOUNDED quantity.
      //!
      //! \sa RWS::set_check_confluence_interval, RWS::set_max_rules,
      //! RWS::set_max_overlap.
      static size_t const UNBOUNDED = static_cast<size_t>(-2);

      //////////////////////////////////////////////////////////////////////////
      // Constructors and destructor
      //////////////////////////////////////////////////////////////////////////

      //! Constructs rewriting system with no rules and the reduction ordering
      //! \p order.
      //!
      //! This constructs a rewriting system with no rules, and with the
      //! reduction ordering ReductionOrdering specifed by the parameter \p
      //! order.
      explicit RWS(ReductionOrdering*, external_string_t = STANDARD_ALPHABET);
      explicit RWS(SemigroupBase*);
      explicit RWS(RWS const*);

      //! Constructs a rewriting system with no rules, and the SHORTLEX
      //! reduction ordering.
      RWS() : RWS(new SHORTLEX(), STANDARD_ALPHABET) {}

      //! Constructs a rewriting system with no rules, and the SHORTLEX
      //! reduction ordering and using the alphabet specified by the parameter
      //! \p alphabet.
      // Apparently old versions of GCC (4.8.2) don't like explicit constructors
      // with single default parameters:
      //    https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60367
      // and so we have two constructors instead.
      explicit RWS(external_string_t alphabet)
          : RWS(new SHORTLEX(), alphabet) {}

      //! A default destructor
      //!
      //! This deletes the reduction order used to construct the object, and the
      //! rules in the system.
      ~RWS();

      //////////////////////////////////////////////////////////////////////////
      // Setters for RWS optional parameters
      //////////////////////////////////////////////////////////////////////////

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
      void set_check_confluence_interval(size_t);

      //! This method can be used to specify the maximum length of the overlap
      //! of
      //! two left hand sides of rules that should be considered in
      //! RWS::knuth_bendix.
      //!
      //! If this value is less than the longest left hand side of a rule, then
      //! RWS::knuth_bendix can terminate without the system being confluent.
      //!
      //! \sa RWS::knuth_bendix.
      void set_max_overlap(size_t);

      //! This method sets the (approximate) maximum number of rules that the
      //! system should contain. If this is number is exceeded in calls to
      //! RWS::knuth_bendix or RWS::knuth_bendix_by_overlap_length, then
      //! these methods will terminate and the system may not be confluent.
      //!
      //! \sa RWS::knuth_bendix and RWS::knuth_bendix.
      void set_max_rules(size_t);

      //! This method can be used to determine the way that the length of an
      //! overlap of two words in the system is meaasured.
      //!
      //! \sa RWS::overlap_measure.
      void set_overlap_policy(overlap_policy);

      //////////////////////////////////////////////////////////////////////////
      // Overridden virtual methods from Interface
      //////////////////////////////////////////////////////////////////////////

      void                     set_nr_generators(size_t) override;
      size_t                   nr_generators() const override;
      void                     set_alphabet(external_string_t) override;
      external_string_t const& alphabet() const override;

     private:
      bool validate_word(word_t const&) const override;
      bool validate_word(external_string_t const&) const override;
      void set_isomorphic_non_fp_semigroup(SemigroupBase*) override;
      void internal_add_relation(word_t, word_t) override;

     public:
      void add_relation(word_t, word_t) override;
      void add_relation(external_string_t, external_string_t) override;
      using Interface::add_relation;

      bool           is_obviously_finite() const override;
      bool           is_obviously_infinite() const override;
      size_t         size() override;
      SemigroupBase* isomorphic_non_fp_semigroup() override;
      bool           has_isomorphic_non_fp_semigroup() override;

      bool equal_to(word_t const&, word_t const&) override;
      bool equal_to(std::string const&, std::string const&) override;
      using Interface::equal_to;

      word_t      normal_form(word_t const&) override;
      std::string normal_form(std::string const&) override;
      using Interface::normal_form;

      //////////////////////////////////////////////////////////////////////////
      // Overridden virtual methods from Runner
      //////////////////////////////////////////////////////////////////////////

      void run() override;

     private:
      //////////////////////////////////////////////////////////////////////////
      // RWS private methods for converting from ints <-> string/char
      //////////////////////////////////////////////////////////////////////////

      static size_t             internal_char_to_uint(internal_char_t c);
      static internal_char_t    uint_to_internal_char(size_t a);
      static internal_string_t* uint_to_internal_string(size_t);
      static word_t* internal_string_to_word(internal_string_t const*);

      // The second parameter is modified in-place and returned.
      static internal_string_t* word_to_internal_string(word_t const&,
                                                        internal_string_t*);

      // Returns a pointer, the caller should delete it.
      static internal_string_t* word_to_internal_string(word_t const&);

      internal_char_t external_to_internal_char(external_char_t) const;
      external_char_t internal_to_external_char(internal_char_t) const;
      void            external_to_internal_string(external_string_t& w) const;
      void            internal_to_external_string(internal_string_t& w) const;

      //////////////////////////////////////////////////////////////////////////
      // RWS private methods for rules
      //////////////////////////////////////////////////////////////////////////

      Rule* new_rule() const;
      Rule* new_rule(internal_string_t* lhs, internal_string_t* rhs) const;
      Rule* new_rule(Rule const* rule) const;
      Rule* new_rule(internal_string_t::const_iterator begin_lhs,
                     internal_string_t::const_iterator end_lhs,
                     internal_string_t::const_iterator begin_rhs,
                     internal_string_t::const_iterator end_rhs) const;

      void add_rule(Rule* rule);

      std::list<Rule const*>::iterator
      remove_rule(std::list<Rule const*>::iterator it);

     public:
      //////////////////////////////////////////////////////////////////////////
      // RWS public methods for rules and rewriting
      //////////////////////////////////////////////////////////////////////////

      //! Returns the current number of active rules in the rewriting system.
      size_t nr_rules() const;

      //! This method returns a vector consisting of the pairs of strings which
      //! represent the rules of the rewriting system. The \c first entry in
      //! every such pair is greater than the \c second according to the
      //! reduction ordering of the rewriting system. The rules are sorted
      //! according to the reduction ordering used by the rewriting system, on
      //! the first entry.
      std::vector<std::pair<external_string_t, external_string_t>>
      rules() const;

      //! Rewrites the word \p w in-place according to the current rules in the
      //! rewriting system, and returns it.
      external_string_t* rewrite(external_string_t* w) const;

      //! Rewrites a copy of the word \p w rewritten according to the current
      //! rules in the rewriting system.
      external_string_t rewrite(external_string_t w) const;

      //! This method allows a RWS object to be left shifted into a
      //! std::ostream, such as std::cout. The currently active rules of the
      //! system are represented in the output.
      friend std::ostream& operator<<(std::ostream& os, RWS const& rws);

      //////////////////////////////////////////////////////////////////////////
      // RWS the main public methods
      //////////////////////////////////////////////////////////////////////////

      //! Returns \c true if the rewriting system is
      //! [confluent](https://en.wikipedia.org/wiki/Confluence_(abstract_rewriting))
      //! and \c false if it is not.
      bool confluent() const;

      //! Run the [Knuth-Bendix
      //! algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
      //! on the rewriting system.
      //!
      //! \warning This will terminate when the rewriting system is confluent,
      //! which might be never.
      //!
      //! \sa knuth_bendix_by_overlap_length.
      void knuth_bendix();

      //! This method runs the Knuth-Bendix algorithm on the rewriting system by
      //! considering all overlaps of a given length \f$n\f$ (according to the
      //! RWS::overlap_measure) before those overlaps of length \f$n + 1\f$.
      //!
      //! \warning This will terminate when the rewriting system is confluent,
      //! which might be never.
      //!
      //! \sa RWS::knuth_bendix.
      void knuth_bendix_by_overlap_length();

     private:
      //////////////////////////////////////////////////////////////////////////
      // RWS private methods and data
      //////////////////////////////////////////////////////////////////////////

      // Rewrites the word pointed to by \p w in-place according to the current
      // rules in the rewriting system.
      void internal_rewrite(internal_string_t* w) const;
      void clear_stack();
      void push_stack(Rule* rule);
      void overlap(Rule const* u, Rule const* v);

      std::list<Rule const*>                               _active_rules;
      external_string_t                                    _alphabet;
      std::unordered_map<external_char_t, internal_char_t> _alphabet_map;
      size_t                           _check_confluence_interval;
      mutable std::atomic<bool>        _confluent;
      mutable std::atomic<bool>        _confluence_known;
      bool                             _delete_isomorphic_non_fp_semigroup;
      mutable std::list<Rule*>         _inactive_rules;
      SemigroupBase*                   _isomorphic_non_fp_semigroup;
      size_t                           _max_overlap;
      size_t                           _max_rules;
      size_t                           _min_length_lhs_rule;
      std::list<Rule const*>::iterator _next_rule_it1;
      std::list<Rule const*>::iterator _next_rule_it2;
      mutable size_t                   _nrgens;
      ReductionOrdering const*         _order;
      OverlapMeasure*                  _overlap_measure;
      overlap_policy                   _overlap_policy;
      std::set<RuleLookup>             _set_rules;
      std::stack<Rule*>                _stack;
      internal_string_t*               _tmp_word1;
      internal_string_t*               _tmp_word2;
      mutable size_t                   _total_rules;

#ifdef LIBSEMIGROUPS_STATS
      size_t                                max_active_word_length();
      size_t                                _max_stack_depth;
      size_t                                _max_word_length;
      size_t                                _max_active_word_length;
      size_t                                _max_active_rules;
      std::unordered_set<internal_string_t> _unique_lhs_rules;
#endif
    };

    // Class for rules in rewriting systems.
    struct RWS::Rule {
      friend std::ostream& operator<<(std::ostream& os, Rule const& rule) {
        os << rule.internal_to_external_string(rule.lhs()) << " -> "
           << rule.internal_to_external_string(rule.rhs());
        return os;
      }

      external_string_t
      internal_to_external_string(internal_string_t const* word) const {
        external_string_t str(*word);
        _rws->internal_to_external_string(str);
        return str;
      }

      // Returns the left hand side of the rule, which is guaranteed to be
      // greater than its right hand side according to the reduction ordering of
      // the RWS used to construct this.
      internal_string_t const* lhs() const {
        return const_cast<internal_string_t const*>(_lhs);
      }

      // Returns the right hand side of the rule, which is guaranteed to be
      // less than its left hand side according to the reduction ordering of
      // the RWS used to construct this.
      internal_string_t const* rhs() const {
        return const_cast<internal_string_t const*>(_rhs);
      }

      // The Rule class does not support an assignment contructor to avoid
      // accidental copying.
      Rule& operator=(Rule const& copy) = delete;

      // The Rule class does not support a copy contructor to avoid
      // accidental copying.
      Rule(Rule const& copy) = delete;

      // Construct from RWS with new but empty internal_string_t's
      explicit Rule(RWS const* rws, int64_t id)
          : _rws(rws),
            _lhs(new internal_string_t()),
            _rhs(new internal_string_t()),
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

      RWS const*         _rws;
      internal_string_t* _lhs;
      internal_string_t* _rhs;
      int64_t            _id;
    };

    // Simple class wrapping a two iterators to an internal_string_t and a Rule
    // const*
    class RWS::RuleLookup {
     public:
      RuleLookup() : _rule(nullptr) {}

      explicit RuleLookup(RWS::Rule* rule)
          : _first(rule->lhs()->cbegin()),
            _last(rule->lhs()->cend()),
            _rule(rule) {}

      RuleLookup& operator()(internal_string_t::iterator const& first,
                             internal_string_t::iterator const& last) {
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
      internal_string_t::const_iterator _first;
      internal_string_t::const_iterator _last;
      Rule const*                       _rule;
    };
  }  // namespace fpsemigroup
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_RWS_H_
