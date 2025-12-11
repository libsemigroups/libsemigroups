//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell + James W. Swent
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

  template <typename Iterator, typename>
  bool recursive_path_compare(Iterator first1,
                              Iterator last1,
                              Iterator first2,
                              Iterator last2) noexcept {
    if (first2 == last2) {
      // Empty word is not bigger than every word
      return false;
    } else if (first1 == last1) {
      // Empty word is smaller than ever word other than the empty word
      return true;
    }

    bool lastmoved = false;
    --last1;
    --last2;
    while (true) {
      if (last1 < first1) {
        if (last2 < first2) {
          return lastmoved;
        }
        return true;
      }
      if (last2 < first2) {
        return false;
      }
      if (*last1 == *last2) {
        last1--;
        last2--;
      } else if (*last1 < *last2) {
        last1--;
        lastmoved = false;
      } else if (*last2 < *last1) {
        last2--;
        lastmoved = true;
      }
    }
  }

  template <typename Iterator, typename>
  bool wt_shortlex_compare_no_checks(Iterator                   first1,
                                     Iterator                   last1,
                                     Iterator                   first2,
                                     Iterator                   last2,
                                     std::vector<size_t> const& weights) {
    size_t weight1 = std::accumulate(
        first1, last1, size_t(0), [&weights](size_t sum, auto letter) {
          return sum + weights[letter];
        });

    size_t weight2 = std::accumulate(
        first2, last2, size_t(0), [&weights](size_t sum, auto letter) {
          return sum + weights[letter];
        });

    if (weight1 != weight2) {
      return weight1 < weight2;
    }

    return shortlex_compare(first1, last1, first2, last2);
  }
  template <typename Iterator, typename>
  bool wt_shortlex_compare(Iterator                   first1,
                           Iterator                   last1,
                           Iterator                   first2,
                           Iterator                   last2,
                           std::vector<size_t> const& weights) {
    size_t const alphabet_size = weights.size();

    auto const it1 = std::find_if(first1, last1, [&alphabet_size](auto letter) {
      return static_cast<size_t>(letter) >= alphabet_size;
    });
    if (it1 != last1) {
      LIBSEMIGROUPS_EXCEPTION(
          "letter value out of bounds, expected value in [0, {}), found {} in "
          "position {}",
          alphabet_size,
          static_cast<size_t>(*it1),
          std::distance(first1, it1));
    }

    auto const it2 = std::find_if(first2, last2, [&alphabet_size](auto letter) {
      return static_cast<size_t>(letter) >= alphabet_size;
    });
    if (it2 != last2) {
      LIBSEMIGROUPS_EXCEPTION(
          "letter value out of bounds, expected value in [0, {}), found {} in "
          "position {}",
          alphabet_size,
          static_cast<size_t>(*it2),
          std::distance(first2, it2));
    }

    return wt_shortlex_compare_no_checks(first1, last1, first2, last2, weights);
  }

  template <typename Iterator, typename>
  bool wt_lex_compare_no_checks(Iterator                   first1,
                                Iterator                   last1,
                                Iterator                   first2,
                                Iterator                   last2,
                                std::vector<size_t> const& weights) {
    size_t weight1 = std::accumulate(
        first1, last1, size_t(0), [&weights](size_t sum, auto letter) {
          return sum + weights[letter];
        });

    size_t weight2 = std::accumulate(
        first2, last2, size_t(0), [&weights](size_t sum, auto letter) {
          return sum + weights[letter];
        });

    if (weight1 != weight2) {
      return weight1 < weight2;
    }

    return std::lexicographical_compare(first1, last1, first2, last2);
  }

  template <typename Iterator, typename>
  bool wt_lex_compare(Iterator                   first1,
                      Iterator                   last1,
                      Iterator                   first2,
                      Iterator                   last2,
                      std::vector<size_t> const& weights) {
    size_t const alphabet_size = weights.size();

    auto const it1 = std::find_if(first1, last1, [&alphabet_size](auto letter) {
      return static_cast<size_t>(letter) >= alphabet_size;
    });
    if (it1 != last1) {
      LIBSEMIGROUPS_EXCEPTION(
          "letter value out of bounds, expected value in [0, {}), found {} in "
          "position {}",
          alphabet_size,
          static_cast<size_t>(*it1),
          std::distance(first1, it1));
    }

    auto const it2 = std::find_if(first2, last2, [&alphabet_size](auto letter) {
      return static_cast<size_t>(letter) >= alphabet_size;
    });
    if (it2 != last2) {
      LIBSEMIGROUPS_EXCEPTION(
          "letter value out of bounds, expected value in [0, {}), found {} in "
          "position {}",
          alphabet_size,
          static_cast<size_t>(*it2),
          std::distance(first2, it2));
    }

    return wt_lex_compare_no_checks(first1, last1, first2, last2, weights);
  }
}  // namespace libsemigroups
