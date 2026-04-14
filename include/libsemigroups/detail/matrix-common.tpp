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

namespace libsemigroups::detail {

  template <typename Scalar>
  std::string entry_repr(Scalar a) {
    if constexpr (std::is_same_v<Scalar, NegativeInfinity>
                  || std::is_signed_v<Scalar>) {
      if (a == NEGATIVE_INFINITY) {
        return u8"-\u221E";
      }
    }
    if (a == POSITIVE_INFINITY) {
      return u8"+\u221E";
    }
    return fmt::format("{}", a);
  }

  ////////////////////////////////////////////////////////////////////////
  // MatrixCommon
  ////////////////////////////////////////////////////////////////////////

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  template <typename T>
  void MatrixCommon<Container, Subclass, RowView, Semiring>::init(T const& m) {
    size_t const R = m.size();
    if (R == 0) {
      return;
    }
    size_t const C = m.begin()->size();
    resize(R, C);
    for (size_t r = 0; r < R; ++r) {
      auto row = m.begin() + r;
      for (size_t c = 0; c < C; ++c) {
        _container[r * C + c] = *(row->begin() + c);
      }
    }
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  // not noexcept because mem allocate is required
  Subclass MatrixCommon<Container, Subclass, RowView, Semiring>::one() const {
    size_t const n = number_of_rows();
    Subclass     x(semiring(), n, n);
    std::fill(x.begin(), x.end(), scalar_zero());
    for (size_t r = 0; r < n; ++r) {
      x(r, r) = scalar_one();
    }
    return x;
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  // not noexcept because memory is allocated
  void MatrixCommon<Container, Subclass, RowView, Semiring>::
      product_inplace_no_checks(Subclass const& A, Subclass const& B) {
    LIBSEMIGROUPS_ASSERT(number_of_rows() == number_of_cols());
    LIBSEMIGROUPS_ASSERT(A.number_of_rows() == number_of_rows());
    LIBSEMIGROUPS_ASSERT(B.number_of_rows() == number_of_rows());
    LIBSEMIGROUPS_ASSERT(A.number_of_cols() == number_of_cols());
    LIBSEMIGROUPS_ASSERT(B.number_of_cols() == number_of_cols());
    LIBSEMIGROUPS_ASSERT(&A != this);
    LIBSEMIGROUPS_ASSERT(&B != this);

    // Benchmarking boolean matrix multiplication reveals that using a
    // non-static container_type gives the best performance, when compared
    // to static container_type the performance is more or less the same
    // (but not thread-safe), and there appears to be a performance
    // penalty of about 50% when using static thread_local container_type
    // (when compiling with clang).
    size_t const             N = A.number_of_rows();
    std::vector<scalar_type> tmp(N, 0);

    for (size_t c = 0; c < N; c++) {
      for (size_t i = 0; i < N; i++) {
        tmp[i] = B(i, c);
      }
      for (size_t r = 0; r < N; r++) {
        (*this)(r, c) = std::inner_product(
            A._container.begin() + r * N,
            A._container.begin() + (r + 1) * N,
            tmp.begin(),
            scalar_zero(),
            [this](scalar_type x, scalar_type y) {
              return this->plus_no_checks(x, y);
            },
            [this](scalar_type x, scalar_type y) {
              return this->product_no_checks(x, y);
            });
      }
    }
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  void MatrixCommon<Container, Subclass, RowView, Semiring>::product_inplace(
      Subclass const& A,
      Subclass const& B) {
    matrix::throw_if_not_square(*this, "\"*this\"");
    matrix::throw_if_bad_dim(static_cast<Subclass const&>(*this),
                             A,
                             "\"*this\"",
                             "the 1st argument");
    matrix::throw_if_bad_dim(static_cast<Subclass const&>(*this),
                             B,
                             "\"*this\"",
                             "the 2nd argument");
    if (&A == this) {
      LIBSEMIGROUPS_EXCEPTION("the 1st argument (matrix) cannot be the "
                              "same object as \"*this\"")
    } else if (&B == this) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument (matrix) cannot be the "
                              "same object as \"*this\"")
    }
    product_inplace_no_checks(A, B);
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  void
  MatrixCommon<Container, Subclass, RowView, Semiring>::plus_inplace_no_checks(
      Subclass const& that) {
    LIBSEMIGROUPS_ASSERT(that.number_of_rows() == number_of_rows());
    LIBSEMIGROUPS_ASSERT(that.number_of_cols() == number_of_cols());
    for (size_t i = 0; i < _container.size(); ++i) {
      _container[i] = plus_no_checks(_container[i], that._container[i]);
    }
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  void MatrixCommon<Container, Subclass, RowView, Semiring>::operator+=(
      Subclass const& that) {
    matrix::throw_if_bad_dim(static_cast<Subclass const&>(*this),
                             that,
                             "the 1st summand",
                             "the 2nd summand");
    plus_inplace_no_checks(that);
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  void MatrixCommon<Container, Subclass, RowView, Semiring>::operator+=(
      RowView const& that) {
    if (number_of_rows() != 1 || number_of_cols() != that.size()) {
      LIBSEMIGROUPS_EXCEPTION("expected matrices with the same dimensions, "
                              "the 1st summand is a {}x{} matrix, and the "
                              "2nd summand is a 1x{} matrix",
                              number_of_rows(),
                              number_of_cols(),
                              that.size());
    }
    plus_inplace_no_checks(that);
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  Subclass MatrixCommon<Container, Subclass, RowView, Semiring>::operator+(
      Subclass const& y) const {
    matrix::throw_if_bad_dim(static_cast<Subclass const&>(*this),
                             y,
                             "the 1st summand",
                             "the 2nd summand");
    return plus_no_checks(y);
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  Subclass MatrixCommon<Container, Subclass, RowView, Semiring>::operator*(
      Subclass const& y) const {
    matrix::throw_if_not_square(*this, "the 1st factor");
    matrix::throw_if_bad_dim(static_cast<Subclass const&>(*this),
                             y,
                             "the 1st factor",
                             "the 2nd factor");
    return product_no_checks(y);
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  template <typename Iterator>
  std::pair<typename Container::value_type, typename Container::value_type>
  MatrixCommon<Container, Subclass, RowView, Semiring>::coords(
      Iterator const& it) const {
    static_assert(
        std::is_same_v<Iterator, iterator>
            || std::is_same_v<Iterator, const_iterator>,
        "the parameter it must be of type iterator or const_iterator");
    scalar_type const v = std::distance(_container.begin(), it);
    return std::make_pair(v / number_of_cols(), v % number_of_cols());
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  void MatrixCommon<Container, Subclass, RowView, Semiring>::
      transpose_no_checks() noexcept {
    LIBSEMIGROUPS_ASSERT(number_of_rows() == number_of_cols());
    if (number_of_rows() == 0) {
      return;
    }
    auto& x = *this;
    for (size_t r = 0; r < number_of_rows() - 1; ++r) {
      for (size_t c = r + 1; c < number_of_cols(); ++c) {
        std::swap(x(r, c), x(c, r));
      }
    }
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  RowView MatrixCommon<Container, Subclass, RowView, Semiring>::row_no_checks(
      size_t i) const {
    auto& container = const_cast<Container&>(_container);
    return RowView(static_cast<Subclass const*>(this),
                   container.begin() + i * number_of_cols(),
                   number_of_cols());
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  RowView
  MatrixCommon<Container, Subclass, RowView, Semiring>::row(size_t i) const {
    if (i >= number_of_rows()) {
      LIBSEMIGROUPS_EXCEPTION(
          "index out of range, expected value in [{}, {}), found {}",
          0,
          number_of_rows(),
          i);
    }
    return row_no_checks(i);
  }

  template <typename Container,
            typename Subclass,
            typename RowView,
            typename Semiring>
  template <typename T>
  void MatrixCommon<Container, Subclass, RowView, Semiring>::rows(T& x) const {
    auto& container = const_cast<Container&>(_container);
    for (auto itc = container.begin(); itc != container.end();
         itc += number_of_cols()) {
      x.emplace_back(static_cast<Subclass const*>(this), itc, number_of_cols());
    }
    LIBSEMIGROUPS_ASSERT(x.size() == number_of_rows());
  }

  ////////////////////////////////////////////////////////////////////////
  // RowViewCommon
  ////////////////////////////////////////////////////////////////////////

  template <typename Mat, typename Subclass>
  void throw_if_bad_dim(RowViewCommon<Mat, Subclass> const& x,
                        RowViewCommon<Mat, Subclass> const& y,
                        std::string_view                    arg_desc_x,
                        std::string_view                    arg_desc_y) {
    if (x.size() != y.size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected matrices with the same dimensions, {} is a "
          "1x{} matrix, and {} is a 1x{} matrix",
          arg_desc_x,
          x.size(),
          arg_desc_y,
          y.size());
    }
  }

  template <typename Mat, typename Subclass>
  void
  RowViewCommon<Mat, Subclass>::plus_inplace_no_checks(RowViewCommon const& x) {
    auto& this_ = *this;
    for (size_t i = 0; i < size(); ++i) {
      this_[i] = plus_no_checks(this_[i], x[i]);
    }
  }

  // This is out of line because throw_if_bad_dim is implemented after this is
  // declared
  template <typename Mat, typename Subclass>
  void RowViewCommon<Mat, Subclass>::operator+=(RowViewCommon const& x) {
    throw_if_bad_dim(*this, x, "the 1st summand", "the 2nd summand");
    plus_inplace_no_checks(x);
  }

  // This is out of line because throw_if_bad_dim is implemented after this is
  // declared
  template <typename Mat, typename Subclass>
  typename RowViewCommon<Mat, Subclass>::Row
  RowViewCommon<Mat, Subclass>::operator+(RowViewCommon const& x) {
    throw_if_bad_dim(*this, x, "the 1st summand", "the 2nd summand");
    return plus_no_checks(x);
  }
}  // namespace libsemigroups::detail
