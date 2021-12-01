//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
// Copyright (C) 2020 Reinis Cirpons
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

#ifndef LIBSEMIGROUPS_OBVINF_HPP_
#define LIBSEMIGROUPS_OBVINF_HPP_

#include <cstddef>  // for size_t
#include <numeric>  // for accumulate
#include <string>   // for string
#include <utility>  // for pair
#include <vector>   // for vector

#include "config.hpp"  // for LIBSEMIGROUPS_EIGEN_ENABLED
#include "types.hpp"   // for word_type etc
#include "uf.hpp"      // for Duf

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
#include <Eigen/Core>
#endif

namespace libsemigroups {
  namespace detail {
    class IsObviouslyInfinite final {
      using const_iterator_word_type =
          typename std::vector<word_type>::const_iterator;
      using const_iterator_pair_string = typename std::vector<
          std::pair<std::string, std::string>>::const_iterator;

     public:
      explicit IsObviouslyInfinite(size_t);

      explicit IsObviouslyInfinite(std::string const& lphbt)
          : IsObviouslyInfinite(lphbt.size()) {}

      IsObviouslyInfinite(IsObviouslyInfinite const&) = delete;
      IsObviouslyInfinite(IsObviouslyInfinite&&)      = delete;
      IsObviouslyInfinite& operator=(IsObviouslyInfinite const&) = delete;
      IsObviouslyInfinite& operator=(IsObviouslyInfinite&&) = delete;

      ~IsObviouslyInfinite();

      void add_rules(const_iterator_word_type first,
                     const_iterator_word_type last);

      void add_rules(std::string const&         lphbt,
                     const_iterator_pair_string first,
                     const_iterator_pair_string last);

      bool result() const;

     private:
      void private_add_rule(size_t const, word_type const&, word_type const&);

      inline void letters_in_word(size_t row, word_type const& w, int64_t adv) {
        for (size_t const& x : w) {
          matrix(row, x) += adv;
          _seen[x] = true;
        }
      }

      inline void plus_letters_in_word(size_t row, word_type const& w) {
        letters_in_word(row, w, 1);
      }

      inline void minus_letters_in_word(size_t row, word_type const& w) {
        letters_in_word(row, w, -1);
      }

      inline int64_t& matrix(size_t row, size_t col) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
        return _matrix(row, col);
#else
        (void) row;
        return _matrix[col];
#endif
      }

      inline bool matrix_row_sums_to_0(size_t row) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
        return _matrix.row(row).sum() == 0;
#else
        (void) row;
        return std::accumulate(_matrix.cbegin(), _matrix.cend(), 0) == 0;
#endif
      }

      // letter_type i belongs to "preserve" if there exists a relation where
      // the number of occurrences of i is not the same on both sides of the
      // relation letter_type i belongs to "unique" if there is a relation
      // where one side consists solely of i.
      bool              _empty_word;
      detail::Duf<>     _letter_components;
      size_t            _nr_gens;
      size_t            _nr_letter_components;
      size_t            _nr_relations;
      bool              _preserve_length;
      std::vector<bool> _preserve;
      std::vector<bool> _seen;
      std::vector<bool> _unique;

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      Eigen::Matrix<int64_t, Eigen::Dynamic, Eigen::Dynamic> _matrix;
#else
      std::vector<int64_t> _matrix;
#endif
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_OBVINF_HPP_
