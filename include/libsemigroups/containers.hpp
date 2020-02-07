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

#ifndef LIBSEMIGROUPS_CONTAINERS_HPP_
#define LIBSEMIGROUPS_CONTAINERS_HPP_

#include <array>     // for array
#include <cstddef>   // for size_t
#include <iterator>  // for reverse_iterator
#include <vector>    // for vector, allocator

#include "iterator.hpp"             // for ConstIteratorStateful, ConstItera...
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {
  namespace detail {

    // Template class for 2-dimnensional dynamic arrays.
    template <typename T, typename A = std::allocator<T>>
    class DynamicArray2 final {
      // So that DynamicArray2<T> can access private data members of
      // DynamicArray2<S> and vice versa.
      template <typename S, typename B>
      friend class DynamicArray2;

     public:
      using allocator_type  = A;
      using value_type      = typename std::vector<T, A>::value_type;
      using reference       = typename std::vector<T, A>::reference;
      using const_reference = typename std::vector<T, A>::const_reference;
      using difference_type = typename std::vector<T, A>::difference_type;
      using size_type       = typename std::vector<T, A>::size_type;
      using const_pointer   = typename std::vector<T, A>::const_pointer;
      using pointer         = typename std::vector<T, A>::pointer;

      DynamicArray2(DynamicArray2 const&) = default;
      DynamicArray2(DynamicArray2&&)      = default;
      DynamicArray2& operator=(DynamicArray2 const&) = default;
      DynamicArray2& operator=(DynamicArray2&&) = default;

      ~DynamicArray2() = default;

      // Not noexcept because DynamicArray2::add_rows can throw.
      explicit DynamicArray2(size_t nr_cols     = 0,
                             size_t nr_rows     = 0,
                             T      default_val = 0)
          : _vec(),
            _nr_used_cols(nr_cols),
            _nr_unused_cols(0),
            _nr_rows(0),
            _default_val(default_val) {
        this->add_rows(nr_rows);
      }

      // Not noexcept because DynamicArray2::DynamicArray2(size_t, size_t) can
      // throw.
      explicit DynamicArray2(std::initializer_list<std::initializer_list<T>> il)
          : DynamicArray2(il.begin()->size(), il.size()) {
        auto it = _vec.begin();
        for (auto const& row : il) {
          LIBSEMIGROUPS_ASSERT(row.size() == _nr_used_cols);
          std::copy(row.begin(), row.end(), it);
          it += _nr_used_cols + _nr_unused_cols;
        }
      }

      // Not noexcept
      template <typename S, typename B>
      DynamicArray2(DynamicArray2<S, B> const& copy, size_t nr_cols_to_add = 0)
          : _vec(),
            _nr_used_cols(copy._nr_used_cols),
            _nr_unused_cols(copy._nr_unused_cols),
            _nr_rows(copy.nr_rows()),
            _default_val(copy._default_val) {
        if (nr_cols_to_add <= _nr_unused_cols) {
          _vec.assign(copy._vec.cbegin(), copy._vec.cend());
          _nr_used_cols += nr_cols_to_add;
          _nr_unused_cols -= nr_cols_to_add;
          return;
        }

        size_t new_nr_cols
            = std::max(2 * nr_cols(), nr_cols_to_add + nr_cols());
        _nr_used_cols += nr_cols_to_add;
        _nr_unused_cols = new_nr_cols - _nr_used_cols;

        _vec.reserve(new_nr_cols * _nr_rows);
        // TODO improve
        for (size_t i = 0; i < _nr_rows; i++) {
          size_t j;
          for (j = 0; j < copy._nr_used_cols; j++) {
            _vec.push_back(copy.get(i, j));
          }
          for (; j < new_nr_cols; j++) {
            _vec.push_back(_default_val);
          }
        }
      }

      // Not noexcept, since std::equal can throw
      bool operator==(DynamicArray2<T, A> const& that) const {
        if (_nr_used_cols != that._nr_used_cols || _nr_rows != that._nr_rows) {
          return false;
        }
        return std::equal(cbegin(), cend(), that.cbegin());
      }

      // Not noexcept, since operator== can throw
      bool operator!=(DynamicArray2<T, A> const& that) const {
        return !operator==(that);
      }

      bool empty() const noexcept {
        return _nr_rows == 0;
      }

      size_t size() const noexcept {
        return _nr_rows * _nr_used_cols;
      }

      size_t max_size() const noexcept {
        return _vec.max_size();
      }

      // Not noexcept, since std::filll can throw
      void fill(T const& val) {
        std::fill(_vec.begin(), _vec.end(), val);
      }

      void swap(DynamicArray2<T, A>& that) noexcept {
        _vec.swap(that._vec);
        std::swap(_nr_used_cols, that._nr_used_cols);
        std::swap(_nr_unused_cols, that._nr_unused_cols);
        std::swap(_nr_rows, that._nr_rows);
      }

      // Not noexcept because std::vector::operator[] isn't
      void swap(size_t i, size_t j, size_t k, size_t l) {
        LIBSEMIGROUPS_ASSERT(i < _nr_rows);
        LIBSEMIGROUPS_ASSERT(j < _nr_used_cols);
        LIBSEMIGROUPS_ASSERT(k < _nr_rows);
        LIBSEMIGROUPS_ASSERT(l < _nr_used_cols);
        std::swap(_vec[i * (_nr_used_cols + _nr_unused_cols) + j],
                  _vec[k * (_nr_used_cols + _nr_unused_cols) + l]);
      }

      // Not noexcept since std::swap_ranges can throw.
      void swap_rows(size_t i, size_t j) {
        size_t const nr_cols = _nr_used_cols + _nr_unused_cols;
        std::swap_ranges(_vec.begin() + (i * nr_cols),
                         _vec.begin() + ((i + 1) * nr_cols),
                         _vec.begin() + (j * nr_cols));
      }

      // The following is adapted from http://bit.ly/2X4xPlK
      // TODO(later) 1. make this a non-member function
      //             2. should perform checks that p actually permutes the
      //                given row
      // Not noexcept because std::vector::operator[] isn't
      void apply_row_permutation(std::vector<size_t> p) {
        for (size_t i = 0; i < p.size(); i++) {
          size_t current = i;
          while (i != p[current]) {
            size_t next = p[current];
            swap_rows(current, next);
            p[current] = current;
            current    = next;
          }
          p[current] = current;
        }
      }

      void clear() noexcept {
        _nr_unused_cols += _nr_used_cols;
        _nr_used_cols = 0;
        _nr_rows      = 0;
        _vec.clear();  // noexcept
      }

      // Throws if the assignment operator of T throws
      void shrink_rows_to(size_t n) {
        if (n < _nr_rows) {
          _vec.erase(_vec.begin() + n * (_nr_used_cols + _nr_unused_cols),
                     _vec.end());
          _vec.shrink_to_fit();
          _nr_rows = n;
        }
      }

      // Throws if the assignment operator of T throws
      void erase_column(size_t i) {
        LIBSEMIGROUPS_ASSERT(i < _nr_used_cols);
        size_t const n = _nr_used_cols + _nr_unused_cols;
        for (auto it = _vec.rbegin() + (n - i); it < _vec.rend(); it += n) {
          _vec.erase(it.base());
        }
        _nr_used_cols--;
      }

      // Not noexcept since std::vector::resize can throw.
      void inline add_rows(size_t nr) {
        _nr_rows += nr;
        if (nr != 0) {
          _vec.resize(_vec.size() + (_nr_used_cols + _nr_unused_cols) * nr,
                      _default_val);
        }
      }

      void inline set_default_value(T val) noexcept {
        _default_val = val;
      }

      // Not noexcept
      void add_cols(size_t nr) {
        if (nr <= _nr_unused_cols) {
          _nr_used_cols += nr;
          _nr_unused_cols -= nr;
          return;
        }

        size_t old_nr_cols = _nr_used_cols + _nr_unused_cols;
        size_t new_nr_cols = std::max(2 * old_nr_cols, nr + old_nr_cols);
        if (_nr_rows != 0) {
          _vec.resize(new_nr_cols * _nr_rows, _default_val);

          typename std::vector<T>::iterator old_it(
              _vec.begin() + (old_nr_cols * _nr_rows) - old_nr_cols);
          typename std::vector<T>::iterator new_it(
              _vec.begin() + (new_nr_cols * _nr_rows) - new_nr_cols);

          while (old_it != _vec.begin()) {
            std::move(old_it, old_it + _nr_used_cols, new_it);
            old_it -= old_nr_cols;
            new_it -= new_nr_cols;
          }
        }
        _nr_used_cols += nr;
        _nr_unused_cols = new_nr_cols - _nr_used_cols;
      }

      // Not noexcept because std::vector::operator[] isn't
      void inline set(size_t i, size_t j, T val) {
        LIBSEMIGROUPS_ASSERT(i < _nr_rows && j < _nr_used_cols);
        _vec[i * (_nr_used_cols + _nr_unused_cols) + j] = val;
      }

      // Not noexcept because std::vector::operator[] isn't
      inline T get(size_t i, size_t j) const {
        LIBSEMIGROUPS_ASSERT(i < _nr_rows && j < _nr_used_cols);
        return _vec[i * (_nr_used_cols + _nr_unused_cols) + j];
      }

      size_t nr_rows() const noexcept {
        return _nr_rows;
      }

      size_t nr_cols() const noexcept {
        return _nr_used_cols;
      }

      // Not noexcept
      template <typename S, typename B>
      void append(const DynamicArray2<S, B>& copy) {
        LIBSEMIGROUPS_ASSERT(copy._nr_used_cols == _nr_used_cols);

        size_t old_nr_rows = _nr_rows;
        add_rows(copy._nr_rows);

        if (copy._nr_unused_cols == _nr_unused_cols) {
          std::copy(copy._vec.begin(),
                    copy._vec.end(),
                    _vec.begin()
                        + (_nr_used_cols + _nr_unused_cols) * old_nr_rows);
        } else {  // TODO(later) improve this
          for (size_t i = old_nr_rows; i < _nr_rows; i++) {
            for (size_t j = 0; j < _nr_used_cols; j++) {
              set(i, j, copy.get(i - old_nr_rows, j));
            }
          }
        }
      }

      // Not noexcept
      void reserve(size_t nr_rows) {
        _vec.reserve(nr_rows * (_nr_unused_cols + _nr_used_cols));
      }

     private:
      std::vector<T, A> _vec;
      size_t            _nr_used_cols;
      size_t            _nr_unused_cols;
      size_t            _nr_rows;
      T                 _default_val;

      // Helper functions for iterators
      static inline size_type remainder(difference_type a,
                                        difference_type b) noexcept {
        if (a >= 0) {
          return a % b;
        } else {
          return (a % b) + b;
        }
      }

      static inline size_type quotient(difference_type a,
                                       difference_type b) noexcept {
        if (a % b >= 0) {
          return a / b;
        } else {
          return (a / b) - 1;
        }
      }

      // Types for ConstIteratorStateful
      template <typename TInternalIteratorType>
      friend struct AddAssign;

      template <typename TInternalIteratorType>
      struct AddAssign {
        TInternalIteratorType& operator()(DynamicArray2 const*   da,
                                          TInternalIteratorType& it,
                                          size_t val) const noexcept {
          if (da->_nr_unused_cols == 0 || val == 0) {
            return it += val;
          }
          size_type s = da->nr_cols();
          size_type n = da->_nr_unused_cols;

          size_type q = val / s;
          size_type r = val % s;

          difference_type p = it - da->_vec.cbegin();
          it += q * (s + n) + r;

          if (r + remainder(p, s + n) >= s) {
            it += n;
          }
          return it;
        }
      };

      template <typename TInternalIteratorType>
      friend struct SubtractAssign;

      template <typename TInternalIteratorType>
      struct SubtractAssign {
        TInternalIteratorType& operator()(DynamicArray2 const*   da,
                                          TInternalIteratorType& it,
                                          size_t val) const noexcept {
          if (da->_nr_unused_cols == 0 || val == 0) {
            return it -= val;
          }
          size_type s = da->nr_cols();
          size_type n = da->_nr_unused_cols;

          size_type q = val / s;
          size_type r = val % s;

          difference_type p = it - da->_vec.cbegin();
          it -= (q * (s + n) + r);
          if (remainder(p, s + n) < r) {
            it -= n;
          }
          return it;
        }
      };

      template <typename TOperator, typename TInternalIteratorType>
      struct PrefixIncrement {
        TInternalIteratorType& operator()(DynamicArray2 const*   da,
                                          TInternalIteratorType& it) const
            noexcept {
          return TOperator()(da, it, 1);
        }
      };

      template <typename TOperator, typename TInternalIteratorType>
      struct PrefixDecrement {
        TInternalIteratorType& operator()(DynamicArray2 const*   da,
                                          TInternalIteratorType& it) const
            noexcept {
          return TOperator()(da, it, 1);
        }
      };

      template <typename TInternalIteratorType>
      friend struct Difference;

      template <typename TInternalIteratorType>
      struct Difference {
        inline difference_type
        operator()(DynamicArray2 const*         da,
                   TInternalIteratorType const& it1,
                   TInternalIteratorType const& it2) const noexcept {
          difference_type s = da->_nr_used_cols;
          difference_type n = da->_nr_unused_cols;
          difference_type b = it1 - da->_vec.begin();
          difference_type a = it2 - da->_vec.begin();
          return (b - a) - n * (quotient(b, s + n) - quotient(a, s + n));
        }
      };

      template <typename TInternalIteratorType>
      friend struct ColumnAddAssign;

      template <typename TInternalIteratorType>
      struct ColumnAddAssign {
        TInternalIteratorType& operator()(DynamicArray2 const*   da,
                                          TInternalIteratorType& it,
                                          size_t val) const noexcept {
          return it += val * (da->_nr_used_cols + da->_nr_unused_cols);
        }
      };

      template <typename TInternalIteratorType>
      friend struct ColumnSubtractAssign;

      template <typename TInternalIteratorType>
      struct ColumnSubtractAssign {
        TInternalIteratorType& operator()(DynamicArray2 const*   da,
                                          TInternalIteratorType& it,
                                          size_t val) const noexcept {
          return it -= val * (da->_nr_used_cols + da->_nr_unused_cols);
        }
      };

      template <typename TInternalIteratorType>
      friend struct ColumnDifference;

      template <typename TInternalIteratorType>
      struct ColumnDifference {
        inline difference_type
        operator()(DynamicArray2 const*         da,
                   TInternalIteratorType const& it1,
                   TInternalIteratorType const& it2) const noexcept {
          return (it1 - it2)
                 / static_cast<difference_type>(da->_nr_used_cols
                                                + da->_nr_unused_cols);
        }
      };

      // Aliases for iterators
      using internal_const_iterator_type =
          typename std::vector<T, A>::const_iterator;

      using internal_iterator_type = typename std::vector<T, A>::iterator;

      struct IteratorTraits : public detail::IteratorTraits<std::vector<T, A>> {
        using state_type = DynamicArray2<T, A>*;
        using AddAssign  = DynamicArray2::AddAssign<internal_iterator_type>;
        using SubtractAssign
            = DynamicArray2::SubtractAssign<internal_iterator_type>;
        using PrefixIncrement = DynamicArray2::PrefixIncrement<
            DynamicArray2::AddAssign<internal_iterator_type>,
            internal_iterator_type>;
        using PrefixDecrement = DynamicArray2::PrefixDecrement<
            DynamicArray2::SubtractAssign<internal_iterator_type>,
            internal_iterator_type>;
        using Difference = DynamicArray2::Difference<internal_iterator_type>;
      };

      struct ConstIteratorTraits
          : public detail::ConstIteratorTraits<std::vector<T, A>> {
        using state_type = DynamicArray2<T, A> const*;
        using AddAssign
            = DynamicArray2::AddAssign<internal_const_iterator_type>;
        using SubtractAssign
            = DynamicArray2::SubtractAssign<internal_const_iterator_type>;
        using PrefixIncrement = DynamicArray2::PrefixIncrement<
            DynamicArray2::AddAssign<internal_const_iterator_type>,
            internal_const_iterator_type>;
        using PrefixDecrement = DynamicArray2::PrefixDecrement<
            DynamicArray2::SubtractAssign<internal_const_iterator_type>,
            internal_const_iterator_type>;
        using Difference
            = DynamicArray2::Difference<internal_const_iterator_type>;
      };

      struct ColumnIteratorTraits
          : public detail::IteratorTraits<std::vector<T, A>> {
        using state_type      = DynamicArray2<T, A>*;
        using AddAssign       = ColumnAddAssign<internal_iterator_type>;
        using SubtractAssign  = ColumnSubtractAssign<internal_iterator_type>;
        using PrefixIncrement = DynamicArray2::PrefixIncrement<
            ColumnAddAssign<internal_iterator_type>,
            internal_iterator_type>;
        using PrefixDecrement = DynamicArray2::PrefixDecrement<
            ColumnSubtractAssign<internal_iterator_type>,
            internal_iterator_type>;
        using Difference = ColumnDifference<internal_iterator_type>;
      };

      struct ConstColumnIteratorTraits
          : public detail::ConstIteratorTraits<std::vector<T, A>> {
        using state_type = DynamicArray2<T, A> const*;
        using AddAssign  = ColumnAddAssign<internal_const_iterator_type>;
        using SubtractAssign
            = ColumnSubtractAssign<internal_const_iterator_type>;
        using PrefixIncrement = DynamicArray2::PrefixIncrement<
            ColumnAddAssign<internal_const_iterator_type>,
            internal_const_iterator_type>;
        using PrefixDecrement = DynamicArray2::PrefixDecrement<
            ColumnSubtractAssign<internal_const_iterator_type>,
            internal_const_iterator_type>;
        using Difference = ColumnDifference<internal_const_iterator_type>;
      };

     public:
      using iterator       = IteratorStateful<IteratorTraits>;
      using const_iterator = ConstIteratorStateful<ConstIteratorTraits>;

      using column_iterator = IteratorStateful<ColumnIteratorTraits>;
      using const_column_iterator
          = ConstIteratorStateful<ConstColumnIteratorTraits>;

      iterator begin() noexcept {
        return iterator(this, _vec.begin());
      }

      iterator end() noexcept {
        return iterator(this, _vec.end());
      }

      const_iterator cbegin() const noexcept {
        return const_iterator(this, _vec.cbegin());
      }

      const_iterator cend() const noexcept {
        return const_iterator(this, _vec.cend());
      }

      typedef std::reverse_iterator<iterator>       reverse_iterator;
      typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

      // Not noexcept
      reverse_iterator rbegin() {
        return reverse_iterator(end());
      }

      // Not noexcept
      reverse_iterator rend() {
        return reverse_iterator(begin());
      }

      // Not noexcept
      const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
      }

      // Not noexcept
      const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
      }

      // Not noexcept
      const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
      }

      // Not noexcept
      const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
      }

      iterator begin_row(size_t row_index) noexcept {
        LIBSEMIGROUPS_ASSERT(row_index < _nr_rows);
        return iterator(
            this, _vec.begin() + (_nr_used_cols + _nr_unused_cols) * row_index);
      }

      iterator end_row(size_t row_index) noexcept {
        LIBSEMIGROUPS_ASSERT(row_index < _nr_rows);
        return begin_row(row_index) + _nr_used_cols;
      }

      column_iterator begin_column(size_t col_index) noexcept {
        LIBSEMIGROUPS_ASSERT(col_index < _nr_used_cols);
        return column_iterator(this, _vec.begin() + col_index);
      }

      column_iterator end_column(size_t col_index) noexcept {
        LIBSEMIGROUPS_ASSERT(col_index < _nr_used_cols);
        return column_iterator(this, _vec.end() + col_index);
      }

      const_iterator cbegin_row(size_t row_index) const noexcept {
        LIBSEMIGROUPS_ASSERT(row_index < _nr_rows);
        return const_iterator(
            this,
            _vec.cbegin() + (_nr_used_cols + _nr_unused_cols) * row_index);
      }

      const_iterator cend_row(size_t row_index) const noexcept {
        LIBSEMIGROUPS_ASSERT(row_index < _nr_rows);
        return cbegin_row(row_index) + _nr_used_cols;
      }

      const_column_iterator cbegin_column(size_t col_index) const noexcept {
        LIBSEMIGROUPS_ASSERT(col_index < _nr_used_cols);
        return const_column_iterator(this, _vec.begin() + col_index);
      }

      const_column_iterator cend_column(size_t col_index) const noexcept {
        LIBSEMIGROUPS_ASSERT(col_index < _nr_used_cols);
        return const_column_iterator(this, _vec.end() + col_index);
      }
    };

    // Currently only supports N x N 2-dimensional static vectors
    template <typename T, size_t N>
    class StaticVector2 final {
      // So that StaticVector2<T, N> can access private data members of
      // StaticVector2<S, M> and vice versa.
      template <typename S, size_t M>
      friend class StaticVector2;

     public:
      StaticVector2() : _arrays(), _sizes() {
        clear();
      }

      StaticVector2(StaticVector2 const&) = default;
      StaticVector2(StaticVector2&&)      = default;
      StaticVector2& operator=(StaticVector2 const&) = default;
      StaticVector2& operator=(StaticVector2&&) = default;
      ~StaticVector2()                          = default;

      // Not noexcept since std::array::fill is not.
      void clear() {
        _sizes.fill(0);
      }

      // Not noexcept because std::array::operator[] isn't
      void push_back(size_t depth, T x) {
        LIBSEMIGROUPS_ASSERT(depth < N);
        LIBSEMIGROUPS_ASSERT(_sizes[depth] < N);
        _arrays[depth][_sizes[depth]] = x;
        _sizes[depth]++;
      }

      // Not noexcept because std::array::operator[] isn't
      inline T back(size_t depth) const {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth][_sizes[depth] - 1];
      }

      // Not noexcept because std::array::operator[] isn't
      inline T const& at(size_t depth, size_t index) const {
        LIBSEMIGROUPS_ASSERT(depth < N);
        LIBSEMIGROUPS_ASSERT(index < _sizes[depth]);
        return _arrays[depth][index];
      }

      // Not noexcept because std::array::operator[] isn't
      inline size_t size(size_t depth) const {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _sizes[depth];
      }

      // Not noexcept because std::array::operator[] isn't
      inline typename std::array<T, N>::const_iterator
      cbegin(size_t depth) const {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth].cbegin();
      }

      // Not noexcept because std::array::operator[] isn't
      inline typename std::array<T, N>::const_iterator
      cend(size_t depth) const {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth].cbegin() + _sizes[depth];
      }

      // Not noexcept because std::array::operator[] isn't
      inline typename std::array<T, N>::iterator begin(size_t depth) {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth].begin();
      }

      // Not noexcept because std::array::operator[] isn't
      inline typename std::array<T, N>::iterator end(size_t depth) {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth].begin() + _sizes[depth];
      }

     private:
      std::array<std::array<T, N>, N> _arrays;
      std::array<size_t, N>           _sizes;
    };

    // Currently only supports N x N 2-dimensional arrays
    template <typename T, size_t N>
    class Array2 final {
      // So that Array2<T, N> can access private data members of
      // Array2<S, M> and vice versa.
      template <typename S, size_t M>
      friend class Array2;

     public:
      Array2()              = default;
      Array2(Array2 const&) = default;
      Array2(Array2&&)      = default;
      Array2& operator=(Array2 const&) = default;
      Array2& operator=(Array2&&) = default;
      ~Array2()                   = default;

      // Not noexcept cos std::array::fill isn't
      inline void fill(T const& value) {
        for (auto& x : _arrays) {
          x.fill(value);
        }
      }

      // Not noexcept because std::array::operator[] isn't
      inline std::array<T, N>& operator[](size_t depth) {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth];
      }

      // Not noexcept because std::array::at can throw
      inline T const& at(size_t depth, size_t index) const {
        return _arrays.at(depth).at(index);
      }

      // Not noexcept because std::array::operator[] isn't
      inline typename std::array<T, N>::const_iterator
      cbegin(size_t depth) const {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth].cbegin();
      }

      // Not noexcept because std::array::operator[] isn't
      inline typename std::array<T, N>::const_iterator
      cend(size_t depth) const {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth].cend();
      }

      // Not noexcept because std::array::operator[] isn't
      inline typename std::array<T, N>::iterator begin(size_t depth) {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth].begin();
      }

      // Not noexcept because std::array::operator[] isn't
      inline typename std::array<T, N>::iterator end(size_t depth) {
        LIBSEMIGROUPS_ASSERT(depth < N);
        return _arrays[depth].end();
      }

     private:
      std::array<std::array<T, N>, N> _arrays;
    };
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_CONTAINERS_HPP_
