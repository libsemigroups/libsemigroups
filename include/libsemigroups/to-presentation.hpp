//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 - 2024 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_TO_PRESENTATION_HPP_
#define LIBSEMIGROUPS_TO_PRESENTATION_HPP_

#include <type_traits>  // for enable_if_t
#include <utility>      // for pair
#include <vector>       // for vector

#include "froidure-pin-base.hpp"  // for FroidurePinBase::const_rule_i...
#include "presentation.hpp"       // for Presentation

namespace libsemigroups {

  //! \defgroup to_presentation_group Making presentations from other objects
  //!
  //! This file contains documentation for creating semigroup and monoid
  //! presentations in `libsemigroups` from other types of objects.

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
  //! \tparam WordOutput the type of the presentation to construct (must be a
  //! type of Presentation).
  //!
  //! \param fp the FroidurePin object from which to obtain the rules.
  //!
  //! \returns An object of type \c Presentation<WordOutput>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename WordOutput>
  Presentation<WordOutput> to_presentation(FroidurePinBase& fp);

#ifdef PARSED_BY_DOXYGEN
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
  template <typename WordOutput, typename WordInput, typename Func>
  Presentation<WordOutput> to_presentation(Presentation<WordInput> const& p,
                                           Func&&                         f);
#else
  template <typename WordOutput, typename WordInput, typename Func>
  auto to_presentation(Presentation<WordInput> const& p, Func&& f)
      -> std::enable_if_t<
          std::is_invocable_v<std::decay_t<Func>,
                              typename Presentation<WordInput>::letter_type>
              && !std::is_same_v<WordOutput, WordInput>,
          Presentation<WordOutput>>;
#endif

#ifdef PARSED_BY_DOXYGEN
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
  //! `Presentation<WordOutput>::letter_type` is either:
  //!   * \f$a_i \mapsto\f$ `human_readable_char(size_t i)` if `WordOutput` is
  //!   `std::string`; or
  //!   * \f$a_i \mapsto\f$ `i` otherwise.
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
  template <typename WordOutput, typename WordInput>
  Presentation<WordOutput> to_presentation(Presentation<WordInput> const& p);
#else
  template <typename WordOutput, typename WordInput>
  auto to_presentation(Presentation<WordInput> const& p)
      -> std::enable_if_t<!std::is_same_v<WordOutput, WordInput>,
                          Presentation<WordOutput>>;
#endif

#ifdef PARSED_BY_DOXYGEN
  //! \ingroup to_presentation_group
  //!
  //! \brief Make an inverse presentation from a different type of inverse
  //! presentation.
  //!
  //! Returns an inverse presentation equivalent to the input inverse
  //! presentation but of a different type (for example, can be used to convert
  //! from `std::string` to \ref word_type). The second parameter specifies how
  //! to map the letters of one inverse presentation to the other.
  //!
  //! \tparam WordOutput the type of the words in the returned inverse
  //! presentation.
  //! \tparam WordInput the type of the words in the input inverse presentation.
  //! \tparam Func the type of a function for transforming letters.
  //! \param ip the input inverse presentation.
  //! \param f a function  mapping `InversePresentation<WordInput>::letter_type`
  //! to `InversePresentation<WordOutput>::letter_type`.
  //!
  //! \returns A value of type `InversePresentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if the alphabet of \p ip contains duplicate
  //! letters.
  //!
  //! \note The types specified by `WordInput` and `WordOutput` must be
  //! different, and `Func` must be a function type that is invocable with
  //! values of `Presentation<WordInput>::letter_type`. If either of this are
  //! not true, then template substitution will fail.
  template <typename WordOutput, typename WordInput, typename Func>
  InversePresentation<WordOutput>
  to_inverse_presentation(InversePresentation<WordInput> const& ip, Func&& f);
#else
  template <typename WordOutput, typename WordInput, typename Func>
  auto to_inverse_presentation(InversePresentation<WordInput> const& ip,
                               Func&&                                f)
      -> std::enable_if_t<
          std::is_invocable_v<std::decay_t<Func>,
                              typename Presentation<WordInput>::letter_type>
              && !std::is_same_v<WordOutput, WordInput>,
          InversePresentation<WordOutput>>;
#endif

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
  template <typename Word>
  InversePresentation<Word>
  to_inverse_presentation(Presentation<Word> const& p);

  //! No doc
  // TODO(now) is this the correct way to prevent the nonsensical conversion of
  // an inverse presentation to an inverse presentation of the same type?
  // JDE wanted some way of preventing the user from calling
  // to_inverse_presentation(Presentation<Word> const& p) in the case where p
  // is, in fact, already an inverse presentation.
  template <typename Word>
  InversePresentation<Word>
  to_inverse_presentation(InversePresentation<Word> const& ip) {
    return ip;
  }

#ifdef PARSED_BY_DOXYGEN
  //! \ingroup to_presentation_group
  //!
  //! \brief Make an inverse presentation from a different type of inverse
  //! presentation.
  //!
  //! Returns an inverse presentation equivalent to the input presentation but
  //! of a different type (for example, can be used to convert from
  //! `std::string` to \ref word_type).
  //!
  //! If the alphabet of of \p ip is \f$\{a_0, a_1, \dots a_{n-1}\}\f$, then the
  //! conversion from `InversePresentation<WordInput>::letter_type` to
  //! `InversePresentation<WordOutput>::letter_type` is either:
  //!   * \f$a_i \mapsto\f$ `human_readable_char(size_t i)` if `WordOutput` is
  //!   `std::string`; or
  //!   * \f$a_i \mapsto\f$ `i` otherwise.
  //!
  //! \tparam WordOutput the type of the words in the returned inverse
  //! presentation.
  //! \tparam WordInput the type of the words in the input inverse presentation.
  //! \param ip the input inverse presentation.
  //!
  //! \returns A value of type `InversePresentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if the alphabet of \p ip contains duplicate
  //! letters.
  //!
  //! \note The types specified by `WordInput` and `WordOutput` must be
  //! different. If not, then template substitution will fail.
  template <typename WordOutput, typename WordInput>
  InversePresentation<WordOutput>
  to_inverse_presentation(InversePresentation<WordInput> const& ip);
#else
  template <typename WordOutput, typename WordInput>
  auto to_inverse_presentation(InversePresentation<WordInput> const& ip)
      -> std::enable_if_t<!std::is_same_v<WordOutput, WordInput>,
                          InversePresentation<WordOutput>>;
#endif

}  // namespace libsemigroups

#include "to-presentation.tpp"

#endif  // LIBSEMIGROUPS_TO_PRESENTATION_HPP_
