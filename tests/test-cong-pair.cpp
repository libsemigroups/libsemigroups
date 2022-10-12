//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Michael Young
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

// The purpose of this file is to test the classes CongruenceByPairs
// and FpSemigroupByPairs.

#include <algorithm>   // for sort, transform
#include <functional>  // for mem_fn
#include <vector>      // for vector

#include "catch.hpp"  // for REQUIRE, SECTION, REQUIRE_THROWS_AS, REQ...
#include "libsemigroups/cong-intf.hpp"  // for congruence_kind, CongruenceInterface::non_tr...
#include "libsemigroups/cong-pair.hpp"  // for KnuthBendixCongruenceByPairs, CongruenceByPairs
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix
#include "libsemigroups/report.hpp"        // for ReportGuard
#include "libsemigroups/tce.hpp"           // for TCE
#include "libsemigroups/todd-coxeter.hpp"  // for ToddCoxeter
#include "libsemigroups/transf.hpp"        // for Transf<>
#include "libsemigroups/types.hpp"         // for relation_type, word_type
#include "test-main.hpp"                   // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;
}  // namespace libsemigroups

namespace libsemigroups {
  constexpr bool REPORT = false;

  congruence_kind const twosided = congruence_kind::twosided;
  congruence_kind const left     = congruence_kind::left;
  congruence_kind const right    = congruence_kind::right;
  using congruence::ToddCoxeter;

  using detail::TCE;
  using FroidurePinTCE = FroidurePin<TCE, FroidurePinTraits<TCE, TCE::Table>>;

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "001",
                          "(cong) 2-sided cong. on finite semigroup",
                          "[quick][cong][cong-pair]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Transf<>> gens
        = {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})};
    FroidurePin<Transf<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size() == 88);
    // REQUIRE(S.number_of_rules() == 18);

    CongruenceByPairs<decltype(S)> p(twosided, S);  // p copies S!
    REQUIRE(p.has_parent_froidure_pin());

    p.add_pair({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1});

    REQUIRE(p.word_to_class_index({0, 0, 0, 1})
            == p.word_to_class_index({0, 0, 1, 0, 0}));
    REQUIRE(p.finished());
    REQUIRE(!p.parent_froidure_pin()->started());
    REQUIRE(!p.parent_froidure_pin()->finished());

    REQUIRE(p.number_of_classes() == 21);
    REQUIRE(p.number_of_classes() == 21);
    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "002",
                          "(cong) left congruence on finite semigroup",
                          "[quick][cong][cong-pair]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Transf<>> gens
        = {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})};
    FroidurePin<Transf<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 88);
    // REQUIRE(S.number_of_rules(false) == 18);

    CongruenceByPairs<decltype(S)> p(left, S);
    p.add_pair({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1});

    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 1);
    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 69);
    REQUIRE(p.number_of_classes() == 69);
    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "003",
                          "(cong) right congruence on finite semigroup",
                          "[quick][cong][cong-pair]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Transf<>> gens
        = {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})};
    FroidurePin<Transf<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 88);
    // REQUIRE(S.number_of_rules(false) == 18);

    CongruenceByPairs<decltype(S)> p(right, S);
    p.add_pair({0, 1, 0, 0, 0, 1, 1, 0, 0}, {1, 0, 0, 0, 1});

    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 4);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 5);
    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 72);
    REQUIRE(p.number_of_classes() == 72);
    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "004",
                          "(cong) trivial congruence on finite semigroup",
                          "[quick][cong][cong-pair]") {
    auto                 rg   = ReportGuard(REPORT);
    std::vector<PPerm<>> gens = {PPerm<>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
                                 PPerm<>({0, 1, 2}, {0, 4, 3}, 5)};
    FroidurePin<PPerm<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 53);
    // REQUIRE(S.number_of_rules(false) == 20);

    CongruenceByPairs<decltype(S)> p(twosided, S);

    // Class indices are assigned starting at 0
    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 2);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 3);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 3);
    REQUIRE(p.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
    REQUIRE(p.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0}) != p.word_to_class_index({0, 0, 0}));
    REQUIRE(p.word_to_class_index({1, 1}) == p.word_to_class_index({1, 1, 1}));
    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 53);
    REQUIRE(p.number_of_classes() == 53);
    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "005",
                          "(cong) trivial left congruence on finite semigroup",
                          "[quick][cong][cong-pair]") {
    auto                 rg   = ReportGuard(REPORT);
    std::vector<PPerm<>> gens = {PPerm<>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
                                 PPerm<>({0, 1, 2}, {0, 4, 3}, 5)};
    FroidurePin<PPerm<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 53);
    // REQUIRE(S.number_of_rules(false) == 20);

    CongruenceByPairs<decltype(S)> p(left, S);

    // Class indices are assigned starting at 0
    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 2);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 3);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 3);
    REQUIRE(p.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
    REQUIRE(p.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0}) != p.word_to_class_index({0, 0, 0}));
    REQUIRE(p.word_to_class_index({1, 1}) == p.word_to_class_index({1, 1, 1}));
    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 53);
    REQUIRE(p.number_of_classes() == 53);

    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "006",
                          "(cong) trivial right congruence on finite semigroup",
                          "[quick][cong][cong-pair]") {
    auto                 rg   = ReportGuard(REPORT);
    std::vector<PPerm<>> gens = {PPerm<>({0, 1, 3, 4}, {1, 4, 0, 3}, 5),
                                 PPerm<>({0, 1, 2}, {0, 4, 3}, 5)};
    FroidurePin<PPerm<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 53);
    // REQUIRE(S.number_of_rules(false) == 20);

    CongruenceByPairs<decltype(S)> p(right, S);

    // Class indices are assigned starting at 0
    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 2);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 3);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 3);
    REQUIRE(p.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 4);
    REQUIRE(p.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0}) != p.word_to_class_index({0, 0, 0}));
    REQUIRE(p.word_to_class_index({1, 1}) == p.word_to_class_index({1, 1, 1}));
    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 53);
    REQUIRE(p.number_of_classes() == 53);

    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "007",
                          "(cong) universal congruence on finite semigroup",
                          "[quick][cong][cong-pair]") {
    auto                 rg   = ReportGuard(REPORT);
    std::vector<PPerm<>> gens = {PPerm<>({0, 1, 3}, {4, 1, 0}, 5),
                                 PPerm<>({0, 1, 2, 3, 4}, {0, 2, 4, 1, 3}, 5)};
    FroidurePin<PPerm<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 142);
    // REQUIRE(S.number_of_rules(false) == 32);

    CongruenceByPairs<decltype(S)> p(twosided, S);
    p.add_pair({1}, {0, 0, 0, 1, 0});

    // Class indices are assigned starting at 0
    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 0);
    REQUIRE(p.word_to_class_index({1, 0, 0, 1, 0, 0, 0}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 0, 0, 0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0}) == p.word_to_class_index({0, 0, 0}));
    REQUIRE(p.word_to_class_index({1, 1}) == p.word_to_class_index({1, 1, 1}));
    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 1);
    REQUIRE(p.number_of_classes() == 1);

    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "008",
                          "(cong) 2-sided congruence on finite semigroup",
                          "[standard][cong][cong-pair]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({7, 3, 5, 3, 4, 2, 7, 7}),
                                  Transf<>({1, 2, 4, 4, 7, 3, 0, 7}),
                                  Transf<>({0, 6, 4, 2, 2, 6, 6, 4}),
                                  Transf<>({3, 6, 3, 4, 0, 6, 0, 7})};
    FroidurePin<Transf<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 11804);
    // REQUIRE(S.number_of_rules(false) == 2460);

    CongruenceByPairs<decltype(S)> p(twosided, S);
    p.add_pair({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3});

    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 0}) == 0);
    REQUIRE(p.word_to_class_index({0, 0, 1, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({1, 1, 0, 1}) == 0);
    REQUIRE(p.word_to_class_index({1, 1, 0, 0}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 3}) == 2);

    REQUIRE(p.word_to_class_index({1, 2, 1, 3, 3, 2, 1, 2})
            == p.word_to_class_index({2, 1, 3, 3, 2, 1, 0}));
    REQUIRE(p.word_to_class_index({0, 3, 1, 1, 1, 3, 2, 2, 1, 0})
            == p.word_to_class_index({0, 3, 2, 2, 1}));
    REQUIRE(p.word_to_class_index({0, 3, 2, 1, 3, 3, 3})
            != p.word_to_class_index({0, 0, 3}));
    REQUIRE(p.word_to_class_index({1, 1, 0})
            != p.word_to_class_index({1, 3, 3, 2, 2, 1, 0}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 525);
    REQUIRE(p.number_of_classes() == 525);

    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "009",
                          "(cong) 2-sided congruence on finite semigroup",
                          "[quick][cong][cong-pair][no-valgrind]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({7, 3, 5, 3, 4, 2, 7, 7}),
                                  Transf<>({1, 2, 4, 4, 7, 3, 0, 7}),
                                  Transf<>({0, 6, 4, 2, 2, 6, 6, 4}),
                                  Transf<>({3, 6, 3, 4, 0, 6, 0, 7})};
    FroidurePin<Transf<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 11804);
    // REQUIRE(S.number_of_rules(false) == 2460);

    std::vector<relation_type> extra(
        {relation_type({1, 3, 0, 1, 2, 2, 0, 2}, {1, 0, 0, 1, 3, 1})});
    CongruenceByPairs<decltype(S)> p(twosided, S);
    p.add_pair({1, 3, 0, 1, 2, 2, 0, 2}, {1, 0, 0, 1, 3, 1});

    REQUIRE(p.word_to_class_index({0, 0, 0, 1}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 3}) == 2);
    REQUIRE(p.word_to_class_index({0, 1, 1, 2, 3}) == 0);

    REQUIRE(p.word_to_class_index({0, 1, 1, 2, 3})
            == p.word_to_class_index({1, 0, 3, 3, 3, 2, 0}));
    REQUIRE(p.word_to_class_index({3, 0, 2, 0, 2, 0, 2})
            == p.word_to_class_index({1, 2, 3, 1, 2}));
    REQUIRE(p.word_to_class_index({0, 3, 2, 1, 3, 3, 3})
            != p.word_to_class_index({0, 0, 3}));
    REQUIRE(p.word_to_class_index({1, 1, 0})
            != p.word_to_class_index({1, 3, 3, 2, 2, 1, 0}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 9597);
    REQUIRE(p.number_of_classes() == 9597);

    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "010",
                          "(cong) left congruence on big finite semigroup",
                          "[quick][cong][cong-pair][no-valgrind]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({7, 3, 5, 3, 4, 2, 7, 7}),
                                  Transf<>({1, 2, 4, 4, 7, 3, 0, 7}),
                                  Transf<>({0, 6, 4, 2, 2, 6, 6, 4}),
                                  Transf<>({3, 6, 3, 4, 0, 6, 0, 7})};
    FroidurePin<Transf<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that CongruenceByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size(false) == 11804);
    // REQUIRE(S.number_of_rules(false) == 2460);
    std::vector<relation_type>     extra({relation_type()});
    CongruenceByPairs<decltype(S)> p(left, S);
    p.add_pair({0, 3, 2, 1, 3, 2, 2}, {3, 2, 2, 1, 3, 3});

    REQUIRE(p.word_to_class_index({1, 1, 0, 3}) == 1);
    REQUIRE(p.word_to_class_index({0, 0, 3}) == 2);
    REQUIRE(p.word_to_class_index({2, 2, 0, 1}) == 0);

    REQUIRE(p.word_to_class_index({1, 1, 3, 2, 2, 1, 3, 1, 3, 3})
            == p.word_to_class_index({2, 2, 0, 1}));
    REQUIRE(p.word_to_class_index({2, 1, 3, 1, 2, 2, 1, 3, 3})
            == p.word_to_class_index({1, 2, 1, 2, 1, 3, 2, 2, 1, 3, 3}));
    REQUIRE(p.word_to_class_index({1, 1, 0, 3})
            != p.word_to_class_index({1, 0, 3, 2, 0, 2, 0, 3, 2, 2, 1}));
    REQUIRE(p.word_to_class_index({1, 3, 2, 1, 3, 1, 3, 2, 2, 1, 3, 3, 3})
            != p.word_to_class_index({3, 1, 0, 2, 0, 3, 1}));

    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.number_of_classes() == 7449);
    REQUIRE(p.number_of_classes() == 7449);

    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(p.parent_froidure_pin()->finished());
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "011",
                          "(cong) left congruence on TCE",
                          "[quick][cong][cong-pair]") {
    using detail::TCE;
    auto rg = ReportGuard(REPORT);

    ToddCoxeter tc(twosided);
    tc.set_number_of_generators(2);
    tc.add_pair({0, 0, 0}, {0});
    tc.add_pair({1, 1, 1, 1}, {1});
    tc.add_pair({0, 1, 0, 1}, {0, 0});
    // TODO(later)
    // This doesn't throw because the type of tc.quotient_froidure_pin() is
    // std::shared_ptr<FroidurePinBase> and so it isn't easy to check if
    // the element type is TCE.
    REQUIRE_NOTHROW(
        CongruenceByPairs<FroidurePinTCE>(left, tc.quotient_froidure_pin()));
    REQUIRE_NOTHROW(CongruenceByPairs<FroidurePinTCE>(
        twosided, tc.quotient_froidure_pin()));
    REQUIRE_NOTHROW(
        CongruenceByPairs<FroidurePinTCE>(right, tc.quotient_froidure_pin()));

    REQUIRE_THROWS_AS(
        CongruenceByPairs<FroidurePinTCE>(
            left, static_cast<FroidurePinTCE&>(*tc.quotient_froidure_pin())),
        LibsemigroupsException);
    REQUIRE_THROWS_AS(
        CongruenceByPairs<FroidurePinTCE>(
            twosided,
            static_cast<FroidurePinTCE&>(*tc.quotient_froidure_pin())),
        LibsemigroupsException);
    REQUIRE_NOTHROW(CongruenceByPairs<FroidurePinTCE>(
        right, static_cast<FroidurePinTCE&>(*tc.quotient_froidure_pin())));

    CongruenceByPairs<FroidurePinTCE> cong(
        right, static_cast<FroidurePinTCE&>(*tc.quotient_froidure_pin()));

    REQUIRE_THROWS_AS(cong.quotient_froidure_pin(), LibsemigroupsException);
    REQUIRE(cong.number_of_classes() == 27);
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "012",
                          "(cong) is_quotient_obviously_finite",
                          "[quick][cong][cong-pair]") {
    auto rg = ReportGuard(REPORT);

    ToddCoxeter tc(twosided);
    tc.set_number_of_generators(2);
    tc.add_pair({0, 0, 0}, {0});
    tc.add_pair({1, 1, 1, 1}, {1});
    tc.add_pair({0, 1, 0, 1}, {0, 0});
    REQUIRE(tc.quotient_froidure_pin()->size() == 27);

    using detail::TCE;
    CongruenceByPairs<FroidurePinTCE> cong(right, tc.quotient_froidure_pin());
    REQUIRE(!cong.finished());
    REQUIRE(cong.has_parent_froidure_pin());
    REQUIRE(cong.parent_froidure_pin()->finished());
    REQUIRE(cong.is_quotient_obviously_finite());
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "013",
                          "(cong) class_index_to_word/quotient",
                          "[quick][cong][cong-pair][no-valgrind]") {
    auto                  rg   = ReportGuard(REPORT);
    std::vector<Transf<>> gens = {Transf<>({5, 3, 5, 3, 4, 2}),
                                  Transf<>({1, 2, 4, 4, 5, 3}),
                                  Transf<>({0, 5, 4, 2, 2, 4})};
    FroidurePin<Transf<>> S(gens);

    REQUIRE(S.size() == 321);

    using P = CongruenceByPairs<decltype(S)>;
    std::unique_ptr<P> cong;

    SECTION("right congruence") {
      cong = std::make_unique<P>(right, S);
      cong->add_pair({0}, {1});
      REQUIRE(cong->number_of_classes() == 168);
      REQUIRE(cong->word_to_class_index(word_type({0})) == 0);
      REQUIRE(cong->word_to_class_index(word_type({1})) == 0);
      REQUIRE(cong->class_index_to_word(0) == word_type({0}));

      REQUIRE(cong->word_to_class_index(word_type({0, 0})) == 1);
      REQUIRE(cong->class_index_to_word(1) == word_type({0, 0}));

      REQUIRE(cong->word_to_class_index(word_type({0, 1})) == 2);
      REQUIRE(cong->class_index_to_word(2) == word_type({0, 1}));

      REQUIRE(cong->class_index_to_word(3) == word_type({0, 2}));
      REQUIRE(cong->word_to_class_index(word_type({0, 2})) == 3);
    }
    SECTION("left congruence") {
      cong = std::make_unique<P>(left, S);
      cong->add_pair({0}, {1});
      REQUIRE(cong->number_of_classes() == 24);
    }
    SECTION("2-sided congruence") {
      cong = std::make_unique<P>(twosided, S);
      cong->add_pair({0}, {1});
      REQUIRE(cong->number_of_classes() == 4);
    }

    for (size_t i = 0; i < cong->number_of_classes(); ++i) {
      auto w = cong->class_index_to_word(i);
      REQUIRE(cong->word_to_class_index(w) == i);
    }

    REQUIRE_THROWS_AS(cong->class_index_to_word(cong->number_of_classes() + 1),
                      LibsemigroupsException);

    if (cong->kind() != twosided) {
      REQUIRE_THROWS_AS(cong->quotient_froidure_pin(), LibsemigroupsException);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "014",
                          "(cong) const_word_to_class_index",
                          "[quick][cong][cong-pair]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S({Transf<>({7, 3, 5, 3, 4, 2, 7, 7}),
                             Transf<>({1, 2, 4, 4, 7, 3, 0, 7}),
                             Transf<>({0, 6, 4, 2, 2, 6, 6, 4}),
                             Transf<>({3, 6, 3, 4, 0, 6, 0, 7})});

    using P = CongruenceByPairs<decltype(S)>;
    std::unique_ptr<P> cong;

    SECTION("right congruence") {
      cong = std::make_unique<P>(right, S);
    }
    SECTION("left congruence") {
      cong = std::make_unique<P>(left, S);
    }
    SECTION("2-sided congruence") {
      cong = std::make_unique<P>(twosided, S);
    }
    cong->add_pair({0}, {1});
    REQUIRE(!cong->finished());
    REQUIRE(cong->const_contains({0}, {1}) == tril::unknown);
  }

  LIBSEMIGROUPS_TEST_CASE("CongruenceByPairs",
                          "015",
                          "(cong) size non-Element*",
                          "[quick][cong][cong-pair]") {
    auto                  rg = ReportGuard(REPORT);
    FroidurePin<Transf<>> S({Transf<>({7, 3, 5, 3, 4, 2, 7, 7}),
                             Transf<>({1, 2, 4, 4, 7, 3, 0, 7}),
                             Transf<>({0, 6, 4, 2, 2, 6, 6, 4}),
                             Transf<>({3, 6, 3, 4, 0, 6, 0, 7})});

    using P = CongruenceByPairs<decltype(S)>;

    P cong1(right, S);
    REQUIRE(cong1.number_of_classes() == 11804);
    P cong2(left, S);
    REQUIRE(cong2.number_of_classes() == 11804);
    P cong3(twosided, S);
    REQUIRE(cong3.number_of_classes() == 11804);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "016",
                          "non-trivial congruence on an infinite fp semigroup",
                          "[quick][kbp][cong-pair]") {
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(3);
    kb.add_rule({0, 1}, {1, 0});
    kb.add_rule({0, 2}, {2, 0});
    kb.add_rule({0, 0}, {0});
    kb.add_rule({0, 2}, {0});
    kb.add_rule({2, 0}, {0});
    kb.add_rule({1, 2}, {2, 1});
    kb.add_rule({1, 1, 1}, {1});
    kb.add_rule({1, 2}, {1});
    kb.add_rule({2, 1}, {1});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);
    kbp.add_pair({0}, {1});

    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1}));
    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1, 0}));
    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1, 1}));
    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1, 0, 1}));

    REQUIRE(kbp.number_of_non_trivial_classes() == 1);
    REQUIRE(kbp.cbegin_ntc()->size() == 5);
    REQUIRE(std::vector<word_type>(kbp.cbegin_ntc()->cbegin(),
                                   kbp.cbegin_ntc()->cend())
            == std::vector<word_type>({{0}, {1}, {0, 1}, {1, 1}, {0, 1, 1}}));
    REQUIRE_NOTHROW(kbp.quotient_froidure_pin());
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "017",
                          "non-trivial congruence on an infinite fp semigroup",
                          "[quick][kbp][cong-pair]") {
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(4);
    kb.add_rule({0, 1}, {1, 0});
    kb.add_rule({0, 2}, {2, 0});
    kb.add_rule({0, 0}, {0});
    kb.add_rule({0, 2}, {0});
    kb.add_rule({2, 0}, {0});
    kb.add_rule({1, 2}, {2, 1});
    kb.add_rule({1, 1, 1}, {1});
    kb.add_rule({1, 2}, {1});
    kb.add_rule({2, 1}, {1});
    kb.add_rule({0, 3}, {0});
    kb.add_rule({3, 0}, {0});
    kb.add_rule({1, 3}, {1});
    kb.add_rule({3, 1}, {1});
    kb.add_rule({2, 3}, {2});
    kb.add_rule({3, 2}, {2});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);
    kbp.add_pair({0}, {1});

    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1}));
    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1, 0}));
    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1, 1}));
    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({1, 0, 1}));

    REQUIRE(kbp.number_of_non_trivial_classes() == 1);
    REQUIRE(kbp.cbegin_ntc()->size() == 5);
    REQUIRE(std::vector<word_type>(kbp.cbegin_ntc()->cbegin(),
                                   kbp.cbegin_ntc()->cend())
            == std::vector<word_type>({{0}, {1}, {0, 1}, {1, 1}, {0, 1, 1}}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "018",
                          "non-trivial congruence on an infinite fp semigroup",
                          "[quick][kbp][cong-pair]") {
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(5);
    kb.add_rule({0, 1}, {0});
    kb.add_rule({1, 0}, {0});
    kb.add_rule({0, 2}, {0});
    kb.add_rule({2, 0}, {0});
    kb.add_rule({0, 3}, {0});
    kb.add_rule({3, 0}, {0});
    kb.add_rule({0, 0}, {0});
    kb.add_rule({1, 1}, {0});
    kb.add_rule({2, 2}, {0});
    kb.add_rule({3, 3}, {0});
    kb.add_rule({1, 2}, {0});
    kb.add_rule({2, 1}, {0});
    kb.add_rule({1, 3}, {0});
    kb.add_rule({3, 1}, {0});
    kb.add_rule({2, 3}, {0});
    kb.add_rule({3, 2}, {0});
    kb.add_rule({4, 0}, {0});
    kb.add_rule({4, 1}, {1});
    kb.add_rule({4, 2}, {2});
    kb.add_rule({4, 3}, {3});
    kb.add_rule({0, 4}, {0});
    kb.add_rule({1, 4}, {1});
    kb.add_rule({2, 4}, {2});
    kb.add_rule({3, 4}, {3});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);
    kbp.add_pair({1}, {2});

    REQUIRE(kbp.word_to_class_index({1}) == kbp.word_to_class_index({2}));

    REQUIRE(kbp.number_of_non_trivial_classes() == 1);
    REQUIRE(kbp.cbegin_ntc()->size() == 2);
    REQUIRE(std::vector<word_type>(kbp.cbegin_ntc()->cbegin(),
                                   kbp.cbegin_ntc()->cend())
            == std::vector<word_type>({{1}, {2}}));

    REQUIRE(kbp.word_to_class_index({1}) == kbp.word_to_class_index({2}));
    REQUIRE(kbp.is_quotient_obviously_finite());
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "019",
                          "non-trivial congruence on an infinite fp semigroup",
                          "[quick][kbp][cong-pair]") {
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(5);
    kb.add_rule({0, 1}, {0});
    kb.add_rule({1, 0}, {0});
    kb.add_rule({0, 2}, {0});
    kb.add_rule({2, 0}, {0});
    kb.add_rule({0, 3}, {0});
    kb.add_rule({3, 0}, {0});
    kb.add_rule({0, 0}, {0});
    kb.add_rule({1, 1}, {0});
    kb.add_rule({2, 2}, {0});
    kb.add_rule({3, 3}, {0});
    kb.add_rule({1, 2}, {0});
    kb.add_rule({2, 1}, {0});
    kb.add_rule({1, 3}, {0});
    kb.add_rule({3, 1}, {0});
    kb.add_rule({2, 3}, {0});
    kb.add_rule({3, 2}, {0});
    kb.add_rule({4, 0}, {0});
    kb.add_rule({4, 1}, {2});
    kb.add_rule({4, 2}, {3});
    kb.add_rule({4, 3}, {1});
    kb.add_rule({0, 4}, {0});
    kb.add_rule({1, 4}, {2});
    kb.add_rule({2, 4}, {3});
    kb.add_rule({3, 4}, {1});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);
    kbp.add_pair({2}, {3});

    REQUIRE(kbp.word_to_class_index({3}) == kbp.word_to_class_index({2}));

    REQUIRE(kbp.number_of_non_trivial_classes() == 1);
    REQUIRE(kbp.cbegin_ntc()->size() == 3);
    REQUIRE(std::vector<word_type>(kbp.cbegin_ntc()->cbegin(),
                                   kbp.cbegin_ntc()->cend())
            == std::vector<word_type>({{2}, {3}, {1}}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "020",
                          "trivial congruence on a finite fp semigroup",
                          "[quick][kbp][cong-pair]") {
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(2);
    kb.add_rule({0, 0, 1}, {0, 0});
    kb.add_rule({0, 0, 0, 0}, {0, 0});
    kb.add_rule({0, 1, 1, 0}, {0, 0});
    kb.add_rule({0, 1, 1, 1}, {0, 0, 0});
    kb.add_rule({1, 1, 1, 0}, {1, 1, 0});
    kb.add_rule({1, 1, 1, 1}, {1, 1, 1});
    kb.add_rule({0, 1, 0, 0, 0}, {0, 1, 0, 1});
    kb.add_rule({0, 1, 0, 1, 0}, {0, 1, 0, 0});
    kb.add_rule({0, 1, 0, 1, 1}, {0, 1, 0, 1});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);

    REQUIRE(kbp.number_of_classes() == 27);
    REQUIRE(kbp.word_to_class_index({0}) == 0);

    REQUIRE(kbp.word_to_class_index({0, 0, 0, 0}) == 1);
    REQUIRE(kbp.word_to_class_index({0}) == 0);
    REQUIRE(kbp.word_to_class_index({1, 0, 1}) == 2);
    REQUIRE(kbp.word_to_class_index({0, 1, 1, 0}) == 1);

    REQUIRE(kbp.number_of_non_trivial_classes() == 0);
    REQUIRE(kbp.cbegin_ntc() == kbp.cend_ntc());
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "021",
                          "universal congruence on a finite fp semigroup",
                          "[quick][kbp][cong-pair]") {
    auto rg = ReportGuard(REPORT);

    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(2);
    kb.add_rule({0, 0, 1}, {0, 0});
    kb.add_rule({0, 0, 0, 0}, {0, 0});
    kb.add_rule({0, 1, 1, 0}, {0, 0});
    kb.add_rule({0, 1, 1, 1}, {0, 0, 0});
    kb.add_rule({1, 1, 1, 0}, {1, 1, 0});
    kb.add_rule({1, 1, 1, 1}, {1, 1, 1});
    kb.add_rule({0, 1, 0, 0, 0}, {0, 1, 0, 1});
    kb.add_rule({0, 1, 0, 1, 0}, {0, 1, 0, 0});
    kb.add_rule({0, 1, 0, 1, 1}, {0, 1, 0, 1});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);
    kbp.add_pair({0}, {1});
    kbp.add_pair({0, 0}, {0});

    REQUIRE(kbp.number_of_classes() == 1);

    REQUIRE(kbp.cbegin_ntc()->size() == 27);
    REQUIRE(kb.size() == 27);
    REQUIRE(std::vector<word_type>(kbp.cbegin_ntc()->cbegin(),
                                   kbp.cbegin_ntc()->cend())
            == std::vector<word_type>({{0},
                                       {1},
                                       {0, 0},
                                       {0, 1},
                                       {1, 0},
                                       {1, 1},
                                       {0, 0, 0},
                                       {1, 0, 0},
                                       {0, 1, 0},
                                       {1, 0, 1},
                                       {0, 1, 1},
                                       {1, 1, 0},
                                       {1, 1, 1},
                                       {1, 0, 0, 0},
                                       {0, 1, 0, 0},
                                       {1, 1, 0, 0},
                                       {1, 0, 1, 0},
                                       {0, 1, 0, 1},
                                       {1, 1, 0, 1},
                                       {1, 0, 1, 1},
                                       {1, 1, 0, 0, 0},
                                       {1, 0, 1, 0, 0},
                                       {1, 1, 0, 1, 0},
                                       {1, 0, 1, 0, 1},
                                       {1, 1, 0, 1, 1},
                                       {1, 1, 0, 1, 0, 0},
                                       {1, 1, 0, 1, 0, 1}}));

    REQUIRE(kbp.number_of_non_trivial_classes() == 1);

    REQUIRE(kbp.word_to_class_index({0, 0, 0, 0}) == 0);
    REQUIRE(kbp.word_to_class_index({0}) == 0);
    REQUIRE(kbp.word_to_class_index({1, 0, 1}) == 0);
    REQUIRE(kbp.word_to_class_index({0, 1, 1, 0}) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "022",
                          "left congruence on a finite fp semigroup",
                          "[quick][kbp][cong-pair]") {
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(2);
    kb.add_rule({0, 0, 1}, {0, 0});
    kb.add_rule({0, 0, 0, 0}, {0, 0});
    kb.add_rule({0, 1, 1, 0}, {0, 0});
    kb.add_rule({0, 1, 1, 1}, {0, 0, 0});
    kb.add_rule({1, 1, 1, 0}, {1, 1, 0});
    kb.add_rule({1, 1, 1, 1}, {1, 1, 1});
    kb.add_rule({0, 1, 0, 0, 0}, {0, 1, 0, 1});
    kb.add_rule({0, 1, 0, 1, 0}, {0, 1, 0, 0});
    kb.add_rule({0, 1, 0, 1, 1}, {0, 1, 0, 1});

    KnuthBendixCongruenceByPairs kbp(left, kb);
    kbp.add_pair({0}, {1});
    kbp.add_pair({0, 0}, {0});

    REQUIRE(kbp.number_of_non_trivial_classes() == 6);

    std::vector<size_t> v(kbp.number_of_non_trivial_classes(), 0);
    std::transform(kbp.cbegin_ntc(),
                   kbp.cend_ntc(),
                   v.begin(),
                   std::mem_fn(&std::vector<word_type>::size));
    std::sort(v.begin(), v.end());
    REQUIRE(v == std::vector<size_t>({4, 4, 4, 5, 5, 5}));

    REQUIRE(
        std::vector<std::vector<word_type>>(kbp.cbegin_ntc(), kbp.cend_ntc())
        == std::vector<std::vector<word_type>>(
            {{{0}, {1}, {0, 0}, {0, 1}, {0, 0, 0}},
             {{1, 0}, {1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 0, 0, 0}},
             {{0, 1, 0}, {0, 1, 1}, {0, 1, 0, 0}, {0, 1, 0, 1}},
             {{1, 1, 0},
              {1, 1, 1},
              {1, 1, 0, 0},
              {1, 1, 0, 1},
              {1, 1, 0, 0, 0}},
             {{1, 0, 1, 0}, {1, 0, 1, 1}, {1, 0, 1, 0, 0}, {1, 0, 1, 0, 1}},
             {{1, 1, 0, 1, 0},
              {1, 1, 0, 1, 1},
              {1, 1, 0, 1, 0, 0},
              {1, 1, 0, 1, 0, 1}}}));

    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({0, 0, 0}));
    REQUIRE(kbp.word_to_class_index({1, 0, 1, 1})
            == kbp.word_to_class_index({1, 0, 1, 0, 1}));
    REQUIRE(kbp.word_to_class_index({1, 1, 0, 0})
            != kbp.word_to_class_index({0, 1}));
    REQUIRE(kbp.word_to_class_index({1, 0, 1, 0})
            != kbp.word_to_class_index({1, 1, 0, 1, 0, 1}));

    REQUIRE(kbp.word_to_class_index({1, 0, 1}) == 1);
    REQUIRE(kbp.word_to_class_index({0}) == 0);
    REQUIRE(kbp.word_to_class_index({0, 1, 1, 0}) == 0);

    REQUIRE(kbp.number_of_classes() == 6);
  }

  // KnuthBendixCongruenceByPairs 07 only really tests
  // fpsemigroup::KnuthBendix
  LIBSEMIGROUPS_TEST_CASE(
      "KnuthBendixCongruenceByPairs",
      "023",
      "finite group, Chapter 11, Theorem 1.9, H, q = 4 in NR ",
      "[quick][kbp][cong-pair][no-valgrind]") {
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(4);
    kb.add_rule({0, 0}, {0});
    kb.add_rule({0, 1}, {1});
    kb.add_rule({1, 0}, {1});
    kb.add_rule({0, 2}, {2});
    kb.add_rule({2, 0}, {2});
    kb.add_rule({0, 3}, {3});
    kb.add_rule({3, 0}, {3});
    kb.add_rule({2, 3}, {0});
    kb.add_rule({3, 2}, {0});
    kb.add_rule({1, 1}, {0});
    kb.add_rule({2, 2, 2, 2}, {0});
    kb.add_rule({1, 2, 1, 3, 1, 3, 1, 2, 1, 3, 1, 2}, {0});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);
    REQUIRE(kbp.number_of_classes() == 120);
    REQUIRE(kbp.number_of_non_trivial_classes() == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "024",
                          "right congruence on infinite fp semigroup",
                          "[quick][kbp][cong-pair]") {
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(3);
    kb.add_rule({1, 1, 1, 1, 1, 1, 1}, {1});
    kb.add_rule({2, 2, 2, 2, 2}, {2});
    kb.add_rule({1, 2, 2, 1, 0}, {1, 2, 2, 1});
    kb.add_rule({1, 2, 2, 1, 2}, {1, 2, 2, 1});
    kb.add_rule({1, 1, 2, 1, 2, 0}, {1, 1, 2, 1, 2});
    kb.add_rule({1, 1, 2, 1, 2, 1}, {1, 1, 2, 1, 2});

    KnuthBendixCongruenceByPairs kbp(right, kb);
    kbp.add_pair({1, 2, 2, 1}, {1, 1, 2, 1, 2});

    // Generating pair
    REQUIRE(kbp.word_to_class_index({1, 2, 2, 1})
            == kbp.word_to_class_index({1, 1, 2, 1, 2}));

    REQUIRE(kbp.number_of_non_trivial_classes() == 1);
    REQUIRE(std::vector<word_type>(kbp.cbegin_ntc()->begin(),
                                   kbp.cbegin_ntc()->end())
            == std::vector<word_type>({{1, 2, 2, 1}, {1, 1, 2, 1, 2}}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "025",
                          "finite fp semigroup, dihedral group of order 6",
                          "[quick][kbp][cong-pair]") {
    using detail::KBE;
    auto                     rg = ReportGuard(REPORT);
    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(5);
    kb.add_rule({0, 0}, {0});
    kb.add_rule({0, 1}, {1});
    kb.add_rule({1, 0}, {1});
    kb.add_rule({0, 2}, {2});
    kb.add_rule({2, 0}, {2});
    kb.add_rule({0, 3}, {3});
    kb.add_rule({3, 0}, {3});
    kb.add_rule({0, 4}, {4});
    kb.add_rule({4, 0}, {4});
    kb.add_rule({1, 2}, {0});
    kb.add_rule({2, 1}, {0});
    kb.add_rule({3, 4}, {0});
    kb.add_rule({4, 3}, {0});
    kb.add_rule({2, 2}, {0});
    kb.add_rule({1, 4, 2, 3, 3}, {0});
    kb.add_rule({4, 4, 4}, {0});

    auto fp = static_cast<
        FroidurePin<KBE, FroidurePinTraits<KBE, fpsemigroup::KnuthBendix>>*>(
        kb.froidure_pin().get());
    REQUIRE(fp->size() == 6);

    std::vector<detail::KBE> expected
        = {KBE(kb, kb.alphabet(0)),
           KBE(kb, kb.alphabet(1)),
           KBE(kb, kb.alphabet(3)),
           KBE(kb, kb.alphabet(4)),
           KBE(kb, kb.alphabet(1) + kb.alphabet(3)),
           KBE(kb, kb.alphabet(1) + kb.alphabet(4))};
    auto result = std::vector<detail::KBE>(fp->cbegin(), fp->cend());

    REQUIRE(result == expected);

    KnuthBendixCongruenceByPairs kbp(twosided, kb);

    REQUIRE(kbp.number_of_classes() == 6);
    REQUIRE(kbp.number_of_non_trivial_classes() == 0);
    REQUIRE(kbp.word_to_class_index({1}) == kbp.word_to_class_index({2}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "026",
                          "finite fp semigroup, size 16",
                          "[quick][kbp][cong-pair]") {
    auto rg = ReportGuard(REPORT);

    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(4);
    kb.add_rule({3}, {2});
    kb.add_rule({0, 3}, {0, 2});
    kb.add_rule({1, 1}, {1});
    kb.add_rule({1, 3}, {1, 2});
    kb.add_rule({2, 1}, {2});
    kb.add_rule({2, 2}, {2});
    kb.add_rule({2, 3}, {2});
    kb.add_rule({0, 0, 0}, {0});
    kb.add_rule({0, 0, 1}, {1});
    kb.add_rule({0, 0, 2}, {2});
    kb.add_rule({0, 1, 2}, {1, 2});
    kb.add_rule({1, 0, 0}, {1});
    kb.add_rule({1, 0, 2}, {0, 2});
    kb.add_rule({2, 0, 0}, {2});
    kb.add_rule({0, 1, 0, 1}, {1, 0, 1});
    kb.add_rule({0, 2, 0, 2}, {2, 0, 2});
    kb.add_rule({1, 0, 1, 0}, {1, 0, 1});
    kb.add_rule({1, 2, 0, 1}, {1, 0, 1});
    kb.add_rule({1, 2, 0, 2}, {2, 0, 2});
    kb.add_rule({2, 0, 1, 0}, {2, 0, 1});
    kb.add_rule({2, 0, 2, 0}, {2, 0, 2});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);
    kbp.add_pair({2}, {3});

    REQUIRE(kbp.number_of_classes() == 16);
    REQUIRE(kbp.number_of_non_trivial_classes() == 0);
    REQUIRE(kbp.word_to_class_index({2}) == kbp.word_to_class_index({3}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "027",
                          "finite fp semigroup, size 16",
                          "[quick][kbp][cong-pair]") {
    auto rg = ReportGuard(REPORT);

    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(11);
    kb.add_rule({2}, {1});
    kb.add_rule({4}, {3});
    kb.add_rule({5}, {0});
    kb.add_rule({6}, {3});
    kb.add_rule({7}, {1});
    kb.add_rule({8}, {3});
    kb.add_rule({9}, {3});
    kb.add_rule({10}, {0});
    kb.add_rule({0, 2}, {0, 1});
    kb.add_rule({0, 4}, {0, 3});
    kb.add_rule({0, 5}, {0, 0});
    kb.add_rule({0, 6}, {0, 3});
    kb.add_rule({0, 7}, {0, 1});
    kb.add_rule({0, 8}, {0, 3});
    kb.add_rule({0, 9}, {0, 3});
    kb.add_rule({0, 10}, {0, 0});
    kb.add_rule({1, 1}, {1});
    kb.add_rule({1, 2}, {1});
    kb.add_rule({1, 4}, {1, 3});
    kb.add_rule({1, 5}, {1, 0});
    kb.add_rule({1, 6}, {1, 3});
    kb.add_rule({1, 7}, {1});
    kb.add_rule({1, 8}, {1, 3});
    kb.add_rule({1, 9}, {1, 3});
    kb.add_rule({1, 10}, {1, 0});
    kb.add_rule({3, 1}, {3});
    kb.add_rule({3, 2}, {3});
    kb.add_rule({3, 3}, {3});
    kb.add_rule({3, 4}, {3});
    kb.add_rule({3, 5}, {3, 0});
    kb.add_rule({3, 6}, {3});
    kb.add_rule({3, 7}, {3});
    kb.add_rule({3, 8}, {3});
    kb.add_rule({3, 9}, {3});
    kb.add_rule({3, 10}, {3, 0});
    kb.add_rule({0, 0, 0}, {0});
    kb.add_rule({0, 0, 1}, {1});
    kb.add_rule({0, 0, 3}, {3});
    kb.add_rule({0, 1, 3}, {1, 3});
    kb.add_rule({1, 0, 0}, {1});
    kb.add_rule({1, 0, 3}, {0, 3});
    kb.add_rule({3, 0, 0}, {3});
    kb.add_rule({0, 1, 0, 1}, {1, 0, 1});
    kb.add_rule({0, 3, 0, 3}, {3, 0, 3});
    kb.add_rule({1, 0, 1, 0}, {1, 0, 1});
    kb.add_rule({1, 3, 0, 1}, {1, 0, 1});
    kb.add_rule({1, 3, 0, 3}, {3, 0, 3});
    kb.add_rule({3, 0, 1, 0}, {3, 0, 1});
    kb.add_rule({3, 0, 3, 0}, {3, 0, 3});

    KnuthBendixCongruenceByPairs kbp(twosided, kb);
    kbp.add_pair({1}, {3});

    REQUIRE(kbp.number_of_classes() == 3);
    REQUIRE(kbp.number_of_non_trivial_classes() == 1);
    REQUIRE(std::vector<word_type>(kbp.cbegin_ntc()->begin(),
                                   kbp.cbegin_ntc()->end())
            == std::vector<word_type>({{1},
                                       {3},
                                       {0, 1},
                                       {0, 3},
                                       {1, 0},
                                       {3, 0},
                                       {1, 3},
                                       {0, 1, 0},
                                       {0, 3, 0},
                                       {1, 0, 1},
                                       {3, 0, 1},
                                       {3, 0, 3},
                                       {1, 3, 0},
                                       {0, 3, 0, 1}}));
    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({5}));
    REQUIRE(kbp.word_to_class_index({0}) == kbp.word_to_class_index({10}));
    REQUIRE(kbp.word_to_class_index({1}) == kbp.word_to_class_index({2}));
    REQUIRE(kbp.word_to_class_index({1}) == kbp.word_to_class_index({7}));
    REQUIRE(kbp.word_to_class_index({3}) == kbp.word_to_class_index({4}));
    REQUIRE(kbp.word_to_class_index({3}) == kbp.word_to_class_index({6}));
    REQUIRE(kbp.word_to_class_index({3}) == kbp.word_to_class_index({8}));
    REQUIRE(kbp.word_to_class_index({3}) == kbp.word_to_class_index({9}));
  }

  LIBSEMIGROUPS_TEST_CASE("KnuthBendixCongruenceByPairs",
                          "028",
                          "infinite fp semigroup with infinite classes",
                          "[quick][kbp][cong-pair]") {
    auto rg = ReportGuard(REPORT);

    fpsemigroup::KnuthBendix kb;
    kb.set_alphabet(2);
    kb.add_rule({0, 0, 0}, {0});
    kb.add_rule({0, 1}, {1, 0});
    kb.add_rule({0}, {0, 0});
    KnuthBendixCongruenceByPairs kbp(twosided, kb);

    word_type x
        = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    word_type y
        = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    REQUIRE(kbp.contains(x, y));
    REQUIRE(kbp.contains({0, 0}, {0}));
    REQUIRE(!kbp.contains({1}, {0}));
    REQUIRE(kbp.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroupByPairs",
                          "029",
                          "(fpsemi) 2-sided congruence on finite semigroup",
                          "[quick][fpsemi][cong-pair]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Transf<>> gens
        = {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})};
    FroidurePin<Transf<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that FpSemigroupByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size() == 88);
    // REQUIRE(S.number_of_rules() == 18);

    FpSemigroupByPairs<decltype(S)> p(S);
    p.add_rule(word_type({0, 1, 0, 0, 0, 1, 1, 0, 0}),
               word_type({1, 0, 0, 0, 1}));

    REQUIRE(p.equal_to(word_type({0, 0, 0, 1}), word_type({0, 0, 1, 0, 0})));
    // REQUIRE(p.finished());
    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.size() == 21);
    REQUIRE(p.size() == 21);

    // number_of_classes requires p.parent_froidure_pin().size();
    REQUIRE(!S.started());  // p copies S
    REQUIRE(!S.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("FpSemigroupByPairs",
                          "030",
                          "(fpsemi) 2-sided congruence on finite semigroup",
                          "[quick][fpsemi][cong-pair]") {
    auto                  rg = ReportGuard(REPORT);
    std::vector<Transf<>> gens
        = {Transf<>({1, 3, 4, 2, 3}), Transf<>({3, 2, 1, 3, 3})};
    FroidurePin<Transf<>> S(gens);

    // The following lines are intentionally commented out so that we can
    // check that FpSemigroupByPairs does not enumerate the semigroup, they
    // remain to remind us of the size and number of rules of the semigroups.
    // REQUIRE(S.size() == 88);
    // REQUIRE(S.number_of_rules() == 18);

    FpSemigroupByPairs<decltype(S)> p(S);
    p.add_rule(word_type({0, 1, 0, 0, 0, 1, 1, 0, 0}),
               word_type({1, 0, 0, 0, 1}));

    REQUIRE(p.equal_to(word_type({0, 0, 0, 1}), word_type({0, 0, 1, 0, 0})));
    REQUIRE(!p.finished());
    REQUIRE(!S.started());
    REQUIRE(!S.finished());

    REQUIRE(p.size() == 21);
    REQUIRE(p.size() == 21);
    REQUIRE(!S.finished());  // S is copied into p
  }

  // This test is commented out because it does not and should not compile,
  // the FpSemigroupByPairs class requires a base semigroup over which to
  // compute, in the example below there is no such base semigroup.
  //
  // LIBSEMIGROUPS_TEST_CASE("FpSemigroupByPairs", "031",
  //                         "infinite fp semigroup from GAP library ",
  //                         "[quick][fpsemi][cong-pair]") {
  //   auto rg = ReportGuard(REPORT);
  //   FpSemigroupByPairs<decltype(S)::element_type> p;
  //   p.set_alphabet(2);
  //   p.add_rule(word_type({0, 0}), word_type({0, 0}));
  //   p.add_rule(word_type({0, 1}), {1, 0});
  //   p.add_rule(word_type({0, 2}), {2, 0});
  //   p.add_rule(word_type({0, 0}), {0});
  //   p.add_rule(word_type({0, 2}), {0});
  //   p.add_rule(word_type({2, 0}), {0});
  //   p.add_rule(word_type({1, 0}), {0, 1});
  //   p.add_rule(word_type({1, 1}), {1, 1});
  //   p.add_rule(word_type({1, 2}), {2, 1});
  //   p.add_rule(word_type({1, 1, 1}), {1});
  //   p.add_rule(word_type({1, 2}), word_type({1}));
  //   p.add_rule(word_type({2, 1}), word_type({1}));
  //   p.add_rule(word_type({0}), word_type({1}));

  //   REQUIRE(!p.finished());
  // }
}  // namespace libsemigroups
