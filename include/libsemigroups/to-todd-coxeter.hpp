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
//
// This file contains declarations for overloads of the "to" function for
// outputting a ToddCoxeter object.

#ifndef LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_TO_TODD_COXETER_HPP_

#include <type_traits>  // for is_same_v, enable_if_t

#include "constants.hpp"           // for POSITIVE_INFINITY
#include "exception.hpp"           // for LIBSEMIGROUPS_EXCEPTION
#include "froidure-pin.hpp"        // for FroidurePin
#include "to-froidure-pin.hpp"     // for to<FroidurePin>
#include "todd-coxeter-class.hpp"  // for KnuthBendix
#include "todd-coxeter-class.hpp"  // for ToddCoxeter

namespace libsemigroups {

  // TODO(1):
  // * to<ToddCoxeter> for FroidurePin<TCE> just return the original
  // ToddCoxeter instance.
  // * to<ToddCoxeter<std::string>>(ToddCoxeter<word_type> const&)
  // * to<ToddCoxeter<std::string>>(KnuthBendix<word_type> const&)

  //! \defgroup to_todd_coxeter_group to<ToddCoxeter>
  //! \ingroup todd_coxeter_group
  //!
  //! \brief Convert to \ref_todd_coxeter instance.
  //!
  //! This page contains documentation related to converting `libsemigroups`
  //! objects into \ref_todd_coxeter instances.
  //!
  //! \sa \ref to_group for an overview of possible conversions between
  //! `libsemigroups` types.

  //! \ingroup to_todd_coxeter_group
  //!
  //! \brief Convert a \ref FroidurePin object to a \ref_todd_coxeter object.
  //!
  //! Defined in \c to-todd-coxeter.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
//! \skip-test
//! to<ToddCoxeter<Word>>(knd, fpb, wg);
//! \endcode
//!
//! This function converts the \ref FroidurePin object \p fpb into a
//! \ref_todd_coxeter object using the WordGraph \p wg (which should be either
//! the \ref FroidurePinBase::left_cayley_graph or the
//! \ref FroidurePinBase::right_cayley_graph of \p fpb).
//!
//! \tparam Result used for SFINAE, the return type of this function, must be
//! \c ToddCoxeter<Word> for some type \c Word.
//! \tparam Node the type of the nodes in the WordGraph \p wg.
//!
//! \param knd the kind of the congruence being constructed.
//! \param fpb the FroidurePin instance to be converted.
//! \param wg the left or right Cayley graph of \p fpb.
//!
//! \returns A \ref_todd_coxeter object representing the trivial congruence
//! over the semigroup defined by \p fpb.
//!
//! \throws LibsemigroupsException if \p wg is not the
//! \ref FroidurePinBase::left_cayley_graph or the
//! \ref FroidurePinBase::right_cayley_graph of \p fpb.
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  // FIXME(1) doxygen conflates this version of "to" with the one of the same
  // signature in "to-cong.hpp" and so we misspell one of the types of the
  // parameters to avoid this
  template <typename Result, typename Node>
  auto to(congruence_knd knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<ToddCoxeter<typename Result::native_word_type>,
                         Result>,
          Result>;
#else
  template <typename Result, typename Node>
  auto to(congruence_kind knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<ToddCoxeter<typename Result::native_word_type>,
                         Result>,
          Result>;
#endif

  //! \ingroup to_todd_coxeter_group
  //!
  //! \brief Convert a \ref_knuth_bendix object to a \ref_todd_coxeter object.
  //!
  //! Defined in \c to-todd-coxeter.hpp
  //!
  //! Despite the hideous signature, this function should be invoked as follows:
  //!
  //! \code
  //! \skip-test
  //! to<ToddCoxeter>(kb);
  //! \endcode
  //!
  //! This function converts the \ref_knuth_bendix object \p kb into a
  //! \ref_todd_coxeter object using the right Cayley graph of the semigroup
  //! represented by \p kb.
  //!
  //! \tparam Thing used for SFINAE, must be \ref_todd_coxeter.
  //! \tparam Word the type of the words used in relations in \p kb.
  //! \tparam Rewriter the type of rewriter used by \p kb.
  //! \tparam ReductionOrder the type of reduction ordering used by \p kb.
  //!
  //! \param knd the kind of the congruence being constructed.
  //! \param kb the \ref_knuth_bendix object being converted.
  //!
  //! \returns A \ref_todd_coxeter object representing the trivial congruence
  //! over the semigroup defined by \p kb.
  //!
  //! \throws LibsemigroupsException if `kb.kind()` is not
  //! \ref congruence_kind::twosided.
  //!
  //! \throws LibsemigroupsException if `kb.number_of_classes()` is not finite.
  //! Use ToddCoxeter(knd, kb.presentation()) in this case.
  template <template <typename...> typename Thing,
            typename Word,
            typename Rewriter,
            typename ReductionOrder>
  auto to(congruence_kind knd, KnuthBendix<Word, Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<std::is_same_v<ToddCoxeter<Word>, Thing<Word>>,
                          ToddCoxeter<Word>>;

}  // namespace libsemigroups

#include "to-todd-coxeter.tpp"
#endif  // LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
