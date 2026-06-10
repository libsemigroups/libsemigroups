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

// This file implements an algorithm for constructing an alphabet ordering such
// that the rules of a presentation are RPO-ordered with respect to that
// alphabet ordering, if possible. The algorithm is a backtrack search algorithm
// for finding a word accepted by the automaton defined in section 4 of Du,
// Narendran and Rusinowitch's article "Inferring RPO symbol orderings"
// (https://doi.org/10.1016/j.jlamp.2026.101126).

#ifndef LIBSEMIGROUPS_DU_NARENDRAN_RUSINOWITCH_HPP_
#define LIBSEMIGROUPS_DU_NARENDRAN_RUSINOWITCH_HPP_

#include <algorithm>      // for mismatch
#include <cstddef>        // for size_t
#include <unordered_set>  // for unordered_set

#include "constants.hpp"     // for UNDEFINED
#include "debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "presentation.hpp"  // for Presentation

#include "detail/containers.hpp"  // for DynamicArray2

namespace libsemigroups {

  namespace detail {
    // Return the next letter in p.alphabet() with index >= index that is not in
    // result.
    template <typename Word>
    auto next_letter_not_in(Presentation<Word> const& p,
                            size_t                    index,
                            Word const&               result) {
      return std::find_if(p.alphabet().begin() + index,
                          p.alphabet().end(),
                          [&result](auto letter) {
                            return std::find(
                                       result.begin(), result.end(), letter)
                                   == result.end();
                          });
    }
  }  // namespace detail

  // This function returns the alphabet of p ordered so that the rules of p
  // satisfy x_i -> y_i and x_i >_rpo y_i with respect to the returned
  // alphabet order. The returned alphabet is empty if this fails.
  template <typename Word>
  Word du_narendran_rusinowitch(Presentation<Word> const& p);

  template <typename Word>
  Word du_narendran_rusinowitch(Presentation<Word> const& p) {
    using native_letter_type = typename Word::value_type;
    p.throw_if_bad_alphabet_or_rules();
    Word result;

    if (p.alphabet().size() == 0) {
      return result;
    }

    size_t const R = p.alphabet().size() + 1;
    size_t const C = p.rules.size();
    if (C == 0) {
      return p.alphabet();
    }

    // At depth <r> of the backtrack algorithm, the the subword of rule <c> that
    // needs to be explored is the word contained in the range
    // [subword_start_indices[r, c], subword_end_indices[c])
    detail::DynamicArray2<size_t> subword_start_indices(C, R);
    std::vector<size_t>           subword_end_indices(C, 0);

    // Don't consider common prefixes and suffixes
    for (size_t c = 0; c < C; c += 2) {
      auto const   lhs = p.rules[c], rhs = p.rules[c + 1];
      size_t const prefix_index = std::distance(
          lhs.begin(),
          std::mismatch(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()).first);

      size_t const suffix_index = std::distance(
          lhs.rbegin(),
          std::mismatch(lhs.rbegin(), lhs.rend(), rhs.rbegin(), rhs.rend())
              .first);

      if (prefix_index == rhs.size()) {
        // If the rhs is empty, set the indices to be UNDEFINED to indicate no
        // work needs to be done on the subword
        subword_start_indices.set(0, c, UNDEFINED);
        subword_start_indices.set(0, c + 1, UNDEFINED);
        subword_end_indices[c]     = UNDEFINED;
        subword_end_indices[c + 1] = UNDEFINED;
      } else if (prefix_index == lhs.size()) {
        // If the lhs is empty and the rhs is not, no valid alphabet order will
        // exist
        return result;
      } else {
        subword_start_indices.set(0, c, prefix_index);
        subword_start_indices.set(0, c + 1, prefix_index);
        subword_end_indices[c]     = lhs.size() - suffix_index;
        subword_end_indices[c + 1] = rhs.size() - suffix_index;
      }
    }

    native_letter_type letter = p.letter_no_checks(0);

    // If true, then there exists some rule lhs -> rhs where there are more
    // occurrences of <letter> in rhs than lhs.
    bool contains_incompatible_rules = false;

    while (result.size() < p.alphabet().size()) {
      // If the rules are compatible with the current partial-alphabet defined
      // by <result>, then we should define <letter> to be the first available
      // letter in <p.alphabet()> that is not already in <result>. Otherwise,
      // there is some rule subword for which there are more instance of
      // <letter> in the right hand side than the left hand side. In this case,
      // we need to set <letter> to the next unused letter and check again. If
      // there are no possible letters, then it is not possible to extend the
      // partial-alphabet defined by <result>, so we back-track by popping a
      // letter from the back of <result> and continuing.
      if (!contains_incompatible_rules) {
        letter = *detail::next_letter_not_in(p, 0, result);
      } else {
        auto const it = detail::next_letter_not_in(
            p, p.index_no_checks(letter) + 1, result);

        // Backtrack if possible, or return if not.
        if (it == p.alphabet().end()) {
          if (result.empty()) {
            return result;
          }
          letter = result.back();
          result.pop_back();
          continue;
        }
        letter = *it;
      }

      size_t const r              = result.size();
      contains_incompatible_rules = false;

      // Indicates whether all rules are correctly oriented with respect to the
      // current partial-alphabet, and therefore we can stop early.
      bool all_rules_oriented = true;

      for (size_t c = 0; c < C; c += 2) {
        auto const   lhs = p.rules[c], rhs = p.rules[c + 1];
        size_t const lhs_start = subword_start_indices.get(r, c);
        size_t const lhs_end   = subword_end_indices[c];

        if (lhs_start == UNDEFINED) {
          // This rule is already correctly oriented by the partial-alphabet
          // order in <result>.
          LIBSEMIGROUPS_ASSERT(subword_start_indices.get(r, c + 1)
                               == UNDEFINED);
          subword_start_indices.set(r + 1, c, UNDEFINED);
          subword_start_indices.set(r + 1, c + 1, UNDEFINED);
          continue;
        }
        all_rules_oriented = false;

        size_t const lhs_count = std::count(
            lhs.begin() + lhs_start, lhs.begin() + lhs_end, letter);

        size_t const rhs_start = subword_start_indices.get(r, c + 1);
        size_t const rhs_end   = subword_end_indices[c + 1];
        size_t const rhs_count = std::count(
            rhs.begin() + rhs_start, rhs.begin() + rhs_end, letter);

        if (lhs_count > rhs_count) {
          // Indicate success
          subword_start_indices.set(r + 1, c, UNDEFINED);
          subword_start_indices.set(r + 1, c + 1, UNDEFINED);
        } else if (lhs_count < rhs_count) {
          // Indicate failure
          contains_incompatible_rules = true;
          break;
        } else {
          // Update the subword index to contain the longest suffix of <lhs> and
          // <rhs> that does not contain <letter>
          size_t const new_lhs_start
              = lhs.size()
                - std::distance(
                    lhs.rbegin(),
                    std::find(
                        std::make_reverse_iterator(lhs.begin() + lhs_end),
                        std::make_reverse_iterator(lhs.begin() + lhs_start),
                        letter));
          size_t const new_rhs_start
              = rhs.size()
                - std::distance(
                    rhs.rbegin(),
                    std::find(
                        std::make_reverse_iterator(rhs.begin() + rhs_end),
                        std::make_reverse_iterator(rhs.begin() + rhs_start),
                        letter));

          if (new_rhs_start == rhs_end) {
            subword_start_indices.set(r + 1, c, UNDEFINED);
            subword_start_indices.set(r + 1, c + 1, UNDEFINED);
          } else if (new_lhs_start == lhs_end) {
            contains_incompatible_rules = true;
            break;
          } else {
            subword_start_indices.set(r + 1, c, new_lhs_start);
            subword_start_indices.set(r + 1, c + 1, new_rhs_start);
          }
        }
      }

      // If all of the rules are correctly oriented with respect to the current
      // partial-alphabet, fill the remaining spots of the alphabet arbitrarily.
      if (all_rules_oriented) {
        for (letter_type const l : p.alphabet()) {
          if (std::find(result.begin(), result.end(), l) == result.end()) {
            result.push_back(l);
          }
        }
        break;
      }

      if (!contains_incompatible_rules) {
        result.push_back(letter);
      }
    }
    std::reverse(result.begin(), result.end());
    return result;
  }

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_DU_NARENDRAN_RUSINOWITCH_HPP_
