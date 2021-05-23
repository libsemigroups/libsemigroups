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

#ifndef LIBSEMIGROUPS_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_HPP_

#include <chrono>      // for chrono::nanoseconds
#include <cstddef>     // for size_t
#include <functional>  // for function
#include <memory>      // for shared_ptr
#include <numeric>     // for std::iota
#include <stack>       // for stack
#include <utility>     // for pair
#include <vector>      // for vector

#include "cong-intf.hpp"   // for congruence_type,...
#include "cong-wrap.hpp"   // for CongruenceWrapper
#include "containers.hpp"  // for DynamicArray2
#include "coset.hpp"       // for CosetManager
#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "int-range.hpp"   // for IntegralRange
#include "iterator.hpp"    // for ConstIteratorStateful
#include "order.hpp"       // shortlex_compare
#include "report.hpp"      // for REPORT
#include "string.hpp"      // for to_string
#include "types.hpp"       // for word_type, letter_type...

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
    //! congruence::ToddCoxeter, and related things in
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
    //! tc.set_number_of_generators(2);                // 2 generators
    //! tc.add_pair({0, 0}, {0});               // generator 0 squared is itself
    //! tc.add_pair({0}, {1});                  // generator 0 equals 1
    //! tc.strategy(options::strategy::felsch);  // set the strategy
    //! tc.number_of_classes();                        // calculate number of
    //! classes tc.contains({0, 0, 0, 0}, {0, 0});      // check if 2 words are
    //! equal tc.word_to_class_index({0, 0, 0, 0});   // get the index of a
    //! class tc.standardize(order::lex);             // standardize to lex
    //! order \endcode
    //!
    //! \par Example 2
    //! \code
    //! ToddCoxeter tc(congruence_type::twosided);
    //! tc.set_number_of_generators(4);
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
    //! tc.strategy(options::strategy::hlt)
    //!    .standardize(false)
    //!    .lookahead(options::lookahead::partial)
    //!    .save(false)
    //! tc.number_of_classes()  // 10752
    //! tc.complete();   // true
    //! tc.compatible(); // true
    //! auto& S = tc.quotient_semigroup();  // FroidurePin<TCE>
    //! S.size()                            // 10752
    //! S.number_of_idempotents()                  // 1
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
     public:
      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - typedefs + enums - public
      ////////////////////////////////////////////////////////////////////////

      //! Type of the underlying table.
      //!
      //! This is the type of the coset table stored inside a ToddCoxeter
      //! instance.
      using table_type = detail::DynamicArray2<class_index_type>;

      //! Type of the indices of cosets.
      using coset_type = CongruenceInterface::class_index_type;

      //! Holds values of various options.
      //!
      //! This struct holds various enums which effect the coset enumeration
      //! process used by \ref run.
      //!
      //! \sa \ref strategy, \ref lookahead, and \ref froidure_pin.
      struct options {
        //! Values for defining the strategy.
        //!
        //! The values in this enum can be used as the argument for the member
        //! function strategy(options::strategy) to specify which strategy
        //! should be used when performing a coset enumeration.
        enum class strategy {
          //! This value indicates that the HLT (Hazelgrove-Leech-Trotter)
          //! strategy should be used. This is analogous to ACE's R-style.
          hlt,
          //! This value indicates that the Felsch strategy should be used.
          //! This is analogous to ACE's C-style.
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
          //! of time specified by the setting random_interval(T).
          random
        };

        //! Values for specifying the type of lookahead to perform.
        //!
        //! The values in this enum can be used as the argument for
        //! lookahead(options::lookahead) to specify the type of lookahead that
        //! should be performed when using the HLT strategy.
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

        //! Values for specifying whether to use relations or Cayley graph.
        //!
        //! The values in this enum can be used as the argument for
        //! \ref froidure_pin_policy(options::froidure_pin) to specify whether
        //! the defining relations, or the left/right Cayley graph, of a
        //! FroidurePin instance, should be used in the coset enumeration.
        //!
        //! If the number of classes in the congruence represented by a
        //! ToddCoxeter instance is relatively small, by some definition,
        //! compared to the size of the semigroup represented by the
        //! FroidurePin instance, then the  froidure_pin::use_relations
        //! policy is often faster. If the number of classes is relatively
        //! large, then  froidure_pin::use_cayley_graph is often faster. It
        //! is guaranteed that  run will terminate in an amount of time
        //! proportionate to the size of the input if the policy
        //! froidure_pin::use_cayley_graph is used, whereas the run time when
        //! using the policy froidure_pin::use_relations can be arbitrarily
        //! high regardless of the size of the input.
        enum class froidure_pin {
          //! No policy has been specified.
          none,
          //! Use the relations of a FroidurePin instance
          use_relations,
          //! Use the left or right Cayley graph of a FroidurePin instance
          use_cayley_graph
        };
      };

      //! The possible arguments for standardize(order).
      //!
      //! The values in this enum can be used as the argument for
      //! standardize(order) to specify which ordering should be used.  The
      //! normal forms for congruence classes are given with respect to one of
      //! the orders specified by the values in this enum.
      enum class order {
        //! No standardization has been done.
        none = 0,
        //! Normal forms are the short-lex least word belonging to a given
        //! congruence class.
        shortlex,
        //! Normal forms are the lexicographical least word belonging to a given
        //! congruence class.
        lex,  // TODO(now) does this even make sense?
        //! Normal forms are the recursive-path least word belonging to a given
        //! congruence class.
        recursive
        // wreath TODO(later)
      };

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - constructors and destructor - public
      ////////////////////////////////////////////////////////////////////////

      //! Construct from kind (left/right/2-sided).
      //!
      //! This constructor creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence specified by the given
      //! \ref congruence_type.
      //!
      //! \param knd the handedness (left/right/2-sided) of the congruence
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      explicit ToddCoxeter(congruence_type knd);

      //! Construct from kind (left/right/2-sided) and FroidurePin or
      //! FpSemigroupInterface.
      //!
      //! This constructor creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence over the semigroup
      //! represented by a FroidurePin object or instance of a class derived
      //! from FpSemigroupInterface.
      //!
      //! \tparam T a type derived from FroidurePinBase or FpSemigroupInterface
      //!
      //! \param knd the handedness (left/right/2-sided) of the congruence
      //! \param S the underlying semigroup of the congruence.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \warning The parameter \p S is copied, this might be expensive, use a
      //! \shared_ptr to avoid the copy!
      template <typename T>
      ToddCoxeter(congruence_type knd, T const& S) : ToddCoxeter(knd) {
        static_assert(std::is_base_of<FroidurePinBase, T>::value
                          || std::is_base_of<FpSemigroupInterface, T>::value,
                      "the template parameter must be a derived class of "
                      "FroidurePinBase or FpSemigroupInterface");
        set_parent_froidure_pin(S);
        set_number_of_generators(S.number_of_generators());
      }

      //! Construct from kind (left/right/2-sided), shared pointer to
      //! FroidurePinBase, and options.
      //!
      //! This constructor creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence over the semigroup
      //! represented by a shared pointer to a FroidurePin object.
      //!
      //! \param knd the type of the congruence being constructed
      //! \param fp a \shared_ptr
      //! to a FroidurePinBase, this parameter is not copied by this
      //! constructor.
      //! \param p the options::froidure_pin to use (default:
      //! options::froidure_pin::use_cayley_graph).
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      ToddCoxeter(congruence_type                  knd,
                  std::shared_ptr<FroidurePinBase> fp,
                  options::froidure_pin            p
                  = options::froidure_pin::use_cayley_graph);
      // TODO(later) remove the final argument here
      // This options::froidure_pin is guaranteed to terminate relatively
      // quickly

      //! Construct from kind (left/right/2-sided) and ToddCoxeter.
      //!
      //! This constructor creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence over the quotient semigroup
      //! represented by a ToddCoxeter instance.
      //!
      //! \param knd the handedness (left/right/2-sided) of the congruence
      //! \param tc the ToddCoxeter representing the underlying semigroup
      //!
      //! \throws LibsemigroupsException if \p tc is a left, or right,
      //! congruence, and \p knd is not left, or not right, respectively.
      ToddCoxeter(congruence_type knd, ToddCoxeter& tc);

      //! Construct from kind (left/right/2-sided) and ToddCoxeter.
      //!
      //! This constructor creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence over the semigroup
      //! represented by a libsemigroups::fpsemigroup::ToddCoxeter instance.
      //!
      //! \param knd the handedness (left/right/2-sided) of the congruence
      //! \param tc the ToddCoxeter representing the underlying semigroup
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      ToddCoxeter(congruence_type knd, fpsemigroup::ToddCoxeter& tc);

      //! Construct from kind (left/right/2-sided) and KnuthBendix.
      //!
      //! A constructor that creates a new ToddCoxeter instance representing a
      //! left, right, or two-sided congruence over the semigroup
      //! represented by a fpsemigroup::KnuthBendix instance.
      //! \param knd the handedness (left/right/2-sided) of the congruence
      //! \param kb the KnuthBendix representing the underlying semigroup
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      ToddCoxeter(congruence_type knd, fpsemigroup::KnuthBendix& kb);

      //! Copy constructor.
      //!
      //! Constructs a complete copy of \p that, including all of the settings,
      //! table, defining relations, and generating pairs.
      //!
      //! \param that the ToddCoxeter instance to copy.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      ToddCoxeter(ToddCoxeter const& that);

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

      //! Prefill the coset table.
      //!
      //! This member function allows a ToddCoxeter instance to be prefilled
      //! with an existing coset table. The argument should represent the left
      //! or right Cayley graph of a finite semigroup.
      //!
      //! \param t the table
      //!
      //! \returns
      //! (None)
      //!
      //! \throws LibsemigroupsException if the table \p t is not valid.
      //!
      //! \complexity
      //! Linear in the total number of entries in the table \p t.
      void prefill(table_type const& t);

      // Settings

      //! Specify whether to use the relations or the Cayley graph.
      //!
      //! Sets whether to use the defining relations or the Cayley graph of the
      //! FroidurePin instance used to initialise the object.
      //!
      //! If the ToddCoxeter instance is not created from a FroidurePin
      //! instance, then the value of this setting is ignored.
      //! The default value is options::froidure_pin::use_cayley_graph.
      //!
      //! \param val value indicating whether to use relations or Cayley graph
      //! (options::froidure_pin::use_cayley_graph or
      //!  options::froidure_pin::use_relations).
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& froidure_pin_policy(options::froidure_pin val) noexcept;

      //! Get the current value of the Froidure-Pin policy.
      //!
      //! If the ToddCoxeter instance is not created from a FroidurePin
      //! instance, or from an object that has an already computed FroidurePin
      //! instance, then the value of this setting is ignored.
      //!
      //! \sa froidure_pin_policy(options::froidure_pin)
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns
      //! A value of type options::froidure_pin.
      //!
      //! \exceptions
      //! \noexcept
      options::froidure_pin froidure_pin_policy() const noexcept;

      //! Set the lookahead to use in HLT.
      //!
      //! If the strategy is not HLT, then the value of this setting is
      //! ignored.
      //!
      //! The default value is options::lookahead::partial, and the other
      //! possible value is options::lookahead::full.
      //!
      //! \param val value indicating whether to perform a full or partial
      //! lookahead.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa ToddCoxeter::options::lookahead.
      ToddCoxeter& lookahead(options::lookahead val) noexcept;

      //! Specify minimum number of classes that may trigger early stop.
      //!
      //! Set a lower bound for the number of classes of the congruence
      //! represented by a ToddCoxeter instance. If the number of active cosets
      //! becomes at least the value of the argument, and the table is complete
      //! (\ref complete returns \c true), then the coset enumeration is
      //! terminated. When the given bound is equal to the number of classes,
      //! this may save tracing relations at many cosets when there is no
      //! possibility of finding coincidences.
      //!
      //! The default value is \ref UNDEFINED.
      //!
      //! \param val value indicating the lower bound.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& lower_bound(size_t val) noexcept;

      //! Set the threshold that will trigger a lookahead in HLT.
      //!
      //! If the number of cosets active exceeds the value set by this
      //! function, then a lookahead, of the type set using the function
      //! \ref lookahead, is triggered. This only applies when using
      //! the HLT strategy.
      //!
      //! The default value is 5 million.
      //!
      //! \param val value indicating the initial threshold.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& next_lookahead(size_t val) noexcept;

      //! Process deductions during HLT.
      //!
      //! If the argument of this function is \c true and the HLT strategy is
      //! being used, then deductions are processed during the enumeration.
      //!
      //! The default value is \c false.
      //!
      //! \param val value indicating the initial threshold.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \ref prefill was used to
      //! initialise \c this.
      //!
      //! \throws LibsemigroupsException if the parent FroidurePin (if any) is
      //! finite, and the value of froidure_pin_policy() is not
      //! options::froidure_pin::use_relations.
      ToddCoxeter& save(bool val);  // NOLINT()

      //! Short-lex standardize the table during enumeration.
      //!
      //! If the argument of this function is \c true, then the coset table is
      //! standardized (according to the short-lex order) during the coset
      //! enumeration.
      //!
      //! The default value is \c false.
      //!
      //! \param val value indicating whether or not to standardize.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& standardize(bool val) noexcept;  // NOLINT()

      //! Specify the strategy.
      //!
      //! The strategy used during the coset enumeration can be specified using
      //! this function. It can be set to HLT, Felsch, or random.
      //!
      //! The default value is options::strategy::hlt.
      //!
      //! \param val value indicating which strategy to use, the possible
      //! values are:
      //! * options::strategy::hlt
      //! * options::strategy::felsch
      //! * options::strategy::random
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is options::strategy::felsch
      //! and any of the following conditions apply:
      //! * \ref prefill as used to initialise \c this
      //! * if the parent FroidurePin (if any) is
      //! finite, and the value of froidure_pin_policy() is not
      //! options::froidure_pin::use_relations.
      ToddCoxeter& strategy(options::strategy val);

      //! The current strategy for enumeration.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current strategy, a value of type options::strategy.
      //!
      //! \exceptions
      //! \noexcept
      options::strategy strategy() const noexcept;

      //! Set the amount of time per strategy for options::strategy::random.
      //!
      //! Sets the duration in nanoseconds that a given randomly selected
      //! strategy will run for, when using the random strategy
      //! (options::strategy::random).
      //!
      //! The default value is 200ms.
      //!
      //! \param val the number of nanoseconds used per strategy if the
      //! strategy is random.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& random_interval(std::chrono::nanoseconds val) noexcept;

      //! Set the amount of time per strategy for options::strategy::random.
      //!
      //! Sets the duration (by converting to nanoseconds) that a given
      //! randomly selected strategy will run for, when using the random
      //! strategy (options::strategy::random).
      //!
      //! The default value is 200ms.
      //!
      //! \tparam T \chrono_duration
      //!
      //! \param val the duration per strategy if the strategy is random.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      template <typename T>
      ToddCoxeter& random_interval(T val) noexcept {
        return random_interval(std::chrono::nanoseconds(val));
      }

      //! Type of the argument to \ref sort_generating_pairs.
      //!
      //! A type alias for functions that can be used as an argument to
      //! \ref sort_generating_pairs.
      // This alias only really exists to make the documentation work :(
      using sort_function_type
          = std::function<bool(word_type const&, word_type const&)>;

      //! Sort generating pairs.
      //!
      //! Sorts all existing generating pairs according to the binary function
      //! \p func.  Additionally, if \c this was defined over a finitely
      //! presented semigroup, then the copy of the defining relations of that
      //! semigroup contained in \c this (if any) are also sorted according to
      //! \p func.
      //!
      //! \param func a value of type \ref sort_function_type
      //! that defines a linear order on the relations in a
      //! ToddCoxeter instance.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if started() returns \c true.
      //!
      //! \warning
      //! If \ref add_pair is called after this function, then it may no longer
      //! be the case that the defining relations and generating pairs of \c
      //! this are sorted by \p func.
      //!
      //! \sa
      //! random_shuffle_generating_pairs
      ToddCoxeter& sort_generating_pairs(sort_function_type func);

      //! Type of the argument to \ref sort_generating_pairs.
      //!
      //! A type alias for free functions that can be used as an argument to
      //! \ref sort_generating_pairs.
      // This alias only really exists to make the documentation work :(
      using sort_free_function_type = bool(word_type const&, word_type const&);

      //! \copydoc sort_generating_pairs(sort_function_type)
      ToddCoxeter& sort_generating_pairs(sort_free_function_type func
                                         = shortlex_compare) {
        return sort_generating_pairs(sort_function_type(func));
      }

      //! Randomly shuffle the generating pairs.
      //!
      //! Additionally, if \c this was defined over a finitely presented
      //! semigroup, then the copy of the defining relations of that semigroup
      //! contained in \c this (if any) are also sorted according to \p func.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if started() returns \c true.
      //!
      //! \parameters
      //! (None)
      ToddCoxeter& random_shuffle_generating_pairs();

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (container-like) - public
      ////////////////////////////////////////////////////////////////////////

      //! Check if there are no relations or generating pairs.
      //!
      //! Returns \c true if there are no relations or generating pairs in the
      //! ToddCoxeter instance, and the number of active cosets is \c 1 (the
      //! minimum possible).
      //!
      //! \returns A value of type \c bool.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \parameters
      //! (None)
      bool empty() const;

      //! Reserve the specified capacity in the coset table.
      //!
      //! Reserves the capacity specified by the argument in the data
      //! structures for cosets used in a ToddCoxeter instance.
      //!
      //! \param val the capacity to reserve.
      //!
      //! \returns
      //! (None)
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      void reserve(size_t val);

      //! Release unused memory if \ref finished.
      //!
      //! Release all memory used to store free cosets, and any other
      //! unnecessary data if the enumeration is \ref finished. Otherwise, it
      //! does nothing.
      //!
      //! \returns
      //! (None)
      //!
      //! \parameters
      //! (None)
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      void shrink_to_fit();

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (state) - public
      ////////////////////////////////////////////////////////////////////////

      //! Check if the table is complete.
      //!
      //! Returns \c true if the coset table is complete, and \c false if it is
      //! not. The table is *complete* if the value \ref UNDEFINED
      //! does not appear in any row of an active coset.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \parameters
      //! (None)
      //!
      //! \exceptions
      //! \noexcept
      bool complete() const noexcept;

      //! Check if the table is compatible with the relations.
      //!
      //! Returns \c true if the coset table is compatible with the relations
      //! and generating pairs used to create \c this, and \c false if it is
      //! not. The table is *compatible* if the values obtained by pushing
      //! a coset through the left-hand side and the right-hand side of
      //! a relation coincide for every coset and every relation.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \parameters
      //! (None)
      //!
      //! \exceptions
      //! \noexcept
      bool compatible() const noexcept;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (standardization) - public
      ////////////////////////////////////////////////////////////////////////

      //! Check if the table has been standardized.
      //!
      //! Returns \c true if the ToddCoxeter instance is standardized. In other
      //! words, if standardize(order) has been called with any argument other
      //! than order::none.
      //!
      //! \returns A value of type \c bool.
      //!
      //! \parameters
      //! (None)
      //!
      //! \exceptions
      //! \noexcept
      bool is_standardized() const noexcept;

      //! Standardize the table according to the specified order.
      //!
      //! Standardizes the current coset table according to the order specified
      //! by \ref order.
      //!
      //! \param val the order used for standardization, the possible values
      //! are:
      //! * order::shortlex
      //! * order::lex
      //! * order::recursive
      //!
      //! \returns
      //! (None)
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      void standardize(order val);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - iterators - public
      ////////////////////////////////////////////////////////////////////////

      //! No doc
      struct NormalFormIteratorTraits
          : detail::ConstIteratorTraits<IntegralRange<coset_type>> {
        using value_type      = word_type;
        using const_reference = value_type const;
        using reference       = value_type;
        using const_pointer   = value_type const*;
        using pointer         = value_type*;

        using state_type = ToddCoxeter*;

        struct Deref {
          value_type operator()(state_type                                tc,
                                IntegralRange<coset_type>::const_iterator it) {
            return tc->class_index_to_word(*it);
          }
        };

        struct AddressOf {
          pointer operator()(state_type,
                             IntegralRange<coset_type>::const_iterator) {
            LIBSEMIGROUPS_ASSERT(false);
            return nullptr;
          }
        };
      };

      //! The type of a const iterator pointing to a normal form.
      //!
      //! Iterators of this type point to a \ref word_type.
      //!
      //! \sa cbegin_normal_forms, cend_normal_forms.
      using normal_form_iterator
          = detail::ConstIteratorStateful<NormalFormIteratorTraits>;

      //! Returns a \ref normal_form_iterator pointing at the first normal
      //! form.
      //!
      //! Returns a const iterator pointing to the normal form of the first
      //! class of the congruence represented by an instance of ToddCoxeter.
      //! The order of the classes, and the normal form, that is returned are
      //! controlled by standardize(order).
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type \ref normal_form_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      normal_form_iterator cbegin_normal_forms() {
        auto range = IntegralRange<coset_type>(0, number_of_classes());
        return normal_form_iterator(this, range.cbegin());
      }

      //! Returns a \ref normal_form_iterator pointing one past the last normal
      //! form.
      //!
      //! Returns a const iterator one past the normal form of the last class
      //! of the congruence represented by an instance of ToddCoxeter. The
      //! order of the classes, and the normal form, that is returned are
      //! controlled by standardize(order).
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type \ref normal_form_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      normal_form_iterator cend_normal_forms() {
        auto range = IntegralRange<coset_type>(0, number_of_classes());
        return normal_form_iterator(this, range.cend());
      }

     private:
      void run_impl() override;
      bool finished_impl() const override;

      ////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////

      word_type class_index_to_word_impl(coset_type) override;
      size_t    number_of_classes_impl() override;
      // Guaranteed to return a FroidurePin<TCE>.
      std::shared_ptr<FroidurePinBase> quotient_impl() override;
      coset_type word_to_class_index_impl(word_type const&) override;

      ////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////

      coset_type const_word_to_class_index(word_type const&) const override;
      bool       is_quotient_obviously_finite_impl() override;
      bool       is_quotient_obviously_infinite_impl() override;
      void       set_number_of_generators_impl(size_t) override;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (validation) - private
      ////////////////////////////////////////////////////////////////////////

      void validate_table(table_type const&, size_t const, size_t const) const;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (initialisation) - private
      ////////////////////////////////////////////////////////////////////////

      void copy_relations_for_quotient(ToddCoxeter&);
      void init();
      void init_felsch_tree();
      void init_preimages_from_table();
      void prefill(FroidurePinBase&);
      void prefill_and_validate(table_type const&, bool);
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

      inline coset_type tau(coset_type const  c,
                            letter_type const a) const noexcept {
        LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
        LIBSEMIGROUPS_ASSERT(a < _table.number_of_cols());
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
                               detail::to_string(u).c_str(),
                               detail::to_string(v).c_str());

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
                               detail::to_string(u).c_str(),
                               detail::to_string(v).c_str());
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
                                 static_cast<uint64_t>(_coinc.size()));
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

            size_t const n = _table.number_of_cols();
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
        LIBSEMIGROUPS_ASSERT(x < number_of_generators());
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
        LIBSEMIGROUPS_ASSERT(x < number_of_generators());
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
      table_type                  _preim_init;
      table_type                  _preim_next;
      std::vector<word_type>      _relations;
      std::unique_ptr<Settings>   _settings;
      order                       _standardized;
      state                       _state;
      table_type                  _table;
      std::unique_ptr<Tree>       _tree;
    };

  }  // namespace congruence
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TODD_COXETER_HPP_
