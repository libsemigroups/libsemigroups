//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2024 James D. Mitchell
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

// TODO(0):
// * iwyu
// * "Defined in to-presentation.hpp"
// * "Despite the horrible signature..."

#ifndef LIBSEMIGROUPS_TO_PRESENTATION_HPP_
#define LIBSEMIGROUPS_TO_PRESENTATION_HPP_

#include <type_traits>  // for enable_if_t
#include <utility>      // for pair
#include <vector>       // for vector

#include "froidure-pin-base.hpp"   // for FroidurePinBase::const_rule_i...
#include "knuth-bendix-class.hpp"  // for KnuthBendix
#include "presentation.hpp"        // for Presentation

namespace libsemigroups {

  //! \defgroup to_presentation_group Making presentations from other objects
  //!
  //! This file contains documentation for creating semigroup and monoid
  //! presentations in `libsemigroups` from other types of objects.

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin -> Presentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a FroidurePin object.
  //!
  //! This function constructs and returns a Presentation object using the
  //! rules of a FroidurePin object, accessed via FroidurePin::cbegin_rules and
  //! FroidurePin::cend_rules.
  //!
  //! No enumeration of the argument \p fp is performed, so it might be the
  //! case that the resulting presentation does not define the same semigroup
  //! as \p fp. To ensure that the resulting presentation defines the same
  //! semigroup as \p fp, run FroidurePin::run (or any other function that
  //! fully enumerates \p fp) prior to calling this function.
  //!
  //! \tparam Word the type of the rules in the presentation being constructing.
  //!
  //! \param fp the FroidurePin object from which to obtain the rules.
  //!
  //! \returns An object of type \c Presentation<Word>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Result>
  auto to(FroidurePinBase& fp) -> std::enable_if_t<
      std::is_same_v<Presentation<typename Result::word_type>, Result>,
      Result>;

  ////////////////////////////////////////////////////////////////////////
  // KnuthBendix -> Presentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a \ref_knuth_bendix object.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! This function constructs and returns a Presentation object using the
  //! currently active rules of \p kb.
  //!
  //! No enumeration of the argument \p kb is performed, so it might be the
  //! case that the resulting presentation does not define the same
  //! semigroup/monoid as \p kb. To ensure that the resulting presentation
  //! defines the same semigroup as \p kb, run \ref KnuthBendix::run (or any
  //! other function that fully enumerates \p kb) prior to calling this
  //! function.
  //!
  //! \tparam Word the type of the rules in the presentation being
  //! constructed.
  //!
  //! \param kb the \ref_knuth_bendix object from which to obtain the rules.
  //!
  //! \returns An object of type \c Presentation<Word>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <template <typename...> typename Thing,
            typename Word,
            typename Rewriter,
            typename ReductionOrder>
  auto to(KnuthBendix<Word, Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<std::is_same_v<Thing<Word>, Presentation<Word>>,
                          Presentation<Word>> {
    return to<Presentation<Word>>(kb);
  }

  ////////////////////////////////////////////////////////////////////////
  // Presentation + function -> Presentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a different type of presentation.
  //!
  //! Returns a presentation equivalent to the input presentation but of a
  //! different type (for example, can be used to convert from `std::string`
  //! to \ref word_type). The second parameter specifies how to map the letters
  //! of one presentation to the other.
  //!
  //! \tparam WordOutput the type of the words in the returned presentation.
  //! \tparam WordInput the type of the words in the input presentation.
  //! \tparam Func the type of a function for transforming letters.
  //! \param p the input presentation.
  //! \param f a function mapping `Presentation<WordInput>::letter_type` to
  //! `Presentation<WordOutput>::letter_type`.
  //!
  //! \returns A value of type `Presentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if `p.validate()` throws.
  //!
  //! \note The types specified by `WordInput` and `WordOutput` must be
  //! different, and `Func` must be a function type that is invocable with
  //! values of `Presentation<WordInput>::letter_type`. If either of this are
  //! not true, then template substitution will fail.
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  // TODO update
  template <typename WordOutput, typename WordInput, typename Func>
  Presentation<WordOutput> to(Presentation<WordInput> const& p, Func&& f);
#else
  template <typename Result, typename Word, typename Func>
  auto to(Presentation<Word> const& p, Func&& f) -> std::enable_if_t<
      std::is_same_v<Presentation<typename Result::word_type>, Result>,
      Result>;
#endif

  ////////////////////////////////////////////////////////////////////////
  // InversePresentation + function -> InversePresentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make an inverse presentation from a different type of inverse
  //! presentation.
  //!
  //! Returns an inverse presentation equivalent to the input inverse
  //! presentation but of a different type (for example, can be used to
  //! convert from `std::string` to \ref word_type). The second parameter
  //! specifies how to map the letters of one inverse presentation to the
  //! other.
  //!
  //! \tparam WordOutput the type of the words in the returned inverse
  //! presentation.
  //! \tparam WordInput the type of the words in the input inverse
  //! presentation.
  //! \tparam Func the type of a function for transforming letters.
  //! \param ip the input inverse presentation.
  //! \param f a function  mapping
  //! `InversePresentation<WordInput>::letter_type` to
  //! `InversePresentation<WordOutput>::letter_type`.
  //!
  //! \returns A value of type `InversePresentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if the alphabet of \p ip contains
  //! duplicate letters.
  //!
  //! \note The types specified by `WordInput` and `WordOutput` must be
  //! different, and `Func` must be a function type that is invocable with
  //! values of `Presentation<WordInput>::letter_type`. If either of this are
  //! not true, then template substitution will fail.
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename WordOutput, typename WordInput, typename Func>
  InversePresentation<WordOutput> to(InversePresentation<WordInput> const& ip,
                                     Func&&                                f);
// TODO(0) update the doc
#else
  template <typename Result, typename Word, typename Func>
  auto to(InversePresentation<Word> const& p, Func&& f) -> std::enable_if_t<
      std::is_same_v<InversePresentation<typename Result::word_type>, Result>,
      Result>;
#endif

  ////////////////////////////////////////////////////////////////////////
  // Presentation -> Presentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a different type of presentation.
  //!
  //! Returns a presentation equivalent to the input presentation but of a
  //! different type (for example, can be used to convert from `std::string`
  //! to \ref word_type).
  //!
  //! If the alphabet of of \p p is \f$\{a_0, a_1, \dots a_{n-1}\}\f$, then the
  //! conversion from `Presentation<WordInput>::letter_type` to
  //! `Presentation<WordOutput>::letter_type` is \f$a_i \mapsto\f$
  //! `human_readable_letter<WordOutput>(size_t i)`.
  //!
  //! \tparam WordOutput the type of the words in the returned presentation.
  //! \tparam WordInput the type of the words in the input presentation.
  //! \param p the input presentation.
  //!
  //! \returns A value of type `Presentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if `p.validate()` throws.
  //!
  //! \note The types specified by `WordInput` and `WordOutput` must be
  //! different. If not, then template substitution will fail.
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename WordOutput, typename WordInput>
  Presentation<WordOutput> to(Presentation<WordInput> const& p);
// TODO(0) update
#else
  template <typename Result, typename Word>
  auto to(Presentation<Word> const& p) -> std::enable_if_t<
      std::is_same_v<Presentation<typename Result::word_type>, Result>
          && !std::is_same_v<typename Result::word_type, Word>,
      Result>;
#endif

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from the same type of presentation.
  //!
  //! This function just returns its argument \p p, and is included solely for
  //! the purpose of simplifying certain client code, where presentations must
  //! be converted from one type to another sometimes, but not other times.
  //!
  //! \tparam Word the type of the words in the presentation.
  //! \param p the input presentation.
  //!
  //! \returns The argument \p p by const reference.
  //!
  //! \exceptions
  //! \noexcept
  template <typename Result, typename Word>
  auto to(Presentation<Word> const& p) noexcept
      -> std::enable_if_t<std::is_same_v<Presentation<Word>, Result>,
                          Result const&> {
    return p;
  }

  ////////////////////////////////////////////////////////////////////////
  // InversePresentation -> InversePresentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make an inverse presentation from a different type of inverse
  //! presentation.
  //!
  //! Returns an inverse presentation equivalent to the input presentation but
  //! of a different type (for example, can be used to convert from
  //! `std::string` to \ref word_type).
  //!
  //! If the alphabet of of \p ip is \f$\{a_0, a_1, \dots a_{n-1}\}\f$, then
  //! the conversion from `InversePresentation<WordInput>::letter_type` to
  //! `InversePresentation<WordOutput>::letter_type` is \f$a_i \mapsto\f$
  //! `human_readable_letter<WordOutput>(size_t i)`.
  //!
  //! \tparam WordOutput the type of the words in the returned inverse
  //! presentation.
  //! \tparam WordInput the type of the words in the input inverse
  //! presentation.
  //! \param ip the input inverse presentation.
  //!
  //! \returns A value of type `InversePresentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if the alphabet of \p ip contains
  //! duplicate letters.
  //!
  //! \note The types specified by `WordInput` and `WordOutput` must be
  //! different. If not, then template substitution will fail.
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  template <typename WordOutput, typename WordInput>
  InversePresentation<WordOutput> to(InversePresentation<WordInput> const& ip);
// TODO(0) update doc
#else
  template <typename Result, typename Word>
  auto to(InversePresentation<Word> const& ip) noexcept -> std::enable_if_t<
      std::is_same_v<InversePresentation<typename Result::word_type>, Result>
          && !std::is_same_v<Word, typename Result::word_type>,
      Result> {
    using WordOutput = typename Result::word_type;
    return to<InversePresentation<WordOutput>>(ip, [&ip](auto val) {
      return words::human_readable_letter<WordOutput>(ip.index(val));
    });
  }

  // TODO(0) doc
  template <typename Result, typename Word>
  auto to(InversePresentation<Word> const& ip) noexcept
      -> std::enable_if_t<std::is_same_v<InversePresentation<Word>, Result>,
                          Result const&> {
    return ip;
  }

  ////////////////////////////////////////////////////////////////////////
  // Presentation -> InversePresentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make an inverse presentation from a presentation
  //!
  //! Returns an inverse presentation with rules equivalent to those of the
  //! input presentation, but over a normalised alphabet. If the alphabet of
  //! \p p is \f$\{a_0, a_1, \dots, a_{n-1}\}\f$, then the alphabet of the
  //! returned inverse presentation will be \f$\{0, 1, \dots, n-1, n, \dots,
  //! 2n-1\}\f$, where the inverse of letter \f$i\f$ is the letter \f$i + n\,
  //! (\text{mod }2n)\f$.
  //!
  //! \tparam Word the type of the words in the presentation.
  //! \param p the input presentation.
  //!
  //! \returns A value of type `InversePresentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if `p.validate()` throws.
  //!
  //! \note The parameter \p p must not be an `InversePresentation`, otherwise
  //! a compilation error is thrown.
  template <template <typename...> typename Thing, typename Word>
  auto to(Presentation<Word> const& p) -> std::enable_if_t<
      std::is_same_v<InversePresentation<Word>, Thing<Word>>,
      InversePresentation<Word>>;

#endif

}  // namespace libsemigroups

#include "to-presentation.tpp"

#endif  // LIBSEMIGROUPS_TO_PRESENTATION_HPP_
