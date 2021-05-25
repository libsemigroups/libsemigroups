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

#include <array>    // for array
#include <cstddef>  // for size_t
#include <cstdint>  // for int64_t
#include <vector>   // for vector

#include "catch.hpp"      // for  REQUIRE
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/matrix.hpp"        // for Matrix

namespace libsemigroups {
  // Forward declaration
  struct LibsemigroupsException;

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
  void
  tropical_max_plus_row_basis(std::vector<std::array<int64_t, dim>>& rows) {
    static thread_local std::vector<std::array<int64_t, dim>> buf;
    buf.clear();
    std::sort(rows.begin(), rows.end());
    for (size_t row = 0; row < rows.size(); ++row) {
      std::array<int64_t, dim> sum;
      sum.fill(NEGATIVE_INFINITY);
      if (row == 0 || rows[row] != rows[row - 1]) {
        for (size_t row2 = 0; row2 < row; ++row2) {
          int64_t max_scalar = thresh;
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
                = scalar_row_product<MaxPlusTruncProd<thresh, int64_t>,
                                     std::array<int64_t, dim>>(rows[row2],
                                                               max_scalar);
            RowAddition<MaxPlusPlus<int64_t>, std::array<int64_t, dim>>()(
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

  constexpr bool REPORT = false;

  LIBSEMIGROUPS_TEST_CASE("FroidurePin",
                          "055",
                          "(tropical max-plus semiring matrices)",
                          "[quick][froidure-pin][tropmaxplus]") {
    auto rg = ReportGuard(REPORT);
    // threshold 9, 2 x 2
    using Mat = MaxPlusTruncMat<9, 2>;
    using Row = typename Mat::Row;

    FroidurePin<Mat> S;
    S.add_generator(Mat({{1, 3}, {2, 1}}));
    S.add_generator(Mat({{2, 1}, {4, 0}}));

    REQUIRE(S.size() == 20);
    REQUIRE(S.number_of_idempotents() == 1);
    size_t pos = 0;

    for (auto it = S.cbegin(); it < S.cend(); ++it) {
      REQUIRE(S.position(*it) == pos);
      pos++;
    }
    S.add_generator(Mat({{1, 1}, {0, 2}}));
    REQUIRE(S.size() == 73);
    S.closure({Mat({{1, 1}, {0, 2}})});
    REQUIRE(S.size() == 73);
    REQUIRE(
        S.minimal_factorisation(Mat({{1, 1}, {0, 2}}) * Mat({{2, 1}, {4, 0}}))
        == word_type({2, 1}));
    REQUIRE(S.minimal_factorisation(52) == word_type({0, 2, 2, 1}));
    REQUIRE(S.at(52) == Mat({{9, 7}, {9, 5}}));
    REQUIRE_THROWS_AS(S.minimal_factorisation(1000000000),
                      LibsemigroupsException);
    pos = 0;
    for (auto it = S.cbegin_idempotents(); it < S.cend_idempotents(); ++it) {
      REQUIRE(*it * *it == *it);
      pos++;
    }
    REQUIRE(pos == S.number_of_idempotents());
    for (auto it = S.cbegin_sorted() + 1; it < S.cend_sorted(); ++it) {
      REQUIRE(*(it - 1) < *it);
    }
    {
      auto const& x  = S[4];
      auto        rb = matrix_helpers::row_basis(x);
      REQUIRE(rb.size() == 1);
      REQUIRE(rb[0] == Row({3, 5}));
      REQUIRE(x.row(0) == Row({3, 5}));
      REQUIRE(x.row(1) == Row({5, 7}));

      std::vector<std::array<int64_t, 2>> expected;
      for (auto& r : matrix_helpers::rows(x)) {
        std::array<int64_t, 2> rr;
        std::copy(r.cbegin(), r.cend(), rr.begin());
        expected.push_back(rr);
      }
      REQUIRE(std::array<int64_t, 2>({3, 5}) == expected.at(0));
      REQUIRE(std::array<int64_t, 2>({5, 7}) == expected.at(1));
      tropical_max_plus_row_basis<2, 9>(expected);
      REQUIRE(expected.size() == rb.size());
    }

    for (auto const& x : S) {
      auto                                rb = matrix_helpers::row_basis(x);
      std::vector<std::array<int64_t, 2>> expected;
      for (auto& r : matrix_helpers::rows(x)) {
        std::array<int64_t, 2> rr;
        std::copy(r.cbegin(), r.cend(), rr.begin());
        expected.push_back(rr);
      }
      tropical_max_plus_row_basis<2, 9>(expected);
      REQUIRE(expected.size() == rb.size());
      REQUIRE(std::equal(rb.cbegin(), rb.cend(), expected.cbegin()));
    }
  }
}  // namespace libsemigroups
