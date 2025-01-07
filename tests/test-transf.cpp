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

#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t, int32_t, int64_t
#include <vector>   // for vector

#include "catch_amalgamated.hpp"  // for TEST_CASE
#include "test-main.hpp"          // LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_SIZEOF_VO...
#include "libsemigroups/transf.hpp"  // for Transf<>
#include "libsemigroups/types.hpp"   // for SmallestInteger, Smalle...

namespace libsemigroups {
  struct LibsemigroupsException;

  namespace {
    template <typename T>
    void test_transf000() {
      static_assert(IsTransf<T>, "IsTransf<T> must be true!");
      auto x = T({0, 1, 0});
      auto y = T({0, 1, 0});
      REQUIRE(x == y);
      REQUIRE(y * y == x);
      REQUIRE((x < y) == false);

      auto z = T({0, 1, 0, 3});
      REQUIRE(x < z);
      REQUIRE(image(z) == std::vector<typename T::point_type>({0, 1, 3}));

      auto expected = T({0, 0, 0});
      REQUIRE(expected < x);

      REQUIRE(z.degree() == 4);
      REQUIRE(Complexity<T>()(z) == 4);
      REQUIRE(z.rank() == 3);
      auto id = one(z);

      expected = T({0, 1, 2, 3});
      REQUIRE(id == expected);

      if (IsDynamic<T>) {
        x.increase_degree_by(10);
        REQUIRE(x.degree() == 13);
        REQUIRE(x.end() - x.begin() == 13);
      } else {
        REQUIRE_THROWS_AS(x.increase_degree_by(10), LibsemigroupsException);
      }
      auto t = to_transf({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
      REQUIRE(t.hash_value() != 0);
      REQUIRE_NOTHROW(t.undef());
    }

    template <typename T>
    void test_pperm001() {
      static_assert(IsPPerm<T>, "IsPPerm<T> must be true!");
      auto x = T({4, 5, 0}, {9, 0, 1}, 10);
      auto y = T({4, 5, 0}, {9, 0, 1}, 10);
      REQUIRE(x.undef() == UNDEFINED);
      REQUIRE(x == y);
      auto yy = x * x;
      REQUIRE(yy[0] == UNDEFINED);
      REQUIRE(yy[1] == UNDEFINED);
      REQUIRE(yy.at(2) == UNDEFINED);
      REQUIRE(yy.at(3) == UNDEFINED);
      REQUIRE(yy.at(4) == UNDEFINED);
      REQUIRE(yy.at(5) == 1);

      REQUIRE(yy > y);
      REQUIRE(!(x < x));
      auto expected = T({UNDEFINED, UNDEFINED, UNDEFINED});
      REQUIRE(expected > x);

      REQUIRE(x.degree() == 10);
      REQUIRE(y.degree() == 10);
      REQUIRE(Complexity<T>()(x) == 10);
      REQUIRE(Complexity<T>()(y) == 10);
      REQUIRE(yy.rank() == 1);
      REQUIRE(y.rank() == 3);
      REQUIRE(x.rank() == 3);

      auto id  = one(x);
      expected = T({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
      REQUIRE(id == expected);

      if (IsDynamic<T>) {
        x.increase_degree_by(10);
        REQUIRE(x.degree() == 20);
        REQUIRE(x.end() >= x.begin());
        REQUIRE(static_cast<size_t>(x.end() - x.begin()) == x.degree());
      } else {
        REQUIRE_THROWS_AS(x.increase_degree_by(10), LibsemigroupsException);
      }
      REQUIRE(x.hash_value() != 0);
    }
    template <typename T>
    bool test_inverse(T const& p) {
      return p * inverse(p) == one(p) && inverse(p) * p == one(p);
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Transf",
                          "000",
                          "comparison operators",
                          "[quick][transf]") {
    auto x = Transf<>({0, 1, 0});
    auto y = Transf<>({0, 1});
    REQUIRE(x > y);
    // Can't compare static transf of different degrees
    // REQUIRE(to_string(x, "{}") == "Transf<0, uint32_t>({0, 1, 0})");
  }

  LIBSEMIGROUPS_TEST_CASE("Transf", "001", "mem fns", "[quick][transf]") {
    test_transf000<Transf<>>();
    test_transf000<Transf<4>>();
  }

  LIBSEMIGROUPS_TEST_CASE("Transf",
                          "002",
                          "exceptions (dynamic)",
                          "[quick][transf]") {
    using point_type = typename Transf<>::point_type;
    REQUIRE_NOTHROW(to_transf());
    REQUIRE_NOTHROW(to_transf({0}));
    REQUIRE_THROWS_AS(to_transf({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(to_transf({0, 1, 2}));
    REQUIRE_NOTHROW(to_transf(std::initializer_list<point_type>({0, 1, 2})));
    REQUIRE_NOTHROW(to_transf({0, 1, 2}));

    REQUIRE_THROWS_AS(to_transf({1, 2, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(to_transf(std::initializer_list<point_type>({1, 2, 3})),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(to_transf(std::initializer_list<point_type>(
                          {UNDEFINED, UNDEFINED, UNDEFINED})),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Transf",
                          "003",
                          "exceptions (static)",
                          "[quick][transf]") {
    REQUIRE_NOTHROW(to_transf({0}));
    REQUIRE_THROWS_AS(to_transf({1}), LibsemigroupsException);
    REQUIRE_THROWS_AS(to_transf({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(to_transf({0, 1, 2}));

    REQUIRE_THROWS_AS(to_transf({1, 2, 3}), LibsemigroupsException);

    REQUIRE_THROWS_AS(to_transf({UNDEFINED, UNDEFINED, UNDEFINED}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("PPerm", "004", "mem fns", "[quick][pperm]") {
    test_pperm001<PPerm<>>();
    test_pperm001<PPerm<10>>();
  }

  LIBSEMIGROUPS_TEST_CASE("PPerm",
                          "005",
                          "exceptions (dynamic)",
                          "[quick][pperm]") {
    using point_type = typename Transf<>::point_type;
    REQUIRE_NOTHROW(PPerm<>::make());
    REQUIRE_NOTHROW(PPerm<>::make({0}));
    REQUIRE_NOTHROW(PPerm<>::make({UNDEFINED}));
    REQUIRE_THROWS_AS(PPerm<>::make({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(PPerm<>::make({0, 1, 2}));
    REQUIRE_NOTHROW(
        PPerm<>::make(std::initializer_list<point_type>({0, 1, 2})));
    REQUIRE_NOTHROW(PPerm<>::make({0, UNDEFINED, 2}));
    REQUIRE_NOTHROW(PPerm<>::make({0, UNDEFINED, 5, UNDEFINED, UNDEFINED, 1}));

    REQUIRE_THROWS_AS(PPerm<>::make({1, 2, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({UNDEFINED, UNDEFINED, 3}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({1, UNDEFINED, 1}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make(std::vector<point_type>(
                          {3, UNDEFINED, 2, 1, UNDEFINED, 3})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        PPerm<>::make(std::initializer_list<point_type>({1, 2, 3})),
        LibsemigroupsException);
    REQUIRE_NOTHROW(PPerm<>::make({1, 2}, {0, 3}, 5));
    REQUIRE_NOTHROW(PPerm<>::make({1, 2}, {0, 5}, 6));
    REQUIRE_THROWS_AS(PPerm<>::make({1, 2}, {0}, 5), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({1, 2}, {0, 5}, 4), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({1, 5}, {0, 2}, 4), LibsemigroupsException);

    REQUIRE_THROWS_AS(PPerm<>::make({1, 1}, {0, 2}, 3), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({1, 0}, {2, 2}, 3), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({1, 0, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({1, 0, 3, 6, 4}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<>::make({1, 5, 0, 3, 2}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("PPerm",
                          "006",
                          "exceptions (static)",
                          "[quick][pperm]") {
    using point_type = typename PPerm<6>::point_type;
    REQUIRE_NOTHROW(PPerm<1>::make({0}));
    REQUIRE_NOTHROW(PPerm<1>::make({UNDEFINED}));
    REQUIRE_THROWS_AS(PPerm<1>::make({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(PPerm<3>::make({0, 1, 2}));
    REQUIRE_NOTHROW(PPerm<3>::make({0, 1, 2}));
    REQUIRE_NOTHROW(PPerm<3>::make({0, UNDEFINED, 2}));
    REQUIRE_NOTHROW(PPerm<6>::make({0, UNDEFINED, 5, UNDEFINED, UNDEFINED, 1}));

    REQUIRE_THROWS_AS(PPerm<3>::make({1, 2, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<3>::make({UNDEFINED, UNDEFINED, 3}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<3>::make({1, UNDEFINED, 1}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<6>::make({3, UNDEFINED, 2, 1, UNDEFINED, 3}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<3>::make({1, 2, 3}), LibsemigroupsException);
    REQUIRE_NOTHROW(PPerm<5>::make({1, 2}, {0, 3}, 5));
    REQUIRE_NOTHROW(PPerm<6>::make({1, 2}, {0, 5}, 6));
    REQUIRE_THROWS_AS(PPerm<5>::make({1, 2}, {0}, 5), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<4>::make({1, 2}, {0, 5}, 4),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<4>::make({1, 5}, {0, 2}, 4),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(PPerm<3>::make({1, 1}, {0, 2}, 3),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<3>::make({0, 2}, {1, 1}, 3),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(PPerm<1>::make({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<2>::make({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<3>::make({1, 0, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<5>::make({1, 0, 3, 6, 4}), LibsemigroupsException);
    REQUIRE_THROWS_AS(PPerm<5>::make({1, 5, 0, 3, 2}), LibsemigroupsException);
    auto x = PPerm<5>::make({0, 2}, {3, 0}, 5);
    REQUIRE(image(x) == std::vector<point_type>({0, 3}));
    REQUIRE(domain(x) == std::vector<point_type>({0, 2}));
  }

  LIBSEMIGROUPS_TEST_CASE("Perm", "007", "inverse", "[quick][perm]") {
    // TODO(now) check if Issue #87 is resolved?
    REQUIRE(test_inverse(Perm<>({})));
    REQUIRE(test_inverse(Perm<>({0})));
    REQUIRE(test_inverse(Perm<>({1, 0})));
    REQUIRE(test_inverse(Perm<>({0, 1})));
    REQUIRE(test_inverse(Perm<>({2, 0, 1, 4, 3})));
    REQUIRE(test_inverse(Perm<>({4, 2, 0, 1, 3})));
    REQUIRE(test_inverse(Perm<>({0, 1, 2, 3, 4})));

    REQUIRE(test_inverse(Perm<2>({1, 0})));
    REQUIRE(test_inverse(Perm<2>({0, 1})));
    REQUIRE(test_inverse(Perm<5>({2, 0, 1, 4, 3})));
    REQUIRE(test_inverse(Perm<5>({4, 2, 0, 1, 3})));
    REQUIRE(test_inverse(Perm<5>({0, 1, 2, 3, 4})));
  }

  LIBSEMIGROUPS_TEST_CASE("Perm",
                          "008",
                          "exceptions (dynamic)",
                          "[quick][perm]") {
    REQUIRE_NOTHROW(Perm<>::make({}));
    REQUIRE_NOTHROW(Perm<>::make({0}));
    REQUIRE_NOTHROW(Perm<>::make({0, 1}));
    REQUIRE_NOTHROW(Perm<>::make({1, 0}));
    REQUIRE_NOTHROW(Perm<>::make({1, 4, 0, 3, 2}));

    REQUIRE_THROWS_AS(Perm<>::make({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<>::make({1, 0, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<>::make({1, 0, 3, 6, 4}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<>::make({1, 5, 0, 3, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<>::make({0, 1, 2, 3, 0}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Perm",
                          "009",
                          "exceptions (static)",
                          "[quick][perm]") {
    REQUIRE_NOTHROW(Perm<1>::make({0}));
    REQUIRE_NOTHROW(Perm<2>::make({0, 1}));
    REQUIRE_NOTHROW(Perm<2>::make({1, 0}));
    REQUIRE_NOTHROW(Perm<5>::make({1, 4, 0, 3, 2}));

    REQUIRE_THROWS_AS(Perm<1>::make({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<2>::make({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<3>::make({1, 0, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<5>::make({1, 0, 3, 6, 4}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<5>::make({1, 5, 0, 3, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(Perm<5>::make({0, 1, 2, 3, 0}), LibsemigroupsException);

    REQUIRE_NOTHROW(Perm<5>::make());
  }

  LIBSEMIGROUPS_TEST_CASE("LeastTransf etc",
                          "010",
                          "No throw",
                          "[quick][transf][pperm][perm]") {
    REQUIRE_NOTHROW(LeastTransf<3>({0, 1, 2}));
    REQUIRE_NOTHROW(LeastPPerm<3>({0, 1, 2}));
    REQUIRE_NOTHROW(LeastPerm<3>({0, 1, 2}));
  }
}  // namespace libsemigroups
