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

// The purpose of this file is to test the CongruenceInterface class.

// TODO IWYU!

#include "catch.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "libsemigroups/cong-pair.hpp"
#include "libsemigroups/cong.hpp"
#include "libsemigroups/element.hpp"
#include "libsemigroups/fpsemi.hpp"
#include "libsemigroups/knuth-bendix.hpp"
#include "libsemigroups/tce.hpp"
#include "libsemigroups/todd-coxeter.hpp"
#include "test-main.hpp"

namespace libsemigroups {
  struct LibsemigroupsException;  // Forward declaration

  constexpr bool REPORT              = false;
  congruence_type constexpr twosided = congruence_type::twosided;
  congruence_type constexpr left     = congruence_type::left;
  congruence_type constexpr right    = congruence_type::right;

  using detail::TCE;
  using FroidurePinTCE = FroidurePin<TCE, FroidurePinTraits<TCE, TCE::Table>>;

  namespace congruence {
    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "000",
                            "add_pair",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<CongruenceInterface> cong;

      SECTION("ToddCoxeter") {
        cong = detail::make_unique<ToddCoxeter>(twosided);
      }
      SECTION("KnuthBendix") {
        cong = detail::make_unique<KnuthBendix>();
      }
      SECTION("CongruenceByPairs") {
        ToddCoxeter tc(twosided);
        tc.set_nr_generators(2);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({1, 1, 1, 1}, {1});
        tc.add_pair({0, 1, 0, 1}, {0, 0});
        cong = detail::make_unique<CongruenceByPairs<FroidurePinTCE>>(
            right, tc.quotient_froidure_pin());
        // FIXME(when CongruenceByPairs has a proper traits class) We can't use
        // "twosided" here because TCE's products are only defined on the right
        // by generators, and CongruenceByPairs(twosided) does left and right
        // multiplication by generators. When CongruenceByPairs has a proper
        // traits class we can define Product to be the product in
        // tc.quotient_froidure_pin and via operator*.
      }
      SECTION("Congruence") {
        FpSemigroup S;  // free semigroup
        S.set_alphabet(2);
        cong = detail::make_unique<Congruence>(twosided, S);
      }

      cong->set_nr_generators(2);
      cong->add_pair({0, 0, 0}, {0});
      cong->add_pair({1, 1, 1, 1}, {1});
      cong->add_pair({0, 1, 0, 1}, {0, 0});

      REQUIRE(!cong->finished());
      REQUIRE(cong->nr_classes() == 27);
      REQUIRE(cong->finished());
      REQUIRE(cong->started());
      REQUIRE_THROWS_AS(cong->add_pair({0}, {1}), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "001",
                            "contains/const_contains",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<CongruenceInterface> cong;
      SECTION("no parent") {
        SECTION("ToddCoxeter") {
          cong = detail::make_unique<ToddCoxeter>(twosided);
        }
        SECTION("KnuthBendix") {
          cong = detail::make_unique<KnuthBendix>();
        }
        REQUIRE_THROWS_AS(cong->const_contains({0}, {1}),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(cong->const_contains({0}, {0}),
                          LibsemigroupsException);
      }
      SECTION("parent") {
        SECTION("Congruence") {
          FpSemigroup S;  // free semigroup
          S.set_alphabet(2);
          cong = detail::make_unique<Congruence>(twosided, S);
        }
        SECTION("CongruenceByPairs") {
          ToddCoxeter tc(twosided);
          tc.set_nr_generators(2);
          tc.add_pair({0, 0, 0}, {0});
          tc.add_pair({1, 1, 1, 1}, {1});
          tc.add_pair({0, 1, 0, 1}, {0, 0});
          cong = detail::make_unique<CongruenceByPairs<FroidurePinTCE>>(
              right, tc.quotient_froidure_pin());
          // FIXME(when CongruenceByPairs has a proper traits class) We can't
          // use "twosided" here because TCE's products are only defined on the
          // right by generators, and CongruenceByPairs(twosided) does left and
          // right multiplication by generators. When CongruenceByPairs has a
          // proper traits class we can define Product to be the product in
          // tc.quotient_froidure_pin and via operator*.
          REQUIRE(cong->const_contains({0}, {1}) == tril::unknown);
        }
        REQUIRE(cong->contains({0}, {0}));
      }

      REQUIRE(!cong->finished());
      REQUIRE(!cong->started());

      cong->set_nr_generators(2);
      cong->add_pair({0, 0, 0}, {0});
      cong->add_pair({1, 1, 1, 1}, {1});
      cong->add_pair({0, 1, 0, 1}, {0, 0});

      REQUIRE(cong->const_contains({0, 0, 0}, {0, 0}) == tril::unknown);
      REQUIRE(!cong->contains({0, 0, 0}, {0, 0}));
      REQUIRE(cong->const_contains({0, 0, 0}, {0, 0}) == tril::FALSE);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "002",
                            "less",
                            "[quick][no-valgrind]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<CongruenceInterface> cong;

      SECTION("ToddCoxeter") {
        cong = detail::make_unique<ToddCoxeter>(twosided);
        REQUIRE_THROWS_AS(cong->less({0}, {1}), LibsemigroupsException);
        REQUIRE(!cong->has_parent_froidure_pin());
      }
      SECTION("KnuthBendix") {
        cong = detail::make_unique<KnuthBendix>();
        REQUIRE_THROWS_AS(cong->less({0}, {1}), LibsemigroupsException);
        REQUIRE(!cong->has_parent_froidure_pin());
      }
      SECTION("CongruenceByPairs") {
        ToddCoxeter tc(twosided);
        tc.set_nr_generators(2);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({1, 1, 1, 1}, {1});
        tc.add_pair({0, 1, 0, 1}, {0, 0});
        cong = detail::make_unique<CongruenceByPairs<FroidurePinTCE>>(
            right, tc.quotient_froidure_pin());
        // FIXME(when CongruenceByPairs has a proper traits class) We can't use
        // "twosided" here because TCE's products are only defined on the right
        // by generators, and CongruenceByPairs(twosided) does left and right
        // multiplication by generators. When CongruenceByPairs has a proper
        // traits class we can define Product to be the product in
        // tc.quotient_froidure_pin and via operator*.

        // Wouldn't throw because of the parent semigroup, commented out anyway
        // so can add more pairs below
        // REQUIRE_NOTHROW(cong->less({0}, {1}));
        REQUIRE(cong->has_parent_froidure_pin());
      }
      SECTION("Congruence") {
        FpSemigroup S;  // free semigroup
        S.set_alphabet(2);
        cong = detail::make_unique<Congruence>(twosided, S);
      }

      cong->set_nr_generators(2);
      cong->add_pair({0, 0, 0}, {0});
      cong->add_pair({1, 1, 1, 1}, {1});
      cong->add_pair({0, 1, 0, 1}, {0, 0});

      REQUIRE(cong->less({0}, {1}));
      REQUIRE(cong->nr_classes() == 27);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "003",
                            "is_quotient_obviously_(in)finite",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<CongruenceInterface> cong;

      SECTION("ToddCoxeter") {
        cong = detail::make_unique<ToddCoxeter>(twosided);
      }
      SECTION("KnuthBendix") {
        cong = detail::make_unique<KnuthBendix>();
      }

      REQUIRE(!cong->is_quotient_obviously_infinite());
      // So far cong is not-defined, and hence not finite or infinite
      REQUIRE(!cong->is_quotient_obviously_finite());

      cong->set_nr_generators(2);
      REQUIRE(cong->is_quotient_obviously_infinite());
      REQUIRE(!cong->is_quotient_obviously_finite());

      cong->add_pair({0, 0, 0}, {0});
      cong->add_pair({1, 1, 1, 1}, {1});
      cong->add_pair({0, 1, 0, 1}, {0, 0});

      REQUIRE(!cong->is_quotient_obviously_infinite());
      REQUIRE(!cong->is_quotient_obviously_finite());

      REQUIRE(cong->nr_classes() == 27);
      REQUIRE(!cong->is_quotient_obviously_infinite());
      REQUIRE(cong->is_quotient_obviously_finite());
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "004",
                            "is_quotient_obviously_(in)finite",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<CongruenceInterface> cong;
      SECTION("CongruenceByPairs") {
        ToddCoxeter tc(twosided);
        tc.set_nr_generators(2);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({1, 1, 1, 1}, {1});
        tc.add_pair({0, 1, 0, 1}, {0, 0});
        cong = detail::make_unique<CongruenceByPairs<FroidurePinTCE>>(
            right, tc.quotient_froidure_pin());
      }
      SECTION("Congruence") {
        FpSemigroup S;
        S.set_alphabet(2);
        S.add_rule({0, 0, 0}, {0});
        S.add_rule({1, 1, 1, 1}, {1});
        S.add_rule({0, 1, 0, 1}, {0, 0});
        REQUIRE(!S.is_obviously_infinite());
        REQUIRE(!S.is_obviously_finite());
        cong = detail::make_unique<Congruence>(right, S);

        REQUIRE_THROWS_AS(cong->set_nr_generators(3), LibsemigroupsException);
      }

      cong->add_pair({0, 0, 0}, {0, 0});

      REQUIRE(!cong->is_quotient_obviously_infinite());
      REQUIRE(!cong->is_quotient_obviously_finite());
      REQUIRE(cong->nr_classes() == 24);
      REQUIRE(!cong->is_quotient_obviously_infinite());
      REQUIRE(cong->is_quotient_obviously_finite());
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "005",
                            "parent_froidure_pin",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<CongruenceInterface> cong;

      SECTION("ToddCoxeter") {
        cong = detail::make_unique<ToddCoxeter>(twosided);
      }
      SECTION("KnuthBendix") {
        cong = detail::make_unique<KnuthBendix>();
      }
      REQUIRE_THROWS_AS(cong->parent_froidure_pin(), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "009",
                            "parent_froidure_pin",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<CongruenceInterface> cong;
      SECTION("CongruenceByPairs") {
        ToddCoxeter tc(twosided);
        tc.set_nr_generators(2);
        tc.add_pair({0, 0, 0}, {0});
        tc.add_pair({1, 1, 1, 1}, {1});
        tc.add_pair({0, 1, 0, 1}, {0, 0});
        cong = detail::make_unique<CongruenceByPairs<FroidurePinTCE>>(
            left, tc.quotient_froidure_pin());
      }

      SECTION("Congruence") {
        FpSemigroup S;
        S.set_alphabet(2);
        S.add_rule({0, 0, 0}, {0});
        S.add_rule({1, 1, 1, 1}, {1});
        S.add_rule({0, 1, 0, 1}, {0, 0});
        REQUIRE(!S.is_obviously_infinite());
        REQUIRE_NOTHROW(S.froidure_pin());
        cong = detail::make_unique<Congruence>(left, S);
        REQUIRE_THROWS_AS(cong->set_nr_generators(3), LibsemigroupsException);
      }

      cong->add_pair({0, 0, 0}, {0, 0});

      REQUIRE(cong->has_parent_froidure_pin());
      REQUIRE(cong->parent_froidure_pin()->size() == 27);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "006",
                            "cbegin/cend_ntc",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transformation<uint16_t>>(
          {Transformation<uint16_t>({1, 3, 4, 2, 3}),
           Transformation<uint16_t>({3, 2, 1, 3, 3})});

      std::unique_ptr<CongruenceInterface> cong;

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      SECTION("ToddCoxeter") {
        cong = detail::make_unique<ToddCoxeter>(twosided, S);
      }
      SECTION("KnuthBendix") {
        cong = detail::make_unique<KnuthBendix>(S);
      }

      cong->add_pair(
          S.factorisation(Transformation<uint16_t>({3, 4, 4, 4, 4})),
          S.factorisation(Transformation<uint16_t>({3, 1, 3, 3, 3})));

      REQUIRE(cong->nr_classes() == 21);
      REQUIRE(cong->nr_non_trivial_classes() == 1);
      REQUIRE(cong->cbegin_ntc()->size() == 68);
      std::vector<word_type> expect = {{0, 0, 1},
                                       {1, 0, 1},
                                       {0, 0, 0, 1},
                                       {0, 0, 1, 0},
                                       {0, 0, 1, 1},
                                       {0, 1, 0, 1},
                                       {1, 0, 0, 1},
                                       {1, 0, 1, 0},
                                       {1, 0, 1, 1},
                                       {0, 0, 0, 0, 1},
                                       {0, 0, 0, 1, 0},
                                       {0, 0, 0, 1, 1},
                                       {0, 0, 1, 0, 0},
                                       {0, 0, 1, 0, 1},
                                       {0, 0, 1, 1, 0},
                                       {0, 1, 0, 1, 0},
                                       {0, 1, 0, 1, 1},
                                       {1, 0, 0, 0, 1},
                                       {1, 0, 0, 1, 0},
                                       {1, 0, 0, 1, 1},
                                       {1, 0, 1, 0, 0},
                                       {1, 0, 1, 0, 1},
                                       {1, 0, 1, 1, 0},
                                       {0, 0, 0, 0, 1, 0},
                                       {0, 0, 0, 0, 1, 1},
                                       {0, 0, 0, 1, 0, 0},
                                       {0, 0, 0, 1, 0, 1},
                                       {0, 0, 0, 1, 1, 0},
                                       {0, 0, 1, 0, 0, 0},
                                       {0, 0, 1, 1, 0, 0},
                                       {0, 1, 0, 0, 0, 1},
                                       {0, 1, 0, 1, 0, 0},
                                       {0, 1, 0, 1, 0, 1},
                                       {0, 1, 0, 1, 1, 0},
                                       {1, 0, 0, 0, 1, 0},
                                       {1, 0, 0, 0, 1, 1},
                                       {1, 0, 0, 1, 0, 0},
                                       {1, 0, 0, 1, 0, 1},
                                       {1, 0, 0, 1, 1, 0},
                                       {1, 0, 1, 0, 0, 0},
                                       {1, 0, 1, 1, 0, 0},
                                       {0, 0, 0, 0, 1, 0, 0},
                                       {0, 0, 0, 0, 1, 0, 1},
                                       {0, 0, 0, 0, 1, 1, 0},
                                       {0, 0, 0, 1, 0, 0, 0},
                                       {0, 0, 0, 1, 1, 0, 0},
                                       {0, 0, 1, 0, 0, 0, 1},
                                       {0, 1, 0, 0, 0, 1, 0},
                                       {0, 1, 0, 0, 0, 1, 1},
                                       {0, 1, 0, 1, 0, 0, 0},
                                       {0, 1, 0, 1, 1, 0, 0},
                                       {1, 0, 0, 0, 1, 0, 0},
                                       {1, 0, 0, 0, 1, 0, 1},
                                       {1, 0, 0, 0, 1, 1, 0},
                                       {1, 0, 0, 1, 0, 0, 0},
                                       {1, 0, 0, 1, 1, 0, 0},
                                       {0, 0, 0, 0, 1, 0, 0, 0},
                                       {0, 0, 0, 0, 1, 1, 0, 0},
                                       {0, 0, 1, 0, 0, 0, 1, 0},
                                       {0, 1, 0, 0, 0, 1, 0, 0},
                                       {0, 1, 0, 0, 0, 1, 0, 1},
                                       {0, 1, 0, 0, 0, 1, 1, 0},
                                       {1, 0, 0, 0, 1, 0, 0, 0},
                                       {1, 0, 0, 0, 1, 1, 0, 0},
                                       {0, 0, 1, 0, 0, 0, 1, 0, 0},
                                       {0, 0, 1, 0, 0, 0, 1, 0, 1},
                                       {0, 1, 0, 0, 0, 1, 0, 0, 0},
                                       {0, 1, 0, 0, 0, 1, 1, 0, 0}};
      auto                   it     = cong->cbegin_ntc();
      REQUIRE(it + 1 == cong->cend_ntc());
      REQUIRE(std::vector<word_type>(it->cbegin(), it->cend()) == expect);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "007",
                            "cbegin/cend_ntc",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transformation<uint16_t>>(
          {Transformation<uint16_t>({1, 3, 4, 2, 3}),
           Transformation<uint16_t>({3, 2, 1, 3, 3})});

      std::unique_ptr<CongruenceInterface> cong;

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 18);

      SECTION("CongruenceByPairs") {
        cong
            = detail::make_unique<CongruenceByPairs<decltype(S)::element_type>>(
                right, S);
      }
      SECTION("Congruence") {
        cong = detail::make_unique<Congruence>(right, S);
      }
      cong->add_pair(
          S.factorisation(Transformation<uint16_t>({3, 4, 4, 4, 4})),
          S.factorisation(Transformation<uint16_t>({3, 1, 3, 3, 3})));

      REQUIRE(cong->nr_classes() == 72);
      REQUIRE(cong->nr_non_trivial_classes() == 4);
      std::vector<size_t> actual(4, 0);
      std::transform(cong->cbegin_ntc(),
                     cong->cend_ntc(),
                     actual.begin(),
                     std::mem_fn(&std::vector<word_type>::size));
      std::sort(actual.begin(), actual.end());
      std::vector<size_t> expect = {3, 5, 5, 7};
      REQUIRE(actual == expect);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "008",
                            "quotient is immutable",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<CongruenceInterface> cong;

      SECTION("ToddCoxeter") {
        cong = detail::make_unique<ToddCoxeter>(twosided);
      }
      SECTION("KnuthBendix") {
        cong = detail::make_unique<KnuthBendix>();
      }
      // TODO not yet implemented
      // SECTION("CongruenceByPairs") {
      //   cong =
      //   detail::make_unique<CongruenceByPairs<decltype(S)::element_type>>(twosided,
      //   S);
      // }
      SECTION("Congruence") {
        cong = detail::make_unique<Congruence>(twosided);
      }

      cong->set_nr_generators(2);
      cong->add_pair({0, 0, 0}, {0});
      cong->add_pair({1, 1, 1, 1}, {1});
      cong->add_pair({0, 1, 0, 1}, {0, 0});

      REQUIRE(cong->quotient_froidure_pin()->immutable());
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "010",
                            "no generating pairs added",
                            "[quick][cong]") {
      auto                                 rg = ReportGuard(REPORT);
      std::unique_ptr<CongruenceInterface> cong;
      SECTION("KnuthBendix") {
        cong = detail::make_unique<KnuthBendix>();
        cong->set_nr_generators(4);
        // KnuthBendix can find the class index, but the others can't
        REQUIRE(cong->word_to_class_index({2, 2, 2, 2}) == 254);
        REQUIRE(cong->class_index_to_word(2) == word_type({2}));
      }

      SECTION("common behaviour") {
        SECTION("Congruence") {
          cong = detail::make_unique<Congruence>(twosided);
          cong->set_nr_generators(4);
          REQUIRE(cong->const_contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2})
                  == tril::FALSE);
        }
        SECTION("ToddCoxeter") {
          cong = detail::make_unique<ToddCoxeter>(twosided);
          cong->set_nr_generators(4);
          REQUIRE(cong->const_contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2})
                  == tril::unknown);
        }
        REQUIRE_THROWS_AS(cong->word_to_class_index({2, 2, 2, 2}),
                          LibsemigroupsException);
        REQUIRE_THROWS_AS(cong->class_index_to_word(2), LibsemigroupsException);
      }
      REQUIRE(!cong->contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}));
      REQUIRE(cong->nr_classes() == POSITIVE_INFINITY);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "011",
                            "nr generators not set",
                            "[quick][cong]") {
      auto                                 rg = ReportGuard(REPORT);
      std::unique_ptr<CongruenceInterface> cong;
      SECTION("ToddCoxeter") {
        cong = detail::make_unique<ToddCoxeter>(twosided);
      }
      SECTION("KnuthBendix") {
        cong = detail::make_unique<KnuthBendix>();
      }
      SECTION("Congruence") {
        cong = detail::make_unique<Congruence>(twosided);
      }
      REQUIRE_THROWS_AS(cong->set_nr_generators(0), LibsemigroupsException);
      REQUIRE_THROWS_AS(
          cong->const_contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}),
          LibsemigroupsException);
      REQUIRE_THROWS_AS(cong->contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}),
                        LibsemigroupsException);
      REQUIRE(cong->nr_classes() == UNDEFINED);
      REQUIRE_THROWS_AS(cong->word_to_class_index({2, 2, 2, 2}),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(cong->class_index_to_word(0), LibsemigroupsException);
      REQUIRE_THROWS_AS(cong->class_index_to_word(1), LibsemigroupsException);
      REQUIRE_THROWS_AS(cong->class_index_to_word(2), LibsemigroupsException);
      REQUIRE_THROWS_AS(cong->run(), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("CongruenceInterface",
                            "012",
                            "no generating pairs",
                            "[quick][cong]") {
      auto rg = ReportGuard(REPORT);
      auto S  = FroidurePin<Transformation<uint16_t>>(
          {Transformation<uint16_t>({1, 3, 4, 2, 3}),
           Transformation<uint16_t>({1, 3, 4, 2, 3}),
           Transformation<uint16_t>({3, 2, 1, 3, 3})});

      REQUIRE(S.size() == 88);
      REQUIRE(S.nr_rules() == 21);

      CongruenceByPairs<decltype(S)::element_type> cong(twosided, S);
      REQUIRE_THROWS_AS(cong.set_nr_generators(0), LibsemigroupsException);
      REQUIRE(cong.word_to_class_index({2, 2, 2, 2}) == 0);
      REQUIRE(cong.const_contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2})
              == tril::FALSE);
      REQUIRE_THROWS_AS(cong.class_index_to_word(2), LibsemigroupsException);
      REQUIRE(!cong.contains({1}, {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}));
      REQUIRE(cong.nr_classes() == 88);
    }
  }  // namespace congruence
}  // namespace libsemigroups
