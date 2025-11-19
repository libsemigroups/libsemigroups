// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2025 James D. Mitchell + Maria Tsalakou
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

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "libsemigroups/libsemigroups.hpp"  // for *
#include "test-main.hpp"                    // for LIBSEMIGROUPS_TEST_CASE

using namespace libsemigroups::literals;

namespace libsemigroups {
  // action.hpp: Line 60
  LIBSEMIGROUPS_TEST_CASE("docs", "000", "action.hpp", "[docs][quick]") {
    RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>> o;
    o.add_seed(PPerm<16>::one(16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.add_generator(
        PPerm<16>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.reserve(70000);
    REQUIRE(o.size() == 65536);
  }

  // action.hpp: Line 143
  LIBSEMIGROUPS_TEST_CASE("docs", "001", "action.hpp", "[docs][quick]") {
    auto rg = ReportGuard(true);
    RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>> o;
    o.add_seed(PPerm<16>::one(16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.add_generator(
        PPerm<16>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  16));
    o.add_generator(
        PPerm<16>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14},
                  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
                  16));
    o.reserve(70000);
    REQUIRE(o.size() == 65536);
    REQUIRE(o.scc().number_of_components() == 17);
  }

  // cong-class.hpp: Line 148
  LIBSEMIGROUPS_TEST_CASE("docs", "002", "cong-class.hpp", "[docs][quick]") {
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    presentation::add_rule(p, {0, 1}, {});

    Congruence cong(congruence_kind::twosided, p);
    is_obviously_infinite(cong);  // true
    congruence_common::add_generating_pair(cong, {0, 0, 0}, {});
    REQUIRE(cong.number_of_classes() == 3);
  }

  // freeband.hpp: Line 76
  LIBSEMIGROUPS_TEST_CASE("docs", "003", "freeband.hpp", "[docs][quick]") {
    freeband_equal_to({0, 1, 2, 3, 2, 1, 0},
                      {0, 1, 2, 3, 2, 3, 2, 1, 0});               // true
    freeband_equal_to({1, 2, 3}, {0, 1, 2});                      // false
    freeband_equal_to({1, 4, 2, 3, 10}, {1, 4, 1, 4, 2, 3, 10});  // true
    freeband_equal_to({0, 1, 2, 3, 4, 0, 1, 2, 3, 4},
                      {4, 3, 2, 1, 0, 4, 3, 2, 1, 0});    // false
    freeband_equal_to({0, 1, 2, 1, 0, 1, 2}, {0, 1, 2});  // true
    freeband_equal_to({0, 1, 2, 3, 0, 1},
                      {0, 1, 2, 3, 3, 2, 2, 1, 0, 2, 1, 0, 2, 3,
                       0, 2, 1, 3, 2, 1, 2, 3, 2, 1, 0, 2, 0, 1,
                       0, 2, 0, 3, 2, 0, 1, 2, 2, 3, 0, 1});  // true
  }

  // froidure-pin-base.hpp: Line 1337
  LIBSEMIGROUPS_TEST_CASE("docs",
                          "004",
                          "froidure-pin-base.hpp",
                          "[docs][quick]") {
    FroidurePin<BMat8> S;
    S.add_generator(
        BMat8({{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}}));
    S.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}));
    S.add_generator(
        BMat8({{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}}));
    S.add_generator(
        BMat8({{0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}}));
    REQUIRE(S.size() == 4);
    std::vector<relation_type>(S.cbegin_rules(), S.cend_rules());
    // {{{0, 0}, {0}},
    //  {{0, 1}, {1}},
    //  {{0, 2}, {2}},
    //  {{0, 3}, {3}},
    //  {{1, 0}, {0}},
    //  {{1, 1}, {1}},
    //  {{1, 2}, {2}},
    //  {{1, 3}, {3}},
    //  {{2, 0}, {0}},
    //  {{2, 1}, {1}},
    //  {{2, 2}, {2}},
    //  {{2, 3}, {3}},
    //  {{3, 0}, {0}},
    //  {{3, 1}, {1}},
    //  {{3, 2}, {2}},
    //  {{3, 3}, {3}}}
  }

  // froidure-pin-base.hpp: Line 1441
  LIBSEMIGROUPS_TEST_CASE("docs",
                          "005",
                          "froidure-pin-base.hpp",
                          "[docs][quick]") {
    FroidurePin<BMat8> S;
    S.add_generator(
        BMat8({{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}}));
    S.add_generator(
        BMat8({{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}));
    S.add_generator(
        BMat8({{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}}));
    S.add_generator(
        BMat8({{0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}}));
    REQUIRE(S.size() == 4);
    std::vector<relation_type>(S.cbegin_rules(), S.cend_rules());
    // {{{0, 0}, {0}},
    //  {{0, 1}, {1}},
    //  {{0, 2}, {2}},
    //  {{0, 3}, {3}},
    //  {{1, 0}, {0}},
    //  {{1, 1}, {1}},
    //  {{1, 2}, {2}},
    //  {{1, 3}, {3}},
    //  {{2, 0}, {0}},
    //  {{2, 1}, {1}},
    //  {{2, 2}, {2}},
    //  {{2, 3}, {3}},
    //  {{3, 0}, {0}},
    //  {{3, 1}, {1}},
    //  {{3, 2}, {2}},
    //  {{3, 3}, {3}}}
  }

  // is_specialization_of.hpp: Line 44
  LIBSEMIGROUPS_TEST_CASE("docs",
                          "006",
                          "is_specialization_of.hpp",
                          "[docs][quick]") {
    using iso_1 = is_specialization_of<std::vector<int>, std::vector>;
    REQUIRE(iso_1::value == true);
    using iso_2 = is_specialization_of<std::vector<int>, std::unordered_map>;
    REQUIRE(iso_2::value == false);
  }

  // is_specialization_of.hpp: Line 76
  LIBSEMIGROUPS_TEST_CASE("docs",
                          "007",
                          "is_specialization_of.hpp",
                          "[docs][quick]") {
    REQUIRE(is_specialization_of_v<std::vector<int>, std::vector> == true);
    REQUIRE(is_specialization_of_v<std::vector<int>, std::unordered_map>
            == false);
  }

  // knuth-bendix-class.hpp: Line 72
  LIBSEMIGROUPS_TEST_CASE("docs",
                          "008",
                          "knuth-bendix-class.hpp",
                          "[docs][quick]") {
    Presentation<std::string> p;
    p.contains_empty_word(true);
    p.alphabet("abcd");
    presentation::add_rule_no_checks(p, "ab", "");
    presentation::add_rule_no_checks(p, "ba", "");
    presentation::add_rule_no_checks(p, "cd", "");
    presentation::add_rule_no_checks(p, "dc", "");

    KnuthBendix kb(congruence_kind::twosided, p);

    REQUIRE(kb.number_of_active_rules() == 0);
    REQUIRE(kb.number_of_pending_rules() == 4);
    kb.run();
    REQUIRE(kb.number_of_active_rules() == 4);
    REQUIRE(kb.number_of_pending_rules() == 0);
    REQUIRE(kb.confluent() == true);
    REQUIRE(kb.number_of_classes() == POSITIVE_INFINITY);
  }

  // konieczny.hpp: Line 70
  LIBSEMIGROUPS_TEST_CASE("docs", "009", "konieczny.hpp", "[docs][quick]") {
    auto S = make<Konieczny>(
        {BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
         BMat8({{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
         BMat8({{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}})});
    S.size();                   // returns 63'904
    S.number_of_idempotents();  // returns 2'360
  }

  // matrix.hpp: Line 1964
  LIBSEMIGROUPS_TEST_CASE("docs", "010", "matrix.hpp", "[docs][quick]") {
    using Mat = BMat8;
    Mat m({{1, 1}, {0, 0}});
  }

  // matrix.hpp: Line 2867
  LIBSEMIGROUPS_TEST_CASE("docs", "011", "matrix.hpp", "[docs][quick]") {
    using Mat = IntMat<>;
    Mat m(2, 3);  // construct a 2 x 3 matrix
  }

  // matrix.hpp: Line 2890
  LIBSEMIGROUPS_TEST_CASE("docs", "012", "matrix.hpp", "[docs][quick]") {
    using Mat = BMat<>;
    Mat m({1, 1, 0, 0});
  }

  // matrix.hpp: Line 2913
  LIBSEMIGROUPS_TEST_CASE("docs", "013", "matrix.hpp", "[docs][quick]") {
    using Mat = IntMat<>;
    Mat m({{1, 1}, {0, 0}});
  }

  // matrix.hpp: Line 3818
  LIBSEMIGROUPS_TEST_CASE("docs", "014", "matrix.hpp", "[docs][quick]") {
    // default construct an uninitialized 3 x 3 static matrix
    [[maybe_unused]] BMat<3> m3;
    // construct an uninitialized 4 x 4 dynamic matrix
    BMat<> m4(4, 4);
  }

  // matrix.hpp: Line 4125
  LIBSEMIGROUPS_TEST_CASE("docs", "015", "matrix.hpp", "[docs][quick]") {
    // default construct an uninitialized 3 x 3 static matrix
    [[maybe_unused]] IntMat<3> m3;
    // construct an uninitialized 4 x 4 dynamic matrix
    IntMat<> m4(4, 4);
  }

  // matrix.hpp: Line 4415
  LIBSEMIGROUPS_TEST_CASE("docs", "016", "matrix.hpp", "[docs][quick]") {
    // default construct an uninitialized 3 x 3 static matrix
    [[maybe_unused]] MaxPlusMat<3> m3;
    // construct an uninitialized 4 x 4 dynamic matrix
    MaxPlusMat<> m4(4, 4);
  }

  // matrix.hpp: Line 4724
  LIBSEMIGROUPS_TEST_CASE("docs", "017", "matrix.hpp", "[docs][quick]") {
    // default construct an uninitialized 3 x 3 static matrix
    [[maybe_unused]] MinPlusMat<3> m3;
    // construct an uninitialized 4 x 4 dynamic matrix
    MinPlusMat<> m4(4, 4);
  }

  // matrix.hpp: Line 5043
  LIBSEMIGROUPS_TEST_CASE("docs", "018", "matrix.hpp", "[docs][quick]") {
    // construct an uninitialized 3 x 3 static matrix with threshold 11
    [[maybe_unused]] MaxPlusTruncMat<11, 3> m3_11;
    // construct an uninitialized 4 x 4 dynamic matrix with threshold 11
    MaxPlusTruncMat<11> m4_11(4, 4);
    // construct a truncated max-plus semiring with threshold 11
    MaxPlusTruncSemiring sr_11(11);
    // construct an uninitialized 5 x 5 dynamic matrix with threshold 11
    // (defined at run time)
    MaxPlusTruncMat<> m5_11(&sr_11, 5, 5);
  }

  // matrix.hpp: Line 5524
  LIBSEMIGROUPS_TEST_CASE("docs", "019", "matrix.hpp", "[docs][quick]") {
    // construct an uninitialized 3 x 3 static matrix with threshold 11
    [[maybe_unused]] MinPlusTruncMat<11, 3> m3_11;
    // construct an uninitialized 4 x 4 dynamic matrix with threshold 11
    MinPlusTruncMat<11> m4_11(4, 4);
    // construct a truncated min-plus semiring with threshold 11
    MinPlusTruncSemiring sr_11(11);
    // construct an uninitialized 5 x 5 dynamic matrix with threshold 11
    // (defined at run time)
    MinPlusTruncMat<> m5_11(&sr_11, 5, 5);
  }

  // matrix.hpp: Line 6010
  LIBSEMIGROUPS_TEST_CASE("docs", "020", "matrix.hpp", "[docs][quick]") {
    // construct an uninitialized 3 x 3 static matrix with threshold
    // 11, period 2
    [[maybe_unused]] NTPMat<11, 2, 3> m3_11_2;
    // construct an uninitialized 4 x 4 dynamic matrix with threshold 11,
    // period 2
    NTPMat<11, 2> m4_11_2(4, 4);
    // construct an ntp semiring with threshold 11, period 2
    NTPSemiring<> sr_11_2(11, 2);
    // construct an uninitialized 5 x 5 dynamic matrix with threshold 11,
    // period 2
    NTPMat<> m_5_11_2(&sr_11_2, 5, 5);
  }

  // matrix.hpp: Line 6988
  LIBSEMIGROUPS_TEST_CASE("docs", "021", "matrix.hpp", "[docs][quick]") {
    // default construct an uninitialized 3 x 3 static matrix
    ProjMaxPlusMat<3> m3;
    // construct an uninitialized 4 x 4 dynamic matrix
    ProjMaxPlusMat<> m4(4, 4);
  }

  // matrix.hpp: Line 7153
  LIBSEMIGROUPS_TEST_CASE("docs", "022", "matrix.hpp", "[docs][quick]") {
    auto x = make<ProjMaxPlusMat<>>({{-2, 2, 0}, {-1, 0, 0}, {1, -3, 1}});
    // returns {{-1, 0, -1}, {-2, -1, -2}, {-1, 0, -1}}
    matrix::pow(x, 100);
  }

  // matrix.hpp: Line 7901
  LIBSEMIGROUPS_TEST_CASE("docs", "023", "matrix.hpp", "[docs][quick]") {
    auto x = make<BMat<>>({{1, 0, 0}, {0, 0, 1}, {0, 1, 0}});
    matrix::row_space_size(x);  // returns 7
  }

  // order.hpp: Line 98
  LIBSEMIGROUPS_TEST_CASE("docs", "024", "order.hpp", "[docs][quick]") {
    word_type x = 1101_w;
    word_type y = 1001_w;

    // x > y
    REQUIRE(lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend())
            == false);
  }

  // order.hpp: Line 134
  LIBSEMIGROUPS_TEST_CASE("docs", "025", "order.hpp", "[docs][quick]") {
    word_type x = 0001_w;
    word_type y = 0010_w;
    // x < y
    REQUIRE(lexicographical_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend())
            == true);
  }

  // order.hpp: Line 311
  LIBSEMIGROUPS_TEST_CASE("docs", "026", "order.hpp", "[docs][quick]") {
    word_type x = random_word(5, 10);
    word_type y = random_word(5, 10);
    shortlex_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  // order.hpp: Line 347
  LIBSEMIGROUPS_TEST_CASE("docs", "027", "order.hpp", "[docs][quick]") {
    word_type x = random_word(5, 10);
    word_type y = random_word(5, 10);
    shortlex_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  // order.hpp: Line 483
  LIBSEMIGROUPS_TEST_CASE("docs", "028", "order.hpp", "[docs][quick]") {
    word_type x = random_word(5, 10);
    word_type y = random_word(5, 10);
    recursive_path_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  // order.hpp: Line 517
  LIBSEMIGROUPS_TEST_CASE("docs", "029", "order.hpp", "[docs][quick]") {
    word_type x = random_word(5, 10);
    word_type y = random_word(5, 10);
    recursive_path_compare(x.cbegin(), x.cend(), y.cbegin(), y.cend());
  }

  // presentation.hpp: Line 863
  LIBSEMIGROUPS_TEST_CASE("docs", "030", "presentation.hpp", "[docs][quick]") {
    Presentation<std::string> p;
    presentation::to_report_string(p);
    // "|A| = 0, |R| = 0, |u| + |v| ∈ [0, 0], ∑(|u| + |v|) = 0"
  }

  // ranges.hpp: Line 87
  LIBSEMIGROUPS_TEST_CASE("docs", "031", "ranges.hpp", "[docs][quick]") {
    using rx::operator|;
    auto      wg = make<WordGraph<uint8_t>>(4, {{0, 1}, {1, 0}, {2, 2}});
    Paths     p(wg);
    p.source(0).max(10);
    REQUIRE(p.count() == 1023);
    // (p | Random()).get(); // returns random path in p (Pipe operator not
    // implemented for Paths?)
  }

  // schreier-sims.hpp: Line 166
  LIBSEMIGROUPS_TEST_CASE("docs", "032", "schreier-sims.hpp", "[docs][quick]") {
    SchreierSims<5> S;
    using Perm = decltype(S)::element_type;
    S.add_generator(Perm({1, 0, 2, 3, 4}));
    S.add_generator(Perm({1, 2, 3, 4, 0}));
    REQUIRE(S.size() == 120);
  }

  // todd-coxeter-class.hpp: Line 80
  LIBSEMIGROUPS_TEST_CASE("docs",
                          "033",
                          "todd-coxeter-class.hpp",
                          "[docs][quick]") {
    using options = detail::ToddCoxeterImpl::options;

    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 0_w, 1_w);
    ToddCoxeter tc(congruence_kind::onesided, p);
    tc.strategy(options::strategy::felsch);
    REQUIRE(tc.number_of_classes() == 1);

    auto w1 = 0000_w;
    auto w2 = 00_w;
    REQUIRE(todd_coxeter::contains(tc, w1, w2) == true);
    REQUIRE(todd_coxeter::index_of(tc, w1) == 0);
  }

  // todd-coxeter-class.hpp: Line 98
  LIBSEMIGROUPS_TEST_CASE("docs",
                          "034",
                          "todd-coxeter-class.hpp",
                          "[docs][quick]") {
    using options = detail::ToddCoxeterImpl::options;

    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(p, 00_w, 0_w);
    presentation::add_rule(p, 10_w, 1_w);
    presentation::add_rule(p, 01_w, 1_w);
    presentation::add_rule(p, 20_w, 2_w);
    presentation::add_rule(p, 02_w, 2_w);
    presentation::add_rule(p, 30_w, 3_w);
    presentation::add_rule(p, 03_w, 3_w);
    presentation::add_rule(p, 11_w, 0_w);
    presentation::add_rule(p, 23_w, 0_w);
    presentation::add_rule(p, 222_w, 0_w);
    presentation::add_rule(p, 12121212121212_w, 0_w);
    presentation::add_rule(p, 12131213121312131213121312131213_w, 0_w);
    ToddCoxeter tc(congruence_kind::twosided, p);
    tc.strategy(options::strategy::hlt)
        .lookahead_extent(options::lookahead_extent::partial)
        .save(false);
    REQUIRE(tc.number_of_classes() == 10752);
    tc.standardize(Order::recursive);
    todd_coxeter::normal_forms(tc) | rx::take(10) | rx::to_vector();
    // {0_w,
    //  1_w,
    //  2_w,
    //  21_w,
    //  12_w,
    //  121_w,
    //  22_w,
    //  221_w,
    //  212_w,
    //  2121_w}
    tc.standardize(Order::lex);
    todd_coxeter::normal_forms(tc) | rx::take(10) | rx::to_vector();
    // {0_w,
    //  01_w,
    //  012_w,
    //  0121_w,
    //  01212_w,
    //  012121_w,
    //  0121212_w,
    //  01212121_w,
    //  012121212_w,
    //  0121212121_w};
  }

  // word-graph.hpp: Line 1540
  LIBSEMIGROUPS_TEST_CASE("docs", "035", "word-graph.hpp", "[docs][quick]") {
    WordGraph<size_t> wg;
    wg.add_nodes(2);
    wg.add_to_out_degree(1);
    wg.target(0, 0, 1);
    wg.target(1, 0, 0);
    REQUIRE(word_graph::is_acyclic(wg) == false);
  }

  // word-graph.hpp: Line 1583
  LIBSEMIGROUPS_TEST_CASE("docs", "036", "word-graph.hpp", "[docs][quick]") {
    WordGraph<size_t> wg;
    wg.add_nodes(4);
    wg.add_to_out_degree(1);
    wg.target(0, 0, 1);
    wg.target(1, 0, 0);
    wg.target(2, 0, 3);
    REQUIRE(word_graph::is_acyclic(wg) == false);
    REQUIRE(word_graph::is_acyclic(wg, 0) == false);
    REQUIRE(word_graph::is_acyclic(wg, 1) == false);
    REQUIRE(word_graph::is_acyclic(wg, 2) == true);
    REQUIRE(word_graph::is_acyclic(wg, 3) == true);
  }

  // word-graph.hpp: Line 1973
  LIBSEMIGROUPS_TEST_CASE("docs", "037", "word-graph.hpp", "[docs][quick]") {
    WordGraph<size_t> wg;
    wg.add_nodes(4);
    wg.add_to_out_degree(4);
    wg.target(0, 1, 0);
    wg.target(1, 0, 0);
    wg.target(2, 3, 0);
    REQUIRE(word_graph::is_reachable_no_checks(wg, 0, 1) == false);
    REQUIRE(word_graph::is_reachable_no_checks(wg, 1, 0) == true);
    REQUIRE(word_graph::is_reachable_no_checks(wg, 1, 2) == false);
    REQUIRE(word_graph::is_reachable_no_checks(wg, 2, 3) == false);
    REQUIRE(word_graph::is_reachable_no_checks(wg, 3, 2) == false);
  }

  // word-graph.hpp: Line 2053
  LIBSEMIGROUPS_TEST_CASE("docs", "038", "word-graph.hpp", "[docs][quick]") {
    auto wg = make<WordGraph<uint8_t>>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
    REQUIRE(word_graph::is_strictly_cyclic(wg) == false);
  }

  // word-graph.hpp: Line 2793
  LIBSEMIGROUPS_TEST_CASE("docs", "039", "word-graph.hpp", "[docs][quick]") {
    // Construct a word graph with 5 nodes and 10 edges (7 specified)
    auto wg = make<WordGraph<uint8_t>>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
  }

  // word-range.hpp: Line 183
  LIBSEMIGROUPS_TEST_CASE("docs", "040", "word-range.hpp", "[docs][quick]") {
    std::vector<word_type>(cbegin_wilo(2, 3, {0}, {1, 1, 1}),
                           cend_wilo(2, 3, {0}, {1, 1, 1}));
    // {{0}, {0, 0}, {0, 1}, {1}, {1, 0}, {1, 1}};
  }

  // word-range.hpp: Line 253
  LIBSEMIGROUPS_TEST_CASE("docs", "041", "word-range.hpp", "[docs][quick]") {
    std::vector<word_type>(cbegin_wislo(2, {0}, {0, 0, 0}),
                           cend_wislo(2, {0}, {0, 0, 0}));
    // {{0}, {1}, {0, 0}, {0, 1}, {1, 0}, {1, 1}};
  }

  // word-range.hpp: Line 313
  LIBSEMIGROUPS_TEST_CASE("docs", "042", "word-range.hpp", "[docs][quick]") {
    WordRange words;
    words
        .order(Order::shortlex)  // words in shortlex order
        .alphabet_size(2)        // on 2 letters
        .min(1)                  // of length in the range from 1
        .max(5);                 // to 5
  }

  // word-range.hpp: Line 761
  LIBSEMIGROUPS_TEST_CASE("docs", "043", "word-range.hpp", "[docs][quick]") {
    ToWord toword("bac");
    REQUIRE(toword("bac") == std::vector<unsigned long>{0, 1, 2});
    REQUIRE(toword("bababbbcbc")
            == std::vector<unsigned long>{0, 1, 0, 1, 0, 0, 0, 2, 0, 2});

    toword.init();
    REQUIRE(toword("bac") == std::vector<unsigned long>{1, 0, 2});
  }

  // word-range.hpp: Line 1050
  LIBSEMIGROUPS_TEST_CASE("docs", "044", "word-range.hpp", "[docs][quick]") {
    using namespace rx;
    StringRange strings;
    strings.alphabet("ab").first("a").last("bbbb");
    auto words = (strings | ToWord("ba"));
    // contains the words
    // {1_w,    0_w,    11_w,   10_w,   01_w,   00_w,   111_w,
    //  110_w,  101_w,  100_w,  011_w,  010_w,  001_w,  000_w,
    //  1111_w, 1110_w, 1101_w, 1100_w, 1011_w, 1010_w, 1001_w,
    //  1000_w, 0111_w, 0110_w, 0101_w, 0100_w, 0011_w, 0010_w,
    //  0001_w}));
  }

  // word-range.hpp: Line 1151
  LIBSEMIGROUPS_TEST_CASE("docs", "045", "word-range.hpp", "[docs][quick]") {
    ToString tostring("bac");
    REQUIRE(tostring(word_type({1, 0, 2})) == "abc");
    REQUIRE(tostring(word_type({0, 1, 1, 0, 1, 1, 0, 2})) == "baabaabc");

    tostring.init();
    REQUIRE(tostring(word_type({1, 0, 2})) == "bac");
  }

  // word-range.hpp: Line 1402
  LIBSEMIGROUPS_TEST_CASE("docs", "046", "word-range.hpp", "[docs][quick]") {
    using namespace rx;
    WordRange words;
    words.alphabet_size(1).min(0).max(10);

    auto strings = (words | ToString("a"));
    // Contains the strings
    // {"", "a", "aa", "aaa", "aaaa", "aaaaa", "aaaaaa", "aaaaaaa",
    // "aaaaaaaa", "aaaaaaaaa"};
  }

  // word-range.hpp: Line 1596
  LIBSEMIGROUPS_TEST_CASE("docs", "047", "word-range.hpp", "[docs][quick]") {
    StringRange strings;
    strings
        .order(Order::shortlex)  // strings in shortlex order
        .alphabet("ab")          // on 2 letters
        .min(1)                  // of length in the range from 1
        .max(5);                 // to 5
  }

  // word-range.hpp: Line 2307
  LIBSEMIGROUPS_TEST_CASE("docs", "048", "word-range.hpp", "[docs][quick]") {
    using namespace words;
    word_type w = 012345_w;
    prod(w, 0, 5, 2);               // {0, 2, 4}
    prod(w, 1, 9, 2);               // {1, 3, 5, 1}
    prod("abcde", 4, 1, -1);        // "edc"
    prod({"aba", "xyz"}, 0, 4, 1);  // "abaxyzabaxyz"
  }

}  // namespace libsemigroups