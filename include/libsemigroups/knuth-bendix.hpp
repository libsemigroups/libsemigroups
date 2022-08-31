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

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_HPP_

#include <cstddef>  // for size_t
#include <iosfwd>   // for string, ostream
#include <memory>   // for unique_ptr
#include <vector>   // for vector

#include "cong-intf.hpp"     // for CongruenceInterface
#include "digraph.hpp"       // for ActionDigraph
#include "fpsemi-intf.hpp"   // for FpSemigroupInterface
#include "froidure-pin.hpp"  // for FroidurePin
#include "make-present.hpp"  // for Presentation
#include "present.hpp"       // for Presentation
#include "types.hpp"         // for word_type
#include "word.hpp"          // for word_to_string

namespace libsemigroups {
  // Forward declarations
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
    //! of the class fpsemigroup::KnuthBendix.
    //!
    //! This class is used to represent a
    //! [string rewriting system](https://w.wiki/9Re)
    //! defining a finitely presented monoid or semigroup.
    //!
    //! \sa congruence::KnuthBendix.
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
    //! kb.run();
    //! kb.number_of_active_rules();  // 31
    //! kb.confluent();        // true
    //! \endcode
    class KnuthBendix final : public FpSemigroupInterface {
      friend class ::libsemigroups::congruence::KnuthBendix;
      friend class ::libsemigroups::detail::KBE;  // defined in kbe.hpp

     public:
      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - types - public
      //////////////////////////////////////////////////////////////////////////

      //! This type contains various enums for specifying certain options to a
      //! KnuthBendix instance.
      struct options {
        //! Values for specifying how to measure the length of an overlap.
        //!
        //! The values in this enum determine how a KnuthBendix instance
        //! measures the length \f$d(AB, BC)\f$ of the overlap of two words
        //! \f$AB\f$ and \f$BC\f$:
        //!
        //! \sa overlap_policy(options::overlap)
        enum class overlap {
          //! \f$d(AB, BC) = |A| + |B| + |C|\f$
          ABC = 0,
          //! \f$d(AB, BC) = |AB| + |BC|\f$
          AB_BC = 1,
          //! \f$d(AB, BC) = max(|AB|, |BC|)\f$
          MAX_AB_BC = 2
        };
      };

      //! The type of the return value of froidure_pin().
      //!
      //! froidure_pin() returns a \shared_ptr to a FroidurePinBase,
      //! which is really of type \ref froidure_pin_type.
      using froidure_pin_type
          = FroidurePin<detail::KBE,
                        FroidurePinTraits<detail::KBE, KnuthBendix>>;

      //////////////////////////////////////////////////////////////////////////
      // KnuthBendix - constructors and destructor - public
      //////////////////////////////////////////////////////////////////////////

      //! Default constructor.
      //!
      //! Constructs a KnuthBendix instance with no rules, and the short-lex
      //! reduction ordering.
      //!
      //! \parameters
      //! (None)
      //!
      //! \complexity
      //! Constant.
      KnuthBendix();

      //! Constructs from a FroidurePin instance.
      //!
      //! \param S the FroidurePin instance.
      //!
      //! \complexity
      //! \f$O(|S||A|)\f$ where \f$A\f$ is the set of generators used to define
      //! \p S.
      explicit KnuthBendix(FroidurePinBase& S) : KnuthBendix() {
        init_from(S);
      }

      //! Constructs from a shared pointer to a FroidurePin instance.
      //!
      //! \param S the FroidurePin instance.
      //!
      //! \complexity
      //! \f$O(|S||A|)\f$ where \f$A\f$ is the set of generators used to define
      //! \p S.
      explicit KnuthBendix(std::shared_ptr<FroidurePinBase> S)
          : KnuthBendix(*S) {}

      //! Copy constructor.
      //!
      //! \param copy the KnuthBendix instance to copy.
      //!
      //! \complexity
      //! \f$O(n)\f$ where \f$n\f$ is the sum of the lengths of the words in
      //! rules of \p copy.
      KnuthBendix(KnuthBendix const& copy);

      //! Deleted.
      KnuthBendix(KnuthBendix&&);

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
      //! The function \ref run periodically checks if
      //! the system is already confluent. This function can be used to
      //! set how frequently this happens, it is the number of new overlaps
      //! that should be considered before checking confluence. Setting this
      //! value too low can adversely affect the performance of
      //! \ref run.
      //!
      //! The default value is \c 4096, and should be set to
      //! \ref LIMIT_MAX if \ref run should never
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
      //! \sa \ref run.
      KnuthBendix& check_confluence_interval(size_t val) {
        _settings._check_confluence_interval = val;
        return *this;
      }

      //! Set the maximum length of overlaps to be considered.
      //!
      //! This function can be used to specify the maximum length of the
      //! overlap of two left hand sides of rules that should be considered in
      //! \ref run.
      //!
      //! If this value is less than the longest left hand side of a rule, then
      //! \ref run can terminate without the system being
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
      //! \sa \ref run.
      KnuthBendix& max_overlap(size_t val) {
        _settings._max_overlap = val;
        return *this;
      }

      //! Set the maximum number of rules.
      //!
      //! This member function sets the (approximate) maximum number of rules
      //! that the system should contain. If this is number is exceeded in
      //! calls to \ref run or
      //! knuth_bendix_by_overlap_length, then they
      //! will terminate and the system may not be confluent.
      //!
      //! By default this value is \ref POSITIVE_INFINITY.
      //!
      //! \param val the maximum number of rules.
      //!
      //! \returns
      //! A reference to \c *this.
      //!
      //! \complexity
      //! Constant.
      //!
      //! \sa \ref run.
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
      //! \sa options::overlap.
      KnuthBendix& overlap_policy(options::overlap val);

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
      //! \parameters
      //! (None)
      size_t number_of_active_rules() const noexcept;

      //! Returns a copy of the active rules.
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
      //! \parameters
      //! (None)
      // TODO(later) delete this
      std::vector<rule_type> active_rules() const;

      // TODO(later)
      // using FpSemigroupInterface::const_iterator;
      // const_iterator cbegin_active_rules() const;
      // const_iterator cend_active_rules() const;

      struct NormalFormsIteratorTraits {
        // state_type::first = this, state_type::second = current value
        using state_type = std::pair<std::string, std::string>;
        using internal_iterator_type
            = ActionDigraph<size_t>::const_pislo_iterator;
        using value_type        = std::string;
        using reference         = std::string&;
        using const_reference   = std::string const&;
        using difference_type   = std::ptrdiff_t;
        using size_type         = std::size_t;
        using const_pointer     = std::string const*;
        using pointer           = std::string*;
        using iterator_category = std::forward_iterator_tag;

        struct Deref {
          const_reference
          operator()(state_type&                   state,
                     internal_iterator_type const& it) const noexcept {
            if (state.second.empty()) {
              detail::word_to_string(state.first, *it, state.second);
            }
            return state.second;
          }
        };

        struct AddressOf {
          const_pointer
          operator()(state_type&                   state,
                     internal_iterator_type const& it) const noexcept {
            Deref()(state, it);  // to ensure that state.second is initialised
            return &state.second;
          }
        };

        struct PrefixIncrement {
          void operator()(state_type&             state,
                          internal_iterator_type& it) const noexcept {
            ++it;
            state.second.clear();
          }
        };

        struct Swap {
          void operator()(internal_iterator_type& it_this,
                          internal_iterator_type& it_that,
                          state_type&             state_this,
                          state_type&             state_that) const noexcept {
            swap(it_this, it_that);
            swap(state_this, state_that);
          }
        };

        using EqualTo          = void;
        using NotEqualTo       = void;
        using PostfixIncrement = void;
      };

      //! Type of an const iterator to a normal form.
      using const_normal_form_iterator
          = detail::ConstIteratorStateful<NormalFormsIteratorTraits>;

      static_assert(
          std::is_default_constructible<const_normal_form_iterator>::value,
          "forward iterator requires default-constructible");
      static_assert(
          std::is_copy_constructible<const_normal_form_iterator>::value,
          "forward iterator requires copy-constructible");
      static_assert(std::is_copy_assignable<const_normal_form_iterator>::value,
                    "forward iterator requires copy-assignable");
      static_assert(std::is_destructible<const_normal_form_iterator>::value,
                    "forward iterator requires destructible");

      //! Returns a forward iterator pointing at the first normal form with
      //! length in a given range.
      //!
      //! If incremented, the iterator will point to the next least short-lex
      //! normal form (if it's less than \p max in length).  Iterators of the
      //! type returned by this function should only be compared with other
      //! iterators created from the same KnuthBendix instance.
      //!
      //! \param lphbt the alphabet to use for the normal forms
      //! \param min the minimum length of a normal form
      //! \param max one larger than the maximum length of a normal form.
      //!
      //! \returns
      //! A value of type \ref const_normal_form_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \warning
      //! Copying iterators of this type is relatively expensive.  As a
      //! consequence, prefix incrementing \c ++it the iterator \c it returned
      //! by \c cbegin_normal_forms is significantly cheaper than postfix
      //! incrementing \c it++.
      //!
      //! \warning
      //! If the finitely presented semigroup represented by \c this is
      //! infinite, then \p max should be chosen with some care.
      //!
      //! \sa
      //! \ref cend_normal_forms.
      const_normal_form_iterator cbegin_normal_forms(std::string const& lphbt,
                                                     size_t             min,
                                                     size_t             max);

      //! Returns a forward iterator pointing at the first normal form with
      //! length in a given range.
      //!
      //! If incremented, the iterator will point to the next least short-lex
      //! normal form (if it's less than \p max in length).  Iterators of the
      //! type returned by this function should only be compared with other
      //! iterators created from the same KnuthBendix instance.
      //!
      //! \param min the minimum length of a normal form
      //! \param max one larger than the maximum length of a normal form.
      //!
      //! \returns
      //! A value of type `const_normal_form_iterator`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \warning
      //! Copying iterators of this type is expensive.  As a consequence, prefix
      //! incrementing \c ++it the iterator \c it returned by \c
      //! cbegin_normal_forms is significantly cheaper than postfix
      //! incrementing \c it++.
      //!
      //! \warning
      //! If the finitely presented semigroup represented by \c this is infinite
      //! then \p max should be chosen with some care.
      //!
      //! \sa
      //! cend_normal_forms.
      const_normal_form_iterator cbegin_normal_forms(size_t min, size_t max) {
        return cbegin_normal_forms(alphabet(), min, max);
      }

      //! Returns a forward iterator pointing to one after the last normal form.
      //!
      //! The iterator returned by this function can be compared with the
      //! return value of \ref cbegin_normal_forms with any parameters.
      //!
      //! \warning The iterator returned by this function may still
      //! dereferencable and incrementable, but will not point to a normal form
      //! in the correct range.
      //!
      //! \sa
      //! \ref cbegin_normal_forms.
      const_normal_form_iterator cend_normal_forms() {
        using state_type = NormalFormsIteratorTraits::state_type;
        return const_normal_form_iterator(state_type("", ""),
                                          gilman_digraph().cend_pislo());
      }

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

      //! Check confluence of the current rules.
      //!
      //! \returns \c true if the KnuthBendix instance is
      //! [confluent](https://w.wiki/9DA) and \c false if it is not.
      //!
      //! \parameters
      //! (None)
      bool confluent() const;

      //! Run the Knuth-Bendix by considering all overlaps of a given length.
      //!
      //! This function runs the Knuth-Bendix algorithm on the rewriting system
      //! represented by a KnuthBendix instance by considering all overlaps of
      //! a given length \f$n\f$ (according to the \ref options::overlap) before
      //! those overlaps of length \f$n + 1\f$.
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
      //! \sa \ref run.
      //!
      //! \parameters
      //! (None)
      void knuth_bendix_by_overlap_length();

      //! Returns the Gilman digraph.
      //!
      //! \returns A const reference to a \ref ActionDigraph.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \warning This will terminate when the KnuthBendix instance is
      //! reduced and confluent, which might be never.
      //!
      //! \sa \ref number_of_normal_forms,
      //! \ref cbegin_normal_forms, and \ref cend_normal_forms.
      //!
      //! \parameters
      //! (None)
      ActionDigraph<size_t> const& gilman_digraph();

      //! Returns whether or not the empty string belongs in the system.
      //!
      //! \returns
      //! A value of type `bool`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! \f$O(n)\f$ where \f$n\f$ is the number of rules.
      //!
      //! \parameters
      //! (None)
      bool contains_empty_string() const;

      //! Returns the number of normal forms with length in a given range.
      //!
      //! \param min the minimum length of a normal form to count
      //! \param max one larger than the maximum length of a normal form to
      //! count.
      //!
      //! \returns
      //! A value of type `uint64_t`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Assuming that \c this has been run until finished, the complexity of
      //! this function is at worst \f$O(mn)\f$ where \f$m\f$ is the number of
      //! letters in the alphabet, and \f$n\f$ is the number of nodes in the
      //! \ref gilman_digraph.
      uint64_t number_of_normal_forms(size_t min, size_t max);

      //////////////////////////////////////////////////////////////////////////
      // FpSemigroupInterface - pure virtual member functions - public
      //////////////////////////////////////////////////////////////////////////

      //! \copydoc FpSemigroupInterface::size
      //!
      //! \note If \c this has been run until finished, then this function can
      //! determine the size of the semigroup represented by \c this even if it
      //! is infinite. Moreover, the complexity of this function is at worst
      //! \f$O(mn)\f$ where \f$m\f$ is the number of letters in the alphabet,
      //! and \f$n\f$ is the number of nodes in the
      //! \ref gilman_digraph.
      uint64_t size() override;

      bool equal_to(std::string const&, std::string const&) override;

      std::string normal_form(std::string const& w) override;

      //////////////////////////////////////////////////////////////////////////
      // FpSemigroupInterface - non-pure virtual member functions - public
      //////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
      // The following are required for overload resolution.
      // Documented in FpSemigroupInterface.
      // Sphinx/doxygen get confused by this, so we don't allow Doxygen to parse
      // these two declarations.
      using FpSemigroupInterface::equal_to;
      using FpSemigroupInterface::normal_form;
#endif

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

      bool finished_impl() const override;

      bool is_obviously_infinite_impl() override;
      bool is_obviously_finite_impl() override;

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
        size_t           _check_confluence_interval;
        size_t           _max_overlap;
        size_t           _max_rules;
        options::overlap _overlap_policy;
      } _settings;

      // Forward declarations
      class KnuthBendixImpl;

      ActionDigraph<size_t>            _gilman_digraph;
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
    //! congruence::KnuthBendix.
    //!
    //! \sa fpsemigroup::KnuthBendix.
    //!
    //! \note congruence::KnuthBendix can only be used to compute 2-sided
    //! congruences.
    //!
    //! \par Example
    //! \code
    //! KnuthBendix kb;
    //! kb.set_number_of_generators(2);
    //! kb.add_pair({0, 0, 0}, {0});
    //! kb.add_pair({0}, {1, 1});
    //!
    //! kb.number_of_classes();                            // 5
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

      //! Default constructor.
      //!
      //! \parameters
      //! (None)
      //!
      //! \complexity
      //! Constant.
      KnuthBendix();

      //! Constructs from FroidurePin instance.
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

      //! Construct from fpsemigroup::KnuthBendix.
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

      //! Construct from \shared_ptr to FroidurePin instance.
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

      //! Copy constructor
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

      ~KnuthBendix();

      //! Returns the underlying fpsemigroup::KnuthBendix.
      //!
      //! \parameters
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
      size_t    number_of_classes_impl() override;
      std::shared_ptr<FroidurePinBase> quotient_impl() override;
      class_index_type word_to_class_index_impl(word_type const&) override;
      void             run_impl() override;

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////////

      void add_pair_impl(word_type const&, word_type const&) override;
      void set_number_of_generators_impl(size_t) override;
      bool is_quotient_obviously_finite_impl() override;
      bool is_quotient_obviously_infinite_impl() override;

      ////////////////////////////////////////////////////////////////////////////
      // KnuthBendix - data - private
      ////////////////////////////////////////////////////////////////////////////

      std::unique_ptr<fpsemigroup::KnuthBendix> _kb;
    };
  }  // namespace congruence

  namespace presentation {

    //! Return an iterator pointing at the left hand side of a redundant rule.
    //!
    //! This function is defined in ``knuth-bendix.hpp``.
    //!
    //! Starting with the last rule in the presentation, this function attempts
    //! to run the Knuth-Bendix algorithm on the rules of the presentation
    //! except for the given omitted rule. For every such omitted rule,
    //! Knuth-Bendix is run for the length of time indicated by the second
    //! parameter \p t, and then it is checked if the omitted rule can be shown
    //! to be redundant (rewriting both sides of the omitted rule using the
    //! other rules using the output of the, not necessarily finished,
    //! Knuth-Bendix algorithm).
    //!
    //! If the omitted rule can be shown to be redundant in this way, then an
    //! iterator pointing to its left hand side is returned.
    //!
    //! If no rule can be shown to be redundant in this way, then an iterator
    //! pointing to `p.cend()` is returned.
    //!
    //! \tparam T type of the 2nd parameter (time to try running Knuth-Bendix).
    //! \param p the presentation
    //! \param t time to run KnuthBendix for every omitted rule
    //!
    //! \warning The progress of the Knuth-Bendix algorithm may differ between
    //! different calls to this function even if the parameters are identical.
    //! As such this is non-deterministic, and may produce different results
    //! with the same input.
    template <typename T>
    auto redundant_rule(Presentation<std::string>& p, T t) {
      p.validate();
      for (auto omit = p.rules.crbegin(); omit != p.rules.crend(); omit += 2) {
        fpsemigroup::KnuthBendix kb;
        kb.set_alphabet(p.alphabet());

        for (auto it = p.rules.crbegin(); it != omit; it += 2) {
          kb.add_rule(*it, *(it + 1));
        }
        for (auto it = omit + 2; it < p.rules.crend(); it += 2) {
          kb.add_rule(*it, *(it + 1));
        }
        kb.run_for(t);
        if (kb.rewrite(*omit) == kb.rewrite(*(omit + 1))) {
          return (omit + 1).base() - 1;
        }
      }
      return p.rules.cend();
    }

    //! Return an iterator pointing at the left hand side of a redundant rule.
    //!
    //! This function is defined in ``knuth-bendix.hpp``.
    //!
    //! Starting with the last rule in the presentation, this function attempts
    //! to run the Knuth-Bendix algorithm on the rules of the presentation
    //! except for the given omitted rule. For every such omitted rule,
    //! Knuth-Bendix is run for the length of time indicated by the second
    //! parameter \p t, and then it is checked if the omitted rule can be shown
    //! to be redundant (rewriting both sides of the omitted rule using the
    //! other rules using the output of the, not necessarily finished,
    //! Knuth-Bendix algorithm).
    //!
    //! If the omitted rule can be shown to be redundant in this way, then an
    //! iterator pointing to its left hand side is returned.
    //!
    //! If no rule can be shown to be redundant in this way, then an iterator
    //! pointing to `p.cend()` is returned.
    //!
    //! \tparam W type of words in the Presentation
    //! \tparam T type of the 2nd parameter (time to try running Knuth-Bendix).
    //! \param p the presentation
    //! \param t time to run KnuthBendix for every omitted rule
    //!
    //! \warning The progress of the Knuth-Bendix algorithm may differ between
    //! different calls to this function even if the parameters are identical.
    //! As such this is non-deterministic, and may produce different results
    //! with the same input.
    template <typename W, typename T>
    auto redundant_rule(Presentation<W>& p, T t) {
      auto pp = make<Presentation<std::string>>(p);
      return p.rules.cbegin()
             + std::distance(pp.rules.cbegin(), redundant_rule(pp, t));
    }
  }  // namespace presentation
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_HPP_
