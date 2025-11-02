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
LIBSEMIGROUPS_TEST_CASE("docs", "0", "action.hpp", "[docs][quick]") {
     RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>>
 o;
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
 o.size();  // returns 65536
}

// action.hpp: Line 143
LIBSEMIGROUPS_TEST_CASE("docs", "1", "action.hpp", "[docs][quick]") {
     auto rg = ReportGuard(true);
 RightAction<PPerm<16>, PPerm<16>, ImageRightAction<PPerm<16>, PPerm<16>>>
 o;
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
 o.size(); // 65536
 o.scc().number_of_components(); // 17
}

// cong-class.hpp: Line 148
LIBSEMIGROUPS_TEST_CASE("docs", "2", "cong-class.hpp", "[docs][quick]") {
     Presentation<word_type> p;
 p.alphabet(2);
 presentation::add_rule(p, {0, 1}, {});
 Congruence cong(congruence_kind::twosided, p);
 is_obviously_infinite(cong);  // true
 congruence_common::add_generating_pair(cong, {0, 0, 0}, {});
 cong.number_of_classes(); // 3
}

// freeband.hpp: Line 76
LIBSEMIGROUPS_TEST_CASE("docs", "3", "freeband.hpp", "[docs][quick]") {
     freeband_equal_to({0, 1, 2, 3, 2, 1, 0},
                   {0, 1, 2, 3, 2, 3, 2, 1, 0}); // true
 freeband_equal_to({1, 2, 3}, {0, 1, 2}); // false
 freeband_equal_to({1, 4, 2, 3, 10}, {1, 4, 1, 4, 2, 3, 10}); // true
 freeband_equal_to({0, 1, 2, 3, 4, 0, 1, 2, 3, 4},
                   {4, 3, 2, 1, 0, 4, 3, 2, 1, 0}); // false
 freeband_equal_to({0, 1, 2, 1, 0, 1, 2}, {0, 1, 2}); // true
 freeband_equal_to({0, 1, 2, 3, 0, 1},
                   {0, 1, 2, 3, 3, 2, 2, 1, 0, 2, 1, 0, 2, 3,
                    0, 2, 1, 3, 2, 1, 2, 3, 2, 1, 0, 2, 0, 1,
                    0, 2, 0, 3, 2, 0, 1, 2, 2, 3, 0, 1}); // true
}

// froidure-pin-base.hpp: Line 1337
LIBSEMIGROUPS_TEST_CASE("docs", "4", "froidure-pin-base.hpp", "[docs][quick]") {
     FroidurePin<BMat8> S;
 S.add_generator(BMat8({{1, 0, 0, 0},
                        {1, 0, 0, 0},
                        {1, 0, 0, 0},
                        {1, 0, 0, 0}}));
 S.add_generator(BMat8({{0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0}}));
 S.add_generator(BMat8({{0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0}}));
 S.add_generator(BMat8({{0, 0, 0, 1},
                        {0, 0, 0, 1},
                        {0, 0, 0, 1},
                        {0, 0, 0, 1}}));
 S.size(); // 4
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
LIBSEMIGROUPS_TEST_CASE("docs", "5", "froidure-pin-base.hpp", "[docs][quick]") {
     FroidurePin<BMat8> S;
 S.add_generator(BMat8({{1, 0, 0, 0},
                        {1, 0, 0, 0},
                        {1, 0, 0, 0},
                        {1, 0, 0, 0}}));
 S.add_generator(BMat8({{0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0}}));
 S.add_generator(BMat8({{0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0},
                        {0, 0, 1, 0}}));
 S.add_generator(BMat8({{0, 0, 0, 1},
                        {0, 0, 0, 1},
                        {0, 0, 0, 1},
                        {0, 0, 0, 1}}));
 S.size(); // 4
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
LIBSEMIGROUPS_TEST_CASE("docs", "6", "is_specialization_of.hpp", "[docs][quick]") {
     is_specialization_of<std::vector<int>, std::vector>::value; // true
 is_specialization_of<std::vector<int>, std::unordered_map>::value;
 // false
}

// is_specialization_of.hpp: Line 74
LIBSEMIGROUPS_TEST_CASE("docs", "7", "is_specialization_of.hpp", "[docs][quick]") {
     is_specialization_of_v<std::vector<int>, std::vector>; // true
 is_specialization_of_v<std::vector<int>, std::unordered_map>; // false
}

// knuth-bendix-class.hpp: Line 72
LIBSEMIGROUPS_TEST_CASE("docs", "8", "knuth-bendix-class.hpp", "[docs][quick]") {
     Presentation<std::string> p;
 p.contains_empty_word(true);
 p.alphabet("abcd");
 presentation::add_rule_no_checks(p, "ab", "");
 presentation::add_rule_no_checks(p, "ba", "");
 presentation::add_rule_no_checks(p, "cd", "");
 presentation::add_rule_no_checks(p, "dc", "");

 KnuthBendix kb(congruence_kind::twosided, p);

 !kb.confluent();              // true
 kb.run();
 kb.number_of_active_rules();  // 8
 kb.confluent();               // true
 kb.number_of_classes();       // POSITIVE_INFINITY
}

// konieczny.hpp: Line 70
LIBSEMIGROUPS_TEST_CASE("docs", "9", "konieczny.hpp", "[docs][quick]") {
      auto S = make<Konieczny>({
     BMat8({{0, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
     BMat8({{0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}, {1, 0, 0, 0}}),
     BMat8({{1, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}),
     BMat8({{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}})});
  S.size();   // returns 63'904
  S.number_of_idempotents();   //returns 2'360
}

// matrix.hpp: Line 1964
LIBSEMIGROUPS_TEST_CASE("docs", "10", "matrix.hpp", "[docs][quick]") {
      Mat m({{1, 1}, {0, 0}});
}

// matrix.hpp: Line 2866
LIBSEMIGROUPS_TEST_CASE("docs", "11", "matrix.hpp", "[docs][quick]") {
     Mat m(2, 3); // construct a 2 x 3 matrix
}

// matrix.hpp: Line 2888
LIBSEMIGROUPS_TEST_CASE("docs", "12", "matrix.hpp", "[docs][quick]") {
     Mat m({1, 1, 0, 0});
}

// matrix.hpp: Line 2910
LIBSEMIGROUPS_TEST_CASE("docs", "13", "matrix.hpp", "[docs][quick]") {
     Mat m({{1, 1}, {0, 0}});
}

// matrix.hpp: Line 3814
LIBSEMIGROUPS_TEST_CASE("docs", "14", "matrix.hpp", "[docs][quick]") {
     // default construct an uninitialized 3 x 3 static matrix
 BMat<3> m;
 // construct an uninitialized 4 x 4 dynamic matrix
 BMat<> m(4, 4);
}

// matrix.hpp: Line 4121
LIBSEMIGROUPS_TEST_CASE("docs", "15", "matrix.hpp", "[docs][quick]") {
     // default construct an uninitialized 3 x 3 static matrix
 IntMat<3> m;
 // construct an uninitialized 4 x 4 dynamic matrix
 IntMat<>  m(4, 4);
}

// matrix.hpp: Line 4411
LIBSEMIGROUPS_TEST_CASE("docs", "16", "matrix.hpp", "[docs][quick]") {
     // default construct an uninitialized 3 x 3 static matrix
 MaxPlusMat<3> m;
 // construct an uninitialized 4 x 4 dynamic matrix
 MaxPlusMat<>  m(4, 4);
}

// matrix.hpp: Line 4720
LIBSEMIGROUPS_TEST_CASE("docs", "17", "matrix.hpp", "[docs][quick]") {
     // default construct an uninitialized 3 x 3 static matrix
 MinPlusMat<3> m;
 // construct an uninitialized 4 x 4 dynamic matrix
 MinPlusMat<> m(4, 4);
}

// matrix.hpp: Line 5039
LIBSEMIGROUPS_TEST_CASE("docs", "18", "matrix.hpp", "[docs][quick]") {
     // construct an uninitialized 3 x 3 static matrix with threshold 11
 MaxPlusTruncMat<11, 3> m;
 // construct an uninitialized 4 x 4 dynamic matrix with threshold 11
 MaxPlusTruncMat<11> m(4, 4);
 // construct a truncated max-plus semiring with threshold 11
 MaxPlusTruncSemiring sr(11);
 // construct an uninitialized 5 x 5 dynamic matrix with threshold 11
 // (defined at run time)
 MaxPlusTruncMat<>  m(sr, 5, 5);
}

// matrix.hpp: Line 5520
LIBSEMIGROUPS_TEST_CASE("docs", "19", "matrix.hpp", "[docs][quick]") {
     // construct an uninitialized 3 x 3 static matrix with threshold 11
 MinPlusTruncMat<11, 3> m;
 // construct an uninitialized 4 x 4 dynamic matrix with threshold 11
 MinPlusTruncMat<11> m(4, 4);
 // construct a truncated min-plus semiring with threshold 11
 MinPlusTruncSemiring sr(11);
 // construct an uninitialized 5 x 5 dynamic matrix with threshold 11
 // (defined at run time)
 MinPlusTruncMat<>  m(sr, 5, 5);
}

// matrix.hpp: Line 6006
LIBSEMIGROUPS_TEST_CASE("docs", "20", "matrix.hpp", "[docs][quick]") {
     // construct an uninitialized 3 x 3 static matrix with threshold
 // 11, period 2
 NTPMat<11, 2, 3> m;
 // construct an uninitialized 4 x 4 dynamic matrix with threshold 11,
 // period 2
 NTPMat<11, 2> m(4, 4);
 // construct an ntp semiring with threshold 11, period 2
 NTPSemiring sr(11, 2);
 // construct an uninitialized 5 x 5 dynamic matrix with threshold 11,
 // period 2
 NTPMat<> m(sr, 5, 5);
}

// matrix.hpp: Line 6984
LIBSEMIGROUPS_TEST_CASE("docs", "21", "matrix.hpp", "[docs][quick]") {
     // default construct an uninitialized 3 x 3 static matrix
 ProjMaxPlusMat<3> m;
 // construct an uninitialized 4 x 4 dynamic matrix
 ProjMaxPlusMat<>  m(4, 4);
}

// matrix.hpp: Line 7149
LIBSEMIGROUPS_TEST_CASE("docs", "22", "matrix.hpp", "[docs][quick]") {
     auto x == make<ProjMaxPlusMat<>>({{-2, 2, 0}, {-1, 0, 0}, {1, -3,
 1}}));
 // returns {{-1, 0, -1}, {-2, -1, -2}, {-1, 0, -1}}
 matrix::pow(x, 100);
}

// matrix.hpp: Line 7897
LIBSEMIGROUPS_TEST_CASE("docs", "23", "matrix.hpp", "[docs][quick]") {
     auto x = make<BMat<>>({{1, 0, 0}, {0, 0, 1}, {0, 1, 0}});
 matrix::row_space_size(x); // returns 7
}

// order.hpp: Line 98
LIBSEMIGROUPS_TEST_CASE("docs", "24", "order.hpp", "[docs][quick]") {
     word_type x = random_word(5, 10);
 word_type y = random_word(5, 10);
 std::lexicographical_compare(
   x.cbegin(), x.cend(), y.cbegin(), y.cend());
}

// order.hpp: Line 133
LIBSEMIGROUPS_TEST_CASE("docs", "25", "order.hpp", "[docs][quick]") {
     word_type x = random_word(5, 10);
 word_type y = random_word(5, 10);
 lexicographical_compare(
   x.cbegin(), x.cend(), y.cbegin(), y.cend());
}

// order.hpp: Line 310
LIBSEMIGROUPS_TEST_CASE("docs", "26", "order.hpp", "[docs][quick]") {
     word_type x = random_word(5, 10);
 word_type y = random_word(5, 10);
 shortlex_compare(
   x.cbegin(), x.cend(), y.cbegin(), y.cend());
}

// order.hpp: Line 346
LIBSEMIGROUPS_TEST_CASE("docs", "27", "order.hpp", "[docs][quick]") {
     word_type x = random_word(5, 10);
 word_type y = random_word(5, 10);
 shortlex_compare(
   x.cbegin(), x.cend(), y.cbegin(), y.cend());
}

// order.hpp: Line 482
LIBSEMIGROUPS_TEST_CASE("docs", "28", "order.hpp", "[docs][quick]") {
     word_type x = random_word(5, 10);
 word_type y = random_word(5, 10);
 recursive_path_compare(
   x.cbegin(), x.cend(), y.cbegin(), y.cend());
}

// order.hpp: Line 516
LIBSEMIGROUPS_TEST_CASE("docs", "29", "order.hpp", "[docs][quick]") {
     word_type x = random_word(5, 10);
 word_type y = random_word(5, 10);
 recursive_path_compare(
   x.cbegin(), x.cend(), y.cbegin(), y.cend());
}

// presentation.hpp: Line 863
LIBSEMIGROUPS_TEST_CASE("docs", "30", "presentation.hpp", "[docs][quick]") {
     Presentation<std::string> p;
 presentation::to_report_string(p);
 // "|A| = 0, |R| = 0, |u| + |v| ∈ [0, 0], ∑(|u| + |v|) = 0"
}

// ranges.hpp: Line 87
LIBSEMIGROUPS_TEST_CASE("docs", "31", "ranges.hpp", "[docs][quick]") {
     using rx::operator|;
 auto wg = make<WordGraph<uint8_t>>(4, {{0, 1}, {1, 0}, {2, 2}});
 Paths p(wg);
 p.source(0).max(10);
 p.count();            // returns 1023
 // (p | Random()).get(); // returns random path in p (Pipe operator not
 // implemented for Paths?)
}

// schreier-sims.hpp: Line 166
LIBSEMIGROUPS_TEST_CASE("docs", "32", "schreier-sims.hpp", "[docs][quick]") {
      SchreierSims<5> S;
  using Perm = decltype(S)::element_type;
  S.add_generator(Perm({1, 0, 2, 3, 4}));
  S.add_generator(Perm({1, 2, 3, 4, 0}));
  S.size(); // 120
}

// todd-coxeter-class.hpp: Line 80
LIBSEMIGROUPS_TEST_CASE("docs", "33", "todd-coxeter-class.hpp", "[docs][quick]") {
     Presentation<word_type> p;
 p.alphabet(2);
 presentation::add_rule(p, 00_w, 0_w);
 presentation::add_rule(p, 0_w, 1_w);
 ToddCoxeter tc(congruence_kind::onesided, p);
 tc.strategy(options::strategy::felsch);
 tc.number_of_classes();
 tc.contains(0000_w, 00_w);
 tc.index_of(0000_w);
}

// todd-coxeter-class.hpp: Line 93
LIBSEMIGROUPS_TEST_CASE("docs", "34", "todd-coxeter-class.hpp", "[docs][quick]") {
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
 tc.number_of_classes()  // 10'752
 tc.standardize(order::recursive);
 normal_forms(tc) | rx::take(10) | rx::to_vector()
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
 tc.standardize(order::lex);
 normal_forms(tc) | rx::take(10) | rx::to_vector()
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

// todd-coxeter-helpers.hpp: Line 95
LIBSEMIGROUPS_TEST_CASE("docs", "35", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.current_index_of_no_checks(std::begin(w), std::end(w));
}

// todd-coxeter-helpers.hpp: Line 118
LIBSEMIGROUPS_TEST_CASE("docs", "36", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.current_index_of(std::begin(w), std::end(w));
}

// todd-coxeter-helpers.hpp: Line 141
LIBSEMIGROUPS_TEST_CASE("docs", "37", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.index_of_no_checks(std::begin(w), std::end(w));
}

// todd-coxeter-helpers.hpp: Line 164
LIBSEMIGROUPS_TEST_CASE("docs", "38", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.index_of(std::begin(w), std::end(w));
}

// todd-coxeter-helpers.hpp: Line 186
LIBSEMIGROUPS_TEST_CASE("docs", "39", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.current_index_of_no_checks(std::begin(w), std::end(w));
}

// todd-coxeter-helpers.hpp: Line 211
LIBSEMIGROUPS_TEST_CASE("docs", "40", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.current_index_of(std::begin(w), std::end(w));
}

// todd-coxeter-helpers.hpp: Line 236
LIBSEMIGROUPS_TEST_CASE("docs", "41", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.index_of_no_checks(std::begin(w), std::end(w));
}

// todd-coxeter-helpers.hpp: Line 261
LIBSEMIGROUPS_TEST_CASE("docs", "42", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.index_of(std::begin(w), std::end(w));
}

// todd-coxeter-helpers.hpp: Line 285
LIBSEMIGROUPS_TEST_CASE("docs", "43", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.current_index_of_no_checks(w, w + std::strlen(w));
}

// todd-coxeter-helpers.hpp: Line 307
LIBSEMIGROUPS_TEST_CASE("docs", "44", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.current_index_of(w, w + std::strlen(w));
}

// todd-coxeter-helpers.hpp: Line 329
LIBSEMIGROUPS_TEST_CASE("docs", "45", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.index_of_no_checks(w, w + std::strlen(w));
}

// todd-coxeter-helpers.hpp: Line 351
LIBSEMIGROUPS_TEST_CASE("docs", "46", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.index_of(w, w + std::strlen(w));
}

// todd-coxeter-helpers.hpp: Line 834
LIBSEMIGROUPS_TEST_CASE("docs", "47", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     Presentation<std::string> p;
 p.alphabet("abcdef");
 p.contains_empty_word(true);
 presentation::add_inverse_rules(p, "defabc");
 presentation::add_rule(p, "bbdeaecbffdbaeeccefbccefb", "");
 presentation::add_rule(p, "ccefbfacddecbffaafdcaafdc", "");
 presentation::add_rule(p, "aafdcdbaeefacddbbdeabbdea", "");
 ToddCoxeter tc(congruence_kind.twosided, p);
}

// todd-coxeter-helpers.hpp: Line 852
LIBSEMIGROUPS_TEST_CASE("docs", "48", "todd-coxeter-helpers.hpp", "[docs][quick]") {
     tc.lookahead_extent(options::lookahead_extent::full)
     .lookahead_style(options::lookahead_style::felsch);

 tc.run_for(std::chrono::seconds(1));
 tc.perform_lookahead(true);

 todd_coxeter::perform_lookbehind(tc);
 tc.run_for(std::chrono::seconds(1));
 todd_coxeter::perform_lookbehind(tc);
 tc.perform_lookahead(true);
 tc.number_of_classes(); // returns 1
}

// word-graph.hpp: Line 1540
LIBSEMIGROUPS_TEST_CASE("docs", "49", "word-graph.hpp", "[docs][quick]") {
     WordGraph<size_t> wg;
 wg.add_nodes(2);
 wg.add_to_out_degree(1);
 wg.target(0, 0, 1);
 wg.target(1, 0, 0);
 word_graph::is_acyclic(wg); // returns false
}

// word-graph.hpp: Line 1583
LIBSEMIGROUPS_TEST_CASE("docs", "50", "word-graph.hpp", "[docs][quick]") {
     WordGraph<size_t> wg;
 wg.add_nodes(4);
 wg.add_to_out_degree(1);
 wg.target(0, 0, 1);
 wg.target(1, 0, 0);
 wg.target(2, 0, 3);
 word_graph::is_acyclic(wg); // returns false
 word_graph::is_acyclic(wg, 0); // returns false
 word_graph::is_acyclic(wg, 1); // returns false
 word_graph::is_acyclic(wg, 2); // returns true
 word_graph::is_acyclic(wg, 3); // returns true
}

// word-graph.hpp: Line 1973
LIBSEMIGROUPS_TEST_CASE("docs", "51", "word-graph.hpp", "[docs][quick]") {
     WordGraph<size_t> wg;
 wg.add_nodes(4);
 wg.add_to_out_degree(1);
 wg.target(0, 1, 0);
 wg.target(1, 0, 0);
 wg.target(2, 3, 0);
 word_graph::is_reachable_no_checks(wg, 0, 1); // returns true
 word_graph::is_reachable_no_checks(wg, 1, 0); // returns true
 word_graph::is_reachable_no_checks(wg, 1, 2); // returns false
 word_graph::is_reachable_no_checks(wg, 2, 3); // returns true
 word_graph::is_reachable_no_checks(wg, 3, 2); // returns false
}

// word-graph.hpp: Line 2053
LIBSEMIGROUPS_TEST_CASE("docs", "52", "word-graph.hpp", "[docs][quick]") {
     auto wg = make<WordGraph<uint8_t>>(
     5, {{0, 0}, {1, 1}, {2}, {3, 3}});
 word_graph::is_strictly_cyclic(wg);  // returns false
}

// word-graph.hpp: Line 2793
LIBSEMIGROUPS_TEST_CASE("docs", "53", "word-graph.hpp", "[docs][quick]") {
     // Construct a word graph with 5 nodes and 10 edges (7 specified)
 make<WordGraph<uint8_t>>(5, {{0, 0}, {1, 1}, {2}, {3, 3}});
}

// word-range.hpp: Line 183
LIBSEMIGROUPS_TEST_CASE("docs", "54", "word-range.hpp", "[docs][quick]") {
     std::vector<word_type>(cbegin_wilo(2, 3, {0}, {1, 1, 1}),
                        cend_wilo(2, 3, {0}, {1, 1, 1}));
 // {{0}, {0, 0}, {0, 1}, {1}, {1, 0}, {1, 1}};
}

// word-range.hpp: Line 253
LIBSEMIGROUPS_TEST_CASE("docs", "55", "word-range.hpp", "[docs][quick]") {
     std::vector<word_type>(cbegin_wislo(2, {0}, {0, 0, 0}),
                        cend_wislo(2,  {0}, {0, 0, 0}));
 // {{0}, {1}, {0, 0}, {0, 1}, {1, 0}, {1, 1}};
}

// word-range.hpp: Line 313
LIBSEMIGROUPS_TEST_CASE("docs", "56", "word-range.hpp", "[docs][quick]") {
     WordRange words;
 words.order(Order::shortlex)  // words in shortlex order
      .alphabet_size(2)        // on 2 letters
      .min(1)                  // of length in the range from 1
      .max(5);                 // to 5
}

// word-range.hpp: Line 761
LIBSEMIGROUPS_TEST_CASE("docs", "57", "word-range.hpp", "[docs][quick]") {
     ToWord toword("bac");
 toword("bac");        // returns {0, 1, 2}
 toword("bababbbcbc"); // returns { 0, 1, 0, 1, 0, 0, 0, 2, 0, 2}

 toword.init();
 toword("bac");        // returns {1, 0, 2}
}

// word-range.hpp: Line 1050
LIBSEMIGROUPS_TEST_CASE("docs", "58", "word-range.hpp", "[docs][quick]") {
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

// word-range.hpp: Line 1150
LIBSEMIGROUPS_TEST_CASE("docs", "59", "word-range.hpp", "[docs][quick]") {
     ToString tostring("bac");
 tostring(word_type({1, 0, 2}));                 // returns "abc"
 tostring(word_type({0, 1, 1, 0, 1, 1, 0, 2}));  // returns "baabaabc"

 tostring.init();
 tostring(word_type({1, 0, 2}));                 // returns "bac"
}

// word-range.hpp: Line 1401
LIBSEMIGROUPS_TEST_CASE("docs", "60", "word-range.hpp", "[docs][quick]") {
     WordRange words;
 words.alphabet_size(1).min(0).max(10);

 auto strings = (words | ToString("a"));
 // Contains the strings
 // {"", "a", "aa", "aaa", "aaaa", "aaaaa", "aaaaaa", "aaaaaaa",
 // "aaaaaaaa", "aaaaaaaaa"};
}

// word-range.hpp: Line 1594
LIBSEMIGROUPS_TEST_CASE("docs", "61", "word-range.hpp", "[docs][quick]") {
     StringRange strings;
 strings.order(Order::shortlex) // strings in shortlex order
        .alphabet("ab")         // on 2 letters
        .min(1)                 // of length in the range from 1
        .max(5);                // to 5
}

// word-range.hpp: Line 2305
LIBSEMIGROUPS_TEST_CASE("docs", "62", "word-range.hpp", "[docs][quick]") {
     using namespace words;
 word_type w = 012345_w;
 prod(w, 0, 5, 2);              // {0, 2, 4}
 prod(w, 1, 9, 2);              // {1, 3, 5, 1}
 prod("abcde", 4, 1, -1);       // "edc"
 prod({"aba", "xyz"}, 0, 4, 1); // "abaxyzabaxyz"
}


}