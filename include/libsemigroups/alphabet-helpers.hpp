//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

// TODO:
// * IWYU

#ifndef LIBSEMIGROUPS_ALPHABET_HELPERS_HPP_
#define LIBSEMIGROUPS_ALPHABET_HELPERS_HPP_

#include <limits>  // for numeric_limits

#include "alphabet-class.hpp"  // for Alphabet

#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "word-range.hpp"  // for human_readable_letter

//! \ingroup presentations_group
//!
//! \brief Namespace for Alphabet helper functions.
//!
//! Defined in `alphabet-helpers.hpp`.
//!
//! This namespace contains various helper functions for the class
//! \ref Alphabet. These functions could be functions of \ref Alphabet
//! but they only use public member functions of \ref Alphabet, and so
//! they are declared as free functions instead.
namespace libsemigroups::alphabet {
  //! \brief Return the first letter **not** in the alphabet.
  //!
  //! This function returns `words::human_readable_letter<Word>(i)`, where
  //! \c i is the least possible value such that the returned letter does not
  //! belong to \p alphabet.
  //!
  //! \tparam Word the word type of the alphabet.
  //!
  //! \param alphabet the alphabet.
  //!
  //! \returns A value of type `Alphabet<Word>::native_letter_type`.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \warning
  //! This function does not check that an unused letter exists. Use
  //! \ref first_unused_letter if the alphabet might already contain every
  //! supported letter.
  template <typename Word>
  typename Alphabet<Word>::native_letter_type
  first_unused_letter_no_checks(Alphabet<Word> const& alphabet);

  //! \brief Return the first letter **not** in the alphabet.
  //!
  //! This function returns `words::human_readable_letter<Word>(i)`, where
  //! \c i is the least possible value such that the returned letter does not
  //! belong to \p alphabet.
  //!
  //! \tparam Word the word type of the alphabet.
  //!
  //! \param alphabet the alphabet.
  //!
  //! \returns A value of type `Alphabet<Word>::native_letter_type`.
  //!
  //! \throws LibsemigroupsException if \p alphabet already contains the
  //! maximum possible number of letters supported by
  //! `Alphabet<Word>::native_letter_type`.
  template <typename Word>
  typename Alphabet<Word>::native_letter_type
  first_unused_letter(Alphabet<Word> const& alphabet);

  //! \brief Add a letter.
  //!
  //! Adds the first letter not in \p alphabet and returns the added letter.
  //! The added letter is the value returned by
  //! \ref first_unused_letter before the alphabet is modified.
  //!
  //! \param alphabet the alphabet.
  //!
  //! \returns A value of type `Alphabet<Word>::native_letter_type`.
  //!
  //! \throws LibsemigroupsException if \p alphabet already contains the
  //! maximum possible number of letters supported by
  //! `Alphabet<Word>::native_letter_type`.
  template <typename Word>
  typename Alphabet<Word>::native_letter_type
  add_letter(Alphabet<Word>& alphabet);
}  // namespace libsemigroups::alphabet

#include "alphabet-helpers.tpp"

#endif  // LIBSEMIGROUPS_ALPHABET_HELPERS_HPP_
