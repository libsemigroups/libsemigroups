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

#include <algorithm>    // for lexicographical_compare
#include <array>        // for array
#include <cstddef>      // for size_t
#include <numeric>      // for accumulate
#include <type_traits>  // for move, swap, dec...
#include <utility>      // for operator==, pair
#include <vector>       // for vector

#define CATCH_CONFIG_ENABLE_PAIR_STRINGMAKER

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS, REQUIRE_NOTHROW
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/adapters.hpp"      // for Complexity, Degree
#include "libsemigroups/bmat8.hpp"         // for BMat8
#include "libsemigroups/constants.hpp"     // for NEGATIVE_INFINITY
#include "libsemigroups/containers.hpp"    // for StaticVector1
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_A...
#include "libsemigroups/exception.hpp"     // for LibsemigroupsEx...
#include "libsemigroups/fastest-bmat.hpp"  // for FastestBMat
#include "libsemigroups/matrix.hpp"        // for BMat, NTPMat
#include "libsemigroups/report.hpp"        // for ReportGuard

namespace libsemigroups {
  template <size_t N>
  class BitSet;

  constexpr bool REPORT = false;

  namespace {

    ////////////////////////////////////////////////////////////////////////
    // For comparison
    ////////////////////////////////////////////////////////////////////////

    template <typename Plus, typename Container>
    struct RowAddition {
      void operator()(Container& x, Container const& y) const {
        LIBSEMIGROUPS_ASSERT(x.size() == y.size());
        for (size_t i = 0; i < x.size(); ++i) {
          x[i] = Plus()(x[i], y[i]);
        }
      }

      void operator()(Container&       res,
                      Container const& x,
                      Container const& y) const {
        LIBSEMIGROUPS_ASSERT(res.size() == x.size());
        LIBSEMIGROUPS_ASSERT(x.size() == y.size());
        for (size_t i = 0; i < x.size(); ++i) {
          res[i] = Plus()(x[i], y[i]);
        }
      }
    };

    template <typename Prod, typename Container>
    Container scalar_row_product(Container                      row,
                                 typename Container::value_type scalar) {
      Container out(row);
      for (size_t i = 0; i < out.size(); ++i) {
        out[i] = Prod()(out[i], scalar);
      }
      return out;
    }

    template <size_t dim, size_t thresh>
    void tropical_max_plus_row_basis(std::vector<std::array<int, dim>>& rows) {
      static thread_local std::vector<std::array<int, dim>> buf;
      buf.clear();
      std::sort(rows.begin(), rows.end());
      for (size_t row = 0; row < rows.size(); ++row) {
        std::array<int, dim> sum;
        sum.fill(NEGATIVE_INFINITY);
        if (row == 0 || rows[row] != rows[row - 1]) {
          for (size_t row2 = 0; row2 < row; ++row2) {
            int max_scalar = thresh;
            for (size_t col = 0; col < dim; ++col) {
              if (rows[row2][col] == NEGATIVE_INFINITY) {
                continue;
              }
              if (rows[row][col] >= rows[row2][col]) {
                if (rows[row][col] != thresh) {
                  max_scalar
                      = std::min(max_scalar, rows[row][col] - rows[row2][col]);
                }
              } else {
                max_scalar = NEGATIVE_INFINITY;
                break;
              }
            }
            if (max_scalar != NEGATIVE_INFINITY) {
              auto scalar_prod
                  = scalar_row_product<MaxPlusTruncProd<thresh, int>,
                                       std::array<int, dim>>(rows[row2],
                                                             max_scalar);
              RowAddition<MaxPlusPlus<int>, std::array<int, dim>>()(
                  sum, scalar_prod);
            }
          }
          if (sum != rows[row]) {
            buf.push_back(rows[row]);
          }
        }
      }
      std::swap(buf, rows);
    }

    ////////////////////////////////////////////////////////////////////////
    // Test functions - BMat
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test_BMat000() {
      auto rg = ReportGuard(REPORT);
      {
        Mat m = Mat::make({{0, 1}, {0, 1}});
        REQUIRE_NOTHROW(validate(m));
        REQUIRE(m == Mat({{0, 1}, {0, 1}}));
        REQUIRE(!(m == Mat({{0, 0}, {0, 1}})));
        REQUIRE(m == Mat({{0, 1}, {0, 1}}));
        m.product_inplace(Mat({{0, 0}, {0, 0}}), Mat({{0, 0}, {0, 0}}));
        REQUIRE(m == Mat({{0, 0}, {0, 0}}));
        m.product_inplace(Mat({{0, 0}, {0, 0}}), Mat({{1, 1}, {1, 1}}));
        REQUIRE(m == Mat({{0, 0}, {0, 0}}));
        m.product_inplace(Mat({{1, 1}, {1, 1}}), Mat({{0, 0}, {0, 0}}));
        REQUIRE(m == Mat({{0, 0}, {0, 0}}));

        m.product_inplace(Mat({{0, 1}, {1, 0}}), Mat({{1, 0}, {1, 0}}));
        REQUIRE(m == Mat({{1, 0}, {1, 0}}));
        size_t const M = detail::BitSetCapacity<Mat>::value;
        detail::StaticVector1<BitSet<M>, M> result;
        matrix_helpers::bitset_rows(m, result);
        REQUIRE(result.size() == 2);
        REQUIRE(matrix_helpers::bitset_rows(m).size() == 2);
        result.clear();
        matrix_helpers::bitset_row_basis(m, result);
        REQUIRE(result.size() == 1);
        REQUIRE(matrix_helpers::bitset_row_basis(m).size() == 1);
      }

      {
        Mat m({{1, 1}, {0, 0}});
        using RowView = typename Mat::RowView;
        auto r        = matrix_helpers::rows(m);
        REQUIRE(std::vector<bool>(r[0].cbegin(), r[0].cend())
                == std::vector<bool>({true, true}));
        REQUIRE(std::vector<bool>(r[1].cbegin(), r[1].cend())
                == std::vector<bool>({false, false}));
        REQUIRE(r.size() == 2);
        std::sort(
            r.begin(), r.end(), [](RowView const& rv1, RowView const& rv2) {
              return std::lexicographical_compare(
                  rv1.begin(), rv1.end(), rv2.begin(), rv2.end());
            });
        REQUIRE(std::vector<bool>(r[0].cbegin(), r[0].cend())
                == std::vector<bool>({false, false}));
        REQUIRE(std::vector<bool>(r[1].cbegin(), r[1].cend())
                == std::vector<bool>({true, true}));
      }

      {
        using Row = typename Mat::Row;

        Mat A(2, 2);
        std::fill(A.begin(), A.end(), false);
        REQUIRE(A.number_of_rows() == 2);
        REQUIRE(A.number_of_cols() == 2);
        REQUIRE(A == Mat({{false, false}, {false, false}}));

        A(0, 0) = true;
        A(1, 1) = true;
        REQUIRE(A == Mat({{true, false}, {false, true}}));

        Mat B(2, 2);
        B(0, 1) = true;
        B(1, 0) = true;
        B(0, 0) = false;
        B(1, 1) = false;
        REQUIRE(B == Mat({{false, true}, {true, false}}));

        REQUIRE(A + B == Mat({{true, true}, {true, true}}));
        REQUIRE(A * B == B);
        REQUIRE(B * A == B);
        REQUIRE(B * B == A);
        REQUIRE((A + B) * B == Mat({{true, true}, {true, true}}));

        Row C({0, 1});
        REQUIRE(C.number_of_rows() == 1);
        REQUIRE(C.number_of_cols() == 2);

        auto rv = A.row(0);
        Row  D(rv);
        REQUIRE(D.number_of_rows() == 1);
        REQUIRE(D.number_of_cols() == 2);
        REQUIRE(D != C);
        auto views = matrix_helpers::rows(A);
        REQUIRE(B < A);
        B.swap(A);
        REQUIRE(A < B);
        std::swap(B, A);
        REQUIRE(B < A);
        REQUIRE(views[0] == Row({true, false}));
        REQUIRE(Row({true, false}) == views[0]);
        REQUIRE(Row({true, true}) != views[0]);
        REQUIRE(Row({false, false}) < views[0]);
        REQUIRE(A.hash_value() != 0);
        A *= false;
        REQUIRE(A == Mat({{false, false}, {false, false}}));
        auto r = Row({true, false});
        views  = matrix_helpers::rows(B);
        REQUIRE(views[0].size() == 2);
        r += views[0];
        REQUIRE(r.number_of_cols() == 2);
        REQUIRE(r.number_of_rows() == 1);
        REQUIRE(r == Row({true, true}));

        auto E = Mat::identity(2);
        REQUIRE(E.number_of_rows() == 2);
        REQUIRE(E.number_of_cols() == 2);
        auto viewse = matrix_helpers::rows(E);
        REQUIRE(viewse.size() == 2);

        std::ostringstream oss;
        oss << E;  // Does not do anything visible

        std::stringbuf buff;
        std::ostream   os(&buff);
        os << E;  // Also does not do anything visible
      }
      {
        Mat m({{0, 0}, {0, 0}});
        using scalar_type = typename Mat::scalar_type;
        auto it           = m.cbegin();
        REQUIRE(m.coords(it) == std::pair<scalar_type, scalar_type>({0, 0}));
        REQUIRE(m.coords(++it) == std::pair<scalar_type, scalar_type>({0, 1}));
        REQUIRE(m.coords(++it) == std::pair<scalar_type, scalar_type>({1, 0}));
        REQUIRE(m.coords(++it) == std::pair<scalar_type, scalar_type>({1, 1}));
      }
      {
        REQUIRE_THROWS_AS(Mat::make({{0, 0}, {0, 2}}), LibsemigroupsException);
      }
    }

    template <typename Mat>
    void test_BMat001() {
      auto x = Mat({{1, 0, 1}, {0, 1, 0}, {0, 1, 0}});
      auto y = Mat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
      auto z = Mat({{0, 0, 0}, {0, 0, 0}, {0, 0, 0}});
      REQUIRE(y == z);
      z.product_inplace(x, y);
      REQUIRE(y == z);
      z.product_inplace(y, x);
      REQUIRE(y == z);
      REQUIRE(!(y < z));
      auto id = x.identity();
      z.product_inplace(id, x);
      REQUIRE(z == x);
      z.product_inplace(x, id);
      REQUIRE(z == x);
      REQUIRE(x.hash_value() != 0);
    }

    template <typename Mat>
    void test_BMat002() {
      using RowView = typename Mat::RowView;
      auto x        = Mat::make({{1, 0, 0}, {1, 0, 0}, {1, 0, 0}});
      REQUIRE(matrix_helpers::row_basis(x).size() == 1);
      REQUIRE(matrix_helpers::row_space_size(x) == 1);
      x = Mat::make({{1, 0, 0}, {1, 1, 0}, {1, 1, 1}});
      REQUIRE(matrix_helpers::row_basis(x).size() == 3);
      REQUIRE_THROWS_AS(x.row(3), LibsemigroupsException);
      std::vector<RowView> v = {x.row(0), x.row(2)};
      REQUIRE(matrix_helpers::row_basis<Mat>(v).size() == 2);
      REQUIRE(matrix_helpers::row_space_size(x) == 3);
      x = Mat::make({{1, 0, 0}, {0, 1, 1}, {1, 1, 1}});
      REQUIRE(matrix_helpers::row_basis(x).size() == 2);
      REQUIRE(matrix_helpers::row_space_size(x) == 3);
      x = Mat::make({{1, 0, 0}, {0, 0, 1}, {0, 1, 0}});
      REQUIRE(matrix_helpers::row_space_size(x) == 7);
      std::vector<typename Mat::RowView> views;
      std::vector<typename Mat::RowView> result;
      matrix_helpers::row_basis<Mat, std::vector<typename Mat::RowView>&>(
          views, result);
    }

    ////////////////////////////////////////////////////////////////////////
    // Test functions - NTPMat
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test_NTPMat000(NTPSemiring<> const* sr = nullptr) {
      using Row = typename Mat::Row;
      auto rg   = ReportGuard(REPORT);
      Mat  m(sr, 3, 3);
      // REQUIRE(validate(m)); // m might not be valid!
      m.product_inplace(Mat::make(sr, {{1, 1, 0}, {0, 0, 1}, {1, 0, 1}}),
                        Mat::make(sr, {{1, 0, 1}, {0, 0, 1}, {1, 1, 0}}));
      REQUIRE(m == Mat::make(sr, {{1, 0, 2}, {1, 1, 0}, {2, 1, 1}}));
      REQUIRE(m.row(0) == Row(sr, {1, 0, 2}));
      REQUIRE(m.row(0).size() == 3);
      auto r = matrix_helpers::rows(m);
      REQUIRE(r[0] == Row(sr, {1, 0, 2}));
      REQUIRE(r[1] == Row(sr, {1, 1, 0}));
      REQUIRE(r[2] == Row(sr, {2, 1, 1}));
      REQUIRE(m * Mat::identity(sr, 3) == m);
      REQUIRE(Mat::identity(sr, 3) * m == m);
    }

    template <typename Mat>
    void test_NTPMat001(NTPSemiring<> const* sr = nullptr) {
      using Row         = typename Mat::Row;
      using RowView     = typename Mat::RowView;
      using scalar_type = typename Mat::scalar_type;

      auto rg = ReportGuard(REPORT);

      Mat m = Mat::make(
          sr, {{1, 1, 0, 0}, {2, 0, 2, 0}, {1, 2, 3, 9}, {0, 0, 0, 7}});
      REQUIRE(m.number_of_cols() == 4);
      REQUIRE(m.number_of_rows() == 4);
      auto r = matrix_helpers::rows(m);
      REQUIRE(r.size() == 4);
      REQUIRE(std::vector<scalar_type>(r[0].cbegin(), r[0].cend())
              == std::vector<scalar_type>({1, 1, 0, 0}));
      r[0] += r[1];
      REQUIRE(std::vector<scalar_type>(r[0].cbegin(), r[0].cend())
              == std::vector<scalar_type>({3, 1, 2, 0}));
      REQUIRE(std::vector<scalar_type>(r[1].cbegin(), r[1].cend())
              == std::vector<scalar_type>({2, 0, 2, 0}));
      REQUIRE(
          m
          == Mat::make(
              sr, {{3, 1, 2, 0}, {2, 0, 2, 0}, {1, 2, 3, 9}, {0, 0, 0, 7}}));
      REQUIRE(r[0][0] == 3);
      REQUIRE(r[0](0) == 3);
      REQUIRE(r[2](3) == 9);
      std::sort(r[0].begin(), r[0].end());
      REQUIRE(std::vector<scalar_type>(r[0].cbegin(), r[0].cend())
              == std::vector<scalar_type>({0, 1, 2, 3}));
      REQUIRE(
          m
          == Mat::make(
              sr, {{0, 1, 2, 3}, {2, 0, 2, 0}, {1, 2, 3, 9}, {0, 0, 0, 7}}));
      r[0] += 9;
      REQUIRE(std::vector<scalar_type>(r[0].cbegin(), r[0].cend())
              == std::vector<scalar_type>({9, 0, 1, 2}));
      REQUIRE(
          m
          == Mat::make(
              sr, {{9, 0, 1, 2}, {2, 0, 2, 0}, {1, 2, 3, 9}, {0, 0, 0, 7}}));
      r[1] *= 3;
      REQUIRE(
          m
          == Mat::make(
              sr, {{9, 0, 1, 2}, {6, 0, 6, 0}, {1, 2, 3, 9}, {0, 0, 0, 7}}));
      REQUIRE(std::vector<scalar_type>(r[1].cbegin(), r[1].cend())
              == std::vector<scalar_type>({6, 0, 6, 0}));
      REQUIRE(r[2] < r[1]);
      r[1] = r[2];
      REQUIRE(
          m
          == Mat::make(
              sr, {{9, 0, 1, 2}, {6, 0, 6, 0}, {1, 2, 3, 9}, {0, 0, 0, 7}}));
      REQUIRE(r[1] == r[2]);
      REQUIRE(r[1] == Row::make(sr, {{1, 2, 3, 9}}));

      RowView rv;
      {
        rv = r[0];
        REQUIRE(rv == r[0]);
        REQUIRE(&rv != &r[0]);
      }
    }

    template <typename Mat>
    void test_NTPMat002(NTPSemiring<> const* sr = nullptr) {
      using Row = typename Mat::Row;

      auto rg = ReportGuard(REPORT);
      Mat  m(sr, {{1, 1, 0, 0}, {2, 0, 2, 0}, {1, 2, 3, 9}, {0, 0, 0, 7}});
      REQUIRE(m.number_of_cols() == 4);
      REQUIRE(m.number_of_rows() == 4);
      auto r = matrix_helpers::rows(m);
      REQUIRE(r.size() == 4);
      REQUIRE(r[0] == Row::make(sr, {{1, 1, 0, 0}}));
      REQUIRE(r[1] == Row::make(sr, {{2, 0, 2, 0}}));
      REQUIRE(r[0] != Row::make(sr, {{2, 0, 2, 0}}));
      REQUIRE(r[1] != Row::make(sr, {{1, 1, 0, 0}}));
      REQUIRE(Row::make(sr, {{1, 1, 0, 0}}) == r[0]);
      REQUIRE(Row::make(sr, {{2, 0, 2, 0}}) == r[1]);
      REQUIRE(Row::make(sr, {{2, 0, 2, 0}}) != r[0]);
      REQUIRE(Row::make(sr, {{1, 1, 0, 0}}) != r[1]);
      REQUIRE(Row::make(sr, {{1, 1, 0, 0}}) < Row(sr, {{9, 9, 9, 9}}));
      REQUIRE(r[0] < Row::make(sr, {{9, 9, 9, 9}}));
      REQUIRE(!(Row::make(sr, {{9, 9, 9, 9}}) < r[0]));
      Row x(r[3]);
      x *= 3;
      REQUIRE(x == Row::make(sr, {{0, 0, 0, 1}}));
      REQUIRE(x.number_of_rows() == 1);
      REQUIRE(x.number_of_cols() == 4);
      REQUIRE(r[3] == Row::make(sr, {{0, 0, 0, 7}}));
      REQUIRE(r[3] != x);
      REQUIRE(x != r[3]);
      REQUIRE(!(x != x));
    }

    template <typename Mat>
    void test_NTPMat003(NTPSemiring<> const* sr = nullptr) {
      auto x        = Mat::make(sr, {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}});
      auto expected = Mat::make(sr, {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}});
      REQUIRE(x == expected);
      REQUIRE(x.number_of_cols() == 3);
      REQUIRE(x.number_of_rows() == 3);

      auto y = Mat::make(sr, {{10, 0, 0}, {0, 1, 0}, {1, 1, 0}});
      REQUIRE(!(x == y));

      y.product_inplace(x, x);
      expected = Mat::make(sr, {{34, 34, 0}, {34, 34, 0}, {33, 33, 1}});
      REQUIRE(y == expected);

      REQUIRE(x < y);
      auto id = x.identity();
      y.product_inplace(id, x);
      REQUIRE(y == x);
      y.product_inplace(x, id);
      REQUIRE(y == x);
      REQUIRE(Hash<Mat>()(y) != 0);
    }

    ////////////////////////////////////////////////////////////////////////
    // Test functions - MaxPlusTruncMat
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test_MaxPlusTruncMat000(MaxPlusTruncSemiring<> const* sr = nullptr) {
      using scalar_type = typename Mat::scalar_type;
      {
        Mat m1(sr, 2, 2);
        std::fill(m1.begin(), m1.end(), NEGATIVE_INFINITY);
        REQUIRE(m1
                == Mat::make(sr,
                             {{NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                              {NEGATIVE_INFINITY, NEGATIVE_INFINITY}}));
        Mat m2(sr, 2, 2);
        std::fill(m2.begin(), m2.end(), 4);
        REQUIRE(m1 + m2 == m2);
        REQUIRE(m2(0, 1) == 4);
      }

      auto rg = ReportGuard(REPORT);
      {
        std::vector<std::array<scalar_type, 2>> expected;
        expected.push_back({1, 1});
        expected.push_back({0, 0});
        tropical_max_plus_row_basis<2, 5>(expected);
        REQUIRE(expected.size() == 1);
        REQUIRE(expected.at(0) == std::array<scalar_type, 2>({0, 0}));

        Mat  m(sr, {{1, 1}, {0, 0}});
        auto r = matrix_helpers::row_basis(m);
        REQUIRE(r.size() == 1);
        REQUIRE(std::vector<scalar_type>(r[0].cbegin(), r[0].cend())
                == std::vector<scalar_type>({0, 0}));
      }
      {
        Mat m(sr, {{1, 1}, {0, 0}});
        m      = m.identity();
        auto r = matrix_helpers::row_basis(m);
        REQUIRE(r.size() == 2);
        REQUIRE(std::vector<scalar_type>(r[0].cbegin(), r[0].cend())
                == std::vector<scalar_type>({NEGATIVE_INFINITY, 0}));
        REQUIRE(std::vector<scalar_type>(r[1].cbegin(), r[1].cend())
                == std::vector<scalar_type>({0, NEGATIVE_INFINITY}));
      }
      std::vector<typename Mat::RowView> views;
      std::vector<typename Mat::RowView> result;
      matrix_helpers::row_basis<Mat>(views, result);
    }

    template <typename Mat>
    void test_MaxPlusTruncMat001(MaxPlusTruncSemiring<> const* sr = nullptr) {
      // Threshold 5, 4 x 4
      using scalar_type = typename Mat::scalar_type;
      using Row         = typename Mat::Row;

      auto m  = Mat::make(sr,
                         {{2, 2, 0, 1},
                          {0, 0, 1, 3},
                          {1, NEGATIVE_INFINITY, 0, 0},
                          {0, 1, 0, 1}});
      auto rg = ReportGuard(REPORT);
      auto r  = matrix_helpers::row_basis(m);
      REQUIRE(r.size() == 4);
      REQUIRE(r[0] == Row::make(sr, {0, 0, 1, 3}));
      REQUIRE(r[1] == Row::make(sr, {0, 1, 0, 1}));
      REQUIRE(r[2] == Row::make(sr, {1, NEGATIVE_INFINITY, 0, 0}));
      REQUIRE(r[3] == Row::make(sr, {2, 2, 0, 1}));
      m.transpose();
      REQUIRE(m
              == Mat::make(sr,
                           {{2, 0, 1, 0},
                            {2, 0, NEGATIVE_INFINITY, 1},
                            {0, 1, 0, 0},
                            {1, 3, 0, 1}}));
      m.transpose();
      REQUIRE(m
              == Mat::make(sr,
                           {{2, 2, 0, 1},
                            {0, 0, 1, 3},
                            {1, NEGATIVE_INFINITY, 0, 0},
                            {0, 1, 0, 1}}));

      std::vector<std::array<scalar_type, 4>> expected;
      expected.push_back({2, 2, 0, 1});
      expected.push_back({0, 0, 1, 3});
      expected.push_back({1, NEGATIVE_INFINITY, 0, 0});
      expected.push_back({0, 1, 0, 1});
      tropical_max_plus_row_basis<4, 5>(expected);
      REQUIRE(expected.size() == 4);
      REQUIRE(m * Mat::identity(sr, 4) == m);
      REQUIRE(Mat::identity(sr, 4) * m == m);
    }

    template <typename Mat>
    void test_MaxPlusTruncMat002(MaxPlusTruncSemiring<> const* sr = nullptr) {
      auto x        = Mat(sr, {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}});
      auto expected = Mat(sr, {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}});
      REQUIRE(x == expected);

      REQUIRE_THROWS_AS(Mat::make(sr, {{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}}),
                        LibsemigroupsException);
      auto y = Mat(sr, {{10, 0, 0}, {0, 1, 0}, {1, 1, 0}});
      REQUIRE(!(x == y));

      y.product_inplace(x, x);
      expected = Mat(sr, {{33, 33, 22}, {32, 32, 10}, {33, 33, 32}});
      REQUIRE(y == expected);

      REQUIRE(x < y);
      auto id = x.identity();
      y.product_inplace(id, x);
      REQUIRE(y == x);
      y.product_inplace(x, id);
      REQUIRE(y == x);
      REQUIRE(Hash<Mat>()(y) != 0);
      REQUIRE(x * Mat::identity(sr, 3) == x);
      REQUIRE(Mat::identity(sr, 3) * x == x);
    }

    ////////////////////////////////////////////////////////////////////////
    // Test functions - IntMat
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test_IntMat000() {
      {
        auto x        = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
        auto expected = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
        REQUIRE(x == expected);

        auto y = Mat({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}});
        REQUIRE(!(x == y));

        y.product_inplace(x, x);
        expected = Mat({{2, -4, 0}, {2, -2, 0}, {2, -1, 1}});
        REQUIRE(y == expected);
        REQUIRE(y.number_of_rows() == 3);

        REQUIRE(x < y);
        REQUIRE(Degree<Mat>()(x) == 3);
        REQUIRE(Degree<Mat>()(y) == 3);
        REQUIRE(Complexity<Mat>()(x) == 27);
        REQUIRE(Complexity<Mat>()(y) == 27);
        auto id = x.identity();
        y.product_inplace(id, x);
        REQUIRE(y == x);
        y.product_inplace(x, id);
        REQUIRE(y == x);
      }
      {
        auto x        = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
        auto expected = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
        REQUIRE(x == expected);

        auto y = Mat({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}});
        REQUIRE(!(x == y));

        y.product_inplace(x, x);
        expected = Mat({{2, -4, 0}, {2, -2, 0}, {2, -1, 1}});
        REQUIRE(y == expected);

        REQUIRE(x < y);
        auto id = x.identity();
        y.product_inplace(id, x);
        REQUIRE(y == x);
        y.product_inplace(x, id);
        REQUIRE(y == x);
        REQUIRE(Hash<Mat>()(y) != 0);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Test functions - MaxPlusMat
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test_MaxPlusMat000() {
      auto x        = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
      auto expected = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
      REQUIRE(x == expected);

      auto y = Mat{{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}};
      REQUIRE(!(x == y));
      REQUIRE(x != y);

      y.product_inplace(x, x);
      expected = Mat({{1, 2, 2}, {1, 1, 1}, {2, 3, 2}});
      REQUIRE(y == expected);

      REQUIRE(x < y);
      REQUIRE(Degree<Mat>()(x) == 3);
      REQUIRE(Degree<Mat>()(y) == 3);
      REQUIRE(Complexity<Mat>()(x) == 27);
      REQUIRE(Complexity<Mat>()(y) == 27);
      auto id = x.identity();
      y.product_inplace(id, x);
      REQUIRE(y == x);
      y.product_inplace(x, id);
      REQUIRE(y == x);
      REQUIRE(Hash<Mat>()(y) != 0);
    }

    ////////////////////////////////////////////////////////////////////////
    // Test functions - MinPlusMat
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test_MinPlusMat000() {
      {
        auto x        = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
        auto expected = Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
        // Just testing the below doesn't compile
        // matrix_helpers::row_basis(x);
        REQUIRE(x == expected);

        auto y = Mat({{-100, 0, 0}, {0, 1, 0}, {1, -1, 0}});
        REQUIRE(!(x == y));

        y.product_inplace(x, x);
        expected = Mat({{-4, -3, -2}, {-3, -3, -1}, {-4, -3, -3}});
        REQUIRE(y == expected);

        REQUIRE(!(x < y));
        REQUIRE(Degree<Mat>()(x) == 3);
        REQUIRE(Degree<Mat>()(y) == 3);
        REQUIRE(Complexity<Mat>()(x) == 27);
        REQUIRE(Complexity<Mat>()(y) == 27);
        auto id = x.identity();
        y.product_inplace(id, x);
        REQUIRE(y == x);
        y.product_inplace(x, id);
        REQUIRE(y == x);
      }
      {
        auto x        = Mat({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}});
        auto expected = Mat({{22, 21, 0}, {10, 0, 0}, {1, 32, 1}});
        REQUIRE(x == expected);

        auto y = Mat({{10, 0, 0}, {0, 1, 0}, {1, 1, 0}});
        REQUIRE(!(x == y));

        y.product_inplace(x, x);
        REQUIRE(y == Mat({{1, 21, 1}, {1, 0, 0}, {2, 22, 1}}));

        REQUIRE(x > y);
        REQUIRE(Degree<Mat>()(x) == 3);
        REQUIRE(Degree<Mat>()(y) == 3);
        REQUIRE(Complexity<Mat>()(x) == 27);
        REQUIRE(Complexity<Mat>()(y) == 27);
        auto id = x.identity();
        y.product_inplace(id, x);
        REQUIRE(y == x);
        y.product_inplace(x, id);
        REQUIRE(y == x);
        REQUIRE(Hash<Mat>()(y) != 0);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Test functions - MinPlusTruncMat
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test_MinPlusTruncMat000(MinPlusTruncSemiring<> const* sr = nullptr) {
      auto x = Mat(sr, {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}});

      auto expected = Mat::make(sr, {{22, 21, 0}, {10, 0, 0}, {1, 32, 1}});
      REQUIRE(x == expected);

      auto y = Mat(sr, {{10, 0, 0}, {0, 1, 0}, {1, 1, 0}});
      REQUIRE(!(x == y));

      y.product_inplace(x, x);
      expected = Mat(sr, {{1, 21, 1}, {1, 0, 0}, {2, 22, 1}});
      REQUIRE(y == expected);

      REQUIRE(!(x < y));
      REQUIRE(Degree<Mat>()(x) == 3);
      REQUIRE(Degree<Mat>()(y) == 3);
      REQUIRE(Complexity<Mat>()(x) == 27);
      REQUIRE(Complexity<Mat>()(y) == 27);
      auto id = x.identity();
      y.product_inplace(id, x);
      REQUIRE(y == x);
      y.product_inplace(x, id);
      REQUIRE(y == x);
      REQUIRE(Hash<Mat>()(y) != 0);
      REQUIRE_THROWS_AS(Mat::make(sr, {{-22, 21, 0}, {10, 0, 0}, {1, 32, 1}}),
                        LibsemigroupsException);
      REQUIRE(x * Mat::identity(sr, 3) == x);
      REQUIRE(Mat::identity(sr, 3) * x == x);
    }

    ////////////////////////////////////////////////////////////////////////
    // Test functions - ProjMaxPlusMat
    ////////////////////////////////////////////////////////////////////////

    template <typename Mat>
    void test_ProjMaxPlusMat000() {
      using Row     = typename Mat::Row;
      auto x        = Mat::make({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
      auto expected = Mat::make({{-4, 0, -2}, {-3, -2, -2}, {-1, -5, -1}});
      REQUIRE(x == expected);
      REQUIRE(x.zero() == NEGATIVE_INFINITY);
      REQUIRE(x.one() == 0);

      auto y = Mat::make({{NEGATIVE_INFINITY, 0, 0}, {0, 1, 0}, {1, -1, 0}});
      expected
          = Mat::make({{NEGATIVE_INFINITY, -1, -1}, {-1, 0, -1}, {0, -2, -1}});
      REQUIRE(y == expected);
      REQUIRE(!(x == y));

      y.product_inplace(x, x);
      expected = Mat({{-2, -1, -1}, {-2, -2, -2}, {-1, 0, -1}});
      REQUIRE(y == expected);

      REQUIRE(x < y);
      REQUIRE(y > x);
      REQUIRE(Degree<Mat>()(x) == 3);
      REQUIRE(Degree<Mat>()(y) == 3);
      REQUIRE(Complexity<Mat>()(x) == 27);
      REQUIRE(Complexity<Mat>()(y) == 27);
      auto id = x.identity();
      y.product_inplace(id, x);
      REQUIRE(y == x);
      y.product_inplace(x, id);
      REQUIRE(y == x);

      REQUIRE(Mat::make({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}).hash_value()
              != 0);

      y = x;
      REQUIRE(&x != &y);
      REQUIRE(x == y);
      REQUIRE(y == Mat::make(nullptr, {{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}));

      auto yy(y);
      REQUIRE(yy == y);

      std::ostringstream oss;
      oss << y;  // Does not do anything visible
      std::stringbuf buff;
      std::ostream   os(&buff);
      os << y;  // Also does not do anything visible

      REQUIRE(y.row(0) == Row::make({-4, 0, -2}));
      REQUIRE(y.row(1) == Row({-3, -2, -2}));
      REQUIRE(Row(y.row(0)) == y.row(0));

      auto zz(std::move(y));

      Mat tt;
      REQUIRE(tt != zz);
      REQUIRE(Mat::identity(3)
              == Mat({{0, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
                      {NEGATIVE_INFINITY, 0, NEGATIVE_INFINITY},
                      {NEGATIVE_INFINITY, NEGATIVE_INFINITY, 0}}));
      REQUIRE(zz(0, 0) == -4);
      REQUIRE(zz.number_of_cols() == 3);
      zz += zz;
      REQUIRE(zz == Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}));
      zz *= 2;
      REQUIRE(zz == Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}));
      REQUIRE(zz + x == Mat({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}}));
      REQUIRE(zz * x == Mat({{-2, -1, -1}, {-2, -2, -2}, {-1, 0, -1}}));
      REQUIRE(std::accumulate(zz.cbegin(), zz.cend(), 0) == -20);
      REQUIRE(std::accumulate(zz.begin(), zz.end(), 0) == -20);
      x.transpose();
      REQUIRE(x == Mat({{-4, -3, -1}, {0, -2, -5}, {-2, -2, -1}}));
      x.swap(zz);
      REQUIRE(zz == Mat({{-4, -3, -1}, {0, -2, -5}, {-2, -2, -1}}));
      REQUIRE(matrix_helpers::pow(x, 100)
              == Mat({{-1, 0, -1}, {-2, -1, -2}, {-1, 0, -1}}));
      REQUIRE_THROWS_AS(matrix_helpers::pow(x, -100), LibsemigroupsException);
      REQUIRE(matrix_helpers::pow(x, 1)
              == Mat({{-4, 0, -2}, {-3, -2, -2}, {-1, -5, -1}}));
      REQUIRE(matrix_helpers::pow(x, 0) == Mat::identity(3));
    }

  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // Test cases - BMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Matrix", "000", "BMat<2>", "[quick]") {
    test_BMat000<BMat<2>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "001", "BMat<>", "[quick]") {
    test_BMat000<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "002", "BMat<3> + BMat<>", "[quick]") {
    auto rg = ReportGuard(REPORT);
    {
      BMat<3> m;
      m.product_inplace(BMat<3>({{1, 1, 0}, {0, 0, 1}, {1, 0, 1}}),
                        BMat<3>({{1, 0, 1}, {0, 0, 1}, {1, 1, 0}}));
      REQUIRE(m == BMat<3>({{1, 0, 1}, {1, 1, 0}, {1, 1, 1}}));
    }
    {
      BMat<> m(3, 3);
      m.product_inplace(BMat<>({{1, 1, 0}, {0, 0, 1}, {1, 0, 1}}),
                        BMat<>({{1, 0, 1}, {0, 0, 1}, {1, 1, 0}}));
      REQUIRE(m == BMat<>({{1, 0, 1}, {1, 1, 0}, {1, 1, 1}}));
    }
    {
      BMat<>* A = new BMat<>();
      delete A;
      BMat<3>* B = new BMat<3>();
      delete B;
      BMat<3>::Row* C = new BMat<3>::Row();
      delete C;
      BMat<2>::Row* D = new BMat<2>::Row();
      delete D;
      BMat<2>* E = new BMat<2>();
      delete E;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "003", "BMat<2> + BMat<>", "[quick]") {
    BMat<2> AB;
    BMat<2> A;
    BMat<2> B;
    std::fill(A.begin(), A.end(), false);
    std::fill(B.begin(), B.end(), false);
    std::fill(AB.begin(), AB.end(), false);
    A(1, 1) = true;

    AB.product_inplace(A, B);
    REQUIRE(AB == B);

    REQUIRE(A.identity() == BMat<2>({{true, false}, {false, true}}));

    BMat<> CD(2, 2);
    BMat<> C(2, 2);
    BMat<> D(2, 2);

    std::fill(CD.begin(), CD.end(), false);
    std::fill(D.begin(), D.end(), false);
    C(1, 1) = true;
    CD.product_inplace(C, D);
    REQUIRE(CD == D);
    REQUIRE(D.identity() == BMat<>({{true, false}, {false, true}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "004", "BMat<3>", "[quick]") {
    test_BMat001<BMat<3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "005", "BMat<>", "[quick]") {
    test_BMat001<BMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "006", "BMat<3> row_basis", "[quick]") {
    test_BMat002<BMat<3>>();
  }
  LIBSEMIGROUPS_TEST_CASE("Matrix", "007", "BMat<> row_basis", "[quick]") {
    test_BMat002<BMat<>>();
  }

  ////////////////////////////////////////////////////////////////////////
  // Test cases - IntMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Matrix", "008", "IntMat<3>", "[quick]") {
    test_IntMat000<IntMat<3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "009", "IntMat<>", "[quick]") {
    test_IntMat000<IntMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "010", "IntMat code cov", "[quick]") {
    IntMat<>* A = new IntMat<>();
    delete A;
    IntMat<3>* B = new IntMat<3>();
    delete B;
    IntMat<3>::Row* C = new IntMat<3>::Row();
    delete C;
    IntMat<2>::Row* D = new IntMat<2>::Row();
    delete D;
    IntMat<2>* E = new IntMat<2>();
    delete E;
    IntMat<> F(3, 3);
    IntMat<> G(4, 4);
    std::swap(F, G);
    REQUIRE(G.number_of_cols() == 3);
    REQUIRE(G.number_of_rows() == 3);
    REQUIRE(F.number_of_cols() == 4);
    REQUIRE(F.number_of_rows() == 4);
  }

  ////////////////////////////////////////////////////////////////////////
  // Test cases - MaxPlusMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Matrix", "011", "MaxPlusMat<3>", "[quick]") {
    test_MaxPlusMat000<MaxPlusMat<3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "012", "MaxPlusMat<>", "[quick]") {
    test_MaxPlusMat000<MaxPlusMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "013", "MaxPlusMat code cov", "[quick]") {
    MaxPlusMat<3>* B = new MaxPlusMat<3>();
    delete B;
    MaxPlusMat<3>::Row* C = new MaxPlusMat<3>::Row();
    delete C;
  }

  ////////////////////////////////////////////////////////////////////////
  // Test cases - MinPlusMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Matrix", "014", "MinPlusMat<3>", "[quick]") {
    test_MinPlusMat000<MinPlusMat<3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "015", "MinPlusMat<>", "[quick]") {
    test_MinPlusMat000<MinPlusMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "016", "MinPlusMat code cov", "[quick]") {
    MinPlusMat<3>* B = new MinPlusMat<3>();
    delete B;
    MinPlusMat<3>::Row* C = new MinPlusMat<3>::Row();
    delete C;
  }

  ////////////////////////////////////////////////////////////////////////
  // Test cases - MaxPlusTruncMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Matrix", "017", "MaxPlusTruncMat<5, 2>", "[quick]") {
    // Threshold 5, 2 x 2
    test_MaxPlusTruncMat000<MaxPlusTruncMat<5, 2>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "018", "MaxPlusTruncMat<5>", "[quick]") {
    // Threshold 5, 2 x 2
    test_MaxPlusTruncMat000<MaxPlusTruncMat<5>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "019", "MaxPlusTruncMat<>", "[quick]") {
    // Threshold 5, 2 x 2 (specified in test_MaxPlusTruncMat000)
    REQUIRE_THROWS_AS(MaxPlusTruncSemiring<>(-1), LibsemigroupsException);
    MaxPlusTruncSemiring<> const* sr = new MaxPlusTruncSemiring<>(5);
    test_MaxPlusTruncMat000<MaxPlusTruncMat<>>(sr);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "020", "MaxPlusTruncMat<5, 4>", "[quick]") {
    // Threshold 5, 4 x 4
    test_MaxPlusTruncMat001<MaxPlusTruncMat<5, 4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "021", "MaxPlusTruncMat<5>", "[quick]") {
    // Threshold 5, 4 x 4
    test_MaxPlusTruncMat001<MaxPlusTruncMat<5>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "022", "MaxPlusTruncMat<>", "[quick]") {
    // Threshold 5, 4 x 4
    MaxPlusTruncSemiring<> const* sr = new MaxPlusTruncSemiring<>(5);
    test_MaxPlusTruncMat001<MaxPlusTruncMat<>>(sr);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix",
                          "023",
                          "MaxPlusTruncMat<33, 3>",
                          "[quick]") {
    // Threshold 33, 3 x 3
    test_MaxPlusTruncMat002<MaxPlusTruncMat<33, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "024", "MaxPlusTruncMat<33>", "[quick]") {
    // Threshold 33, 3 x 3
    test_MaxPlusTruncMat002<MaxPlusTruncMat<33>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "025", "MaxPlusTruncMat<>", "[quick]") {
    // Threshold 33, 3 x 3
    MaxPlusTruncSemiring<> const* sr = new MaxPlusTruncSemiring<>(33);
    test_MaxPlusTruncMat002<MaxPlusTruncMat<>>(sr);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "026", "MaxPlusMat code cov", "[quick]") {
    MaxPlusTruncMat<33, 3>* B = new MaxPlusTruncMat<33, 3>();
    delete B;
    MaxPlusTruncMat<5, 2>::Row* C = new MaxPlusTruncMat<5, 2>::Row();
    delete C;
    MaxPlusTruncMat<5, 4>::Row* D = new MaxPlusTruncMat<5, 4>::Row();
    delete D;
    auto x = MaxPlusTruncMat<5, 2>();
    REQUIRE(x.number_of_rows() == 2);
    MaxPlusTruncMat<5, 2>* E = new MaxPlusTruncMat<5, 2>();
    delete E;
    MaxPlusTruncMat<5, 4>* F = new MaxPlusTruncMat<5, 4>();
    delete F;
  }

  ////////////////////////////////////////////////////////////////////////
  // Test cases - MinPlusTruncMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Matrix",
                          "027",
                          "MinPlusTruncMat<33, 3>",
                          "[quick]") {
    // 3 x 3 matrices with threshold 33
    test_MinPlusTruncMat000<MinPlusTruncMat<33, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "028", "MinPlusTruncMat<33>", "[quick]") {
    // 3 x 3 matrices with threshold 33
    test_MinPlusTruncMat000<MinPlusTruncMat<33>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "029", "MinPlusTruncMat<>", "[quick]") {
    // 3 x 3 matrices with threshold 33
    REQUIRE_THROWS_AS(MinPlusTruncSemiring<>(-1), LibsemigroupsException);
    MinPlusTruncSemiring<> const* sr = new MinPlusTruncSemiring<>(33);
    test_MinPlusTruncMat000<MinPlusTruncMat<>>(sr);
    delete sr;
  }

  ////////////////////////////////////////////////////////////////////////
  // Test cases - NTPMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Matrix", "030", "NTPMat<0, 3, 3, 3>", "[quick]") {
    test_NTPMat000<NTPMat<0, 3, 3, 3>>();
    test_NTPMat000<NTPMat<0, 3, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "031", "NTPMat<0, 3>", "[quick]") {
    test_NTPMat000<NTPMat<0, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "032", "NTPMat<>", "[quick]") {
    REQUIRE_THROWS_AS(NTPSemiring<int>(4, -1), LibsemigroupsException);
    REQUIRE_THROWS_AS(NTPSemiring<int>(-1, -1), LibsemigroupsException);
    NTPSemiring<> const* sr = new NTPSemiring<>(0, 3);
    test_NTPMat000<NTPMat<>>(sr);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "033", "NTPMat<0, 10, 4, 4>", "[quick]") {
    test_NTPMat001<NTPMat<0, 10, 4, 4>>();
    test_NTPMat001<NTPMat<0, 10, 4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "034", "NTPMat<0, 10>", "[quick]") {
    test_NTPMat001<NTPMat<0, 10>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "035", "NTPMat<>", "[quick]") {
    NTPSemiring<> const* sr = new NTPSemiring<>(0, 10);
    test_NTPMat001<NTPMat<>>(sr);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "036", "NTPMat<0, 10, 4, 4>", "[quick]") {
    test_NTPMat002<NTPMat<0, 10, 4, 4>>();
    test_NTPMat002<NTPMat<0, 10, 4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "037", "NTPMat<0, 10>", "[quick]") {
    test_NTPMat002<NTPMat<0, 10>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "038", "NTPMat<>", "[quick]") {
    NTPSemiring<> const* sr = new NTPSemiring<>(0, 10);
    test_NTPMat002<NTPMat<>>(sr);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "039", "NTPMat<33, 2, 3, 3>", "[quick]") {
    test_NTPMat003<NTPMat<33, 2, 3, 3>>();
    test_NTPMat003<NTPMat<33, 2, 3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "040", "NTPMat<33, 2>", "[quick]") {
    test_NTPMat003<NTPMat<33, 2>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "041", "NTPMat<>", "[quick]") {
    NTPSemiring<> const* sr = new NTPSemiring<>(33, 2);
    test_NTPMat003<NTPMat<>>(sr);
    delete sr;
  }

  ////////////////////////////////////////////////////////////////////////
  // Test cases - ProjMaxPlusMat
  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Matrix", "042", "ProjMaxPlusMat<3>", "[quick]") {
    test_ProjMaxPlusMat000<ProjMaxPlusMat<3, 3>>();
    test_ProjMaxPlusMat000<ProjMaxPlusMat<3>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "043", "ProjMaxPlusMat<>", "[quick]") {
    test_ProjMaxPlusMat000<ProjMaxPlusMat<>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "044", "exceptions", "[quick][element]") {
    using Mat               = NTPMat<>;
    using scalar_type       = typename Mat::scalar_type;
    NTPSemiring<> const* sr = new NTPSemiring<>(23, 1);
    REQUIRE(sr->one() == 1);
    REQUIRE(sr->zero() == 0);
    REQUIRE_THROWS_AS(Mat::make(nullptr, {{0, 0}, {0, 0}}),
                      LibsemigroupsException);
    auto x = Mat::make(sr, std::vector<std::vector<scalar_type>>());
    REQUIRE(x.number_of_cols() == x.number_of_rows());
    REQUIRE(x.number_of_cols() == 0);

    REQUIRE_THROWS_AS(Mat::make(sr, {{2, 2, 0}, {0, 0, 140}, {1, 3, 1}}),
                      LibsemigroupsException);
    delete sr;
  }

  LIBSEMIGROUPS_TEST_CASE("Matrix", "045", "code coverage", "[quick]") {
    {
      BMat<> x(0, 0);
      x.transpose();
      REQUIRE(x == x);
      BMat<> y(2, 1);
      REQUIRE_THROWS_AS(matrix_helpers::pow(y, 2), LibsemigroupsException);
    }
    { REQUIRE_THROWS_AS(BMat<>::make({{0, 1}, {0}}), LibsemigroupsException); }
    {
      BMat<> y(2, 2);
      std::fill(y.begin(), y.end(), 0);
      auto        r   = matrix_helpers::rows(y);
      auto const& val = r[0](0);
      REQUIRE(!val);
      REQUIRE(r[0] + r[1] == r[0]);
      std::ostringstream oss;
      oss << r[0];  // Does not do anything visible
      std::stringbuf buff;
      std::ostream   os(&buff);
      os << r[0];  // Also does not do anything visible
    }
    {
      using Row = BMat<2>::Row;
      BMat<2> x;
      std::fill(x.begin(), x.end(), 1);
      auto r = matrix_helpers::rows(x);
      Row  y(nullptr, r[0]);
      REQUIRE(y == Row({1, 1}));
    }
    {
      std::vector<std::vector<int>> v = {{true, false}, {true, false}};
      BMat<>                        x(v);
      REQUIRE(x == BMat<>({{true, false}, {true, false}}));
    }
    { BMat<> x; }
    {
      using Mat               = NTPMat<>;
      NTPSemiring<> const* sr = new NTPSemiring<>(23, 1);
      auto                 x  = Mat(sr, {{0, 1}, {0, 2}});
      Mat                  y  = x;
      REQUIRE(x == y);
      REQUIRE(&x != &y);

      Mat z(std::move(x));
      REQUIRE(z == y);
      REQUIRE(&z != &y);
      z(1, 0) = 2;
      y.swap(z);
      REQUIRE(y(1, 0) == 2);
      REQUIRE(z(1, 0) == 0);
      delete sr;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("FastestBMat",
                          "046",
                          "check no throw",
                          "[quick][matrix]") {
    REQUIRE_NOTHROW(FastestBMat<3>({{0, 1}, {0, 1}}));
  }

}  // namespace libsemigroups
