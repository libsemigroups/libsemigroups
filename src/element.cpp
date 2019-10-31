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

// This file contains implementations of the non-template derived classes of
// the Elements abstract base class.

#include "element.hpp"

#include <algorithm>  // for fill, max_element, find
#include <cmath>      // for abs
#include <ostream>    // for operator<<, basic_ostream, ostringstream
#include <thread>     // for thread, get_id

#include "blocks.hpp"    // for Blocks
#include "report.hpp"    // for THREAD_ID_MANAGER
#include "semiring.hpp"  // for BooleanSemiring, Semiring (ptr only)

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // BooleanMat
  ////////////////////////////////////////////////////////////////////////

  BooleanSemiring const* const BooleanMat::_semiring = new BooleanSemiring();

  BooleanMat::BooleanMat(std::vector<bool> const& matrix)
      : MatrixOverSemiringBase<bool, BooleanMat>(matrix, _semiring) {}

  BooleanMat::BooleanMat(std::vector<std::vector<bool>> const& matrix)
      : MatrixOverSemiringBase<bool, BooleanMat>(matrix, _semiring) {}

  BooleanMat::BooleanMat(size_t degree)
      : BooleanMat(std::vector<bool>(degree * degree)) {}

  BooleanMat::BooleanMat(BooleanMat const& copy)
      : MatrixOverSemiringBase<bool, BooleanMat>(copy._vector, copy._semiring) {
  }

  void BooleanMat::redefine(Element const& x, Element const& y) {
    LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
    LIBSEMIGROUPS_ASSERT(x.degree() == this->degree());
    LIBSEMIGROUPS_ASSERT(&x != this && &y != this);

    size_t                   k;
    size_t                   dim = this->degree();
    std::vector<bool> const& xx  = static_cast<BooleanMat const&>(x)._vector;
    std::vector<bool> const& yy  = static_cast<BooleanMat const&>(y)._vector;

    for (size_t i = 0; i < dim; i++) {
      for (size_t j = 0; j < dim; j++) {
        for (k = 0; k < dim; k++) {
          if (xx[i * dim + k] && yy[k * dim + j]) {
            break;
          }
        }
        _vector[i * dim + j] = (k < dim);
      }
    }
    this->reset_hash_value();
  }

  // Private
  BooleanMat::BooleanMat(bool x) : MatrixOverSemiringBase(x) {}

  // Private
  BooleanMat::BooleanMat(std::vector<bool>     matrix,
                         Semiring<bool> const* semiring)
      : MatrixOverSemiringBase<bool, BooleanMat>(matrix, semiring) {}

  ////////////////////////////////////////////////////////////////////////
  // Bipartitions
  ////////////////////////////////////////////////////////////////////////

  Bipartition::Bipartition()
      : ElementWithVectorDataDefaultHash<uint32_t, Bipartition>(),
        _nr_blocks(UNDEFINED),
        _nr_left_blocks(UNDEFINED),
        _trans_blocks_lookup(),
        _rank(UNDEFINED) {}

  Bipartition::Bipartition(size_t degree) : Bipartition() {
    this->_vector.resize(2 * degree);
  }

  Bipartition::Bipartition(std::vector<uint32_t> const& blocks)
      : ElementWithVectorDataDefaultHash<uint32_t, Bipartition>(blocks),
        _nr_blocks(UNDEFINED),
        _nr_left_blocks(UNDEFINED),
        _trans_blocks_lookup(),
        _rank(UNDEFINED) {
    validate();
  }

  Bipartition::Bipartition(std::vector<uint32_t>&& blocks)
      : ElementWithVectorDataDefaultHash<uint32_t, Bipartition>(
          std::move(blocks)),
        _nr_blocks(UNDEFINED),
        _nr_left_blocks(UNDEFINED),
        _trans_blocks_lookup(),
        _rank(UNDEFINED) {
    validate();
  }

  Bipartition::Bipartition(std::initializer_list<uint32_t> blocks)
      : Bipartition(std::vector<uint32_t>(blocks)) {}

  Bipartition::Bipartition(
      std::initializer_list<std::vector<int32_t>> const& blocks)
      : Bipartition(blocks_to_list(blocks)) {}

  void Bipartition::set_nr_blocks(size_t nr_blocks) {
    LIBSEMIGROUPS_ASSERT(_nr_blocks == UNDEFINED || _nr_blocks == nr_blocks);
    _nr_blocks = nr_blocks;
  }

  void Bipartition::set_nr_left_blocks(size_t nr_left_blocks) {
    LIBSEMIGROUPS_ASSERT(_nr_left_blocks == UNDEFINED
                         || _nr_left_blocks == nr_left_blocks);
    _nr_left_blocks = nr_left_blocks;
  }

  void Bipartition::set_rank(size_t rank) {
    LIBSEMIGROUPS_ASSERT(_rank == UNDEFINED || _rank == rank);
    _rank = rank;
  }

  std::vector<std::vector<uint32_t>>
      Bipartition::_fuse(std::thread::hardware_concurrency() + 1);
  std::vector<std::vector<uint32_t>>
      Bipartition::_lookup(std::thread::hardware_concurrency() + 1);

  void Bipartition::validate() const {
    size_t const n = _vector.size();
    if (n == 0) {
      return;
      // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
      //     } else if (n == 1
      //                && this->_vector[0] ==
      //                std::numeric_limits<uint32_t>::max()) {
      //       return;
      // #endif
    } else if (n % 2 != 0) {
      LIBSEMIGROUPS_EXCEPTION("expected argument of even length");
    }
    size_t next = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t const j = this->_vector[i];
      if (j == next) {
        ++next;
      } else if (j > next) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected %d but found %d, in position %d", next, j, i);
      }
    }
  }

  size_t Bipartition::complexity() const {
    return (_vector.empty() ? 0 : pow(degree(), 2));
  }

  size_t Bipartition::degree() const {
    return (_vector.empty() ? 0 : _vector.size() / 2);
  }

  // the identity of this
  Bipartition Bipartition::identity() const {
    size_t const          n = this->degree();
    std::vector<uint32_t> vector(2 * n);
    std::iota(vector.begin(), vector.begin() + n, 0);
    std::iota(vector.begin() + n, vector.end(), 0);
    return Bipartition(std::move(vector));
  }

  Bipartition Bipartition::identity(size_t n) {
    std::vector<uint32_t> vector(2 * n);
    std::iota(vector.begin(), vector.begin() + n, 0);
    std::iota(vector.begin() + n, vector.end(), 0);
    return Bipartition(std::move(vector));
  }

  // multiply x and y into this
  void Bipartition::redefine(Element const& x,
                             Element const& y,
                             size_t         thread_id) {
    LIBSEMIGROUPS_ASSERT(x.degree() == y.degree());
    LIBSEMIGROUPS_ASSERT(x.degree() == this->degree());
    LIBSEMIGROUPS_ASSERT(&x != this && &y != this);

    uint32_t n = this->degree();

    auto const& xx = static_cast<Bipartition const&>(x);
    auto const& yy = static_cast<Bipartition const&>(y);

    std::vector<uint32_t> const& xblocks = xx._vector;
    std::vector<uint32_t> const& yblocks = yy._vector;

    uint32_t nrx(xx.const_nr_blocks());
    uint32_t nry(yy.const_nr_blocks());

    std::vector<uint32_t>& fuse(_fuse.at(thread_id));
    std::vector<uint32_t>& lookup(_lookup.at(thread_id));

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
      this->_vector[i] = lookup[j];
    }

    for (size_t i = n; i < 2 * n; i++) {
      uint32_t j = fuseit(fuse, yblocks[i] + nrx);
      if (lookup[j] == static_cast<uint32_t>(-1)) {
        lookup[j] = next;
        next++;
      }
      this->_vector[i] = lookup[j];
    }
    this->reset_hash_value();
  }

  inline uint32_t Bipartition::fuseit(std::vector<uint32_t>& fuse,
                                      uint32_t               pos) {
    while (fuse[pos] < pos) {
      pos = fuse[pos];
    }
    return pos;
  }

  // nr blocks

  uint32_t Bipartition::const_nr_blocks() const {
    if (_nr_blocks != UNDEFINED) {
      return _nr_blocks;
    } else if (degree() == 0) {
      return 0;
    }

    return *std::max_element(_vector.begin(), _vector.end()) + 1;
  }

  uint32_t Bipartition::nr_blocks() {
    if (_nr_blocks == UNDEFINED) {
      _nr_blocks = this->const_nr_blocks();
    }
    return _nr_blocks;
  }

  uint32_t Bipartition::nr_left_blocks() {
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

  uint32_t Bipartition::nr_right_blocks() {
    return nr_blocks() - nr_left_blocks() + rank();
  }

  // Transverse blocks lookup table.
  // Returns a vector of bools <out> for the left blocks so that <out[i]> is
  // <true> if and only the <i>th block of <this> is a transverse block.
  //
  // @return a const std::vector<bool>*.

  bool Bipartition::is_transverse_block(size_t index) {
    if (index < nr_left_blocks()) {
      init_trans_blocks_lookup();
      return _trans_blocks_lookup[index];
    }
    return false;
  }

  void Bipartition::init_trans_blocks_lookup() {
    if (_trans_blocks_lookup.empty() && degree() > 0) {
      _trans_blocks_lookup.resize(this->nr_left_blocks());
      for (auto it = _vector.begin() + degree(); it < _vector.end(); it++) {
        if ((*it) < this->nr_left_blocks()) {
          _trans_blocks_lookup[(*it)] = true;
        }
      }
    }
  }

  size_t Bipartition::rank() {
    if (_rank == UNDEFINED) {
      init_trans_blocks_lookup();
      _rank = std::count(
          _trans_blocks_lookup.begin(), _trans_blocks_lookup.end(), true);
    }
    return _rank;
  }

  Blocks* Bipartition::left_blocks() {
    if (degree() == 0) {
      return new Blocks();
    }
    init_trans_blocks_lookup();
    return new Blocks(
        new std::vector<uint32_t>(_vector.begin(),
                                  _vector.begin() + (_vector.size() / 2)),
        new std::vector<bool>(_trans_blocks_lookup));
  }

  Blocks* Bipartition::right_blocks() {
    if (degree() == 0) {
      return new Blocks();
    }

    std::vector<uint32_t>* blocks        = new std::vector<uint32_t>();
    std::vector<bool>*     blocks_lookup = new std::vector<bool>();

    // must reindex the blocks
    std::vector<uint32_t>& lookup
        = _lookup[THREAD_ID_MANAGER.tid(std::this_thread::get_id())];

    lookup.clear();
    lookup.resize(this->nr_blocks(), UNDEFINED);
    uint32_t nr_blocks = 0;

    for (auto it = _vector.begin() + (_vector.size() / 2); it < _vector.end();
         it++) {
      if (lookup[*it] == UNDEFINED) {
        lookup[*it] = nr_blocks;
        blocks_lookup->push_back(this->is_transverse_block(*it));
        nr_blocks++;
      }
      blocks->push_back(lookup[*it]);
    }

    return new Blocks(blocks, blocks_lookup, nr_blocks);
  }

  std::vector<uint32_t>
  Bipartition::blocks_to_list(std::vector<std::vector<int32_t>> blocks) {
    int32_t max = 0;
    int32_t deg = 0;
    for (std::vector<int32_t> block : blocks) {
      for (int32_t x : block) {
        if (std::abs(x) > max) {
          max = std::abs(x);
        }
        deg++;
      }
    }
    if (deg < 2 * max) {
      LIBSEMIGROUPS_EXCEPTION("the union of the given blocks is not "
                              "[%d, -1] ∪ [1, %d], only %d values were given",
                              -max,
                              max,
                              deg);
    }
    if (max >= static_cast<int32_t>(0x40000000)) {
      LIBSEMIGROUPS_EXCEPTION("too many points");
    }

    std::vector<uint32_t> out
        = std::vector<uint32_t>(2 * max, std::numeric_limits<uint32_t>::max());

    for (uint32_t i = 0; i < blocks.size(); ++i) {
      for (int32_t x : blocks[i]) {
        if (x == 0) {
          LIBSEMIGROUPS_EXCEPTION("value out of bounds, expected values in "
                                  "[%d, -1] or [1, %d], found 0",
                                  -max,
                                  max);
        }
        if (x < 0) {
          if (out[static_cast<uint32_t>(max - x - 1)]
              != std::numeric_limits<uint32_t>::max()) {
            LIBSEMIGROUPS_EXCEPTION("duplicate value, found %d more than once",
                                    x);
          }
          out[static_cast<uint32_t>(max - x - 1)] = i;
        } else {
          if (out[static_cast<uint32_t>(x - 1)]
              != std::numeric_limits<uint32_t>::max()) {
            LIBSEMIGROUPS_EXCEPTION("duplicate value, found %d more than once",
                                    x);
          }

          out[static_cast<uint32_t>(x - 1)] = i;
        }
      }
    }
    LIBSEMIGROUPS_ASSERT(
        std::find(out.begin(), out.end(), std::numeric_limits<uint32_t>::max())
        == out.end());
    return out;
  }

  ////////////////////////////////////////////////////////////////////////
  // Projective max-plus matrices
  ////////////////////////////////////////////////////////////////////////

  ProjectiveMaxPlusMatrix::ProjectiveMaxPlusMatrix(
      std::vector<int64_t> const& matrix,
      Semiring<int64_t> const*    semiring)
      : MatrixOverSemiringBase(matrix, semiring) {
    after();  // this is to put the matrix in normal form
  }

  ProjectiveMaxPlusMatrix::ProjectiveMaxPlusMatrix(
      std::vector<std::vector<int64_t>> const& matrix,
      Semiring<int64_t> const*                 semiring)
      : MatrixOverSemiringBase(matrix, semiring) {
    after();  // this is to put the matrix in normal form
  }

  ProjectiveMaxPlusMatrix
      ProjectiveMaxPlusMatrix::operator*(ElementWithVectorData const& y) const {
    ProjectiveMaxPlusMatrix xy(std::vector<int64_t>(pow(y.degree(), 2)),
                               this->semiring());
    xy.Element::redefine(*this, y);
    // after() is called in Element::redefine.
    return xy;
  }

  ProjectiveMaxPlusMatrix::ProjectiveMaxPlusMatrix(int64_t x)
      : MatrixOverSemiringBase(x) {}

  ProjectiveMaxPlusMatrix::ProjectiveMaxPlusMatrix(std::vector<int64_t> matrix)
      : MatrixOverSemiringBase(matrix) {}

  void ProjectiveMaxPlusMatrix::after() {
    int64_t norm = std::numeric_limits<int64_t>::min();
    for (auto const& x : _vector) {
      if (x != NEGATIVE_INFINITY && x > norm) {
        norm = x;
      }
    }
    for (auto& x : _vector) {
      if (x != NEGATIVE_INFINITY) {
        x -= norm;
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Partitioned binary relations (PBRs)
  ////////////////////////////////////////////////////////////////////////

  std::vector<std::vector<bool>>
      PBR::_x_seen(std::thread::hardware_concurrency() + 1);
  std::vector<std::vector<bool>>
      PBR::_y_seen(std::thread::hardware_concurrency() + 1);

  std::vector<detail::DynamicArray2<bool>>
      PBR::_out(std::thread::hardware_concurrency() + 1);
  std::vector<detail::DynamicArray2<bool>>
      PBR::_tmp(std::thread::hardware_concurrency() + 1);

  PBR::PBR(size_t degree)
      : PBR(std::vector<std::vector<uint32_t>>(degree * 2,
                                               std::vector<uint32_t>())) {}

  PBR::PBR(std::initializer_list<std::vector<int32_t>> const& left,
           std::initializer_list<std::vector<int32_t>> const& right)
      : PBR(process_left_right(left, right)) {}

  PBR::PBR(std::initializer_list<std::vector<uint32_t>> vec)
      : ElementWithVectorData<std::vector<uint32_t>, PBR>(vec) {
    validate();
  }

  std::ostringstream& operator<<(std::ostringstream& os, PBR const& pbr) {
    os << "{";
    for (size_t i = 0; i < pbr.degree() * 2 - 1; ++i) {
      os << "{";
      for (size_t j = 0; j < pbr[i].size() - 1; ++j) {
        os << pbr[i][j] << ", ";
      }
      os << detail::to_string(pbr[i].back()) << "}, ";
    }

    os << "{";
    for (size_t j = 0; j < pbr[2 * pbr.degree() - 1].size() - 1; ++j) {
      os << pbr[2 * pbr.degree() - 1][j] << ", ";
    }
    os << detail::to_string(pbr[2 * pbr.degree() - 1].back()) << "}}";
    return os;
  }

  std::ostream& operator<<(std::ostream& os, PBR const& pbr) {
    os << detail::to_string(pbr);
    return os;
  }

  void PBR::validate() const {
    size_t n = this->_vector.size();
    // #ifdef LIBSEMIGROUPS_DENSEHASHMAP
    //     if (n == 1 && this->_vector[0][0] ==
    //     std::numeric_limits<uint32_t>::max()) {
    //       return;
    //     }
    // #endif
    if (n % 2 == 1) {
      LIBSEMIGROUPS_EXCEPTION("expected argument of even length");
    }
    for (size_t u = 0; u < n; ++u) {
      for (auto const& v : this->_vector.at(u)) {
        if (v >= n) {
          LIBSEMIGROUPS_EXCEPTION(
              "entry out of bounds, vertex " + detail::to_string(u)
              + " is adjacent to " + detail::to_string(v)
              + ", should be less than " + detail::to_string(n));
        }
      }
    }
  }

  size_t PBR::complexity() const {
    return pow((2 * this->degree()), 3);
  }

  size_t PBR::degree() const {
    return _vector.size() / 2;
  }

  void PBR::cache_hash_value() const {
    this->_hash_value = 0;
    for (auto const& row : this->_vector) {
      this->_hash_value += vector_hash(row);
    }
  }

  PBR PBR::identity() const {
    std::vector<std::vector<uint32_t>> adj;
    size_t                             n = this->degree();
    adj.reserve(2 * n);
    for (uint32_t i = 0; i < 2 * n; i++) {
      adj.push_back(std::vector<uint32_t>());
    }
    for (uint32_t i = 0; i < n; i++) {
      adj[i].push_back(i + n);
      adj[i + n].push_back(i);
    }
    return PBR(adj);
  }

  PBR PBR::identity(size_t n) {
    std::vector<std::vector<uint32_t>> adj;
    adj.reserve(2 * n);
    for (uint32_t i = 0; i < 2 * n; i++) {
      adj.push_back(std::vector<uint32_t>());
    }
    for (uint32_t i = 0; i < n; i++) {
      adj[i].push_back(i + n);
      adj[i + n].push_back(i);
    }
    return PBR(adj);
  }

  void PBR::redefine(Element const& xx, Element const& yy, size_t thread_id) {
    LIBSEMIGROUPS_ASSERT(xx.degree() == yy.degree());
    LIBSEMIGROUPS_ASSERT(xx.degree() == this->degree());
    LIBSEMIGROUPS_ASSERT(&xx != this && &yy != this);

    PBR const& x = static_cast<PBR const&>(xx);
    PBR const& y = static_cast<PBR const&>(yy);

    uint32_t const n = this->degree();

    std::vector<bool>&           x_seen = _x_seen.at(thread_id);
    std::vector<bool>&           y_seen = _y_seen.at(thread_id);
    detail::DynamicArray2<bool>& tmp    = _tmp.at(thread_id);
    detail::DynamicArray2<bool>& out    = _out.at(thread_id);

    if (x_seen.size() != 2 * n) {
      x_seen.clear();
      x_seen.resize(2 * n, false);
      y_seen.clear();
      y_seen.resize(2 * n, false);
      out.clear();
      out.add_cols(2 * n);
      out.add_rows(2 * n);
      tmp.clear();
      tmp.add_cols(2 * n + 1);
    } else {
      std::fill(x_seen.begin(), x_seen.end(), false);
      std::fill(y_seen.begin(), y_seen.end(), false);
      std::fill(out.begin(), out.end(), false);
      std::fill(tmp.begin(), tmp.end(), false);
    }

    for (size_t i = 0; i < n; i++) {
      for (auto const& j : x[i]) {
        if (j < n) {
          out.set(i, j, true);
        } else if (j < tmp.nr_rows() && tmp.get(j, 0)) {
          unite_rows(out, tmp, i, j);
        } else {
          if (j >= tmp.nr_rows()) {
            tmp.add_rows(j - tmp.nr_rows() + 1);
          }
          tmp.set(j, 0, true);
          x_seen[i] = true;
          y_dfs(x_seen, y_seen, tmp, n, j - n, &x, &y, j);
          unite_rows(out, tmp, i, j);
          std::fill(x_seen.begin(), x_seen.end(), false);
          std::fill(y_seen.begin(), y_seen.end(), false);
        }
        if (std::all_of(out.begin_row(i), out.end_row(i), [](bool val) {
              return val;
            })) {
          break;
        }
      }
    }

    for (size_t i = n; i < 2 * n; i++) {
      for (auto const& j : y[i]) {
        if (j >= n) {
          out.set(i, j, true);
        } else if (j < tmp.nr_rows() && tmp.get(j, 0)) {
          unite_rows(out, tmp, i, j);
        } else {
          if (j >= tmp.nr_rows()) {
            tmp.add_rows(j - tmp.nr_rows() + 1);
          }
          tmp.set(j, 0, true);
          y_seen[i] = true;
          x_dfs(x_seen, y_seen, tmp, n, j + n, &x, &y, j);
          unite_rows(out, tmp, i, j);
          std::fill(x_seen.begin(), x_seen.end(), false);
          std::fill(y_seen.begin(), y_seen.end(), false);
        }
        if (std::all_of(out.begin_row(i), out.end_row(i), [](bool val) {
              return val;
            })) {
          break;
        }
      }
    }

    for (size_t i = 0; i < 2 * n; i++) {
      _vector[i].clear();
      for (size_t j = 0; j < 2 * n; j++) {
        if (out.get(i, j)) {
          _vector[i].push_back(j);
        }
      }
    }
    this->reset_hash_value();
  }

  inline void PBR::unite_rows(detail::DynamicArray2<bool>& out,
                              detail::DynamicArray2<bool>& tmp,
                              size_t const&                i,
                              size_t const&                j) {
    for (size_t k = 0; k < out.nr_cols(); k++) {
      out.set(i, k, (out.get(i, k) || tmp.get(j, k + 1)));
    }
  }

  void PBR::x_dfs(std::vector<bool>&           x_seen,
                  std::vector<bool>&           y_seen,
                  detail::DynamicArray2<bool>& tmp,
                  uint32_t const&              n,
                  uint32_t const&              i,
                  PBR const* const             x,
                  PBR const* const             y,
                  size_t const&                adj) {
    if (!x_seen[i]) {
      x_seen[i] = true;
      for (auto const& j : (*x)[i]) {
        if (j < n) {
          tmp.set(adj, j + 1, true);
        } else {
          y_dfs(x_seen, y_seen, tmp, n, j - n, x, y, adj);
        }
      }
    }
  }

  void PBR::y_dfs(std::vector<bool>&           x_seen,
                  std::vector<bool>&           y_seen,
                  detail::DynamicArray2<bool>& tmp,
                  uint32_t const&              n,
                  uint32_t const&              i,
                  PBR const* const             x,
                  PBR const* const             y,
                  size_t const&                adj) {
    if (!y_seen[i]) {
      y_seen[i] = true;
      for (auto const& j : (*y)[i]) {
        if (j >= n) {
          tmp.set(adj, j + 1, true);
        } else {
          x_dfs(x_seen, y_seen, tmp, n, j + n, x, y, adj);
        }
      }
    }
  }

  std::vector<std::vector<uint32_t>>
  PBR::process_left_right(std::vector<std::vector<int32_t>> const& left,
                          std::vector<std::vector<int32_t>> const& right) {
    size_t                             n = left.size();
    std::vector<std::vector<uint32_t>> out;
    std::vector<uint32_t>              v;

    if (n != right.size()) {
      LIBSEMIGROUPS_EXCEPTION("the two vectors must have the same length");
    }
    if (n > 0x40000000) {
      LIBSEMIGROUPS_EXCEPTION("too many points!");
    }
    for (std::vector<int32_t> vec : left) {
      v = std::vector<uint32_t>();
      for (int32_t x : vec) {
        if (x == 0 || x < -static_cast<int32_t>(n)
            || x > static_cast<int32_t>(n)) {
          LIBSEMIGROUPS_EXCEPTION(
              "value out of bounds in the 1st argument, expected values in "
              "[%d, -1] or [1, %d] but found %d",
              -n,
              n,
              x);
        }
        if (x < 0) {
          v.push_back(static_cast<uint32_t>(n - x - 1));
        }
        if (x > 0) {
          v.push_back(static_cast<uint32_t>(x - 1));
        }
      }
      out.push_back(v);
    }
    for (std::vector<int32_t> vec : right) {
      v = std::vector<uint32_t>();
      for (int32_t x : vec) {
        if (x == 0 || x < -static_cast<int32_t>(n)
            || x > static_cast<int32_t>(n)) {
          LIBSEMIGROUPS_EXCEPTION(
              "value out of bounds in the 1st argument, expected values in "
              "[%d, -1] or [1, %d] but found %d",
              -n,
              n,
              x);
        }
        if (x < 0) {
          v.push_back(static_cast<uint32_t>(n - x - 1));
        }
        if (x > 0) {
          v.push_back(static_cast<uint32_t>(x - 1));
        }
      }
      out.push_back(v);
    }
    return out;
  }
}  // namespace libsemigroups
