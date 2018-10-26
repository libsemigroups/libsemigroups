// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Finn Smith
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

#include "froidure-pin.hpp"
#include "konieczny.hpp"
#include "libsemigroups.tests.hpp"

using namespace libsemigroups;

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "001",
                        "regular elements and idempotents",
                        "[quick]") {
  const std::vector<BMat8> gens
      = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Konieczny          KS(gens);
  FroidurePin<BMat8> S(gens);

  S.enumerate();
  REQUIRE(KS.size() == S.size());
  for (auto it = S.cbegin(); it < S.cend(); it++) {
    BMat8 idem = KS.find_idem(*it);
    if (KS.is_regular_element(*it)) {
      REQUIRE(idem * idem == idem);
    } else {
      REQUIRE(idem == BMat8(static_cast<size_t>(UNDEFINED)));
    }
  }
  const std::vector<BMat8> gens2
      = {BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}),
         BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 1}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}})};

  Konieczny          KS2(gens2);
  FroidurePin<BMat8> S2(gens2);

  S2.enumerate();
  for (auto it = S2.cbegin(); it < S2.cend(); it++) {
    BMat8 idem = KS2.find_idem(*it);
    if (KS2.is_regular_element(*it)) {
      REQUIRE(idem * idem == idem);
    } else {
      REQUIRE(idem == BMat8(static_cast<size_t>(UNDEFINED)));
    }
  }
}

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "002",
                        "regular elements and idempotents",
                        "[standard]") {
  const std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};
  Konieczny          KS(gens);
  FroidurePin<BMat8> S(gens);

  S.enumerate();

  for (auto it = S.cbegin(); it < S.cend(); it++) {
    if (KS.is_regular_element(*it)) {
      BMat8 idem = KS.find_idem(*it);
      REQUIRE(idem * idem == idem);
    }
  }
}

LIBSEMIGROUPS_TEST_CASE("Konieczny", "003", "regular D class 01", "[quick]") {
  const std::vector<BMat8> gens = {BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                                   BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                                   BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                   BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})};
  Konieczny                KS(gens);
  FroidurePin<BMat8>       S(gens);

  BMat8 x({{1, 0, 0}, {1, 0, 1}, {1, 1, 0}});
  REQUIRE(S.contains(x));
  BMat8                    idem = KS.find_idem(x);
  Konieczny::RegularDClass D    = Konieczny::RegularDClass(&KS, idem);
  REQUIRE(D.cend_left_indices() - D.cbegin_left_indices() == 3);
  REQUIRE(D.cend_right_indices() - D.cbegin_right_indices() == 3);
  // REQUIRE(D.stab_chain().size() == 2);
  REQUIRE(D.size() == 18);
}

LIBSEMIGROUPS_TEST_CASE("Konieczny", "004", "regular D class 02", "[quick]") {
  const std::vector<BMat8> gens
      = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Konieczny KS(gens);
  BMat8 idem(BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
  Konieczny::RegularDClass D = Konieczny::RegularDClass(&KS, idem);
  // REQUIRE(D.stab_chain().size() == 24);
  REQUIRE(D.size() == 24);
}

LIBSEMIGROUPS_TEST_CASE("Konieczny", "005", "regular D class 03", "[quick]") {
  const std::vector<BMat8> gens = {BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                                   BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                                   BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                   BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})};
  Konieczny                KS(gens);
  FroidurePin<BMat8>       S(gens);

  BMat8                    idem({{1, 0, 0}, {1, 1, 0}, {1, 0, 1}});
  Konieczny::RegularDClass D = Konieczny::RegularDClass(&KS, idem);
  REQUIRE(D.cend_left_idem_reps() - D.cbegin_left_idem_reps() == 3);
  REQUIRE(D.cend_right_idem_reps() - D.cbegin_right_idem_reps() == 3);
  for (auto it = D.cbegin_left_idem_reps(); it < D.cend_left_idem_reps();
       it++) {
    BMat8 x = *it;
    for (auto it2 = it + 1; it2 < D.cend_left_idem_reps(); it2++) {
      REQUIRE(x != *it2);
    }
    REQUIRE(x * x == x);
    REQUIRE(S.contains(x));
  }
  for (auto it = D.cbegin_right_idem_reps(); it < D.cend_right_idem_reps();
       it++) {
    BMat8 x = *it;
    for (auto it2 = it + 1; it2 < D.cend_right_idem_reps(); it2++) {
      REQUIRE(x != *it2);
    }
    REQUIRE(x * x == x);
    REQUIRE(S.contains(x));
  }
}

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "006",
                        "regular D class 04: contains",
                        "[quick]") {
  const std::vector<BMat8> gens
      = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Konieczny          KS(gens);
  FroidurePin<BMat8> S(gens);
  S.enumerate();
  BMat8 idem(BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}));
  Konieczny::RegularDClass D = Konieczny::RegularDClass(&KS, idem);

  // test that the top D class contains only permutation matrices
  for (auto it = S.cbegin(); it < S.cend(); it++) {
    if (((*it) * (*it).transpose()) == gens[0]) {
      REQUIRE(D.contains(*it));
    } else {
      REQUIRE(!D.contains(*it));
    }
  }
}

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "007",
                        "non-regular D classes 01",
                        "[quick]") {
  const std::vector<BMat8> gens = {BMat8({{0, 1, 0}, {0, 0, 1}, {1, 0, 0}}),
                                   BMat8({{0, 1, 0}, {1, 0, 0}, {0, 0, 1}}),
                                   BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                   BMat8({{1, 1, 0}, {0, 1, 1}, {1, 0, 1}})};
  Konieczny                KS(gens);
  const std::vector<BMat8> idems = {BMat8({{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}),
                                    BMat8({{1, 0, 0}, {1, 1, 0}, {0, 0, 1}}),
                                    BMat8({{1, 0, 0}, {1, 1, 1}, {0, 0, 1}}),
                                    BMat8({{1, 0, 0}, {1, 1, 0}, {1, 0, 1}}),
                                    BMat8({{1, 0, 0}, {1, 1, 0}, {1, 1, 1}}),
                                    BMat8({{1, 1, 0}, {1, 1, 0}, {0, 0, 1}}),
                                    BMat8({{1, 0, 0}, {1, 1, 1}, {1, 1, 1}}),
                                    BMat8({{1, 1, 0}, {1, 1, 0}, {1, 1, 1}}),
                                    BMat8({{1, 1, 1}, {1, 1, 1}, {1, 1, 1}})};

  REQUIRE(KS.regular_D_classes().size() == idems.size());

  size_t count = 0;
  for (BMat8 id : idems) {
    Konieczny::RegularDClass* D = new Konieczny::RegularDClass(&KS, id);
    count += D->size();
  }

  REQUIRE(count == 142);

  const std::vector<BMat8> non_reg_reps
      = {BMat8({{0, 0, 1}, {1, 0, 1}, {1, 1, 0}}),
         BMat8({{0, 0, 1}, {1, 1, 1}, {1, 1, 0}}),
         BMat8({{0, 1, 1}, {1, 0, 1}, {1, 1, 1}}),
         BMat8({{0, 1, 1}, {1, 1, 0}, {1, 0, 1}}),
         BMat8({{1, 0, 1}, {1, 0, 1}, {1, 1, 0}}),
         BMat8({{1, 1, 0}, {1, 1, 1}, {1, 1, 1}})};

  Konieczny::NonRegularDClass X(&KS, non_reg_reps[0]);
  REQUIRE(X.size() == 36);
  REQUIRE(X.cend_H_class() - X.cbegin_H_class() == 1);
  REQUIRE(X.cend_left_reps() - X.cbegin_left_reps() == 6);
  REQUIRE(X.cend_right_reps() - X.cbegin_right_reps() == 6);

  Konieczny::NonRegularDClass Y(&KS, non_reg_reps[1]);
  REQUIRE(Y.size() == 18);
  REQUIRE(Y.cend_H_class() - Y.cbegin_H_class() == 1);
  REQUIRE(Y.cend_left_reps() - Y.cbegin_left_reps() == 3);
  REQUIRE(Y.cend_right_reps() - Y.cbegin_right_reps() == 6);

  Konieczny::NonRegularDClass Z(&KS, non_reg_reps[2]);
  REQUIRE(Z.size() == 18);
  REQUIRE(Z.cend_H_class() - Z.cbegin_H_class() == 2);
  REQUIRE(Z.cend_left_reps() - Z.cbegin_left_reps() == 3);
  REQUIRE(Z.cend_right_reps() - Z.cbegin_right_reps() == 3);

  Konieczny::NonRegularDClass A(&KS, non_reg_reps[3]);
  REQUIRE(A.size() == 6);
  REQUIRE(A.cend_H_class() - A.cbegin_H_class() == 6);
  REQUIRE(A.cend_left_reps() - A.cbegin_left_reps() == 1);
  REQUIRE(A.cend_right_reps() - A.cbegin_right_reps() == 1);

  Konieczny::NonRegularDClass B(&KS, non_reg_reps[4]);
  REQUIRE(B.size() == 18);
  REQUIRE(B.cend_H_class() - B.cbegin_H_class() == 1);
  REQUIRE(B.cend_left_reps() - B.cbegin_left_reps() == 6);
  REQUIRE(B.cend_right_reps() - B.cbegin_right_reps() == 3);

  Konieczny::NonRegularDClass C(&KS, non_reg_reps[5]);
  REQUIRE(C.size() == 9);
  REQUIRE(C.cend_H_class() - C.cbegin_H_class() == 1);
  REQUIRE(C.cend_left_reps() - C.cbegin_left_reps() == 3);
  REQUIRE(C.cend_right_reps() - C.cbegin_right_reps() == 3);

  for (BMat8 x : non_reg_reps) {
    Konieczny::NonRegularDClass N(&KS, x);
    count += N.size();
  }

  REQUIRE(count == 247);

  REQUIRE(KS.size() == 247);
}

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "008",
                        "non-regular D classes 02",
                        "[standard]") {
  const std::vector<BMat8> gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 1}, {1, 0, 0, 1}, {0, 1, 1, 0}}),
         BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 1, 0}, {1, 1, 1, 1}}),
         BMat8({{1, 1, 0, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
         BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {1, 0, 0, 0}}),
         BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 0}}),
         BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}, {0, 0, 1, 0}})};

  Konieczny KS(gens);

  const std::vector<BMat8> idems
      = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 1, 1, 1}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}}),
         BMat8({{1, 1, 1, 1}, {0, 1, 0, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}}),
         BMat8({{1, 0, 0, 1}, {0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 1}, {1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 1, 1, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}),
         BMat8({{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}}),
         BMat8({{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}})};

  size_t count = 0;
  for (BMat8 id : idems) {
    Konieczny::RegularDClass* D = new Konieczny::RegularDClass(&KS, id);
    count += D->size();
  }

  REQUIRE(KS.regular_D_classes().size() == idems.size());

  REQUIRE(count == 8712);

  const std::vector<BMat8> non_reg_reps
      = {BMat8({{1, 1, 1, 1}, {1, 1, 1, 1}, {0, 1, 1, 1}, {1, 1, 1, 0}}),
         BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 1}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 1, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {1, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 1, 1}}),
         BMat8({{1, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {0, 0, 1, 0}}),
         BMat8({{0, 1, 1, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
         BMat8({{0, 0, 0, 0}, {0, 1, 0, 1}, {0, 1, 1, 1}, {1, 1, 1, 0}}),
         BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}, {1, 0, 1, 0}}),
         BMat8({{0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}, {1, 1, 1, 0}}),
         BMat8({{1, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {1, 1, 0, 1}}),
         BMat8({{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 0, 1}}),
         BMat8({{1, 0, 0, 1}, {1, 1, 0, 1}, {0, 0, 0, 0}, {0, 1, 1, 0}}),
         BMat8({{0, 1, 1, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}, {1, 1, 0, 1}}),
         BMat8({{1, 1, 0, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
         BMat8({{0, 1, 1, 1}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 1, 1, 0}, {0, 1, 0, 1}, {0, 0, 0, 0}, {0, 1, 1, 0}}),
         BMat8({{0, 1, 1, 1}, {1, 1, 0, 1}, {0, 0, 0, 0}, {0, 1, 1, 0}}),
         BMat8({{0, 1, 1, 1}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 1, 0}}),
         BMat8({{1, 1, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 1, 0, 1}}),
         BMat8({{0, 0, 1, 0}, {1, 0, 1, 0}, {1, 0, 0, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 0, 1}}),
         BMat8({{0, 0, 1, 0}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 0, 1, 0}, {1, 0, 1, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 1, 0, 1}, {1, 1, 1, 1}, {1, 1, 1, 0}}),
         BMat8({{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 1, 1}, {1, 0, 1, 0}}),
         BMat8({{1, 0, 0, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}, {1, 0, 0, 0}}),
         BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 1, 1, 1}, {1, 1, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{0, 0, 1, 1}, {1, 0, 1, 1}, {1, 0, 0, 0}, {1, 0, 1, 0}}),
         BMat8({{1, 0, 0, 0}, {0, 0, 1, 1}, {0, 0, 1, 1}, {1, 0, 1, 0}}),
         BMat8({{1, 0, 0, 0}, {1, 0, 1, 1}, {0, 0, 1, 1}, {1, 0, 1, 0}}),
         BMat8({{0, 1, 0, 0}, {1, 1, 1, 0}, {1, 1, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{0, 1, 0, 1}, {0, 1, 0, 1}, {0, 1, 1, 0}, {1, 1, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {0, 1, 0, 1}, {1, 1, 1, 0}, {0, 1, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 0, 1}, {0, 1, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 1, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 0}, {1, 1, 0, 1}}),
         BMat8({{0, 1, 0, 1}, {0, 1, 1, 0}, {0, 1, 1, 0}, {1, 1, 1, 1}}),
         BMat8({{0, 1, 1, 0}, {0, 1, 0, 1}, {0, 1, 0, 1}, {1, 1, 1, 0}}),
         BMat8({{1, 1, 1, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 1, 0}, {1, 1, 1, 0}, {0, 1, 1, 1}, {1, 1, 1, 0}}),
         BMat8({{1, 1, 1, 0}, {1, 1, 1, 1}, {0, 1, 1, 1}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 1, 0}, {0, 1, 0, 1}, {0, 1, 0, 1}, {1, 1, 1, 0}}),
         BMat8({{0, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 1, 1, 0}, {1, 1, 1, 0}, {0, 1, 1, 1}}),
         BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {1, 0, 1, 0}, {0, 1, 1, 0}}),
         BMat8({{0, 1, 0, 1}, {1, 1, 1, 1}, {1, 1, 1, 0}, {0, 1, 1, 1}}),
         BMat8({{1, 0, 1, 0}, {1, 1, 1, 1}, {1, 1, 0, 1}, {1, 1, 1, 0}}),
         BMat8({{0, 1, 1, 0}, {0, 1, 1, 0}, {1, 1, 0, 1}, {0, 1, 1, 1}}),
         BMat8({{0, 1, 1, 0}, {0, 1, 1, 1}, {0, 1, 1, 1}, {1, 1, 1, 0}}),
         BMat8({{1, 1, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 1}, {0, 1, 1, 0}}),
         BMat8({{0, 1, 1, 0}, {1, 1, 1, 0}, {1, 1, 1, 1}, {0, 1, 1, 1}}),
         BMat8({{1, 0, 0, 1}, {1, 1, 1, 1}, {0, 1, 1, 0}, {1, 1, 0, 1}}),
         BMat8({{0, 1, 1, 0}, {1, 1, 0, 1}, {1, 1, 0, 1}, {0, 1, 1, 1}}),
         BMat8({{0, 1, 1, 0}, {1, 1, 1, 1}, {1, 1, 0, 1}, {0, 1, 1, 1}}),
         BMat8({{0, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 0}})};

  Konieczny::NonRegularDClass X(&KS, non_reg_reps[0]);

  Konieczny::NonRegularDClass X0
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[0]);
  REQUIRE(X0.size() == 180);
  REQUIRE(X0.cend_H_class() - X0.cbegin_H_class() == 2);
  REQUIRE(X0.cend_left_reps() - X0.cbegin_left_reps() == 2);
  REQUIRE(X0.cend_right_reps() - X0.cbegin_right_reps() == 45);

  Konieczny::NonRegularDClass X1
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[1]);
  REQUIRE(X1.size() == 16);
  REQUIRE(X1.cend_H_class() - X1.cbegin_H_class() == 1);
  REQUIRE(X1.cend_left_reps() - X1.cbegin_left_reps() == 4);
  REQUIRE(X1.cend_right_reps() - X1.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X2
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[2]);
  REQUIRE(X2.size() == 64);
  REQUIRE(X2.cend_H_class() - X2.cbegin_H_class() == 1);
  REQUIRE(X2.cend_left_reps() - X2.cbegin_left_reps() == 4);
  REQUIRE(X2.cend_right_reps() - X2.cbegin_right_reps() == 16);

  Konieczny::NonRegularDClass X3
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[3]);
  REQUIRE(X3.size() == 16);
  REQUIRE(X3.cend_H_class() - X3.cbegin_H_class() == 1);
  REQUIRE(X3.cend_left_reps() - X3.cbegin_left_reps() == 4);
  REQUIRE(X3.cend_right_reps() - X3.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X4
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[4]);
  REQUIRE(X4.size() == 32);
  REQUIRE(X4.cend_H_class() - X4.cbegin_H_class() == 1);
  REQUIRE(X4.cend_left_reps() - X4.cbegin_left_reps() == 4);
  REQUIRE(X4.cend_right_reps() - X4.cbegin_right_reps() == 8);

  Konieczny::NonRegularDClass X5
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[5]);
  REQUIRE(X5.size() == 64);
  REQUIRE(X5.cend_H_class() - X5.cbegin_H_class() == 2);
  REQUIRE(X5.cend_left_reps() - X5.cbegin_left_reps() == 4);
  REQUIRE(X5.cend_right_reps() - X5.cbegin_right_reps() == 8);

  Konieczny::NonRegularDClass X6
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[6]);
  REQUIRE(X6.size() == 16);
  REQUIRE(X6.cend_H_class() - X6.cbegin_H_class() == 1);
  REQUIRE(X6.cend_left_reps() - X6.cbegin_left_reps() == 4);
  REQUIRE(X6.cend_right_reps() - X6.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X7
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[7]);
  REQUIRE(X7.size() == 16);
  REQUIRE(X7.cend_H_class() - X7.cbegin_H_class() == 1);
  REQUIRE(X7.cend_left_reps() - X7.cbegin_left_reps() == 4);
  REQUIRE(X7.cend_right_reps() - X7.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X8
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[8]);
  REQUIRE(X8.size() == 16);
  REQUIRE(X8.cend_H_class() - X8.cbegin_H_class() == 1);
  REQUIRE(X8.cend_left_reps() - X8.cbegin_left_reps() == 4);
  REQUIRE(X8.cend_right_reps() - X8.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X9
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[9]);
  REQUIRE(X9.size() == 16);
  REQUIRE(X9.cend_H_class() - X9.cbegin_H_class() == 1);
  REQUIRE(X9.cend_left_reps() - X9.cbegin_left_reps() == 4);
  REQUIRE(X9.cend_right_reps() - X9.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X10
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[10]);
  REQUIRE(X10.size() == 16);
  REQUIRE(X10.cend_H_class() - X10.cbegin_H_class() == 1);
  REQUIRE(X10.cend_left_reps() - X10.cbegin_left_reps() == 4);
  REQUIRE(X10.cend_right_reps() - X10.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X11
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[11]);
  REQUIRE(X11.size() == 16);
  REQUIRE(X11.cend_H_class() - X11.cbegin_H_class() == 1);
  REQUIRE(X11.cend_left_reps() - X11.cbegin_left_reps() == 4);
  REQUIRE(X11.cend_right_reps() - X11.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X12
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[12]);
  REQUIRE(X12.size() == 16);
  REQUIRE(X12.cend_H_class() - X12.cbegin_H_class() == 1);
  REQUIRE(X12.cend_left_reps() - X12.cbegin_left_reps() == 4);
  REQUIRE(X12.cend_right_reps() - X12.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X13
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[13]);
  REQUIRE(X13.size() == 16);
  REQUIRE(X13.cend_H_class() - X13.cbegin_H_class() == 1);
  REQUIRE(X13.cend_left_reps() - X13.cbegin_left_reps() == 4);
  REQUIRE(X13.cend_right_reps() - X13.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X14
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[14]);
  REQUIRE(X14.size() == 16);
  REQUIRE(X14.cend_H_class() - X14.cbegin_H_class() == 1);
  REQUIRE(X14.cend_left_reps() - X14.cbegin_left_reps() == 4);
  REQUIRE(X14.cend_right_reps() - X14.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X15
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[15]);
  REQUIRE(X15.size() == 16);
  REQUIRE(X15.cend_H_class() - X15.cbegin_H_class() == 1);
  REQUIRE(X15.cend_left_reps() - X15.cbegin_left_reps() == 4);
  REQUIRE(X15.cend_right_reps() - X15.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X16
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[16]);
  REQUIRE(X16.size() == 16);
  REQUIRE(X16.cend_H_class() - X16.cbegin_H_class() == 1);
  REQUIRE(X16.cend_left_reps() - X16.cbegin_left_reps() == 4);
  REQUIRE(X16.cend_right_reps() - X16.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X17
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[17]);
  REQUIRE(X17.size() == 16);
  REQUIRE(X17.cend_H_class() - X17.cbegin_H_class() == 2);
  REQUIRE(X17.cend_left_reps() - X17.cbegin_left_reps() == 2);
  REQUIRE(X17.cend_right_reps() - X17.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X18
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[18]);
  REQUIRE(X18.size() == 32);
  REQUIRE(X18.cend_H_class() - X18.cbegin_H_class() == 1);
  REQUIRE(X18.cend_left_reps() - X18.cbegin_left_reps() == 8);
  REQUIRE(X18.cend_right_reps() - X18.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X19
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[19]);
  REQUIRE(X19.size() == 16);
  REQUIRE(X19.cend_H_class() - X19.cbegin_H_class() == 1);
  REQUIRE(X19.cend_left_reps() - X19.cbegin_left_reps() == 4);
  REQUIRE(X19.cend_right_reps() - X19.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X20
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[20]);
  REQUIRE(X20.size() == 156);
  REQUIRE(X20.cend_H_class() - X20.cbegin_H_class() == 2);
  REQUIRE(X20.cend_left_reps() - X20.cbegin_left_reps() == 39);
  REQUIRE(X20.cend_right_reps() - X20.cbegin_right_reps() == 2);

  Konieczny::NonRegularDClass X21
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[21]);
  REQUIRE(X21.size() == 16);
  REQUIRE(X21.cend_H_class() - X21.cbegin_H_class() == 1);
  REQUIRE(X21.cend_left_reps() - X21.cbegin_left_reps() == 4);
  REQUIRE(X21.cend_right_reps() - X21.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X22
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[22]);
  REQUIRE(X22.size() == 32);
  REQUIRE(X22.cend_H_class() - X22.cbegin_H_class() == 1);
  REQUIRE(X22.cend_left_reps() - X22.cbegin_left_reps() == 4);
  REQUIRE(X22.cend_right_reps() - X22.cbegin_right_reps() == 8);

  Konieczny::NonRegularDClass X23
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[23]);
  REQUIRE(X23.size() == 32);
  REQUIRE(X23.cend_H_class() - X23.cbegin_H_class() == 1);
  REQUIRE(X23.cend_left_reps() - X23.cbegin_left_reps() == 4);
  REQUIRE(X23.cend_right_reps() - X23.cbegin_right_reps() == 8);

  Konieczny::NonRegularDClass X24
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[24]);
  REQUIRE(X24.size() == 16);
  REQUIRE(X24.cend_H_class() - X24.cbegin_H_class() == 1);
  REQUIRE(X24.cend_left_reps() - X24.cbegin_left_reps() == 4);
  REQUIRE(X24.cend_right_reps() - X24.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X25
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[25]);
  REQUIRE(X25.size() == 16);
  REQUIRE(X25.cend_H_class() - X25.cbegin_H_class() == 1);
  REQUIRE(X25.cend_left_reps() - X25.cbegin_left_reps() == 4);
  REQUIRE(X25.cend_right_reps() - X25.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X26
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[26]);
  REQUIRE(X26.size() == 32);
  REQUIRE(X26.cend_H_class() - X26.cbegin_H_class() == 1);
  REQUIRE(X26.cend_left_reps() - X26.cbegin_left_reps() == 4);
  REQUIRE(X26.cend_right_reps() - X26.cbegin_right_reps() == 8);

  Konieczny::NonRegularDClass X27
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[27]);
  REQUIRE(X27.size() == 16);
  REQUIRE(X27.cend_H_class() - X27.cbegin_H_class() == 1);
  REQUIRE(X27.cend_left_reps() - X27.cbegin_left_reps() == 4);
  REQUIRE(X27.cend_right_reps() - X27.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X28
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[28]);
  REQUIRE(X28.size() == 16);
  REQUIRE(X28.cend_H_class() - X28.cbegin_H_class() == 1);
  REQUIRE(X28.cend_left_reps() - X28.cbegin_left_reps() == 4);
  REQUIRE(X28.cend_right_reps() - X28.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X29
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[29]);
  REQUIRE(X29.size() == 16);
  REQUIRE(X29.cend_H_class() - X29.cbegin_H_class() == 1);
  REQUIRE(X29.cend_left_reps() - X29.cbegin_left_reps() == 4);
  REQUIRE(X29.cend_right_reps() - X29.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X30
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[30]);
  REQUIRE(X30.size() == 16);
  REQUIRE(X30.cend_H_class() - X30.cbegin_H_class() == 1);
  REQUIRE(X30.cend_left_reps() - X30.cbegin_left_reps() == 4);
  REQUIRE(X30.cend_right_reps() - X30.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X31
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[31]);
  REQUIRE(X31.size() == 16);
  REQUIRE(X31.cend_H_class() - X31.cbegin_H_class() == 1);
  REQUIRE(X31.cend_left_reps() - X31.cbegin_left_reps() == 4);
  REQUIRE(X31.cend_right_reps() - X31.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X32
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[32]);
  REQUIRE(X32.size() == 32);
  REQUIRE(X32.cend_H_class() - X32.cbegin_H_class() == 1);
  REQUIRE(X32.cend_left_reps() - X32.cbegin_left_reps() == 8);
  REQUIRE(X32.cend_right_reps() - X32.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X33
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[33]);
  REQUIRE(X33.size() == 16);
  REQUIRE(X33.cend_H_class() - X33.cbegin_H_class() == 1);
  REQUIRE(X33.cend_left_reps() - X33.cbegin_left_reps() == 4);
  REQUIRE(X33.cend_right_reps() - X33.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X34
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[34]);
  REQUIRE(X34.size() == 32);
  REQUIRE(X34.cend_H_class() - X34.cbegin_H_class() == 1);
  REQUIRE(X34.cend_left_reps() - X34.cbegin_left_reps() == 8);
  REQUIRE(X34.cend_right_reps() - X34.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X35
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[35]);
  REQUIRE(X35.size() == 32);
  REQUIRE(X35.cend_H_class() - X35.cbegin_H_class() == 2);
  REQUIRE(X35.cend_left_reps() - X35.cbegin_left_reps() == 4);
  REQUIRE(X35.cend_right_reps() - X35.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X36
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[36]);
  REQUIRE(X36.size() == 16);
  REQUIRE(X36.cend_H_class() - X36.cbegin_H_class() == 1);
  REQUIRE(X36.cend_left_reps() - X36.cbegin_left_reps() == 4);
  REQUIRE(X36.cend_right_reps() - X36.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X37
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[37]);
  REQUIRE(X37.size() == 32);
  REQUIRE(X37.cend_H_class() - X37.cbegin_H_class() == 1);
  REQUIRE(X37.cend_left_reps() - X37.cbegin_left_reps() == 8);
  REQUIRE(X37.cend_right_reps() - X37.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X38
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[38]);
  REQUIRE(X38.size() == 16);
  REQUIRE(X38.cend_H_class() - X38.cbegin_H_class() == 1);
  REQUIRE(X38.cend_left_reps() - X38.cbegin_left_reps() == 4);
  REQUIRE(X38.cend_right_reps() - X38.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X39
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[39]);
  REQUIRE(X39.size() == 32);
  REQUIRE(X39.cend_H_class() - X39.cbegin_H_class() == 1);
  REQUIRE(X39.cend_left_reps() - X39.cbegin_left_reps() == 8);
  REQUIRE(X39.cend_right_reps() - X39.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X40
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[40]);
  REQUIRE(X40.size() == 16);
  REQUIRE(X40.cend_H_class() - X40.cbegin_H_class() == 1);
  REQUIRE(X40.cend_left_reps() - X40.cbegin_left_reps() == 4);
  REQUIRE(X40.cend_right_reps() - X40.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X41
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[41]);
  REQUIRE(X41.size() == 16);
  REQUIRE(X41.cend_H_class() - X41.cbegin_H_class() == 1);
  REQUIRE(X41.cend_left_reps() - X41.cbegin_left_reps() == 4);
  REQUIRE(X41.cend_right_reps() - X41.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X42
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[42]);
  REQUIRE(X42.size() == 16);
  REQUIRE(X42.cend_H_class() - X42.cbegin_H_class() == 1);
  REQUIRE(X42.cend_left_reps() - X42.cbegin_left_reps() == 4);
  REQUIRE(X42.cend_right_reps() - X42.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X43
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[43]);
  REQUIRE(X43.size() == 16);
  REQUIRE(X43.cend_H_class() - X43.cbegin_H_class() == 1);
  REQUIRE(X43.cend_left_reps() - X43.cbegin_left_reps() == 4);
  REQUIRE(X43.cend_right_reps() - X43.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X44
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[44]);
  REQUIRE(X44.size() == 16);
  REQUIRE(X44.cend_H_class() - X44.cbegin_H_class() == 1);
  REQUIRE(X44.cend_left_reps() - X44.cbegin_left_reps() == 4);
  REQUIRE(X44.cend_right_reps() - X44.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X45
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[45]);
  REQUIRE(X45.size() == 16);
  REQUIRE(X45.cend_H_class() - X45.cbegin_H_class() == 1);
  REQUIRE(X45.cend_left_reps() - X45.cbegin_left_reps() == 4);
  REQUIRE(X45.cend_right_reps() - X45.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X46
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[46]);
  REQUIRE(X46.size() == 16);
  REQUIRE(X46.cend_H_class() - X46.cbegin_H_class() == 1);
  REQUIRE(X46.cend_left_reps() - X46.cbegin_left_reps() == 4);
  REQUIRE(X46.cend_right_reps() - X46.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X47
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[47]);
  REQUIRE(X47.size() == 16);
  REQUIRE(X47.cend_H_class() - X47.cbegin_H_class() == 1);
  REQUIRE(X47.cend_left_reps() - X47.cbegin_left_reps() == 4);
  REQUIRE(X47.cend_right_reps() - X47.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X48
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[48]);
  REQUIRE(X48.size() == 16);
  REQUIRE(X48.cend_H_class() - X48.cbegin_H_class() == 1);
  REQUIRE(X48.cend_left_reps() - X48.cbegin_left_reps() == 4);
  REQUIRE(X48.cend_right_reps() - X48.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X49
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[49]);
  REQUIRE(X49.size() == 16);
  REQUIRE(X49.cend_H_class() - X49.cbegin_H_class() == 1);
  REQUIRE(X49.cend_left_reps() - X49.cbegin_left_reps() == 4);
  REQUIRE(X49.cend_right_reps() - X49.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X50
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[50]);
  REQUIRE(X50.size() == 16);
  REQUIRE(X50.cend_H_class() - X50.cbegin_H_class() == 1);
  REQUIRE(X50.cend_left_reps() - X50.cbegin_left_reps() == 4);
  REQUIRE(X50.cend_right_reps() - X50.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X51
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[51]);
  REQUIRE(X51.size() == 16);
  REQUIRE(X51.cend_H_class() - X51.cbegin_H_class() == 2);
  REQUIRE(X51.cend_left_reps() - X51.cbegin_left_reps() == 2);
  REQUIRE(X51.cend_right_reps() - X51.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X52
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[52]);
  REQUIRE(X52.size() == 16);
  REQUIRE(X52.cend_H_class() - X52.cbegin_H_class() == 1);
  REQUIRE(X52.cend_left_reps() - X52.cbegin_left_reps() == 4);
  REQUIRE(X52.cend_right_reps() - X52.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X53
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[53]);
  REQUIRE(X53.size() == 16);
  REQUIRE(X53.cend_H_class() - X53.cbegin_H_class() == 1);
  REQUIRE(X53.cend_left_reps() - X53.cbegin_left_reps() == 4);
  REQUIRE(X53.cend_right_reps() - X53.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X54
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[54]);
  REQUIRE(X54.size() == 16);
  REQUIRE(X54.cend_H_class() - X54.cbegin_H_class() == 1);
  REQUIRE(X54.cend_left_reps() - X54.cbegin_left_reps() == 4);
  REQUIRE(X54.cend_right_reps() - X54.cbegin_right_reps() == 4);

  Konieczny::NonRegularDClass X55
      = Konieczny::NonRegularDClass(&KS, non_reg_reps[55]);
  REQUIRE(X55.size() == 8);
  REQUIRE(X55.cend_H_class() - X55.cbegin_H_class() == 2);
  REQUIRE(X55.cend_left_reps() - X55.cbegin_left_reps() == 2);
  REQUIRE(X55.cend_right_reps() - X55.cbegin_right_reps() == 2);
}

LIBSEMIGROUPS_TEST_CASE("Konieczny", "009", "D class containment", "[quick]") {
  const std::vector<BMat8> gens
      = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 0, 1, 1}}),
         BMat8({{0, 1, 0, 1}, {1, 0, 1, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}})};

  Konieczny                KS(gens);
  BMat8                    idem = BMat8({{1, 0, 1}, {0, 1, 0}, {1, 0, 1}});
  Konieczny::RegularDClass D    = Konieczny::RegularDClass(&KS, idem);
  for (auto it = D.cbegin_left_reps(); it < D.cend_left_reps(); it++) {
    REQUIRE(D.index_positions(*it).first != UNDEFINED);
    REQUIRE(D.index_positions(*it).second != UNDEFINED);
    REQUIRE(D.contains(*it));
  }
  for (auto it = D.cbegin_right_reps(); it < D.cend_right_reps(); it++) {
    REQUIRE(D.contains(*it));
  }
}

LIBSEMIGROUPS_TEST_CASE("Konieczny", "010", "full bmat monoids", "[extreme]") {
  const std::vector<BMat8> bmat4_gens
      = {BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 1, 1, 0}, {1, 0, 0, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{1, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 0, 0, 1}}),
         BMat8({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}),
         BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}})};

  const std::vector<BMat8> bmat5_gens = {BMat8({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                         BMat8({{0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1},
                                                {1, 0, 0, 0, 0}}),
                                         BMat8({{0, 1, 0, 0, 0},
                                                {1, 0, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                         BMat8({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {1, 0, 0, 0, 1}}),
                                         BMat8({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                         BMat8({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                         BMat8({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 1, 0},
                                                {0, 0, 0, 0, 1}}),
                                         BMat8({{1, 1, 0, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                         BMat8({{1, 1, 1, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                         BMat8({{1, 0, 0, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 0}}),
                                         BMat8({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1},
                                                {0, 0, 0, 1, 1}}),
                                         BMat8({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 1, 0},
                                                {0, 0, 1, 0, 1}}),
                                         BMat8({{1, 1, 1, 0, 0},
                                                {1, 0, 0, 1, 1},
                                                {0, 1, 0, 1, 0},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 1, 0}}),
                                         BMat8({{1, 1, 1, 0, 0},
                                                {1, 1, 0, 1, 0},
                                                {1, 0, 0, 0, 1},
                                                {0, 1, 0, 0, 1},
                                                {0, 0, 1, 1, 1}})};

  Konieczny S(bmat4_gens);
  REQUIRE(S.size() == 65536);

  // Konieczny T(bmat5_gens);
  // REQUIRE(T.size() == 33554432);
}

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "011",
                        "regular generated bmat monoids",
                        "[extreme]") {
  const std::vector<BMat8> reg_bmat4_gens
      = {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}})};

  const std::vector<BMat8> reg_bmat5_gens = {BMat8({{0, 1, 0, 0, 0},
                                                    {1, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}}),
                                             BMat8({{0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1},
                                                    {1, 0, 0, 0, 0}}),
                                             BMat8({{1, 0, 0, 0, 0},
                                                    {1, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}}),
                                             BMat8({{0, 0, 0, 0, 0},
                                                    {0, 1, 0, 0, 0},
                                                    {0, 0, 1, 0, 0},
                                                    {0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 1}})};
  const std::vector<BMat8> reg_bmat6_gens = {BMat8({{0, 1, 0, 0, 0, 0},
                                                    {1, 0, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 0, 0},
                                                    {0, 0, 0, 1, 0, 0},
                                                    {0, 0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 0, 1}}),
                                             BMat8({{0, 1, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 0, 0},
                                                    {0, 0, 0, 1, 0, 0},
                                                    {0, 0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 0, 1},
                                                    {1, 0, 0, 0, 0, 0}}),
                                             BMat8({{1, 0, 0, 0, 0, 0},
                                                    {0, 1, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 0, 0},
                                                    {0, 0, 0, 1, 0, 0},
                                                    {0, 0, 0, 0, 1, 0},
                                                    {1, 0, 0, 0, 0, 1}}),
                                             BMat8({{1, 0, 0, 0, 0, 0},
                                                    {0, 1, 0, 0, 0, 0},
                                                    {0, 0, 1, 0, 0, 0},
                                                    {0, 0, 0, 1, 0, 0},
                                                    {0, 0, 0, 0, 1, 0},
                                                    {0, 0, 0, 0, 0, 0}})};
  Konieczny                S(reg_bmat4_gens);
  REQUIRE(S.size() == 63904);

  // Konieczny T(reg_bmat5_gens);
  // REQUIRE(T.size() == 32311832);

  size_t reg_elts = 0;
  for (auto D : S.regular_D_classes()) {
    reg_elts += D->size();
  }
  REQUIRE(reg_elts == 40408);

  // reg_elts = 0;
  // for (auto D : T.regular_D_classes()) {
  //   reg_elts += D->size();
  // }
  // REQUIRE(reg_elts == 8683982);

  // Konieczny U(reg_bmat6_gens);
  // REQUIRE(U.size() == 1);
}

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "012",
                        "my favourite example",
                        "[standard][finite]") {
  const std::vector<BMat8> gens = {BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0}}),
                                   BMat8({{0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}})};

  Konieczny S(gens);
  REQUIRE(S.size() == 597369);
}

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "013",
                        "another large example",
                        "[standard][finite]") {
  const std::vector<BMat8> gens = {BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat8({{1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0}}),
                                   BMat8({{0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0}})};

  Konieczny S(gens);
  REQUIRE(S.size() == 201750);
}

LIBSEMIGROUPS_TEST_CASE("Konieczny",
                        "014",
                        "my favourite example transposed",
                        "[standard][finite]") {
  const std::vector<BMat8> gens = {BMat8({{0, 0, 0, 0, 1, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1}}),
                                   BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 1, 0},
                                          {1, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 1, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 1, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 1, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 1, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 1, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 1, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 1, 1, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 1},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {1, 0, 0, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0}}),
                                   BMat8({{0, 0, 0, 1, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 1, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 1, 0, 0, 1, 0, 0},
                                          {0, 0, 0, 0, 0, 0, 0, 0},
                                          {0, 0, 0, 0, 1, 0, 0, 0},
                                          {1, 1, 0, 0, 0, 0, 0, 1}})};

  Konieczny S(gens);
  REQUIRE(S.size() == 597369);
}
