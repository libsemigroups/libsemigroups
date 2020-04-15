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

// TODO(later):
// 1) remove code duplication
// 2) check if the kernel of the matrix is not null using eigen, so that the
//    semigroup has an infinite homomorphic image.

#ifndef LIBSEMIGROUPS_OBVINF_HPP_
#define LIBSEMIGROUPS_OBVINF_HPP_

#include <cstddef>        // for size_t
#include <string>         // for string
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector
#include <Eigen/QR>       // for dimensionOfKernel
#include <iostream>

#include "libsemigroups-debug.hpp"      // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {
  namespace detail {
    template <typename TLetterType, typename TWordType>
    class IsObviouslyInfinitePairs final {
      using const_iterator =
          typename std::vector<std::pair<TWordType, TWordType>>::const_iterator;

     public:
      explicit IsObviouslyInfinitePairs(size_t n)
          : _empty_word(false), _map(), _nr_gens(n), _preserve(), _unique() {}

      explicit IsObviouslyInfinitePairs(std::string const& lphbt)
          : IsObviouslyInfinitePairs(lphbt.size()) {}

      IsObviouslyInfinitePairs(IsObviouslyInfinitePairs const&) = delete;
      IsObviouslyInfinitePairs(IsObviouslyInfinitePairs&&)      = delete;
      IsObviouslyInfinitePairs& operator=(IsObviouslyInfinitePairs const&)
          = delete;
      IsObviouslyInfinitePairs& operator=(IsObviouslyInfinitePairs&&) = delete;

      void add_rules(const_iterator first, const_iterator last) {
        for (auto it = first; it < last; ++it) {
          if ((*it).first.empty() || (*it).second.empty()) {
            _empty_word = true;
          }
          _map.clear();
          plus_letters_in_word((*it).first);
          if (!_empty_word && _map.size() == 1) {
            _unique.insert((*it).first[0]);
          }
          minus_letters_in_word((*it).second);
          if (!_empty_word && !(*it).second.empty()
              && std::all_of((*it).second.cbegin() + 1,
                             (*it).second.cend(),
                             [&it](TLetterType i) -> bool {
                               return i == (*it).second[0];
                             })) {
            _unique.insert((*it).second[0]);
          }
          for (auto const& x : _map) {
            if (x.second != 0) {
              _preserve.insert(x.first);
            }
          }
        }
      }

      bool result() const {
        return (!_empty_word && _unique.size() != _nr_gens)
               || _preserve.size() != _nr_gens;
      }

     private:
      void letters_in_word(TWordType const& w, size_t adv) {
        for (auto const& x : w) {
          auto it = _map.find(x);
          if (it == _map.end()) {
            _map.emplace(x, adv);
          } else {
            it->second += adv;
          }
        }
      }

      void plus_letters_in_word(TWordType const& w) {
        letters_in_word(w, 1);
      }

      void minus_letters_in_word(TWordType const& w) {
        letters_in_word(w, -1);
      }

      // TLetterType i belongs to "preserve" if there exists a relation where
      // the number of occurrences of i is not the same on both sides of the
      // relation TLetterType i belongs to "unique" if there is a relation
      // where one side consists solely of i.
      bool                                     _empty_word;
      std::unordered_map<TLetterType, int64_t> _map;
      size_t                                   _nr_gens;
      std::unordered_set<TLetterType>          _preserve;
      std::unordered_set<TLetterType>          _unique;
    };

    template <typename TLetterType, typename TWordType>
    class IsObviouslyInfinite final {
      using const_iterator = typename std::vector<TWordType>::const_iterator;

     public:
      explicit IsObviouslyInfinite(size_t n)
          : _empty_word(false), _map(), _nr_gens(n), _preserve(), _unique(), 
            _matrix_col_index(), _matrix() {}

      explicit IsObviouslyInfinite(std::string const& lphbt)
          : IsObviouslyInfinite(lphbt.size()) {}

      IsObviouslyInfinite(IsObviouslyInfinite const&) = delete;
      IsObviouslyInfinite(IsObviouslyInfinite&&)      = delete;
      IsObviouslyInfinite& operator=(IsObviouslyInfinite const&) = delete;
      IsObviouslyInfinite& operator=(IsObviouslyInfinite&&) = delete;

      void add_rules(const_iterator first, const_iterator last) {
        auto matrix_start = _matrix.rows();
        _matrix.conservativeResize(matrix_start+(last-first)/2, 
                                   Eigen::NoChange);
        _matrix.block(matrix_start, 0, 
                      (last-first)/2, _matrix.cols()).setZero();
        for (auto it = first; it < last; it += 2) {
          if ((*it).empty() || (*(it + 1)).empty()) {
            _empty_word = true;
          }
          _map.clear();
          plus_letters_in_word(*it);
          if (!_empty_word && _map.size() == 1) {
            _unique.insert((*it)[0]);
          }
          minus_letters_in_word(*(it + 1));
          if (!_empty_word && !(*(it + 1)).empty()
              && std::all_of((*(it + 1)).cbegin() + 1,
                             (*(it + 1)).cend(),
                             [&it](TLetterType i) -> bool {
                               return i == (*(it + 1))[0];
                             })) {
            _unique.insert((*(it + 1))[0]);
          }
          for (auto const& x : _map) {
            if (x.second != 0) {
              _preserve.insert(x.first);
            }
            auto col = _matrix_col_index.find(x.first);
            if (col == _matrix_col_index.end()) {
              _matrix_col_index.emplace(x.first, _matrix.cols());
              _matrix.conservativeResize(Eigen::NoChange, 
                                         _matrix.cols()+1);
              _matrix.block(0, _matrix.cols()-1, 
                            _matrix.rows(), 1).setZero();
              _matrix(matrix_start+(it-first)/2, _matrix.cols()-1) += x.second;  
            } else {
              //std::cout << "Matrix:" << std::endl;
              //std::cout << _matrix << std::endl;
              //std::cout << "i, j:" << std::endl;
              //std::cout << matrix_start+(it-first)/2 << " " << col->second << std::endl;
              _matrix(matrix_start+(it-first)/2, col->second) += x.second;  
            }
          }
        }
        //std::cout << _matrix << std::endl;
      }

      bool result() const {
        LIBSEMIGROUPS_ASSERT(_matrix.rows()>=0);
        LIBSEMIGROUPS_ASSERT(_matrix.cast<float>().colPivHouseholderQr().rank()>=0);
        return (!_empty_word && _unique.size() != _nr_gens)
               || _preserve.size() != _nr_gens 
               || size_t(_matrix.rows()) < _nr_gens
               || size_t(_matrix.cast<float>().colPivHouseholderQr().rank()) != _nr_gens;
      }

     private:
      void letters_in_word(TWordType const& w, size_t adv) {
        for (auto const& x : w) {
          auto it = _map.find(x);
          if (it == _map.end()) {
            _map.emplace(x, adv);
          } else {
            it->second += adv;
          }
        }
      }

      void plus_letters_in_word(TWordType const& w) {
        letters_in_word(w, 1);
      }

      void minus_letters_in_word(TWordType const& w) {
        letters_in_word(w, -1);
      }

      // TLetterType i belongs to "preserve" if there exists a relation where
      // the number of occurrences of i is not the same on both sides of the
      // relation TLetterType i belongs to "unique" if there is a relation
      // where one side consists solely of i.
      bool                                                   _empty_word;
      std::unordered_map<TLetterType, int64_t>               _map;
      size_t                                                 _nr_gens;
      std::unordered_set<TLetterType>                        _preserve;
      std::unordered_set<TLetterType>                        _unique;
      std::unordered_map<TLetterType, size_t>                _matrix_col_index;
      Eigen::Matrix<int64_t, Eigen::Dynamic, Eigen::Dynamic> _matrix;
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_OBVINF_HPP_
