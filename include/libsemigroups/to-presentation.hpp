//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 James D. Mitchell
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

#include <algorithm>    // for transform, rotate
#include <string>       // for basic_string
#include <type_traits>  // for is_same_v, ena...
#include <utility>      // for move
#include <vector>       // for vector

#include "froidure-pin-base.hpp"   // for FroidurePinBase
#include "knuth-bendix-class.hpp"  // for KnuthBendix
#include "presentation.hpp"        // for Presentation
#include "word-range.hpp"          // for human_readable...

// TODO(1): Make as many of these functions const as possible
namespace libsemigroups {

  //! \defgroup to_presentation_group to<Presentation>
  //!
  //! This file contains documentation for creating semigroup and monoid
  //! presentations in `libsemigroups` from other types of objects.
  //!
  //! \sa \ref to_group for an overview of possible conversions between
  //! `libsemigroups` types.

  //! \defgroup to_inverse_presentation_group to<InversePresentation>
  //!
  //! This file contains documentation for creating inverse semigroup and monoid
  //! presentations in `libsemigroups` from other types of objects.
  //!
  //! \sa \ref to_group for an overview of possible conversions between
  //! `libsemigroups` types.

  ////////////////////////////////////////////////////////////////////////
  // FroidurePin -> Presentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a FroidurePin object.
  //!
  //! Defined in `to-presentation.hpp`.
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<Presentation<Word>>(fp);
  //! \endcode
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
  //! \tparam Result the return type, also used for SFINAE, should be
  //! `Presentation<Word>` for some type \c Word.
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
  //! Defined in `to-presentation.hpp`.
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<Presentation<WordOut>>(kb);
  //! \endcode
  //!
  //! This function constructs and returns a `Presentation<WordOut>` object
  //! using the currently active rules of \p kb.
  //!
  //! No enumeration of the argument \p kb is performed, so it might be the
  //! case that the resulting presentation does not define the same
  //! semigroup/monoid as \p kb. To ensure that the resulting presentation
  //! defines the same semigroup as \p kb, run \ref KnuthBendix::run (or any
  //! other function that fully enumerates \p kb) prior to calling this
  //! function.
  //!
  //! \tparam Result the return type, also used for SFINAE. Must be
  //! `Presentation<WordOut>` for some type `WordOut`.
  //! \tparam WordIn the type of the rules in the presentation of the
  //! \ref_knuth_bendix object \p kb.
  //! \tparam Rewriter the second template parameter for \ref_knuth_bendix.
  //! \tparam ReductionOrder the third template parameter for \ref_knuth_bendix.
  //!
  //! \param kb the \ref_knuth_bendix object from which to obtain the rules.
  //!
  //! \returns An object of type \c Presentation<WordOut>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \note
  //! If the word type of the desired presentation is the same as that \p kb,
  //! then the simpler `to<Presentation>(kb)` may be used instead.
  template <typename Result,
            typename WordIn,
            typename Rewriter,
            typename ReductionOrder>
  auto to(KnuthBendix<WordIn, Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<
          std::is_same_v<Presentation<typename Result::word_type>, Result>,
          Result>;

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a \ref_knuth_bendix object.
  //!
  //! Defined in `to-presentation.hpp`.
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<Presentation>(kb);
  //! \endcode
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
  //! \tparam Thing used for SFINAE, must be Presentation.
  //! \tparam Word the type of the rules in the presentation of the
  //! \ref_knuth_bendix object \p kb.
  //! \tparam Rewriter the second template parameter for \ref_knuth_bendix.
  //! \tparam ReductionOrder the third template parameter for \ref_knuth_bendix.
  //!
  //! \param kb the \ref_knuth_bendix object from which to obtain the rules.
  //!
  //! \returns An object of type \c Presentation<Word>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  // FIXME(1) this is the same hack as elsewhere (deliberately introducing a
  // typo) because Doxygen conflates functions with trailing return type but the
  // same name and signature.
  template <template <typename...> typename Thing,
            typename Word,
            typename Rewriter typename ReductionOrder>
  auto to(KnutBendix<Word, Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<std::is_same_v<Thing<Word>, Presentation<Word>>,
                          Presentation<Word>>;
#else
  template <template <typename...> typename Thing,
            typename Word,
            typename Rewriter,
            typename ReductionOrder>
  auto to(KnuthBendix<Word, Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<std::is_same_v<Thing<Word>, Presentation<Word>>,
                          Presentation<Word>> {
    return to<Presentation<Word>>(kb);
  }
#endif

  ////////////////////////////////////////////////////////////////////////
  // Kambites -> Presentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a kambites
  //!
  //! Defined in `to-presentation.hpp`
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! to<Presentation<Word>>(k);
  //! \endcode
  //!
  //! There are two versions of this function:
  //!
  //! 1. When `typename Result::word_type` and `Word` are not the same, this
  //! function uses `to<Presentation<typename Result::word_type>` to return a
  //! presentation equivalent to the object used to construct or initialise the
  //! Kambites object (if any) but of a different type (for example, can be used
  //! to convert from `std::string` to \ref word_type).
  //!
  //! 2. If the word representations are the same, the function returns a
  //! reference to the presentation used to construct or initialise the Kambites
  //! object (if any) via `k.presentation()`.
  //!
  //! \tparam Result the return type, also used for SFINAE, should be
  //! \c Presentation<T> for some type \c T.
  //! \tparam Word the type of the words in the input Kambites.
  //! \param k the Kambites object from which to obtain the rules.
  //!
  //! \returns A value of type `Presentation<Word>`.
  template <typename Result, typename Word>
  auto to(Kambites<Word>& k) -> std::enable_if_t<
      std::is_same_v<Presentation<typename Result::word_type>, Result>
          && !std::is_same_v<typename Result::word_type, Word>,
      Result> {
    return v4::to<Result>(k.presentation());
  }

  // This function is documented above because Doxygen conflates these two
  // functions
  template <typename Result, typename Word>
  auto to(Kambites<Word>& k) -> std::enable_if_t<
      std::is_same_v<Presentation<typename Result::word_type>, Result>
          && std::is_same_v<typename Result::word_type, Word>,
      Result const&> {
    return k.presentation();
  }

  ////////////////////////////////////////////////////////////////////////
  // Presentation + function -> Presentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a different type of presentation.
  //!
  //! Defined in `to-presentation.hpp`.
  //!
  //! Despite the hideous signature, this function should be invoked as
  //! follows:
  //!
  //! \code
  //! to<Presentation<Word>>(p, func);
  //! \endcode
  //!
  //! This function returns a presentation equivalent to the input
  //! presentation but of a different type (for example, can be used to
  //! convert from `std::string` to \ref word_type). The second parameter
  //! specifies how to map the letters of one presentation to the other.
  //!
  //! \tparam Result the return type, also used for SFINAE, should be
  //! \c Presentation<T> for some type \c T.
  //! \tparam Word the type of the words in the input presentation.
  //! \tparam Func the type of a function for transforming letters.
  //!
  //! \param p the input presentation.
  //! \param f a function mapping `Presentation<Word>::letter_type` to
  //! `Result::letter_type`.
  //!
  //! \returns A value of type `Result`.
  //!
  //! \throws LibsemigroupsException if `p.throw_if_bad_alphabet_or_rules()`
  //! throws.
  //!
  //! \note \c Func must be a function type that is invocable with values of
  //! `Presentation<Word>::letter_type`. If this is not true, then template
  //! substitution will fail.
  //!
  //! \note This function will be moved from the header `to-presentation.hpp`
  //! to `presentation.hpp` in v4 of libsemigroups.
  template <typename Result, typename Word, typename Func>
  [[deprecated]] auto to(Presentation<Word> const& p, Func&& f)
      -> std::enable_if_t<
          std::is_same_v<Presentation<typename Result::word_type>, Result>,
          Result> {
    return v4::to<Result>(p, f);
  }

  ////////////////////////////////////////////////////////////////////////
  // InversePresentation + function -> InversePresentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_inverse_presentation_group
  //!
  //! \brief Make an inverse presentation from a different type of inverse
  //! presentation.
  //!
  //! Defined in `to-presentation.hpp`.
  //!
  //! Despite the hideous signature, this function should be invoked as
  //! follows:
  //!
  //! \code
  //! to<InversePresentation<Word>>(p, func);
  //! \endcode
  //!
  //! This function returns an inverse presentation equivalent to the input
  //! inverse presentation but of a different type (for example, can be used
  //! to convert from `std::string` to \ref word_type). The second parameter
  //! specifies how to map the letters of one inverse presentation to the
  //! other.
  //!
  //! \tparam Result the return type, also used for SFINAE, should be
  //! \c InversePresentation<T> for some type \c T.
  //! \tparam Word the type of the words in the input inverse
  //! presentation.
  //! \tparam Func the type of a function for transforming letters.
  //!
  //! \param ip the input inverse presentation.
  //! \param f a function  mapping `InversePresentation<Word>::letter_type` to
  //! `Result::letter_type`.
  //!
  //! \returns A value of type `InversePresentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if `p.throw_if_bad_alphabet_or_rules()`
  //! throws.
  //!
  //! \note \c Func must be a function type that is invocable with values of
  //! `Presentation<Word>::letter_type`. If this is not true, then template
  //! substitution will fail.
  //!
  //! \note This function will be moved from the header `to-presentation.hpp`
  //! to `presentation.hpp` in v4 of libsemigroups.
  template <typename Result, typename Word, typename Func>
  [[deprecated]] auto
  to(InversePresentation<Word> const& ip, Func&& f) -> std::enable_if_t<
      std::is_same_v<InversePresentation<typename Result::word_type>, Result>,
      Result> {
    return v4::to<Result>(ip, f);
  }

  ////////////////////////////////////////////////////////////////////////
  // Presentation -> Presentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_presentation_group
  //!
  //! \brief Make a presentation from a different type of presentation.
  //!
  //! Defined in `to-presentation.hpp`.
  //!
  //! Despite the hideous signature, this function should be invoked as
  //! follows:
  //!
  //! \code
  //! to<Presentation<Word>>(p, func);
  //! \endcode
  //!
  //! There are 2 versions of this function:
  //!
  //! 1. When `typename Result::word_type` and `Word` are not the same,
  //! this function returns a presentation equivalent to the input
  //! presentation but of a different type (for example, can be used to
  //! convert from `std::string` to \ref word_type).
  //!
  //! 2. When `typename Result::word_type` and `Word` are the same,
  //! this function just returns its argument \p p (as a reference), and is
  //! included solely for the purpose of simplifying certain client code,
  //! where presentations must be converted from one type to another
  //! sometimes, but not other times.
  //!
  //! If the alphabet of of \p p is \f$\{a_0, a_1, \dots a_{n-1}\}\f$, then
  //! the conversion from `Presentation<WordInput>::letter_type` to
  //! `Presentation<WordOutput>::letter_type` is \f$a_i \mapsto\f$
  //! `human_readable_letter<WordOutput>(size_t i)`.
  //!
  //! \tparam Result the return type, also used for SFINAE, should be
  //! \c Presentation<T> for some type \c T.
  //! \tparam Word the type of the words in the input presentation.
  //! \param p the input presentation.
  //!
  //! \returns A value of type `Result`.
  //!
  //! \throws LibsemigroupsException if `typename Result::word_type` and
  //! `Word` are not the same and `p.throw_if_bad_alphabet_or_rules()` throws.
  //! If `typename Result::word_type` and `Word` are the same, then this
  //! function is `noexcept`.
  //!
  //! \note This function will be moved from the header `to-presentation.hpp`
  //! to `presentation.hpp` in v4 of libsemigroups.
  template <typename Result, typename Word>
  [[deprecated]] auto to(Presentation<Word> const& p) -> std::enable_if_t<
      std::is_same_v<Presentation<typename Result::word_type>, Result>
          && !std::is_same_v<typename Result::word_type, Word>,
      Result> {
    return v4::to<Result>(p);
  }

  // This function is documented above because Doxygen conflates these two
  // functions.
  template <typename Result, typename Word>
  [[deprecated]] auto to(Presentation<Word> const& p) noexcept
      -> std::enable_if_t<std::is_same_v<Presentation<Word>, Result>,
                          Result const&> {
    return v4::to<Result>(p);
  }

  ////////////////////////////////////////////////////////////////////////
  // InversePresentation -> InversePresentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_inverse_presentation_group
  //!
  //! \brief Make an inverse presentation from a different type of inverse
  //! presentation.
  //!
  //! Defined in `to-presentation.hpp`.
  //!
  //! Despite the hideous signature, this function should be invoked as
  //! follows:
  //!
  //! \code
  //! to<InversePresentation<Word>>(ip);
  //! \endcode
  //!
  //! There are 2 versions of this function:
  //!
  //! 1. When `typename Result::word_type` and `Word` are not the same,
  //! this function returns an inverse presentation equivalent to the input
  //! inverse presentation but of a different type (for example, can be used
  //! to convert from `std::string` to \ref word_type).
  //!
  //! 2. When `typename Result::word_type` and `Word` are the same,
  //! this function just returns its argument \p ip (as a reference), and is
  //! included solely for the purpose of simplifying certain client code,
  //! where inverse presentations must be converted from one type to another
  //! sometimes, but not other times.
  //!
  //! If the alphabet of of \p ip is \f$\{a_0, a_1, \dots a_{n-1}\}\f$, then
  //! the conversion from `InversePresentation<WordInput>::letter_type` to
  //! `InversePresentation<WordOutput>::letter_type` is \f$a_i \mapsto\f$
  //! `human_readable_letter<WordOutput>(size_t i)`.
  //!
  //! \tparam Result the return type, also used for SFINAE, should be
  //! \c InversePresentation<T> for some type \c T.
  //! \tparam Word the type of the words in the input inverse
  //! presentation.
  //! \param ip the input inverse presentation.
  //!
  //! \returns A value of type `InversePresentation<WordOutput>`.
  //!
  //! \throws LibsemigroupsException if `typename Result::word_type` and
  //! `Word` are not the same and `p.throw_if_bad_alphabet_or_rules()` throws.
  //! If `typename Result::word_type` and `Word` are the same, then this
  //! function is `noexcept`.
  //!
  //! \note This function will be moved from the header `to-presentation.hpp`
  //! to `presentation.hpp` in v4 of libsemigroups.
  template <typename Result, typename Word>
  [[deprecated]] auto
  to(InversePresentation<Word> const& ip) noexcept -> std::enable_if_t<
      std::is_same_v<InversePresentation<typename Result::word_type>, Result>
          && !std::is_same_v<Word, typename Result::word_type>,
      Result> {
    return v4::to<Result>(ip);
  }

  // This function is documented above because Doxygen conflates these two
  // functions.
  template <typename Result, typename Word>
  [[deprecated]] auto to(InversePresentation<Word> const& ip) noexcept
      -> std::enable_if_t<std::is_same_v<InversePresentation<Word>, Result>,
                          Result const&> {
    return v4::to<Result>(ip);
  }

  ////////////////////////////////////////////////////////////////////////
  // Presentation -> InversePresentation
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup to_inverse_presentation_group
  //!
  //! \brief Make an inverse presentation from a presentation.
  //!
  //! Defined in `to-presentation.hpp`.
  //!
  //! Despite the hideous signature, this function should be invoked as
  //! follows:
  //!
  //! \code
  //! to<InversePresentation>(p);
  //! \endcode
  //!
  //! This function returns an inverse presentation with rules equivalent to
  //! those of the input presentation, but over a normalised alphabet. If the
  //! alphabet of \p p is \f$\{a_0, a_1, \dots, a_{n-1}\}\f$, then the
  //! alphabet of the returned inverse presentation will be
  //! \f$\{0, 1, \dots, n-1, n, \dots, 2n-1\}\f$, where the inverse of letter
  //! \f$i\f$ is the letter \f$i + n\, (\text{mod }2n)\f$.
  //!
  //! \tparam Thing used for SFINAE, must be InversePresentation.
  //! \tparam Word the type of the words in the input presentation.
  //! \param p the input presentation.
  //!
  //! \returns A value of type `InversePresentation<Word>`.
  //!
  //! \throws LibsemigroupsException if `p.throw_if_bad_alphabet_or_rules()`
  //! throws.
//!
//! \note This function will be moved from the header `to-presentation.hpp` to
//! `presentation.hpp` in v4 of libsemigroups.
// \note The parameter \p p must not be an `InversePresentation`, otherwise
// a compilation error is thrown.
// NOTE: not sure this is true anymore so just leaving it out
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  // FIXME(1) this is the same hack as elsewhere because Doxygen conflates
  // functions with trailing return type but the same name and signature.
  template <template <typename...> typename Thing, typename Word>
  auto to(Presentation<Word> const& p) -> std::enable_if_t<
      std::is_same_v<InversePresentation<Word>, Thing<Word>>,
      InversePresentation<Word>>;
#else
  template <template <typename...> typename Thing, typename Word>
  [[deprecated]] auto to(Presentation<Word> const& p) -> std::enable_if_t<
      std::is_same_v<InversePresentation<Word>, Thing<Word>>,
      InversePresentation<Word>> {
    return v4::to<InversePresentation>(p);
  }
#endif
}  // namespace libsemigroups

#include "to-presentation.tpp"

#endif  // LIBSEMIGROUPS_TO_PRESENTATION_HPP_
