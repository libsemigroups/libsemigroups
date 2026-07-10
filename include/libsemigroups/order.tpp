//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James D. Mitchell + James W. Swent
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

// This file contains the implementations of several functions and structs
// defining linear orders on words.

namespace libsemigroups {

  namespace detail {

    template <typename Iterator>
    void throw_if_incompat_weights(std::vector<size_t> const& weights,
                                   Iterator                   first,
                                   Iterator                   last) {
      auto const it = std::find_if(first, last, [&weights](auto letter) {
        return static_cast<size_t>(letter) >= weights.size();
      });
      if (it != last) {
        LIBSEMIGROUPS_EXCEPTION("letter value out of bounds, expected value in "
                                "[0, {}), found {} in position {}",
                                weights.size(),
                                static_cast<size_t>(*it),
                                std::distance(first, it));
      }
    }

    template <typename Word, typename Iterator>
    void throw_if_incompat_weights(Alphabet<Word> const&      alphabet,
                                   std::vector<size_t> const& weights,
                                   Iterator                   first,
                                   Iterator                   last) {
      auto const it
          = std::find_if(first, last, [&alphabet, &weights](auto letter) {
              return alphabet.index_no_checks(letter) >= weights.size();
            });
      if (it != last) {
        LIBSEMIGROUPS_EXCEPTION("letter index out of bounds, expected index in "
                                "[0, {}), found index {} in position {}",
                                weights.size(),
                                alphabet.index_no_checks(*it),
                                std::distance(first, it));
      }
    }

    template <typename Iterator>
    [[nodiscard]] size_t weight(std::vector<size_t> const& weights,
                                Iterator                   first,
                                Iterator                   last) {
      return std::accumulate(
          first, last, size_t(0), [&weights](size_t sum, auto letter) {
            return sum + weights[letter];
          });
    }

    template <typename Word, typename Iterator>
    [[nodiscard]] size_t weight(Alphabet<Word> const&      alphabet,
                                std::vector<size_t> const& weights,
                                Iterator                   first,
                                Iterator                   last) {
      return std::accumulate(
          first,
          last,
          size_t(0),
          [&alphabet, &weights](size_t sum, auto letter) {
            return sum + weights[alphabet.index_no_checks(letter)];
          });
    }

  }  // namespace detail

  template <typename Iterator>
  bool rpo_cmp(Iterator first1,
               Iterator last1,
               Iterator first2,
               Iterator last2) noexcept {
    int lastmoved = 0;

    while (true) {
      if (first1 == last1) {
        if (first2 == last2) {
          return lastmoved == 2;
        }
        return true;
      }
      if (first2 == last2) {
        return false;
      }
      if (*first1 == *first2) {
        ++first1;
        ++first2;
      } else if (*first1 < *first2) {
        ++first1;
        lastmoved = 1;
      } else {
        // in this case *first1 > *first2
        ++first2;
        lastmoved = 2;
      }
    }
  }

  template <typename Word, typename Iterator>
  bool rpo_cmp_no_checks(Alphabet<Word> const& alphabet,
                         Iterator              first1,
                         Iterator              last1,
                         Iterator              first2,
                         Iterator              last2) {
    return rpo_cmp(detail::citow(alphabet, first1),
                   detail::citow(alphabet, last1),
                   detail::citow(alphabet, first2),
                   detail::citow(alphabet, last2));
  }

  template <typename Word, typename Iterator>
  bool rpo_cmp(Alphabet<Word> const& alphabet,
               Iterator              first1,
               Iterator              last1,
               Iterator              first2,
               Iterator              last2) {
    alphabet.throw_if_letter_not_in_alphabet(first1, last1);
    alphabet.throw_if_letter_not_in_alphabet(first2, last2);
    return rpo_cmp_no_checks(alphabet, first1, last1, first2, last2);
  }

  template <typename Iterator>
  bool rev_rpo_cmp(Iterator first1,
                   Iterator last1,
                   Iterator first2,
                   Iterator last2) noexcept {
    return rpo_cmp(std::make_reverse_iterator(last1),
                   std::make_reverse_iterator(first1),
                   std::make_reverse_iterator(last2),
                   std::make_reverse_iterator(first2));
  }

  template <typename Word, typename Iterator>
  bool rev_rpo_cmp_no_checks(Alphabet<Word> const& alphabet,
                             Iterator              first1,
                             Iterator              last1,
                             Iterator              first2,
                             Iterator              last2) {
    return rpo_cmp_no_checks(alphabet,
                             std::make_reverse_iterator(last1),
                             std::make_reverse_iterator(first1),
                             std::make_reverse_iterator(last2),
                             std::make_reverse_iterator(first2));
  }

  template <typename Word, typename Iterator>
  bool rev_rpo_cmp(Alphabet<Word> const& alphabet,
                   Iterator              first1,
                   Iterator              last1,
                   Iterator              first2,
                   Iterator              last2) {
    alphabet.throw_if_letter_not_in_alphabet(first1, last1);
    alphabet.throw_if_letter_not_in_alphabet(first2, last2);
    return rev_rpo_cmp_no_checks(alphabet, first1, last1, first2, last2);
  }

  template <typename Iterator>
  bool wt_lenlex_cmp_no_checks(std::vector<size_t> const& weights,
                               Iterator                   first1,
                               Iterator                   last1,
                               Iterator                   first2,
                               Iterator                   last2) {
    size_t const weight1 = detail::weight(weights, first1, last1);
    size_t const weight2 = detail::weight(weights, first2, last2);

    if (weight1 != weight2) {
      return weight1 < weight2;
    }

    return lenlex_cmp(first1, last1, first2, last2);
  }

  template <typename Word, typename Iterator>
  bool wt_lenlex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                               std::vector<size_t> const& weights,
                               Iterator                   first1,
                               Iterator                   last1,
                               Iterator                   first2,
                               Iterator                   last2) {
    size_t const weight1 = detail::weight(alphabet, weights, first1, last1);
    size_t const weight2 = detail::weight(alphabet, weights, first2, last2);

    if (weight1 != weight2) {
      return weight1 < weight2;
    }

    return lenlex_cmp(alphabet, first1, last1, first2, last2);
  }

  template <typename Iterator>
  bool wt_lenlex_cmp(std::vector<size_t> const& weights,
                     Iterator                   first1,
                     Iterator                   last1,
                     Iterator                   first2,
                     Iterator                   last2) {
    detail::throw_if_incompat_weights(weights, first1, last1);
    detail::throw_if_incompat_weights(weights, first2, last2);

    return wt_lenlex_cmp_no_checks(weights, first1, last1, first2, last2);
  }

  template <typename Word, typename Iterator>
  bool wt_lenlex_cmp(Alphabet<Word> const&      alphabet,
                     std::vector<size_t> const& weights,
                     Iterator                   first1,
                     Iterator                   last1,
                     Iterator                   first2,
                     Iterator                   last2) {
    alphabet.throw_if_letter_not_in_alphabet(first1, last1);
    alphabet.throw_if_letter_not_in_alphabet(first2, last2);

    detail::throw_if_incompat_weights(alphabet, weights, first1, last1);
    detail::throw_if_incompat_weights(alphabet, weights, first2, last2);

    return wt_lenlex_cmp_no_checks(
        alphabet, weights, first1, last1, first2, last2);
  }

  template <typename Iterator>
  bool wt_lex_cmp_no_checks(std::vector<size_t> const& weights,
                            Iterator                   first1,
                            Iterator                   last1,
                            Iterator                   first2,
                            Iterator                   last2) {
    size_t weight1 = detail::weight(weights, first1, last1);
    size_t weight2 = detail::weight(weights, first2, last2);

    if (weight1 != weight2) {
      return weight1 < weight2;
    }

    return std::lexicographical_compare(first1, last1, first2, last2);
  }

  template <typename Iterator>
  bool wt_lex_cmp(std::vector<size_t> const& weights,
                  Iterator                   first1,
                  Iterator                   last1,
                  Iterator                   first2,
                  Iterator                   last2) {
    detail::throw_if_incompat_weights(weights, first1, last1);
    detail::throw_if_incompat_weights(weights, first2, last2);

    return wt_lex_cmp_no_checks(weights, first1, last1, first2, last2);
  }
}  // namespace libsemigroups
