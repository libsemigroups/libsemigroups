//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// This file defines a class UF used to make an equivalence relation on the
// integers {0, .., n - 1}, using the UNION-FIND METHOD: new pairs can be added
// and the appropriate classes combined quickly.

// The implementation here is based on the implementation in the datastructures
// package for GAP (v0.2.5) by Markus Pfeiffer, Max Horn, Christopher
// Jefferson, Steve Linton.

// An earlier version of the UF class was written by M. Torpey.

#ifndef LIBSEMIGROUPS_UF_HPP_
#define LIBSEMIGROUPS_UF_HPP_

#include <algorithm>      // for for_each
#include <array>          // for array
#include <cstddef>        // for size_t
#include <numeric>        // for iota
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "config.hpp"    // for LIBSEMIGROUPS_SIZEOF_VOID_P
#include "debug.hpp"     // for LIBSEMIGROUPS_ASSERT
#include "iterator.hpp"  // for ConstIteratorStateless
#include "types.hpp"     // for SmallestInteger

namespace libsemigroups {
  namespace detail {

#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    constexpr size_t RANK_BITS = 6;
#else
    constexpr size_t RANK_BITS = 5;
#endif

    template <typename T>
    class UF {
      ////////////////////////////////////////////////////////////////////////
      // Aliases - private
      ////////////////////////////////////////////////////////////////////////

      using node_type = typename T::value_type;
      using rank_type = typename T::value_type;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Aliases - public
      ////////////////////////////////////////////////////////////////////////
      using size_type      = size_t;
      using container_type = T;
      using index_type     = typename T::value_type;

      ////////////////////////////////////////////////////////////////////////
      // Constructors + destructor
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because the constructors of std::vector and std::array
      // aren't
      explicit UF(container_type&& c) : _data(std::move(c)) {
        init_data(_data.begin(), _data.end());
      }

      // not noexcept because the constructors of std::vector and std::array
      // aren't
      explicit UF(container_type const& c) : _data(c) {
        init_data(_data.begin(), _data.end());
      }

      // not noexcept because the constructors of std::vector and std::array
      // aren't
      explicit UF(size_type size = 0) : _data() {
        set_data_capacity(_data, size);
        std::iota(_data.begin(), _data.end(), 0);
        init_data(_data.begin(), _data.end());
      }

      // not noexcept because the constructors of std::vector and std::array
      // aren't
      UF(UF const&) = default;
      UF& operator=(UF const&) = default;
      UF(UF&&)                 = default;
      UF& operator=(UF&&) = default;
      ~UF()               = default;

      ////////////////////////////////////////////////////////////////////////
      // Union-Find
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because std::array::operator[] and
      // std::vector::operator[] aren't
      index_type find(index_type x) const {
        LIBSEMIGROUPS_ASSERT(x < _data.size());
        while (true) {
          index_type y = index(_data[x]);
          LIBSEMIGROUPS_ASSERT(0 <= y && y < _data.size());
          if (y == x) {
            return y;
          }
          index_type z = index(_data[y]);
          LIBSEMIGROUPS_ASSERT(0 <= z && z < _data.size());
          if (y == z) {
            return y;
          }
          _data[x] = node(z) | rank(_data[x]);
          x        = z;
        }
      }

      // not noexcept because UF::find isn't
      void unite(index_type x, index_type y) {
        LIBSEMIGROUPS_ASSERT(x < _data.size());
        LIBSEMIGROUPS_ASSERT(y < _data.size());
        x = find(x);
        y = find(y);
        if (x == y) {
          return;
        }
        rank_type rx = rank(_data[x]);
        rank_type ry = rank(_data[y]);
        if (rx > ry) {
          _data[y] = node(x) | ry;
        } else {
          _data[x] = node(y) | rx;
          if (rx == ry) {
            _data[y] = node(y) | (ry + 1);
          }
        }
      }

      ////////////////////////////////////////////////////////////////////////
      // operators
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because UF::contains isn't
      bool operator==(UF const& that) const {
        return contains(that) && that.contains(*this);
      }

      // not noexcept because UF::operator== isn't
      bool operator!=(UF const& that) const {
        return !(*this == that);
      }

      // not noexcept because find isn't
      // if this contains that
      bool contains(UF const& that) const {
        LIBSEMIGROUPS_ASSERT(size() == that.size());
        // reps of that to reps of this
        std::unordered_map<index_type, index_type> lookup;

        for (index_type i = 0; i < size(); ++i) {
          index_type j  = that.find(i);
          auto       it = lookup.find(j);
          if (it == lookup.end()) {
            lookup.emplace(j, find(i));
          } else if (it->second != find(i)) {
            return false;
          }
        }
        return true;
      }

      ////////////////////////////////////////////////////////////////////////
      // Container like
      ////////////////////////////////////////////////////////////////////////

      // noexcept because std::array::size and std::vector::size are noexcept
      size_type size() const noexcept {
        return _data.size();
      }

      // noexcept because std::array::empty and std::vector::empty are noexcept
      size_type empty() const noexcept {
        return _data.empty();
      }

      // not noexcept because std::array::operator[] and
      // std::vector::operator[] aren't
      size_type number_of_blocks() const {
        size_type count = 0;
        for (index_type i = 0; i < _data.size(); ++i) {
          if (index(_data[i]) == i) {
            count++;
          }
        }
        return count;
      }

      ////////////////////////////////////////////////////////////////////////
      // Modifiers
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because std::array::operator[] and
      // std::vector::operator[] aren't
      void compress() {
        for (index_type i = 0; i < _data.size(); ++i) {
          index_type j  = find(index(_data[i]));
          rank_type  rk = (j == i ? 0 : 1);
          _data[i]      = node(j) | rk;
        }
      }

      // not noexcept because compress isn't
      void normalize() {
        compress();
        // old -> new class indexing
        std::unordered_map<index_type, index_type> lookup;
        container_type                             data;
        set_data_capacity(data, size());
        for (index_type i = 0; i < size(); ++i) {
          index_type j  = find(i);
          auto       it = lookup.find(j);
          if (it == lookup.end()) {
            lookup.emplace(j, i);
            data[i] = node(i);  // rank 0
          } else {
            data[i] = node(it->second) | 1;
          }
        }
        std::swap(_data, data);
      }

      // not noexcept because unite isn't
      void join(UF const& that) {
        LIBSEMIGROUPS_ASSERT(size() == that.size());
        for (index_type i = 0; i < _data.size(); ++i) {
          unite(index(_data[i]), index(that._data[i]));
        }
      }

      void swap(UF& that) noexcept {
        std::swap(_data, that._data);
      }

      ////////////////////////////////////////////////////////////////////////
      // Iterators
      ////////////////////////////////////////////////////////////////////////

      struct IteratorTraits : ConstIteratorTraits<container_type> {
        using internal_iterator_type = typename ConstIteratorTraits<
            container_type>::internal_iterator_type;
        using state_type = std::pair<internal_iterator_type,   // _data.cbegin()
                                     internal_iterator_type>;  // _data.cend()
        using value_type = index_type;
        using reference  = value_type;
        using const_reference   = value_type const;
        using const_pointer     = value_type const*;
        using pointer           = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        struct Deref {
          value_type operator()(state_type, internal_iterator_type const& it)
              const noexcept {
            return index(*it);
          }
        };

        struct PrefixIncrement {
          void operator()(state_type const&       st,
                          internal_iterator_type& it) const noexcept {
            index_type i = std::distance(st.first, it);
            do {
              ++it;
              ++i;
            } while (it != st.second && index(*it) != i);
          }
        };

        struct PrefixDecrement {
          void operator()(state_type const&       st,
                          internal_iterator_type& it) const noexcept {
            index_type i = std::distance(st.first, it);
            do {
              --it;
              --i;
            } while (it != st.first && index(*it) != i);
          }
        };
      };

      using const_iterator = detail::ConstIteratorStateful<IteratorTraits>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      // not noexcept because std::array::operator[] and
      // std::vector::operator[] aren't
      const_iterator cbegin() const {
        auto it = const_iterator(std::make_pair(_data.cbegin(), _data.cend()),
                                 _data.cbegin());
        return (index(_data[0]) == 0 ? it : ++it);
      }

      // not noexcept because std::array::operator[] and
      // std::vector::operator[] aren't
      const_iterator cend() const {
        return const_iterator(std::make_pair(_data.cbegin(), _data.cend()),
                              _data.cend());
      }

      // not noexcept because cend() isn't
      const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
      }

      // not noexcept because cbegin() isn't
      const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
      }

      struct DataIteratorTraits : ConstIteratorTraits<container_type> {
        using internal_iterator_type = typename ConstIteratorTraits<
            container_type>::internal_iterator_type;
        using value_type      = index_type;
        using reference       = value_type;
        using const_reference = value_type const;
        using const_pointer   = value_type const*;
        using pointer         = value_type*;

        struct Deref {
          value_type
          operator()(internal_iterator_type const& it) const noexcept {
            return index(*it);
          }
        };
      };

      using const_data_iterator
          = detail::ConstIteratorStateless<DataIteratorTraits>;

      // noexcept because _data.cbegin is noexcept
      const_data_iterator cbegin_data() const noexcept {
        return const_data_iterator(_data.cbegin());
      }

      // noexcept because _data.cend is noexcept
      const_data_iterator cend_data() const noexcept {
        return const_data_iterator(_data.cend());
      }

      struct RankIteratorTraits : ConstIteratorTraits<container_type> {
        using internal_iterator_type = typename ConstIteratorTraits<
            container_type>::internal_iterator_type;
        using value_type      = rank_type;
        using reference       = value_type;
        using const_reference = value_type const;
        using const_pointer   = value_type const*;
        using pointer         = value_type*;

        struct Deref {
          value_type
          operator()(internal_iterator_type const& it) const noexcept {
            return rank(*it);
          }
        };
      };

      using const_rank_iterator
          = detail::ConstIteratorStateless<RankIteratorTraits>;

      // noexcept because _data.cbegin is noexcept
      const_rank_iterator cbegin_rank() const noexcept {
        return const_rank_iterator(_data.cbegin());
      }

      // noexcept because _data.cbegin is noexcept
      const_rank_iterator cend_rank() const noexcept {
        return const_rank_iterator(_data.cend());
      }

     private:
      ////////////////////////////////////////////////////////////////////////
      // Private
      ////////////////////////////////////////////////////////////////////////

      static constexpr index_type rank_mask
          = ((index_type(1) << RANK_BITS) - index_type(1));

      static inline rank_type rank(node_type i) noexcept {
        return i & rank_mask;
      }

      static inline node_type node(index_type i) noexcept {
        return i << RANK_BITS;
      }

      static inline index_type index(node_type i) noexcept {
        return i >> RANK_BITS;
      }

      template <typename SFINAE = void>
      static auto set_data_capacity(container_type& data, size_t N)
          -> std::enable_if_t<
              std::is_base_of<std::vector<node_type>, container_type>::value,
              SFINAE> {
        data.resize(N, 0);
      }

      template <typename SFINAE = void>
      static auto set_data_capacity(container_type&, size_t const)
          -> std::enable_if_t<
              !std::is_base_of<std::vector<node_type>, container_type>::value,
              SFINAE> {}

     protected:
      ////////////////////////////////////////////////////////////////////////
      // Protected
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because std::for_each can throw
      static void init_data(typename container_type::iterator first,
                            typename container_type::iterator last) {
        std::for_each(first, last, [](index_type& i) { i = node(i); });
      }

      mutable container_type _data;
    };

    template <typename T>
    inline void swap(UF<T>& x, UF<T>& y) noexcept {
      x.swap(y);
    }

    // Dynamic Union-Find
    template <typename T = size_t>
    class Duf : public UF<std::vector<T>> {
     public:
      using UF<std::vector<T>>::UF;
      using size_type = typename UF<std::vector<T>>::size_type;
      // not noexcept because std::vector::resize can throw
      void resize(size_type N) {
        size_type const M = this->size();
        if (N <= M) {
          return;
        }
        this->_data.resize(N);
        std::iota(this->_data.begin() + M, this->_data.end(), M);
        this->init_data(this->_data.begin() + M, this->_data.end());
      }
    };

    // Static Union-Find
    template <size_t N,
              typename T =
                  typename SmallestInteger<N*(RANK_BITS == 5 ? 32 : 64)>::type>
    using Suf = UF<std::array<T, N>>;
  }  // namespace detail
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_UF_HPP_
