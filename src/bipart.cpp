//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell
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

// This file contains the implementation of the Bipartition and Blocks classes.

#include "libsemigroups/bipart.hpp"

#include <limits>   // for numeric_limits
#include <numeric>  // for iota
#include <thread>   // for get_id
#include <thread>   // for thread

#include "libsemigroups/constants.hpp"  // for UNDEFINED, operator==, operator!=
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/report.hpp"  // for THREAD_ID_MANAGER, ThreadIdManager

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Helpers
  ////////////////////////////////////////////////////////////////////////

  namespace {
    std::vector<uint32_t>& thread_lookup(size_t thread_id) {
      static std::vector<std::vector<uint32_t>> lookups(
          std::thread::hardware_concurrency() + 1);
      LIBSEMIGROUPS_ASSERT(thread_id < lookups.size());
      return lookups[thread_id];
    }

    std::vector<uint32_t>
    blocks_to_list(std::vector<std::vector<int32_t>> const& blocks) {
      int32_t m = 0;
      for (std::vector<int32_t> const& b : blocks) {
        m = std::max(std::abs(*std::max_element(b.cbegin(), b.cend())), m);
      }
      std::vector<uint32_t> out
          = std::vector<uint32_t>(2 * m, std::numeric_limits<uint32_t>::max());

      for (uint32_t i = 0; i < blocks.size(); ++i) {
        for (int32_t x : blocks[i]) {
          if (x < 0) {
            out[static_cast<uint32_t>(m - x - 1)] = i;
          } else {
            out[static_cast<uint32_t>(x - 1)] = i;
          }
        }
      }
      LIBSEMIGROUPS_ASSERT(std::find(out.begin(),
                                     out.end(),
                                     std::numeric_limits<uint32_t>::max())
                           == out.end());
      return out;
    }

    inline uint32_t fuseit(std::vector<uint32_t>& fuse, uint32_t pos) {
      while (fuse[pos] < pos) {
        pos = fuse[pos];
      }
      return pos;
    }
  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // Blocks
  ////////////////////////////////////////////////////////////////////////

  Blocks::~Blocks() = default;

  Blocks::Blocks(const_iterator first, const_iterator last) {
    _blocks.assign(first, last);
    // must reindex the blocks
    std::vector<uint32_t>& lookup
        = thread_lookup(THREAD_ID_MANAGER.tid(std::this_thread::get_id()));

    lookup.clear();
    lookup.resize(2 * degree(), UNDEFINED);
    uint32_t n = 0;

    for (auto it = _blocks.begin(); it < _blocks.end(); ++it) {
      if (lookup[*it] == UNDEFINED) {
        lookup[*it] = n++;
      }
      *it = lookup[*it];
    }
    _lookup.resize(n, false);
  }

  bool Blocks::operator==(Blocks const& that) const {
    return _blocks == that._blocks && _lookup == that._lookup;
  }

  bool Blocks::operator<(Blocks const& that) const {
    if (_blocks != that._blocks) {
      return _blocks < that._blocks;
    }
    for (size_t i = 0; i < number_of_blocks(); i++) {
      if (_lookup[i] != that._lookup[i]) {
        return _lookup[i] > that._lookup[i];
      }
    }
    return false;
  }

  uint32_t Blocks::rank() const {
    return std::count(_lookup.cbegin(), _lookup.cend(), true);
  }

  size_t Blocks::hash_value() const noexcept {
    if (number_of_blocks() == 0) {
      return 0;
    }
    size_t       seed = 0;
    size_t const n    = _blocks.size();
    for (auto const& index : _blocks) {
      seed = ((seed * n) + index);
    }
    for (auto val : _lookup) {
      seed = ((seed * n) + val);
    }
    return seed;
  }

  void validate(Blocks const& x) {
    size_t const n = x.degree();
    size_t const m = std::distance(x.cbegin_lookup(), x.cend_lookup());
    if (n == 0) {
      if (m != 0) {
        LIBSEMIGROUPS_EXCEPTION("expected lookup of size 0, found %llu",
                                uint64_t(m));
      }
    } else {
      size_t next = 0;
      for (auto it = x.cbegin(); it < x.cend(); ++it) {
        if (*it == next) {
          ++next;
        } else if (*it > next) {
          LIBSEMIGROUPS_EXCEPTION(
              "expected %llu but found %llu, in position %llu",
              uint64_t(next),
              uint64_t(*it),
              uint64_t(it - x.cbegin()));
        }
      }
      if (next != m) {
        LIBSEMIGROUPS_EXCEPTION("expected lookup of size %llu, found %llu",
                                uint64_t(next),
                                uint64_t(m));
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Bipartition friends
  ////////////////////////////////////////////////////////////////////////

  // TODO(later) check other things like _nr_blocks is correct etc...
  void validate(Bipartition const& x) {
    size_t const n = static_cast<size_t>(std::distance(x.cbegin(), x.cend()));
    if (2 * x.degree() != n) {
      LIBSEMIGROUPS_EXCEPTION(
          "the degree of a bipartition must be even, found %llu", uint64_t(n));
    }
    size_t next = 0;
    for (size_t i = 0; i < n; ++i) {
      if (x[i] == next) {
        ++next;
      } else if (x[i] > next) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected %llu but found %llu, in position %llu",
            uint64_t(next),
            uint64_t(x[i]),
            uint64_t(i));
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Bipartition friends
  ////////////////////////////////////////////////////////////////////////

  // TODO(later) Should be defined for all of the new element types
  Bipartition operator*(Bipartition const& x, Bipartition const& y) {
    Bipartition xy(x.degree());
    xy.product_inplace(x, y);
    return xy;
  }

  ////////////////////////////////////////////////////////////////////////
  // Bipartitions
  ////////////////////////////////////////////////////////////////////////

  Bipartition::Bipartition()
      : _nr_blocks(UNDEFINED),
        _nr_left_blocks(UNDEFINED),
        _trans_blocks_lookup(),
        _rank(UNDEFINED),
        _vector() {}

  Bipartition::Bipartition(Bipartition const&) = default;
  Bipartition::Bipartition(Bipartition&&)      = default;
  Bipartition& Bipartition::operator=(Bipartition const&) = default;
  Bipartition& Bipartition::operator=(Bipartition&&) = default;

  Bipartition::Bipartition(size_t degree) : Bipartition() {
    _vector.resize(2 * degree);
  }

  Bipartition::Bipartition(std::vector<uint32_t> const& blocks)
      : Bipartition() {
    _vector = blocks;
  }

  Bipartition::Bipartition(std::vector<uint32_t>&& blocks) : Bipartition() {
    _vector = std::move(blocks);
  }

  Bipartition::Bipartition(std::initializer_list<uint32_t> const& blocks)
      : Bipartition(std::vector<uint32_t>(blocks)) {}

  Bipartition::Bipartition(
      std::initializer_list<std::vector<int32_t>> const& blocks)
      : Bipartition(blocks_to_list(blocks)) {}

  Bipartition::~Bipartition() = default;

  void Bipartition::set_number_of_blocks(size_t number_of_blocks) noexcept {
    LIBSEMIGROUPS_ASSERT(_nr_blocks == UNDEFINED
                         || _nr_blocks == number_of_blocks);
    _nr_blocks = number_of_blocks;
  }

  void Bipartition::set_number_of_left_blocks(
      size_t number_of_left_blocks) noexcept {
    LIBSEMIGROUPS_ASSERT(_nr_left_blocks == UNDEFINED
                         || _nr_left_blocks == number_of_left_blocks);
    _nr_left_blocks = number_of_left_blocks;
  }

  void Bipartition::set_rank(size_t rank) noexcept {
    LIBSEMIGROUPS_ASSERT(_rank == UNDEFINED || _rank == rank);
    _rank = rank;
  }

  size_t Bipartition::degree() const noexcept {
    return (_vector.empty() ? 0 : _vector.size() / 2);
  }

  // Non-static
  Bipartition Bipartition::identity() const {
    return Bipartition::identity(degree());
  }

  // Static
  Bipartition Bipartition::identity(size_t n) {
    std::vector<uint32_t> vector(2 * n);
    std::iota(vector.begin(), vector.begin() + n, 0);
    std::iota(vector.begin() + n, vector.end(), 0);
    return Bipartition(std::move(vector));
  }

  // multiply x and y into this
  void Bipartition::product_inplace(Bipartition const& x,
                                    Bipartition const& y,
                                    size_t             thread_id) {
    LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
    LIBSEMIGROUPS_ASSERT(x.degree() == degree());
    LIBSEMIGROUPS_ASSERT(&x != this && &y != this);

    uint32_t n = degree();

    auto const& xx = static_cast<Bipartition const&>(x);
    auto const& yy = static_cast<Bipartition const&>(y);

    std::vector<uint32_t> const& xblocks = xx._vector;
    std::vector<uint32_t> const& yblocks = yy._vector;

    uint32_t const nrx(xx.number_of_blocks());
    uint32_t const nry(yy.number_of_blocks());

    static std::vector<std::vector<uint32_t>> fuses(
        std::thread::hardware_concurrency() + 1);
    LIBSEMIGROUPS_ASSERT(thread_id < fuses.size());
    std::vector<uint32_t>& fuse = fuses[thread_id];
    std::vector<uint32_t>& lookup(thread_lookup(thread_id));

    fuse.resize(nrx + nry);
    std::iota(fuse.begin(), fuse.end(), 0);
    lookup.assign(nrx + nry, -1);

    for (size_t i = 0; i < n; i++) {
      uint32_t j = fuseit(fuse, xblocks[i + n]);
      uint32_t k = fuseit(fuse, yblocks[i] + nrx);
      if (j != k) {
        if (j < k) {
          fuse[k] = j;
        } else {
          fuse[j] = k;
        }
      }
    }

    uint32_t next = 0;

    for (size_t i = 0; i < n; i++) {
      uint32_t j = fuseit(fuse, xblocks[i]);
      if (lookup[j] == static_cast<uint32_t>(-1)) {
        lookup[j] = next;
        next++;
      }
      _vector[i] = lookup[j];
    }

    for (size_t i = n; i < 2 * n; i++) {
      uint32_t j = fuseit(fuse, yblocks[i] + nrx);
      if (lookup[j] == static_cast<uint32_t>(-1)) {
        lookup[j] = next;
        next++;
      }
      _vector[i] = lookup[j];
    }
  }

  uint32_t Bipartition::number_of_blocks() const {
    if (_nr_blocks != UNDEFINED) {
      return _nr_blocks;
    } else if (degree() == 0) {
      return 0;
    }

    return *std::max_element(_vector.begin(), _vector.end()) + 1;
  }

  uint32_t Bipartition::number_of_left_blocks() {
    if (_nr_left_blocks == UNDEFINED) {
      if (degree() == 0) {
        _nr_left_blocks = 0;
      } else {
        _nr_left_blocks
            = *std::max_element(_vector.begin(),
                                _vector.begin() + (_vector.size() / 2))
              + 1;
      }
    }
    return _nr_left_blocks;
  }

  uint32_t Bipartition::number_of_right_blocks() {
    return number_of_blocks() - number_of_left_blocks() + rank();
  }

  bool Bipartition::is_transverse_block(size_t index) {
    if (index < number_of_left_blocks()) {
      init_trans_blocks_lookup();
      return _trans_blocks_lookup[index];
    }
    return false;
  }

  void Bipartition::init_trans_blocks_lookup() {
    if (_trans_blocks_lookup.empty() && degree() > 0) {
      _trans_blocks_lookup.resize(number_of_left_blocks());
      for (auto it = _vector.begin() + degree(); it < _vector.end(); it++) {
        if ((*it) < number_of_left_blocks()) {
          _trans_blocks_lookup[(*it)] = true;
        }
      }
    }
  }

  size_t Bipartition::rank() {
    if (_rank == UNDEFINED) {
      _rank = std::count(cbegin_lookup(), cend_lookup(), true);
    }
    return _rank;
  }

  Blocks* Bipartition::left_blocks() {
    Blocks*      result = new Blocks(cbegin_left_blocks(), cend_left_blocks());
    size_t const n      = degree();
    for (size_t i = 0; i < n; ++i) {
      result->set_is_transverse_block((*this)[i],
                                      is_transverse_block((*this)[i]));
    }
    return result;
  }

  Blocks* Bipartition::right_blocks() {
    Blocks* result = new Blocks(cbegin_right_blocks(), cend_right_blocks());
    size_t const n = degree();
    for (size_t i = n; i < 2 * n; ++i) {
      result->set_is_transverse_block((*result)[i - n],
                                      is_transverse_block((*this)[i]));
    }
    return result;
  }

}  // namespace libsemigroups
