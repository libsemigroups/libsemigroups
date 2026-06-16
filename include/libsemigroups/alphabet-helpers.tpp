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

namespace libsemigroups::alphabet {

  template <typename Word>
  typename Alphabet<Word>::native_letter_type
  first_unused_letter_no_checks(Alphabet<Word> const& alphabet) {
    using native_letter_type = typename Alphabet<Word>::native_letter_type;
    using size_type          = typename Word::size_type;

    auto const max_letter = static_cast<size_type>(
        std::numeric_limits<native_letter_type>::max()
        - std::numeric_limits<native_letter_type>::min());

    native_letter_type x;
    for (size_type i = 0; i < max_letter; ++i) {
      x = words::human_readable_letter<Word>(i);
      if (!alphabet.contains(x)) {
        break;
      }
    }
    return x;
  }

  template <typename Word>
  typename Alphabet<Word>::native_letter_type
  first_unused_letter(Alphabet<Word> const& alphabet) {
    using native_letter_type = typename Alphabet<Word>::native_letter_type;
    using size_type          = typename Word::size_type;

    auto const max_letter = static_cast<size_type>(
        std::numeric_limits<native_letter_type>::max()
        - std::numeric_limits<native_letter_type>::min());

    // If the size of native_letter_type is the same as the size of size_t,
    // then the largest possible alphabet [0, max] has size one larger than
    // max. To prevent alphabet.size() overflowing, we don't allow this.
    if constexpr (sizeof(native_letter_type) >= sizeof(size_type)) {
      if (alphabet.letters().size() == max_letter) {
        // This is untestable at present, can't construct an alphabet with
        // such a large number of letters.
        LIBSEMIGROUPS_EXCEPTION(
            "the alphabet of the 1st argument already has the maximum size "
            "of {}, there are no unused letters",
            max_letter);
      }
    } else {
      if (alphabet.letters().size() == max_letter + 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "the alphabet of the 1st argument already has the maximum size "
            "of {}, there are no unused letters",
            std::numeric_limits<native_letter_type>::max()
                - std::numeric_limits<native_letter_type>::min());
      }
    }

    return first_unused_letter_no_checks(alphabet);
  }

  template <typename Word>
  typename Alphabet<Word>::native_letter_type
  add_letter(Alphabet<Word>& alphabet) {
    auto result = first_unused_letter(alphabet);
    alphabet.add_letter_no_checks(result);
    return result;
  }
}  // namespace libsemigroups::alphabet
