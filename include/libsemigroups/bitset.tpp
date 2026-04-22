//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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
// This file contains implementations of the member functions for the
// BitSet class.
//

namespace libsemigroups {
  template <size_t N>
  template <typename T>
  BitSet<N>::BitSet(T first, T last) {
    LIBSEMIGROUPS_ASSERT(first <= last);
    size_t const K = std::distance(first, last);
    if (K > size()) {
      LIBSEMIGROUPS_EXCEPTION("the size of the container is {}, trying to "
                              "initialize with {} items",
                              size(),
                              K)
    }
    auto it = first;
    for (size_t i = 0; i < K; ++i, ++it) {
      set(i, *it);
    }
  }

  template <size_t N>
  BitSet<N>& BitSet<N>::set(size_t pos, bool value) noexcept {
    LIBSEMIGROUPS_ASSERT(pos < N);
    if (value) {
      _block |= mask(pos);
    } else {
      _block &= ~mask(pos);
    }
    return *this;
  }

  template <size_t N>
  BitSet<N>& BitSet<N>::set(size_t first, size_t last, bool value) noexcept {
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

  template <size_t N>
  BitSet<N>& BitSet<N>::reset(size_t first, size_t last) {
    LIBSEMIGROUPS_ASSERT(first < N);
    LIBSEMIGROUPS_ASSERT(last <= N);
    LIBSEMIGROUPS_ASSERT(first < last);
    return set(first, last, false);
  }

  template <size_t N>
  template <typename S>
  void BitSet<N>::apply(S&& func) const {
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

  template <size_t N>
  std::ostringstream& operator<<(std::ostringstream& os, BitSet<N> const& bs) {
    for (size_t i = 0; i < N; ++i) {
      os << bs.test(i);
    }
    return os;
  }

}  // namespace libsemigroups
