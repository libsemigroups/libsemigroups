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

  //////////////////////////////////////////////////////////////////////
  // WordRange
  //////////////////////////////////////////////////////////////////////

  namespace v4 {

    template <typename Word>
    void WordRange<Word>::set_iterator() const {
      if (!_current_valid) {
        _current_valid = true;
        _visited       = 0;
        std::visit(
            [&](auto& visitor) { visitor.reset(_upper_bound, _first, _last); },
            _current);
      }
    }

    template <typename Word>
    size_t WordRange<Word>::count() const {
      if (std::holds_alternative<detail::const_wislo_iterator<Word>>(
              _current)) {
        return size_hint();
      } else {
        return (*this | rx::count());
      }
    }

    template <typename Word>
    WordRange<Word>& WordRange<Word>::init() {
      _current_valid = false;
      _first         = {};
      _last          = {};
      _upper_bound   = 0;  // does nothing if the comparison order is lenlex
      _visited       = 0;
      order(LenLexCmp((Alphabet<Word>())));
      return *this;
    }

    template <typename Word>
    WordRange<Word>::WordRange(WordRange const&) = default;

    template <typename Word>
    WordRange<Word>::WordRange(WordRange&&) = default;

    template <typename Word>
    WordRange<Word>& WordRange<Word>::operator=(WordRange const&) = default;

    template <typename Word>
    WordRange<Word>& WordRange<Word>::operator=(WordRange&&) = default;

    template <typename Word>
    WordRange<Word>::~WordRange() = default;

    template <typename Word>
    template <typename Cmp, typename>
    WordRange<Word>& WordRange<Word>::order(Cmp&& cmp) {
      if constexpr (std::is_same_v<Cmp, LenLexCmp<>>
                    || std::is_same_v<Cmp, LenLexCmp<Word>>) {
        _current = cbegin_wislo(cmp.alphabet(), _first, _last);
        _end     = cend_wislo(cmp.alphabet(), _first, _last);
        // NOLINTNEXTLINE(readability/braces)
      } else if constexpr (std::is_same_v<Cmp, LexCmp<>>
                           || std::is_same_v<Cmp, LexCmp<Word>>) {
        _current = cbegin_wilo(cmp.alphabet(), _upper_bound, _first, _last);
        _end     = cend_wilo(cmp.alphabet(), _upper_bound, _first, _last);
      } else {
        _current = cbegin_wio(_upper_bound, _first, _last, cmp);
        _end = cend_wio(_upper_bound, _first, _last, std::forward<Cmp>(cmp));
      }
      _current_valid = true;
      _visited       = 0;
      return *this;
    }

    template <typename Word>
    std::string to_human_readable_repr(WordRange<Word> const& wr,
                                       size_t                 max_width) {
      using detail::group_digits;
      Word const&  first = wr.first();
      Word const&  last  = wr.last();
      size_t const count = wr.count();
      std::string  out;

      bool print_short = false;

      if (first.size() > max_width || last.size() > max_width) {
        print_short = true;
      }

      // TODO(1): re-add order info
      if (!print_short) {
        out = fmt::format("<WordRange of length {} between {} and {} over {}>",
                          group_digits(count),
                          first,
                          last,
                          to_human_readable_repr(wr.alphabet()));
      }

      if (out.size() > max_width) {
        print_short = true;
      }

      if (print_short) {
        out = fmt::format("<WordRange of length {} over {}>",
                          group_digits(count),
                          to_human_readable_repr(wr.alphabet()));
      }

      return out;
    }
  }  // namespace v4

}  // namespace libsemigroups
