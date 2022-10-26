// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

#include <stddef.h>  // for size_t

#include <algorithm>      // for count_if, all_of
#include <iostream>       // for string, char_traits
#include <iterator>       // for distance
#include <memory>         // for allocator, shared_ptr
#include <string>         // for basic_string, operator==, operator!=, operator+
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch.hpp"      // for REQUIRE, REQUIRE_THROWS_AS
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"          // for UNDEFINED
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/iterator.hpp"           // for ConstIteratorStateful
#include "libsemigroups/kambites.hpp"           // for Kambites
#include "libsemigroups/knuth-bendix.hpp"       // for KnuthBendix
#include "libsemigroups/report.hpp"             // for ReportGuard
#include "libsemigroups/siso.hpp"               // for const_sislo_iterator
#include "libsemigroups/string.hpp"             // for random_string etc
#include "libsemigroups/transf.hpp"             // for LeastTransf
#include "libsemigroups/types.hpp"              // for tril etc
#include "libsemigroups/word.hpp"               // for number_of_words

namespace libsemigroups {
  struct LibsemigroupsException;  // Forward decl

  constexpr bool REPORT = false;
  using detail::MultiStringView;
  using detail::random_string;

  namespace {
    // TODO(later) remove this, or put it in test-suffix-tree.cpp
    template <typename T>
    size_t number_of_subwords(T first, T last) {
      std::unordered_set<std::string> mp;

      for (auto it = first; it < last; ++it) {
        {
          auto& w = it->first;
          for (auto suffix = w.cbegin(); suffix < w.cend(); ++suffix) {
            for (auto prefix = suffix + 1; prefix < w.cend(); ++prefix) {
              mp.emplace(suffix, prefix);
            }
          }
        }
        {
          auto& w = it->second;
          for (auto suffix = w.cbegin(); suffix < w.cend(); ++suffix) {
            for (auto prefix = suffix + 1; prefix < w.cend(); ++prefix) {
              mp.emplace(suffix, prefix);
            }
          }
        }
      }
      return mp.size();
    }

    // TODO(later) remove this
    template <typename T>
    size_t sum_lengths(T first, T last) {
      size_t result = 0;
      for (auto it = first; it < last; ++it) {
        result += it->first.size();
        result += it->second.size();
      }
      return result;
    }

    std::string random_power_string(std::string const& s,
                                    std::string const& t,
                                    std::string const& u,
                                    size_t             exp) {
      static std::random_device              rd;
      static std::mt19937                    generator(rd());
      static std::uniform_int_distribution<> distribution(0, 2);
      std::string                            result = "";
      while (exp > 0) {
        switch (distribution(generator)) {
          case 0: {
            result += s;
            break;
          }
          case 1: {
            result += t;
            break;
          }
          case 2: {
            result += u;
          }
          default:
            break;
        }
        exp--;
      }
      return result;
    }

    // std::string swap_a_and_b(std::string const& w) {
    //   std::string result;
    //   for (auto l : w) {
    //     if (l == 'a') {
    //       result += "b";
    //     } else if (l == '#') {
    //       result += '#';
    //     } else {
    //       result += "a";
    //     }
    //   }
    //   return result;
    // }

    auto sample(std::string A,
                size_t      R,
                size_t      min,
                size_t      max,
                size_t      sample_size) {
      if (min < 7) {
        LIBSEMIGROUPS_EXCEPTION("the minimum value of <min> is at least 7");
        // Otherwise we get lhs = rhs in many of the things below and this
        // skews the results.
      } else if (min < max && max - min <= 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "the minimum and maximum values must be at least 2 apart");
      }
      auto     rg              = ReportGuard(false);
      uint64_t total_c4        = 0;
      uint64_t total_confluent = 0;
      for (size_t j = 0; j < sample_size; ++j) {
        fpsemigroup::Kambites<std::string> k;
        k.set_alphabet(A);
        fpsemigroup::KnuthBendix kb1;
        kb1.set_alphabet(A);
        fpsemigroup::KnuthBendix kb2;
        std::reverse(A.begin(), A.end());
        kb2.set_alphabet(A);
        std::reverse(A.begin(), A.end());
        for (size_t r = 0; r < R; ++r) {
          auto        lhs = random_string(A, min, max);
          std::string rhs;
          if (lhs.size() == min) {
            rhs = random_string(A, min + 1, max);
          } else {
            rhs = random_string(A, min, lhs.size());
          }

          k.add_rule(lhs, rhs);
          kb1.add_rule(lhs, rhs);
          kb2.add_rule(lhs, rhs);
        }
        kb1.run_for(std::chrono::milliseconds(1));
        kb2.run_for(std::chrono::milliseconds(1));
        if (k.small_overlap_class() >= 4) {
          total_c4++;
        }
        if (kb1.confluent() || kb2.confluent()) {
          total_confluent++;
        }
      }
      return std::make_tuple(total_c4, total_confluent);
    }

    std::array<std::string, 5> swap_a_b_c(std::string const& w) {
      static std::array<std::string, 5> perms
          = {"bac", "acb", "cba", "bca", "cab"};
      std::array<std::string, 5> result;
      size_t                     count = 0;
      for (auto const& p : perms) {
        std::string ww;
        for (auto l : w) {
          if (l == 'a') {
            ww += p[0];
          } else if (l == 'b') {
            ww += p[1];
          } else {
            ww += p[2];
          }
        }
        result[count] = ww;
        count++;
      }
      return result;
    }

  }  // namespace

#ifdef false
  namespace {

    Kambites<> random_example(std::string const& alphabet) {
      static std::random_device       rd;
      static std::mt19937             generator(rd());
      std::uniform_int_distribution<> distribution(1, 25);

      Kambites<> k;
      k.set_alphabet(alphabet);

      std::vector<std::string> pieces;
      for (size_t i = 0; i < 13; ++i) {
        pieces.push_back(random_string(alphabet, distribution(generator)));
      }

      k.add_rule(pieces[0] + pieces[1] + pieces[2] + pieces[3],
                 pieces[2] + pieces[0] + pieces[7] + pieces[4]);
      k.add_rule(pieces[4] + pieces[0] + pieces[5], pieces[6]);
      k.add_rule(pieces[7] + pieces[1] + pieces[8], pieces[9]);
      k.add_rule(pieces[10] + pieces[3] + pieces[11], pieces[12]);

      std::cout << "k.add_rule(\""
                << pieces[0] + pieces[1] + pieces[2] + pieces[3] << "\", \""
                << pieces[2] + pieces[0] + pieces[7] + pieces[4] << "\");\n";
      std::cout << "k.add_rule(\"" << pieces[4] + pieces[0] + pieces[5]
                << "\", \"" << pieces[6] << "\");\n";
      std::cout << "k.add_rule(\"" << pieces[7] + pieces[1] + pieces[8]
                << "\", \"" << pieces[9] << "\");\n";
      std::cout << "k.add_rule(\"" << pieces[10] + pieces[3] + pieces[11]
                << "\", \"" << pieces[12] << "\");\n";
      return k;
    }
  }  // namespace
#endif

  namespace fpsemigroup {

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_4() {
      auto rg = ReportGuard(REPORT);

      Kambites<T> k;
      k.set_alphabet("abcdefg");
      k.add_rule("abcd", "aaaeaa");
      k.add_rule("ef", "dg");

      REQUIRE(k.equal_to("abcd", "aaaeaa"));
      REQUIRE(k.equal_to("ef", "dg"));
      REQUIRE(k.equal_to("aaaaaef", "aaaaadg"));
      REQUIRE(k.equal_to("efababa", "dgababa"));
      k.froidure_pin()->enumerate(100);
      REQUIRE(k.froidure_pin()->current_size() == 8205);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "000",
                            "(fpsemi) MT test 4 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_4<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "001",
                            "(fpsemi) MT test 4 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_4<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_no_name_1() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("aAbBcCe");

      k.add_rule("aaa", "e");
      k.add_rule("bbb", "e");
      k.add_rule("ccc", "e");
      k.add_rule("ABa", "BaB");
      k.add_rule("bcB", "cBc");
      k.add_rule("caC", "aCa");
      k.add_rule("abcABCabcABCabcABC", "e");
      k.add_rule("BcabCABcabCABcabCA", "e");
      k.add_rule("cbACBacbACBacbACBa", "e");

      REQUIRE(k.number_of_pieces(0) == 2);
      REQUIRE(k.number_of_pieces(1) == POSITIVE_INFINITY);

      REQUIRE(k.number_of_pieces(2) == 2);
      REQUIRE(k.number_of_pieces(3) == POSITIVE_INFINITY);

      REQUIRE(k.number_of_pieces(4) == 2);
      REQUIRE(k.number_of_pieces(5) == POSITIVE_INFINITY);

      REQUIRE(k.number_of_pieces(6) == 2);
      REQUIRE(k.number_of_pieces(7) == 2);

      REQUIRE(k.number_of_pieces(8) == 2);
      REQUIRE(k.number_of_pieces(9) == 2);

      REQUIRE(k.number_of_pieces(10) == 2);
      REQUIRE(k.number_of_pieces(11) == 2);

      REQUIRE(k.number_of_pieces(12) == 2);
      REQUIRE(k.number_of_pieces(13) == POSITIVE_INFINITY);

      REQUIRE(k.number_of_pieces(14) == 2);
      REQUIRE(k.number_of_pieces(15) == POSITIVE_INFINITY);

      REQUIRE(k.number_of_pieces(16) == 2);
      REQUIRE(k.number_of_pieces(17) == POSITIVE_INFINITY);

      REQUIRE(k.small_overlap_class() == 2);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "002",
                            "(fpsemi) number_of_pieces (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_no_name_1<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "003",
                            "(fpsemi) number_of_pieces (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_no_name_1<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_no_name_2() {
      auto rg = ReportGuard(REPORT);
      for (size_t i = 4; i < 20; ++i) {
        std::string lhs;
        for (size_t b = 1; b <= i; ++b) {
          lhs += "a" + std::string(b, 'b');
        }
        std::string rhs;
        for (size_t b = i + 1; b <= 2 * i; ++b) {
          rhs += "a" + std::string(b, 'b');
        }

        Kambites<T> k;

        k.set_alphabet("ab");
        k.add_rule(lhs, rhs);

        REQUIRE(k.number_of_pieces(0) == i);
        REQUIRE(k.number_of_pieces(1) == i + 1);

        REQUIRE(k.small_overlap_class() == i);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "004",
                            "(fpsemi) small_overlap_class (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_no_name_2<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "005",
                            "(fpsemi) small_overlap_class (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_no_name_2<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_random() {
      auto rg = ReportGuard(REPORT);
      {
        Kambites<T> k;
        k.set_alphabet("abcdefghi");
        k.add_rule("eiehiegiggfaigcdfdfdgiidcebacgfaf",
                   "cgfaeiehiegiggfaigcdfdfdgigcccbddchbbhgaaedfiiahhehihcba");
        k.add_rule("hihcbaeiehiegiggfaigcdfdfdgiefhbidhbdgb", "chhfgafiiddg");
        k.add_rule("gcccbddchbbhgaaedfiiahheidcebacbdefegcehgffedacddiaiih",
                   "eddfcfhbedecacheahcdeeeda");
        k.add_rule("dfbiccfeagaiffcfifg", "dceibahghaedhefh");

        REQUIRE(k.small_overlap_class() == 4);
        REQUIRE(number_of_subwords(k.cbegin_rules(), k.cend_rules()) == 3762);
        size_t n = sum_lengths(k.cbegin_rules(), k.cend_rules());
        REQUIRE(n == 254);
        REQUIRE(n * n == 64516);
      }
      {
        Kambites<T> k;
        k.set_alphabet("abcdefghi");
        k.add_rule("feffgccdgcfbeagiifheabecdfbgebfcibeifibccahaafabeihfgfieade"
                   "bciheddeigbaf",
                   "ifibccahaafabeihfgfiefeffgccdgcfbeagiifheabecfeibghddfgbaia"
                   "acghhdhggagaide");
        k.add_rule("ghhdhggagaidefeffgccdgcfbeagiifheabeccbeiddgdcbcf",
                   "ahccccffdeb");
        k.add_rule("feibghddfgbaiaacdfbgebfcibeieaacdbdb", "gahdfgbghhhbcci");
        k.add_rule("dgibafaahiabfgeiiibadebciheddeigbaficfbfdbfbbiddgdcifbe",
                   "iahcfgdbggaciih");
        REQUIRE(k.small_overlap_class() == 4);
        REQUIRE(number_of_subwords(k.cbegin_rules(), k.cend_rules()) == 7197);
        size_t n = sum_lengths(k.cbegin_rules(), k.cend_rules());
        REQUIRE(n == 327);
        REQUIRE(n * n == 106929);
      }
      {
        Kambites<T> k;
        k.set_alphabet("abcdefghi");
        k.add_rule("adichhbhibfchbfbbibaidfibifgagcgdedfeeibhggdbchfdaefbefcbaa"
                   "hcbhbidgaahbahhahhb",
                   "edfeeibhggdbchfdaefbeadichhbhibfchbfbbibaiihebabeabahcgdbic"
                   "bgiciffhfggbfadf");
        k.add_rule("bgiciffhfggbfadfadichhbhibfchbfbbibaaggfdcfcebehhbdegiaeaf",
                   "hebceeicbhidcgahhcfbb");
        k.add_rule("iihebabeabahcgdbicidfibifgagcgdedehed",
                   "ecbcgaieieicdcdfdbgagdbf");
        k.add_rule("iagaadbfcbaahcbhbidgaahbahhahhbd", "ddddh");
        REQUIRE(k.small_overlap_class() == 3);
        REQUIRE(number_of_subwords(k.cbegin_rules(), k.cend_rules()) == 7408);
        size_t n = sum_lengths(k.cbegin_rules(), k.cend_rules());
        REQUIRE(n == 330);
        REQUIRE(n * n == 108900);
      }
      {
        Kambites<T> k;
        k.set_alphabet("abcdefghi");
        k.add_rule("ibddgdgddiabcahbidbedffeddciiabahbbiacbfehdfccacbhgafbgcdg",
                   "iabahibddgdgddbdfacbafhcgfhdheieihd");
        k.add_rule("hdheieihdibddgdgddebhaeaicciidebegg", "giaeehdeeec");
        k.add_rule("bdfacbafhcgfiabcahbidbedffeddcifdfcdcdadhhcbcbebhei",
                   "icaebehdff");
        k.add_rule("aggiiacdbbiacbfehdfccacbhgafbgcdghiahfccdchaiagaha",
                   "hhafbagbhghhihg");

        REQUIRE(k.small_overlap_class() == 4);
        REQUIRE(number_of_subwords(k.cbegin_rules(), k.cend_rules()) == 4560);
        size_t n = sum_lengths(k.cbegin_rules(), k.cend_rules());
        REQUIRE(n == 265);
        REQUIRE(n * n == 70225);
      }
      {
        Kambites<T> k;
        k.set_alphabet("abcdefghi");
        k.add_rule(
            "fibehffegdeggaddgfdaeaiacbhbgbbccceaibfcabbiedhecggbbdgihddd",
            "ceafibehffegdeggafidbaefcebegahcbhciheceaehaaehih");
        k.add_rule("haaehihfibehffegdeggaecbedccaeabifeafi",
                   "bfcccibgefiidgaih");
        k.add_rule("fidbaefcebegahcbhciheceaeddgfdaeaiacbhbgbbcccgiahbibehgbgab"
                   "efdieiggc",
                   "abigdadaecdfdeeciggbdfdf");
        k.add_rule("eeaaiicigieiabibfcabbiedhecggbbdgihdddifadgbgidbfeg",
                   "daheebdgdiaeceeiicddg");
        REQUIRE(k.small_overlap_class() == 4);
        REQUIRE(number_of_subwords(k.cbegin_rules(), k.cend_rules()) == 6398);
        size_t n = sum_lengths(k.cbegin_rules(), k.cend_rules());
        REQUIRE(n == 328);
        REQUIRE(n * n == 107584);
      }
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "006",
                            "(fpsemi) random (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_random<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "007",
                            "(fpsemi) random (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_random<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_knuth_bendix_055() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcdefg");

      k.add_rule("abcd", "ce");
      k.add_rule("df", "dg");

      REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);
      REQUIRE(k.is_obviously_infinite());

      REQUIRE(k.equal_to("dfabcdf", "dfabcdg"));
      REQUIRE(k.equal_to("abcdf", "ceg"));
      REQUIRE(k.equal_to("abcdf", "cef"));
      REQUIRE(k.equal_to("dfabcdf", "dfabcdg"));
      REQUIRE(k.equal_to("abcdf", "ceg"));
      REQUIRE(k.equal_to("abcdf", "cef"));
      REQUIRE(k.normal_form("abcdfceg") == "abcdfabcdf");

      REQUIRE(k.size() == POSITIVE_INFINITY);
      REQUIRE(number_of_words(k.alphabet().size(), 0, 6) == 19608);
      REQUIRE(k.number_of_normal_forms(0, 6) == 17921);

      REQUIRE(number_of_subwords(k.cbegin_rules(), k.cend_rules()) == 7);
      // REQUIRE(std::vector<std::string>(k.cbegin_normal_forms(0, 2),
      //                                 k.cend_normal_forms())
      //        == std::vector<std::string>({"a", "b", "c", "d", "e", "f",
      //        "g"}));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "008",
                            "(fpsemi) KnuthBendix 055 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_knuth_bendix_055<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "009",
        "(fpsemi) KnuthBendix 055 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_knuth_bendix_055<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_gap_smalloverlap_85() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("cab");

      k.add_rule("aabc", "acba");
      REQUIRE(!k.equal_to("a", "b"));
      REQUIRE(k.equal_to("aabcabc", "aabccba"));

      REQUIRE(k.size() == POSITIVE_INFINITY);
      REQUIRE(number_of_words(3, 4, 16) == 21523320);
      REQUIRE(std::distance(cbegin_sislo("cab", "aabc", "aaabc"),
                            cend_sislo("cab", "aabc", "aaabc"))
              == 162);

      REQUIRE(std::count_if(
                  cbegin_sislo("cab", "cccc", "ccccc"),
                  cend_sislo("cab", "cccc", "ccccc"),
                  [&k](std::string const& w) { return k.equal_to(w, "acba"); })
              == 2);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "010",
        "(fpsemi) smalloverlap/gap/test.gi:85 (std::string)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_gap_smalloverlap_85<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "011",
        "(fpsemi) smalloverlap/gap/test.gi:85 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_gap_smalloverlap_85<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "012",
                            "(fpsemi) free semigroup",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      Kambites<std::string> k;
      k.set_alphabet("cab");
      REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);

      Kambites<detail::MultiStringView> kk;
      kk.set_alphabet("cab");
      REQUIRE(kk.small_overlap_class() == POSITIVE_INFINITY);
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_gap_smalloverlap_49() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcdefgh");

      k.add_rule("abcd", "ce");
      k.add_rule("df", "hd");

      REQUIRE(k.is_obviously_infinite());

      REQUIRE(k.equal_to("abchd", "abcdf"));
      REQUIRE(!k.equal_to("abchf", "abcdf"));
      REQUIRE(k.equal_to("abchd", "abchd"));
      REQUIRE(k.equal_to("abchdf", "abchhd"));
      // Test cases (4) and (5)
      REQUIRE(k.equal_to("abchd", "cef"));
      REQUIRE(k.equal_to("cef", "abchd"));

      REQUIRE(k.size() == POSITIVE_INFINITY);
      REQUIRE(k.number_of_normal_forms(0, 6) == 35199);
      REQUIRE(k.normal_form("hdfabce") == "dffababcd");
      REQUIRE(k.equal_to("hdfabce", "dffababcd"));
      // TODO(later) include when we have cbegin_normal_forms,
      // cend_normal_forms
      //  REQUIRE(std::vector<std::string>(k.cbegin_normal_forms(0, 2),
      //                                   k.cend_normal_forms())
      //          == std::vector<std::string>(
      //              {"a", "b", "c", "d", "e", "f", "g", "h"}));*/
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "013",
        "(fpsemi) smalloverlap/gap/test.gi:49 (std::string)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_gap_smalloverlap_49<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "014",
        "(fpsemi) smalloverlap/gap/test.gi:49 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_gap_smalloverlap_49<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_gap_smalloverlap_63() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcdefgh");

      k.add_rule("afh", "bgh");
      k.add_rule("hc", "d");

      REQUIRE(k.is_obviously_infinite());

      // Test case (6)
      REQUIRE(k.equal_to("afd", "bgd"));
      REQUIRE(k.equal_to("bghcafhbgd", "afdafhafd"));
      REQUIRE(k.normal_form("bghcafhbgd") == "afdafhafd");
      REQUIRE(k.number_of_normal_forms(0, 6) == 34819);

      REQUIRE(k.size() == POSITIVE_INFINITY);
    }
    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "015",
        "(fpsemi) smalloverlap/gap/test.gi:63 (std::string)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_gap_smalloverlap_63<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "016",
        "(fpsemi) smalloverlap/gap/test.gi:63 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_gap_smalloverlap_63<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_gap_smalloverlap_70() {
      auto rg = ReportGuard(REPORT);
      // The following permits a more complex test of case (6), which also
      // involves using the case (2) code to change the prefix being looked
      // for:
      Kambites<T> k;
      k.set_alphabet("abcdefghij");

      k.add_rule("afh", "bgh");
      k.add_rule("hc", "de");
      k.add_rule("ei", "j");

      REQUIRE(k.number_of_normal_forms(0, 6) == 102255);
      REQUIRE(k.is_obviously_infinite());

      REQUIRE(k.equal_to("afdj", "bgdj"));
      REQUIRE(!k.equal_to("xxxxxxxxxxxxxxxxxxxxxxx", "b"));
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "017",
        "(fpsemi) smalloverlap/gap/test.gi:70 (std::string)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_gap_smalloverlap_70<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "018",
        "(fpsemi) smalloverlap/gap/test.gi:70 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_gap_smalloverlap_70<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_1_million_equals() {
      auto rg = ReportGuard(REPORT);
      // A slightly more complicated presentation for testing case (6), in
      // which the max piece suffixes of the first two relation words no
      // longer agree (since fh and gh are now pieces).
      Kambites<T> k;
      k.set_alphabet("abcdefghijkl");

      k.add_rule("afh", "bgh");
      k.add_rule("hc", "de");
      k.add_rule("ei", "j");
      k.add_rule("fhk", "ghl");

      REQUIRE(k.is_obviously_infinite());

      REQUIRE(k.equal_to("afdj", "bgdj"));
      REQUIRE(k.equal_to("afdj", "afdj"));
      REQUIRE(k.normal_form("bfhk") == "afhl");
      REQUIRE(k.equal_to("bfhk", "afhl"));

      REQUIRE(k.size() == POSITIVE_INFINITY);

      size_t N = std::distance(cbegin_sislo("abcdefghijkl", "a", "bgdk"),
                               cend_sislo("abcdefghijkl", "a", "bgdk"));
      REQUIRE(N == 4522);
      size_t M = 0;
      for (auto it1 = cbegin_sislo("abcdefghijkl", "a", "bgdk");
           it1 != cend_sislo("abcdefghijkl", "a", "bgdk");
           ++it1) {
        for (auto it2 = cbegin_sislo("abcdefghijkl", "a", "bgdk"); it2 != it1;
             ++it2) {
          M++;
          if (k.equal_to(*it1, *it2)) {
            N--;
            break;
          }
        }
      }
      REQUIRE(M == 10052729);
      REQUIRE(N == 4392);
      REQUIRE(k.number_of_normal_forms(0, 6) == 255932);

      // TODO(later) include when we have cbegin_normal_forms,
      // cend_normal_forms
      // REQUIRE(
      //     std::vector<std::string>(k.cbegin_normal_forms(0, 2),
      //                              k.cend_normal_forms())
      //     == std::vector<std::string>(
      //         {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
      // "l"}));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "019",
                            "(fpsemi) std::string smalloverlap/gap/test.gi:77 "
                            "(infinite) (KnuthBendix 059)",
                            "[standard][kambites][fpsemigroup][fpsemi]") {
      test_case_1_million_equals<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "020",
        "(fpsemi) MultiStringView smalloverlap/gap/test.gi:77 "
        "(infinite) (KnuthBendix 059)",
        "[standard][kambites][fpsemigroup][fpsemi]") {
      test_case_1_million_equals<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_code_cov() {
      auto rg = ReportGuard(REPORT);
      // A slightly more complicated presentation for testing case (6), in
      // which the max piece suffixes of the first two relation words no
      // longer agree (since fh and gh are now pieces).
      Kambites<T> k;
      k.set_alphabet("abcde");
      k.add_rule("cadeca", "baedba");
      REQUIRE(!k.equal_to("cadece", "baedce"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "021",
                            "(fpsemi) code coverage (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_code_cov<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "022",
                            "(fpsemi) code coverage (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_code_cov<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_ex_3_13_14() {
      auto rg = ReportGuard(REPORT);
      // Example 3.13 + 3.14
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("abbba", "cdc");
      std::string p = "c";
      REQUIRE(k.normal_form("cdcdcabbbabbbabbcd") == "abbbadcabbbabbbabbcd");
      REQUIRE(k.equal_to(k.normal_form("cdcdcabbbabbbabbcd"),
                         "cdcdcabbbabbbabbcd"));
      REQUIRE(k.equal_to("abbbadcbbba", "cdabbbcdc"));
      REQUIRE(k.equal_to(k.normal_form("cdabbbcdc"), "cdabbbcdc"));
      REQUIRE(k.normal_form("cdabbbcdc") == "abbbadcbbba");
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "023",
                            "(fpsemi) prefix (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_ex_3_13_14<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "024",
                            "(fpsemi) prefix (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_ex_3_13_14<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_ex_3_15() {
      auto rg = ReportGuard(REPORT);
      // Example 3.15
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("aabc", "acba");
      std::string original = "cbacbaabcaabcacbacba";
      std::string expected = "cbaabcabcaabcaabcabc";

      REQUIRE(k.equal_to("cbaabcabcaabcaabccba", original));
      REQUIRE(k.equal_to(original, expected));
      REQUIRE(k.equal_to(expected, original));
      REQUIRE(k.equal_to("cbaabcabcaabcaabccba", expected));

      REQUIRE(k.equal_to(original, "cbaabcabcaabcaabccba"));

      REQUIRE(k.equal_to(expected, "cbaabcabcaabcaabccba"));
      REQUIRE(k.equal_to(k.normal_form(original), original));
      REQUIRE(k.normal_form(original) == expected);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "025",
                            "(fpsemi) normal_form (Example 3.15) (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_ex_3_15<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "026",
        "(fpsemi) normal_form (Example 3.15) (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_ex_3_15<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_ex_3_16() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("abcd", "acca");
      std::string original = "bbcabcdaccaccabcddd";
      std::string expected = "bbcabcdabcdbcdbcddd";

      REQUIRE(k.equal_to(original, expected));
      REQUIRE(k.equal_to(expected, original));

      REQUIRE(k.normal_form(original) == expected);
      REQUIRE(k.equal_to(k.normal_form(original), original));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "027",
                            "(fpsemi) normal_form (Example 3.16) (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_ex_3_16<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "028",
        "(fpsemi) normal_form (Example 3.16) (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_ex_3_16<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_ex_3_16_again() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("abcd", "acca");

      REQUIRE(std::all_of(
          cbegin_sislo("abcd", "a", "aaaa"),
          cend_sislo("abcd", "a", "aaaa"),
          [&k](std::string const& w) { return k.normal_form(w) == w; }));
      REQUIRE(std::count_if(
                  cbegin_sislo("abcd", "aaaa", "aaaaa"),
                  cend_sislo("abcd", "aaaa", "aaaaa"),
                  [&k](std::string const& w) { return k.normal_form(w) != w; })
              == 1);
      REQUIRE(std::count_if(
                  cbegin_sislo("abcd", "aaaaa", "aaaaaa"),
                  cend_sislo("abcd", "aaaaa", "aaaaaa"),
                  [&k](std::string const& w) { return k.normal_form(w) != w; })
              == 8);
      REQUIRE(std::count_if(
                  cbegin_sislo("abcd", "aaaaaa", "aaaaaaa"),
                  cend_sislo("abcd", "aaaaaa", "aaaaaaa"),
                  [&k](std::string const& w) { return k.normal_form(w) != w; })
              == 48);
      {
        std::string w = k.normal_form("accaccabd");

        REQUIRE(w == "abcdbcdbd");

        REQUIRE(std::count_if(cbegin_sislo("abcd", "aaaaaaaaa", w),
                              cend_sislo("abcd", "aaaaaaaaa", w),
                              [&k, &w](std::string const& u) {
                                return !k.equal_to(u, w);
                              })
                == std::distance(cbegin_sislo("abcd", "aaaaaaaaa", w),
                                 cend_sislo("abcd", "aaaaaaaaa", w)));
      }
      {
        std::string w = k.normal_form("accbaccad");
        REQUIRE(w == "accbabcdd");

        REQUIRE(std::count_if(cbegin_sislo("abcd", "aaaaaaaaa", w),
                              cend_sislo("abcd", "aaaaaaaaa", w),
                              [&k, &w](std::string const& u) {
                                return !k.equal_to(u, w);
                              })
                == std::distance(cbegin_sislo("abcd", "aaaaaaaaa", w),
                                 cend_sislo("abcd", "aaaaaaaaa", w)));
      }
      {
        std::string w = k.normal_form("abcdbcacca");
        REQUIRE(w == "abcdbcabcd");
        REQUIRE(k.equal_to(w, "abcdbcacca"));

        REQUIRE(
            std::count_if(
                cbegin_sislo("abcd", std::string(w.size(), 'a'), w),
                cend_sislo("abcd", std::string(w.size(), 'a'), w),
                [&k, &w](std::string const& u) { return !k.equal_to(u, w); })
            == std::distance(
                cbegin_sislo("abcd", std::string(w.size(), 'a'), w),
                cend_sislo("abcd", std::string(w.size(), 'a'), w)));
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "029",
        "(fpsemi) normal_form (Example 3.16) more exhaustive (std::string)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_ex_3_16_again<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "030",
        "(fpsemi) normal_form (Example 3.16) more exhaustive (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_ex_3_16_again<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_small() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("ab");
      REQUIRE(k.is_obviously_infinite());
      REQUIRE(k.size() == POSITIVE_INFINITY);
      k.add_rule("aaa", "a");
      k.add_rule("a", "bb");
      REQUIRE(k.small_overlap_class() == 1);
      REQUIRE_THROWS_AS(k.size(), LibsemigroupsException);
      REQUIRE_THROWS_AS(k.equal_to("a", "aaa"), LibsemigroupsException);
      REQUIRE(!k.finished());
      k.run();
      REQUIRE(!k.finished());
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "031",
                            "(fpsemi) small presentation (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_small<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "032",
                            "(fpsemi) small presentation (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_small<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_non_smalloverlap() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcdefg");
      k.add_rule("abcd", "aaaeaa");
      k.add_rule("ef", "dg");
      k.add_rule("a", "b");
      REQUIRE(k.small_overlap_class() == 1);
      REQUIRE_THROWS_AS(k.size(), LibsemigroupsException);
      REQUIRE_THROWS_AS(k.equal_to("a", "aaa"), LibsemigroupsException);
      REQUIRE(!k.finished());
      k.run();
      REQUIRE(!k.finished());
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "033",
                            "(fpsemi) non-smalloverlap (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_non_smalloverlap<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "034",
                            "(fpsemi) non-smalloverlap (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_non_smalloverlap<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_3() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("abcd", "accca");
      REQUIRE(k.number_of_pieces(0) == POSITIVE_INFINITY);
      REQUIRE(k.number_of_pieces(1) == 4);

      REQUIRE(k.small_overlap_class() == 4);
      REQUIRE(k.normal_form("bbcabcdaccaccabcddd") == "bbcabcdaccaccabcddd");
      REQUIRE(k.equal_to("bbcabcdaccaccabcddd", "bbcabcdaccaccabcddd"));
      k.run();
      REQUIRE(k.started());
      REQUIRE(k.finished());

      Kambites<T> l(k);
      REQUIRE(l.started());
      REQUIRE(l.finished());

      REQUIRE(l.number_of_pieces(0) == POSITIVE_INFINITY);
      REQUIRE(l.number_of_pieces(1) == 4);

      REQUIRE(l.small_overlap_class() == 4);
      REQUIRE(l.normal_form("bbcabcdaccaccabcddd") == "bbcabcdaccaccabcddd");
      REQUIRE(l.equal_to("bbcabcdaccaccabcddd", "bbcabcdaccaccabcddd"));
      REQUIRE(l.number_of_normal_forms(0, 0) == 0);
      REQUIRE(l.number_of_normal_forms(6, 6) == 0);
      REQUIRE(l.number_of_normal_forms(10, 1) == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "035",
                            "(fpsemi) MT test 3 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_3<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "036",
                            "(fpsemi) MT test 3 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_3<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_5() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abc");
      k.add_rule("ac", "cbbbbc");
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(k.normal_form("acbbbbc") == "aac");
      REQUIRE(k.equal_to("acbbbbc", "aac"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "037",
                            "(fpsemi) MT test 5 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_5<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "038",
                            "(fpsemi) MT test 5 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_5<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_6() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abc");
      k.add_rule("ccab", "cbac");
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(k.normal_form("bacbaccabccabcbacbac") == "bacbacbaccbaccbacbac");
      REQUIRE(k.equal_to("bacbaccabccabcbacbac", "bacbacbaccbaccbacbac"));
      REQUIRE(k.normal_form("ccabcbaccab") == "cbaccbacbac");
      REQUIRE(k.equal_to("ccabcbaccab", "cbaccbacbac"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "039",
                            "(fpsemi) MT test 6 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_6<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "040",
                            "(fpsemi) MT test 6 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_6<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_10() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcdefghij");
      k.add_rule("afh", "bgh");
      k.add_rule("hc", "de");
      k.add_rule("ei", "j");
      REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);

      REQUIRE(k.normal_form("bgdj") == "afdei");
      REQUIRE(k.equal_to("bgdj", "afdei"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "041",
                            "(fpsemi) MT test 10 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_6<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "042",
                            "(fpsemi) MT test 10 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_6<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_13() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("abcd", "dcba");
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(k.normal_form("dcbdcba") == "abcdbcd");
      REQUIRE(k.equal_to("dcbdcba", "abcdbcd"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "043",
                            "(fpsemi) MT test 13 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_13<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "044",
                            "(fpsemi) MT test 13 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_13<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////
    template <typename T>
    void test_case_mt_14() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("abca", "dcbd");
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(k.normal_form("dcbabca") == "abcacbd");
      REQUIRE(k.equal_to("dcbabca", "abcacbd"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "045",
                            "(fpsemi) MT test 14 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_14<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "046",
                            "(fpsemi) MT test 14 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_14<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_15() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("abcd", "dcba");
      k.add_rule("adda", "dbbd");
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(k.normal_form("dbbabcd") == "addacba");
      REQUIRE(k.equal_to("dbbabcd", "addacba"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "047",
                            "(fpsemi) MT test 15 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_15<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "048",
                            "(fpsemi) MT test 15 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_15<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_16() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcdefg");
      k.add_rule("abcd", "acca");
      k.add_rule("gf", "ge");
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(k.normal_form("accabcdgf") == "abcdbcdge");
      REQUIRE(k.equal_to("accabcdgf", "abcdbcdge"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "049",
                            "(fpsemi) MT test 16 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_16<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "050",
                            "(fpsemi) MT test 16 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_16<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_mt_17() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("ababbabbbabbbb", "abbbbbabbbbbbabbbbbbbabbbbbbbb");
      k.add_rule("cdcddcdddcdddd", "cdddddcddddddcdddddddcdddddddd");
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(k.normal_form("abbbacdddddcddddddcdddddddcdddddddd")
              == "abbbacdcddcdddcdddd");
      REQUIRE(k.equal_to("abbbacdddddcddddddcdddddddcdddddddd",
                         "abbbacdcddcdddcdddd"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "051",
                            "(fpsemi) MT test 17 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_17<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "052",
                            "(fpsemi) MT test 17 (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_mt_17<MultiStringView>();
    }

    ///////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_weak_1() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("acba", "aabc");
      k.add_rule("acba", "dbbbd");

      REQUIRE(k.small_overlap_class() == 4);
      REQUIRE(k.equal_to("aaabc", "adbbbd"));
      REQUIRE(k.equal_to("adbbbd", "aaabc"));
      REQUIRE(number_of_words(4, 4, 6) == 1280);

      REQUIRE(std::count_if(
                  cbegin_sislo("abcd", "aaaa", "aaaaaa"),
                  cend_sislo("abcd", "aaaa", "aaaaaa"),
                  [&k](std::string const& w) { return k.equal_to("acba", w); })
              == 3);

      REQUIRE(k.equal_to("aaabcadbbbd", "adbbbdadbbbd"));
      REQUIRE(k.equal_to("aaabcaaabc", "adbbbdadbbbd"));
      REQUIRE(k.equal_to("acba", "dbbbd"));
      REQUIRE(k.equal_to("acbabbbd", "aabcbbbd"));
      REQUIRE(k.equal_to("aabcbbbd", "acbabbbd"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "053",
                            "(fpsemi) weak C(4) not strong x 1 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_1<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "054",
        "(fpsemi) weak C(4) not strong x 1 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_1<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_weak_2() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("acba", "aabc");
      k.add_rule("acba", "adbd");
      REQUIRE(k.equal_to("acbacba", "aabcabc"));
      REQUIRE(k.normal_form("acbacba") == "aabcabc");
      REQUIRE(k.equal_to(k.normal_form("acbacba"), "aabcabc"));
      REQUIRE(k.equal_to("aabcabc", k.normal_form("acbacba")));

      REQUIRE(std::count_if(
                  cbegin_sislo("abcd", "aaaa", "aaaaaa"),
                  cend_sislo("abcd", "aaaa", "aaaaaa"),
                  [&k](std::string const& w) { return k.equal_to("acba", w); })
              == 3);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "055",
                            "(fpsemi) weak C(4) not strong x 2 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_2<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "056",
        "(fpsemi) weak C(4) not strong x 2 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_2<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_weak_3() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcde");
      k.add_rule("bceac", "aeebbc");
      k.add_rule("aeebbc", "dabcd");
      REQUIRE(k.normal_form("bceacdabcd") == "aeebbcaeebbc");
      REQUIRE(k.equal_to(k.normal_form("bceacdabcd"), "aeebbcaeebbc"));
      REQUIRE(k.equal_to("aeebbcaeebbc", k.normal_form("bceacdabcd")));

      REQUIRE(std::count_if(
                  cbegin_sislo("abcd", "aaaa", "aaaaaa"),
                  cend_sislo("abcd", "aaaa", "aaaaaa"),
                  [&k](std::string const& w) { return k.equal_to("acba", w); })
              == 1);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "057",
                            "(fpsemi) weak C(4) not strong x 3 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_3<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "058",
        "(fpsemi) weak C(4) not strong x 3 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_3<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_weak_4() {
      auto        rg = ReportGuard(REPORT);
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("acba", "aabc");
      k.add_rule("acba", "dbbd");
      REQUIRE(k.normal_form("bbacbcaaabcbbd") == "bbacbcaaabcbbd");
      REQUIRE(k.equal_to(k.normal_form("bbacbcaaabcbbd"), "bbacbcaaabcbbd"));
      REQUIRE(k.equal_to("bbacbcaaabcbbd", k.normal_form("bbacbcaaabcbbd")));
      REQUIRE(k.normal_form("acbacba") == "aabcabc");
      REQUIRE(k.equal_to(k.normal_form("acbacba"), "aabcabc"));
      REQUIRE(k.equal_to("aabcabc", k.normal_form("acbacba")));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "059",
                            "(fpsemi) weak C(4) not strong x 4 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_4<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "060",
        "(fpsemi) weak C(4) not strong x 4 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_4<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_weak_5() {
      Kambites<T> k;
      k.set_alphabet("abcde");
      k.add_rule("abcd", "aaeaaa");
      REQUIRE(number_of_subwords(k.cbegin_rules(), k.cend_rules()) == 16);
      size_t n = sum_lengths(k.cbegin_rules(), k.cend_rules());
      REQUIRE(n == 10);
      REQUIRE(n * n == 100);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "061",
                            "(fpsemi) weak C(4) not strong x 5 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_5<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "062",
        "(fpsemi) weak C(4) not strong x 5 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_5<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_weak_6() {
      Kambites<T> k;
      k.set_alphabet("abcd");
      k.add_rule("acba", "aabc");
      k.add_rule("acba", "adbd");
      REQUIRE(k.normal_form("acbacba") == "aabcabc");
      REQUIRE(k.equal_to(k.normal_form("acbacba"), "aabcabc"));
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "063",
                            "(fpsemi) weak C(4) not strong x 6 (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_6<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "064",
        "(fpsemi) weak C(4) not strong x 6 (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_weak_6<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_konovalov() {
      Kambites<T> k;
      k.set_alphabet("abAB");
      k.add_rule("Abba", "BB");
      k.add_rule("Baab", "AA");
      REQUIRE(k.small_overlap_class() == 2);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "065",
                            "(fpsemi) Konovalov example (std::string)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_konovalov<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "066",
                            "(fpsemi) Konovalov example (MultiStringView)",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      test_case_konovalov<MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void test_case_long_words() {
      Kambites<T> k;
      k.set_alphabet("abcde");
      k.add_rule("bceac", "aeebbc");
      k.add_rule("aeebbc", "dabcd");
      REQUIRE(k.small_overlap_class() == 4);

      std::string w1  = "bceac";
      std::string w2  = "dabcd";
      std::string w3  = "aeebbc";
      auto        lhs = random_power_string(w1, w2, w3, 4000);
      auto        rhs = random_power_string(w1, w2, w3, 4000);
      for (size_t i = 0; i < 10; ++i) {
        REQUIRE(k.equal_to(lhs, rhs));
      }
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "067",
        "(fpsemi) long words (std::string)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_long_words<std::string>();
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "068",
        "(fpsemi) long words (MultiStringView)",
        "[quick][kambites][fpsemigroup][fpsemi][no-valgrind]") {
      test_case_long_words<detail::MultiStringView>();
    }

    ////////////////////////////////////////////////////////////////////////
    // Some tests for exploration of the space of all 2-generator 1-relation
    // semigroups
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    auto count_2_gen_1_rel(size_t min, size_t max) {
      Sislo x;
      x.alphabet("ab");
      x.first(min);
      x.last(max);

      auto last = x.cbegin();
      std::advance(last, std::distance(x.cbegin(), x.cend()) - 1);

      uint64_t total_c4 = 0;
      uint64_t total    = 0;

      for (auto it1 = x.cbegin(); it1 != last; ++it1) {
        for (auto it2 = it1 + 1; it2 != x.cend(); ++it2) {
          total++;
          Kambites<std::string> k;
          k.set_alphabet("ab");
          k.add_rule(*it1, *it2);
          if (k.small_overlap_class() >= 4) {
            total_c4++;
          }
        }
      }
      return std::make_pair(total_c4, total);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "069",
        "(fpsemi) almost all 2-generated 1-relation monoids are C(4)",
        "[extreme][kambites][fpsemigroup][fpsemi]") {
      auto x = count_2_gen_1_rel<std::string>(1, 7);
      REQUIRE(x.first == 1);
      REQUIRE(x.second == 7875);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "070",
        "(fpsemi) almost all 2-generated 1-relation monoids are C(4)",
        "[extreme][kambites][fpsemigroup][fpsemi]") {
      auto x = count_2_gen_1_rel<std::string>(1, 11);
      REQUIRE(x.first == 18171);
      REQUIRE(x.second == 2092035);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "071",
        "(fpsemi) almost all 2-generated 1-relation monoids are C(4)",
        "[extreme][kambites][fpsemigroup][fpsemi]") {
      auto x = count_2_gen_1_rel<std::string>(1, 12);
      REQUIRE(x.first == 0);
      REQUIRE(x.second == 0);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "072",
        "(fpsemi) almost all 2-generated 1-relation monoids are C(4)",
        "[extreme][kambites][fpsemigroup][fpsemi]") {
      auto x = count_2_gen_1_rel<std::string>(1, 13);
      REQUIRE(x.first == 0);
      REQUIRE(x.second == 0);
    }

    LIBSEMIGROUPS_TEST_CASE(
        "Kambites",
        "073",
        "(fpsemi) almost all 2-generated 1-relation monoids are C(4)",
        "[extreme][kambites][fpsemigroup][fpsemi]") {
      std::cout.precision(10);
      size_t const sample_size = 1000;
      std::cout << "Sample size = " << sample_size << std::endl;
      for (size_t i = 8; i < 100; ++i) {
        size_t const min = 7;
        size_t const max = i + 1;
        auto         x   = sample("ab", 1, min, max, sample_size);
        std::cout << "Estimate of C(4) / non-C(4) (length " << min << " to "
                  << max << ") =           " << std::fixed
                  << double(std::get<0>(x)) / sample_size << std::endl;
        std::cout << "Estimate of confluent / non-confluent (length " << min
                  << " to " << max << ") = " << std::fixed
                  << double(std::get<1>(x)) / sample_size << std::endl
                  << std::endl;
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Some tests for exploration of the space of all 3-generator 1-relation
    // semigroups
    ////////////////////////////////////////////////////////////////////////

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "074",
                            "(fpsemi) 3-generated 1-relation C(4)-semigroups",
                            "[extreme][kambites][fpsemigroup][fpsemi]") {
      auto   first = cbegin_sislo("abc", "a", std::string(8, 'a'));
      auto   last  = cbegin_sislo("abc", "a", std::string(8, 'a'));
      size_t N
          = std::distance(first, cend_sislo("abc", "a", std::string(8, 'a')));
      REQUIRE(N == 3279);
      std::advance(last, N - 1);

      size_t total_c4 = 0;
      size_t total    = 0;
      auto   llast    = last;
      ++llast;
      std::unordered_set<std::string> set;

      for (auto it1 = first; it1 != last; ++it1) {
        auto it2 = it1;
        ++it2;
        for (; it2 != llast; ++it2) {
          total++;
          Kambites<std::string> k;
          k.set_alphabet("abc");
          k.add_rule(*it1, *it2);
          if (k.small_overlap_class() >= 4) {
            auto tmp = *it1 + "#" + *it2;
            if (set.insert(tmp).second) {
              auto u = swap_a_b_c(*it1);
              auto v = swap_a_b_c(*it2);
              for (size_t i = 0; i < 5; ++i) {
                if (shortlex_compare(u[i], v[i])) {
                  set.insert(u[i] + "#" + v[i]);
                } else {
                  set.insert(v[i] + "#" + u[i]);
                }
              }
              std::cout << *it1 << " = " << *it2 << std::endl;
              total_c4++;
            }
          }
        }
      }
      REQUIRE(total_c4 == 307511);
      REQUIRE(total == 5374281);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "079",
                            "(fpsemi) normal form possible bug",
                            "[standard][kambites][fpsemigroup][fpsemi]") {
      // There was a bug in MultiStringView::append, that caused this test to
      // fail, so we keep this test to check that the bug in
      // MultiStringView::append is resolved.
      Kambites<MultiStringView> k;
      k.set_alphabet("ab");
      k.add_rule("aaabbab", "bbbaaba");

      std::vector<std::string> words = {
          "bbbaabaabbbbbaabaabaaabbaabbbbbaaabaaabababbbbaaabbabababbaabbabaabb"
          "aaabbabbaaaabbabbbbbbaabbbbaabbabaaabaaaaabbaabababababaaaabaabbabba"
          "bbaaabbabababbabaabbbbbbabaabbabaaaababbababbabbabbabbbabbbabbbabbbb"
          "aaaaaabbabbaababbbaaababbbbababababbabaabbbbbabaaaababaaabbaaabbaaab"
          "babaabbbaababbbaaabbaabbbbaabbbbaaaaababbabbbaaaaaababbbbaaabbbabbba"
          "babbbbbbaabaabababbabbbbbaaaabbbbabbababbbaaaabbbbaabbbbbabbbbbabaab"
          "bbaaabaaabbababbbabbaaaaaabbbbabababbaabbabbbbabbabbaabbbaaabaaabbab"
          "abbbabbbbaabaaababbabbaababbbabbaababbabbbbabbbbabaabaaaabaaaabababa"
          "abababbaaabbabbbbbbaaaaaabbbbabbabbabaaaaabaabbaababbbbaabaaabbabaaa"
          "abaaabbbaaaabbabbababaaaabbbbaaabbababababaabbaaaabaabbababbabbaaaba"
          "bbaaabbbbbabbbaababaaabbababbbbbaabbbabaaaaabbbbabbabaaaababbabbabab"
          "aabbaababbaaabbabbbbabbbaaabbabbbaabbababbaabbaaaaaabaaabbbaababbaaa"
          "ababaabbaaabbbaabababbbbbababbbbbbbaabbbbaabababbbaabbbbbbbaabbbbaaa"
          "babaaaabaababbbaabaaabaaabaaaaabaabbbbabbabaaabbabbaabbaabbabaaabbbb"
          "baaabababbaabbbaababababaababbaabbabaaaaabaaabaaababaababaaaaaababaa"
          "aaaaaabaababbbbaabaabbabbabaaaaaabaabbabbbabbaabbbbbbbaaaababababbbb"
          "ababbbabbbaaabbabbabbaabbbbbbbababaabaabababbaaabbaabbbaabbbabbbbbab"
          "aaabbbababbbaabaaaabaabbaaaabbabbbabababbaaabbbbaabaabbababaaaabbbaa"
          "aabbbaabaa",
          "aaabbababbbbbaabaabaaabbaabbbbbaaabaaabababbbbaaabbabababbaabbabaabb"
          "bbbaababaaaabbabbbbbbaabbbbaabbabaaabaaaaabbaabababababaaaabaabbabba"
          "bbaaabbabababbabaabbbbbbabaabbabaaaababbababbabbabbabbbabbbabbbabbbb"
          "aaabbbaababaababbbaaababbbbababababbabaabbbbbabaaaababaaabbaaabbaaab"
          "babaabbbaababbbaaabbaabbbbaabbbbaaaaababbabbbaaaaaababbbbaaabbbabbba"
          "babbbbbbaabaabababbabbbbbaaaabbbbabbababbbaaaabbbbaabbbbbabbbbbabaab"
          "bbaaabbbbaabaabbbabbaaaaaabbbbabababbaabbabbbbabbabbaabbbaaabaaabbab"
          "abbbabbbbaabaaababbabbaababbbabbaababbabbbbabbbbabaabaaaabaaaabababa"
          "abababbaaabbabbbbbbaaaaaabbbbabbabbabaaaaabaabbaababbbbaabaaabbabaaa"
          "abaaabbbabbbaababababaaaabbbbaaabbababababaabbaaaabaabbababbabbaaaba"
          "bbaaabbbbbabbbaababaaabbababbbbbaabbbabaaaaabbbbabbabaaaababbabbabab"
          "aabbaababbbbbaababbbabbbaaabbabbbaabbababbaabbaaaaaabaaabbbaababbaaa"
          "ababaabbaaabbbaabababbbbbababbbbbbbaabbbbaabababbbaabbbbbbbaabbbbaaa"
          "babaaaabaabaaaabbabaabaaabaaaaabaabbbbabbabaaabbabbaabbaabbabaaabbbb"
          "baaabababbaaaaabbabbababaababbaabbabaaaaabaaabaaababaababaaaaaababaa"
          "aaaaaabaababbbbaabaabbabbabaaaaaabaabbabbbabbaabbbbbbbaaaababababbbb"
          "ababbbabbbaaabbabbabbaabbbbbbbababaabaabababbaaabbaabbbaabbbabbbbbab"
          "aaabbbabaaaabbabaaabaabbaaaabbabbbabababbaaabbbbaabaabbababaaaabbbaa"
          "aabbbaabaa",
          "bbbaabaabbbbabaaaaababbbaababbabbabbaabaaabbaaabbabbbabbbaaaababaaab"
          "baaabbabbbbaaabbabaaaaaaababbaaabbaabbaabaabbabbaabaabbababbbbbbbbaa"
          "aaaaaabbabbbabaaababbbbbabababbbaaabbaaaaaabbbbbbabbabbbaaaaabbabbab"
          "bbbaaaaabbabbabbbbababbbababbbaaabaabbabaabbaaaaabbababbaabbbababbaa"
          "abbabaaabbabaaaaaaabbaababbaabbbabbabaaaabaabaaabbbbaaaabbbaaaaaaabb"
          "aabaaabbbaababbaaabbbbaabbabbbbabbbababbabbbbababbbbbbaaabaabaababab"
          "aabbabbbaaabbabbaaabaabbbbaabbaabaabaababbabbaabaabbabbbbbaaabbaaabb"
          "abbbbababbaaabbabbbaabaaabaaaaaababbaaabbbbbababbaabbaaaabbaaaaabaaa"
          "aaabbbaaaaaaaabbabbbaabaaaabaababbaaabbbbbabaaaabbbabaaaaabbaabbaabb"
          "bbaaabbbbaabaabbaaabbbbaabbbaaaaaabbbabbaabbaabbabbbabaabbbbaabababa"
          "abbbbbbaaaabbabbbbabbaaabbbabbabaaabbabbabbbabbbaaaabbbaaabbbaabaabb"
          "aaabaabbabbbbaabaaabaabbaaababaabbabaaabaabbaaabaaababbaabbbbbababba"
          "abbabbabbbaaabbabaaaabbbaaaaabbbbbbbabbbabbbababbbabaaababababaaaaba"
          "aaaaaaaabbabaaabbabbbabbaaababababaaabbabbbbababbbaaaaababaaaabbabaa"
          "babbaaaaabaaaaabbabbbbbbbbbaabbaabaabbabbaabbabaabaaaabaababaababbaa"
          "aabaabaababbaaaaabbabbababbabbbaabbbbbaaabbbaabaaaaabaaabbbaaabbbaba"
          "bbbbbabbabbaaaabbbaababbababbabaabaabbbbaaabaaabbbabbbbbabaaaabaabaa"
          "bbbabbbbaabbbaaabbbbaabaababbaabbabbabaaabbaaaababbabbaabbbabaabbbba"
          "aaabbbaaaaabaaabab",
          "aaabbababbbbabaaaaababbbaababbabbabbaabaaabbaaabbabbbabbbaaaababaaab"
          "baaabbabbbbaaabbabaaaaaaababbaaabbaabbaabaabbabbaabaabbababbbbbbbbaa"
          "aaabbbaababbabaaababbbbbabababbbaaabbaaaaaabbbbbbabbabbbaaaaabbabbab"
          "bbbaaaaabbabbabbbbababbbababbbaaabaabbabaabbaaaaabbababbaabbbababbaa"
          "abbabbbbaabaaaaaaaabbaababbaabbbabbabaaaabaabaaabbbbaaaabbbaaaaaaabb"
          "aabaaabbbaababbaaabbbbaabbabbbbabbbababbabbbbababbbbbbaaabaabaababab"
          "aabbabbbaaabbabbaaabaabbbbaabbaabaabaababbabbaabaabbabbbbbaaabbaaabb"
          "abbbbababbbbbaababbaabaaabaaaaaababbaaabbbbbababbaabbaaaabbaaaaabaaa"
          "aaabbbaaaaaaaabbabbbaabaaaabaababbaaabbbbbabaaaabbbabaaaaabbaabbaabb"
          "bbaaabbbbaabaabbaaabbbbaabbbaaaaaabbbabbaabbaabbabbbabaabbbbaabababa"
          "abbbbbbabbbaababbbabbaaabbbabbabaaabbabbabbbabbbaaaabbbaaabbbaabaabb"
          "aaabaabbabbbbaabaaabaabbaaababaabbabaaabaabbaaabaaababbaabbbbbababba"
          "abbabbabbbbbbaabaaaaabbbaaaaabbbbbbbabbbabbbababbbabaaababababaaaaba"
          "aaaaaaaabbabaaabbabbbabbaaababababaaabbabbbbababbbaaaaababaaaabbabaa"
          "babbaaaaabaabbbaababbbbbbbbaabbaabaabbabbaabbabaabaaaabaababaababbaa"
          "aabaabaababbaaaaabbabbababbabbbaabbbbbaaabbbaabaaaaabaaabbbaaabbbaba"
          "bbbbbabbabbaaaabbbaababbababbabaabaabbbbaaabaaabbbabbbbbabaaaabaabaa"
          "bbbabbbbaabbbaaabbbbaabaababbaabbabbabaaabbaaaababbabbaabbbabaabbbba"
          "aaabbbaaaaabaaabab",
          "bbbaababbaabbababbbaabbbbaaaaaaabbaabbbbbabaababaababbbbabaabbbaabbb"
          "aabaaabbbaabbbabbabbbbabbbabbbbbaaaaaaabaabbbbaabbbbbbaabbaabaabaaba"
          "aabbabbaababbbbababaaaabaababbaababbbbabaabbbabbabaababaaabaaabbbaba"
          "bbbaabaababbbbaaaaabaaaababaababbababaaabaaaaaabbaabaababbbbaaabaaaa"
          "bbaaabbabaaabbababbbabbbbbbababbaabbaaaababbbbaabbbaababbaabaababbbb"
          "aabbbbaabababbbabaabbaaaabaabbbabbbaabaabbabbaababbbbbbbabbbbbbbabaa"
          "bbbaaaaabbabbbbabbbbabbbaaabbbbaabbbabaabaabaabbaaaaabbbababaaabbaaa"
          "bbbbbabaaabbabbaabbbaaabbabbbbbbabbabaaabbbabbbabaabbabbabababbabbaa"
          "ababaabbbbbbaababbbbbbbaaaaaaabaababbaaababbbbbaaaaaaaabbbbabaabbbab"
          "babaabababaaaabbababbabaabbaababaabbbbbabaaabbbbabaababaaaaaaababbbb"
          "bbbbbbbbbaaabbabbbbaaabaabbbabaabaabaaaabaabbbbbbabbaaabbabaaabbbaba"
          "abaaabbbbabbbaababaaabbaaabaabababbabababaaabbabaabbabbaaaabbbbabbab"
          "abbabbababbbbbaababbaabbabaabbaaabaaaababbbbaaaabbabbaaaabaaabbbbaba"
          "bbbbbaaabbaaaabbabbabaaaabbabbaaaababbbaababbabbbaababaaabababbabbab"
          "babbbabbbaababbbaababbbbbbbbababbbabababbababbbaaabbaababaabbbaaabbb"
          "bbaaabababaaabbbbbaabaaababababaabbbbbbabbbabaaabaabababbbabaaabaabb"
          "bbaabbaababbbabaaabbabaaaaaabbaaaababbaabbbaababbaaababbbaabaabbbbbb"
          "ababbbbbbbbaabaabbbaabaaaabababbaaabaabaababaabababbabbabbbaabbbbaba"
          "baaababbbbabbaaa",
          "aaabbabbbaabbababbbaabbbbaaaaaaabbaabbbbbabaababaababbbbabaabbbaaaaa"
          "bbabaabbbaabbbabbabbbbabbbabbbbbaaaaaaabaabbbbaabbbbbbaabbaabaabaabb"
          "bbaababaababbbbababaaaabaababbaababbbbabaabbbabbabaababaaabaaabbbaba"
          "bbbaabaababbbbaaaaabaaaababaababbababaaabaaaaaabbaabaababbbbaaabaaaa"
          "bbaaabbabaaabbababbbabbbbbbababbaabbaaaababbbbaabbbaababbaabaababbbb"
          "aabbbbaabababbbabaabbaaaabaabbbabbbaabaabbabbaababbbbbbbabbbbbbbabaa"
          "bbbaabbbaababbbabbbbabbbaaabbbbaabbbabaabaabaabbaaaaabbbababaaabbaaa"
          "bbbbbabbbbaababaabbbaaabbabbbbbbabbabaaabbbabbbabaabbabbabababbabbaa"
          "ababaabbbaaabbabbbbbbbbaaaaaaabaababbaaababbbbbaaaaaaaabbbbabaabbbab"
          "babaababababbbaabaabbabaabbaababaabbbbbabaaabbbbabaababaaaaaaababbbb"
          "bbbbbbbbbaaabbabbbbaaabaabbbabaabaabaaaabaabbbbbbabbaaabbabaaabbbaba"
          "abaaabbbbaaaabbabbaaabbaaabaabababbabababaaabbabaabbabbaaaabbbbabbab"
          "abbabbababbaaabbabbbaabbabaabbaaabaaaababbbbaaaabbabbaaaabaaabbbbaba"
          "bbbbbaaabbabbbaabababaaaabbabbaaaababbbaababbabbbaababaaabababbabbab"
          "babbbabbbaabaaaabbabbbbbbbbbababbbabababbababbbaaabbaababaabbbaaabbb"
          "bbaaabababaaabbbbbaabaaababababaabbbbbbabbbabaaabaabababbbabaaabaabb"
          "bbaabbaababbbabaaabbabaaaaaabbaaaababbaabbbaababbaaababbbaabaabbbbbb"
          "ababbbbbbbbaabaaaaabbabaaabababbaaabaabaababaabababbabbabbbaabbbbaba"
          "baaababbbbabbaaa",
          "bbbaabaaaabababaabbbbbbbabbbaaabbbabbabbbbbabaaaabaaaabaabbbaabbbbbb"
          "bbaaabbabbabaaabbaaaaaabbbabaaaaabababbbbabbbaaaabbbabbaaabbbabbbabb"
          "aababbbaababaaaaabaaaaababaabbaaaaaaabbbaaaaaaaaaaaaaabbaabbababbabb"
          "bababaaaabbababbabbabbbaaaabbaaaababaabaababaabbaababaaaabbbbbbbbaba"
          "babbbbbabbbaabaabaabaaababbababaababaaaaaababbabaabaabbbabaaaabbbabb"
          "aaabbbaabbaaabbabbbabababbabbbaaaaabbaaabaaabaabbaabbbbbbbbaaabaaaab"
          "babbbbbbaaabaaabbabbbbabbbbbaabbabaabbbaaaaababaaaaababbbabbabbabbbb"
          "bbababaaabbaaabbbaababaabaaabbaabababbbbaabbaabbabaaaabbbabbbaabaabb"
          "baaababbbbbbbbaababbaabbbbaaaaaabababababbaababbbabaaaabbbaaabbbbaba"
          "baaaaaabbbabbbbbaabaaaaabbabbaabaaaabbbaaabaaabbabaabaabbbababaaaabb"
          "babbabaabababaaaaabbabbbaabbbaababbaaaababbbabbaaabababbbaaabbababab"
          "baaabbbbbbbbaaabbbbaabababaaaaaabaaabbabaabbabbababbaabaaabaababaaab"
          "babaabbbbbbbbbbbbbbaabaababbbababaaaaaaabababbbbababbaababababbbabbb"
          "abbaabaaaabbabaaaaaaabbabbabaaabaaabbabbababbaaaaaababbababbaababbbb"
          "aababbbbbbaabbbabaabaaabbabaababbabaaaaabbbabaabaaababaaaaaaaaabaaab"
          "bbabbbbabaaabaaaabbaabbbaabaaabbaabbbbaaabbbbbbaabbbabbababbbabaaabb"
          "baaaabbabababbababbabbabbbaababaaabaaabbabaaaabbbbabaaabaababbbaabba"
          "babbbbbbabbababaabaabbbabaaabbabababbbbbabaaababbbabaabbbbaabbbbabba"
          "abaabbaabaaaaabaaabaabbbaaa",
          "aaabbabaaabababaabbbbbbbabbbaaabbbabbabbbbbabaaaabaaaabaabbbaabbbbbb"
          "bbbbbaabababaaabbaaaaaabbbabaaaaabababbbbabbbaaaabbbabbaaabbbabbbabb"
          "aababbbaababaaaaabaaaaababaabbaaaaaaabbbaaaaaaaaaaaaaabbaabbababbabb"
          "babababbbaabaabbabbabbbaaaabbaaaababaabaababaabbaababaaaabbbbbbbbaba"
          "babbbbbaaaabbababaabaaababbababaababaaaaaababbabaabaabbbabaaaabbbabb"
          "aaabbbaabbbbbaababbabababbabbbaaaaabbaaabaaabaabbaabbbbbbbbaaabaaaab"
          "babbbbbbaaabbbbaababbbabbbbbaabbabaabbbaaaaababaaaaababbbabbabbabbbb"
          "bbababaaabbaaaaaabbabbaabaaabbaabababbbbaabbaabbabaaaabbbabbbaabaabb"
          "baaababbbbbaaabbabbbaabbbbaaaaaabababababbaababbbabaaaabbbaaabbbbaba"
          "baaaaaabbbabbbbbaabaaaaabbabbaabaaaabbbaaabaaabbabaabaabbbababaaaabb"
          "babbabaabababaaaaabbabbbaabbbaababbaaaababbbabbaaabababbbaaabbababab"
          "baaabbbbbbbbaaabbbbaabababaaaaaabaaabbabaabbabbababbaabaaabaababaaab"
          "babaabbbbbbbbbbbbbbaabaababbbababaaaaaaabababbbbababbaababababbbabbb"
          "abbaabaaaabbabaaaabbbaabababaaabaaabbabbababbaaaaaababbababbaababbbb"
          "aababbbbbbaabbbabaabbbbaabaaababbabaaaaabbbabaabaaababaaaaaaaaabaaab"
          "bbabbbbabaaabaaaabbaaaaabbabaabbaabbbbaaabbbbbbaabbbabbababbbabaaabb"
          "baaaabbabababbababbabbaaaabbabbaaabaaabbabaaaabbbbabaaabaababbbaabba"
          "babbbbbbabbababaabaabbbabaaabbabababbbbbabaaababbbabaabbbbaabbbbabba"
          "abaabbaabaaaaabaaabaabbbaaa, "
          "bbbaababbababbaaaabbaabbabbbbaababbabbbabbbababbbbbaaababbabbababaaa"
          "abbabbabbbbaaaaaaaaabbbabaaabbbbababaaaabaabbbbbbaababbaaaaabbababab"
          "aaabbabbbbbbabbbbbaababbbbaabaabaaabbababaaabaaabbbaaaaabbbaababbbba"
          "abaaabbababaabababbababbbababbbabbababbabaabbbabbabbaaaabbbbabbbbbbb"
          "ababbbaabaaaabaaaaaaaaabbabbabaaaabbbaabbaababababaaabbbabbaabbbbaba"
          "aaabbabbabbaaaababababaabbbbaabbababbbbbabbabbbbbbaabbabaabbabbbbabb"
          "abbbaabaaabbbbbbabbaaabbaaabbbababaabababbabbbaababaabbaabbabbbbaabb"
          "babbbbaabaabaaaabaaabbbbaaaaaaaabbbbbbabaabbabbbaaabababbbabaaababbb"
          "bbabaaabbabaabbbbbabbabaababbabbabbabaabbbaaaaabbbaabbbaaaabaabababb"
          "bbabaaaaabbabaaaabbbaabbbbbbbabbabbababbaaaaabababaaabbbbbabaabbbabb"
          "bbaabaaaaabbabaabbabbaabbbabaabbbaaaaabbababbbaaaabaabababaaabbbbbaa"
          "abbaaababbbaabaaaaaaababbbbabbbabaaaababbaababaababaaababbabbbabbaba"
          "ababbbaabbaaabbabaabaaaabbbbbababbbbabbababbbabaabaabbbabbabbabaaaaa"
          "abbabaababaaaaabbabbaaaaaaabaaaabbaaabaababbaababaaabbbbaaaababaaaba"
          "abbabababaababaaabbabbbaababbbabbbbabbaaaabaabbbababbbbbbabaabbbbaba"
          "abbbaabbbbbabbbbbaabababbbbaabbbbbabbbbabbaaababbabaabbabababaababab"
          "bbbbaaaabbaaabaaaaabbabaaaaaaaabbbaababbaaabbbabbbbaaabaababbaababbb"
          "ababbaaaabbbbaaaaaaaabbabbbbbababaabbababbaabbaaaaaaaabbabbbabbbbaab"
          "aabbbaabbbaaabaaaabb",
          "bbbaababbababbaaaabbaabbabbbbaababbabbbabbbababbbbbaaababbabbababaaa"
          "abbabbabbbbaaaaaaaaabbbabaaabbbbababaaaabaabbbbbbaababbaaaaabbababab"
          "aaabbabbbbbbabbbbbaababbbbaabaabaaabbababaaabaaabbbaaaaabbbaababbbba"
          "abaaabbababaabababbababbbababbbabbababbabaabbbabbabbaaaabbbbabbbbbbb"
          "ababbbaabaaaabaaaaaaaaabbabbabaaaabbbaabbaababababaaabbbabbaabbbbaba"
          "bbbaabababbaaaababababaabbbbaabbababbbbbabbabbbbbbaabbabaabbabbbbabb"
          "abbbaabaaabbbbbbabbaaabbaaabbbababaabababbabbbaababaabbaabbabbbbaabb"
          "babbbbaabaabaaaabaaabbbbaaaaaaaabbbbbbabaabbabbbaaabababbbabaaababbb"
          "bbabbbbaabaaabbbbbabbabaababbabbabbabaabbbaaaaabbbaabbbaaaabaabababb"
          "bbabaaaaabbabaaaabbbaabbbbbbbabbabbababbaaaaabababaaabbbbbabaabbbabb"
          "bbaabaabbbaabaaabbabbaabbbabaabbbaaaaabbababbbaaaabaabababaaabbbbbaa"
          "abbaaabaaaabbabaaaaaababbbbabbbabaaaababbaababaababaaababbabbbabbaba"
          "ababbbaabbbbbaabaaabaaaabbbbbababbbbabbababbbabaabaabbbabbabbabaaaaa"
          "abbabaababaabbbaababaaaaaaabaaaabbaaabaababbaababaaabbbbaaaababaaaba"
          "abbabababaababaaabbabbbaababbbabbbbabbaaaabaabbbababbbbbbabaabbbbaba"
          "abbbaabbbbbabbaaabbabbabbbbaabbbbbabbbbabbaaababbabaabbabababaababab"
          "bbbbaaaabbaaabaaaaabbabaaaaaaaabbbaababbaaabbbabbbbaaabaababbaababbb"
          "ababbaaaabbbbaaaaaaaabbabbbbbababaabbababbaabbaaaaaaaabbabbbabbbbaab"
          "aabbbaabbbaaabaaaabb, "
          "aaabbabababbbbbbbabbbbaaaabbbabbabaaabbaaaabbbbbababbabbbbbbbbbabbab"
          "abbbbaabaabababaabbababaababbbaaaabbbbbbbbaaabbbaaabaaabbbbbaaaaabba"
          "babaaabbabbbbabaabbabaababaabababaaabbbbbaaabaabbbbaabbbbbbabaaabbbb"
          "bbaabaababaabbbbaabaabbabbbbbababaaababababbababaabaabbbbbbbbabaabaa"
          "baaabbabaababbabbabbbbbbaaabababbabbbbbbababaabbaaabaabaaabababbbaba"
          "babbbbaabaababaababababaabbbabababbbbabbbbabbbaaaabaaaaaaabbbbabbabb"
          "abbbabbbaabaabbaabbbbaaabbaabbaabaabaababbabababbbbbabaaaaaaababaaba"
          "bbababbbbbaababbaaaabaaaabbbbbbabbbabbbaabbabababbbabbbbbbbbabaabaab"
          "bbaababaaaaabbabbabbbbabbbbbaababbbbbbbbaabbaabbababbbabaaabbbababaa"
          "aaaaabbabbbbaabaabbabbbbabaabababbaaabbbbbaaabaaabbbaaabbbabaaabaaab"
          "aabbbbaabaaaaaaaaabbbbbaabbaabbbabbaaabaabbbbababaaaaabaaabbaaababbb"
          "bbbbbaaabbababbabaabaabababaabaabaaabaabbbaabaabbaabaaaabaabbbbbbbaa"
          "bbaaabaaaabbabbabbabbaaabbabaaaabbbbababbbaabaaaabbbababbbbababbbaaa"
          "baababbbaaaabbabababbbbbbaaaabaabaaababbaaabbaaaaabaaaaabbabbaababab"
          "abaabbbabbaaabbababaaaababbbbabbabbabababaabbbbabbaabaaabbbabbabaaab"
          "abbabaaaabbbbbbaabaaaabaaaaaababbbbbaaaabbabbbbbbbbabbbabbababbbabaa"
          "bbbaaaaaaabaaaaaabbababbbaabbaaabaaaaaabbbababbaabbbaaaabbaabaaaaaab"
          "ababbabbabbababbbbbaabaaabbabababbbabbbabbabbbabaababbbbbabbabbabaab"
          "abababbabbbab, "
          "bbbaabaababbbbbbbabbbbaaaabbbabbabaaabbaaaabbbbbababbabbbbbbbbbabbab"
          "abaaabbababababaabbababaababbbaaaabbbbbbbbaaabbbaaabaaabbbbbaaaaabba"
          "babbbbaababbbabaabbabaababaabababaaabbbbbaaabaabbbbaabbbbbbabaaabbba"
          "aabbabababaabbbbaabaabbabbbbbababaaababababbababaabaabbbbbbbbabaabaa"
          "bbbbaabaaababbabbabbbbbbaaabababbabbbbbbababaabbaaabaabaaabababbbaba"
          "babbbbaabaababaababababaabbbabababbbbabbbbabbbaaaabaaaaaaabbbbabbabb"
          "abbbaaaabbababbaabbbbaaabbaabbaabaabaababbabababbbbbabaaaaaaababaaba"
          "bbababbaaabbabbbaaaabaaaabbbbbbabbbabbbaabbabababbbabbbbbbbbabaabaab"
          "bbaababaabbbaabababbbbabbbbbaababbbbbbbbaabbaabbababbbabaaabbbababaa"
          "aaaaabbabbbbaabaabbabbbbabaabababbaaabbbbbaaabaaabbbaaabbbabaaabaaab"
          "aabaaabbabaaaaaaaabbbbbaabbaabbbabbaaabaabbbbababaaaaabaaabbaaababbb"
          "bbbbbbbbaabaabbabaabaabababaabaabaaabaabbbaabaabbaabaaaabaabbbbbbbaa"
          "bbaaababbbaabababbabbaaabbabaaaabbbbababbbaabaaaabbbababbbbababbbaaa"
          "baababbbabbbaabaababbbbbbaaaabaabaaababbaaabbaaaaabaaaaabbabbaababab"
          "abaabbbabbaaabbababaaaababbbbabbabbabababaabbbbabbaabaaabbbabbabaaab"
          "abbabaaaabbbaaabbabaaabaaaaaababbbbbaaaabbabbbbbbbbabbbabbababbbabaa"
          "bbbaaaaaaabaaabbbaabaabbbaabbaaabaaaaaabbbababbaabbbaaaabbaabaaaaaab"
          "ababbabbabbababbbbbaabaaabbabababbbabbbabbabbbabaababbbbbabbabbabaab"
          "abababbabbbab, "
          "aaabbabaabbabbbbabbaabbaabaaaabbababbbbaaababbbbabbbaaabbabaaabbabba"
          "babbbaababbbaabbbbaabbbbbbbbbaaabbaaabaababbabaaabaabaabaaabaabaaaba"
          "abbabbbaabaababbbbabbbaaababbababaaaaaaabbbabbbbbaaaabbaaabbbbbbabab"
          "bababbbbbaabababbbabbabaaabbabbabaaabbbbabaaaaababbbbabbbbabbabaabba"
          "aaaaabbbbbaabaababbbabbabaabbaababbabaaaaaaabbbbbabbbbbbbbbbbaababab"
          "ababbbbbbbaabababababaabbbbbaabbabbbbbaabbabbbbbaabaabbbbbabaaaaaaab"
          "aabbbababbbbaabaaabbbaaaaaabbbabbabaaabbbabaababbabbbaaabababbabaaba"
          "ababaabbaaaaaabbababbaaabbbabbaabaababbabaabaabaababababaaaaaaaaaaba"
          "aababbaababbaaaabbabbbaabaaababaaabaabaaabbbaabbababbabaaaaaaabababb"
          "abbaabbbbabbaabbbbaabaaabbaabaaaaababbabbaaaabbaabaabbabaababaabaaaa"
          "aabbbbaabababbbabaaabbabaabbaababbaabaaabaababbabbbbaabbaaabbbbbabba"
          "abbbaabaabaaaaaabbaaabbabaabbbbabbababaabbaabbbabbaaabaaaababbaabbab"
          "babbbaabaaaabbababaabbbaababaaaabbbaaabaaaaaaaaaaaaabbabaabaaabbabbb"
          "bbabbababaababaaabbbbbbaabababbabbbbbbbbabbbaaaaabbbababaabaaabbbaba"
          "bbaaabaaabaaaababbaaabbabbbabbbbbbabababbbaaabbabaabbabbabaaaabbabaa"
          "aaaaaabbbbbabaabbbaaaabbababbbbbabbbbbaaaaabbabbbabaabaaabaaaabbaaba"
          "baabbaababaabaaabbabbbbbaabaabaaaaabbaabaababbabbabbbbbbabbbaabbbbab"
          "aaababbbbbbbababbbbbbabbbaabaabaaaaaaaaaaaaabbbabbbabbbaababbbababaa"
          "abaaaaaabbabbaaabaaa, "
          "bbbaabaaabbabbbbabbaabbaabaaaabbababbbbaaababbbbabbbaaabbabaaabbabba"
          "baaaabbabbbbaabbbbaabbbbbbbbbaaabbaaabaababbabaaabaabaabaaabaabaaaba"
          "abbaaaabbabababbbbabbbaaababbababaaaaaaabbbabbbbbaaaabbaaabbbbbbabab"
          "bababbaaabbabbabbbabbabaaabbabbabaaabbbbabaaaaababbbbabbbbabbabaabba"
          "aaaaabbbbbaabaababbbabbabaabbaababbabaaaaaaabbbbbabbbbbbbbbbbaababab"
          "ababbbbaaabbabbabababaabbbbbaabbabbbbbaabbabbbbbaabaabbbbbabaaaaaaab"
          "aabbbababaaabbabaabbbaaaaaabbbabbabaaabbbabaababbabbbaaabababbabaaba"
          "ababaabbaaaaaabbababbaaabbbabbaabaababbabaabaabaababababaaaaaaaaaaba"
          "aababbaababbabbbaababbaabaaababaaabaabaaabbbaabbababbabaaaaaaabababb"
          "abbaabbbbabbaabaaabbabaabbaabaaaaababbabbaaaabbaabaabbabaababaabaaaa"
          "aabbbbaabababbbabbbbaabaaabbaababbaabaaabaababbabbbbaabbaaabbbbbabba"
          "abbbaabaabaaaaaabbaaabbabaabbbbabbababaabbaabbbabbaaabaaaababbaabbab"
          "babbbaabaaaabbababaaaaabbabbaaaabbbaaabaaaaaaaaaaaaabbabaabaaabbabbb"
          "bbabbababaababaaabbbaaabbabbabbabbbbbbbbabbbaaaaabbbababaabaaabbbaba"
          "bbaaabaaabaaaababbaaabbabbbabbbbbbabababbbaaabbabaabbabbabaaaabbabaa"
          "aaaaaabbbbbabaabbbaaaabbababbbbbabbbbbaaaaabbabbbabaabaaabaaaabbaaba"
          "baabbaababaabaaabbabbbbbaabaabaaaaabbaabaababbabbabbbbbbabbbaabbbbab"
          "aaababbbbbbbababbbbbbabbbaabaabaaaaaaaaaaaaabbbabbbabbbaababbbababaa"
          "abaaaaaabbabbaaabaaa",
          "bbbaabaabbaabbababbbbabaabaaaaabaabbbbaabbbbbbbabaababbaabaabaaabaaa"
          "abbbaabaaaabbaabbaaaabababbaaaaabbbbabbaabababbbbbabbaaaaabbabbbbabb"
          "babbbbaababaaaaabbbbaaaabababbaaabbabaaaabaabbabaababbbabbbaaabaabba"
          "abbbbaaabbababbbbabababbaabbabbaaabbbbabbabababbbbbbabbbabbbbaaabaab"
          "aababbbaaabbababbbaabbaaabaabbabbaaaaaaaaaaabbbbabbaaabaabaaaababaaa"
          "aabbbabaaabbababaaaaabaaaababbabaabbabbababbaabbbabbabaabbabaaaababb"
          "babbbaaaabbbaabaaababbabaaaababbbbaaaaabaabbabaababaaaaaaaaabbabbbba"
          "baabaaaaaaabbababbbaabbbbaabbbbaabbbbaabaababbaabbbaaaaabbaabaabbaaa"
          "abaaaabbabaabbbbbabababaababbbbbabbbabbaabaabbaaaaaabbaaabbaabbbbbbb"
          "baabaaaabbabbbabbbaabbababaaabbbbbbbabbaaabbbabbaaaaaabaababbaababba"
          "aaaababaaabbabbaababbabbababbabaaaaabbbababbababaabaaabababbbaabaaab"
          "aabbbabbbbbbaabaaaaababbbabbbabaabababbababbbabaaabbbbbbbabbaaaaaaaa"
          "babbbaabaaabbabaaabaabaaabbbaaaaaaaabbbbaaabaaaabaaabaabbabbaaaabbaa"
          "bbabbaaaabaaabbababbbbaababaabbbbbbababaabababbbabbbaaabbbabbbaabaab"
          "bbabaabbbababbbaababaabaababaaabbbaabbabbaaaaabbbababbababbaaaaababa"
          "bbbaabbbaababbbbbaababaaababbbaabaaabaabbbaaabbbbabaaabbbbabbaaabaab"
          "babaaabbaaabbaaaabbabaaabbbbaabaabbbabaabbbaaabbbabaaabbbaabaaaababa"
          "bbbbaabaaabbbaabaabaaabbaaaabaabbabbabaabbbaaababbbaababaaaabbaaabba"
          "baaaababbab",
          "aaabbababbaabbababbbbabaabaaaaabaabbbbaabbbbbbbabaababbaabaabaaabaaa"
          "abbbaabaaaabbaabbaaaabababbaaaaabbbbabbaabababbbbbabbaaaaabbabbbbabb"
          "babbbbaababaaaaabbbbaaaabababbaaabbabaaaabaabbabaababbbabbbaaabaabba"
          "abbbbbbbaabaabbbbabababbaabbabbaaabbbbabbabababbbbbbabbbabbbbaaabaab"
          "aababbbaaabbababbbaabbaaabaabbabbaaaaaaaaaaabbbbabbaaabaabaaaababaaa"
          "aabbbabaaabbababaaaaabaaaababbabaabbabbababbaabbbabbabaabbabaaaababb"
          "babbbaaaabbbaabaaababbabaaaababbbbaaaaabaabbabaababaaaaaaaaabbabbbba"
          "baabaaaabbbaabaabbbaabbbbaabbbbaabbbbaabaababbaabbbaaaaabbaabaabbaaa"
          "ababbbaabaaabbbbbabababaababbbbbabbbabbaabaabbaaaaaabbaaabbaabbbbbbb"
          "baabaaaabbabbbabbbaabbababaaabbbbbbbabbaaabbbabbaaaaaabaababbaababba"
          "aaaababbbbaababaababbabbababbabaaaaabbbababbababaabaaabababbbaabaaab"
          "aabbbabbbbbbaabaaaaababbbabbbabaabababbababbbabaaabbbbbbbabbaaaaaaaa"
          "babbbaabbbbaabaaaabaabaaabbbaaaaaaaabbbbaaabaaaabaaabaabbabbaaaabbaa"
          "bbabbaaaabbbbaabaabbbbaababaabbbbbbababaabababbbabbbaaabbbabbbaabaab"
          "bbabaabbbababbbaababaabaababaaabbbaabbabbaaaaabbbababbababbaaaaababa"
          "bbbaabbbaababbaaabbabbaaababbbaabaaabaabbbaaabbbbabaaabbbbabbaaabaab"
          "babaaabbaaabbabbbaabaaaabbbbaabaabbbabaabbbaaabbbabaaabbbaabaaaababa"
          "bbbbaabaaabbbaabaabaaabbaaaabaabbabbabaabbbaaababbbaababaaaabbaaabba"
          "baaaababbab",
          "aaabbabbbbaaabaaaabaabbaaabbabaabababaaaaabbaabbabaabbaaabbbbabbbbbb"
          "baababaaababbbbbababababaaabbbaabbbabaaabbbbbbbaaaaabbbbababbbabaaab"
          "bbaabababababaaabbbaabbbbbaaabbaababbbaabbbaababaaaababbaabbaaababba"
          "baaabbababbbaaaaababaabbbbbaaaabbbaaabaabaababaaabbabbbbbbabbbbbaaaa"
          "bbaaabbabaabbbbbaabbbbbbbaabaabbabbbbbabbbaabbbaababababbbabaaaaabbb"
          "bbaaabbabbabaabaaaaaabbaabbbaabbbaabbbbaababaaababbbaababaabaaabaaba"
          "aaaaabbababaaabbbbaaabbabbabbbbbbabbaaababbbabaababbbbaaaabababbabba"
          "aabbaaabbabbbbbabbaaabaaaabbbaaabbabbababababbbbbabbbbbbbbabaaababba"
          "aaaabbaaabbabbababbbbbaabbaaaabaaababbbbabaabaabbbbabaaaabbbbbbbabab"
          "abababaaabbabbabaaababaaabbaaababbbabaabaaaabaaaaabbabaabbabbaabbabb"
          "bbabaabbbbaabaaaabbabbabbabbaaaaaababaaaaaabbabbaabaaababaaaabaaaaaa"
          "aabbaabbaaaabbabaababbabbababbaabbbbababaabaaaabaaaaabaaabbabbaaabaa"
          "aaaaaaaabbbaabaababaabbbbbaababaabbaaaabbbbabaabbabaabbabaabaaaaaabb"
          "aababbbabaaaabbababbbbbaababbaabbbabbaabaabbbabababbaabbbaaaaaaabaaa"
          "aaaababbaaabbbaabaaababaaabbbaaaaabaaaabbbbbbbabbbaaaababaababbababb"
          "bbaaaabbbababbbbaabaaabbababbbabaaabbbbbaabbababbaabbbbaaababbbbbbab"
          "babbababaabaaabbbbaababbbaababbabbbbabbbbbbabbbaaabbbabaaaaaabbbbaba"
          "bbbbabaaaaabaababbbaababbabbbabaaababbaaabaabbbabaaaabbaabbbbaababaa"
          "baabbaabaaabbaa",
          "bbbaababbbaaabaaaabaabbaaabbabaabababaaaaabbaabbabaabbaaabbbbabbbbbb"
          "baababaaababbbbbababababaaabbbaabbbabaaabbbbbbbaaaaabbbbababbbabaaab"
          "bbaabababababaaabbbaabbbbbaaabbaababbbaabbbaababaaaababbaabbaaababba"
          "baaabbababbbaaaaababaabbbbbaaaabbbaaabaabaababaaabbabbbbbbabbbbbaaaa"
          "bbaaabbabaabbbbbaabbbbbbbaabaabbabbbbbabbbaabbbaababababbbabaaaaabbb"
          "bbaaabbabbabaabaaaaaabbaabbbaabbbaabbbbaababaaababbbaababaabaaabaaba"
          "aabbbaabaabaaabbbbaaabbabbabbbbbbabbaaababbbabaababbbbaaaabababbabba"
          "aabbbbbaababbbbabbaaabaaaabbbaaabbabbababababbbbbabbbbbbbbabaaababba"
          "aaaabbbbbaababababbbbbaabbaaaabaaababbbbabaabaabbbbabaaaabbbbbbbabab"
          "abababaaabbabbabaaababaaabbaaababbbabaabaaaabaaaaabbabaabbabbaabbabb"
          "bbabaabbbbaabaaaabbabbabbabbaaaaaababaaaaaabbabbaabaaababaaaabaaaaaa"
          "aabbaabbaaaabbabaababbabbababbaabbbbababaabaaaabaaaaabaaabbabbaaabaa"
          "aaaaaaaabbbaabaababaabbbbbaababaabbaaaabbbbabaabbabaabbabaabaaaaaabb"
          "aababbbabaaaabbababbbbbaababbaabbbabbaabaabbbabababbaabbbaaaaaaabaaa"
          "aaaababbaaaaaabbabaababaaabbbaaaaabaaaabbbbbbbabbbaaaababaababbababb"
          "bbaaaabbbababbbbaabaaabbababbbabaaabbbbbaabbababbaabbbbaaababbbbbbab"
          "babbababaabaaabaaabbabbbbaababbabbbbabbbbbbabbbaaabbbabaaaaaabbbbaba"
          "bbbbabaaaaabaabaaaabbabbbabbbabaaababbaaabaabbbabaaaabbaabbbbaababaa"
          "baabbaabaaabbaa",
          "bbbaababababaabbbabbaabbbbabbabbbbbbbaaabbaabbbbbbbaaabbaabaabaaabbb"
          "abbbbaabaabbbabbaaabababbaabaaaaaaaabbbbbabbbbbbababaababababbbbabba"
          "baaabbbaabaabbbababbabbbaaababbbabbbbbaababbaababaaaaababaaabababbbb"
          "ababaaaaabbababaabaabbaaaaabbbbbbabaabbabaaabaaaaabbabbbbbaaaaaabaaa"
          "babbbbabaaabbabbbaaaabaaaabababbbbaaaabbabbbabbbbababaabbaaabbaababa"
          "baaaabbbbaaaabbabbbbabbbbbbbaaabbaabbbbbabaabbbbababbabaaababbabbaab"
          "bbbbbabbaaabaaabbababaabbbbaaaaababaaabaaaababaaaabbaabbaabbaaaabbbb"
          "abaabaabaaaaaabbbaababababaabbbbbabaaabbbbaabaabbabaababbababbabbbbb"
          "bbbabaabbabaaabaaabbabbabbbabbbabababbaaabaaabbbbbbabaabbaababbabbaa"
          "aaaabaaabbbabbbbbaababbbbbabaababbababaabaaaaabbbbbaaabbbaaaabababba"
          "abaaabaabbbabbaaabbabaabaabbbaaaaaabbbbaabbaabbbaababaabbaabaaaaaabb"
          "bbaaababaabbaaabbbaabaabaabbbbbbbbababaaaaaabbaabaabbaabbbabaaaaabaa"
          "babbbabaababababbbbaabaaaabbababbaabababbbababbabbbbbaaabaaaaabababb"
          "babaaaababbabbbbabbbaababaababbaaaaaaaaaaababbababbbabaabbaaaaaabaaa"
          "aaaaaaaaaabaaaabbabbbbaababaabaababbbbbbbaababaaaaaabababbabbbabbaba"
          "baaabaababaaaabaabbaaaaabbababaabbabaaaabbbbaabaabbabbaabaabaabbabbb"
          "abaabbbbbbaababbaabaaaabbabbababaabbaabaabbabbbbbabaababbbbaabbabaaa"
          "baabaababbaaabababaabbbbaababaaababbbbbbbaaaaabbbbbaababbbaaabbbbbaa"
          "bbaabbaaaabbbabaababb",
          "aaabbabbababaabbbabbaabbbbabbabbbbbbbaaabbaabbbbbbbaaabbaabaabaaabbb"
          "abaaabbababbbabbaaabababbaabaaaaaaaabbbbbabbbbbbababaababababbbbabba"
          "baaabbbaabaabbbababbabbbaaababbbabbbbbaababbaababaaaaababaaabababbbb"
          "ababaabbbaabaabaabaabbaaaaabbbbbbabaabbabaaabaaaaabbabbbbbaaaaaabaaa"
          "babbbbabaaabbabbbaaaabaaaabababbbbaaaabbabbbabbbbababaabbaaabbaababa"
          "baaaabbbbabbbaababbbabbbbbbbaaabbaabbbbbabaabbbbababbabaaababbabbaab"
          "bbbbbabbaaabbbbaabaabaabbbbaaaaababaaabaaaababaaaabbaabbaabbaaaabbbb"
          "abaabaabaaaaaabbbaababababaabbbbbabaaabbbbaabaabbabaababbababbabbbbb"
          "bbbabaabbabaaabaaabbabbabbbabbbabababbaaabaaabbbbbbabaabbaababbabbaa"
          "aaaabaaabbbabbbbbaababbbbbabaababbababaabaaaaabbbbbaaabbbaaaabababba"
          "abaaabaabbbabbbbbaabaaabaabbbaaaaaabbbbaabbaabbbaababaabbaabaaaaaabb"
          "bbaaababaabbaaabbbaabaabaabbbbbbbbababaaaaaabbaabaabbaabbbabaaaaabaa"
          "babbbabaababababaaabbabaaabbababbaabababbbababbabbbbbaaabaaaaabababb"
          "babaaaababbabbbbabbbaababaababbaaaaaaaaaaababbababbbabaabbaaaaaabaaa"
          "aaaaaaaaaabaaaabbabaaabbabbaabaababbbbbbbaababaaaaaabababbabbbabbaba"
          "baaabaababaaaabaabbaaaaabbababaabbabaaaabbbbaabaabbabbaabaabaabbabbb"
          "abaabbbbbbaababbaababbbaababababaabbaabaabbabbbbbabaababbbbaabbabaaa"
          "baabaababbaaabababaabaaabbabbaaababbbbbbbaaaaabbbbbaababbbaaabbbbbaa"
          "bbaabbaaaabbbabaababb"};

      for (auto const& w : words) {
        REQUIRE(k.equal_to(k.normal_form(w), w));
        REQUIRE(k.equal_to(w, k.normal_form(w)));
      }
    }

  }  // namespace fpsemigroup

  namespace congruence {
    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "075",
                            "(cong) example 1",
                            "[quick][kambites][fpsemigroup][fpsemi]") {
      auto     rg = ReportGuard(REPORT);
      Kambites k;
      k.set_number_of_generators(2);
      REQUIRE(k.is_quotient_obviously_infinite());
      REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);
      k.add_pair({0, 0, 0}, {0});
      k.add_pair({0}, {1, 1});
      REQUIRE(k.kambites().small_overlap_class() == 1);
      REQUIRE(k.const_contains({0, 0}, {0}) == tril::unknown);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "076",
                            "(cong) example 2",
                            "[quick][kambites][cong][congruence]") {
      auto     rg = ReportGuard(REPORT);
      Kambites k;
      k.set_number_of_generators(7);
      k.add_pair({0, 1, 2, 3}, {0, 0, 0, 4, 0, 0});
      k.add_pair({4, 5}, {3, 6});
      REQUIRE(k.kambites().small_overlap_class() == 4);
      REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);
      REQUIRE_THROWS_AS(k.number_of_non_trivial_classes(),
                        LibsemigroupsException);

      REQUIRE(k.contains({0, 1, 2, 3}, {0, 0, 0, 4, 0, 0}));
      REQUIRE(k.contains({4, 5}, {3, 6}));
      REQUIRE(k.contains({0, 0, 0, 0, 0, 4, 5}, {0, 0, 0, 0, 0, 3, 6}));
      REQUIRE(k.contains({4, 5, 0, 1, 0, 1, 0}, {3, 6, 0, 1, 0, 1, 0}));
      REQUIRE_NOTHROW(k.quotient_froidure_pin());
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "077",
                            "(cong) code coverage",
                            "[quick][kambites][cong][congruence]") {
      fpsemigroup::Kambites<std::string> k;
      k.set_alphabet("abcd");
      k.add_rule("ababbabbbabbbb", "abbbbbabbbbbbabbbbbbbabbbbbbbb");
      k.add_rule("cdcddcdddcdddd", "cdddddcddddddcdddddddcdddddddd");
      Kambites l(k);
      l.run();
      REQUIRE(l.contains({0, 1, 1, 1, 0}, {0, 1, 1, 1, 0}));
      REQUIRE(l.contains(
          {0, 1, 1, 1, 0, 2, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3,
           2, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3},
          {0, 1, 1, 1, 0, 2, 3, 2, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3}));
      REQUIRE(l.finished());
      REQUIRE(l.is_quotient_obviously_infinite());
      REQUIRE(!l.is_quotient_obviously_finite());
      REQUIRE(l.number_of_classes() == POSITIVE_INFINITY);
      REQUIRE(l.class_index_to_word(100) == word_type({0, 1, 0, 0}));
      REQUIRE_NOTHROW(l.quotient_froidure_pin());
      REQUIRE(l.word_to_class_index({0, 1, 0, 0}) == 100);
    }

    LIBSEMIGROUPS_TEST_CASE("Kambites",
                            "078",
                            "(cong) large number of rules",
                            "[quick][kambites][cong][congruence]") {
      FroidurePin<LeastTransf<6>> S({LeastTransf<6>({1, 2, 3, 4, 5, 0}),
                                     LeastTransf<6>({1, 0, 2, 3, 4, 5}),
                                     LeastTransf<6>({0, 1, 2, 3, 4, 0})});
      REQUIRE(S.size() == 46'656);
      Kambites k;
      k.set_number_of_generators(3);
      for (auto it = S.cbegin_rules(); it != S.cend_rules(); ++it) {
        k.add_pair(it->first, it->second);
      }
      REQUIRE(k.kambites().small_overlap_class() == 1);
    }

  }  // namespace congruence

}  // namespace libsemigroups
