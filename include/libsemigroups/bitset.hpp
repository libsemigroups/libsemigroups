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

// TODO(later)
// 1) doc

#ifndef LIBSEMIGROUPS_BITSET_HPP_
#define LIBSEMIGROUPS_BITSET_HPP_

#include <array>        // for array
#include <bitset>       // for bitset
#include <climits>      // for CHAR_BIT
#include <cstddef>      // for size_t
#include <iosfwd>       // for operator<<, ostringstream
#include <type_traits>  // for false_type
#include <utility>      // for hash

#include "config.hpp"     // for LIBSEMIGROUPS_SIZEOF_VOID_P
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "string.hpp"     // for detail::to_string

namespace libsemigroups {

  // The code below for popcnt is borrowed/adapted from GAP.

#if LIBSEMIGROUPS_USE_POPCNT && defined(LIBSEMIGROUPS_HAVE___BUILTIN_POPCOUNTL)
  template <typename T>
  static inline size_t COUNT_TRUES_BLOCK(T block) {
    return __builtin_popcountl(block);
  }
#else
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
  template <typename T>
  static inline auto COUNT_TRUES_BLOCK(T block)
      -> std::enable_if_t<sizeof(T) == 8, size_t> {
    block
        = (block & 0x5555555555555555L) + ((block >> 1) & 0x5555555555555555L);
    block
        = (block & 0x3333333333333333L) + ((block >> 2) & 0x3333333333333333L);
    block = (block + (block >> 4)) & 0x0f0f0f0f0f0f0f0fL;
    block = (block + (block >> 8));
    block = (block + (block >> 16));
    block = (block + (block >> 32)) & 0x00000000000000ffL;
    return block;
  }
#endif

  template <typename T>
  static inline auto COUNT_TRUES_BLOCK(T block)
      -> std::enable_if_t<sizeof(T) == 4, size_t> {
    block = (block & 0x55555555) + ((block >> 1) & 0x55555555);
    block = (block & 0x33333333) + ((block >> 2) & 0x33333333);
    block = (block + (block >> 4)) & 0x0f0f0f0f;
    block = (block + (block >> 8));
    block = (block + (block >> 16)) & 0x000000ff;
    return block;
  }

  template <typename T>
  static inline auto COUNT_TRUES_BLOCK(T block)
      -> std::enable_if_t<sizeof(T) == 2, size_t> {
    block = (block & 0x5555) + ((block >> 1) & 0x5555);
    block = (block & 0x3333) + ((block >> 2) & 0x3333);
    block = (block + (block >> 4)) & 0x0f0f;
    block = (block + (block >> 8)) & 0x00ff;
    return block;
  }

  template <typename T>
  static inline auto COUNT_TRUES_BLOCK(T block)
      -> std::enable_if_t<sizeof(T) == 1, size_t> {
    static constexpr std::array<size_t, 256> const lookup = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4,
        2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5,
        3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6,
        4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};
    return lookup[block];
  }
#endif

  template <size_t N>
  class BitSet {
    static_assert(N > 0, "BitSet does not support 0 entries");
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
    static_assert(N <= 64, "BitSet does not support more than 64 entries");
#else
    static_assert(N <= 32, "BitSet does not support more than 32 entries");
#endif

   public:
    using block_type = std::conditional_t<
        N <= 8,
        uint_fast8_t,
        std::conditional_t<
            N <= 16,
            uint_fast16_t,
            std::conditional_t<N <= 32, uint_fast32_t, uint64_t>>>;

    explicit constexpr BitSet(block_type block) noexcept : _block(block) {}
    constexpr BitSet() noexcept : BitSet(0) {}
    constexpr BitSet(BitSet const&) noexcept = default;
    constexpr BitSet(BitSet&&) noexcept      = default;
    BitSet& operator=(BitSet const&) noexcept = default;
    BitSet& operator=(BitSet&&) noexcept = default;

    template <typename T>
    BitSet(T first, T last) : BitSet() {
      LIBSEMIGROUPS_ASSERT(first <= last);
      size_t const K = std::distance(first, last);
      if (K > size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the size of the container is %llu, trying to initialize with %llu "
            "items",
            static_cast<uint64_t>(size()),
            static_cast<uint64_t>(K))
      }
      auto it = first;
      for (size_t i = 0; i < K; ++i, ++it) {
        set(i, *it);
      }
    }

    ~BitSet() = default;

    // Could be static
    constexpr size_t size() const noexcept {
      return N;
    }

    static constexpr size_t max_size() noexcept {
#if LIBSEMIGROUPS_SIZEOF_VOID_P == 8
      return 64;
#else
      return 32;
#endif
    }

    bool operator<(BitSet const& that) const noexcept {
      clear_hi_bits();
      that.clear_hi_bits();
      return _block < that._block;
    }

    bool operator==(BitSet const& that) const noexcept {
      clear_hi_bits();
      that.clear_hi_bits();
      return _block == that._block;
    }

    bool operator!=(BitSet const& that) const noexcept {
      return !(*this == that);
    }

    void operator&=(BitSet const& that) const noexcept {
      _block &= that._block;
    }

    BitSet<N> operator&(BitSet const& that) const noexcept {
      return BitSet(_block & that._block);
    }

    void operator|=(BitSet const& that) const noexcept {
      _block |= that._block;
    }

    bool test(size_t pos) const noexcept {
      LIBSEMIGROUPS_ASSERT(pos < N);
      return _block & mask(pos);
    }

    bool operator[](size_t pos) const noexcept {
      return test(pos);
    }

    BitSet& set() noexcept {
      _block = ~0;
      return *this;
    }

    BitSet& set(size_t pos, bool value = true) noexcept {
      LIBSEMIGROUPS_ASSERT(pos < N);
      if (value) {
        _block |= mask(pos);
      } else {
        _block &= ~mask(pos);
      }
      return *this;
    }

    BitSet& set(size_t first, size_t last, bool value) noexcept {
      LIBSEMIGROUPS_ASSERT(first < N);
      LIBSEMIGROUPS_ASSERT(last <= N);
      LIBSEMIGROUPS_ASSERT(first < last);
      block_type m = ~0;
      m            = (m >> first);
      m            = (m << (first + (block_count() - last)));
      m            = (m >> (block_count() - last));
      if (value) {
        _block |= m;
      } else {
        _block &= ~m;
      }
      return *this;
    }

    BitSet& reset() noexcept {
      _block = 0;
      return *this;
    }

    BitSet& reset(size_t pos) noexcept {
      LIBSEMIGROUPS_ASSERT(pos < N);
      _block &= ~mask(pos);
      return *this;
    }

    BitSet& reset(size_t first, size_t last) {
      LIBSEMIGROUPS_ASSERT(first < N);
      LIBSEMIGROUPS_ASSERT(last <= N);
      LIBSEMIGROUPS_ASSERT(first < last);
      return set(first, last, false);
    }

    size_t count() const noexcept {
      clear_hi_bits();
      return COUNT_TRUES_BLOCK(_block);
    }

    template <typename S>
    void apply(S&& func) const {
#if LIBSEMIGROUPS_USE_CLZLL && defined(LIBSEMIGROUPS_HAVE___BUILTIN_CLZLL)
      block_type block = _block;
      while (block != 0) {
        block_type t = block & -block;
        size_t     i = static_cast<size_t>(__builtin_ctzll(block));
        if (i >= size()) {
          break;
        }
        func(i);
        block ^= t;
      }
#else
      for (size_t i = 0; i < size(); ++i) {
        if (test(i)) {
          func(i);
        }
      }
#endif
    }

    block_type to_int() const noexcept {
      clear_hi_bits();
      return _block;
    }

    friend std::ostringstream& operator<<(std::ostringstream& os,
                                          BitSet<N> const&    bs) {
      for (size_t i = 0; i < N; ++i) {
        os << bs.test(i);
      }
      return os;
    }

    friend std::ostream& operator<<(std::ostream& os, BitSet<N> const& bs) {
      os << detail::to_string(bs);
      return os;
    }

   private:
    void clear_hi_bits() const noexcept {
      size_t s = block_count() - N;
      _block   = _block << s;
      _block   = _block >> s;
    }

    constexpr size_t block_count() const noexcept {
      return sizeof(block_type) * CHAR_BIT;
    }

    constexpr block_type mask(size_t i) const noexcept {
      // LIBSEMIGROUPS_ASSERT(i < size());
      return static_cast<block_type>(MASK[i]);
    }

    static constexpr uint64_t MASK[64] = {0x1,
                                          0x2,
                                          0x4,
                                          0x8,
                                          0x10,
                                          0x20,
                                          0x40,
                                          0x80,
                                          0x100,
                                          0x200,
                                          0x400,
                                          0x800,
                                          0x1000,
                                          0x2000,
                                          0x4000,
                                          0x8000,
                                          0x10000,
                                          0x20000,
                                          0x40000,
                                          0x80000,
                                          0x100000,
                                          0x200000,
                                          0x400000,
                                          0x800000,
                                          0x1000000,
                                          0x2000000,
                                          0x4000000,
                                          0x8000000,
                                          0x10000000,
                                          0x20000000,
                                          0x40000000,
                                          0x80000000,
                                          0x100000000,
                                          0x200000000,
                                          0x400000000,
                                          0x800000000,
                                          0x1000000000,
                                          0x2000000000,
                                          0x4000000000,
                                          0x8000000000,
                                          0x10000000000,
                                          0x20000000000,
                                          0x40000000000,
                                          0x80000000000,
                                          0x100000000000,
                                          0x200000000000,
                                          0x400000000000,
                                          0x800000000000,
                                          0x1000000000000,
                                          0x2000000000000,
                                          0x4000000000000,
                                          0x8000000000000,
                                          0x10000000000000,
                                          0x20000000000000,
                                          0x40000000000000,
                                          0x80000000000000,
                                          0x100000000000000,
                                          0x200000000000000,
                                          0x400000000000000,
                                          0x800000000000000,
                                          0x1000000000000000,
                                          0x2000000000000000,
                                          0x4000000000000000,
                                          0x8000000000000000};
    mutable block_type        _block;
  };

  template <size_t N>
  constexpr uint64_t BitSet<N>::MASK[64];

  namespace detail {
    template <typename T>
    struct IsBitSetHelper : std::false_type {};

    template <size_t N>
    struct IsBitSetHelper<BitSet<N>> : std::true_type {};
  }  // namespace detail

  template <typename T>
  static constexpr bool IsBitSet = detail::IsBitSetHelper<T>::value;

  namespace detail {
    struct LessBitSet {
      // not noexcept because std::bitset<N>::to_ullong throws
      // std::overflow_error if N exceeds the capacity of a unsigned long
      // long.
      template <size_t N>
      bool operator()(std::bitset<N> const& x, std::bitset<N> const& y) const {
        return x.to_ullong() < y.to_ullong();
      }

      template <size_t N>
      bool operator()(BitSet<N> const& x, BitSet<N> const& y) const noexcept {
        return x < y;
      }
    };
  }  // namespace detail
}  // namespace libsemigroups

namespace std {
  template <size_t N>
  struct hash<libsemigroups::BitSet<N>> {
    using block_type = typename libsemigroups::BitSet<N>::block_type;
    size_t operator()(libsemigroups::BitSet<N> const& bs) const noexcept(
        std::is_nothrow_default_constructible<hash<block_type>>::value) {
      return hash<block_type>()(bs.to_int());
    }
  };
}  // namespace std
#endif  // LIBSEMIGROUPS_BITSET_HPP_
