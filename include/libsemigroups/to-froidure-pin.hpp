//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_TO_FROIDURE_PIN_HPP_
#define LIBSEMIGROUPS_TO_FROIDURE_PIN_HPP_

#include <cstddef>      // for size_t
#include <memory>       // for unique_ptr
#include <type_traits>  // for enable_if_t, is_base_of

#include "cong-class.hpp"          // for Congruence
#include "froidure-pin.hpp"        // for FroidurePin
#include "kambites-class.hpp"      // for Kambites
#include "knuth-bendix-class.hpp"  // for KnuthBendix
#include "konieczny.hpp"           // for Konieczny
#include "todd-coxeter-class.hpp"  // for ToddCoxeter

#include "detail/kbe.hpp"  // for KBE
#include "detail/ke.hpp"   // for KE
#include "detail/tce.hpp"  // for TCE

namespace libsemigroups {

  //! \defgroup to_group The \`to\` function
  //!
  //! This page contains links to the documentation of the overloads of the
  //! function template `to` for converting from one type of object in
  //! `libsemigroups` to another type. These mostly only apply to the types
  //! implementing the main algorithms in the `libsemigroups`.
  //!
  //! For example, to convert a \ref_todd_coxeter object to a \ref FroidurePin
  //! object, you can simply do `to<FroidurePin>(tc)`.
  //!
  //! A summary of the overloads available in `libsemigroups` of
  //! `to<ToType>(FromType)` are given below, where the rows correspond to
  //! `ToType` and the columns to `FromType`:
  //!
  //! \image html to-table.svg width=60%
  //!
  //! A tick indicates that this conversion is implemented, and a cross
  //! that it is not yet implemented.
  //!
  //! See the following for more details:
  //! * \ref to_cong_group
  //! * \ref to_froidure_pin_group
  //! * \ref to_inverse_presentation_group
  //! * \ref to_knuth_bendix_group
  //! * \ref to_presentation_group
  //! * \ref to_todd_coxeter_group

  //! \defgroup to_froidure_pin_group to<FroidurePin>
  //! \ingroup froidure_pin_group
  //!
  //! \brief Convert to a FroidurePin instance.
  //!
  //! This page contains documentation related to converting a `libsemigroups`
  //! object into a \ref FroidurePin instance.
  //!
  //! \sa \ref to_group for an overview of possible conversions between
  //! `libsemigroups` types.

  ////////////////////////////////////////////////////////////////////////
  // Congruence
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_froidure_pin_group
  //! \brief Convert a Congruence object to a FroidurePin object.
  //!
  //! Defined in \c to-froidure-pin.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<FroidurePin>(cong);
  //! \endcode
  //!
  //! where \p cong is a Congruence object. The returned FroidurePin object is
  //! isomorphic to the quotient of the underlying semigroup or monoid of \p
  //! cong by the congruence represented by \p cong.
  //!
  //! \tparam Thing used for SFINAE should be FroidurePin
  //! \tparam Word the type of the words in relations in \p cong.
  //! \param cong the Congruence instance to convert.
  //!
  //! \returns A FroidurePin instance.
  //!
  //! \throws LibsemigroupsException if `cong.kind()` is not
  //! \ref congruence_kind.twosided.
  template <template <typename...> typename Thing, typename Word>
  auto to(Congruence<Word>& cong)
      -> std::enable_if_t<std::is_same_v<Thing<int>, FroidurePin<int>>,
                          std::unique_ptr<FroidurePinBase>>;

  ////////////////////////////////////////////////////////////////////////
  // Kambites
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_froidure_pin_group
  //! \brief Convert a Kambites object to a FroidurePin object.
  //!
  //! Defined in \c to-froidure-pin.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<FroidurePin>(k);
  //! \endcode
  //!
  //! where \p k is a Kambites instance. The returned FroidurePin object is
  //! isomorphic to the quotient semigroup or monoid represented by \p k.
  //!
  //! \tparam Thing used for SFINAE should be FroidurePin
  //! \tparam Word the type of the words in relations in \p k.
  //!
  //! \param k the Kambites instance to convert.
  //!
  //! \returns A FroidurePin instance.
  //!
  //! \throws LibsemigroupsException if the Kambites::small_overlap_class is not
  //! at least \f$4\f$.
  //!
  //! \warning The returned FroidurePin instance is always infinite, and so any
  //! calls to any member functions that that trigger a full enumeration will
  //! never terminate (or they will when your computer kills the process because
  //! it has run out of memory).
  template <template <typename...> typename Thing, typename Word>
  auto to(Kambites<Word>& k) -> std::enable_if_t<
      std::is_same_v<Thing<detail::KE<Word>>, FroidurePin<detail::KE<Word>>>,
      FroidurePin<detail::KE<Word>>>;

  ////////////////////////////////////////////////////////////////////////
  // KnuthBendix
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_froidure_pin_group
  //!
  //! \brief Convert a \ref_knuth_bendix object to a FroidurePin object.
  //!
  //! Defined in \c to-froidure-pin.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<FroidurePin>(kb);
  //! \endcode
  //!
  //! where \p kb is a \ref_knuth_bendix instance. The returned FroidurePin
  //! object is isomorphic to the quotient semigroup or monoid represented by \p
  //! kb.
  //!
  //! \tparam Thing used for SFINAE should be FroidurePin.
  //! \tparam Rewriter the second template parameter for \ref_knuth_bendix.
  //! \tparam ReductionOrder the third template parameter for \ref_knuth_bendix.
  //!
  //! \param kb the \ref_knuth_bendix instance to convert.
  //!
  //! \returns A FroidurePin instance.
  //!
  //! \throws LibsemigroupsException if `kb.kind()` is not
  //! \ref congruence_kind.twosided.
  template <template <typename...> typename Thing,
            typename Rewriter,
            typename ReductionOrder>
  auto to(detail::KnuthBendixImpl<Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<
          std::is_same_v<Thing<int>, FroidurePin<int>>,
          FroidurePin<
              detail::KBE<detail::KnuthBendixImpl<Rewriter, ReductionOrder>>>>;

  //! \ingroup to_froidure_pin_group
  //!
  //! \brief Convert a \ref Konieczny object to a FroidurePin object.
  //!
  //! Defined in \c to-froidure-pin.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<FroidurePin>(k);
  //! \endcode
  //!
  //! where \p k is a \ref Konieczny instance. The returned FroidurePin
  //! object is isomorphic to the semigroup or monoid represented by \p
  //! k.
  //!
  //! \tparam Thing used for SFINAE should be FroidurePin.
  //! \tparam Element the element type of the Konieczny object.
  //! \tparam Traits the traits type of the Konieczny object.
  //!
  //! \param k the \ref Konieczny instance to convert.
  //!
  //! \returns A FroidurePin instance.
  template <template <typename...> typename Thing,
            typename Element,
            typename Traits>
  auto to(Konieczny<Element, Traits> const& k)
      -> std::enable_if_t<std::is_same_v<Thing<int>, FroidurePin<int>>,
                          FroidurePin<Element>>;

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_froidure_pin_group
  //! \brief Convert a \ref_todd_coxeter object to a FroidurePin object.
  //!
  //! Defined in \c to-froidure-pin.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<FroidurePin>(kb);
  //! \endcode
  //!
  //! where \p kb is a \ref_todd_coxeter instance. The returned FroidurePin
  //! object is isomorphic to the quotient semigroup or monoid represented by \p
  //! kb.
  //!
  //! \tparam Thing used for SFINAE should be FroidurePin.
  //!
  //! \param tc the \ref_todd_coxeter instance to convert.
  //!
  //! \returns A FroidurePin instance.
  //!
  //! \throws LibsemigroupsException if `kb.kind()` is not
  //! \ref congruence_kind.twosided.
  //!
  //! \warning The returned FroidurePin instance is always infinite, and so any
  //! calls to any member functions that that trigger a full enumeration will
  //! never terminate (or they will when your computer kills the process because
  //! it has run out of memory).
  template <template <typename...> typename Thing>
  auto to(detail::ToddCoxeterImpl& tc) -> std::enable_if_t<
      std::is_same_v<Thing<detail::TCE>, FroidurePin<detail::TCE>>,
      FroidurePin<detail::TCE>>;

  ////////////////////////////////////////////////////////////////////////
  // WordGraph
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_froidure_pin_group
  //! \anchor to_froidure_pin
  //! \brief Convert a WordGraph object to a FroidurePin object.
  //!
  //! Defined in \c to-froidure-pin.hpp
  //!
  //! This function converts a WordGraph object to a FroidurePin object.
  //! Unlike the other functions on this page, this function should be invoked
  //! as follows (for example):
  //!
  //! \code
  //! to<FroidurePin<Transf<>>(wg, 0, 10);
  //! \endcode
  //!
  //! In other words, the type of the elements of the FroidurePin object should
  //! be explicitly specified. This type must implement `operator[]` and each
  //! label `n` in the WordGraph will correspond to a generator `f` in the
  //! output FroidurePin such that `f[s] = t` whenever there is an edge from `s`
  //! to `t` in \p wg labelled `n`.
  //!
  //! More precisely, if \f$a\f$ and \f$b\f$ are the parameters \p first and \p
  //! last, respectively, \f$m\f$ is the number of nodes in the WordGraph \p wg,
  //! \f$0 \leq a,  b< m\f$, and \f$n\f$ is an edge label, then we define
  //! \f$f: \{a, \ldots, b - 1\} \to \{0, \ldots, m - 1\}\f$ so that \f$(x)f\f$
  //! equals the target of the edge starting at node \f$x\f$ with label \f$n\f$.
  //! In this way, every edge label in a WordGraph corresponds to a
  //! transformation of the nodes of the digraph. If
  //! \f$\{a, \ldots, b - 1\}f \subseteq \{a, \ldots, b - 1\}\f$, then \f$f\f$
  //! is a transformation in the sense of Transf. Assuming that for every edge
  //! label of the WordGraph the corresponding \f$f\f$ satisfies
  //! \f$\{a, \ldots, b - 1\}f \subseteq \{a, \ldots, b - 1\}\f$, then this
  //! function returns the FroidurePin object corresponding to the semigroup or
  //! monoid generated by the set of all such transformations.
  //!
  //! \tparam Result the return type of the function.
  //! \tparam Node the type of the nodes of the word graph \p wg.
  //!
  //! \param wg the WordGraph being used to construct the FroidurePin
  //! object.
  //! \param first the value of \f$a\f$ in the preceding discussion.
  //! \param last the value of \f$b\f$ in the preceding discussion.
  //!
  //! \returns The constructed FroidurePin object, a value of type \p Result.
  //!
  //! \throws LibsemigroupsException if \p first > \p last.
  //!
  //! \throws LibsemigroupsException if \p first or \p last exceeds
  //! \ref WordGraph::number_of_nodes.
  //!
  //! \throws LibsemigroupsException if
  //! \ref validate(Transf<N, Scalar> const&) throws for any of the
  //! constructed generators. This can happen if, for example, the WordGraph is
  //! not complete (i.e. there exists an edge label and node for which there is
  //! no edge with the given label and given source) or if there is an edge
  //! label such that
  //! \f$\{a, \ldots, b - 1\}f \not\subseteq \{a, \ldots, b - 1\}\f$ for the
  //! corresponding \f$f\f$.
  template <typename Result, typename Node>
  auto to(WordGraph<Node> const& wg, size_t first, size_t last)
      -> std::enable_if_t<
          std::is_same_v<FroidurePin<typename Result::element_type>, Result>,
          Result>;

  //! \ingroup to_froidure_pin_group
  //! \brief Convert a WordGraph object to a FroidurePin object.
  //!
  //! Defined in \c to-froidure-pin.hpp
  //!
  //! Calls `to<FroidurePin>(wg, 0, wg.number_of_nodes())`.
  //!
  //! See \ref to_froidure_pin.
  template <typename Result, typename Node>
  auto to(WordGraph<Node> const& wg) -> std::enable_if_t<
      std::is_same_v<FroidurePin<typename Result::element_type>, Result>,
      Result> {
    return to<Result>(wg, 0, wg.number_of_nodes());
  }
}  // namespace libsemigroups

#include "to-froidure-pin.tpp"
#endif  // LIBSEMIGROUPS_TO_FROIDURE_PIN_HPP_
