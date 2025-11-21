// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

#include "libsemigroups/config.hpp"

#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED

#include <cstddef>  // for size_t

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // FOR LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/konieczny.hpp"  // for Konieczny
#include "libsemigroups/transf.hpp"     // for Transf<>

namespace libsemigroups {

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "043",
                          "partial transformations",
                          "[quick][ptransf]") {
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>({make<HPCombi::PTransf16>({1, 0, 2, 3, 4}),
                                    make<HPCombi::PTransf16>({1, 2, 3, 4, 0}),
                                    make<HPCombi::PTransf16>({0, 0, 2, 3, 4})});
    S.run();
    REQUIRE(S.size() == 3'125);

    size_t sum = 0;
    std::for_each(
        S.cbegin_current_D_classes(),
        S.cend_current_D_classes(),
        [&sum, &S](Konieczny<HPCombi::PTransf16>::DClass const& x) {
          sum += S.D_class_of_element(x.rep()).number_of_idempotents();
        });
    REQUIRE(sum == 196);
    REQUIRE(S.number_of_idempotents() == 196);
    REQUIRE(std::vector(S.cbegin_generators(), S.cend_generators())
            == std::vector({make<HPCombi::PTransf16>({1, 0, 2, 3, 4}),
                            make<HPCombi::PTransf16>({1, 2, 3, 4, 0}),
                            make<HPCombi::PTransf16>({0, 0, 2, 3, 4})}));
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "044",
                          "partial transformations - JDM favourite example",
                          "[quick][no-valgrind][ptransf]") {
    auto      rg = ReportGuard(false);
    Konieczny S
        = make<Konieczny>({make<HPCombi::PTransf16>({1, 7, 2, 6, 0, 4, 1, 5}),
                           make<HPCombi::PTransf16>({2, 4, 6, 1, 4, 5, 2, 7}),
                           make<HPCombi::PTransf16>({3, 0, 7, 2, 4, 6, 2, 4}),
                           make<HPCombi::PTransf16>({3, 2, 3, 4, 5, 3, 0, 1}),
                           make<HPCombi::PTransf16>({4, 3, 7, 7, 4, 5, 0, 4}),
                           make<HPCombi::PTransf16>({5, 6, 3, 0, 3, 0, 5, 1}),
                           make<HPCombi::PTransf16>({6, 0, 1, 1, 1, 6, 3, 4}),
                           make<HPCombi::PTransf16>({7, 7, 4, 0, 6, 4, 1, 7})});
    REQUIRE(S.size() == 597'369);
    size_t sum = 0;
    std::for_each(
        S.cbegin_current_D_classes(),
        S.cend_current_D_classes(),
        [&sum, &S](Konieczny<HPCombi::PTransf16>::DClass const& x) {
          sum += S.D_class_of_element(x.rep()).number_of_idempotents();
        });
    REQUIRE(sum == 8'194);
    REQUIRE(S.number_of_idempotents() == 8'194);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "045",
                          "partial transformations - large example",
                          "[quick][no-valgrind][ptransf]") {
    auto              rg = ReportGuard(false);
    std::vector const gens
        = {make<HPCombi::PTransf16>({2, 1, 0, 4, 2, 1, 1, 8, 0}),
           make<HPCombi::PTransf16>({1, 7, 6, 2, 5, 1, 1, 4, 3}),
           make<HPCombi::PTransf16>({1, 0, 7, 2, 1, 3, 1, 3, 7}),
           make<HPCombi::PTransf16>({0, 3, 8, 1, 2, 8, 1, 7, 0}),
           make<HPCombi::PTransf16>({0, 0, 0, 2, 7, 7, 5, 5, 3})};

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
                          "046",
                          "partial transformations: contains",
                          "[quick][ptransf]") {
    auto      rg = ReportGuard(false);
    Konieczny S  = make<Konieczny>({make<HPCombi::PTransf16>({1, 0, 2, 3, 4}),
                                    make<HPCombi::PTransf16>({1, 2, 3, 4, 0}),
                                    make<HPCombi::PTransf16>({0, 0, 2, 3, 4})});
    REQUIRE(S.contains(make<HPCombi::PTransf16>({1, 0, 2, 3, 4})));
    REQUIRE(S.contains(make<HPCombi::PTransf16>({1, 2, 3, 4, 0})));
    REQUIRE(S.contains(make<HPCombi::PTransf16>({0, 0, 2, 3, 4})));
    REQUIRE(!S.contains(make<HPCombi::PTransf16>({1, 2, 3, 4, 5, 0})));
    REQUIRE(!S.contains(make<HPCombi::PTransf16>({0, 0, 2, 3, 4, 1})));

    REQUIRE_THROWS_AS(
        S.D_class_of_element(make<HPCombi::PTransf16>({1, 0, 2, 3, 4, 0xFF})),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        S.D_class_of_element(make<HPCombi::PTransf16>({1, 2, 3, 4, 0, 0xFF})),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        S.D_class_of_element(make<HPCombi::PTransf16>({0, 0, 2, 3, 4, 1})),
        LibsemigroupsException);

    Konieczny T = make<Konieczny>({make<HPCombi::PTransf16>({1, 0, 3, 4, 2}),
                                   make<HPCombi::PTransf16>({0, 0, 2, 3, 4})});
    REQUIRE(T.contains(make<HPCombi::PTransf16>({1, 0, 2, 3, 4})));
    REQUIRE(T.contains(make<HPCombi::PTransf16>({0, 0, 2, 3, 4})));
    REQUIRE(!T.contains(make<HPCombi::PTransf16>({1, 2, 3, 4, 0})));
    REQUIRE(!T.contains(make<HPCombi::PTransf16>({1, 2, 3, 0, 4})));
    REQUIRE(!T.contains(make<HPCombi::PTransf16>({1, 2, 3, 4, 0, 5})));
    REQUIRE(!T.contains(make<HPCombi::PTransf16>({0, 2, 3, 4, 1})));

    REQUIRE_THROWS_AS(
        T.D_class_of_element(make<HPCombi::PTransf16>({1, 2, 3, 4, 0})),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        T.D_class_of_element(make<HPCombi::PTransf16>({1, 2, 3, 4, 0, 5})),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        T.D_class_of_element(make<HPCombi::PTransf16>({0, 2, 3, 4, 1})),
        LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Konieczny",
                          "047",
                          "add_generator",
                          "[quick][transf][no-valgrind]") {
    auto rg = ReportGuard(false);

    Konieczny S = make<Konieczny>({make<HPCombi::PTransf16>({1, 0, 2, 3, 4})});
    S.add_generator(make<HPCombi::PTransf16>({1, 2, 3, 4, 0}));
    S.add_generator(make<HPCombi::PTransf16>({0, 0, 2, 3, 4}));

    REQUIRE(S.generator(0) == make<HPCombi::PTransf16>({1, 0, 2, 3, 4}));
    REQUIRE(S.generator(1) == make<HPCombi::PTransf16>({1, 2, 3, 4, 0}));
    REQUIRE(S.generator(2) == make<HPCombi::PTransf16>({0, 0, 2, 3, 4}));
    REQUIRE(S.number_of_generators() == 3);
    REQUIRE(
        S.degree()
        == Degree<HPCombi::PTransf16>()(HPCombi::PTransf16({1, 0, 2, 3, 4})));
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_HPCOMBI_ENABLED
