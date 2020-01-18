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

// This file contains a declaration of a class for performing the Todd-Coxeter
// algorithm for semigroups and monoid.

#ifndef LIBSEMIGROUPS_INCLUDE_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_INCLUDE_TODD_COXETER_HPP_

#include <chrono>   // for chrono::nanoseconds
#include <cstddef>  // for size_t
#include <memory>   // for shared_ptr
#include <stack>    // for stack
#include <utility>  // for pair
#include <vector>   // for vector

#include "cong-intf.hpp"            // for congruence_type,...
#include "cong-wrap.hpp"            // for CongruenceWrapper
#include "containers.hpp"           // for DynamicArray2
#include "coset.hpp"                // for CosetManager
#include "int-range.hpp"            // for IntegralRange
#include "iterator.hpp"             // for ConstIteratorStateful
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "report.hpp"               // for REPORT
#include "string.hpp"               // for to_string
#include "types.hpp"                // for word_type, letter_type...

namespace libsemigroups {
  // Forward declarations
  namespace detail {
    class TCE;
  }
  class FroidurePinBase;

  namespace congruence {
    class ToddCoxeter;
  }

  namespace fpsemigroup {
    class KnuthBendix;

    //! This class is a wrapper for congruence::ToddCoxeter that provides the
    //! FpSemigroupInterface.
    //! See CongruenceWrapper for further details.
    using ToddCoxeter = CongruenceWrapper<congruence::ToddCoxeter>;
  }  // namespace fpsemigroup

  namespace congruence {
    //! Defined in ``todd-coxeter.hpp``.
    //!
    //! This class contains the main implementation of the Todd-Coxeter
    //! algorithm for computing left, right, and 2-sided congruences on
    //! semigroups and monoids.
    //!
    //! This page contains a summary of the main member functions of the class
    //! libsemigroups::congruence::ToddCoxeter, and related things in
    //! libsemigroups.
    //!
    //! In this documentation we use the term "coset enumeration" to mean the
    //! execution of (any version of) the Todd-Coxeter algorithm.
    //!
    //! \sa congruence_type and tril.
    //!
    //! \par Example 1
    //! \code
    //! ToddCoxeter tc(congruence_type::left);  // construct a left congruence
    //! tc.set_nr_generators(2);                // 2 generators
    //! tc.add_pair({0, 0}, {0});               // generator 0 squared is itself
    //! tc.add_pair({0}, {1});                  // generator 0 equals 1
    //! tc.strategy(policy::strategy::felsch);  // set the strategy
    //! tc.nr_classes();                        // calculate number of classes
    //! tc.contains({0, 0, 0, 0}, {0, 0});      // check if 2 words are equal
    //! tc.word_to_class_index({0, 0, 0, 0});   // get the index of a class
    //! tc.standardize(order::lex);             // standardize to lex order
    //! \endcode
    //!
    //! \par Example 2
    //! \code
    //! ToddCoxeter tc(congruence_type::twosided);
    //! tc.set_nr_generators(4);
    //! tc.add_pair({0, 0}, {0});
    //! tc.add_pair({1, 0}, {1});
    //! tc.add_pair({0, 1}, {1});
    //! tc.add_pair({2, 0}, {2});
    //! tc.add_pair({0, 2}, {2});
    //! tc.add_pair({3, 0}, {3});
    //! tc.add_pair({0, 3}, {3});
    //! tc.add_pair({1, 1}, {0});
    //! tc.add_pair({2, 3}, {0});
    //! tc.add_pair({2, 2, 2}, {0});
    //! tc.add_pair({1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2}, {0});
    //! tc.add_pair({1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3,
    //!              1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3},
    //!             {0});
    //! tc.strategy(policy::strategy::hlt)
    //!    .standardize(false)
    //!    .lookahead(policy::lookahead::partial)
    //!    .save(false)
    //! tc.nr_classes()  // 10752
    //! tc.complete();   // true
    //! tc.compatible(); // true
    //! auto& S = tc.quotient_semigroup();  // FroidurePin<TCE>
    //! S.size()                            // 10752
    //! S.nr_idempotents()                  // 1
    //! tc.standardize(order::recursive);
    //! std::vector<word_type>(tc.cbegin_normal_forms(),
    //!                        tc.cbegin_normal_forms() + 10);
    //! // {{0},
    //! //  {1},
    //! //  {2},
    //! //  {2, 1},
    //! //  {1, 2},
    //! //  {1, 2, 1},
    //! //  {2, 2},
    //! //  {2, 2, 1},
    //! //  {2, 1, 2},
    //! //  {2, 1, 2, 1}};
    //! tc.standardize(order::lex);
    //! std::vector<word_type>(tc.cbegin_normal_forms(),
    //!                        tc.cbegin_normal_forms() + 10);
    //! // {{0},
    //! //  {0, 1},
    //! //  {0, 1, 2},
    //! //  {0, 1, 2, 1},
    //! //  {0, 1, 2, 1, 2},
    //! //  {0, 1, 2, 1, 2, 1},
    //! //  {0, 1, 2, 1, 2, 1, 2},
    //! //  {0, 1, 2, 1, 2, 1, 2, 1},
    //! //  {0, 1, 2, 1, 2, 1, 2, 1, 2},
    //! //  {0, 1, 2, 1, 2, 1, 2, 1, 2, 1}};
    //! \endcode
    class ToddCoxeter final : public CongruenceInterface,
                              public detail::CosetManager {
      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - typedefs - private
      ////////////////////////////////////////////////////////////////////////

      using Table = detail::DynamicArray2<class_index_type>;

      // Forward declared
      struct NormalFormIteratorTraits;

     public:
      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - typedefs + enums - public
      ////////////////////////////////////////////////////////////////////////

      //! This is the type of the indices used for cosets in a
      //! ToddCoxeter instance.
      using coset_type = CongruenceInterface::class_index_type;

      //! This is the return type of ToddCoxeter::cbegin_normal_forms and
      //! ToddCoxeter::cend_normal_forms, which can be used to access normal
      //! forms from a coset index.
      using normal_form_iterator
          = detail::ConstIteratorStateful<NormalFormIteratorTraits>;

      //! This struct holds various enums which effect the coset enumeration
      //! process used by ToddCoxeter::run.
      //!
      //! \sa policy::strategy, policy::lookahead, and policy::froidure_pin.
      struct policy {
        //! The values in this enum can be used as the argument for the
        //! member function ToddCoxeter::strategy to specify which strategy
        //! should be used when performing a coset enumeration.
        enum class strategy {
          //! This value indicates that the HLT (Hazelgrove-Leech-Trotter)
          //! strategy should be used. This is the same as ACE's R-style.
          hlt,
          //! This value indicates that the Felsch
          //! strategy should be used. This is the same as ACE's C-style.
          felsch,
          //! This value indicates that a random combination of the HLT and
          //! Felsch strategies should be used. A random strategy (and
          //! associated options) are selected from one of the 10 options:
          //! 1. HLT + full lookahead + no deduction processing +
          //!    standardization
          //! 2. HLT + full lookahead + deduction processing + standardization
          //! 3. HLT + full lookahead + no deduction processing +
          //!    no standardization
          //! 4. HLT + full lookahead + deduction processing + no
          //!    standardization
          //! 5. HLT + partial lookahead + no deduction processing +
          //!   standardization
          //! 6. HLT + partial lookahead + deduction processing +
          //!    standardization
          //! 7. HLT + partial lookahead + no deduction processing +
          //!    no standardization
          //! 8. HLT + partial lookahead + deduction processing + no
          //!    standardization
          //! 9. Felsch + standardization
          //! 10. Felsch + no standardization
          //!
          //! and this strategy is then run for approximately the amount
          //! of time specified by the setting random_interval. This strategy is
          //! inspired by Sim's TEN_CE from [Sim94](../biblio.html#sims1994aa).
          random
        };

        //! The values in this enum can be used as the argument for
        //! ToddCoxeter::lookahead to specify the type of lookahead that should
        //! be performed when using the HLT strategy.
        enum class lookahead {
          //! A *full* lookahead is one starting from the initial coset.
          //! Full lookaheads are therefore sometimes slower but may
          //! detect more coincidences than a partial lookahead.
          full,
          //! A *partial* lookahead is one starting from the current coset.
          //! Partial lookaheads are therefore sometimes faster but may not
          //! detect as many coincidences as a full lookahead.
          partial
        };

        //! The values in this enum can be used as the argument for
        //! ToddCoxeter::fpp (which stands for "Froidure-Pin policy") to specify
        //! whether the defining relations, or the left/right Cayley graph, of a
        //! FroidurePin instance, should be used in the coset enumeration.
        //!
        //! If the number of classes in the congruence represented by a
        //! ToddCoxeter instance is relatively small, by some definition,
        //! compared to the size of the semigroup represented by the FroidurePin
        //! instance, then the use_relations policy is often faster. If the
        //! number of classes is relatively large, then use_cayley_graph is
        //! often faster. It is guaranteed that ToddCoxeter::run will terminate
        //! in an amount of time proportionate to the size of the input if the
        //! policy use_cayley_graph is used, whereas the run time when using the
        //! policy use_relations can be arbitrarily high regardless of the size
        //! of the input.
        enum class froidure_pin {
          //! No policy has been specified.
          none,
          //! Use the relations of a FroidurePin instance
          use_relations,
          //! Use the left or right Cayley graph of a FroidurePin instance
          use_cayley_graph
        };
      };

      //! The values in this enum can be used as the argument for
      //! ToddCoxeter::standardize to specify which ordering should be used.
      //! The normal forms for congruence classes are given with respect to one
      //! of the orders specified by the values in this enum.
      enum class order {
        //! No standardization has been done.
        none = 0,
        //! Normal forms are the short-lex least word belonging to a given
        //! congruence class.
        shortlex,
        //! Normal forms are the lexicographical least word belonging to a given
        //! congruence class.
        lex,
        //! Normal forms are the recursive-path least word belonging to a given
        //! congruence class.
        recursive
        // wreath TODO(later)
      };

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - constructors and destructor - public
      ////////////////////////////////////////////////////////////////////////

      //! A constructor that creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence specified by the given
      //! libsemigroups::congruence_type.
      explicit ToddCoxeter(congruence_type);

      //! A constructor that creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence over the semigroup
      //! represented by a FroidurePin instance of the type specified by
      //! the libsemigroups::congruence_type.
      //!
      //! \warning The parameter \p S is copied, this might be expensive, use a
      //! std::shared_ptr to avoid the copy!
      template <typename T>
      ToddCoxeter(congruence_type type, T const& S) : ToddCoxeter(type) {
        static_assert(std::is_base_of<FroidurePinBase, T>::value
                          || std::is_base_of<FpSemigroupInterface, T>::value,
                      "the template parameter must be a derived class of "
                      "FroidurePinBase or FpSemigroupInterface");
        set_parent_froidure_pin(S);
        set_nr_generators(S.nr_generators());
      }

      //! Construct from a FroidurePinBase.
      //!
      //! \param t the type of the congruence being constructed
      //! \param fp a std::shared_ptr to a FroidurePinBase, this parameter is
      //! not copied by this constructor.
      //! \param p the policy::froidure_pin to use.
      //!
      //! \returns
      //! A ToddCoxeter instance representing a congruence of type \p t over
      //! the semigroup represented by \p fp.
      ToddCoxeter(congruence_type                  t,
                  std::shared_ptr<FroidurePinBase> fp,
                  policy::froidure_pin             p
                  = policy::froidure_pin::use_cayley_graph);
      // TODO(later) remove the final argument here
      // This policy::froidure_pin is guaranteed to terminate relatively quickly

      //! A constructor that creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence from the congruence represented
      //! by the second argument. If the second argument is a left or right
      //! congruence, then the first argument must be
      //! libsemigroups::congruence_type::left or
      //! libsemigroups::congruence_type::right, otherwise an exception is
      //! thrown.
      ToddCoxeter(congruence_type, ToddCoxeter&);

      //! A constructor that creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence from the quotient semigroup
      //! represented by the second argument.
      ToddCoxeter(congruence_type, fpsemigroup::ToddCoxeter&);

      //! A constructor that creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence from the quotient semigroup
      //! represented by the second argument.
      ToddCoxeter(congruence_type, fpsemigroup::KnuthBendix&);

      //! Copy constructor.
      //!
      //! Constructs a copy of \p copy.
      //!
      //! \param copy the ToddCoxeter instance to copy.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      ToddCoxeter(ToddCoxeter const& copy);

      //! Deleted
      ToddCoxeter() = delete;

      //! Deleted
      ToddCoxeter(ToddCoxeter&&) = delete;

      //! Deleted
      ToddCoxeter& operator=(ToddCoxeter const&) = delete;

      //! Deleted
      ToddCoxeter& operator=(ToddCoxeter&&) = delete;

      ~ToddCoxeter();

      ////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual member functions - public
      ////////////////////////////////////////////////////////////////////////

      bool contains(word_type const&, word_type const&) override;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (init + settings) - public
      ////////////////////////////////////////////////////////////////////////

      // Initialisation
      //! This member function allows a ToddCoxeter instance to be prefilled
      //! with an existing coset table. The argument should represent the left
      //! or right Cayley graph of a finite semigroup. If an invalid table is
      //! given, then an exception is thrown.
      void prefill(Table const&);

      // Settings
      //! Sets the value of the "Froidure-Pin policy" specified by the argument
      //! ToddCoxeter::policy::froidure_pin.
      //!
      //! If the ToddCoxeter instance is not created from a FroidurePin
      //! instance, or from an object that has an already computed FroidurePin
      //! instance, then the value of this setting is ignored.
      //!
      //! The default value is policy::froidure_pin::use_cayley_graph.
      ToddCoxeter& froidure_pin_policy(policy::froidure_pin) noexcept;

      //! Gets the current value of the "Froidure-Pin policy".
      //!
      //! If the ToddCoxeter instance is not created from a FroidurePin
      //! instance, or from an object that has an already computed FroidurePin
      //! instance, then the value of this setting is ignored.
      //!
      //! \sa froidure_pin_policy(policy::froidure_pin)
      policy::froidure_pin froidure_pin_policy() const noexcept;

      //! Sets the type of lookahead to be used when using the HLT strategy. If
      //! the strategy is not HLT, then the value of this setting is
      //! ignored.
      //!
      //! The default value is policy::lookahead::partial.
      //!
      //! \sa ToddCoxeter::policy::lookahead.
      ToddCoxeter& lookahead(policy::lookahead) noexcept;

      //! Sets a lower bound for the number of classes of the congruence
      //! represented by a ToddCoxeter instance. If
      //! ToddCoxeter::nr_cosets_active becomes at least the value of the
      //! argument, and the table is complete (ToddCoxeter::complete returns
      //! \c true), then the coset enumeration is terminated. When the given
      //! bound is equal to the number of classes, this may save tracing
      //! relations at many cosets when there is no possibility of finding
      //! coincidences.
      //!
      //! The default value is libsemigroups::UNDEFINED.
      ToddCoxeter& lower_bound(size_t) noexcept;

      //! If the number of cosets active exceeds the value set by this
      //! function, then a lookahead, of the type set by
      //! ToddCoxeter::lookahead, is triggered. This only applies when using
      //! the HLT strategy.
      //!
      //! The default value is 5 million.
      ToddCoxeter& next_lookahead(size_t) noexcept;

      //! If the argument of this function is \c true and the HLT strategy is
      //! being used, then deductions are processed during the enumeration.
      //!
      //! The default value is \c false.
      ToddCoxeter& save(bool);  // NOLINT()

      //! If the argument of this function is \c true, then the coset table is
      //! standardized (according to the short-lex order) during the coset
      //! enumeration.
      //!
      //! The default value is \c false.
      ToddCoxeter& standardize(bool) noexcept;  // NOLINT()

      //! The strategy used during the coset enumeration can be specified using
      //! this function. It can be set to HLT, Felsch, or random.
      //!
      //! The default value is policy::strategy::hlt.
      //!
      //! \sa ToddCoxeter::policy::strategy.
      ToddCoxeter& strategy(policy::strategy);

      //! The current strategy being used for coset enumeration.
      //!
      //! \sa ToddCoxeter::policy::strategy
      policy::strategy strategy() const noexcept;

      //! Sets the duration in nanoseconds that a given randomly selected
      //! strategy will run for, when using the random strategy
      //! (policy::strategy::random).
      //!
      //! The default value is 200ms.
      ToddCoxeter& random_interval(std::chrono::nanoseconds) noexcept;

      //! Sets the duration (by converting to nanoseconds) that a given
      //! randomly selected strategy will run for, when using the random
      //! strategy (policy::strategy::random).
      //!
      //! The default value is 200ms.
      template <typename T>
      ToddCoxeter& random_interval(T x) noexcept {
        return random_interval(std::chrono::nanoseconds(x));
      }

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (container-like) - public
      ////////////////////////////////////////////////////////////////////////

      //! Returns \c true if there are no relations or generating pairs in the
      //! ToddCoxeter instance, and the number of active cosets is \c 1 (the
      //! minimum possible).
      bool empty() const;

      //! Reserves the capacity specified by the argument in the data
      //! structures for cosets used in a ToddCoxeter instance.
      //!
      //! The default is \c 0.
      void reserve(size_t);

      //! Release all memory used to store free cosets, and any other
      //! unnecessary data if the enumeration is finished. Otherwise, it does
      //! nothing.
      void shrink_to_fit();

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (state) - public
      ////////////////////////////////////////////////////////////////////////

      //! Returns \c true if the coset table is complete, and \c false if it is
      //! not. The table is *complete* if the value libsemigroups::UNDEFINED
      //! does not appear in any row of an active coset.
      bool complete() const noexcept;

      //! Returns \c true if the coset table is compatible with the relations
      //! and generating pairs used to create \c this, and \c false if it is
      //! not. The table is *compatible* if the values obtained by pushing
      //! a coset through the left-hand side and the right-hand side of
      //! a relation coincide for every coset and every relation.
      bool compatible() const noexcept;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (standardization) - public
      ////////////////////////////////////////////////////////////////////////

      //! Returns \c true if the ToddCoxeter instance is standardized. In other
      //! words, if ToddCoxeter::standardize(order) has been
      //! called with any argument other than order::none.
      bool is_standardized() const noexcept;

      //! Standardizes the current coset table according to the order specified
      //! by ToddCoxeter::order.
      void standardize(order);

      //! Friend functions for TCE
      friend Table* table(ToddCoxeter*);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - iterators - public
      ////////////////////////////////////////////////////////////////////////

      //! Returns a const iterator pointing to the normal form of the first
      //! class of the congruence represented by an instance of ToddCoxeter.
      //! The order of the classes, and the normal form, that is returned are
      //! controlled by ToddCoxeter::standardize(order).
      normal_form_iterator cbegin_normal_forms() {
        auto range = IntegralRange<coset_type>(0, nr_classes());
        return normal_form_iterator(this, range.cbegin());
      }

      //! Returns a const iterator one past the normal form of the last class
      //! of the congruence represented by an instance of ToddCoxeter. The
      //! order of the classes, and the normal form, that is returned are
      //! controlled by ToddCoxeter::standardize(order).
      normal_form_iterator cend_normal_forms() {
        auto range = IntegralRange<coset_type>(0, nr_classes());
        return normal_form_iterator(this, range.cend());
      }

     private:
      void run_impl() override;
      bool finished_impl() const override;

      ////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////

      word_type class_index_to_word_impl(coset_type) override;
      size_t    nr_classes_impl() override;
      // Guaranteed to return a FroidurePin<TCE>.
      std::shared_ptr<FroidurePinBase> quotient_impl() override;
      coset_type word_to_class_index_impl(word_type const&) override;

      ////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////

      coset_type const_word_to_class_index(word_type const&) const override;
      bool       is_quotient_obviously_finite_impl() override;
      bool       is_quotient_obviously_infinite_impl() override;
      void       set_nr_generators_impl(size_t) override;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (validation) - private
      ////////////////////////////////////////////////////////////////////////

      void validate_table(Table const&, size_t const, size_t const) const;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (initialisation) - private
      ////////////////////////////////////////////////////////////////////////

      void copy_relations_for_quotient(ToddCoxeter&);
      void init();
      void init_felsch_tree();
      void init_preimages_from_table();
      void prefill(FroidurePinBase&);
      void prefill_and_validate(Table const&, bool);
      void reverse_if_necessary_and_push_back(word_type,
                                              std::vector<word_type>&);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (cosets) - private
      ////////////////////////////////////////////////////////////////////////

      coset_type new_coset();
      void       remove_preimage(coset_type const,
                                 letter_type const,
                                 coset_type const);

      void make_deductions_dfs(coset_type const);
      void process_deductions();

      inline coset_type tau(coset_type const c, letter_type const a) const
          noexcept {
        LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
        LIBSEMIGROUPS_ASSERT(a < _table.nr_cols());
        return _table.get(c, a);
      }

      template <typename TIteratorType>
      coset_type tau(coset_type    c,
                     TIteratorType first,
                     TIteratorType last) const noexcept {
        LIBSEMIGROUPS_ASSERT(is_valid_coset(c) || c == UNDEFINED);
        for (auto it = first; it < last && c != UNDEFINED; ++it) {
          c = _table.get(c, *it);
          LIBSEMIGROUPS_ASSERT(is_valid_coset(c) || c == UNDEFINED);
        }
        return c;
      }

      template <typename TStackDeduct>
      coset_type
      tau_and_define_if_necessary(coset_type                c,
                                  word_type::const_iterator first,
                                  word_type::const_iterator last) noexcept {
        for (auto it = first; it < last; ++it) {
          coset_type d = tau(c, *it);
          if (d == UNDEFINED) {
            d = new_coset();
            define<TStackDeduct>(c, *it, d);
          }
          c = d;
        }
        return c;
      }

      template <typename TStackDeduct, typename TProcessCoincide>
      void push_definition_hlt(coset_type const& c,
                               word_type const&  u,
                               word_type const&  v) noexcept {
        REPORT_VERBOSE_DEFAULT("pushing coset %d through %s = %s\n",
                               c,
                               detail::to_string(u),
                               detail::to_string(v));

        LIBSEMIGROUPS_ASSERT(is_active_coset(c));
        LIBSEMIGROUPS_ASSERT(!u.empty());
        LIBSEMIGROUPS_ASSERT(!v.empty());
        coset_type const x = tau_and_define_if_necessary<TStackDeduct>(
            c, u.cbegin(), u.cend() - 1);
        coset_type const y = tau_and_define_if_necessary<TStackDeduct>(
            c, v.cbegin(), v.cend() - 1);
        letter_type const a  = u.back();
        letter_type const b  = v.back();
        coset_type const  xa = tau(x, a);
        coset_type const  yb = tau(y, b);

        if (xa == UNDEFINED && yb == UNDEFINED) {
          coset_type d = new_coset();
          define<TStackDeduct>(x, a, d);
          if (a != b || x != y) {
            define<TStackDeduct>(y, b, d);
          }
        } else if (xa == UNDEFINED && yb != UNDEFINED) {
          // tau(x, a) <- yb
          REPORT_VERBOSE_DEFAULT("deducing tau(%d, %d) = %d ...\n", x, a, yb);
          define<TStackDeduct>(x, a, yb);
        } else if (xa != UNDEFINED && yb == UNDEFINED) {
          // tau(y, b) <- xa
          REPORT_VERBOSE_DEFAULT("deducing tau(%d, %d) = %d ...\n", y, b, xa);
          define<TStackDeduct>(y, b, xa);
        } else if (xa != UNDEFINED && yb != UNDEFINED && xa != yb) {
          // tau(x, a) and tau(y, b) are defined and not equal
          _coinc.emplace(xa, yb);
          TProcessCoincide().template operator()<TStackDeduct>(this);
        }
      }

      template <typename TStackDeduct, typename TProcessCoincide>
      void push_definition_felsch(coset_type const& c,
                                  word_type const&  u,
                                  word_type const&  v) noexcept {
        REPORT_VERBOSE_DEFAULT("pushing coset %d through %s = %s\n",
                               c,
                               detail::to_string(u),
                               detail::to_string(v));
        LIBSEMIGROUPS_ASSERT(is_active_coset(c));
        LIBSEMIGROUPS_ASSERT(!u.empty());
        LIBSEMIGROUPS_ASSERT(!v.empty());
        class_index_type x = tau(c, u.cbegin(), u.cend() - 1);
        if (x == UNDEFINED) {
          return;
        }
        LIBSEMIGROUPS_ASSERT(is_valid_coset(x));
        class_index_type y = tau(c, v.cbegin(), v.cend() - 1);
        if (y == UNDEFINED) {
          return;
        }
        LIBSEMIGROUPS_ASSERT(is_valid_coset(y));
        letter_type const      a  = u.back();
        letter_type const      b  = v.back();
        class_index_type const xa = tau(x, a);
        class_index_type const yb = tau(y, b);

        if (xa == UNDEFINED && yb != UNDEFINED) {
          // tau(x, a) <- yb
          REPORT_VERBOSE_DEFAULT("deducing tau(%d, %d) = %d ...\n", x, a, yb);
          define<TStackDeduct>(x, a, yb);
        } else if (xa != UNDEFINED && yb == UNDEFINED) {
          // tau(y, b) <- xa
          REPORT_VERBOSE_DEFAULT("deducing tau(%d, %d) = %d ...\n", y, b, xa);
          define<TStackDeduct>(y, b, xa);
        } else if (xa != UNDEFINED && yb != UNDEFINED && xa != yb) {
          // tau(x, a) and tau(y, b) are defined and not equal
          _coinc.emplace(xa, yb);
          TProcessCoincide().template operator()<TStackDeduct>(this);
        }
      }

      template <typename TStackDeduct>
      void process_coincidences() {
#ifdef LIBSEMIGROUPS_VERBOSE
        if (!_coinc.empty()) {
          REPORT_VERBOSE_DEFAULT("processing %llu coincidences...\n",
                                 _coinc.size());
        }
#endif
        while (!_coinc.empty()) {
          Coincidence c = _coinc.top();
          _coinc.pop();
          coset_type min = find_coset(c.first);
          coset_type max = find_coset(c.second);
          if (min != max) {
            if (min > max) {
              std::swap(min, max);
            }
            union_cosets(min, max);

            size_t const n = _table.nr_cols();
            for (letter_type i = 0; i < n; ++i) {
              // Let <v> be the first PREIMAGE of <max>
              coset_type v = _preim_init.get(max, i);
              while (v != UNDEFINED) {
                v = define<TStackDeduct>(v, i, min);
                // TODO(later) are there possibly duplicates in the preimages,
                // if so, then we should not add further duplicates to the
                // preimages of min, we can check this by checking if tau(v, i)
                // == min already.
              }

              // Now let <v> be the IMAGE of <max>
              v = _table.get(max, i);
              if (v != UNDEFINED) {
                remove_preimage(v, i, max);
                // Let <u> be the image of <min>, and ensure <u> = <v>
                coset_type u = _table.get(min, i);
                if (u == UNDEFINED) {
                  define<TStackDeduct>(min, i, v);
                } else if (u != v) {
                  // Add (u,v) to the stack of pairs to be identified
                  _coinc.emplace(u, v);
                }
              }
            }
          }
        }
      }

      // Add d to the list of preimages of c under x, i.e. _table[d][x] = c
      inline void add_preimage(coset_type const  c,
                               letter_type const x,
                               coset_type const  d) noexcept {
        LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
        LIBSEMIGROUPS_ASSERT(x < nr_generators());
        LIBSEMIGROUPS_ASSERT(is_valid_coset(d));
        // c -> e -> ... -->  c -> d -> e -> ..
        _preim_next.set(d, x, _preim_init.get(c, x));
        _preim_init.set(c, x, d);
      }

      template <typename TStackDeduct>
      inline coset_type define(coset_type const  c,
                               letter_type const x,
                               coset_type const  d) noexcept {
        LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
        LIBSEMIGROUPS_ASSERT(x < nr_generators());
        LIBSEMIGROUPS_ASSERT(is_valid_coset(d));
        TStackDeduct()(_deduct, c, x);
        _table.set(c, x, d);
        coset_type e = _preim_next.get(c, x);
        add_preimage(d, x, c);
        return e;
      }

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (main strategies) - private
      ////////////////////////////////////////////////////////////////////////

      void felsch();
      void hlt();
      void sims();

      void perform_lookahead();

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (standardize) - private
      ////////////////////////////////////////////////////////////////////////

      void init_standardize();
      bool standardize_immediate(coset_type const,
                                 coset_type&,
                                 letter_type const);

      bool standardize_deferred(std::vector<coset_type>&,
                                std::vector<coset_type>&,
                                coset_type const,
                                coset_type&,
                                letter_type const);

      void lex_standardize();
      void recursive_standardize();
      void shortlex_standardize();

      void apply_permutation(std::vector<coset_type>&,
                             std::vector<coset_type>&);
      void swap(coset_type const, coset_type const);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (debug) - private
      ////////////////////////////////////////////////////////////////////////

#ifdef LIBSEMIGROUPS_DEBUG
      void debug_validate_table() const;
      void debug_validate_preimages() const;
      void debug_verify_no_missing_deductions() const;
#endif

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - inner classes - private
      ////////////////////////////////////////////////////////////////////////

      class FelschTree;                   // Forward declaration
      struct Settings;                    // Forward declaration
      friend struct ProcessCoincidences;  // Forward declaration
      struct TreeNode;                    // Forward declaration

      struct DerefNormalForm {
        word_type operator()(ToddCoxeter*                              tc,
                             IntegralRange<coset_type>::const_iterator it) {
          return tc->class_index_to_word(*it);
        }
      };

      struct AddressOfNormalForm {
        word_type* operator()(ToddCoxeter*,
                              IntegralRange<coset_type>::const_iterator) {
          LIBSEMIGROUPS_ASSERT(false);
          return nullptr;
        }
      };

      struct NormalFormIteratorTraits
          : detail::ConstIteratorTraits<IntegralRange<coset_type>> {
        using value_type      = word_type;
        using const_reference = word_type const;
        using reference       = word_type;
        using const_pointer   = word_type const*;
        using pointer         = word_type*;

        using state_type = ToddCoxeter*;
        using Deref      = DerefNormalForm;
        using AddressOf  = AddressOfNormalForm;
      };

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - aliases - private
      ////////////////////////////////////////////////////////////////////////

      using Coincidence = std::pair<coset_type, coset_type>;
      using Deduction   = std::pair<coset_type, letter_type>;
      using Tree        = std::vector<TreeNode>;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - enums - private
      ////////////////////////////////////////////////////////////////////////

      enum class state {
        constructed = 0,
        initialized,
        hlt,
        felsch,
        lookahead,
        finished
      };

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - data - private
      ////////////////////////////////////////////////////////////////////////

      std::stack<Coincidence>     _coinc;
      std::stack<Deduction>       _deduct;
      std::vector<word_type>      _extra;
      std::unique_ptr<FelschTree> _felsch_tree;
      size_t                      _nr_pairs_added_earlier;
      bool                        _prefilled;
      Table                       _preim_init;
      Table                       _preim_next;
      std::vector<word_type>      _relations;
      std::unique_ptr<Settings>   _settings;
      order                       _standardized;
      state                       _state;
      Table                       _table;
      std::unique_ptr<Tree>       _tree;
    };

  }  // namespace congruence
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_TODD_COXETER_HPP_
