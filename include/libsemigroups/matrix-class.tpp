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

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // StaticMatrix
  ////////////////////////////////////////////////////////////////////////

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t R,
            size_t C,
            typename Scalar>
  StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>::StaticMatrix(
      std::initializer_list<typename StaticMatrix::scalar_type> const& c)
      : MatrixCommon(c) {
    static_assert(R == 1,
                  "cannot construct Matrix from the given initializer list, "
                  "incompatible dimensions");
    LIBSEMIGROUPS_ASSERT(c.size() == C);
  }

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t R,
            size_t C,
            typename Scalar>
  StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>::StaticMatrix(
      size_t r,
      size_t c)
      : StaticMatrix() {
    (void) r;
    (void) c;
    LIBSEMIGROUPS_ASSERT(r == number_of_rows());
    LIBSEMIGROUPS_ASSERT(c == number_of_cols());
  }

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t R,
            size_t C,
            typename Scalar>
  StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>
  StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>::one(size_t n) {
    // If specified the value of n must equal R or otherwise weirdness will
    // ensue...
    LIBSEMIGROUPS_ASSERT(n == 0 || n == R);
    (void) n;
#if defined(__APPLE__) && defined(__clang__) \
    && (__clang_major__ == 13 || __clang_major__ == 14)
    // With Apple clang version 13.1.6 (clang-1316.0.21.2.5) something goes
    // wrong and the value R is optimized away somehow, meaning that the
    // values on the diagonal aren't actually set. This only occurs when
    // libsemigroups is compiled with -O2 or higher.
    size_t volatile const m = R;
#else
    size_t const m = R;
#endif
    StaticMatrix x(m, m);
    std::fill(x.begin(), x.end(), ZeroOp()());
    for (size_t r = 0; r < m; ++r) {
      x(r, r) = OneOp()();
    }
    return x;
  }

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t R,
            size_t C,
            typename Scalar>
  StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>
  StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>::one(
      void const* ptr,
      size_t      n) {
    (void) ptr;
    LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    LIBSEMIGROUPS_ASSERT(n == 0 || n == R);
    return one(n);
  }

  ////////////////////////////////////////////////////////////////////////
  // DynamicMatrix<Semiring, Scalar>
  ////////////////////////////////////////////////////////////////////////

  template <typename Semiring, typename Scalar>
  DynamicMatrix<Semiring, Scalar>::DynamicMatrix(
      Semiring const*                                                  sr,
      std::initializer_list<std::initializer_list<scalar_type>> const& rws)
      : MatrixDynamicDim(rws.size(), std::empty(rws) ? 0 : rws.begin()->size()),
        MatrixCommon(rws),
        _semiring(sr) {}

  template <typename Semiring, typename Scalar>
  DynamicMatrix<Semiring, Scalar>
  DynamicMatrix<Semiring, Scalar>::one(Semiring const* semiring, size_t n) {
    DynamicMatrix x(semiring, n, n);
    std::fill(x.begin(), x.end(), x.scalar_zero());
    for (size_t r = 0; r < n; ++r) {
      x(r, r) = x.scalar_one();
    }
    return x;
  }

  template <typename Semiring, typename Scalar>
  void DynamicMatrix<Semiring, Scalar>::swap(DynamicMatrix& that) noexcept {
    static_cast<MatrixDynamicDim&>(*this).swap(
        static_cast<MatrixDynamicDim&>(that));
    static_cast<MatrixCommon&>(*this).swap(static_cast<MatrixCommon&>(that));
    std::swap(_semiring, that._semiring);
  }

  ////////////////////////////////////////////////////////////////////////
  // MaxPlusTruncSemiring
  ////////////////////////////////////////////////////////////////////////

  template <typename Scalar>
  MaxPlusTruncSemiring<Scalar>::MaxPlusTruncSemiring(Scalar threshold)
      : _threshold(threshold) {
    if (threshold < 0) {
      LIBSEMIGROUPS_EXCEPTION("expected non-negative value, found {}",
                              threshold);
    }
  }

  template <typename Scalar>
  Scalar MaxPlusTruncSemiring<Scalar>::product_no_checks(Scalar x, Scalar y)
      const noexcept {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold) || x == NEGATIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold) || y == NEGATIVE_INFINITY);
    if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
      return NEGATIVE_INFINITY;
    }
    return std::min(x + y, _threshold);
  }

  template <typename Scalar>
  Scalar MaxPlusTruncSemiring<Scalar>::plus_no_checks(Scalar x,
                                                      Scalar y) const noexcept {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold) || x == NEGATIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold) || y == NEGATIVE_INFINITY);
    if (x == NEGATIVE_INFINITY) {
      return y;
    } else if (y == NEGATIVE_INFINITY) {
      return x;
    }
    return std::max(x, y);
  }

  ////////////////////////////////////////////////////////////////////////
  // MinPlusTruncProd
  ////////////////////////////////////////////////////////////////////////

  template <size_t T, typename Scalar>
  Scalar MinPlusTruncProd<T, Scalar>::operator()(Scalar x,
                                                 Scalar y) const noexcept {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= static_cast<Scalar>(T))
                         || x == POSITIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= static_cast<Scalar>(T))
                         || y == POSITIVE_INFINITY);
    if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
      return POSITIVE_INFINITY;
    }
    return std::min(x + y, static_cast<Scalar>(T));
  }

  ////////////////////////////////////////////////////////////////////////
  // MinPlusTruncSemiring
  ////////////////////////////////////////////////////////////////////////

  template <typename Scalar>
  MinPlusTruncSemiring<Scalar>::MinPlusTruncSemiring(Scalar threshold)
      : _threshold(threshold) {
    if constexpr (std::is_signed<Scalar>::value) {
      if (threshold < 0) {
        LIBSEMIGROUPS_EXCEPTION("expected non-negative value, found {}",
                                threshold);
      }
    }
  }

  template <typename Scalar>
  Scalar MinPlusTruncSemiring<Scalar>::product_no_checks(Scalar x, Scalar y)
      const noexcept {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold) || x == POSITIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold) || y == POSITIVE_INFINITY);
    if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
      return POSITIVE_INFINITY;
    }
    return std::min(x + y, _threshold);
  }

  template <typename Scalar>
  Scalar MinPlusTruncSemiring<Scalar>::plus_no_checks(Scalar x,
                                                      Scalar y) const noexcept {
    LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold) || x == POSITIVE_INFINITY);
    LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold) || y == POSITIVE_INFINITY);
    if (x == POSITIVE_INFINITY) {
      return y;
    } else if (y == POSITIVE_INFINITY) {
      return x;
    }
    return std::min(x, y);
  }

  ////////////////////////////////////////////////////////////////////////
  // detail helpers
  ////////////////////////////////////////////////////////////////////////

  template <size_t T, size_t P, typename Scalar>
  constexpr Scalar detail::thresholdperiod(Scalar x) noexcept {
    if (x > T) {
      return T + (x - T) % P;
    }
    return x;
  }

  template <typename Scalar>
  inline Scalar detail::thresholdperiod(Scalar x,
                                        Scalar threshold,
                                        Scalar period) noexcept {
    if (x > threshold) {
      return threshold + (x - threshold) % period;
    }
    return x;
  }

  ////////////////////////////////////////////////////////////////////////
  // detail::ProjMaxPlusMat
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void detail::ProjMaxPlusMat<T>::normalize(bool force) const {
    if ((_is_normalized && !force) || (_underlying_mat.number_of_rows() == 0)
        || (_underlying_mat.number_of_cols() == 0)) {
      _is_normalized = true;
      return;
    }
    scalar_type const n
        = *std::max_element(_underlying_mat.cbegin(), _underlying_mat.cend());
    std::for_each(
        _underlying_mat.begin(), _underlying_mat.end(), [&n](scalar_type& s) {
          if (s != NEGATIVE_INFINITY) {
            s -= n;
          }
        });
    _is_normalized = true;
  }

}  // namespace libsemigroups
