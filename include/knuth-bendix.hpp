//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains a class KnuthBendix which implements the Knuth-Bendix
// algorithm for finitely presented monoids.

#ifndef LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_HPP_
#define LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_HPP_

#include <string>

#include "cong-base.hpp"
#include "fpsemi-base.hpp"
#include "types.hpp"

namespace libsemigroups {
  class KBE;                // Forward declaration
  class ReductionOrdering;  // Forward declaration
  namespace congruence {
    class KnuthBendix;  // Forward declaration
  }

  namespace fpsemigroup {
    //! This class is used to represent a
    //! [string rewriting
    //! system](https://en.wikipedia.org/wiki/Semi-Thue_system)
    //! defining a finitely presented monoid or semigroup.

    class KnuthBendix : public FpSemiBase {
      friend class ::libsemigroups::congruence::KnuthBendix;
      friend class ::libsemigroups::KBE;  // defined in kbe.hpp

     public:
      //////////////////////////////////////////////////////////////////////////
      // Runner - overridden virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      void run() override;

      //////////////////////////////////////////////////////////////////////////
      // FpSemiBase - overridden pure virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      void add_rule(std::string const&, std::string const&) override;

      bool   is_obviously_finite() override;
      bool   is_obviously_infinite() override;
      size_t size() override;

      bool        equal_to(std::string const&, std::string const&) override;
      std::string normal_form(std::string const&) override;

      FroidurePinBase* isomorphic_non_fp_semigroup() override;

      //////////////////////////////////////////////////////////////////////////
      // FpSemiBase - overridden non-pure virtual methods - public
      //////////////////////////////////////////////////////////////////////////

      void set_alphabet(std::string const&) override;
      void set_alphabet(size_t) override;

      using FpSemiBase::add_rule;
      using FpSemiBase::equal_to;
      using FpSemiBase::normal_form;

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - static data members and enums - public
      //////////////////////////////////////////////////////////////////////////

      //! The values in this enum determine how a rewriting system measures the
      //! length \f$d(AB, BC)\f$ of the overlap of two words \f$AB\f$ and
      //! \f$BC\f$:
      //! * ***ABC***:        \f$d(AB, BC) = |A| + |B| + |C|\f$
      //! * ***AB_BC***:      \f$d(AB, BC) = |AB| + |BC|\f$
      //! * ***MAX_AB_BC***:  \f$d(AB, BC) = max(|AB|, |BC|)\f$
      //!
      //! \sa KnuthBendix::set_overlap_policy.
      enum class overlap_policy { ABC = 0, AB_BC = 1, MAX_AB_BC = 2 };

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - constructors and destructor - public
      //////////////////////////////////////////////////////////////////////////

      //! Constructs rewriting system with no rules and the reduction ordering
      //! \p order.
      //!
      //! This constructs a rewriting system with no rules, and with the
      //! reduction ordering ReductionOrdering specifed by the parameter \p
      //! order.
      explicit KnuthBendix(ReductionOrdering*);
      explicit KnuthBendix(FroidurePinBase*);
      explicit KnuthBendix(FroidurePinBase&);
      explicit KnuthBendix(KnuthBendix const*);

      //! Constructs a rewriting system with no rules, and the SHORTLEX
      //! reduction ordering.
      KnuthBendix();

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
      std::vector<std::pair<std::string, std::string>> rules() const;

      //! Rewrites the word \p w in-place according to the current rules in the
      //! rewriting system, and returns it.
      std::string* rewrite(std::string*) const;

      //! Rewrites a copy of the word \p w rewritten according to the current
      //! rules in the rewriting system.
      std::string rewrite(std::string) const;

      //! This method allows a KnuthBendix object to be left shifted into a
      //! std::ostream, such as std::cout. The currently active rules of the
      //! system are represented in the output.
      friend std::ostream& operator<<(std::ostream&, KnuthBendix const&);

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
      // KnuthBendix - data - private
      //////////////////////////////////////////////////////////////////////////

      struct Settings {
        Settings();
        size_t         _check_confluence_interval;
        size_t         _max_overlap;
        size_t         _max_rules;
        overlap_policy _overlap_policy;
      } _settings;

      class KnuthBendixImpl;  // forward declaration
      KnuthBendixImpl* _impl;
    };
  }  // namespace fpsemigroup

  namespace congruence {
    class KnuthBendix : public CongBase {
     public:
      ////////////////////////////////////////////////////////////////////////////
      // KnuthBendix - constructors - public
      ////////////////////////////////////////////////////////////////////////////

      KnuthBendix();
      explicit KnuthBendix(FroidurePinBase&);
      explicit KnuthBendix(fpsemigroup::KnuthBendix const*);

      ////////////////////////////////////////////////////////////////////////////
      // Runner - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      void run() override;

      ////////////////////////////////////////////////////////////////////////////
      // CongBase - overridden pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      void             add_pair(word_type const&, word_type const&) override;
      word_type        class_index_to_word(class_index_type) override;
      FroidurePinBase* quotient_semigroup() override;
      size_t           nr_classes() override;
      class_index_type word_to_class_index(word_type const&) override;

      ////////////////////////////////////////////////////////////////////////////
      // CongBase - overridden non-pure virtual methods - public
      ////////////////////////////////////////////////////////////////////////////

      result_type const_contains(word_type const&,
                                 word_type const&) const override;
      bool        contains(word_type const&, word_type const&) override;
      void        set_nr_generators(size_t) override;

     protected:
      ////////////////////////////////////////////////////////////////////////////
      // Runner - overridden non-pure virtual methods - protected
      ////////////////////////////////////////////////////////////////////////////

      bool finished_impl() const override;

     private:
      std::unique_ptr<fpsemigroup::KnuthBendix> _kb;
    };
  }  // namespace congruence
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_HPP_
