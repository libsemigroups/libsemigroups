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

#ifndef LIBSEMIGROUPS_INCLUDE_ITERATOR_HPP_
#define LIBSEMIGROUPS_INCLUDE_ITERATOR_HPP_

#include <vector>

namespace libsemigroups {
  namespace internal {
    template <typename TInternalIteratorType,
              typename TExternalValueType,
              typename TExternalConstPointer,
              typename TExternalConstReference>
    class const_iterator_base {
      using internal_value_type = typename TInternalIteratorType::value_type;

     public:
      using size_type         = size_t;
      using difference_type   = typename TInternalIteratorType::difference_type;
      using const_pointer     = TExternalConstPointer;
      using pointer           = TExternalConstPointer;
      using const_reference   = TExternalConstReference;
      using reference         = TExternalConstReference;
      using value_type        = TExternalValueType;
      using iterator_category = std::random_access_iterator_tag;

      explicit const_iterator_base(
          TInternalIteratorType it_vec)
          : _it_vec(it_vec) {}

      const_iterator_base(const_iterator_base const& that)
          : const_iterator_base(that._it_vec) {}

      virtual ~const_iterator_base() {}

      bool operator==(const_iterator_base const& that) const {
        return _it_vec == that._it_vec;
      }

      bool operator!=(const_iterator_base const& that) const {
        return _it_vec != that._it_vec;
      }

      bool operator<(const_iterator_base const& that) const {
        return _it_vec < that._it_vec;
      }

      bool operator>(const_iterator_base const& that) const {
        return _it_vec > that._it_vec;
      }

      bool operator<=(const_iterator_base const& that) const {
        return operator<(that) || operator==(that);
      }

      bool operator>=(const_iterator_base const& that) const {
        return operator>(that) || operator==(that);
      }

      // postfix
      const_iterator_base operator++(int) {
        const_iterator_base  tmp(*this);
        const_iterator_base::operator++();
        return tmp;
      }

      const_iterator_base operator--(int) {
        const_iterator_base  tmp(*this);
        const_iterator_base::operator--();
        return tmp;
      }

      const_iterator_base operator+(size_type val) const {
        const_iterator_base out(*this);
        return out += val;
      }

      friend const_iterator_base operator+(size_type                  val,
                                           const_iterator_base const& it) {
        return it + val;
      }

      const_iterator_base operator-(size_type val) const {
        const_iterator_base out(*this);
        return out -= val;
      }

      const_iterator_base& operator++() {  // prefix
        ++_it_vec;
        return *this;
      }

      const_iterator_base& operator--() {
        --_it_vec;
        return *this;
      }

      const_iterator_base& operator+=(size_type val) {
        _it_vec += val;
        return *this;
      }

      const_iterator_base& operator-=(size_type val) {
        _it_vec -= val;
        return *this;
      }

      difference_type operator-(const_iterator_base that) const {
        return _it_vec - that._it_vec;
      }

      const_reference operator[](size_type pos) const {
        return *(*this + pos);
      }

     protected:
      TInternalIteratorType _it_vec;
    };  // const_iterator_base definition ends

    template <typename TInternalIteratorType,
              typename TExternalConstPointer,
              typename TExternalConstReference,
              typename TExternalValueType,
              typename TStateType,
              typename TMethodsType>
    class const_iterator_stateful
        : public const_iterator_base<TInternalIteratorType,
                                     TExternalConstPointer,
                                     TExternalConstReference,
                                     TExternalValueType> {

      using const_iterator_base = const_iterator_base<TInternalIteratorType,
                                     TExternalConstPointer,
                                     TExternalConstReference,
                                     TExternalValueType>;

      const_iterator_stateful(TStateType* st, TInternalIteratorType it_vec)
          : const_iterator_base(it_vec), _state(st) {}

      using const_reference = typename const_iterator_base::const_reference;
      using const_pointer   = typename const_iterator_base::const_pointer;

      const_reference operator*() const {
        return TMethodsType().indirection(_state, this->_it_vec);
      }

      const_pointer operator->() const {
        return TMethodsType().addressof(_state, this->_it_vec);
      }

     private:
      TStateType* _state;
    };

    template <typename TMethodsType,
              typename TInternalIteratorType,
              typename TExternalValueType,
              typename TExternalConstPointer = TExternalValueType const* const,
              typename TExternalConstReference = TExternalValueType const&>
    class const_iterator_stateless
        : public const_iterator_base<TInternalIteratorType,
                                     TExternalValueType,
                                     TExternalConstPointer,
                                     TExternalConstReference> {
      static_assert(
          std::is_trivially_default_constructible<TMethodsType>::value,
          "the final template parameter is not trivially default "
          "constructible!");

      using const_iterator_base = const_iterator_base<TInternalIteratorType,
                                                      TExternalValueType,
                                                      TExternalConstPointer,
                                                      TExternalConstReference>;

     public:
      using const_iterator_base::const_iterator_base;

      using size_type         = typename const_iterator_base::size_type;
      using difference_type   = typename const_iterator_base::difference_type;
      using const_pointer     = typename const_iterator_base::const_pointer;
      using pointer           = typename const_iterator_base::pointer;
      using const_reference   = typename const_iterator_base::const_reference;
      using reference         = typename const_iterator_base::reference;
      using value_type        = typename const_iterator_base::value_type;
      using iterator_category = std::random_access_iterator_tag;

      const_reference operator*() const {
        return TMethodsType().indirection(this->_it_vec);
      }

      const_pointer operator->() const {
        return TMethodsType().addressof(this->_it_vec);
      }
    };

    // Is just TInternalIteratorType if the external and internal types are the
    // same.
    template <typename TMethodsType,
              typename TInternalIteratorType,
              typename TExternalValueType,
              typename TExternalConstPointer = TExternalValueType const* const,
              typename TExternalConstReference = TExternalValueType const&>
    using const_iterator_stateless_helper = typename std::conditional<
        std::is_same<typename TInternalIteratorType::value_type,
                     TExternalValueType>::value,
        TInternalIteratorType,
        const_iterator_stateless<TMethodsType,
                                 TInternalIteratorType,
                                 TExternalValueType,
                                 TExternalConstPointer,
                                 TExternalConstReference>>::type;

    // The class iterator_base provides a base class for random access
    // iterators that wrap an iterator to a vector of TWrappedItemType. The
    // template parameter TMethodsType must provide two methods:
    // 1. indirection - returning a FroidurePin::const_reference to a
    //    FroidurePin::element_type for use by the operator* method of
    //    iterator_base;
    // 2. addressof - returning a pointer to const FroidurePin::element_type for
    //    use by the operator& method of iterator_base.
    // FIXME remove this (use const_iterator_stateless instead)!
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
#endif  // LIBSEMIGROUPS_INCLUDE_ITERATOR_HPP_
