//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-21 James D. Mitchell
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

#include <chrono>       // for chrono::nanoseconds
#include <cstddef>      // for size_t
#include <cstdint>      // for uint64_t, int64_t
#include <functional>   // for function
#include <memory>       // for shared_ptr
#include <stack>        // for stack
#include <string>       // for string
#include <type_traits>  // for is_base_of
#include <utility>      // for pair
#include <vector>       // for vector

#include "cong-intf.hpp"             // for congruence_kind,...
#include "cong-wrap.hpp"             // for CongruenceWrapper
#include "constants.hpp"             // for UNDEFINED
#include "containers.hpp"            // for DynamicArray2
#include "coset.hpp"                 // for CosetManager
#include "debug.hpp"                 // for LIBSEMIGROUPS_ASSERT
#include "digraph-with-sources.hpp"  // for DigraphWithSources
#include "froidure-pin.hpp"          // for FroidurePin
#include "int-range.hpp"             // for IntegralRange
#include "iterator.hpp"              // for ConstIteratorStateful
#include "order.hpp"                 // shortlex_compare
#include "types.hpp"                 // for word_type, letter_type...

namespace libsemigroups {
  // Forward declarations
  namespace detail {
    class TCE;
    class Timer;
    class FelschTree;
  }  // namespace detail
  class FroidurePinBase;

  namespace congruence {
    class ToddCoxeter;
  }
  class FpSemigroupInterface;  // forward decl

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
    //! Some of the features of this class were inspired by similar features in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) by George Havas and Colin
    //! Ramsay.
    //!
    //! \sa congruence_kind and tril.
    //!
    //! \par Example 1
    //! \code
    //! ToddCoxeter tc(congruence_kind::left);  // construct a left congruence
    //! tc.set_number_of_generators(2);         // 2 generators
    //! tc.add_pair({0, 0}, {0});               // generator 0 squared is itself
    //! tc.add_pair({0}, {1});                  // generator 0 equals 1
    //! tc.strategy(options::strategy::felsch); // set the strategy
    //! tc.number_of_classes();
    //! tc.contains({0, 0, 0, 0}, {0, 0});
    //! equal tc.word_to_class_index({0, 0, 0, 0});
    //! tc.standardize(order::lex);
    //! \endcode
    //!
    //! \par Example 2
    //! \code
    //! ToddCoxeter tc(congruence_kind::twosided);
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
      struct Stats;  // forward decl
      using Perm = typename CosetManager::Perm;

     public:
      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - typedefs + enums - public
      ////////////////////////////////////////////////////////////////////////

      //! Type of the underlying table.
      //!
      //! This is the type of the coset table stored inside a ToddCoxeter
      //! instance.
      using table_type = detail::DynamicArray2<coset_type>;

      //! The type of the return value of quotient_froidure_pin().
      //!
      //! quotient_froidure_pin() returns a \shared_ptr to a FroidurePinBase,
      //! which is really of type \ref froidure_pin_type.
      using froidure_pin_type
          = FroidurePin<detail::TCE,
                        FroidurePinTraits<detail::TCE, table_type>>;

      //! Holds values of various options.
      //!
      //! This struct holds various enums which effect the coset enumeration
      //! process used by \ref run.
      //!
      //! \sa \ref strategy, \ref lookahead, \ref deductions, \ref
      //! preferred_defs, and \ref froidure_pin.
      struct options {
        //! Values for defining the strategy.
        //!
        //! The values in this enum can be used as the argument for the member
        //! function strategy(options::strategy) to specify which strategy
        //! should be used when performing a coset enumeration.
        //!
        //! Several of the strategies mimic
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies of the same
        //! name. The [ACE](https://staff.itee.uq.edu.au/havas/) strategy \"R*\"
        //! is equivalent to \c strategy(options::strategy::hlt).save(true).
        enum class strategy {
          //! This value indicates that the HLT (Hazelgrove-Leech-Trotter)
          //! strategy should be used. This is analogous to
          //! [ACE](https://staff.itee.uq.edu.au/havas/)'s R-style.
          hlt,
          //! This value indicates that the Felsch strategy should be used.
          //! This is analogous to [ACE](https://staff.itee.uq.edu.au/havas/)'s
          //! C-style.
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
          random,
          //! This strategy is meant to mimic the
          //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy of the same
          //! name. The Felsch is run until at least f_defs() nodes are
          //! defined, then the HLT strategy is run until at least hlt_defs()
          //! divided by length_of_generating_pairs() nodes have been defined.
          //! These steps are repeated until the enumeration terminates.
          CR,
          //! This strategy is meant to mimic the
          //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy R/C. The HLT
          //! strategy is run until the first lookahead is triggered (when
          //! number_of_cosets_active() is at least next_lookhead()). A full
          //! lookahead is then performed, and then the CR strategy is used.
          R_over_C,
          //! This strategy is meant to mimic the
          //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Cr. The Felsch
          //! strategy is run until at least f_defs() new nodes have been
          //! defined, the HLT strategy is then run until at least hlt_defs()
          //! divided by length_of_generating_pairs() new nodes are defined,
          //! and then the Felsch strategy is run.
          Cr,
          //! This strategy is meant to mimic the
          //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Rc. The HLT
          //! strategy is run until at least hlt_defs() divided by
          //! length_of_generating_pairs() new nodes have been
          //! defined, the Felsch strategy is then run until at least f_defs()
          //! new nodes are defined, and then the HLT strategy is run.
          Rc
        };

        //! Values for specifying the type of lookahead to perform.
        //!
        //! The values in this enum can be used as the argument for
        //! lookahead(options::lookahead) to specify the type of lookahead that
        //! should be performed when using the HLT strategy.
        //!
        //! It is possible to combine values of this type using operator|, for
        //! example a full HLT style lookahead is specified by
        //! ``options::lookahead::full | options::lookahead::hlt``.
        //!
        //! An exception will be thrown if incompatible values of
        //! options::lookahead are combined in this way, such as, for example
        //! ``options::lookahead::full | options::lookahead::partial``.
        enum class lookahead {
          //! A *full* lookahead is one starting from the initial coset.
          //! Full lookaheads are therefore sometimes slower but may
          //! detect more coincidences than a partial lookahead.
          full = 1,  // 00|01
          //! A *partial* lookahead is one starting from the current coset.
          //! Partial lookaheads are therefore sometimes faster but may not
          //! detect as many coincidences as a full lookahead.
          partial = 2,  // 00|10
          //! The lookahead will be done in HLT style by following the paths
          //! labelled by every relation from every coset in the range
          //! specified by lookahead::full or lookahead::partial.
          hlt = 4,  // 01|00
          //! The lookahead will be done in Felsch style where every edge is
          //! considered in every path labelled by a relation in which it
          //! occurs.
          felsch = 8  // 10|00
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
          //! Use the relations of a FroidurePin instance.
          use_relations,
          //! Use the left or right Cayley graph of a FroidurePin instance.
          use_cayley_graph
        };

        //! Values for specifying how to handle deductions.
        //!
        //! The values in this enum can be used as the argument for
        //! \ref deduction_policy(options::deductions).
        //!
        //! For our purposes, a *deduction* is a recently defined edge in the
        //! word graph that we are attempting to construct in an instance of
        //! ToddCoxeter. The values in this enum influence how these
        //! deductions are stored and processed.
        //!
        //! For every deduction held in the deduction stack, a depth first
        //! search through the Felsch tree of the generating pairs is
        //! performed. The aim is to only follow paths from nodes in the word
        //! graph labelled by generating pairs that actually pass through the
        //! edge described by a deduction. There are two versions of this
        //! represented by the values options::deductions::v1 and
        //! options::deductions::v2. The first version is simpler, but may
        //! involve following the same path that leads nowhere multiple times.
        //! The second version is more complex, and attempts to avoid following
        //! the same path multiple times if it is found to lead nowhere once.
        //!
        //! The other values in this enum represent what to do if the number of
        //! deductions in the stack exceeds the value max_deductions().
        //!
        //! It is possible to combine values of this type using operator|, for
        //! example options::deductions::v2 and options::deductions::unlimited
        //! is specified by ``options::deductions::v2 |
        //! options::deductions::unlimited``.
        //!
        //! An exception will be thrown if incompatible values of
        //! options::deductions are combined in this way, such as, for example
        //! ``options::deductions::v1 | options::deductions::v2``.
        enum class deductions {
          //! Version 1 deduction processing
          v1 = 1,  // 000|01
          //! Version 2 deduction processing
          v2 = 3,  // 000|11
          //! Do not put newly generated deductions in the stack if the stack
          //! already has size max_deductions().
          no_stack_if_no_space = 4,  // 001|00
          //! If the deduction stack has size max_deductions() and a new
          //! deduction is generated, then deductions with dead source node are
          //! are popped from the top of the stack (if any).
          purge_from_top = 8,  // 010|00
          //! If the deduction stack has size max_deductions() and a new
          //! deduction is generated, then deductions with dead source node are
          //! are popped from the entire of the stack (if any).
          purge_all = 12,  // 011|00
          //! If the deduction stack has size max_deductions() and a new
          //! deduction is generated, then all deductions in the stack are
          //! discarded.
          discard_all_if_no_space = 16,  // 100|00
          //! There is no limit to the number of deductions that can be put in
          //! the stack.
          unlimited = 20,  // 101|00
        };

        //! Values for specifying how to handle preferred definitions.
        //!
        //! The values in this enum can be used as the argument for
        //! \ref preferred_defs(options::preferred_defs).
        //!
        //! While in a Felsch phase of an enumeration, a definition of the next
        //! new edge is usually made for the first node whose out-degree is not
        //! equal to the number of generators. The exact order this happens is
        //! depends on the implementation and is not specified. When following
        //! the paths from a given node labelled by a relation it might be the
        //! case that both paths end one letter before the end. It might be
        //! beneficial for the next edges defined to be the missing edges from
        //! these paths, these are what we refer to as *preferred definitions*.
        //! The values in this enum influence how preferred definitions
        //! are utilised.
        //!
        //! The maximum number of preferred definitions held at any time is
        //! defined by the value of max_preferred_defs(). These definitions are
        //! stored in a circular buffer, where newer preferred definitions
        //! displace older ones once the number exceeds max_preferred_defs().
        //!
        //! \note The values in this enum roughly correspond to
        //! [ACE](https://staff.itee.uq.edu.au/havas/)'s \"pmode\" options.
        //!
        //! \warning
        //! If the option preferred_defs::deferred is used then the next edges
        //! defined are always taken from the preferred definitions circular
        //! buffer, regardless of the proportion of undefined edges in the word
        //! graph. In [ACE](https://staff.itee.uq.edu.au/havas/), preferred
        //! definitions are only made if the proportion of undefined edges is
        //! sufficiently low (or the
        //! \"fill factor\" is sufficiently high). This is not currently
        //! implemented in ``libsemigroups`` and there are examples where using
        //! preferred definitions causes an enumeration to run for longer than
        //! if they are not used.
        enum class preferred_defs {
          //! Do not use preferred definitions at all.
          none,
          //! Immediately define the new edge and do not stack the
          //! corresponding deductions.
          immediate_no_stack,
          //! Immediately define the new edge and do stack the
          //! corresponding deductions.
          immediate_yes_stack,
          //! Add the preferred definition to the preferred definition buffer.
          deferred
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
        //! The congruence classes are ordered lexicographically by their
        //! normal form. The normal forms themselves are essentially arbitrary
        //! because there is not necessarily a lexicographically least word in
        //! every class.
        lex,
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
      //! \ref congruence_kind.
      //!
      //! \param knd the handedness (left/right/2-sided) of the congruence
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      explicit ToddCoxeter(congruence_kind knd);

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
      ToddCoxeter(congruence_kind knd, T const& S) : ToddCoxeter(knd) {
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
      ToddCoxeter(congruence_kind                  knd,
                  std::shared_ptr<FroidurePinBase> fp,
                  options::froidure_pin            p
                  = options::froidure_pin::use_cayley_graph);
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
      ToddCoxeter(congruence_kind knd, ToddCoxeter& tc);

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
      ToddCoxeter(congruence_kind knd, fpsemigroup::ToddCoxeter& tc);

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
      ToddCoxeter(congruence_kind knd, fpsemigroup::KnuthBendix& kb);

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
      //! This function can be used to prefill a ToddCoxeter instance to be
      //! with an existing coset table. The argument should represent the left
      //! or right Cayley graph of a finite semigroup.
      //!
      //! \param table the table
      //!
      //! \returns
      //! (None)
      //!
      //! \throws LibsemigroupsException if the table \p table is not valid.
      //!
      //! \complexity
      //! Linear in the total number of entries in the table \p table.
      void prefill(table_type const& table) {
        prefill(table, [](size_t i) { return i; });
      }

      // Settings

      //! Specify whether to use the relations or the Cayley graph.
      //!
      //! Sets whether to use the defining relations or the Cayley graph of the
      //! FroidurePin instance used to initialise the object.
      //!
      //! If the ToddCoxeter instance is not created from a FroidurePin
      //! instance, then the value of this setting is ignored.
      //!
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

      //! The current value of the Froidure-Pin policy setting.
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

      //! Set the style of lookahead to use in HLT.
      //!
      //! If the strategy is not HLT, then the value of this setting is
      //! ignored.
      //!
      //! The default value is options::lookahead::partial |
      //! options::lookahead::hlt. The other
      //! possible value are documented in options::lookahead.
      //!
      //! \param val value indicating whether to perform a full or partial
      //! lookahead in HLT or Felsch style.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& lookahead(options::lookahead val) noexcept;

      //! The current value of the setting for lookaheads.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type options::lookahead.
      //!
      //! \exceptions
      //! \noexcept
      options::lookahead lookahead() const noexcept;

      //! Specify minimum number of classes that may trigger early stop.
      //!
      //! Set a lower bound for the number of classes of the congruence
      //! represented by a ToddCoxeter instance. If the number of active cosets
      //! becomes at least the value of the argument, and the table is complete
      //! (\ref complete returns \c true), then the enumeration is terminated.
      //! When the given bound is equal to the number of classes, this may save
      //! tracing relations at many cosets when there is no possibility of
      //! finding coincidences.
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

      //! The current value of the lower bound setting.
      //!
      //! \parameters
      //! None
      //!
      //! \returns A value of type `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa lower_bound(size_t)
      size_t lower_bound() const noexcept;

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

      //! The current value of the next lookahead setting.
      //!
      //! \parameters
      //! None
      //!
      //! \returns A value of type `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa next_lookahead(size_t)
      size_t next_lookahead() const noexcept;

      //! Set the minimum value of next_lookahead().
      //!
      //! After a lookahead is performed the value of next_lookahead() is
      //! modified depending on the outcome of the current lookahead. If the
      //! return value next_lookahead() of is too small or too large, then the
      //! value is adjusted according to lookahead_growth_factor() and
      //! lookahead_growth_threshold(). This setting specified the minimum
      //! possible value for next_lookahead().
      //!
      //! The default value is \c 10'000.
      //!
      //! \param val value indicating the minimum value of next_lookahead().
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& min_lookahead(size_t val) noexcept;

      //! The current value of the minimum lookahead setting.
      //!
      //! \parameters
      //! None
      //!
      //! \returns A value of type `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa min_lookahead(size_t)
      size_t min_lookahead() const noexcept;

      //! Set the lookahead growth factor.
      //!
      //! This setting determines by what factor the number of nodes required
      //! to trigger a lookahead grows. More specifically, at the end of any
      //! lookahead if the number of active nodes already exceeds the value of
      //! next_lookahead() or the number of nodes killed during the lookahead
      //! is less than the number of active nodes divided by
      //! lookahead_growth_threshold(), then the value of
      //! ToddCoxeter::next_lookhead is increased by a multiple of the \p value.
      //!
      //! \param val the value indicating the lookahead growth factor.
      //! The default value is ``2.0``.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is less than ``1.0``.
      ToddCoxeter& lookahead_growth_factor(float val);

      //! The current value of the lookahead growth factor.
      //!
      //! \parameters
      //! None
      //!
      //! \returns A value of type `float`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa lookahead_growth_factor(float)
      float lookahead_growth_factor() const noexcept;

      //! Set the lookahead growth threshold.
      //!
      //! This setting determines by what threshold for changing the number of
      //! nodes required to trigger a lookahead grows. More specifically, at
      //! the end of any lookahead if the number of active nodes already
      //! exceeds the value of next_lookahead() or the number of nodes killed
      //! during the lookahead is less than the number of active nodes divided
      //! by \ref lookahead_growth_threshold, then the value of
      //! ToddCoxeter::next_lookhead() is increased.
      //!
      //! The default value is ``4``.
      //!
      //! \param val the value indicating the lookahead growth threshold.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& lookahead_growth_threshold(size_t val) noexcept;

      //! The current value of the lookahead growth threshold.
      //!
      //! \parameters
      //! None
      //!
      //! \returns A value of type `size_t`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa lookahead_growth_threshold()
      size_t lookahead_growth_threshold() const noexcept;

      //! Process deductions during HLT.
      //!
      //! If the argument of this function is \c true and the HLT strategy is
      //! being used, then deductions are processed during the enumeration.
      //!
      //! The default value is \c false.
      //!
      //! \param val value indicating whether or not to process deductions.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \ref prefill was used to
      //! initialise \c this.
      //!
      //! \throws LibsemigroupsException if the parent FroidurePin (if any) is
      //! finite, and the value of froidure_pin_policy() is not
      //! options::froidure_pin::use_relations.
      ToddCoxeter& save(bool val);

      //! The current value of save setting.
      //!
      //! \parameters
      //! None
      //!
      //! \returns A value of type `bool`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa save(bool)
      bool save() const noexcept;

      //! Partially short-lex standardize the table during enumeration.
      //!
      //! If the argument of this function is \c true, then the coset table is
      //! partially standardized (according to the short-lex order) during the
      //! coset enumeration.
      //!
      //! The default value is \c false.
      //!
      //! \param val value indicating whether or not to standardize.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      // Note to self: the word "partially" is added above because the table
      // might actually not be standardized after deduction or coincidence
      // processing because it's too difficult to keep track of standardization
      // during these processes.
      ToddCoxeter& standardize(bool val) noexcept;

      //! The current value of the standardize setting.
      //!
      //! \parameters
      //! None
      //!
      //! \returns A value of type `bool`.
      //!
      //! \exceptions
      //! \noexcept
      //!
      //! \sa standardize(bool)
      bool standardize() const noexcept;

      //! Specify the strategy.
      //!
      //! The strategy used during the enumeration can be specified using
      //! this function.
      //!
      //! The default value is options::strategy::hlt.
      //!
      //! \param val value indicating which strategy to use
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is options::strategy::felsch
      //! and any of the following conditions apply:
      //! * \ref prefill was used to initialise \c this
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

      //! Set the amount of time per strategy for options::strategy::random
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

      //! Set the amount of time per strategy for options::strategy::random
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

      //! The current value of the random interval setting.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type ``std::chrono::nanoseconds``.
      //!
      //! \exceptions
      //! \noexcept
      std::chrono::nanoseconds random_interval() const noexcept;

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

      //! Remove duplicate generating pairs.
      //!
      //! Additionally, if \c this was defined over a finitely presented
      //! semigroup, then the copy of the defining relations of that semigroup
      //! contained in \c this (if any) also have duplicates removed.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if started() returns \c true.
      //!
      //! \parameters
      //! (None)
      ToddCoxeter& remove_duplicate_generating_pairs();

      //! Simplify defining relations and/or generating pairs.
      //!
      //! In the following description we refer to the defining relations and
      //! generating pairs of a ToddCoxeter instance (if any) as the `relation
      //! words`.  In many examples the performance of the Todd-Coxeter
      //! algorithm is improved by reducing the length of the relation words.
      //! This seems to particularly be the case when using the Felsch
      //! strategy.  This function does three things:
      //!
      //! 1. Attempts to reduce the length of the words by finding the
      //!    equivalence relation on the relation words generated by the pairs
      //!    of relation words. If \f$A = \{u_1, u_2, \ldots, u_n\}\f$ are the
      //!    distinct words in an equivalence class and \f$u_1\f$ is the
      //!    short-lex minimum word in the class, then the relation words are
      //!    replaced by \f$u_1 = u_2, u_1 = u_3, \cdots, u_1 = u_n\f$.
      //! 2. Removes duplicate relation words.
      //! 3. Repeatedly finds the subword of the relation words that will
      //!    result in the maximum reduction in the overall length of the
      //!    relation words when replaced by a redundant generator (if any such
      //!    subword exists).  This step is performed at most \p n times.
      //!
      //! \param n the number of repeats for step c.
      //!
      //! \returns A reference to \c this.
      //!
      //! \throws LibsemigroupsException if started() returns \c true.
      //! \throws LibsemigroupsException if the ToddCoxeter instance was
      //! prefilled.
      //!
      //! \warning This function might change the generators and relation words
      //! of a ToddCoxeter instance.
      ToddCoxeter& simplify(size_t n = 1);

      //! Returns a const iterator pointing at the first word in the first
      //! defining relation (if any).
      //!
      //! Returns a const iterator pointing to the first word in the first
      //! defining relation of the underlying semigroup of the congruence
      //! represented by an instance of ToddCoxeter.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type \ref std::vector<word_type>::const_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      std::vector<word_type>::const_iterator cbegin_relations() const noexcept {
        return _relations.cbegin();
      }

      //! Returns a const iterator pointing one after the last word in the last
      //! defining relation (if any).
      //!
      //! Returns a const iterator pointing one after the last word in the last
      //! defining relation of the underlying semigroup of the congruence
      //! represented by an instance of ToddCoxeter.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type \ref std::vector<word_type>::const_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      std::vector<word_type>::const_iterator cend_relations() const noexcept {
        return _relations.cend();
      }

      //! Returns a const iterator pointing at the first word in a generating
      //! pair.
      //!
      //! Returns a const iterator pointing to the word in the first generating
      //! pair of the congruence represented by a ToddCoxeter instance. This
      //! will often (but not always) be the same as the first component in the
      //! pair pointed at by cbegin_generating_pairs().
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type \ref std::vector<word_type>::const_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      std::vector<word_type>::const_iterator cbegin_extra() const noexcept {
        return _extra.cbegin();
      }

      //! Returns a const iterator pointing one after the last word in any
      //! generating pair.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type \ref std::vector<word_type>::const_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      std::vector<word_type>::const_iterator cend_extra() const noexcept {
        return _extra.cend();
      }

      //! Perform an HLT-style push of the defining relations at the identity.
      //!
      //! If a ToddCoxeter instance is defined over a finitely presented
      //! semigroup and the Felsch strategy is being used, it can be useful
      //! to follow all the paths from the identity labelled by the underlying
      //! relations of the semigroup (if any). The setting specifies whether or
      //! not to do this.
      //!
      //! The default value of this setting is \c false.
      //!
      //! \param val the boolean value.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& use_relations_in_extra(bool val) noexcept;

      //! The current value of the setting for using relations.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type ``bool``.
      //!
      //! \exceptions
      //! \noexcept
      bool use_relations_in_extra() const noexcept;

      //! The maximum number of deductions in the stack.
      //!
      //! This setting specifies the maximum number of deductions that can be
      //! in the stack at any given time. What happens if there are the maximum
      //! number of deductions in the stack and a new deduction is generated is
      //! governed by deduction_policy().
      //!
      //! The default value of this setting is \c 2'000.
      //!
      //! \param val the maximum size of the deduction stack.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& max_deductions(size_t val) noexcept;

      //! The current value of the setting for the maximum number of
      //! deductions.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type
      //! ``size_t``.
      //!
      //! \exceptions
      //! \noexcept
      size_t max_deductions() const noexcept;

      //! Specify how to handle deductions.
      //!
      //! This function can be used to specify how to handle deductions. For
      //! details see options::deductions.
      //!
      //! The default value of this setting is
      //! ``options::deductions::no_stack_if_no_space |
      //! options::deductions::v2``.
      //!
      //! \param val the policy to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is not valid (i.e. if for
      //! example ``options::deductions::v1 & options::deductions::v2`` returns
      //! ``true``).
      ToddCoxeter& deduction_policy(options::deductions val);

      //! The current value of the deduction policy setting.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type
      //! ``options::deductions``.
      //!
      //! \exceptions
      //! \noexcept
      options::deductions deduction_policy() const noexcept;

      //! Specify how to handle preferred definitions.
      //!
      //! This function can be used to specify how to handle preferred
      //! definitions. For details see options::preferred_defs.
      //!
      //! The default value of this setting is
      //! ``options::preferred_defs::deferred``.
      //!
      //! \note
      //! If \p val is options::preferred_defs::none, then max_preferred_defs()
      //! is set to \c 0.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& preferred_defs(options::preferred_defs val) noexcept;

      //! The current value of the preferred definitions setting.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type
      //! ``options::preferred_defs``.
      //!
      //! \exceptions
      //! \noexcept
      options::preferred_defs preferred_defs() const noexcept;

      //! Specify the maximum number of preferred definitions.
      //!
      //! This function can be used to specify the maximum number of preferred
      //! definitions that are held in the circular buffer at any time. For
      //! details see options::preferred_defs.
      //!
      //! The default value of this setting is \c 256.
      //!
      //! \note
      //! If \p val is \c 0, then preferred_defs() is set to
      //! options::preferred_defs::none.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& max_preferred_defs(size_t val) noexcept;

      //! The current value of the maximum preferred definitions setting.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type
      //! ``size_t``.
      //!
      //! \exceptions
      //! \noexcept
      size_t max_preferred_defs() const noexcept;

      //! The approx number of Felsch style definitions in
      //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
      //!
      //! If the strategy being used is any of those mimicking
      //! [ACE](https://staff.itee.uq.edu.au/havas/), then the value of this
      //! setting is used to determine the number of nodes defined in any Felsch
      //! phase of the strategy.
      //!
      //! The default value of this setting is \c 100'000.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is \c 0.
      ToddCoxeter& f_defs(size_t val);

      //! The current value of the f_defs setting.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type
      //! ``size_t``.
      //!
      //! \exceptions
      //! \noexcept
      size_t f_defs() const noexcept;

      //! The approx number of HLT style definitions in
      //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
      //!
      //! If the strategy being used is any of those mimicking
      //! [ACE](https://staff.itee.uq.edu.au/havas/), then the value of this
      //! setting is used to determine the number of nodes defined in any HLT
      //! phase of the strategy.
      //!
      //! The default value of this setting is \c 200'000.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \throws LibsemigroupsException if \p val is less than
      //! length_of_generating_pairs().
      ToddCoxeter& hlt_defs(size_t val);

      //! The current value of the hlt_defs setting.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type
      //! ``size_t``.
      //!
      //! \exceptions
      //! \noexcept
      size_t hlt_defs() const noexcept;

      //! Specify whether to standardize between HLT and Felsch.
      //!
      //! This setting allows the word graph to be standardized when switching
      //! between an HLT and Felsch phase (or vice versa) in an enumeration.
      //!
      //! The default value of this setting is \c false.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& restandardize(bool val) noexcept;

      //! The current value of the restandardize setting.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type
      //! ``bool``.
      //!
      //! \exceptions
      //! \noexcept
      bool restandardize() const noexcept;

      //! Specify what should be considered a large collapse.
      //!
      //! By default when processing coincidences nodes are merged in the word
      //! graph one pair at a time, and the in-neighbours of the surviving node
      //! are updated at the same time. If the number of coincidences is
      //! large, then it might be that a pair of nodes are merged at one step,
      //! then the surviving node is merged with another node at a future step,
      //! and this may happen many many times. This results in the
      //! in-neighbours of the surviving nodes being repeatedly traversed,
      //! which can result in a significant performance penalty.
      //! It can be beneficial to stop updating the in-neighbours as nodes are
      //! merged, and to just rebuild the entire in-neighbours data structure
      //! by traversing the entire word graph after all coincidences have been
      //! processed. This is beneficial if the number of surviving nodes is
      //! relatively small in comparison to the number of nodes merged.
      //! The purpose of this setting is to specify what should be
      //! considered a \"large\" collapse, or more precisely, what number of
      //! coincidences in the stack will trigger a change from updating the
      //! in-neighbours one-by-one to traversing the entire graph once after
      //! all coincidences have been processed.
      //!
      //! The default value of this setting is \c 100'000.
      //!
      //! \param val the value to use.
      //!
      //! \returns A reference to `*this`.
      //!
      //! \exceptions
      //! \noexcept
      ToddCoxeter& large_collapse(size_t val) noexcept;

      //! The current value of the large collapse setting.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns The current value of the setting, a value of type
      //! ``size_t``.
      //!
      //! \exceptions
      //! \noexcept
      size_t large_collapse() const noexcept;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (attributes) - public
      ////////////////////////////////////////////////////////////////////////

      //! Check if the congruence has more than one class.
      //!
      //! Returns tril::TRUE if it is possible to show that the congruence is
      //! non-trivial; tril::FALSE if the congruence is already known to be
      //! trivial; and tril::unknown if it is not possible to show that the
      //! congruence is non-trivial.
      //!
      //! This function attempts to find a non-trivial congruence containing
      //! the congruence represented by a ToddCoxeter instance by repeating the
      //! following steps on a copy until the enumeration concludes:
      //! 1. running the enumeration for the specified amount of time
      //! 2. repeatedly choosing a random pair of cosets and identifying them,
      //!    until the number of cosets left in the quotient is smaller than
      //!    \p threshold times the initial number of cosets for this step.
      //! If at the end of this process, the ToddCoxeter instance is
      //! non-trivial, then the original ToddCoxeter is also non-trivial.
      //! Otherwise, the entire process is repeated again up to a total of \p
      //! tries times.
      //!
      //! \param tries the number of attempts to find non-trivial
      //! super-congruence.
      //! \param try_for the amount of time in millisecond to enumerate the
      //! congruence after choosing a random pair of representatives and
      //! identifying them.
      //! \param threshold the threshold (see description).
      //!
      //! \returns A value of type \ref tril
      tril is_non_trivial(size_t                    tries = 10,
                          std::chrono::milliseconds try_for
                          = std::chrono::milliseconds(100),
                          float threshold = 0.99);

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

      //! Returns the total length of the generating pairs.
      //!
      //! This function returns the total length of the words that make up the
      //! generating pairs and any underlying relations.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \parameters
      //! (None)
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      size_t length_of_generating_pairs();

      //! Returns the height of the Felsch tree.
      //!
      //! This function returns the height of the Felsch tree of a ToddCoxeter
      //! instance. Processing deductions involves performing a depth first
      //! search in this tree.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \parameters
      //! (None)
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      size_t felsch_tree_height();

      //! Returns the number of nodes of the Felsch tree.
      //!
      //! This function returns the number of nodes in the Felsch tree of a
      //! ToddCoxeter instance. Processing deductions involves performing a
      //! depth first search in this tree.
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \parameters
      //! (None)
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      size_t felsch_tree_number_of_nodes();

     public:
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
      //! A value of type \c bool indicating whether or not any changes were
      //! made.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      bool standardize(order val);

      //! Returns the current order in which the table is standardized.
      //!
      //! \returns A value of type \ref order.
      //!
      //! \parameters
      //! (None)
      //!
      //! \exceptions
      //! \noexcept
      order standardization_order() const noexcept;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (reporting + stats) - public
      ////////////////////////////////////////////////////////////////////////

      //! Returns a const reference to a statistics object.
      //!
      //! This object contains a number of statistics related to the
      //! enumeration.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns
      //! A const reference to \c Stats.
      Stats const& stats() const noexcept {
        return _stats;
      }

      //! Returns a string containing a tabularized summary of the statistics.
      //!
      //! This function returns a string containing a tabularized summary of
      //! the enumeration statistics.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns
      //! A \c std::string.
      std::string stats_string() const;

      //! Returns a string containing a tabularized summary of all the
      //! settings.
      //!
      //! This function returns a string containing a tabularized summary of
      //! the settings of a ToddCoxeter instance.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns
      //! A \c std::string.
      std::string settings_string() const;

      //! Returns a string containing a GAP definition of the finitely
      //! presented semigroup represented by a ToddCoxeter instance.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns
      //! A \c std::string.
      //!
      //! \throws LibsemigroupsException if the number of generators exceeds
      //! 49.
      std::string to_gap_string();

     private:
      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (reporting + stats) - private
      ////////////////////////////////////////////////////////////////////////

      // The argument is the calling function's name
      void report_coincidences(char const*);
      void report_active_cosets(char const*);

      // The 1st argument is the calling function's name, and the second is the
      // number of cosets killed.
      void report_cosets_killed(char const*, int64_t) const;
      void report_inc_lookahead(char const*, size_t) const;
      void report_time(char const*, detail::Timer&) const;
      void report_at_coset(char const*, size_t) const;

     public:
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

      //! The type of a const iterator pointing to a word belonging to a
      //! particular class.
      //!
      //! Iterators of this type point to a \ref word_type.
      //!
      //! \sa cbegin_class, cend_class.
      using class_iterator =
          typename DigraphWithSources<coset_type>::const_pstislo_iterator;

      //! Returns a \ref class_iterator pointing at the shortlex least word in
      //! an class.
      //!
      //! Returns a const iterator pointing to the shortlex least word in the
      //! class with index \p i. When incremented this iterator will point at
      //! the shortlex next least word in the class with index \p i.  In this
      //! way, all words belonging to the class with index \p i can be
      //! obtained.
      //!
      //! \param i the index of the class
      //! \param min the minimum length of a word (defaults to \c 0)
      //! \param max the maximum length of a word (defaults to \c
      //! POSITIVE_INFINITY).
      //!
      //! \returns A value of type \ref class_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \warning
      //! This function does not trigger any enumeration!
      class_iterator cbegin_class(class_index_type i,
                                  size_t           min = 0,
                                  size_t max = POSITIVE_INFINITY) const {
        return _word_graph.cbegin_pstislo(_id_coset, i + 1, min, max);
      }

      //! Returns a \ref class_iterator pointing at the shortlex least word in
      //! a class.
      //!
      //! Returns a const iterator pointing to the shortlex least word in the
      //! class of the word \p w. When incremented this iterator will point at
      //! the shortlex next least word in the class of \p w.  In this
      //! way, all words belonging to the class of \p w can be
      //! obtained.
      //!
      //! \param w the word of the class
      //! \param min the minimum length of a word (defaults to \c 0)
      //! \param max the maximum length of a word (defaults to \c
      //! POSITIVE_INFINITY).
      //!
      //! \returns A value of type \ref class_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      // Might trigger an enumeration if word_to_class_index does.
      class_iterator cbegin_class(word_type const& w,
                                  size_t           min = 0,
                                  size_t           max = POSITIVE_INFINITY) {
        return cbegin_class(word_to_class_index(w), min, max);
      }

      //! Returns a \ref class_iterator pointing one past the last word in
      //! a class.
      //!
      //! Returns a const iterator pointing one past the last word in any
      //! class.
      //!
      //! \parameters
      //! (None)
      //!
      //! \returns A value of type \ref class_iterator.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \warning
      //! This function does not trigger any enumeration!
      class_iterator cend_class() const {
        return _word_graph.cend_pstislo();
      }

      //! Returns the size of the specified class.
      //!
      //! This function returns the number of words in the free semigroup that
      //! belong to the class with index \p i.
      //!
      //! \param i the index of the class
      //!
      //! \returns A value of type \c size_t.
      //!
      //! \warning
      //! This function does not trigger any enumeration!
      size_t number_of_words(class_index_type i) const {
        return _word_graph.number_of_paths(0, i + 1, 0, POSITIVE_INFINITY);
      }

      //! Returns the size of the specified class.
      //!
      //! This function returns the number of words in the free semigroup that
      //! belong to the class of the word \p w.
      //!
      //! \param w a word in the class.
      //!
      //! \returns A value of type \c size_t.
      // Might trigger an enumeration if word_to_class_index does.
      size_t number_of_words(word_type const& w) {
        return number_of_words(word_to_class_index(w) + 1);
      }

     private:
      void prefill(table_type const& table, std::function<size_t(size_t)> func);

      template <typename T>
      bool compatible(coset_type, T first, T last) const;
      void push_settings();
      void pop_settings();

      ////////////////////////////////////////////////////////////////////////
      // Runner - pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////
      void run_impl() override;
      bool finished_impl() const override;

      ////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////

      word_type class_index_to_word_impl(class_index_type) override;
      size_t    number_of_classes_impl() override;
      // Guaranteed to return a FroidurePin<TCE>.
      std::shared_ptr<FroidurePinBase> quotient_impl() override;
      class_index_type word_to_class_index_impl(word_type const&) override;

      ////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////

      class_index_type
           const_word_to_class_index(word_type const&) const override;
      bool is_quotient_obviously_finite_impl() override;
      bool is_quotient_obviously_infinite_impl() override;
      void set_number_of_generators_impl(size_t) override;
      void add_generators_impl(size_t) override;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - enums - private
      ////////////////////////////////////////////////////////////////////////

      enum class state {
        constructed = 0,
        relation_extra_initialized,
        hlt,
        felsch,
        lookahead,
        finished
      };

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (validation) - private
      ////////////////////////////////////////////////////////////////////////

      void validate_table(table_type const&, size_t const, size_t const) const;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (initialisation) - private
      ////////////////////////////////////////////////////////////////////////

      void copy_relations_for_quotient(ToddCoxeter&);
      void init_generating_pairs();
      void init_felsch_tree();
      void prefill(FroidurePinBase&);
      void prefill_and_validate(table_type const&,
                                bool,
                                std::function<size_t(size_t)>);

      bool reduce_length_once();

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (cosets) - private
      ////////////////////////////////////////////////////////////////////////

      coset_type new_coset();

      // Helper
      template <typename TStackDeduct,
                typename TProcessCoincide,
                typename TPreferredDef>
      inline void push_definition_felsch(coset_type const& c,
                                         size_t            i) noexcept {
        auto j = (i % 2 == 0 ? i + 1 : i - 1);
        push_definition_felsch<TStackDeduct, TProcessCoincide, TPreferredDef>(
            c, _relations[i], _relations[j]);
      }

      template <typename TStackDeduct>
      inline void def_edge(coset_type c, letter_type x, coset_type d) noexcept {
        LIBSEMIGROUPS_ASSERT(is_valid_coset(c));
        LIBSEMIGROUPS_ASSERT(x < number_of_generators());
        LIBSEMIGROUPS_ASSERT(is_valid_coset(d));
        TStackDeduct()(_deduct.get(), c, x);
        _word_graph.add_edge_nc(c, d, x);
      }

      template <typename TStackDeduct>
      coset_type def_edges(coset_type                c,
                           word_type::const_iterator first,
                           word_type::const_iterator last) noexcept;

      template <typename TStackDeduct, typename TProcessCoincide>
      void push_definition_hlt(coset_type const& c,
                               word_type const&  u,
                               word_type const&  v) noexcept;

      template <typename TStackDeduct,
                typename TProcessCoincide,
                typename TPreferredDef>
      void push_definition_felsch(coset_type const& c,
                                  word_type const&  u,
                                  word_type const&  v) noexcept;

      // The argument means: stack deductions or do not stack deductions
      enum class stack_deductions { yes, no };

      void process_coincidences(stack_deductions);

      template <typename TStackDeduct,
                typename TProcessCoincide,
                typename TPreferredDef>
      void
      push_definition(coset_type x, letter_type a, coset_type y, letter_type b);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (main strategies) - private
      ////////////////////////////////////////////////////////////////////////

      void felsch();
      void hlt();
      void random();
      void CR_style();
      void R_over_C_style();
      void Cr_style();
      void Rc_style();

      void process_deductions();

      template <typename TPreferredDefs>
      void process_deductions_v2();
      template <typename TPreferredDefs>
      void process_deductions_dfs_v2(coset_type, coset_type);
      template <typename TPreferredDefs>
      void process_deductions_v1();
      template <typename TPreferredDefs>
      void process_deductions_dfs_v1(coset_type);

      void init_run();
      void finalise_run(detail::Timer&);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (lookahead) - private
      ////////////////////////////////////////////////////////////////////////

      void   perform_lookahead();
      size_t hlt_lookahead(state const&);
      size_t felsch_lookahead(state const&);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (standardize) - private
      ////////////////////////////////////////////////////////////////////////

      void init_standardize();
      bool standardize_immediate(coset_type const, letter_type const);

      bool standardize_deferred(std::vector<coset_type>&,
                                std::vector<coset_type>&,
                                coset_type const,
                                coset_type&,
                                letter_type const);

      bool lex_standardize();
      bool recursive_standardize();
      bool shortlex_standardize();

      void apply_permutation(Perm&, Perm&);
      void swap_cosets(coset_type const, coset_type const);

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - member functions (debug) - private
      ////////////////////////////////////////////////////////////////////////

#ifdef LIBSEMIGROUPS_DEBUG
      void debug_validate_table() const;
      void debug_validate_word_graph() const;
      void debug_verify_no_missing_deductions() const;
#endif

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - inner classes - private
      ////////////////////////////////////////////////////////////////////////

      struct Settings;            // Forward declaration
      struct TreeNode;            // Forward declaration
      struct QueuePreferredDefs;  // Forward declaration
      struct StackDeductions;     // Forward declaration

      template <typename TStackDeduct>
      struct ImmediateDef;  // Forward declaration

      using Coincidence  = std::pair<coset_type, coset_type>;
      using Coincidences = std::stack<Coincidence>;

      class Deductions;     // Forward declaration
      class PreferredDefs;  // Forward declaration

      template <stack_deductions val>
      struct ProcessCoincidences {
        void operator()(ToddCoxeter* tc) const noexcept {
          tc->process_coincidences(val);
        }
      };

      struct Stats {
        uint64_t tc1_hlt_appl = 0;
        uint64_t tc1_f_appl   = 0;

#ifdef LIBSEMIGROUPS_ENABLE_STATS
        uint64_t prev_active_cosets       = 0;
        uint64_t prev_coincidences        = 0;
        uint64_t f_lookahead_calls        = 0;
        uint64_t hlt_lookahead_calls      = 0;
        uint64_t tc2_appl                 = 0;
        uint64_t tc2_good_appl            = 0;
        uint64_t tc3_appl                 = 0;
        uint64_t max_coinc                = 0;
        uint64_t nr_active_coinc          = 0;
        uint64_t total_coinc              = 0;
        uint64_t max_deduct               = 0;
        uint64_t nr_active_deduct         = 0;
        uint64_t total_deduct             = 0;
        uint64_t max_preferred_defs       = 0;
        uint64_t nr_active_preferred_defs = 0;
        uint64_t total_preferred_defs     = 0;
#endif
      };

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - aliases - private
      ////////////////////////////////////////////////////////////////////////

      using Deduction = std::pair<coset_type, letter_type>;
      using Tree      = std::vector<TreeNode>;

      ////////////////////////////////////////////////////////////////////////
      // ToddCoxeter - data - private
      ////////////////////////////////////////////////////////////////////////

      Coincidences                        _coinc;
      std::unique_ptr<Deductions>         _deduct;
      std::vector<word_type>              _extra;
      std::unique_ptr<detail::FelschTree> _felsch_tree;
      size_t                              _nr_pairs_added_earlier;
      bool                                _prefilled;
      std::unique_ptr<PreferredDefs>      _preferred_defs;
      std::vector<word_type>              _relations;
      std::unique_ptr<Settings>           _settings;
      std::stack<Settings*>               _setting_stack;
      coset_type                          _standard_max;
      order                               _standardized;
      state                               _state;
      Stats                               _stats;
      std::unique_ptr<Tree>               _tree;
      DigraphWithSources<coset_type>      _word_graph;
    };

    ToddCoxeter::options::lookahead
    operator|(ToddCoxeter::options::lookahead const& opt1,
              ToddCoxeter::options::lookahead const& opt2);

    bool operator&(ToddCoxeter::options::lookahead const& opt1,
                   ToddCoxeter::options::lookahead const& opt2);

    ToddCoxeter::options::deductions
    operator|(ToddCoxeter::options::deductions const& opt1,
              ToddCoxeter::options::deductions const& opt2);

    bool operator&(ToddCoxeter::options::deductions const& opt1,
                   ToddCoxeter::options::deductions const& opt2);

  }  // namespace congruence
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TODD_COXETER_HPP_
