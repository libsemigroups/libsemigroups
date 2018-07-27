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

#ifndef LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_H_
#define LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_H_

#include <algorithm>
#include <atomic>
#include <list>
#include <ostream>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "internal/libsemigroups-config.h"
#include "internal/libsemigroups-debug.h"

#include "cong-intf.h"
#include "fpsemi-intf.h"
#include "kb-order.h"
#include "types.h"

// TODO: - move implementation to .cc file
//       - don't use the typedef external_string_type in public methods, use
//       std::string instead.

namespace libsemigroups {
  class KBE; // Forward declaration

  namespace congruence {
    class KnuthBendix; // Forward declaration
  }

  namespace fpsemigroup {
    //! This class is used to represent a
    //! [string rewriting
    //! system](https://en.wikipedia.org/wiki/Semi-Thue_system)
    //! defining a finitely presented monoid or semigroup.

    class KnuthBendix : public FpSemiIntf {
      friend class ::libsemigroups::congruence::KnuthBendix;
     public:
      // TODO typedef isomorphic_non_fp_semigroup_type
      //////////////////////////////////////////////////////////////////////////
      // Runner - overridden virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      void run() override;

      //////////////////////////////////////////////////////////////////////////
      // FpSemiIntf - overridden pure virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      void add_rule(std::string const&, std::string const&) override;

      bool   is_obviously_finite() override;
      bool   is_obviously_infinite() override;
      size_t size() override;

      bool        equal_to(std::string const&, std::string const&) override;
      std::string normal_form(std::string const&) override;

      SemigroupBase* isomorphic_non_fp_semigroup() override;

      //////////////////////////////////////////////////////////////////////////
      // FpSemiIntf - overridden non-pure virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      void set_alphabet(std::string const&) override;
      void set_alphabet(size_t) override;

      using FpSemiIntf::add_rule;
      using FpSemiIntf::normal_form;
      using FpSemiIntf::equal_to;

     private:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - forward declarations - private
      //////////////////////////////////////////////////////////////////////////

      class RuleLookup;       // defined in knuth-bendix.h
      struct OverlapMeasure;  // defined in knuth-bendix.cc
      friend struct Rule;     // defined in knuth-bendix.h
      struct Rule;
      friend class ::libsemigroups::KBE;       // defined in kbe.h

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - typedefs - private
      //////////////////////////////////////////////////////////////////////////

      using internal_char_type = char;
      using external_char_type = char;

      using internal_string_type = std::string;
      using external_string_type = std::string;

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - static data members and enums - public
      //////////////////////////////////////////////////////////////////////////

      //! The values in this enum determine how a rewriting system measures the
      //! length \f$d(AB, BC)\f$ of the overlap of two words \f$AB\f$ and
      //! \f$BC\f$:
      //! * ***ABC***:        \f$d(AB, BC) = |A| + |B| + |C|\f$
      //! * ***AB_BC***:      \f$d(AB, BC) = |AB| + |BC|\f$
      //! * ***max_AB_BC***:  \f$d(AB, BC) = max(|AB|, |BC|)\f$
      //!
      //! \sa KnuthBendix::set_overlap_policy.
      // TODO enum class
      enum overlap_policy { ABC = 0, AB_BC = 1, max_AB_BC = 2 };

      //! The constant value represents an UNBOUNDED quantity.
      //!
      //! \sa KnuthBendix::set_check_confluence_interval,
      //! KnuthBendix::set_max_rules, KnuthBendix::set_max_overlap.
      // TODO move to constants.h
      static size_t const UNBOUNDED = std::numeric_limits<size_t>::max() - 2;

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - constructors and destructor - public
      //////////////////////////////////////////////////////////////////////////

      //! Constructs rewriting system with no rules and the reduction ordering
      //! \p order.
      //!
      //! This constructs a rewriting system with no rules, and with the
      //! reduction ordering ReductionOrdering specifed by the parameter \p
      //! order.
      explicit KnuthBendix(ReductionOrdering*,
                           external_string_type = "");
      explicit KnuthBendix(SemigroupBase*);
      explicit KnuthBendix(SemigroupBase&);
      explicit KnuthBendix(KnuthBendix const*);

      //! Constructs a rewriting system with no rules, and the SHORTLEX
      //! reduction ordering.
      KnuthBendix() : KnuthBendix(new SHORTLEX()) {}

      //! Constructs a rewriting system with no rules, and the SHORTLEX
      //! reduction ordering and using the alphabet specified by the parameter
      //! \p alphabet.
      // Apparently old versions of GCC (4.8.2) don't like explicit constructors
      // with single default parameters:
      //    https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60367
      // and so we have two constructors instead.
      explicit KnuthBendix(external_string_type const& alphabet)
          : KnuthBendix(new SHORTLEX(), alphabet) {}

      explicit KnuthBendix(size_t n) : KnuthBendix(new SHORTLEX()) {
        set_alphabet(n);
      }

      //! A default destructor
      //!
      //! This deletes the reduction order used to construct the object, and the
      //! rules in the system.
      ~KnuthBendix();

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - setters for optional parameters - public
      //////////////////////////////////////////////////////////////////////////

      //! The method KnuthBendix::knuth_bendix periodically checks if the system
      //! is already confluent. This method can be used to set how frequently
      //! this happens, it is the number of new overlaps that should be
      //! considered before checking confluence. Setting this value too low can
      //! adversely affect the performance of KnuthBendix::knuth_bendix.
      //!
      //! The default value is 4096, and should be set to KnuthBendix::UNBOUNDED
      //! if KnuthBendix::knuth_bendix should never check if the system is
      //! already confluent.
      //!
      //! \sa KnuthBendix::knuth_bendix.
      void set_check_confluence_interval(size_t);

      //! This method can be used to specify the maximum length of the overlap
      //! of two left hand sides of rules that should be considered in
      //! KnuthBendix::knuth_bendix.
      //!
      //! If this value is less than the longest left hand side of a rule, then
      //! KnuthBendix::knuth_bendix can terminate without the system being
      //! confluent.
      //!
      //! \sa KnuthBendix::knuth_bendix.
      void set_max_overlap(size_t);

      //! This method sets the (approximate) maximum number of rules that the
      //! system should contain. If this is number is exceeded in calls to
      //! KnuthBendix::knuth_bendix or
      //! KnuthBendix::knuth_bendix_by_overlap_length, then these methods will
      //! terminate and the system may not be confluent.
      //!
      //! \sa KnuthBendix::knuth_bendix and KnuthBendix::knuth_bendix.
      void set_max_rules(size_t);

      //! This method can be used to determine the way that the length of an
      //! overlap of two words in the system is meaasured.
      //!
      //! \sa KnuthBendix::overlap_measure.
      void set_overlap_policy(overlap_policy);

     private:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - methods for converting ints <-> string/char - private
      //////////////////////////////////////////////////////////////////////////

      static size_t                internal_char_to_uint(internal_char_type c);
      static internal_char_type    uint_to_internal_char(size_t a);
      static internal_string_type* uint_to_internal_string(size_t);
      static word_type* internal_string_to_word(internal_string_type const*);

      // The second parameter is modified in-place and returned.
      static internal_string_type*
      word_to_internal_string(word_type const&, internal_string_type*);

      // Returns a pointer, the caller should delete it.
      static internal_string_type* word_to_internal_string(word_type const&);

      internal_char_type external_to_internal_char(external_char_type) const;
      external_char_type internal_to_external_char(internal_char_type) const;
      void external_to_internal_string(external_string_type& w) const;
      void internal_to_external_string(internal_string_type& w) const;

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - methods for rules - private
      //////////////////////////////////////////////////////////////////////////

      Rule* new_rule() const;
      Rule* new_rule(internal_string_type* lhs,
                     internal_string_type* rhs) const;
      Rule* new_rule(Rule const* rule) const;
      Rule* new_rule(internal_string_type::const_iterator begin_lhs,
                     internal_string_type::const_iterator end_lhs,
                     internal_string_type::const_iterator begin_rhs,
                     internal_string_type::const_iterator end_rhs) const;

      void add_rule(Rule* rule);

      std::list<Rule const*>::iterator
      remove_rule(std::list<Rule const*>::iterator it);

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - methods for rules and rewriting - public
      //////////////////////////////////////////////////////////////////////////

      //! Returns the current number of active rules in the rewriting system.
      size_t nr_rules() const noexcept override;

      //! This method returns a vector consisting of the pairs of strings which
      //! represent the rules of the rewriting system. The \c first entry in
      //! every such pair is greater than the \c second according to the
      //! reduction ordering of the rewriting system. The rules are sorted
      //! according to the reduction ordering used by the rewriting system, on
      //! the first entry.
      std::vector<std::pair<external_string_type, external_string_type>>
      rules() const;

      //! Rewrites the word \p w in-place according to the current rules in the
      //! rewriting system, and returns it.
      external_string_type* rewrite(external_string_type* w) const;

      //! Rewrites a copy of the word \p w rewritten according to the current
      //! rules in the rewriting system.
      external_string_type rewrite(external_string_type w) const;

      //! This method allows a KnuthBendix object to be left shifted into a
      //! std::ostream, such as std::cout. The currently active rules of the
      //! system are represented in the output.
      friend std::ostream& operator<<(std::ostream& os, KnuthBendix const& kb);

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - main methods - public
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
      //! KnuthBendix::overlap_measure) before those overlaps of length \f$n +
      //! 1\f$.
      //!
      //! \warning This will terminate when the rewriting system is confluent,
      //! which might be never.
      //!
      //! \sa KnuthBendix::knuth_bendix.
      void knuth_bendix_by_overlap_length();

     private:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - methods and data - private
      //////////////////////////////////////////////////////////////////////////

      // Rewrites the word pointed to by \p w in-place according to the current
      // rules in the rewriting system.
      void internal_rewrite(internal_string_type* w) const;
      void clear_stack();
      void push_stack(Rule* rule);
      void overlap(Rule const* u, Rule const* v);

      std::list<Rule const*>           _active_rules;
      size_t                           _check_confluence_interval;
      mutable std::atomic<bool>        _confluent;
      mutable std::atomic<bool>        _confluence_known;
      mutable std::list<Rule*>         _inactive_rules;
      bool                             _internal_is_same_as_external;
      size_t                           _max_overlap;
      size_t                           _max_rules;
      size_t                           _min_length_lhs_rule;
      std::list<Rule const*>::iterator _next_rule_it1;
      std::list<Rule const*>::iterator _next_rule_it2;
      ReductionOrdering const*         _order;
      OverlapMeasure*                  _overlap_measure;
      overlap_policy                   _overlap_policy;
      std::set<RuleLookup>             _set_rules;
      std::stack<Rule*>                _stack;
      internal_string_type*            _tmp_word1;
      internal_string_type*            _tmp_word2;
      mutable size_t                   _total_rules;

#ifdef LIBSEMIGROUPS_STATS
      size_t                                   max_active_word_length();
      size_t                                   _max_stack_depth;
      size_t                                   _max_word_length;
      size_t                                   _max_active_word_length;
      size_t                                   _max_active_rules;
      std::unordered_set<internal_string_type> _unique_lhs_rules;
#endif
    };

    // Class for rules in rewriting systems.
    struct KnuthBendix::Rule {
      friend std::ostream& operator<<(std::ostream& os, Rule const& rule) {
        os << rule.internal_to_external_string(rule.lhs()) << " -> "
           << rule.internal_to_external_string(rule.rhs());
        return os;
      }

      external_string_type
      internal_to_external_string(internal_string_type const* word) const {
        external_string_type str(*word);
        _kb->internal_to_external_string(str);
        return str;
      }

      // Returns the left hand side of the rule, which is guaranteed to be
      // greater than its right hand side according to the reduction ordering of
      // the KnuthBendix used to construct this.
      internal_string_type const* lhs() const {
        return const_cast<internal_string_type const*>(_lhs);
      }

      // Returns the right hand side of the rule, which is guaranteed to be
      // less than its left hand side according to the reduction ordering of
      // the KnuthBendix used to construct this.
      internal_string_type const* rhs() const {
        return const_cast<internal_string_type const*>(_rhs);
      }

      // The Rule class does not support an assignment contructor to avoid
      // accidental copying.
      Rule& operator=(Rule const& copy) = delete;

      // The Rule class does not support a copy contructor to avoid
      // accidental copying.
      Rule(Rule const& copy) = delete;

      // Construct from KnuthBendix with new but empty internal_string_type's
      explicit Rule(KnuthBendix const* kb, int64_t id)
          : _kb(kb),
            _lhs(new internal_string_type()),
            _rhs(new internal_string_type()),
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
        _kb->internal_rewrite(_lhs);
        _kb->internal_rewrite(_rhs);
        reorder();
      }

      void rewrite_rhs() {
        LIBSEMIGROUPS_ASSERT(_id != 0);
        _kb->internal_rewrite(_rhs);
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
        if ((*(_kb->_order))(_rhs, _lhs)) {
          std::swap(_lhs, _rhs);
        }
      }

      KnuthBendix const*    _kb;
      internal_string_type* _lhs;
      internal_string_type* _rhs;
      int64_t               _id;
    };

    // Simple class wrapping a two iterators to an internal_string_type and a
    // Rule const*
    class KnuthBendix::RuleLookup {
     public:
      RuleLookup() : _rule(nullptr) {}

      explicit RuleLookup(KnuthBendix::Rule* rule)
          : _first(rule->lhs()->cbegin()),
            _last(rule->lhs()->cend()),
            _rule(rule) {}

      RuleLookup& operator()(internal_string_type::iterator const& first,
                             internal_string_type::iterator const& last) {
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
      internal_string_type::const_iterator _first;
      internal_string_type::const_iterator _last;
      Rule const*                          _rule;
    };
  }  // namespace fpsemigroup

  namespace congruence {
    class KnuthBendix : public CongIntf {
     public:
      ////////////////////////////////////////////////////////////////////////////
      // KnuthBendix - constructors - public
      ////////////////////////////////////////////////////////////////////////////

      KnuthBendix();
      explicit KnuthBendix(SemigroupBase&);
      // TODO remove the next, currently unused
      KnuthBendix(size_t                            nrgens,
                  std::vector<relation_type> const& relations,
                  std::vector<relation_type> const& extra = {});

      ////////////////////////////////////////////////////////////////////////////
      // Runner - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      void run() override;

      ////////////////////////////////////////////////////////////////////////////
      // CongIntf - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      void             add_pair(word_type, word_type) override;
      word_type        class_index_to_word(class_index_type) override;
      SemigroupBase*   quotient_semigroup() override;
      size_t           nr_classes() override;
      class_index_type word_to_class_index(word_type const&) override;

      ////////////////////////////////////////////////////////////////////////////
      // CongIntf - overridden non-pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      void set_nr_generators(size_t) override;

     private:
      std::unique_ptr<fpsemigroup::KnuthBendix> _kbfp;
    };
  }  // namespace congruence
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_H_
