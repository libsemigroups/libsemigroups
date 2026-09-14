//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 Joseph Edwards
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

// This file contains implementations for classes related to words and strings
// in libsemigroups.

// TODO(v4): Rename this file to be `word-range.tpp` when we delete the current
// `word-range.hpp`.

namespace libsemigroups {

  //////////////////////////////////////////////////////////////////////
  // cbegin_wio
  //////////////////////////////////////////////////////////////////////

  template <typename Word, typename Cmp, typename>
  [[nodiscard]] detail::const_wio_iterator<Word> cbegin_wio(size_t upper_bound,
                                                            Word const& first,
                                                            Word const& last,
                                                            Cmp&&       cmp) {
    cmp.alphabet().throw_if_letter_not_in_alphabet(first.cbegin(),
                                                   first.cend());
    cmp.alphabet().throw_if_letter_not_in_alphabet(last.cbegin(), last.cend());
    if (!cmp(first, last)) {
      return cend_wio(upper_bound, first, last, std::forward<Cmp>(cmp));
    }

    return detail::const_wio_iterator<Word>(
        upper_bound, first, last, std::forward<Cmp>(cmp));
  }

  //////////////////////////////////////////////////////////////////////
  // cend_wio
  //////////////////////////////////////////////////////////////////////

  template <typename Word, typename Cmp, typename>
  [[nodiscard]] detail::const_wio_iterator<Word>
  cend_wio(size_t upper_bound, Word const&, Word const& last, Cmp&& cmp) {
    return detail::const_wio_iterator<Word>(
        upper_bound, last, last, std::forward<Cmp>(cmp));
  }

  ////////////////////////////////////////////////////////////////////////
  // Word iterators - wilo
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  detail::const_wilo_iterator<Word> cbegin_wilo(Alphabet<Word> const& alphabet,
                                                size_t      upper_bound,
                                                Word const& first,
                                                Word const& last) {
    alphabet.throw_if_letter_not_in_alphabet(first.cbegin(), first.cend());
    alphabet.throw_if_letter_not_in_alphabet(last.cbegin(), last.cend());

    if (!lex_cmp(first, last)) {
      return cend_wilo(alphabet, upper_bound, first, last);
    }

    return detail::const_wilo_iterator<Word>(
        alphabet, upper_bound, first, last);
  }

  template <typename Word>
  detail::const_wilo_iterator<Word> cend_wilo(Alphabet<Word> const& alphabet,
                                              size_t                upper_bound,
                                              Word const&,
                                              Word const& last) {
    return detail::const_wilo_iterator<Word>(alphabet, upper_bound, last, last);
  }

  ////////////////////////////////////////////////////////////////////////
  // Word iterators - wislo
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  detail::const_wislo_iterator<Word>
  cbegin_wislo(Alphabet<Word> const& alphabet,
               Word const&           first,
               Word const&           last) {
    alphabet.throw_if_letter_not_in_alphabet(first.cbegin(), first.cend());
    alphabet.throw_if_letter_not_in_alphabet(last.cbegin(), last.cend());

    if (!lenlex_cmp(first, last)) {
      return cend_wislo(alphabet, first, last);
    }

    return detail::const_wislo_iterator<Word>(alphabet, first, last);
  }

  template <typename Word>
  detail::const_wislo_iterator<Word> cend_wislo(Alphabet<Word> const& alphabet,
                                                Word const&,
                                                Word const& last) {
    return detail::const_wislo_iterator<Word>(alphabet, last, last);
  }

}  // namespace libsemigroups
