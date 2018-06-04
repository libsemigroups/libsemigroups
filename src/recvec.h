//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_SRC_RECVEC_H_
#define LIBSEMIGROUPS_SRC_RECVEC_H_

#include <algorithm>
#include <vector>

#include "libsemigroups-debug.h"

namespace libsemigroups {

  //! Template class for *rectangular vectors* i.e. two dimensional vectors.
  //! The unique template parameter **T** is the type of the objects stored in
  //! the <RecVec>.

  template <typename T, class A = std::allocator<T>> class RecVec {
    // So that RecVec<T> can access private data members of RecVec<S> and vice
    // versa.
    template <typename S, class B> friend class RecVec;

   public:
    typedef A                                           allocator_type;
    typedef typename std::vector<T, A>::value_type      value_type;
    typedef typename std::vector<T, A>::reference       reference;
    typedef typename std::vector<T, A>::const_reference const_reference;
    typedef typename std::vector<T, A>::difference_type difference_type;
    typedef typename std::vector<T, A>::size_type       size_type;

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

    // Constructs a copy of the given <RecVec> with the same number of rows as
    // the original and with some additional columns.
    template <typename S, class B>
    RecVec(RecVec<S, B> const& copy, size_t nr_cols_to_add = 0)
        : _vec(),
          _nr_used_cols(copy._nr_used_cols),
          _nr_unused_cols(copy._nr_unused_cols),
          _nr_rows(copy.nr_rows()),
          _default_val(copy._default_val) {
      if (nr_cols_to_add <= _nr_unused_cols) {
        _vec.reserve(copy._vec.size());
        for (auto const& x : copy._vec) {
          _vec.push_back(static_cast<T>(x));
        }
        _nr_used_cols += nr_cols_to_add;
        _nr_unused_cols -= nr_cols_to_add;
        return;
      }

      size_t new_nr_cols
          = std::max(5 * nr_cols() / 4 + 4, nr_cols_to_add + nr_cols());
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

    RecVec<T, A>& operator=(RecVec<T, A> that) {
      swap(that);
      return *this;
    }

    ~RecVec() {}

    // Required methods to make RecVec STL container like
    bool operator==(RecVec<T, A> const& that) const {
      if (_nr_used_cols != that._nr_used_cols || _nr_rows != that._nr_rows) {
        return false;
      }
      return std::equal(cbegin(), cend(), that.cbegin());
    }

    bool operator!=(RecVec<T, A> const& that) const {
      return !operator==(that);
    }

    bool empty() const {
      return _nr_rows == 0;
    }

    size_t size() const {
      return _nr_rows * _nr_used_cols;
    }

    size_t max_size() const {
      return _vec.max_size();
    }

    void swap(RecVec<T, A>& that) {
      _vec.swap(that._vec);
      std::swap(_nr_used_cols, that._nr_used_cols);
      std::swap(_nr_unused_cols, that._nr_unused_cols);
      std::swap(_nr_rows, that._nr_rows);
    }

    void clear() {
      _nr_unused_cols += _nr_used_cols;
      _nr_used_cols = 0;
      _nr_rows      = 0;
      _vec.clear();
    }

    // Adds the specified number of rows to the <RecVec>. Every new position is
    // initialised with the default value specified when the <RecVec> was
    // constructed.
    void inline add_rows(size_t nr) {
      _nr_rows += nr;
      if (nr != 0) {
        _vec.resize(_vec.size() + (_nr_used_cols + _nr_unused_cols) * nr,
                    _default_val);
      }
    }

    void inline set_default_value(T val) {
      _default_val = val;
    }

    // Adds the specified number of columns to the <RecVec>. Every new position
    // is initialised with the default value specified when the <RecVec> was
    // constructed.
    //
    // The underlying vector reserves more space than is required to add the
    // specified number of columns so that repeated calls to this method
    // require fewer reallocations.
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

    // Sets the position specified by the row and column to the given value.
    //
    // Asserts whether the specified row and column are within the bounds of the
    // <RecVec>.
    void inline set(size_t i, size_t j, T val) {
      LIBSEMIGROUPS_ASSERT(i < _nr_rows && j < _nr_used_cols);
      _vec[i * (_nr_used_cols + _nr_unused_cols) + j] = val;
    }

    // Returns the value in the specified row and column.
    //
    // Asserts whether the specified row and column are within the bounds of the
    // <RecVec>.
    T inline get(size_t i, size_t j) const {
      LIBSEMIGROUPS_ASSERT(i < _nr_rows && j < _nr_used_cols);
      return _vec[i * (_nr_used_cols + _nr_unused_cols) + j];
    }

    // Returns the number of rows (first dimension).
    size_t nr_rows() const {
      return _nr_rows;
    }

    // Returns the number of columns (second dimension).
    size_t nr_cols() const {
      return _nr_used_cols;
    }

    // The parameter \p copy must be a compatible RecVec with equal number of
    // columns.
    //
    // Appends \p copy to the end of \c this given that the number of columns
    // of \c this and \p copy are equal.
    //
    // Asserts that the numbers of columns are equal.
    template <typename S, class B> void append(const RecVec<S, B>& copy) {
      LIBSEMIGROUPS_ASSERT(copy._nr_used_cols == _nr_used_cols);

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

    void reserve(size_t nr_rows) {
      _vec.reserve(nr_rows * (_nr_unused_cols + _nr_used_cols));
    }

   private:
    std::vector<T, A> _vec;
    size_t            _nr_used_cols;
    size_t            _nr_unused_cols;
    size_t            _nr_rows;
    T                 _default_val;

   private:
    template <typename iterator_type,
              typename reference_type,
              typename pointer_type>
    class iterator_base {
      struct iterator_methods;

     public:
      typedef typename std::vector<T, A>::size_type       size_type;
      typedef typename std::vector<T, A>::difference_type difference_type;
      typedef typename std::vector<T, A>::value_type      value_type;
      typedef reference_type                              reference;
      typedef pointer_type                                pointer;
      typedef reference_type                              const_reference;
      typedef pointer_type                                const_pointer;
      typedef std::random_access_iterator_tag             iterator_category;

      iterator_base(iterator_type it_vec, iterator_methods const* methods)
          : _it_vec(it_vec), _methods(methods) {}

      iterator_base(RecVec<T, A> const& rv, iterator_type it_vec)
          : _it_vec(it_vec), _methods(nullptr) {
        if (rv._nr_unused_cols == 0) {
          _methods = _fast_methods;
        } else {
          _methods = new slow_iterator_methods(rv);
        }
      }

      iterator_base(iterator_base const& that)
          : iterator_base(that._it_vec, that._methods->copy()) {}

      iterator_base& operator=(iterator_base const& that) {
        _it_vec  = that._it_vec;
        _methods = that._methods->copy();
        return *this;
      }

      static iterator_base column_iterator(RecVec<T, A> const& rv,
                                           iterator_type       it_vec) {
        return iterator_base(it_vec, new column_iterator_methods(rv));
      }

      ~iterator_base() {
        if (_methods != _fast_methods) {
          delete _methods;
        }
      }

      bool operator==(iterator_base const& that) const {
        return _it_vec == that._it_vec;
      }

      bool operator!=(iterator_base const& that) const {
        return _it_vec != that._it_vec;
      }

      bool operator<(iterator_base const& that) const {
        return _it_vec < that._it_vec;
      }

      bool operator>(iterator_base const& that) const {
        return _it_vec > that._it_vec;
      }

      bool operator<=(iterator_base const& that) const {
        return operator<(that) || operator==(that);
      }

      bool operator>=(iterator_base const& that) const {
        return operator>(that) || operator==(that);
      }

      // postfix
      iterator_base operator++(int) {
        iterator_base  tmp(*this);
        iterator_base::operator++();
        return tmp;
      }

      iterator_base operator--(int) {
        iterator_base  tmp(*this);
        iterator_base::operator--();
        return tmp;
      }

      iterator_base operator+(size_type val) const {
        iterator_base out(*this);
        return out += val;
      }

      friend iterator_base operator+(size_type val, iterator_base const& it) {
        return it + val;
      }

      iterator_base operator-(size_type val) const {
        iterator_base out(*this);
        return out -= val;
      }

      reference_type operator*() const {
        return *_it_vec;
      }

      pointer_type operator->() const {
        return &(*_it_vec);
      }

      reference_type operator[](size_type pos) const {
        return *(*this + pos);
      }

      iterator_base& operator++() {  // prefix
        _methods->prefix_increment(*this);
        return *this;
      }

      iterator_base& operator--() {
        _methods->prefix_decrement(*this);
        return *this;
      }

      iterator_base& operator+=(size_type val) {
        _methods->increment(*this, val);
        return *this;
      }

      iterator_base& operator-=(size_type val) {
        _methods->decrement(*this, val);
        return *this;
      }

      difference_type operator-(iterator_base that) const {
        return _methods->difference(*this, that);
      }

     private:
      iterator_type                  _it_vec;
      iterator_methods const*        _methods;
      static iterator_methods const* _fast_methods;

      struct iterator_methods {
        virtual iterator_methods const* copy() const = 0;
        virtual ~iterator_methods() {}

        virtual inline void
        prefix_increment(iterator_base& it) const {  // prefix
          this->increment(it, 1);
        }

        virtual inline void prefix_decrement(iterator_base& it) const {
          this->decrement(it, 1);
        }

        virtual void increment(iterator_base& it, size_type val) const = 0;
        virtual void decrement(iterator_base& it, size_type val) const = 0;

        virtual difference_type difference(iterator_base const& it,
                                           iterator_base        that) const = 0;
      };

      struct fast_iterator_methods : public iterator_methods {
        iterator_methods const* copy() const final {
          return this;
        }

        inline void prefix_increment(iterator_base& it) const final {  // prefix
          ++it._it_vec;
        }

        inline void prefix_decrement(iterator_base& it) const final {
          --it._it_vec;
        }

        inline void increment(iterator_base& it, size_type val) const final {
          it._it_vec += val;
        }

        inline void decrement(iterator_base& it, size_type val) const final {
          it._it_vec -= val;
        }

        inline difference_type difference(iterator_base const& it,
                                          iterator_base that) const final {
          return it._it_vec - that._it_vec;
        }
      };

      struct slow_iterator_methods : public iterator_methods {
        explicit slow_iterator_methods(RecVec<T, A> const& rv) : _rv(rv) {}

        iterator_methods const* copy() const override {
          return new slow_iterator_methods(_rv);
        }

        inline void increment(iterator_base& it, size_type val) const override {
          size_type s = _rv._nr_used_cols;
          size_type n = _rv._nr_unused_cols;

          size_type q = val / s;
          size_type r = val % s;

          difference_type p = it._it_vec - _rv._vec.begin();

          it._it_vec += q * (s + n) + r;
          if (r + remainder(p, s + n) >= s) {
            it._it_vec += n;
          }
        }

        inline void decrement(iterator_base& it, size_type val) const override {
          size_type s = _rv._nr_used_cols;
          size_type n = _rv._nr_unused_cols;

          size_type q = val / s;
          size_type r = val % s;

          difference_type p = it._it_vec - _rv._vec.begin();

          it._it_vec -= (q * (s + n) + r);
          if (remainder(p, s + n) < r) {
            it._it_vec -= n;
          }
        }

        inline difference_type difference(iterator_base const& it,
                                          iterator_base that) const override {
          difference_type s = _rv._nr_used_cols;
          difference_type n = _rv._nr_unused_cols;
          difference_type b = it._it_vec - _rv._vec.begin();
          difference_type a = that._it_vec - _rv._vec.begin();
          return (b - a) - n * (quotient(b, s + n) - quotient(a, s + n));
        }

        static inline size_type remainder(difference_type a,
                                          difference_type b) {
          if (a >= 0) {
            return a % b;
          } else {
            return (a % b) + b;
          }
        }

        static inline size_type quotient(difference_type a, difference_type b) {
          if (a % b >= 0) {
            return a / b;
          } else {
            return (a / b) - 1;
          }
        }
        RecVec<T, A> const& _rv;
      };

      struct column_iterator_methods : public slow_iterator_methods {
        using slow_iterator_methods::slow_iterator_methods;

        iterator_methods const* copy() const final {
          return new column_iterator_methods(this->_rv);
        }

        inline void increment(iterator_base& it, size_type val) const final {
          it._it_vec
              += val * (this->_rv._nr_used_cols + this->_rv._nr_unused_cols);
        }

        inline void decrement(iterator_base& it, size_type val) const final {
          it._it_vec
              -= val * (this->_rv._nr_used_cols + this->_rv._nr_unused_cols);
        }

        inline difference_type difference(iterator_base const& it,
                                          iterator_base that) const final {
          return (it._it_vec - that._it_vec)
                 / static_cast<difference_type>(this->_rv._nr_used_cols
                                                + this->_rv._nr_unused_cols);
        }
      };
    };  // iterator_base definition ends

   public:
    typedef iterator_base<typename std::vector<T, A>::iterator,
                          typename std::vector<T, A>::reference,
                          typename std::vector<T, A>::pointer>
        iterator;

    typedef iterator_base<typename std::vector<T, A>::const_iterator,
                          typename std::vector<T, A>::const_reference,
                          typename std::vector<T, A>::const_pointer>
        const_iterator;

    iterator begin() {
      return iterator(*this, _vec.begin());
    }

    iterator end() {
      return iterator(*this, _vec.end());
    }

    const_iterator begin() const {
      return cbegin();
    }

    const_iterator cbegin() const {
      return const_iterator(*this, _vec.begin());
    }

    const_iterator end() const {
      return cend();
    }

    const_iterator cend() const {
      return const_iterator(*this, _vec.end());
    }

    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    reverse_iterator rbegin() {
      return reverse_iterator(end());
    }

    reverse_iterator rend() {
      return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
      return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
      return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
      return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
      return const_reverse_iterator(cbegin());
    }

    iterator begin_row(size_t row_index) {
      LIBSEMIGROUPS_ASSERT(row_index < _nr_rows);
      return iterator(
          *this, _vec.begin() + (_nr_used_cols + _nr_unused_cols) * row_index);
    }

    iterator end_row(size_t row_index) {
      LIBSEMIGROUPS_ASSERT(row_index < _nr_rows);
      return begin_row(row_index) + _nr_used_cols;
    }

    iterator begin_column(size_t col_index) {
      LIBSEMIGROUPS_ASSERT(col_index < _nr_used_cols);
      return iterator::column_iterator(*this, _vec.begin() + col_index);
    }

    iterator end_column(size_t col_index) {
      LIBSEMIGROUPS_ASSERT(col_index < _nr_used_cols);
      return iterator::column_iterator(*this, _vec.end() + col_index);
    }

    const_iterator cbegin_row(size_t row_index) const {
      LIBSEMIGROUPS_ASSERT(row_index < _nr_rows);
      return const_iterator(
          *this, _vec.begin() + (_nr_used_cols + _nr_unused_cols) * row_index);
    }

    const_iterator cend_row(size_t row_index) const {
      LIBSEMIGROUPS_ASSERT(row_index < _nr_rows);
      return cbegin_row(row_index) + _nr_used_cols;
    }

    const_iterator cbegin_column(size_t col_index) {
      LIBSEMIGROUPS_ASSERT(col_index < _nr_used_cols);
      return const_iterator::column_iterator(*this, _vec.begin() + col_index);
    }

    const_iterator cend_column(size_t col_index) {
      LIBSEMIGROUPS_ASSERT(col_index < _nr_used_cols);
      return const_iterator::column_iterator(*this, _vec.end() + col_index);
    }
    // TODO reverse row and column iterators
  };

  template <typename T, class A>
  template <typename iterator_type,
            typename reference_type,
            typename pointer_type>
  typename RecVec<T, A>::template iterator_base<
      iterator_type,
      reference_type,
      pointer_type>::iterator_methods const* RecVec<T, A>::
      iterator_base<iterator_type, reference_type, pointer_type>::_fast_methods
      = new typename RecVec<T, A>::template iterator_base<
          iterator_type,
          reference_type,
          pointer_type>::fast_iterator_methods();
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_RECVEC_H_
