//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_INCLUDE_INTERNAL_ITERATOR_HPP_
#define LIBSEMIGROUPS_INCLUDE_INTERNAL_ITERATOR_HPP_

#include <vector>

namespace libsemigroups {
  namespace internal {
    // The class iterator_base provides a base class for random access
    // iterators that wrap an iterator to a vector of TWrappedItemType. The
    // template parameter TMethodsType must provide two methods:
    // 1. indirection - returning a FroidurePin::const_reference to a
    //    FroidurePin::element_type for use by the operator* method of
    //    iterator_base;
    // 2. addressof - returning a pointer to const FroidurePin::element_type for
    //    use by the operator& method of iterator_base.
    template <typename TWrappedItemType,
              typename TConstPointer,
              typename TConstReference,
              typename TValueType,
              class TMethodsType>
    class iterator_base {
     public:
      using size_type = typename std::vector<TWrappedItemType>::size_type;
      using difference_type =
          typename std::vector<TWrappedItemType>::difference_type;
      using const_pointer     = TConstPointer;
      using pointer           = TConstPointer;
      using const_reference   = TConstReference;
      using reference         = TConstReference;
      using value_type        = TValueType;
      using iterator_category = std::random_access_iterator_tag;

      explicit iterator_base(
          typename std::vector<TWrappedItemType>::const_iterator it_vec)
          : _it_vec(it_vec) {}

      iterator_base(iterator_base const& that) : iterator_base(that._it_vec) {}

      iterator_base& operator=(iterator_base const& that) {
        _it_vec = that._it_vec;
        return *this;
      }

      virtual ~iterator_base() {}

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

      iterator_base& operator++() {  // prefix
        ++_it_vec;
        return *this;
      }

      iterator_base& operator--() {
        --_it_vec;
        return *this;
      }

      iterator_base& operator+=(size_type val) {
        _it_vec += val;
        return *this;
      }

      iterator_base& operator-=(size_type val) {
        _it_vec -= val;
        return *this;
      }

      difference_type operator-(iterator_base that) const {
        return _it_vec - that._it_vec;
      }

      const_reference operator[](size_type pos) const {
        return *(*this + pos);
      }

      const_reference operator*() const {
        return TMethodsType().indirection(_it_vec);
      }

      const_pointer operator->() const {
        return TMethodsType().addressof(_it_vec);
      }

     protected:
      typename std::vector<TWrappedItemType>::const_iterator _it_vec;
    };  // iterator_base definition ends
  }     // namespace internal
}  // namespace libsemigroups
#endif // LIBSEMIGROUPS_INCLUDE_INTERNAL_ITERATOR_HPP_
