//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains a helper class for checking whether or not a congruence
// defined by generating pairs or finitely presented semigroup is obviously
// infinite. Currently, all that is checked is that:
//
// 1. For every generator there is at least one side of one relation that
//    consists solely of that generator. If this condition is not met, then
//    there is a generator of infinite order.
//
// 2. The number of occurrences of every generator is not preserved by the
//    relations. Otherwise, it is not possible to use the relations to reduce
//    the number of occurrences of a generator in a word, and so there are
//    infinitely many distinct words.
//
// 3. The number of generators on the left hand side of a relation is not the
//    same as the number of generators on the right hand side for at least
//    one generator. Otherwise the relations preserve the length of any word
//    and so there are infinitely many distinct words.
//
// 4. There are at least as many relations as there are generators. Otherwise
//    we can find a surjective homomorphism onto an infinite subsemigroup of
//    the rationals under addition.
//
// 5. The checks 2., 3. and 4. are a special case of a more general matrix based
//    condition. We construct a matrix whose columns correspond to generators
//    and rows correspond to relations. The (i, j)-th entry is the number of
//    occurences of the j-th generator in the left hand side of the i-th
//    relation minus the number of occurences of it on the right hand side.
//    If this matrix has a non-trivial kernel, then we can construct a
//    surjective homomorphism onto an infinite subsemigroup of the rationals
//    under addition. So we check that the matrix is full rank.
//
// 6. The presentation is not that of a free product. To do this we consider
//    a graph whose vertices are generators and an edge connects two generators
//    if they occur on either side of the same relation. If this graph is
//    disconnected then the presentation is a free product and is therefore
//    infinite. Note that we currently do not consider the case where the
//    identity occurs in the presentation.

// TODO(later):
// 1) remove code duplication

#ifndef LIBSEMIGROUPS_OBVINF_HPP_
#define LIBSEMIGROUPS_OBVINF_HPP_

#include <Eigen/QR>       // for dimensionOfKernel
#include <cstddef>        // for size_t
#include <iterator>       // for next
#include <string>         // for string
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

#include "constants.hpp"            // for UNDEFINED
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "types.hpp"                   // for word_type etc
#include "uf.hpp"                   // for UF
#include "word.hpp"                   // for StringToWord

// TODO (Reinis):
//
// 1) Separate implementation into cpp file.
// 2) IWYU

namespace libsemigroups {
  namespace detail {
    class IsObviouslyInfinite final {
      using const_iterator_word_type =
          typename std::vector<word_type>::const_iterator;
      using const_iterator_pair_string = typename std::vector<
          std::pair<std::string, std::string>>::const_iterator;

     public:
      explicit IsObviouslyInfinite(size_t n)
          : _empty_word(false),
            _nr_gens(n),
            _preserve(n, false),
            _unique(n, false),
            _seen(n, false),
            _matrix(0, n),
            _preserve_length(true),
            _letter_components(n) {}

      explicit IsObviouslyInfinite(std::string const& lphbt)
          : IsObviouslyInfinite(lphbt.size()) {}

      IsObviouslyInfinite(IsObviouslyInfinite const&) = delete;
      IsObviouslyInfinite(IsObviouslyInfinite&&)      = delete;
      IsObviouslyInfinite& operator=(IsObviouslyInfinite const&) = delete;
      IsObviouslyInfinite& operator=(IsObviouslyInfinite&&) = delete;

      void add_rules(const_iterator_word_type first,
                     const_iterator_word_type last) {
        auto matrix_start = _matrix.rows();
        _matrix.conservativeResize(matrix_start + (last - first) / 2,
                                   Eigen::NoChange);
        _matrix.block(matrix_start, 0, (last - first) / 2, _matrix.cols())
            .setZero();
        for (auto it = first; it < last; it += 2) {
          private_add_rule(matrix_start + (it - first)/ 2, *it, *(it + 1));
        }
        _nr_letter_components = _letter_components.nr_blocks();
      }

      void add_rules(std::string const&         lphbt,
                     const_iterator_pair_string first,
                     const_iterator_pair_string last) {
        auto matrix_start = _matrix.rows();
        _matrix.conservativeResize(matrix_start + (last - first),
                                   Eigen::NoChange);
        _matrix.block(matrix_start, 0, (last - first), _matrix.cols())
            .setZero();

        StringToWord stw(lphbt);
        word_type lhs;
        word_type rhs;
        for (auto it = first; it < last; ++it) {
          stw(it->first, lhs);   // lhs changed in-place
          stw(it->second, rhs);  // rhs changed in-place
          private_add_rule(matrix_start + (it - first), lhs, rhs);
        }
        _nr_letter_components = _letter_components.nr_blocks();
      }

      bool result() const {
        LIBSEMIGROUPS_ASSERT(_matrix.rows() >= 0);
        LIBSEMIGROUPS_ASSERT(_matrix.cast<float>().colPivHouseholderQr().rank()
                             >= 0);
        return _preserve_length
               || (!_empty_word
                   && !std::all_of(_unique.begin(),
                                   _unique.end(),
                                   [](bool v) -> bool { return v; }))
               || !std::all_of(_preserve.begin(),
                               _preserve.end(),
                               [](bool v) -> bool { return v; })
               || size_t(_matrix.rows()) < _nr_gens
               || (!_empty_word && _nr_letter_components > 1)
               || size_t(_matrix.cast<float>().colPivHouseholderQr().rank())
                      != _nr_gens;
      }

     private:
      void private_add_rule(size_t const     row_index,
                            word_type const& u,
                            word_type const& v) {
        if (u.empty() || v.empty()) {
          _empty_word = true;
        }
        std::fill(_seen.begin(), _seen.end(), false);
        plus_letters_in_word(row_index, u);
        if (!_empty_word
            && std::all_of(
                u.cbegin() + 1, u.cend(), [&u](letter_type i) -> bool {
                  return i == u[0];
                })) {
          _unique[u[0]] = true;
        }
        minus_letters_in_word(row_index, v);
        if (!_empty_word && !v.empty()
            && std::all_of(
                v.cbegin() + 1,
                v.cend(),
                [&v](letter_type i) -> bool { return i == v[0]; })) {
          _unique[v[0]] = true;
        }
        for (size_t x = 0; x < _nr_gens; x++) {
          if (_matrix(row_index, x) != 0) {
            _preserve[x] = true;
          }
        }
        if (_preserve_length
            && (_matrix.row(row_index).sum() != 0)) {
          _preserve_length = false;
        }
        size_t last_seen = UNDEFINED;
        for (size_t x = 0; x < _nr_gens; ++x) {
          if (_seen[x]) {
            if (last_seen != UNDEFINED) {
              _letter_components.unite(last_seen, x);
            }
            last_seen = x;
          }
        }
      }

      // do not put into cpp file
      void letters_in_word(size_t row, word_type const& w, size_t adv) {
        for (size_t const& x : w) {
          _matrix(row, x) += adv;
          _seen[x] = true;
        }
      }

      // do not put into cpp file
      void plus_letters_in_word(size_t row, word_type const& w) {
        letters_in_word(row, w, 1);
      }

      // do not put into cpp file
      void minus_letters_in_word(size_t row, word_type const& w) {
        letters_in_word(row, w, -1);
      }

      // letter_type i belongs to "preserve" if there exists a relation where
      // the number of occurrences of i is not the same on both sides of the
      // relation letter_type i belongs to "unique" if there is a relation
      // where one side consists solely of i.
      bool                                                   _empty_word;
      size_t                                                 _nr_gens;
      std::vector<bool>                                      _preserve;
      std::vector<bool>                                      _unique;
      std::vector<bool>                                      _seen;
      Eigen::Matrix<int64_t, Eigen::Dynamic, Eigen::Dynamic> _matrix;
      bool                                                   _preserve_length;
      detail::UF                                             _letter_components;
      size_t _nr_letter_components;
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_OBVINF_HPP_
