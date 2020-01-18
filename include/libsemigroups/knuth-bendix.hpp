//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// TODO(later):
// 1. Don't use Pimpl (what was I thinking??)

#ifndef LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_HPP_
#define LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_HPP_

#include <cstddef>  // for size_t
#include <iosfwd>   // for string, ostream
#include <memory>   // for unique_ptr
#include <vector>   // for vector

#include "cong-intf.hpp"    // for CongruenceInterface
#include "fpsemi-intf.hpp"  // for FpSemigroupInterface
#include "types.hpp"        // for word_type

namespace libsemigroups {
  // Forward declarations
  class FroidurePinBase;
  namespace detail {
    class KBE;
  }
  namespace congruence {
    class KnuthBendix;
  }

  namespace fpsemigroup {
    //! Defined in ``knuth-bendix.hpp``.
    //!
    //! On this page we describe the functionality relating to the Knuth-Bendix
    //! algorithm for semigroups and monoids that is available in
    //! ``libsemigroups``. This page contains a details of the member functions
    //! of the class libsemigroups::fpsemigroup::KnuthBendix.
    //!
    //! This class is used to represent a
    //! [string rewriting system](https://w.wiki/9Re)
    //! defining a finitely presented monoid or semigroup.
    //!
    //! \sa libsemigroups::congruence::KnuthBendix.
    //!
    //! \par Example
    //! \code
    //! KnuthBendix kb;
    //! kb.set_alphabet("abc");
    //!
    //! kb.add_rule("aaaa", "a");
    //! kb.add_rule("bbbb", "b");
    //! kb.add_rule("cccc", "c");
    //! kb.add_rule("abab", "aaa");
    //! kb.add_rule("bcbc", "bbb");
    //!
    //! !kb.confluent();       // true
    //! kb.knuth_bendix();
    //! kb.nr_active_rules();  // 31
    //! kb.confluent();        // true
    //! \endcode
    class KnuthBendix final : public FpSemigroupInterface {
      friend class ::libsemigroups::congruence::KnuthBendix;
      friend class ::libsemigroups::detail::KBE;  // defined in kbe.hpp

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - types - public
      //////////////////////////////////////////////////////////////////////////

      //! This type contains various enums for specifying policies to a
      //! KnuthBendix instance.
      struct policy {
        //! The values in this enum determine how a KnuthBendix instance
        //! measures the length \f$d(AB, BC)\f$ of the overlap of two words
        //! \f$AB\f$ and \f$BC\f$:
        //!
        //! \sa KnuthBendix::overlap_policy(policy::overlap)
        enum class overlap {
          //! \f$d(AB, BC) = |A| + |B| + |C|\f$
          ABC = 0,
          //! \f$d(AB, BC) = |AB| + |BC|\f$
          AB_BC = 1,
          //! \f$d(AB, BC) = max(|AB|, |BC|)\f$
          MAX_AB_BC = 2
        };
      };

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - constructors and destructor - public
      //////////////////////////////////////////////////////////////////////////

      //! Constructs a KnuthBendix instance with no rules, and the short-lex
      //! reduction ordering.
      //!
      //! \par Parameters
      //! (None)
      //!
      //! \complexity
      //! Constant.
      KnuthBendix();

      //! Constructs a KnuthBendix instance from a FroidurePin instance.
      //!
      //! \param S the FroidurePin instance.
      //!
      //! \complexity
      //! \f$O(|S||A|)\f$ where \f$A\f$ is the set of generators used to define
      //! \p S.
      explicit KnuthBendix(FroidurePinBase& S) : KnuthBendix() {
        init_from(S);
      }

      //! \copydoc KnuthBendix(FroidurePinBase&).
      explicit KnuthBendix(std::shared_ptr<FroidurePinBase> S)
          : KnuthBendix(*S) {}

      //! Copy construct a KnuthBendix instance.
      //!
      //! \param copy the KnuthBendix instance to copy.
      //!
      //! \complexity
      //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
      //! rules of \p copy.
      KnuthBendix(KnuthBendix const& copy);

      //! Deleted.
      KnuthBendix(KnuthBendix&&) = delete;

      //! Deleted.
      KnuthBendix& operator=(KnuthBendix const&) = delete;

      //! Deleted.
      KnuthBendix& operator=(KnuthBendix&&) = delete;

      ~KnuthBendix();

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - setters for optional parameters - public
      //////////////////////////////////////////////////////////////////////////

      //! Set the interval at which confluence is checked.
      //!
      //! KnuthBendix::knuth_bendix periodically checks if
      //! the system is already confluent. This function can be used to
      //! set how frequently this happens, it is the number of new overlaps
      //! that should be considered before checking confluence. Setting this
      //! value too low can adversely affect the performance of
      //! KnuthBendix::knuth_bendix.
      //!
      //! The default value is 4096, and should be set to
      //! libsemigroups::LIMIT_MAX if KnuthBendix::knuth_bendix should never
      //! check if the system is already confluent.
      //!
      //! \param val the new value of the interval.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa KnuthBendix::knuth_bendix.
      KnuthBendix& check_confluence_interval(size_t val) {
        _settings._check_confluence_interval = val;
        return *this;
      }

      //! Set the maximum length of overlaps to be considered.
      //!
      //! This function can be used to specify the maximum length of the
      //! overlap of two left hand sides of rules that should be considered in
      //! KnuthBendix::knuth_bendix.
      //!
      //! If this value is less than the longest left hand side of a rule, then
      //! KnuthBendix::knuth_bendix can terminate without the system being
      //! confluent.
      //!
      //! \param val the new value of the maximum overlap length.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa KnuthBendix::knuth_bendix.
      KnuthBendix& max_overlap(size_t val) {
        _settings._max_overlap = val;
        return *this;
      }

      //! Set the maximum number of rules.
      //!
      //! This member function sets the (approximate) maximum number of rules
      //! that the system should contain. If this is number is exceeded in
      //! calls to KnuthBendix::knuth_bendix or
      //! KnuthBendix::knuth_bendix_by_overlap_length, then they
      //! will terminate and the system may not be confluent.
      //!
      //! By default this value is libsemigroups::POSITIVE_INFINITY.
      //!
      //! \param val the maximum number of rules.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa KnuthBendix::knuth_bendix and KnuthBendix::knuth_bendix.
      KnuthBendix& max_rules(size_t val) {
        _settings._max_rules = val;
        return *this;
      }

      //! Set the overlap policy.
      //!
      //! This function can be used to determine the way that the length
      //! of an overlap of two words in the system is measured.
      //!
      //! \param val the maximum number of rules.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa KnuthBendix::policy::overlap.
      KnuthBendix& overlap_policy(policy::overlap val);

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - member functions for rules and rewriting - public
      //////////////////////////////////////////////////////////////////////////

      //! Returns the current number of active rules in the KnuthBendix
      //! instance.
      //!
      //! \returns
      //! The current number of active rules, a value of type `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \complexity
      //! Constant.
      //!
      //! \par Parameters
      //! (None)
      size_t nr_active_rules() const noexcept;

      //! Returns a copy of the active rules of the KnuthBendix instance.
      //!
      //! This member function returns a vector consisting of the pairs of
      //! strings which represent the rules of the KnuthBendix instance. The \c
      //! first entry in every such pair is greater than the \c second according
      //! to the reduction ordering of the KnuthBendix instance. The rules are
      //! sorted according to the reduction ordering used by the rewriting
      //! system, on the first entry.
      //!
      //! \returns
      //! A copy of the currently active rules, a value of type
      //! `std::vector<rule_type>`.
      //!
      //! \complexity
      //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
      //! rules of \p copy.
      //!
      //! \par Parameters
      //! (None)
      // TODO(later) delete this
      std::vector<rule_type> active_rules() const;

      // TODO(later)
      // using FpSemigroupInterface::const_iterator;
      // const_iterator cbegin_active_rules() const;
      // const_iterator cend_active_rules() const;

      //! Rewrite a word in-place.
      //!
      //! The word \p w is rewritten in-place according to the current active
      //! rules in the KnuthBendix instance.
      //!
      //! \param w the word to rewrite.
      //!
      //! \returns
      //! The argument \p w after it has been rewritten.
      // TODO(later) change to void rewrite(std::string&);
      std::string* rewrite(std::string* w) const;

      //! Rewrite a word.
      //!
      //! Rewrites a copy of the word \p w rewritten according to the current
      //! rules in the KnuthBendix instance.
      //!
      //! \param w the word to rewrite.
      //!
      //! \returns
      //! A copy of the argument \p w after it has been rewritten.
      std::string rewrite(std::string w) const {
        rewrite(&w);
        return w;
      }

      //! This friend function allows a KnuthBendix object to be left shifted
      //! into a std::ostream, such as std::cout. The currently active rules of
      //! the system are represented in the output.
      friend std::ostream& operator<<(std::ostream&, KnuthBendix const&);

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - main member functions - public
      //////////////////////////////////////////////////////////////////////////

      //! Check if the KnuthBendix instance is confluent.
      //!
      //! \returns \c true if the KnuthBendix instance is
      //! [confluent](https://w.wiki/9DA) and \c false if it is not.
      //!
      //! \par Parameters
      //! (None)
      bool confluent() const;

      //! Run the [Knuth-Bendix algorithm](https://w.wiki/9Cz)
      //! on the KnuthBendix instance.
      //!
      //! \returns
      //! (None)
      //!
      //! \complexity
      //! See warning.
      //!
      //! \warning This will terminate when the KnuthBendix instance is
      //! confluent, which might be never.
      //!
      //! \sa knuth_bendix_by_overlap_length.
      //!
      //! \par Parameters
      //! (None)
      // void knuth_bendix();

      //! Run the [Knuth-Bendix algorithm](https://w.wiki/9Cz)
      //! on the KnuthBendix instance.
      //!
      //! This function runs the Knuth-Bendix algorithm on the rewriting
      //! system represented by a KnuthBendix instance by considering all
      //! overlaps of a given length \f$n\f$ (according to the
      //! KnuthBendix::overlap_measure) before those overlaps of length
      //! \f$n + 1\f$.
      //!
      //! \returns
      //! (None)
      //!
      //! \complexity
      //! See warning.
      //!
      //! \warning This will terminate when the KnuthBendix instance is
      //! confluent, which might be never.
      //!
      //! \sa KnuthBendix::run.
      //!
      //! \par Parameters
      //! (None)
      void knuth_bendix_by_overlap_length();

      //////////////////////////////////////////////////////////////////////////
      // FpSemigroupInterface - pure virtual member functions - public
      //////////////////////////////////////////////////////////////////////////

      size_t size() override;

      bool equal_to(std::string const&, std::string const&) override;

      std::string normal_form(std::string const& w) override {
        validate_word(w);
        run();
        return rewrite(w);
      }

      //////////////////////////////////////////////////////////////////////////
      // FpSemigroupInterface - non-pure virtual member functions - public
      //////////////////////////////////////////////////////////////////////////

      using FpSemigroupInterface::equal_to;
      using FpSemigroupInterface::normal_form;

     private:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - initialisers - private
      //////////////////////////////////////////////////////////////////////////

      void init_from(KnuthBendix const&, bool = true);
      void init_from(FroidurePinBase&);

      //////////////////////////////////////////////////////////////////////////
      // FpSemigroupInterface - pure virtual member functions - private
      //////////////////////////////////////////////////////////////////////////

      void add_rule_impl(std::string const&, std::string const&) override;
      std::shared_ptr<FroidurePinBase> froidure_pin_impl() override;

      void run_impl() override;

      bool finished_impl() const override {
        return confluent();
      }

      bool is_obviously_infinite_impl() override;

      //////////////////////////////////////////////////////////////////////////
      // FpSemigroupInterface - non-pure virtual member functions - private
      //////////////////////////////////////////////////////////////////////////

      void set_alphabet_impl(std::string const&) override;
      void set_alphabet_impl(size_t) override;

      void validate_word_impl(std::string const&) const override {
        // do nothing, the empty string is allowed!
      }

      bool validate_identity_impl(std::string const&) const override;

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - data - private
      //////////////////////////////////////////////////////////////////////////

      struct Settings {
        Settings();
        size_t          _check_confluence_interval;
        size_t          _max_overlap;
        size_t          _max_rules;
        policy::overlap _overlap_policy;
      } _settings;

      class KnuthBendixImpl;  // Forward declaration
      std::unique_ptr<KnuthBendixImpl> _impl;
    };
  }  // namespace fpsemigroup

  namespace congruence {
    //! Defined in ``knuth-bendix.hpp``.
    //!
    //! On this page we describe the functionality relating to the
    //! Knuth-Bendix algorithm for computing congruences of semigroups and
    //! monoids.
    //!
    //! This page contains details of the member functions of the class
    //! libsemigroups::congruence::KnuthBendix.
    //!
    //! \sa libsemigroups::fpsemigroup::KnuthBendix.
    //!
    //! \par Example
    //! \code
    //! KnuthBendix kb;
    //! kb.set_nr_generators(2);
    //! kb.add_pair({0, 0, 0}, {0});
    //! kb.add_pair({0}, {1, 1});
    //!
    //! kb.nr_classes();                            // 5
    //! kb.word_to_class_index({0, 0, 1});          // 4
    //! kb.word_to_class_index({0, 0, 0, 0, 1});    // 4
    //! kb.word_to_class_index({0, 1, 1, 0, 0, 1}); // 4
    //! kb.word_to_class_index({0, 0, 0});          // 0
    //! kb.word_to_class_index({1});                // 1
    //! kb.word_to_class_index({0, 0, 0, 0});       // 2
    //! \endcode
    class KnuthBendix : public CongruenceInterface {
     public:
      ////////////////////////////////////////////////////////////////////////////
      // KnuthBendix - constructors - public
      ////////////////////////////////////////////////////////////////////////////

      //! Constructs a KnuthBendix instance with no generating pairs.
      //!
      //! \par Parameters
      //! (None)
      //!
      //! \complexity
      //! Constant.
      KnuthBendix();

      //! Constructs a KnuthBendix instance from a FroidurePin instance.
      //!
      //! \param fp the FroidurePin instance.
      //!
      //! \complexity
      //! \f$O(|S||A|)\f$ where \f$A\f$ is the set of generators used to define
      //! \p S and \p S is the semigroup represented by the FroidurePin
      //! instance \p fp.
      //!
      //! \warning
      //! The FroidurePin instance used in construction is copied by this
      //! constructor. Use KnuthBendix(std::shared_ptr<FroidurePinBase>) to
      //! avoid making a copy.
      template <typename T>
      explicit KnuthBendix(T const& fp)
          : KnuthBendix(static_cast<std::shared_ptr<FroidurePinBase>>(
              std::make_shared<T>(fp))) {
        static_assert(std::is_base_of<FroidurePinBase, T>::value,
                      "the template parameter must be a derived class of "
                      "FroidurePinBase");
      }

      //! Constructs a congruence::KnuthBendix instance from an
      //! fpsemigroup::KnuthBendix.
      //!
      //! A congruence::KnuthBendix instance simply wraps an
      //! fpsemigroup::KnuthBendix, and provides an API compatible with the
      //! other algorithms for congruences in libsemigroups.
      //!
      //! \param copy the fpsemigroup::KnuthBendix.
      //!
      //! \complexity
      //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
      //! rules of \p copy.
      explicit KnuthBendix(fpsemigroup::KnuthBendix const& copy);

      //! Constructs a KnuthBendix instance from a FroidurePin instance.
      //!
      //! \param fpb the FroidurePin instance.
      //!
      //! \complexity
      //! \f$O(|S||A|)\f$ where \f$A\f$ is the set of generators used to define
      //! \p S.
      //!
      //! \note
      //! The FroidurePin instance used in construction is not copied.
      explicit KnuthBendix(std::shared_ptr<FroidurePinBase> fpb);

      //! Copy construct a congruence::KnuthBendix instance from a
      //! congruence::KnuthBendix.
      //!
      //! \param copy the congruence::KnuthBendix to be copied.
      //!
      //! \complexity
      //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
      //! rules of \p copy.
      KnuthBendix(KnuthBendix const& copy) : KnuthBendix(*copy._kb) {}

      //! Deleted.
      KnuthBendix(KnuthBendix&&) = delete;

      //! Deleted.
      KnuthBendix& operator=(KnuthBendix const&) = delete;

      //! Deleted.
      KnuthBendix& operator=(KnuthBendix&&) = delete;

      //! Returns the underlying fpsemigroup::KnuthBendix.
      //!
      //! \par Parameters
      //! (None)
      //!
      //! \complexity
      //! Constant.
      fpsemigroup::KnuthBendix& knuth_bendix() const {
        return *_kb;
      }
      ////////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual member functions - public
      ////////////////////////////////////////////////////////////////////////////

      tril const_contains(word_type const&, word_type const&) const override;
      bool contains(word_type const&, word_type const&) override;

     private:
      ////////////////////////////////////////////////////////////////////////////
      // Runner - pure virtual member functions - protected
      ////////////////////////////////////////////////////////////////////////////

      bool finished_impl() const override;

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////////

      word_type class_index_to_word_impl(class_index_type) override;
      size_t    nr_classes_impl() override;
      std::shared_ptr<FroidurePinBase> quotient_impl() override;
      class_index_type word_to_class_index_impl(word_type const&) override;
      void             run_impl() override;

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////////

      void add_pair_impl(word_type const&, word_type const&) override;
      void set_nr_generators_impl(size_t) override;
      bool is_quotient_obviously_finite_impl() override;
      bool is_quotient_obviously_infinite_impl() override {
        return _kb->is_obviously_infinite();
      }

      ////////////////////////////////////////////////////////////////////////////
      // KnuthBendix - data - private
      ////////////////////////////////////////////////////////////////////////////

      std::unique_ptr<fpsemigroup::KnuthBendix> _kb;
    };
  }  // namespace congruence
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_KNUTH_BENDIX_HPP_
