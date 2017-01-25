//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_UTIL_RECVEC_H_
#define LIBSEMIGROUPS_UTIL_RECVEC_H_

#include <assert.h>

#include <algorithm>
#include <vector>

namespace libsemigroups {

  //
  // Template class for *rectangular vectors* i.e. two dimensional vectors.
  // The unique template parameter **T** is the type of the objects stored in
  // the
  // <RecVec>.

  template <typename T> class RecVec {
   public:
    // Default constructor
    // @nr_cols the number of columns in the <RecVec> being constructed
    // @nr_rows the number of rows (defaults to 0) in the <RecVec> being
    // constructed
    // @default_val the default value (defaults to 0)
    //
    // Constructs a <RecVec> with the specified number of columns and rows and
    // initialises every position with the default value.

    explicit RecVec(size_t nr_cols = 0, size_t nr_rows = 0, T default_val = 0)
        : _vec(),
          _nr_used_cols(nr_cols),
          _nr_unused_cols(0),
          _nr_rows(0),
          _default_val(default_val) {
      this->add_rows(nr_rows);
    }

    // Copy and add columns
    // @copy the <RecVec> which will be copied
    // @nr_cols_to_add the number of columns to be added
    //
    // Constructs a copy of the given <RecVec> with the same number of rows as
    // the original and with some additional columns.

    RecVec(const RecVec& copy, size_t nr_cols_to_add = 0)
        : _vec(),
          _nr_used_cols(copy._nr_used_cols),
          _nr_unused_cols(copy._nr_unused_cols),
          _nr_rows(copy.nr_rows()),
          _default_val(copy._default_val) {
      if (nr_cols_to_add <= _nr_unused_cols) {
        _vec = copy._vec;
        _nr_used_cols += nr_cols_to_add;
        _nr_unused_cols -= nr_cols_to_add;
        return;
      }

      size_t new_nr_cols =
          std::max(5 * nr_cols() / 4 + 4, nr_cols_to_add + nr_cols());
      _nr_used_cols += nr_cols_to_add;
      _nr_unused_cols = new_nr_cols - _nr_used_cols;

      _vec.reserve(new_nr_cols * _nr_rows);

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

    // Default destructor

    ~RecVec() {}

    // Add rows
    // @nr the number of rows to add (defaults to 1)
    //
    // Adds the specified number of rows to the <RecVec>. Every new position is
    // initialised with the default value specified when the <RecVec> was
    // constructed.

    void inline add_rows(size_t nr = 1) {
      _nr_rows += nr;
      if (nr != 0) {
        _vec.resize(_vec.size() + (_nr_used_cols + _nr_unused_cols) * nr,
                    _default_val);
      }
    }

    // Add columns
    // @nr the number of columns to add (no default value)
    //
    // Adds the specified number of columns to the <RecVec>. Every new position
    // is initialised with the default value specified when the <RecVec> was
    // constructed.
    //
    // The underlying vector reserves more space than is required to add the
    // specified number of columns so that repeated calls to this method require
    // fewer reallocations.

    void add_cols(size_t nr) {
      if (nr <= _nr_unused_cols) {
        _nr_used_cols += nr;
        _nr_unused_cols -= nr;
        return;
      }

      size_t old_nr_cols = _nr_used_cols + _nr_unused_cols;
      size_t new_nr_cols = std::max(5 * old_nr_cols / 4 + 4, nr + old_nr_cols);
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

    // Set a value
    // @i the row
    // @j the column
    // @val the value
    //
    // Sets the position specified by the row and column to the given value.
    //
    // Asserts whether the specified row and column are within the bounds of the
    // <RecVec>.

    void inline set(size_t i, size_t j, T val) {
      assert(i < _nr_rows && j < _nr_used_cols);
      _vec[i * (_nr_used_cols + _nr_unused_cols) + j] = val;
    }

    // Get a value (const)
    // @i the row
    // @j the column
    //
    // This method is const.
    // @return the value in the specified row and column.
    //
    // Asserts whether the specified row and column are within the bounds of the
    // <RecVec>.

    T inline get(size_t i, size_t j) const {
      assert(i < _nr_rows && j < _nr_used_cols);
      return _vec[i * (_nr_used_cols + _nr_unused_cols) + j];
    }

    // Size (const)
    //
    // This method is const.
    // @return The total number of values stored in the <RecVec> which is the
    // <nr_rows> multiplied by <nr_cols>.
    size_t size() const {
      return _nr_rows * _nr_used_cols;
    }

    size_t empty() const {
      return _nr_rows == 0;
    }

    // Number of rows (const)
    //
    // This method is const.
    // @return the number of rows (first dimension).

    size_t nr_rows() const {
      return _nr_rows;
    }

    // Number of columns (const)
    //
    // This method is const.
    // @return the number of columns (second dimension).

    size_t nr_cols() const {
      return _nr_used_cols;
    }

    // Append
    // @copy a <RecVec> of the same type as this with equal number of columns
    //
    // Appends **copy** to the end of **this** given that the number of columns
    // <nr_cols> of **this** and **copy** are equal.
    //
    // Asserts that the numbers of columns are equal.

    void append(const RecVec<T>& copy) {
      assert(copy._nr_used_cols == _nr_used_cols);

      size_t old_nr_rows = _nr_rows;
      add_rows(copy._nr_rows);

      if (copy._nr_unused_cols == _nr_unused_cols) {
        std::copy(copy._vec.begin(),
                  copy._vec.end(),
                  _vec.begin()
                      + (_nr_used_cols + _nr_unused_cols) * old_nr_rows);
      } else {  // TODO(JDM) improve this
        for (size_t i = old_nr_rows; i < _nr_rows; i++) {
          for (size_t j = 0; j < _nr_used_cols; j++) {
            set(i, j, copy.get(i - old_nr_rows, j));
          }
        }
      }
    }

    // Count
    //
    // @return the number of occurrences of **val** in the **i**th row.

    size_t count(size_t i, T val) const {
      assert(i < _nr_rows);
      return std::count(row_cbegin(i), row_cend(i), val);
    }

    // Count
    //
    // @return the number of occurrences of **val** in the **i**th row.

    template <class UnaryPredicate>
    size_t all_of(size_t i, UnaryPredicate pred) {
      assert(i < _nr_rows);
      return std::all_of(row_cbegin(i), row_cend(i), pred);
    }

    inline typename std::vector<T>::iterator row_begin(size_t i) {
      return _vec.begin() + (_nr_used_cols + _nr_unused_cols) * i;
    }

    inline typename std::vector<T>::iterator row_end(size_t i) {
      return row_begin(i) + _nr_used_cols;
    }

    inline typename std::vector<T>::const_iterator row_cbegin(size_t i) const {
      return _vec.cbegin() + (_nr_used_cols + _nr_unused_cols) * i;
    }

    inline typename std::vector<T>::const_iterator row_cend(size_t i) const {
      return row_cbegin(i) + _nr_used_cols;
    }

    // Iterator
    //
    // @return an iterator pointing at the beginning of the <RecVec>.
    inline typename std::vector<T>::iterator begin() {
      return _vec.begin();
    }

    // Iterator
    //
    // @return an iterator pointing at the end of the <RecVec>.
    inline typename std::vector<T>::iterator end() {
      return _vec.end();
    }

    // clear
    //
    // @return an iterator pointing at the end of the <RecVec>.
    void clear() {
      _nr_unused_cols += _nr_used_cols;
      _nr_used_cols = 0;
      _nr_rows      = 0;
      _vec.clear();
    }

   private:
    std::vector<T> _vec;
    size_t         _nr_used_cols;
    size_t         _nr_unused_cols;
    size_t         _nr_rows;
    T              _default_val;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_UTIL_RECVEC_H_
