//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James D. Mitchell
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

#include <cstddef>    // for size_t
#include <cstdint>    // for uint32_t, int32_t, int64_t
#include <stdexcept>  // for out_of_range
#include <vector>     // for vector

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

    REQUIRE(x.at(0) == 0);
    REQUIRE(x.at(1) == 1);
    REQUIRE(x[0] == 0);
    REQUIRE(*x.begin() == 0);
    REQUIRE(*x.cbegin() == 0);
    REQUIRE(*(x.end() - 1) == 0);
    REQUIRE(*(x.cend() - 1) == 0);

    REQUIRE(x == y);
    REQUIRE(y * y == x);
    REQUIRE((x < y) == false);
    REQUIRE(x <= y);
    REQUIRE((x > y) == false);
    REQUIRE(x >= y);

    auto z = make<TestType>({0, 1, 0, 3});
    REQUIRE(x < z);
    REQUIRE(x <= z);
    REQUIRE(x != z);
    REQUIRE(image(z) == std::vector<typename TestType::point_type>({0, 1, 3}));
    z.swap(x);
    REQUIRE(x > z);
    REQUIRE(x >= z);
    REQUIRE(x != z);
    REQUIRE(image(z) == std::vector<typename TestType::point_type>({0, 1}));
    z.swap(x);

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
    REQUIRE_THROWS_AS(make<Transf<1>>({1}), LibsemigroupsException);
    REQUIRE_THROWS_AS(make<Transf<1>>({1}), LibsemigroupsException);

    REQUIRE_NOTHROW(make<Transf<3>>({0, 1, 2}));

    REQUIRE_THROWS_AS(make<Transf<3>>({1, 2, 3}), LibsemigroupsException);

    REQUIRE_THROWS_AS(make<Transf<3>>({UNDEFINED, UNDEFINED, UNDEFINED}),
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
    auto const yy = x * x;
    REQUIRE(yy[0] == UNDEFINED);
    REQUIRE(yy[1] == UNDEFINED);
    REQUIRE(yy.at(2) == UNDEFINED);
    REQUIRE(yy.at(3) == UNDEFINED);
    REQUIRE(yy.at(4) == UNDEFINED);
    REQUIRE(yy.at(5) == 1);
    REQUIRE(*x.begin() == 1);
    REQUIRE(*(x.begin() + 1) == UNDEFINED);
    REQUIRE(*(x.begin() + 4) == 9);
    REQUIRE(*x.cbegin() == 1);
    REQUIRE(*(x.cbegin() + 1) == UNDEFINED);
    REQUIRE(*(x.cbegin() + 4) == 9);
    REQUIRE(*(x.end() - 1) == UNDEFINED);
    REQUIRE(*(x.end() - 5) == 0);
    REQUIRE(*(x.cend() - 1) == UNDEFINED);
    REQUIRE(*(x.cend() - 5) == 0);

    try {
      std::ignore = yy.at(10);
    } catch (std::out_of_range const& e) {
      REQUIRE(std::string(e.what())
              == "index out of range, expected a value in [0, 10) found 10");
    }

    REQUIRE_THROWS_AS(yy.at(10), std::out_of_range);

    REQUIRE(yy > y);
    REQUIRE(!(x < x));
    REQUIRE(x <= y);
    REQUIRE(x >= y);

    auto expected = TestType({UNDEFINED, UNDEFINED, UNDEFINED});
    REQUIRE(expected > x);

    REQUIRE(x.degree() == 10);
    REQUIRE(y.degree() == 10);
    REQUIRE(Complexity<TestType>()(x) == 10);
    REQUIRE(Complexity<TestType>()(y) == 10);
    REQUIRE(yy.rank() == 1);
    REQUIRE(y.rank() == 3);
    REQUIRE(x.rank() == 3);

    auto z = make<TestType>({8, 1, 7, 3}, {2, 1, 0, 5}, 10);
    REQUIRE(x < z);
    REQUIRE(x <= z);
    REQUIRE(x != z);
    REQUIRE(image(z)
            == std::vector<typename TestType::point_type>({0, 1, 2, 5}));
    z.swap(x);
    REQUIRE(x > z);
    REQUIRE(x >= z);
    REQUIRE(x != z);
    REQUIRE(image(z) == std::vector<typename TestType::point_type>({0, 1, 9}));
    z.swap(x);

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

    // TODO(0): uncomment and fix, or delete
    // auto x = make<PPerm<0, uint8_t>>({}, {}, 257);
    // REQUIRE(x * x == x);
    // x = make<PPerm<0, uint8_t>>({}, {}, 256);
    // REQUIRE(x * x == x);

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

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Perm",
                                   "012",
                                   "mem fns",
                                   "[quick][perm]",
                                   Perm<>,
                                   Perm<4>) {
    static_assert(IsPerm<TestType>, "IsPerm<TestType> must be true!");
    TestType x  = make<TestType>({1, 0, 3, 2});
    TestType y  = make<TestType>({1, 0, 3, 2});
    TestType id = one(x);

    REQUIRE(x == y);
    REQUIRE(y * y == id);
    REQUIRE((x < y) == false);

    TestType z = make<TestType>({1, 3, 0, 2});
    REQUIRE(x < z);
    REQUIRE(image(z)
            == std::vector<typename TestType::point_type>({0, 1, 2, 3}));

    TestType expected = make<TestType>({0, 1, 2, 3});
    REQUIRE(expected < x);

    REQUIRE(z.degree() == 4);
    REQUIRE(Complexity<TestType>()(z) == 4);
    REQUIRE(z.rank() == 4);

    expected = make<TestType>({0, 1, 2, 3});
    REQUIRE(id == expected);

    if (IsDynamic<TestType>) {
      x.increase_degree_by(10);
      REQUIRE(x.degree() == 14);
      REQUIRE(x.end() - x.begin() == 14);
    } else {
      REQUIRE_THROWS_AS(x.increase_degree_by(10), LibsemigroupsException);
    }

    REQUIRE(x.hash_value() != 0);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Transf",
                                   "013",
                                   "const mem fns",
                                   "[quick][transf]",
                                   Transf<>,
                                   Transf<4>) {
    static_assert(IsTransf<TestType>, "IsTransf<TestType> must be true!");
    TestType const x = make<TestType>({0, 1, 0, 0});
    TestType const y = make<TestType>({0, 1, 0, 0});

    REQUIRE(x.at(0) == 0);
    REQUIRE(x.at(1) == 1);
    REQUIRE(x[0] == 0);
    REQUIRE(*x.begin() == 0);
    REQUIRE(*x.cbegin() == 0);
    REQUIRE(*(x.end() - 1) == 0);
    REQUIRE(*(x.cend() - 1) == 0);

    REQUIRE(x == y);
    REQUIRE(y * y == x);
    REQUIRE((x < y) == false);
    REQUIRE(x <= y);
    REQUIRE((x > y) == false);
    REQUIRE(x >= y);

    TestType const z = make<TestType>({0, 1, 0, 3});
    REQUIRE(x < z);
    REQUIRE(x <= z);
    REQUIRE(x != z);
    REQUIRE(image(z) == std::vector<typename TestType::point_type>({0, 1, 3}));

    TestType const expected_1 = make<TestType>({0, 0, 0, 0});
    REQUIRE(expected_1 < x);

    REQUIRE(z.degree() == 4);
    REQUIRE(Complexity<TestType>()(z) == 4);
    REQUIRE(z.rank() == 3);
    TestType const id = one(z);

    TestType const expected_2 = make<TestType>({0, 1, 2, 3});
    REQUIRE(id == expected_2);

    REQUIRE(x.hash_value() != 0);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("PPerm",
                                   "014",
                                   "const mem fns",
                                   "[quick][pperm]",
                                   PPerm<>,
                                   PPerm<10>) {
    static_assert(IsPPerm<TestType>, "IsPPerm<TestType> must be true!");
    TestType const x = make<TestType>({4, 5, 0}, {9, 0, 1}, 10);
    TestType const y = make<TestType>({4, 5, 0}, {9, 0, 1}, 10);

    REQUIRE(x.undef() == UNDEFINED);
    REQUIRE(x == y);
    auto yy = x * x;
    REQUIRE(yy[0] == UNDEFINED);
    REQUIRE(yy[1] == UNDEFINED);
    REQUIRE(yy.at(2) == UNDEFINED);
    REQUIRE(yy.at(3) == UNDEFINED);
    REQUIRE(yy.at(4) == UNDEFINED);
    REQUIRE(yy.at(5) == 1);
    REQUIRE(*x.begin() == 1);
    REQUIRE(*(x.begin() + 1) == UNDEFINED);
    REQUIRE(*(x.begin() + 4) == 9);
    REQUIRE(*x.cbegin() == 1);
    REQUIRE(*(x.cbegin() + 1) == UNDEFINED);
    REQUIRE(*(x.cbegin() + 4) == 9);
    REQUIRE(*(x.end() - 1) == UNDEFINED);
    REQUIRE(*(x.end() - 5) == 0);
    REQUIRE(*(x.cend() - 1) == UNDEFINED);
    REQUIRE(*(x.cend() - 5) == 0);

    try {
      std::ignore = yy.at(10);
    } catch (std::out_of_range const& e) {
      REQUIRE(std::string(e.what())
              == "index out of range, expected a value in [0, 10) found 10");
    }

    REQUIRE_THROWS_AS(yy.at(10), std::out_of_range);

    REQUIRE(yy > y);
    REQUIRE(!(x < x));
    REQUIRE(x <= y);
    REQUIRE(x >= y);
    TestType const expected_1 = TestType({UNDEFINED, UNDEFINED, UNDEFINED});
    REQUIRE(expected_1 > x);

    REQUIRE(x.degree() == 10);
    REQUIRE(y.degree() == 10);
    REQUIRE(Complexity<TestType>()(x) == 10);
    REQUIRE(Complexity<TestType>()(y) == 10);
    REQUIRE(yy.rank() == 1);
    REQUIRE(y.rank() == 3);
    REQUIRE(x.rank() == 3);

    TestType const z = make<TestType>({8, 1, 7, 3}, {2, 1, 0, 5}, 10);
    REQUIRE(x < z);
    REQUIRE(x <= z);
    REQUIRE(x != z);
    REQUIRE(image(z)
            == std::vector<typename TestType::point_type>({0, 1, 2, 5}));

    TestType const id         = one(x);
    TestType const expected_2 = TestType({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
    REQUIRE(id == expected_2);

    REQUIRE(x.hash_value() != 0);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("Perm",
                                   "015",
                                   "const mem fns",
                                   "[quick][perm]",
                                   Perm<>,
                                   Perm<4>) {
    static_assert(IsPerm<TestType>, "IsPerm<TestType> must be true!");
    TestType const x  = make<TestType>({1, 0, 3, 2});
    TestType const y  = make<TestType>({1, 0, 3, 2});
    TestType const id = one(x);

    REQUIRE(x == y);
    REQUIRE(y * y == id);
    REQUIRE(!(x > y));
    REQUIRE(!(x < y));
    REQUIRE(x <= y);
    REQUIRE(x >= y);

    REQUIRE((x < y) == false);
    REQUIRE(x[0] == 1);
    REQUIRE(x[1] == 0);
    REQUIRE(x.at(2) == 3);
    REQUIRE(x.at(3) == 2);
    REQUIRE(*x.begin() == 1);
    REQUIRE(*(x.begin() + 1) == 0);
    REQUIRE(*x.cbegin() == 1);
    REQUIRE(*(x.cbegin() + 1) == 0);
    REQUIRE(*(x.end() - 1) == 2);
    REQUIRE(*(x.cend() - 1) == 2);

    TestType const z = make<TestType>({1, 3, 0, 2});
    REQUIRE(x < z);
    REQUIRE(image(z)
            == std::vector<typename TestType::point_type>({0, 1, 2, 3}));

    TestType const expected_1 = make<TestType>({0, 1, 2, 3});
    REQUIRE(expected_1 < x);

    REQUIRE(z.degree() == 4);
    REQUIRE(Complexity<TestType>()(z) == 4);
    REQUIRE(z.rank() == 4);

    TestType const expected_2 = make<TestType>({0, 1, 2, 3});
    REQUIRE(id == expected_2);

    REQUIRE(x.hash_value() != 0);
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("PTransf",
                                   "016",
                                   "increase_degree_by",
                                   "[quick][transf][perm][pperm]",
                                   (Transf<0, uint8_t>),
                                   (Perm<0, uint8_t>),
                                   (PPerm<0, uint8_t>) ) {
    TestType x = make<TestType>(std::vector<uint8_t>());
    REQUIRE_NOTHROW(x.increase_degree_by(0));
    REQUIRE_EXCEPTION_MSG(x.increase_degree_by(257),
                          "the 1st argument (value to increase degree by) is "
                          "too large, expected value in [0, 256], found 257");
    REQUIRE_NOTHROW(x.increase_degree_by(256));
    REQUIRE_EXCEPTION_MSG(x.increase_degree_by(1),
                          "the 1st argument (value to increase degree by) is "
                          "too large, expected value in [0, 0], found 1");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("PTransf",
                                   "017",
                                   "make from large container",
                                   "[quick][transf][perm][pperm]",
                                   (Transf<0, uint8_t>),
                                   (Perm<0, uint8_t>),
                                   (PPerm<0, uint8_t>) ) {
    std::vector<uint8_t> small_sequence(std::numeric_limits<uint8_t>::max()
                                        + 1);
    std::iota(small_sequence.begin(), small_sequence.end(), 0);
    REQUIRE_NOTHROW(std::ignore = make<TestType>(small_sequence));

    std::vector<uint8_t> big_sequence(std::numeric_limits<uint8_t>::max() + 2);
    std::iota(big_sequence.begin(), big_sequence.end(), 0);
    REQUIRE_EXCEPTION_MSG(
        std::ignore = make<TestType>(big_sequence),
        "the degree is too large, expected value in [0, 256], found 257");
  }

  LIBSEMIGROUPS_TEMPLATE_TEST_CASE("PTransf",
                                   "018",
                                   "make from large init. list",
                                   "[quick][transf][perm][pperm]",
                                   (Transf<0, uint8_t>),
                                   (Perm<0, uint8_t>),
                                   (PPerm<0, uint8_t>) ) {
    REQUIRE_NOTHROW(
        std::ignore = make<TestType>(
            {0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,
             13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
             26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
             39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,
             52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,
             65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,
             78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
             91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103,
             104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
             117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
             130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
             143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155,
             156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
             169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181,
             182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
             195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
             208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
             221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
             234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246,
             247, 248, 249, 250, 251, 252, 253, 254, 255}));

    REQUIRE_EXCEPTION_MSG(
        std::ignore = make<TestType>(
            {0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,
             13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,
             26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
             39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,
             52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,
             65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,
             78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
             91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103,
             104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116,
             117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
             130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142,
             143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155,
             156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168,
             169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181,
             182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
             195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
             208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
             221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233,
             234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246,
             247, 248, 249, 250, 251, 252, 253, 254, 255, 0}),
        "the degree is too large, expected value in [0, 256], found 257");
  }
}  // namespace libsemigroups
