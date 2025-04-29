//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell
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

// Some warnings are issues by g++-14 for Perm<1> and PPerm<1>. This makes
// sense for some of them (where we are testing explicitly for too large
// containers) but not others where the container has the correct size.
//
// It would probably be better to put this diagnostic push/pop around the
// particular tests, but that doesn't suppress the warnings.
#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t, int32_t, int64_t
#include <vector>   // for vector

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for TEST_CASE
#include "test-main.hpp"                       // LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_SIZEOF_VO...
#include "libsemigroups/transf.hpp"  // for Transf<>
#include "libsemigroups/types.hpp"   // for SmallestInteger, Smalle...

#pragma GCC diagnostic pop

namespace libsemigroups {
  struct LibsemigroupsException;

  namespace {
    template <typename T>
    bool test_inverse(T const& p) {
      return p * inverse(p) == one(p) && inverse(p) * p == one(p);
    }
  }  // namespace

  LIBSEMIGROUPS_TEST_CASE("Transf",
                          "000",
                          "comparison operators",
                          "[quick][transf]") {
    auto x = make<Transf<>>({0, 1, 0});
    auto y = make<Transf<>>({0, 1});
    REQUIRE(x > y);
    // Can't compare static transf of different degrees
    // REQUIRE(to_string(x, "{}") == "Transf<0, uint32_t>({0, 1, 0})");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Transf",
                                   "001",
                                   "mem fns",
                                   "[quick][transf]",
                                   Transf<>,
                                   Transf<4>) {
    static_assert(IsTransf<TestType>, "IsTransf<TestType> must be true!");
    auto x = make<TestType>({0, 1, 0, 0});
    auto y = make<TestType>({0, 1, 0, 0});
    REQUIRE(x == y);
    REQUIRE(y * y == x);
    REQUIRE((x < y) == false);

    auto z = make<TestType>({0, 1, 0, 3});
    REQUIRE(x < z);
    REQUIRE(image(z) == std::vector<typename TestType::point_type>({0, 1, 3}));

    auto expected = make<TestType>({0, 0, 0, 0});
    REQUIRE(expected < x);

    REQUIRE(z.degree() == 4);
    REQUIRE(Complexity<TestType>()(z) == 4);
    REQUIRE(z.rank() == 3);
    auto id = one(z);

    expected = make<TestType>({0, 1, 2, 3});
    REQUIRE(id == expected);

    if (IsDynamic<TestType>) {
      x.increase_degree_by(10);
      REQUIRE(x.degree() == 14);
      REQUIRE(x.end() - x.begin() == 14);
    } else {
      REQUIRE_THROWS_AS(x.increase_degree_by(10), LibsemigroupsException);
    }
    auto t = make<Transf<>>({9, 7, 3, 5, 3, 4, 2, 7, 7, 1});
    REQUIRE(t.hash_value() != 0);
    REQUIRE_NOTHROW(t.undef());
  }

  LIBSEMIGROUPS_TEST_CASE("Transf",
                          "002",
                          "exceptions (dynamic)",
                          "[quick][transf]") {
    using point_type = typename Transf<>::point_type;
    REQUIRE_NOTHROW(Transf());
    REQUIRE_NOTHROW(make<Transf<>>({0}));
    REQUIRE_THROWS_AS(make<Transf<>>({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(make<Transf<>>({0, 1, 2}));
    REQUIRE_NOTHROW(
        make<Transf<>>(std::initializer_list<point_type>({0, 1, 2})));
    REQUIRE_NOTHROW(make<Transf<>>({0, 1, 2}));

    REQUIRE_THROWS_AS(make<Transf<>>({1, 2, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(
        make<Transf<>>(std::initializer_list<point_type>({1, 2, 3})),
        LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Transf<>>(std::initializer_list<point_type>(
                          {UNDEFINED, UNDEFINED, UNDEFINED})),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Transf",
                          "003",
                          "exceptions (static)",
                          "[quick][transf]") {
    REQUIRE_NOTHROW(make<Transf<>>({0}));
    REQUIRE_THROWS_AS(make<Transf<>>({1}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Transf<>>({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(make<Transf<>>({0, 1, 2}));

    REQUIRE_THROWS_AS(make<Transf<>>({1, 2, 3}), LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Transf<>>({UNDEFINED, UNDEFINED, UNDEFINED}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("PPerm",
                                   "004",
                                   "mem fns",
                                   "[quick][pperm]",
                                   PPerm<>,
                                   PPerm<10>) {
    static_assert(IsPPerm<TestType>, "IsPPerm<TestType> must be true!");
    auto x = make<TestType>({4, 5, 0}, {9, 0, 1}, 10);
    auto y = make<TestType>({4, 5, 0}, {9, 0, 1}, 10);
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
    auto expected = TestType({UNDEFINED, UNDEFINED, UNDEFINED});
    REQUIRE(expected > x);

    REQUIRE(x.degree() == 10);
    REQUIRE(y.degree() == 10);
    REQUIRE(Complexity<TestType>()(x) == 10);
    REQUIRE(Complexity<TestType>()(y) == 10);
    REQUIRE(yy.rank() == 1);
    REQUIRE(y.rank() == 3);
    REQUIRE(x.rank() == 3);

    auto id  = one(x);
    expected = TestType({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    REQUIRE(id == expected);

    if (IsDynamic<TestType>) {
      x.increase_degree_by(10);
      REQUIRE(x.degree() == 20);
      REQUIRE(x.end() >= x.begin());
      REQUIRE(static_cast<size_t>(x.end() - x.begin()) == x.degree());
    } else {
      REQUIRE_THROWS_AS(x.increase_degree_by(10), LibsemigroupsException);
    }
    REQUIRE(x.hash_value() != 0);
  }

  LIBSEMIGROUPS_TEST_CASE("PPerm",
                          "005",
                          "exceptions (dynamic)",
                          "[quick][pperm]") {
    using point_type = typename Transf<>::point_type;

    auto x = make<PPerm<0, uint8_t>>({}, {}, 257);
    REQUIRE(x * x == x);
    x = make<PPerm<0, uint8_t>>({}, {}, 256);
    REQUIRE(x * x == x);

    REQUIRE_THROWS_AS((make<PPerm<0, uint8_t>>({255}, {255}, 256)),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS((make<PPerm<0, uint8_t>>({0}, {255}, 256)),
                      LibsemigroupsException);

    REQUIRE_NOTHROW(PPerm<>());
    REQUIRE_NOTHROW(make<PPerm<>>({0}));
    REQUIRE_NOTHROW(make<PPerm<>>({UNDEFINED}));
    REQUIRE_THROWS_AS(make<PPerm<>>({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(make<PPerm<>>({0, 1, 2}));
    REQUIRE_NOTHROW(
        make<PPerm<>>(std::initializer_list<point_type>({0, 1, 2})));
    REQUIRE_NOTHROW(make<PPerm<>>({0, UNDEFINED, 2}));
    REQUIRE_NOTHROW(make<PPerm<>>({0, UNDEFINED, 5, UNDEFINED, UNDEFINED, 1}));

    REQUIRE_THROWS_AS(make<PPerm<>>({1, 2, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({UNDEFINED, UNDEFINED, 3}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({1, UNDEFINED, 1}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>(std::vector<point_type>(
                          {3, UNDEFINED, 2, 1, UNDEFINED, 3})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(
        make<PPerm<>>(std::initializer_list<point_type>({1, 2, 3})),
        LibsemigroupsException);
    REQUIRE_NOTHROW(make<PPerm<>>(std::initializer_list<uint32_t>({1, 2}),
                                  std::initializer_list<uint32_t>({0, 3}),
                                  5));
    REQUIRE_NOTHROW(make<PPerm<5, uint32_t>>({1, 2}, {0, 3}, 5));
    REQUIRE_NOTHROW(PPerm<5, uint32_t>({1, 2}, {0, 3}, 5));
    REQUIRE_NOTHROW(PPerm<>({1, 2}, {0, 3}, 5));
    REQUIRE_NOTHROW(make<PPerm<>>({1, 2}, {0, 5}, 6));
    REQUIRE_THROWS_AS(make<PPerm<>>({1, 2}, {0}, 5), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({1, 2}, {0, 5}, 4), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({1, 5}, {0, 2}, 4), LibsemigroupsException);

    REQUIRE_THROWS_AS(make<PPerm<>>({1, 1}, {0, 2}, 3), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({1, 0}, {2, 2}, 3), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({1, 0, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({1, 0, 3, 6, 4}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<>>({1, 5, 0, 3, 2}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("PPerm",
                          "006",
                          "exceptions (static)",
                          "[quick][pperm]") {
    using point_type = typename PPerm<6>::point_type;
    REQUIRE_NOTHROW(make<PPerm<1>>({0}));
    REQUIRE_NOTHROW(make<PPerm<1>>({UNDEFINED}));
    REQUIRE_THROWS_AS(make<PPerm<1>>({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(make<PPerm<3>>({0, 1, 2}));
    REQUIRE_NOTHROW(make<PPerm<3>>({0, 1, 2}));
    REQUIRE_NOTHROW(make<PPerm<3>>({0, UNDEFINED, 2}));
    REQUIRE_NOTHROW(make<PPerm<6>>({0, UNDEFINED, 5, UNDEFINED, UNDEFINED, 1}));

    REQUIRE_THROWS_AS(make<PPerm<3>>({1, 2, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<3>>({UNDEFINED, UNDEFINED, 3}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<3>>({1, UNDEFINED, 1}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<6>>({3, UNDEFINED, 2, 1, UNDEFINED, 3}),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<3>>({1, 2, 3}), LibsemigroupsException);
    REQUIRE_NOTHROW(make<PPerm<5>>({1, 2}, {0, 3}, 5));
    REQUIRE_NOTHROW(make<PPerm<6>>({1, 2}, {0, 5}, 6));
    REQUIRE_THROWS_AS(make<PPerm<5>>({1, 2}, {0}, 5), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<4>>({1, 2}, {0, 5}, 4),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<4>>({1, 5}, {0, 2}, 4),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(make<PPerm<3>>({1, 1}, {0, 2}, 3),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<3>>({0, 2}, {1, 1}, 3),
                      LibsemigroupsException);

    REQUIRE_THROWS_AS(make<PPerm<1>>({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<2>>({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<3>>({1, 0, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<5>>({1, 0, 3, 6, 4}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<PPerm<5>>({1, 5, 0, 3, 2}), LibsemigroupsException);
    auto x = make<PPerm<5>>({0, 2}, {3, 0}, 5);
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
    REQUIRE_NOTHROW(make<Perm<>>({}));
    REQUIRE_NOTHROW(make<Perm<>>({0}));
    REQUIRE_NOTHROW(make<Perm<>>({0, 1}));
    REQUIRE_NOTHROW(make<Perm<>>({1, 0}));
    REQUIRE_NOTHROW(make<Perm<>>({1, 4, 0, 3, 2}));

    REQUIRE_THROWS_AS(make<Perm<>>({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<>>({1, 0, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<>>({1, 0, 3, 6, 4}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<>>({1, 5, 0, 3, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<>>({0, 1, 2, 3, 0}), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Perm",
                          "009",
                          "exceptions (static)",
                          "[quick][perm]") {
    REQUIRE_NOTHROW(make<Perm<1>>({0}));
    REQUIRE_NOTHROW(make<Perm<2>>({0, 1}));
    REQUIRE_NOTHROW(make<Perm<2>>({1, 0}));
    REQUIRE_NOTHROW(make<Perm<5>>({1, 4, 0, 3, 2}));

    REQUIRE_THROWS_AS(make<Perm<1>>({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<2>>({1, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<3>>({1, 0, 3}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<5>>({1, 0, 3, 6, 4}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<5>>({1, 5, 0, 3, 2}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Perm<5>>({0, 1, 2, 3, 0}), LibsemigroupsException);

    REQUIRE_NOTHROW(PPerm<5>());
  }

  LIBSEMIGROUPS_TEST_CASE("LeastTransf etc",
                          "010",
                          "No throw",
                          "[quick][transf][pperm][perm]") {
    REQUIRE_NOTHROW(LeastTransf<3>({0, 1, 2}));
    REQUIRE_NOTHROW(LeastPPerm<3>({0, 1, 2}));
    REQUIRE_NOTHROW(LeastPerm<3>({0, 1, 2}));
  }

  LIBSEMIGROUPS_TEST_CASE("Transf",
                          "011",
                          "to_human_readable_repr",
                          "[quick]") {
    auto x = make<Transf<3>>({0, 1, 2});
    REQUIRE(to_human_readable_repr(x) == "Transf<3, uint8_t>({0, 1, 2})");
    REQUIRE(to_human_readable_repr(x, "", "[]")
            == "Transf<3, uint8_t>([0, 1, 2])");
    REQUIRE(to_human_readable_repr(x, "Transf", "[]") == "Transf([0, 1, 2])");
    REQUIRE(to_human_readable_repr(x) == to_input_string(x));
    REQUIRE(to_human_readable_repr(x, "", "{}", 3)
            == "<transformation of degree 3 and rank 3>");
    // Too many braces
    REQUIRE_THROWS_AS(to_human_readable_repr(x, "", "xxx"),
                      LibsemigroupsException);

    auto y = make<Perm<3>>({0, 1, 2});
    REQUIRE(to_human_readable_repr(y) == "Perm<3, uint8_t>({0, 1, 2})");
    REQUIRE(to_human_readable_repr(y, "", "[]")
            == "Perm<3, uint8_t>([0, 1, 2])");
    REQUIRE(to_human_readable_repr(y) == to_input_string(y));
    REQUIRE(to_human_readable_repr(y, "", "{}", 3)
            == "<permutation of degree 3>");
    // Too many braces
    REQUIRE_THROWS_AS(to_human_readable_repr(y, "", "xxx"),
                      LibsemigroupsException);

    auto z = make<PPerm<4>>({0, 1, 2}, {1, 2, 3}, 4);
    REQUIRE(to_human_readable_repr(z)
            == "PPerm<4, uint8_t>({0, 1, 2}, {1, 2, 3}, 4)");
    REQUIRE(to_human_readable_repr(z, "", "[]")
            == "PPerm<4, uint8_t>([0, 1, 2], [1, 2, 3], 4)");
    REQUIRE(to_human_readable_repr(z) == to_input_string(z));
    REQUIRE(to_human_readable_repr(z, "", "{}", 3)
            == "<partial permutation of degree 4 and rank 3>");
    // Too many braces
    REQUIRE_THROWS_AS(to_human_readable_repr(z, "", "xxx"),
                      LibsemigroupsException);
  }
}  // namespace libsemigroups
