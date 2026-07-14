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
        LIBSEMIGROUPS_EXCEPTION(
            "letter value not compatible with weights, expected value in "
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
        LIBSEMIGROUPS_EXCEPTION(
            "letter value not compatible with weights, expected value in "
            "[0, {}), found {} in position {}",
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

    template <typename Word>
    void throw_if_incompat_weights(Alphabet<Word> const&      alphabet,
                                   std::vector<size_t> const& weights) {
      if (alphabet.size() != weights.size()) {
        LIBSEMIGROUPS_EXCEPTION("the alphabet and weights must have the "
                                "same size, but found {} and {}",
                                alphabet.size(),
                                weights.size());
      }
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

  // This algorithm determines if the first sequence (word 1) is strictly
  // smaller than the second sequence (word 2) with respect to a wreath product
  // order. Generators are assigned levels. Differences between generators at
  // higher levels dominate differences at lower levels. Differences within the
  // same level are determined by len-lex.
  //
  // The words are read from right to left, and the dominant level is stored in
  // the variable <relevant_level>. This level is the highest level that is
  // currently determining whether the suffix of word 1 or the suffix of word 2
  // is smaller. The value of <relevant_level> increases every time a generator
  // of a higher level is read.
  //
  // The subword of the suffix of word 1 (respectively, word 2) containing only
  // letters with level equal to <relevant_level> is referred to as the 'head'
  // of word 1 (respectively, word2). If the heads of word 1 and word 2 is the
  // same, then the generators with the level <relevant_level> cannot be used to
  // distinguish between word 1 and word 2. Therefore, the relevant_level is
  // reset to zero.
  //
  // The process of reading letters, updating the <relevant_level> and recording
  // which suffix is smaller is repeated until one of the words has been fully
  // consumed. At this point, a final check of the remaining letters is
  // performed, and the result is returned.
  template <typename Iterator, typename>
  [[nodiscard]] bool wreath_cmp_no_checks(std::vector<size_t> const& levels,
                                          Iterator                   first1,
                                          Iterator                   last1,
                                          Iterator                   first2,
                                          Iterator                   last2) {
    // Remove common prefix
    std::tie(first1, first2) = std::mismatch(first1, last1, first2, last2);

    auto       it1  = std::make_reverse_iterator(last1);
    auto const end1 = std::make_reverse_iterator(first1);
    auto       it2  = std::make_reverse_iterator(last2);
    auto const end2 = std::make_reverse_iterator(first2);

    bool word1_smallest = false;

    // The level which is determining which word is smaller
    size_t relevant_level = 0;

    // Whether the currently parsed head of each word is the same
    bool same_head = true;

    // Process while both words still have elements to compare
    while (it1 != end1 && it2 != end2) {
      size_t level1 = levels[*it1];
      size_t level2 = levels[*it2];

      if (level1 < relevant_level && level2 < relevant_level) {
        ++it1;
        ++it2;
        continue;
      }

      if (level1 < level2) {
        if (level2 > relevant_level) {
          word1_smallest = true;
          same_head      = true;
          relevant_level = level2;
        }
        ++it1;
      } else if (level1 > level2) {
        if (level1 > relevant_level) {
          word1_smallest = false;
          same_head      = true;
          relevant_level = level1;
        }
        ++it2;
      } else {
        // Levels are equal and >= relevant_level
        if (*it1 > *it2) {
          word1_smallest = false;
          same_head      = false;
          relevant_level = level1;
        } else if (*it1 < *it2) {
          word1_smallest = true;
          same_head      = false;
          relevant_level = level1;
        } else if (level1 > relevant_level || same_head) {
          same_head      = true;
          relevant_level = 0;
        }
        ++it1;
        ++it2;
      }
    }

    // Word 1 is exhausted. Check if the remainder of word 2 is larger than
    // word 1.
    while (it2 != end2) {
      if (word1_smallest || levels[*it2] >= relevant_level) {
        return true;
      }
      ++it2;
    }

    // Word 2 is exhausted. Check if the remainder of word 1 is larger than
    // word 2.
    while (it1 != end1) {
      if (!word1_smallest || levels[*it1] >= relevant_level) {
        return false;
      }
      ++it1;
    }

    // Both sequences are fully evaluated
    return word1_smallest;
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

    return lenlex_cmp_no_checks(alphabet, first1, last1, first2, last2);
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

  template <typename Word, typename Iterator>
  bool wt_lex_cmp_no_checks(Alphabet<Word> const&      alphabet,
                            std::vector<size_t> const& weights,
                            Iterator                   first1,
                            Iterator                   last1,
                            Iterator                   first2,
                            Iterator                   last2) {
    size_t weight1 = detail::weight(alphabet, weights, first1, last1);
    size_t weight2 = detail::weight(alphabet, weights, first2, last2);

    if (weight1 != weight2) {
      return weight1 < weight2;
    }

    return lex_cmp_no_checks(alphabet, first1, last1, first2, last2);
  }

  template <typename Word, typename Iterator>
  bool wt_lex_cmp(Alphabet<Word> const&      alphabet,
                  std::vector<size_t> const& weights,
                  Iterator                   first1,
                  Iterator                   last1,
                  Iterator                   first2,
                  Iterator                   last2) {
    alphabet.throw_if_letter_not_in_alphabet(first1, last1);
    alphabet.throw_if_letter_not_in_alphabet(first2, last2);

    detail::throw_if_incompat_weights(alphabet, weights, first1, last1);
    detail::throw_if_incompat_weights(alphabet, weights, first2, last2);

    return wt_lex_cmp_no_checks(
        alphabet, weights, first1, last1, first2, last2);
  }

  template <typename Word>
  WtLenLexCmp<Word>&
  WtLenLexCmp<Word>::init(Alphabet<Word> const&      alphabet,
                          std::vector<size_t> const& weights) {
    detail::throw_if_incompat_weights(alphabet, weights);
    if (&alphabet != &_alphabet) {
      _alphabet = alphabet;
    }
    if (&weights != &_weights) {
      _weights = weights;
    }
    return *this;
  }

  template <typename Word>
  WtLenLexCmp<Word>& WtLenLexCmp<Word>::init(Alphabet<Word>&&      alphabet,
                                             std::vector<size_t>&& weights) {
    detail::throw_if_incompat_weights(alphabet, weights);
    if (&alphabet != &_alphabet) {
      _alphabet = std::move(alphabet);
    }
    if (&weights != &_weights) {
      _weights = std::move(weights);
    }
    return *this;
  }

  template <typename Word>
  WtLenLexCmpNoChecks<Word>&
  WtLenLexCmpNoChecks<Word>::init(Alphabet<Word> const&      alphabet,
                                  std::vector<size_t> const& weights) {
    detail::throw_if_incompat_weights(alphabet, weights);
    if (&alphabet != &_alphabet) {
      _alphabet = alphabet;
    }
    if (&weights != &_weights) {
      _weights = weights;
    }
    return *this;
  }

  template <typename Word>
  WtLenLexCmpNoChecks<Word>&
  WtLenLexCmpNoChecks<Word>::init(Alphabet<Word>&&      alphabet,
                                  std::vector<size_t>&& weights) {
    detail::throw_if_incompat_weights(alphabet, weights);
    if (&alphabet != &_alphabet) {
      _alphabet = std::move(alphabet);
    }
    if (&weights != &_weights) {
      _weights = std::move(weights);
    }
    return *this;
  }

  template <typename Word>
  WtLexCmp<Word>& WtLexCmp<Word>::init(Alphabet<Word> const&      alphabet,
                                       std::vector<size_t> const& weights) {
    detail::throw_if_incompat_weights(alphabet, weights);
    if (&alphabet != &_alphabet) {
      _alphabet = alphabet;
    }
    if (&weights != &_weights) {
      _weights = weights;
    }
    return *this;
  }

  template <typename Word>
  WtLexCmp<Word>& WtLexCmp<Word>::init(Alphabet<Word>&&      alphabet,
                                       std::vector<size_t>&& weights) {
    detail::throw_if_incompat_weights(alphabet, weights);
    if (&alphabet != &_alphabet) {
      _alphabet = std::move(alphabet);
    }
    if (&weights != &_weights) {
      _weights = std::move(weights);
    }
    return *this;
  }

  template <typename Word>
  WtLexCmpNoChecks<Word>&
  WtLexCmpNoChecks<Word>::init(Alphabet<Word> const&      alphabet,
                               std::vector<size_t> const& weights) {
    detail::throw_if_incompat_weights(alphabet, weights);
    if (&alphabet != &_alphabet) {
      _alphabet = alphabet;
    }
    if (&weights != &_weights) {
      _weights = weights;
    }
    return *this;
  }

  template <typename Word>
  WtLexCmpNoChecks<Word>&
  WtLexCmpNoChecks<Word>::init(Alphabet<Word>&&      alphabet,
                               std::vector<size_t>&& weights) {
    detail::throw_if_incompat_weights(alphabet, weights);
    if (&alphabet != &_alphabet) {
      _alphabet = std::move(alphabet);
    }
    if (&weights != &_weights) {
      _weights = std::move(weights);
    }
    return *this;
  }

}  // namespace libsemigroups
