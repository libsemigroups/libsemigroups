// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 Finn Smith
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/konieczny.hpp"  // for Konieczny
#include "libsemigroups/transf.hpp"     // for Transf<>

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "029",
                          "transformations",
                          "[quick][transf]") {
    using Transf = LeastTransf<5>;
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>({make<Transf>({1, 0, 2, 3, 4}),
                                    make<Transf>({1, 2, 3, 4, 0}),
                                    make<Transf>({0, 0, 2, 3, 4})});
    S.run();
    REQUIRE(S.size() == 3'125);

    size_t sum = 0;
    std::for_each(
        S.cbegin_current_D_classes(),
        S.cend_current_D_classes(),
        [&sum, &S](Konieczny<Transf>::DClass const& x) {
          sum += S.D_class_of_element(x.rep()).number_of_idempotents();
        });
    REQUIRE(sum == 196);
    REQUIRE(S.number_of_idempotents() == 196);
    REQUIRE(std::vector(S.cbegin_generators(), S.cend_generators())
            == std::vector({make<Transf>({1, 0, 2, 3, 4}),
                            make<Transf>({1, 2, 3, 4, 0}),
                            make<Transf>({0, 0, 2, 3, 4})}));
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "030",
                          "transformations - JDM favourite example",
                          "[quick][no-valgrind][transf]") {
    using Transf = LeastTransf<8>;
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>({make<Transf>({1, 7, 2, 6, 0, 4, 1, 5}),
                                    make<Transf>({2, 4, 6, 1, 4, 5, 2, 7}),
                                    make<Transf>({3, 0, 7, 2, 4, 6, 2, 4}),
                                    make<Transf>({3, 2, 3, 4, 5, 3, 0, 1}),
                                    make<Transf>({4, 3, 7, 7, 4, 5, 0, 4}),
                                    make<Transf>({5, 6, 3, 0, 3, 0, 5, 1}),
                                    make<Transf>({6, 0, 1, 1, 1, 6, 3, 4}),
                                    make<Transf>({7, 7, 4, 0, 6, 4, 1, 7})});
    REQUIRE(S.size() == 597'369);
    size_t sum = 0;
    std::for_each(
        S.cbegin_current_D_classes(),
        S.cend_current_D_classes(),
        [&sum, &S](Konieczny<Transf>::DClass const& x) {
          sum += S.D_class_of_element(x.rep()).number_of_idempotents();
        });
    REQUIRE(sum == 8'194);
    REQUIRE(S.number_of_idempotents() == 8'194);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "031",
                          "transformations - large example",
                          "[quick][no-valgrind][transf]") {
    auto              rg   = ReportGuard(false);
    std::vector const gens = {make<Transf<>>({2, 1, 0, 4, 2, 1, 1, 8, 0}),
                              make<Transf<>>({1, 7, 6, 2, 5, 1, 1, 4, 3}),
                              make<Transf<>>({1, 0, 7, 2, 1, 3, 1, 3, 7}),
                              make<Transf<>>({0, 3, 8, 1, 2, 8, 1, 7, 0}),
                              make<Transf<>>({0, 0, 0, 2, 7, 7, 5, 5, 3})};

    Konieczny S = make<Konieczny>(gens);

    for (auto const& x : gens) {
      REQUIRE(S.contains(x));
    }

    REQUIRE(S.current_size() < 15'000);
    REQUIRE(S.current_number_of_regular_elements() < 10'000);
    REQUIRE(S.current_number_of_idempotents() < 500);
    REQUIRE(S.current_number_of_D_classes() < 2'000);
    REQUIRE(S.current_number_of_L_classes() < 4'000);
    REQUIRE(S.current_number_of_R_classes() < 6'500);

    REQUIRE(S.size() == 232'511);
    REQUIRE(S.current_number_of_D_classes() == 2'122);
    REQUIRE(S.current_number_of_L_classes() == 8'450);
    REQUIRE(S.current_number_of_R_classes() == 14'706);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "032",
                          "transformations - large example with stop",
                          "[quick][no-valgrind][transf]") {
    auto      rg = ReportGuard(false);
    Konieczny S
        = make<Konieczny>({make<Transf<>>({2, 1, 0, 4, 2, 1, 1, 8, 0}),
                           make<Transf<>>({1, 7, 6, 2, 5, 1, 1, 4, 3}),
                           make<Transf<>>({1, 0, 7, 2, 1, 3, 1, 3, 7}),
                           make<Transf<>>({0, 3, 8, 1, 2, 8, 1, 7, 0}),
                           make<Transf<>>({0, 0, 0, 2, 7, 7, 5, 5, 3})});
    S.run_for(std::chrono::milliseconds(100));
    REQUIRE(S.size() == 232'511);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "033",
                          "transformations - large example with run_until",
                          "[quick][no-valgrind][transf]") {
    auto      rg = ReportGuard(false);
    Konieczny S
        = make<Konieczny>({make<Transf<>>({2, 1, 0, 4, 2, 1, 1, 8, 0}),
                           make<Transf<>>({1, 7, 6, 2, 5, 1, 1, 4, 3}),
                           make<Transf<>>({1, 0, 7, 2, 1, 3, 1, 3, 7}),
                           make<Transf<>>({0, 3, 8, 1, 2, 8, 1, 7, 0}),
                           make<Transf<>>({0, 0, 0, 2, 7, 7, 5, 5, 3})});
    S.run_until([&S]() -> bool {
      return S.cend_current_D_classes() - S.cbegin_current_D_classes() > 20;
    });

    size_t number_of_classes1
        = S.cend_current_D_classes() - S.cbegin_current_D_classes();
    REQUIRE(number_of_classes1 >= 20);
    S.run();
    size_t number_of_classes2
        = S.cend_current_D_classes() - S.cbegin_current_D_classes();
    REQUIRE(S.size() == 232'511);
    REQUIRE(number_of_classes1 < number_of_classes2);
    REQUIRE(number_of_classes2 == 2'122);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "034",
                          "transformations - large example with stop in Action",
                          "[quick][no-valgrind][transf]") {
    auto      rg = ReportGuard(false);
    Konieczny S
        = make<Konieczny>({make<Transf<>>({2, 1, 0, 4, 2, 1, 1, 8, 0}),
                           make<Transf<>>({1, 7, 6, 2, 5, 1, 1, 4, 3}),
                           make<Transf<>>({1, 0, 7, 2, 1, 3, 1, 3, 7}),
                           make<Transf<>>({0, 3, 8, 1, 2, 8, 1, 7, 0}),
                           make<Transf<>>({0, 0, 0, 2, 7, 7, 5, 5, 3})});
    S.run_for(std::chrono::milliseconds(5));
    S.run_for(std::chrono::milliseconds(5));
    S.run_for(std::chrono::milliseconds(5));
    S.run_for(std::chrono::milliseconds(100));
    S.run_for(std::chrono::milliseconds(100));
    S.run();
    S.run_for(std::chrono::milliseconds(100));
    S.run_for(std::chrono::milliseconds(100));
    REQUIRE(S.size() == 232'511);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny", "035", "exceptions", "[quick][transf]") {
    auto rg = ReportGuard(false);
    REQUIRE_THROWS_AS(make<Konieczny>({Transf<>::one(65)}),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "036",
                          "transformations: contains",
                          "[quick][transf]") {
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>({make<Transf<>>({1, 0, 2, 3, 4}),
                                    make<Transf<>>({1, 2, 3, 4, 0}),
                                    make<Transf<>>({0, 0, 2, 3, 4})});
    REQUIRE(S.contains(make<Transf<>>({1, 0, 2, 3, 4})));
    REQUIRE(S.contains(make<Transf<>>({1, 2, 3, 4, 0})));
    REQUIRE(S.contains(make<Transf<>>({0, 0, 2, 3, 4})));
    REQUIRE(!S.contains(make<Transf<>>({1, 0, 2, 3, 4, 5})));
    REQUIRE(!S.contains(make<Transf<>>({1, 2, 3, 4, 0, 5})));
    REQUIRE(!S.contains(make<Transf<>>({0, 0, 2, 3, 4, 1})));

    REQUIRE_THROWS_AS(S.D_class_of_element(make<Transf<>>({1, 0, 2, 3, 4, 5})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(S.D_class_of_element(make<Transf<>>({1, 2, 3, 4, 0, 5})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(S.D_class_of_element(make<Transf<>>({0, 0, 2, 3, 4, 1})),
                      LibsemigroupsException);

    Konieczny T = make<Konieczny>(
        {make<Transf<>>({1, 0, 3, 4, 2}), make<Transf<>>({0, 0, 2, 3, 4})});
    REQUIRE(T.contains(make<Transf<>>({1, 0, 2, 3, 4})));
    REQUIRE(T.contains(make<Transf<>>({0, 0, 2, 3, 4})));
    REQUIRE(!T.contains(make<Transf<>>({1, 2, 3, 4, 0})));
    REQUIRE(!T.contains(make<Transf<>>({1, 2, 3, 0, 4})));
    REQUIRE(!T.contains(make<Transf<>>({1, 2, 3, 4, 0, 5})));
    REQUIRE(!T.contains(make<Transf<>>({0, 2, 3, 4, 1})));

    REQUIRE_THROWS_AS(T.D_class_of_element(make<Transf<>>({1, 2, 3, 4, 0})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(T.D_class_of_element(make<Transf<>>({1, 2, 3, 4, 0, 5})),
                      LibsemigroupsException);
    REQUIRE_THROWS_AS(T.D_class_of_element(make<Transf<>>({0, 2, 3, 4, 1})),
                      LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "037",
                          "transformations Hall monoid 5",
                          "[extreme][transf]") {
    auto rg      = ReportGuard(true);
    using Transf = LeastTransf<31>;
    Konieczny<Transf> K;
    K.add_generator(make<Transf>({0,  16, 1,  17, 2,  18, 3,  19, 4,  20, 5,
                                  21, 6,  22, 7,  23, 8,  24, 9,  25, 10, 26,
                                  11, 27, 12, 28, 13, 29, 14, 30, 15}));
    K.add_generator(make<Transf>({0,  1,  2,  3,  4,  5,  6,  7,  16, 17, 18,
                                  19, 20, 21, 22, 23, 8,  9,  10, 11, 12, 13,
                                  14, 15, 24, 25, 26, 27, 28, 29, 30}));
    K.add_generator(
        make<Transf>({0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30,
                      0, 17, 8, 25, 4, 21, 12, 29, 2, 19, 10, 27, 6, 23, 14}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8, 4, 28, 2, 2, 2, 18, 2, 10, 6, 30,
                      1, 1, 1, 17, 1, 9, 5, 29, 3, 3, 3, 19, 3, 11, 7}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8, 0, 24, 0, 4, 0, 20, 0, 12, 2, 30,
                      1, 1, 1, 17, 1, 9, 1, 25, 1, 5, 1, 21, 1, 13, 3}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8, 0, 24, 0, 0, 4, 20, 2, 10, 6, 30,
                      1, 1, 1, 17, 1, 9, 1, 25, 1, 1, 5, 21, 3, 11, 7}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8,  0, 24, 0, 4, 0, 20, 0, 12, 0, 28,
                      0, 2, 0, 18, 0, 10, 0, 26, 0, 6, 0, 22, 0, 14, 1}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8, 0, 24, 0, 4, 0, 20, 0, 12, 0, 28,
                      0, 0, 2, 18, 0, 8, 2, 26, 0, 4, 2, 22, 1, 13, 3}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8,  0, 24, 0, 0, 4, 20, 0, 8,  4, 28,
                      0, 0, 0, 16, 2, 10, 2, 26, 0, 1, 4, 21, 2, 11, 6}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8, 0, 24, 0, 0, 4, 20, 0, 8, 4, 28,
                      0, 0, 0, 16, 0, 8, 2, 26, 0, 1, 4, 21, 0, 9, 6}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8, 0, 24, 0, 0, 0, 16, 0, 8, 4, 28,
                      0, 0, 0, 16, 0, 8, 2, 26, 0, 1, 0, 17, 0, 9, 6}));
    K.add_generator(
        make<Transf>({0, 0, 0, 16, 0, 8,  0, 24, 0, 0, 4, 20, 0, 8,  4, 28,
                      0, 0, 0, 16, 2, 10, 2, 26, 1, 1, 5, 21, 3, 11, 7}));
    REQUIRE(K.size() == 23'191'071);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "038",
                          "transformations - destructor coverage",
                          "[quick][transf][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<9>;
    Konieczny S  = make<Konieczny>({make<Transf>({2, 1, 0, 4, 2, 1, 1, 8, 0}),
                                    make<Transf>({1, 7, 6, 2, 5, 1, 1, 4, 3}),
                                    make<Transf>({1, 0, 7, 2, 1, 3, 1, 3, 7}),
                                    make<Transf>({0, 3, 8, 1, 2, 8, 1, 7, 0}),
                                    make<Transf>({0, 0, 0, 2, 7, 7, 5, 5, 3})});
    S.run_until(
        [&S]() -> bool { return S.current_number_of_regular_D_classes() > 2; });

    // if these fail, this test won't get the coverage hoped for
    REQUIRE(S.current_number_of_regular_D_classes() < 5);
    REQUIRE(S.current_number_of_D_classes() - S.number_of_regular_D_classes()
            < 2'117);
    // now all of the destructor should run
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "039",
                          "current_number_D_classes",
                          "[quick][transf][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<9>;
    Konieczny S  = make<Konieczny>({make<Transf>({2, 1, 0, 4, 2, 1, 1, 8, 0}),
                                    make<Transf>({1, 7, 6, 2, 5, 1, 1, 4, 3}),
                                    make<Transf>({1, 0, 7, 2, 1, 3, 1, 3, 7}),
                                    make<Transf>({0, 3, 8, 1, 2, 8, 1, 7, 0}),
                                    make<Transf>({0, 0, 0, 2, 7, 7, 5, 5, 3})});
    REQUIRE(S.current_number_of_regular_D_classes() == 0);
    REQUIRE(S.current_number_of_D_classes() == 0);
    S.run_until(
        [&S]() -> bool { return S.current_number_of_regular_D_classes() > 2; });

    S.run();
    REQUIRE(S.current_number_of_regular_D_classes() == 5);
    REQUIRE(S.current_number_of_D_classes() - S.number_of_regular_D_classes()
            == 2'117);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "040",
                          "add_generator",
                          "[quick][transf][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<5>;

    Konieczny S = make<Konieczny>({make<Transf>({1, 0, 2, 3, 4})});
    S.add_generator(make<Transf>({1, 2, 3, 4, 0}));
    S.add_generator(make<Transf>({0, 0, 2, 3, 4}));

    REQUIRE(S.generator(0) == make<Transf>({1, 0, 2, 3, 4}));
    REQUIRE(S.generator(1) == make<Transf>({1, 2, 3, 4, 0}));
    REQUIRE(S.generator(2) == make<Transf>({0, 0, 2, 3, 4}));
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(S.degree() == Degree<Transf>()(Transf({1, 0, 2, 3, 4})));
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "041",
                          "add_generator, init",
                          "[quick][transf][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<5>;

    Konieczny<Transf> S;
    S.add_generator(make<Transf>({1, 2, 3, 4, 0}));
    S.add_generator(make<Transf>({0, 0, 2, 3, 4}));

    REQUIRE(S.degree() == Degree<Transf>()(make<Transf>({1, 2, 3, 4, 0})));
    REQUIRE(S.number_of_generators() == 2);

    S.run();
    REQUIRE(S.size() == 610);

    S.init();
    S.add_generator(make<Transf>({1, 2, 3, 4, 0}));
    S.run();
    REQUIRE(S.size() == 5);

    S.init();
    S.add_generator(make<Transf>({1, 2, 3, 4, 0}));
    S.add_generator(make<Transf>({0, 0, 2, 3, 4}));
    REQUIRE(S.size() == 610);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "042",
                          "exceptions: keep object valid after refusing to run",
                          "[quick][transf][no-valgrind]") {
    auto rg      = ReportGuard(false);
    using Transf = LeastTransf<5>;

    Konieczny<Transf> S;

    REQUIRE_THROWS_AS(S.run(), LibsemigroupsException);

    S.add_generator(make<Transf>({1, 2, 3, 4, 0}));
    S.add_generator(make<Transf>({0, 0, 2, 3, 4}));

    REQUIRE(S.size() == 610);
  }

}  // namespace libsemigroups
