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

#include <algorithm>      // for count_if, all_of
#include <cstddef>        // for size_t
#include <iostream>       // for string, char_traits
#include <iterator>       // for distance
#include <memory>         // for allocator, shared_ptr
#include <string>         // for basic_string, operator==, operator!=, operator+
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_THROWS_AS
#include "test-main.hpp"          // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/constants.hpp"          // for UNDEFINED
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/kambites.hpp"           // for Kambites
#include "libsemigroups/knuth-bendix.hpp"       // for KnuthBendix
#include "libsemigroups/to-froidure-pin.hpp"
#include "libsemigroups/transf.hpp"  // for LeastTransf
#include "libsemigroups/types.hpp"   // for tril etc
#include "libsemigroups/words.hpp"   // for number_of_words

#include "libsemigroups/detail/report.hpp"  // for ReportGuard
#include "libsemigroups/detail/string.hpp"  // for random_string etc

namespace libsemigroups {
  using namespace rx;
  using namespace literals;
  struct LibsemigroupsException;  // Forward decl

  constexpr bool REPORT = false;
  using detail::MultiStringView;

  congruence_kind constexpr onesided = congruence_kind::onesided;
  congruence_kind constexpr twosided = congruence_kind::twosided;

  using kambites::contains;
  using kambites::non_trivial_classes;
  using kambites::normal_forms;
  using kambites::partition;
  using kambites::reduce;

  namespace {

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
      auto                      rg              = ReportGuard(false);
      uint64_t                  total_c4        = 0;
      uint64_t                  total_confluent = 0;
      Presentation<std::string> p;
      p.alphabet(A);

      Kambites<std::string> k;
      KnuthBendix           kb1, kb2;

      for (size_t j = 0; j < sample_size; ++j) {
        for (size_t r = 0; r < R; ++r) {
          auto        lhs = random_string(A, min, max);
          std::string rhs;
          if (lhs.size() == min) {
            rhs = random_string(A, min + 1, max);
          } else {
            rhs = random_string(A, min, lhs.size());
          }

          p.rules = {lhs, rhs};

          k.init(twosided, p);

          kb1.init(congruence_kind::twosided, p);

          std::string AA = p.alphabet();
          std::reverse(AA.begin(), AA.end());
          p.alphabet(AA);
          kb2.init(congruence_kind::twosided, p);

          kb1.run_for(std::chrono::milliseconds(1));
          kb2.run_for(std::chrono::milliseconds(1));
          if (k.small_overlap_class() >= 4) {
            total_c4++;
          }
          if (kb1.confluent() || kb2.confluent()) {
            total_confluent++;
          }
        }
      }
      return std::make_tuple(total_c4, total_confluent);
    }
  }  // namespace

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_4() {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "aaaeaa");
    presentation::add_rule(p, "ef", "dg");

    Kambites<T> k(twosided, p);

    REQUIRE(kambites::contains(k, "abcd", "aaaeaa"));
    REQUIRE(kambites::contains(k, "ef", "dg"));
    REQUIRE(kambites::contains(k, "aaaaaef", "aaaaadg"));
    REQUIRE(kambites::contains(k, "efababa", "dgababa"));

    // TODO uncomment or delete
    //    REQUIRE(to_word("abcd") == 0123_w);
    //  REQUIRE(to_word("aaaeaa") == 000400_w);
    // REQUIRE(kambites::contains(k, to_word("abcd"), to_word("aaaeaa")));
    // REQUIRE(kambites::contains(k, to_word("ef"), to_word("dg")));
    // REQUIRE(kambites::contains(k, to_word("aaaaaef"), to_word("aaaaadg")));
    // REQUIRE(kambites::contains(k, to_word("efababa"), to_word("dgababa")));

    auto s = to_froidure_pin(k);
    s->enumerate(100);
    REQUIRE(s->current_size() == 8'205);

    StringRange strings;
    strings.alphabet(p.alphabet()).min(1).max(4);
    REQUIRE(strings.count() == 399);
    REQUIRE(non_trivial_classes(k, strings)
            == std::vector<std::vector<std::string>>({{"dg", "ef"},
                                                      {"adg", "aef"},
                                                      {"bdg", "bef"},
                                                      {"cdg", "cef"},
                                                      {"ddg", "def"},
                                                      {"dga", "efa"},
                                                      {"dgb", "efb"},
                                                      {"dgc", "efc"},
                                                      {"dgd", "efd"},
                                                      {"dge", "efe"},
                                                      {"dgf", "eff"},
                                                      {"dgg", "efg"},
                                                      {"edg", "eef"},
                                                      {"fdg", "fef"},
                                                      {"gdg", "gef"}}));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "000",
                          "MT test 4 (std::string)",
                          "[quick][kambites][no-valgrind]") {
    test_case_mt_4<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "001",
                          "MT test 4 (MultiStringView)",
                          "[quick][kambites][no-valgrind]") {
    test_case_mt_4<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_no_name_1() {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("aAbBcCe");
    presentation::add_rule(p, "aaa", "e");
    presentation::add_rule(p, "bbb", "e");
    presentation::add_rule(p, "ccc", "e");
    presentation::add_rule(p, "ABa", "BaB");
    presentation::add_rule(p, "bcB", "cBc");
    presentation::add_rule(p, "caC", "aCa");
    presentation::add_rule(p, "abcABCabcABCabcABC", "e");
    presentation::add_rule(p, "BcabCABcabCABcabCA", "e");
    presentation::add_rule(p, "cbACBacbACBacbACBa", "e");

    REQUIRE(p.rules.size() == 18);

    Kambites<T> k(twosided, p);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[0]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[1])
            == POSITIVE_INFINITY);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[2]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[3])
            == POSITIVE_INFINITY);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[4]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[5])
            == POSITIVE_INFINITY);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[6]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[7]) == 2);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[8]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[9]) == 2);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[10]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[11]) == 2);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[12]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[13])
            == POSITIVE_INFINITY);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[14]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[15])
            == POSITIVE_INFINITY);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[16]) == 2);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[17])
            == POSITIVE_INFINITY);

    REQUIRE(k.small_overlap_class() == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "002",
                          "number_of_pieces (std::string)",
                          "[quick][kambites]") {
    test_case_no_name_1<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "003",
                          "number_of_pieces (MultiStringView)",
                          "[quick][kambites]") {
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

      Presentation<std::string> p;
      p.alphabet("ab");
      presentation::add_rule(p, lhs, rhs);

      Kambites<T> k(twosided, p);
      REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), lhs) == i);
      REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), rhs) == i + 1);
      REQUIRE(k.small_overlap_class() == i);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "004",
                          "small_overlap_class (std::string)",
                          "[quick][kambites]") {
    test_case_no_name_2<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "005",
                          "small_overlap_class (MultiStringView) ",
                          "[quick][kambites]") {
    test_case_no_name_2<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_random() {
    auto rg = ReportGuard(REPORT);
    {
      Presentation<std::string> p;
      p.alphabet("abcdefghi");
      presentation::add_rule(
          p,
          "eiehiegiggfaigcdfdfdgiidcebacgfaf",
          "cgfaeiehiegiggfaigcdfdfdgigcccbddchbbhgaaedfiiahhehihcba");
      presentation::add_rule(
          p, "hihcbaeiehiegiggfaigcdfdfdgiefhbidhbdgb", "chhfgafiiddg");
      presentation::add_rule(
          p,
          "gcccbddchbbhgaaedfiiahheidcebacbdefegcehgffedacddiaiih",
          "eddfcfhbedecacheahcdeeeda");
      presentation::add_rule(p, "dfbiccfeagaiffcfifg", "dceibahghaedhefh");

      Kambites<T> k(twosided, p);

      REQUIRE(k.small_overlap_class() == 4);
      REQUIRE(ukkonen::number_of_distinct_subwords(k.ukkonen()) == 3'996);

      size_t n = presentation::length(p);
      REQUIRE(n == 254);
      REQUIRE(n * n == 64'516);
    }
    {
      Presentation<std::string> p;
      p.alphabet("abcdefghi");
      presentation::add_rule(
          p,
          "feffgccdgcfbeagiifheabecdfbgebfcibeifibccahaafabeihfgfieade"
          "bciheddeigbaf",
          "ifibccahaafabeihfgfiefeffgccdgcfbeagiifheabecfeibghddfgbaia"
          "acghhdhggagaide");
      presentation::add_rule(
          p,
          "ghhdhggagaidefeffgccdgcfbeagiifheabeccbeiddgdcbcf",
          "ahccccffdeb");
      presentation::add_rule(
          p, "feibghddfgbaiaacdfbgebfcibeieaacdbdb", "gahdfgbghhhbcci");
      presentation::add_rule(
          p,
          "dgibafaahiabfgeiiibadebciheddeigbaficfbfdbfbbiddgdcifbe",
          "iahcfgdbggaciih");

      Kambites<T> k(twosided, p);
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(ukkonen::number_of_distinct_subwords(k.ukkonen()) == 7'482);
      size_t n = presentation::length(p);

      REQUIRE(n == 327);
      REQUIRE(n * n == 106'929);
    }
    {
      Presentation<std::string> p;
      p.alphabet("abcdefghi");
      presentation::add_rule(
          p,
          "adichhbhibfchbfbbibaidfibifgagcgdedfeeibhggdbchfdaefbefcbaa"
          "hcbhbidgaahbahhahhb",
          "edfeeibhggdbchfdaefbeadichhbhibfchbfbbibaiihebabeabahcgdbic"
          "bgiciffhfggbfadf");
      presentation::add_rule(
          p,
          "bgiciffhfggbfadfadichhbhibfchbfbbibaaggfdcfcebehhbdegiaeaf",
          "hebceeicbhidcgahhcfbb");
      presentation::add_rule(p,
                             "iihebabeabahcgdbicidfibifgagcgdedehed",
                             "ecbcgaieieicdcdfdbgagdbf");
      presentation::add_rule(p, "iagaadbfcbaahcbhbidgaahbahhahhbd", "ddddh");

      Kambites<T> k(twosided, p);
      REQUIRE(k.small_overlap_class() == 3);
      REQUIRE(ukkonen::number_of_distinct_subwords(k.ukkonen()) == 7'685);

      size_t n = presentation::length(p);
      REQUIRE(n == 330);
      REQUIRE(n * n == 108'900);
    }
    {
      Presentation<std::string> p;
      p.alphabet("abcdefghi");
      presentation::add_rule(
          p,
          "ibddgdgddiabcahbidbedffeddciiabahbbiacbfehdfccacbhgafbgcdg",
          "iabahibddgdgddbdfacbafhcgfhdheieihd");
      presentation::add_rule(
          p, "hdheieihdibddgdgddebhaeaicciidebegg", "giaeehdeeec");
      presentation::add_rule(
          p,
          "bdfacbafhcgfiabcahbidbedffeddcifdfcdcdadhhcbcbebhei",
          "icaebehdff");
      presentation::add_rule(
          p,
          "aggiiacdbbiacbfehdfccacbhgafbgcdghiahfccdchaiagaha",
          "hhafbagbhghhihg");

      Kambites<T> k(twosided, p);
      REQUIRE(k.small_overlap_class() == 4);
      REQUIRE(ukkonen::number_of_distinct_subwords(k.ukkonen()) == 4'779);

      size_t n = presentation::length(p);
      REQUIRE(n == 265);
      REQUIRE(n * n == 70'225);
    }
    {
      Presentation<std::string> p;
      p.alphabet("abcdefghi");
      presentation::add_rule(
          p,
          "fibehffegdeggaddgfdaeaiacbhbgbbccceaibfcabbiedhecggbbdgihddd",
          "ceafibehffegdeggafidbaefcebegahcbhciheceaehaaehih");
      presentation::add_rule(
          p, "haaehihfibehffegdeggaecbedccaeabifeafi", "bfcccibgefiidgaih");
      presentation::add_rule(
          p,
          "fidbaefcebegahcbhciheceaeddgfdaeaiacbhbgbbcccgiahbibehgbgab"
          "efdieiggc",
          "abigdadaecdfdeeciggbdfdf");
      presentation::add_rule(
          p,
          "eeaaiicigieiabibfcabbiedhecggbbdgihdddifadgbgidbfeg",
          "daheebdgdiaeceeiicddg");

      Kambites<T> k(twosided, p);
      REQUIRE(k.small_overlap_class() == 4);

      REQUIRE(ukkonen::number_of_distinct_subwords(k.ukkonen()) == 6'681);

      size_t n = presentation::length(p);
      REQUIRE(n == 328);
      REQUIRE(n * n == 107'584);
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "006",
                          "random (std::string)",
                          "[quick][kambites]") {
    test_case_random<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "007",
                          "random (MultiStringView)",
                          "[quick][kambites]") {
    test_case_random<detail::MultiStringView>();
  }
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_knuth_bendix_055() {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "ce");
    presentation::add_rule(p, "df", "dg");

    Kambites<T> k(twosided, p);

    REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);
    REQUIRE(is_obviously_infinite(k));

    REQUIRE(kambites::contains(k, "dfabcdf", "dfabcdg"));
    REQUIRE(kambites::reduce(k, "dfabcdg") == "dfabcdf");

    REQUIRE(kambites::contains(k, "abcdf", "ceg"));
    REQUIRE(kambites::contains(k, "abcdf", "cef"));
    REQUIRE(kambites::contains(k, "dfabcdf", "dfabcdg"));
    REQUIRE(kambites::contains(k, "abcdf", "ceg"));
    REQUIRE(kambites::contains(k, "abcdf", "cef"));
    REQUIRE(kambites::reduce(k, "abcdfceg") == "abcdfabcdf");
    REQUIRE(kambites::contains(k, "abcdfceg", "abcdfabcdf"));

    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(number_of_words(p.alphabet().size(), 0, 6) == 19'608);

    auto s = to_froidure_pin(k);

    s->run_until([&s]() { return s->current_max_word_length() >= 6; });

    REQUIRE(s->number_of_elements_of_length(0, 6) == 17'921);

    REQUIRE(ukkonen::number_of_distinct_subwords(k.ukkonen()) == 17);
    // TODO(0) use normal forms instead
    //    REQUIRE(
    //        (iterator_range(s->cbegin(), s->cbegin() + 8)
    //         | transform([](auto const& val) { return val.value(); }) |
    //         to_vector())
    //        == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g",
    //        "aa"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "008",
                          "KnuthBendix 055 (std::string)",
                          "[quick][kambites][no-valgrind]") {
    test_case_knuth_bendix_055<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "009",
                          "KnuthBendix 055 (MultiStringView)",
                          "[quick][kambites][no-valgrind]") {
    test_case_knuth_bendix_055<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_gap_smalloverlap_85() {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("cab");
    presentation::add_rule(p, "aabc", "acba");

    Kambites<T> k(twosided, p);

    REQUIRE(!kambites::contains(k, "a", "b"));
    REQUIRE(kambites::contains(k, "aabcabc", "aabccba"));

    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(number_of_words(3, 4, 16) == 21'523'320);

    StringRange s;
    s.alphabet("cab").first("aabc").last("aaabc");
    REQUIRE((s | count()) == 162);

    s.first("cccc").last("ccccc");
    REQUIRE(
        (s | filter([&k](auto& w) { return kambites::contains(k, w, "acba"); })
         | count())
        == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "010",
                          "smalloverlap/gap/test.gi:85 (std::string)",
                          "[quick][kambites]") {
    test_case_gap_smalloverlap_85<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "011",
                          "smalloverlap/gap/test.gi:85 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_gap_smalloverlap_85<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "012",
                          "free semigroup",
                          "[quick][kambites]") {
    Presentation<std::string> p;
    p.alphabet("cab");
    Kambites<std::string> k(twosided, p);
    REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);

    Kambites<detail::MultiStringView> kk(twosided, p);
    REQUIRE(kk.small_overlap_class() == POSITIVE_INFINITY);
  }
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_gap_smalloverlap_49() {
    auto                      rg = ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("abcdefgh");
    presentation::add_rule(p, "abcd", "ce");
    presentation::add_rule(p, "df", "hd");

    Kambites<T> k(twosided, p);

    REQUIRE(k.small_overlap_class() >= 4);
    REQUIRE(is_obviously_infinite(k));

    REQUIRE(kambites::contains(k, "abchd", "abcdf"));
    REQUIRE(!kambites::contains(k, "abchf", "abcdf"));
    REQUIRE(kambites::contains(k, "abchd", "abchd"));
    REQUIRE(kambites::contains(k, "abchdf", "abchhd"));
    // Test cases (4) and (5)
    REQUIRE(kambites::contains(k, "abchd", "cef"));
    REQUIRE(kambites::contains(k, "cef", "abchd"));

    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);
    REQUIRE(kambites::reduce(k, "hdfabce") == "dffababcd");
    REQUIRE(kambites::contains(k, "hdfabce", "dffababcd"));

    auto s = to_froidure_pin(k);

    // s.run_until(
    //     [&s]() { return s.current_position(355010123_w) != UNDEFINED; });
    // s.enumerate(10'000);

    // REQUIRE(s.number_of_generators() == p.alphabet().size());
    // REQUIRE(to_word(p, "hdfabce") == 7350124_w);
    // REQUIRE(to_word(p, "dffababcd") == 355010123_w);
    // REQUIRE(s.current_position(to_word(p, "hdfabce")) == 1'175'302);
    // REQUIRE(s.current_position(to_word(p, "dffababcd")) == 2'253'468);

    REQUIRE(p.letter_no_checks(0) == 'a');
    REQUIRE(k.presentation().letter_no_checks(0) == 'a');

    // TODO(0) uncomment or delete
    // REQUIRE(s[0].value() == std::string({'a'}));

    // REQUIRE(
    //     (iterator_range(s.cbegin(), s.cbegin() + 8)
    //      | transform([](auto const& val) { return val.value(); }) |
    //      to_vector())
    //     == std::vector<std::string>({"a", "b", "c", "d", "e", "f", "g",
    //     "h"}));

    s->run_until([&s]() { return s->current_max_word_length() >= 6; });
    {
      auto r = seq()
               | filter([&s](size_t i) { return s->current_length(i) == 6; });
      REQUIRE(r.get() == 35'199);
    }
    {
      auto r = seq()
               | filter([&s](size_t i) { return s->current_length(i) == 1; });
      REQUIRE(r.get() == 0);
    }
    REQUIRE(s->number_of_elements_of_length(0, 6) == 35'199);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "013",
                          "smalloverlap/gap/test.gi:49 (std::string)",
                          "[quick][kambites][no-valgrind]") {
    test_case_gap_smalloverlap_49<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "014",
                          "smalloverlap/gap/test.gi:49 (MultiStringView)",
                          "[quick][kambites][no-valgrind]") {
    test_case_gap_smalloverlap_49<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_gap_smalloverlap_63() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdefgh");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "d");

    Kambites<T> k(twosided, p);

    REQUIRE(is_obviously_infinite(k));

    // Test case (6)
    REQUIRE(kambites::contains(k, "afd", "bgd"));
    REQUIRE(kambites::contains(k, "bghcafhbgd", "afdafhafd"));
    REQUIRE(kambites::reduce(k, "bghcafhbgd") == "afdafhafd");
    auto s = to_froidure_pin(k);
    s->run_until([&s]() { return s->current_max_word_length() >= 6; });
    REQUIRE(s->number_of_elements_of_length(0, 6) == 34'819);

    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "015",
                          "smalloverlap/gap/test.gi:63 (std::string)",
                          "[quick][kambites][no-valgrind]") {
    test_case_gap_smalloverlap_63<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "016",
                          "smalloverlap/gap/test.gi:63 (MultiStringView)",
                          "[quick][kambites][no-valgrind]") {
    test_case_gap_smalloverlap_63<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_gap_smalloverlap_70() {
    auto rg = ReportGuard(REPORT);
    // The following permits a more complex test of case (6), which also
    // involves using the case (2) code to change the prefix being looked
    // for:
    Presentation<std::string> p;
    p.alphabet("abcdefghij");
    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");

    Kambites<T> k(twosided, p);

    REQUIRE(is_obviously_infinite(k));

    REQUIRE(kambites::contains(k, "afdj", "bgdj"));
    REQUIRE_THROWS_AS(kambites::contains(k, "xxxxxxxxxxxxxxxxxxxxxxx", "b"),
                      LibsemigroupsException);
    REQUIRE(!kambites::contains_no_checks(k, "xxxxxxxxxxxxxxxxxxxxxxx", "b"));

    auto s = to_froidure_pin(k);
    s->run_until([&s]() { return s->current_max_word_length() >= 6; });
    REQUIRE(s->number_of_elements_of_length(0, 6) == 102'255);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "017",
                          "smalloverlap/gap/test.gi:70 (std::string)",
                          "[quick][kambites][no-valgrind]") {
    test_case_gap_smalloverlap_70<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "018",
                          "smalloverlap/gap/test.gi:70 (MultiStringView)",
                          "[quick][kambites][no-valgrind]") {
    test_case_gap_smalloverlap_70<detail::MultiStringView>();
  }
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_1_million_equals() {
    auto rg = ReportGuard(REPORT);
    // A slightly more complicated presentation for testing case (6), in
    // which the max piece suffixes of the first two relation words no
    // longer agree (since fh and gh are now pieces).
    Presentation<std::string> p;
    p.alphabet("abcdefghijkl");

    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");
    presentation::add_rule(p, "fhk", "ghl");

    Kambites<T> k(twosided, p);
    REQUIRE(is_obviously_infinite(k));

    REQUIRE(kambites::contains(k, "afdj", "bgdj"));
    REQUIRE(kambites::contains(k, "afdj", "afdj"));
    REQUIRE(kambites::reduce(k, "bfhk") == "afhl");
    REQUIRE(kambites::contains(k, "bfhk", "afhl"));

    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);

    StringRange lhs;
    lhs.alphabet("abcdefghijkl").first("a").last("bgdk");
    StringRange rhs = lhs;

    REQUIRE((lhs | count()) == 4'522);
    size_t N = 4'522;
    size_t M = 0;

    for (auto const& u : lhs) {
      for (auto const& v : (rhs | skip_n(1))) {
        M++;
        if (kambites::contains(k, u, v)) {
          N--;
          break;
        }
      }
    }

    REQUIRE(M == 10'057'120);
    REQUIRE(N == 1);

    auto s = to_froidure_pin(k);
    s->run_until([&s]() { return s->current_max_word_length() >= 6; });
    REQUIRE(s->number_of_elements_of_length(0, 6) == 255'932);

    // REQUIRE((iterator_range(s.cbegin(), s.cbegin() + p.alphabet().size())
    //          | transform([](auto const& val) { return val.value(); })
    //          | to_vector())
    //         == std::vector<std::string>(
    //             {"a", "b", "c", "d", "e", "f", "g", "h", "i", "ei", "k",
    //             "l"}));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "019",
                          "std::string smalloverlap/gap/test.gi:77"
                          "(infinite) (KnuthBendix 059)",
                          "[standard][kambites]") {
    test_case_1_million_equals<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "020",
                          "MultiStringView smalloverlap/gap/test.gi:77"
                          "(infinite) (KnuthBendix 059)",
                          "[standard][kambites]") {
    test_case_1_million_equals<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_code_cov() {
    auto rg = ReportGuard(REPORT);
    // A slightly more complicated presentation for testing case (6), in
    // which the max piece suffixes of the first two relation words no
    // longer agree (since fh and gh are now pieces).
    Presentation<std::string> p;
    p.alphabet("abcde");
    presentation::add_rule(p, "cadeca", "baedba");

    Kambites<T> k(twosided, p);
    REQUIRE(!kambites::contains(k, "cadece", "baedce"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "021",
                          "code coverage (std::string)",
                          "[quick][kambites]") {
    test_case_code_cov<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "022",
                          "code coverage (MultiStringView)",
                          "[quick][kambites]") {
    test_case_code_cov<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_ex_3_13_14() {
    auto rg = ReportGuard(REPORT);
    // Example 3.13 + 3.14
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abbba", "cdc");

    Kambites<T> k(twosided, p);

    REQUIRE(kambites::reduce(k, "cdcdcabbbabbbabbcd")
            == "abbbadcabbbabbbabbcd");
    REQUIRE(kambites::contains(
        k, kambites::reduce(k, "cdcdcabbbabbbabbcd"), "cdcdcabbbabbbabbcd"));
    REQUIRE(kambites::contains(k, "abbbadcbbba", "cdabbbcdc"));
    REQUIRE(
        kambites::contains(k, kambites::reduce(k, "cdabbbcdc"), "cdabbbcdc"));
    REQUIRE(kambites::reduce(k, "cdabbbcdc") == "abbbadcbbba");
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "023",
                          "prefix (std::string)",
                          "[quick][kambites]") {
    test_case_ex_3_13_14<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "024",
                          "prefix (MultiStringView)",
                          "[quick][kambites]") {
    test_case_ex_3_13_14<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_ex_3_15() {
    auto rg = ReportGuard(REPORT);
    // Example 3.15
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "aabc", "acba");

    Kambites<T> k(twosided, p);

    std::string original = "cbacbaabcaabcacbacba";
    std::string expected = "cbaabcabcaabcaabcabc";

    REQUIRE(kambites::contains(k, "cbaabcabcaabcaabccba", original));
    REQUIRE(kambites::contains(k, original, expected));
    REQUIRE(kambites::contains(k, expected, original));
    REQUIRE(kambites::contains(k, "cbaabcabcaabcaabccba", expected));

    REQUIRE(kambites::contains(k, original, "cbaabcabcaabcaabccba"));

    REQUIRE(kambites::contains(k, expected, "cbaabcabcaabcaabccba"));
    REQUIRE(kambites::contains(k, kambites::reduce(k, original), original));
    REQUIRE(kambites::reduce(k, original) == expected);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "025",
                          "normal_form (Example 3.15) (std::string)",
                          "[quick][kambites]") {
    test_case_ex_3_15<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "026",
                          "normal_form (Example 3.15) (MultiStringView)",
                          "[quick][kambites]") {
    test_case_ex_3_15<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_ex_3_16() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "acca");

    Kambites<T> k(twosided, p);
    std::string original = "bbcabcdaccaccabcddd";
    std::string expected = "bbcabcdabcdbcdbcddd";

    REQUIRE(kambites::contains(k, original, expected));
    REQUIRE(kambites::contains(k, expected, original));

    REQUIRE(kambites::reduce(k, original) == expected);
    REQUIRE(kambites::contains(k, kambites::reduce(k, original), original));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "027",
                          "normal_form (Example 3.16) (std::string) ",
                          "[quick][kambites]") {
    test_case_ex_3_16<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "028",
                          "normal_form (Example 3.16) (MultiStringView)",
                          "[quick][kambites]") {
    test_case_ex_3_16<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_ex_3_16_again() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "acca");

    Kambites<T> k(twosided, p);

    StringRange s;
    s.alphabet("abcd").first("a").last("aaaa");
    REQUIRE(
        (s | all_of([&k](auto& w) { return kambites::reduce(k, w) == w; })));

    s.first("aaaa").last("aaaaa");
    REQUIRE((s | filter([&k](auto& w) { return kambites::reduce(k, w) != w; })
             | count())
            == 1);

    s.first("aaaaa").last("aaaaaa");
    REQUIRE((s | filter([&k](auto& w) { return kambites::reduce(k, w) != w; })
             | count())
            == 8);
    s.first("aaaaaa").last("aaaaaaa");
    REQUIRE((s | filter([&k](auto& w) { return kambites::reduce(k, w) != w; })
             | count())
            == 48);

    for (auto& w :
         std::vector<std::string>({"accaccabd", "accbaccad", "abcdbcacca"})) {
      auto nf = kambites::reduce(k, w);
      s.min(w.size()).last(nf);
      REQUIRE((s | all_of([&k, &nf](auto& u) {
                 return !kambites::contains(k, u, nf);
               })));
    }
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Kambites",
      "029",
      "normal_form (Example 3.16) more exhaustive (std::string)",
      "[quick][kambites][no-valgrind]") {
    test_case_ex_3_16_again<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE(
      "Kambites",
      "030",
      "normal_form (Example 3.16) more exhaustive (MultiStringView)",
      "[quick][kambites][no-valgrind]") {
    test_case_ex_3_16_again<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_small() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaa", "a");
    presentation::add_rule(p, "a", "bb");

    Kambites<T> k(twosided, p);

    REQUIRE(k.small_overlap_class() == 1);
    REQUIRE(!is_obviously_infinite(k));

    REQUIRE_THROWS_AS(k.number_of_classes(), LibsemigroupsException);
    REQUIRE_THROWS_AS(kambites::contains(k, "a", "aaa"),
                      LibsemigroupsException);
    REQUIRE(!k.finished());
    k.run();
    REQUIRE(!k.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "031",
                          "small presentation (std::string)",
                          "[quick][kambites]") {
    test_case_small<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "032",
                          "small presentation (MultiStringView)",
                          "[quick][kambites]") {
    test_case_small<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_non_smalloverlap() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "aaaeaa");
    presentation::add_rule(p, "ef", "dg");
    presentation::add_rule(p, "a", "b");

    Kambites<T> k(twosided, p);

    REQUIRE(k.small_overlap_class() == 1);
    REQUIRE_THROWS_AS(k.number_of_classes(), LibsemigroupsException);
    REQUIRE_THROWS_AS(kambites::contains(k, "a", "aaa"),
                      LibsemigroupsException);
    REQUIRE(!k.finished());
    k.run();
    REQUIRE(!k.finished());
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "033",
                          "non-smalloverlap (std::string)",
                          "[quick][kambites]") {
    test_case_non_smalloverlap<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "034",
                          "non-smalloverlap (MultiStringView)",
                          "[quick][kambites]") {
    test_case_non_smalloverlap<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_3() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "accca");
    Kambites<T> k(twosided, p);

    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[0])
            == POSITIVE_INFINITY);
    REQUIRE(ukkonen::number_of_pieces(k.ukkonen(), p.rules[1]) == 4);

    REQUIRE(k.small_overlap_class() == 4);
    REQUIRE(kambites::reduce(k, "bbcabcdaccaccabcddd")
            == "bbcabcdaccaccabcddd");
    REQUIRE(
        kambites::contains(k, "bbcabcdaccaccabcddd", "bbcabcdaccaccabcddd"));
    k.run();
    REQUIRE(k.started());
    REQUIRE(k.finished());

    Kambites<T> l(k);
    REQUIRE(l.started());
    REQUIRE(l.finished());

    REQUIRE(ukkonen::number_of_pieces(l.ukkonen(), p.rules[0])
            == POSITIVE_INFINITY);
    REQUIRE(ukkonen::number_of_pieces(l.ukkonen(), p.rules[1]) == 4);

    REQUIRE(l.small_overlap_class() == 4);
    REQUIRE(kambites::reduce(l, "bbcabcdaccaccabcddd")
            == "bbcabcdaccaccabcddd");
    REQUIRE(
        kambites::contains(l, "bbcabcdaccaccabcddd", "bbcabcdaccaccabcddd"));

    auto s = to_froidure_pin(k);
    REQUIRE(s->number_of_elements_of_length(0, 0) == 0);
    REQUIRE(s->number_of_elements_of_length(6, 6) == 0);
    REQUIRE(s->number_of_elements_of_length(10, 1) == 0);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "035",
                          "MT test 3 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_3<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "036",
                          "MT test 3 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_3<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_5() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "ac", "cbbbbc");
    Kambites<T> k(twosided, p);

    REQUIRE(k.small_overlap_class() == 4);

    REQUIRE(kambites::reduce(k, "acbbbbc") == "aac");
    REQUIRE(kambites::contains(k, "acbbbbc", "aac"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "037",
                          "MT test 5 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_5<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "038",
                          "MT test 5 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_5<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_6() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abc");
    presentation::add_rule(p, "ccab", "cbac");
    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);

    REQUIRE(kambites::reduce(k, "bacbaccabccabcbacbac")
            == "bacbacbaccbaccbacbac");
    REQUIRE(
        kambites::contains(k, "bacbaccabccabcbacbac", "bacbacbaccbaccbacbac"));
    REQUIRE(kambites::reduce(k, "ccabcbaccab") == "cbaccbacbac");
    REQUIRE(kambites::contains(k, "ccabcbaccab", "cbaccbacbac"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "039",
                          "MT test 6 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_6<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "040",
                          "MT test 6 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_6<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_10() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdefghij");
    presentation::add_rule(p, "afh", "bgh");
    presentation::add_rule(p, "hc", "de");
    presentation::add_rule(p, "ei", "j");
    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);

    REQUIRE(kambites::reduce(k, "bgdj") == "afdei");
    REQUIRE(kambites::contains(k, "bgdj", "afdei"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "041",
                          "MT test 10 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_6<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "042",
                          "MT test 10 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_6<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_13() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "dcba");
    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);

    REQUIRE(kambites::reduce(k, "dcbdcba") == "abcdbcd");
    REQUIRE(kambites::contains(k, "dcbdcba", "abcdbcd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "043",
                          "MT test 13 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_13<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "044",
                          "MT test 13 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_13<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////
  template <typename T>
  void test_case_mt_14() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abca", "dcbd");
    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);

    REQUIRE(kambites::reduce(k, "dcbabca") == "abcacbd");
    REQUIRE(kambites::contains(k, "dcbabca", "abcacbd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "045",
                          "MT test 14 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_14<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "046",
                          "MT test 14 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_14<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_15() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "abcd", "dcba");
    presentation::add_rule(p, "adda", "dbbd");

    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);

    REQUIRE(kambites::reduce(k, "dbbabcd") == "addacba");
    REQUIRE(kambites::contains(k, "dbbabcd", "addacba"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "047",
                          "MT test 15 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_15<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "048",
                          "MT test 15 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_15<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_16() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "acca");
    presentation::add_rule(p, "gf", "ge");
    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);

    REQUIRE(kambites::reduce(k, "accabcdgf") == "abcdbcdge");
    REQUIRE(kambites::contains(k, "accabcdgf", "abcdbcdge"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "049",
                          "MT test 16 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_16<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "050",
                          "MT test 16 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_16<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_mt_17() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(
        p, "ababbabbbabbbb", "abbbbbabbbbbbabbbbbbbabbbbbbbb");
    presentation::add_rule(
        p, "cdcddcdddcdddd", "cdddddcddddddcdddddddcdddddddd");

    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);

    REQUIRE(kambites::reduce(k, "abbbacdddddcddddddcdddddddcdddddddd")
            == "abbbacdcddcdddcdddd");
    REQUIRE(kambites::contains(
        k, "abbbacdddddcddddddcdddddddcdddddddd", "abbbacdcddcdddcdddd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "051",
                          "MT test 17 (std::string)",
                          "[quick][kambites]") {
    test_case_mt_17<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "052",
                          "MT test 17 (MultiStringView)",
                          "[quick][kambites]") {
    test_case_mt_17<MultiStringView>();
  }

  ///////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_weak_1() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "acba", "aabc");
    presentation::add_rule(p, "acba", "dbbbd");
    Kambites<T> k(twosided, p);

    REQUIRE(k.small_overlap_class() == 4);
    REQUIRE(kambites::contains(k, "aaabc", "adbbbd"));
    REQUIRE(kambites::contains(k, "adbbbd", "aaabc"));
    REQUIRE(number_of_words(4, 4, 6) == 1280);

    StringRange s;
    s.alphabet("abcd").first("aaaa").last("aaaaaa");
    REQUIRE(
        (s | filter([&k](auto& w) { return kambites::contains(k, "acba", w); })
         | count())
        == 3);

    REQUIRE(kambites::contains(k, "aaabcadbbbd", "adbbbdadbbbd"));
    REQUIRE(kambites::contains(k, "aaabcaaabc", "adbbbdadbbbd"));
    REQUIRE(kambites::contains(k, "acba", "dbbbd"));
    REQUIRE(kambites::contains(k, "acbabbbd", "aabcbbbd"));
    REQUIRE(kambites::contains(k, "aabcbbbd", "acbabbbd"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "053",
                          "weak C(4) not strong x 1 (std::string)",
                          "[quick][kambites]") {
    test_case_weak_1<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "054",
                          "weak C(4) not strong x 1 (MultiStringView) ",
                          "[quick][kambites]") {
    test_case_weak_1<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_weak_2() {
    auto                      rg = ReportGuard(REPORT);
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "acba", "aabc");
    presentation::add_rule(p, "acba", "adbd");

    Kambites<T> k(twosided, p);
    REQUIRE(kambites::contains(k, "acbacba", "aabcabc"));
    REQUIRE(kambites::reduce(k, "acbacba") == "aabcabc");
    REQUIRE(kambites::contains(k, kambites::reduce(k, "acbacba"), "aabcabc"));
    REQUIRE(kambites::contains(k, "aabcabc", kambites::reduce(k, "acbacba")));

    StringRange s;
    s.alphabet("abcd").first("aaaa").last("aaaaaa");

    REQUIRE(
        (s | filter([&k](auto& w) { return kambites::contains(k, "acba", w); })
         | count())
        == 3);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "055",
                          "weak C(4) not strong x 2 (std::string)",
                          "[quick][kambites]") {
    test_case_weak_2<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "056",
                          "weak C(4) not strong x 2 (MultiStringView) ",
                          "[quick][kambites]") {
    test_case_weak_2<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_weak_3() {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("abcde");
    presentation::add_rule(p, "bceac", "aeebbc");
    presentation::add_rule(p, "aeebbc", "dabcd");
    Kambites<T> k(twosided, p);
    REQUIRE(kambites::reduce(k, "bceacdabcd") == "aeebbcaeebbc");
    REQUIRE(kambites::contains(
        k, kambites::reduce(k, "bceacdabcd"), "aeebbcaeebbc"));
    REQUIRE(kambites::contains(
        k, "aeebbcaeebbc", kambites::reduce(k, "bceacdabcd")));

    StringRange s;
    s.alphabet("abcd").first("aaaa").last("aaaaaa");

    REQUIRE(
        (s | filter([&k](auto& w) { return kambites::contains(k, "acba", w); })
         | count())
        == 1);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "057",
                          "weak C(4) not strong x 3 (std::string)",
                          "[quick][kambites]") {
    test_case_weak_3<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "058",
                          "weak C(4) not strong x 3 (MultiStringView) ",
                          "[quick][kambites]") {
    test_case_weak_3<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_weak_4() {
    auto rg = ReportGuard(REPORT);

    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "acba", "aabc");
    presentation::add_rule(p, "acba", "dbbd");

    Kambites<T> k(twosided, p);
    REQUIRE(kambites::reduce(k, "bbacbcaaabcbbd") == "bbacbcaaabcbbd");
    REQUIRE(kambites::contains(
        k, kambites::reduce(k, "bbacbcaaabcbbd"), "bbacbcaaabcbbd"));
    REQUIRE(kambites::contains(
        k, "bbacbcaaabcbbd", kambites::reduce(k, "bbacbcaaabcbbd")));
    REQUIRE(kambites::reduce(k, "acbacba") == "aabcabc");
    REQUIRE(kambites::contains(k, kambites::reduce(k, "acbacba"), "aabcabc"));
    REQUIRE(kambites::contains(k, "aabcabc", kambites::reduce(k, "acbacba")));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "059",
                          "weak C(4) not strong x 4 (std::string)",
                          "[quick][kambites]") {
    test_case_weak_4<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "060",
                          "weak C(4) not strong x 4 (MultiStringView) ",
                          "[quick][kambites]") {
    test_case_weak_4<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_weak_5() {
    Presentation<std::string> p;
    p.alphabet("abcde");
    presentation::add_rule(p, "abcd", "aaeaaa");

    Kambites<T> k(twosided, p);
    REQUIRE(ukkonen::number_of_distinct_subwords(k.ukkonen()) == 25);

    size_t n = presentation::length(p);
    REQUIRE(n == 10);
    REQUIRE(n * n == 100);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "061",
                          "weak C(4) not strong x 5 (std::string)",
                          "[quick][kambites]") {
    test_case_weak_5<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "062",
                          "weak C(4) not strong x 5 (MultiStringView) ",
                          "[quick][kambites]") {
    test_case_weak_5<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_weak_6() {
    Presentation<std::string> p;
    p.alphabet("abcd");
    presentation::add_rule(p, "acba", "aabc");
    presentation::add_rule(p, "acba", "adbd");
    Kambites<T> k(twosided, p);
    REQUIRE(kambites::reduce(k, "acbacba") == "aabcabc");
    REQUIRE(kambites::contains(k, kambites::reduce(k, "acbacba"), "aabcabc"));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "063",
                          "weak C(4) not strong x 6 (std::string)",
                          "[quick][kambites]") {
    test_case_weak_6<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "064",
                          "weak C(4) not strong x 6 (MultiStringView) ",
                          "[quick][kambites]") {
    test_case_weak_6<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_konovalov() {
    Presentation<std::string> p;
    p.alphabet("abAB");
    presentation::add_rule(p, "Abba", "BB");
    presentation::add_rule(p, "Baab", "AA");

    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == 2);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "065",
                          "Konovalov example (std::string)",
                          "[quick][kambites]") {
    test_case_konovalov<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "066",
                          "Konovalov example (MultiStringView)",
                          "[quick][kambites]") {
    test_case_konovalov<MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  void test_case_long_words() {
    Presentation<std::string> p;
    p.alphabet("abcde");
    presentation::add_rule(p, "bceac", "aeebbc");
    presentation::add_rule(p, "aeebbc", "dabcd");

    Kambites<T> k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);

    std::string w1  = "bceac";
    std::string w2  = "dabcd";
    std::string w3  = "aeebbc";
    auto        lhs = random_power_string(w1, w2, w3, 4000);
    auto        rhs = random_power_string(w1, w2, w3, 4000);
    for (size_t i = 0; i < 10; ++i) {
      REQUIRE(kambites::contains(k, lhs, rhs));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "067",
                          "long words (std::string)",
                          "[quick][kambites][no-valgrind]") {
    test_case_long_words<std::string>();
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "068",
                          "long words (MultiStringView)",
                          "[quick][kambites][no-valgrind]") {
    test_case_long_words<detail::MultiStringView>();
  }

  ////////////////////////////////////////////////////////////////////////
  // Some tests for exploration of the space of all 2-generator 1-relation
  // semigroups
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  auto count_2_gen_1_rel(size_t min, size_t max) {
    StringRange x;
    x.alphabet("ab").min(min).max(max);
    StringRange y = x;

    uint64_t total_c4 = 0;
    uint64_t total    = 0;

    Presentation<std::string> p;
    p.alphabet("ab");
    Kambites<std::string> k;

    for (auto const& lhs : x) {
      y.first(lhs);
      for (auto const& rhs : (y | skip_n(1))) {
        REQUIRE(lhs != rhs);
        total++;
        p.rules = {lhs, rhs};
        k.init(twosided, p);
        if (k.small_overlap_class() >= 4) {
          total_c4++;
        }
      }
    }
    return std::make_pair(total_c4, total);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "069",
                          "almost all 2-generated 1-relation monoids are C(4)",
                          "[quick][kambites][no-valgrind]") {
    auto x = count_2_gen_1_rel<std::string>(1, 7);
    REQUIRE(x.first == 1);
    REQUIRE(x.second == 7'875);
  }

  // Takes approx 5s
  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "070",
                          "almost all 2-generated 1-relation monoids are C(4)",
                          "[extreme][kambites]") {
    auto x = count_2_gen_1_rel<std::string>(1, 11);
    REQUIRE(x.first == 18'171);
    REQUIRE(x.second == 2'092'035);
  }

  // Takes approx. 21s
  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "071",
                          "almost all 2-generated 1-relation monoids are C(4)",
                          "[extreme][kambites]") {
    auto x = count_2_gen_1_rel<std::string>(1, 12);
    REQUIRE(x.first == 235'629);
    REQUIRE(x.second == 8'378'371);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "072",
                          "almost all 2-generated 1-relation monoids are C(4)",
                          "[fail][kambites]") {
    auto x = count_2_gen_1_rel<std::string>(1, 13);
    REQUIRE(x.first == 0);
    REQUIRE(x.second == 0);
  }

  // Takes about 1m45s
  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "073",
                          "almost all 2-generated 1-relation monoids are C(4)",
                          "[extreme][kambites]") {
    std::cout.precision(10);
    size_t const sample_size = 1000;
    std::cout << std::string(69, '-') << std::endl;
    fmt::print("Sample size = {}\n", sample_size);
    std::cout << std::string(69, '-') << std::endl;
    for (size_t i = 8; i < 100; ++i) {
      size_t const min = 7;
      size_t const max = i + 1;
      auto         x   = sample("ab", 1, min, max, sample_size);
      fmt::print("Estimate of C(4) / non-C(4) {:<9} (length [{}, {:>2})) = "
                 "{:.10f}\n",
                 " ",
                 min,
                 max + 1,
                 static_cast<double>(std::get<0>(x)) / sample_size);

      fmt::print("Estimate of confluent / non-confluent (length "
                 "[{}, {:>2})) = {:.10f}\n",
                 min,
                 max + 1,
                 static_cast<double>(std::get<1>(x)) / sample_size);
      std::cout << std::string(69, '-') << std::endl;
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "079",
                          "normal form possible bug",
                          "[standard][kambites]") {
    // There was a bug in MultiStringView::append, that caused this
    // test to fail, so we keep this test to check that the bug in
    // MultiStringView::append is resolved.
    Presentation<std::string> p;
    p.alphabet("ab");
    presentation::add_rule(p, "aaabbab", "bbbaaba");

    Kambites<MultiStringView> k(twosided, p);

    std::vector<std::string> words
        = {"bbbaabaabbbbbaabaabaaabbaabbbbbaaabaaabababbbbaaabbababab"
           "baabbabaabb"
           "aaabbabbaaaabbabbbbbbaabbbbaabbabaaabaaaaabbaabababababaa"
           "aabaabbabba"
           "bbaaabbabababbabaabbbbbbabaabbabaaaababbababbabbabbabbbab"
           "bbabbbabbbb"
           "aaaaaabbabbaababbbaaababbbbababababbabaabbbbbabaaaababaaa"
           "bbaaabbaaab"
           "babaabbbaababbbaaabbaabbbbaabbbbaaaaababbabbbaaaaaababbbb"
           "aaabbbabbba"
           "babbbbbbaabaabababbabbbbbaaaabbbbabbababbbaaaabbbbaabbbbb"
           "abbbbbabaab"
           "bbaaabaaabbababbbabbaaaaaabbbbabababbaabbabbbbabbabbaabbb"
           "aaabaaabbab"
           "abbbabbbbaabaaababbabbaababbbabbaababbabbbbabbbbabaabaaaa"
           "baaaabababa"
           "abababbaaabbabbbbbbaaaaaabbbbabbabbabaaaaabaabbaababbbbaa"
           "baaabbabaaa"
           "abaaabbbaaaabbabbababaaaabbbbaaabbababababaabbaaaabaabbab"
           "abbabbaaaba"
           "bbaaabbbbbabbbaababaaabbababbbbbaabbbabaaaaabbbbabbabaaaa"
           "babbabbabab"
           "aabbaababbaaabbabbbbabbbaaabbabbbaabbababbaabbaaaaaabaaab"
           "bbaababbaaa"
           "ababaabbaaabbbaabababbbbbababbbbbbbaabbbbaabababbbaabbbbb"
           "bbaabbbbaaa"
           "babaaaabaababbbaabaaabaaabaaaaabaabbbbabbabaaabbabbaabbaa"
           "bbabaaabbbb"
           "baaabababbaabbbaababababaababbaabbabaaaaabaaabaaababaabab"
           "aaaaaababaa"
           "aaaaaabaababbbbaabaabbabbabaaaaaabaabbabbbabbaabbbbbbbaaa"
           "ababababbbb"
           "ababbbabbbaaabbabbabbaabbbbbbbababaabaabababbaaabbaabbbaa"
           "bbbabbbbbab"
           "aaabbbababbbaabaaaabaabbaaaabbabbbabababbaaabbbbaabaabbab"
           "abaaaabbbaa"
           "aabbbaabaa",
           "aaabbababbbbbaabaabaaabbaabbbbbaaabaaabababbbbaaabbababab"
           "baabbabaabb"
           "bbbaababaaaabbabbbbbbaabbbbaabbabaaabaaaaabbaabababababaa"
           "aabaabbabba"
           "bbaaabbabababbabaabbbbbbabaabbabaaaababbababbabbabbabbbab"
           "bbabbbabbbb"
           "aaabbbaababaababbbaaababbbbababababbabaabbbbbabaaaababaaa"
           "bbaaabbaaab"
           "babaabbbaababbbaaabbaabbbbaabbbbaaaaababbabbbaaaaaababbbb"
           "aaabbbabbba"
           "babbbbbbaabaabababbabbbbbaaaabbbbabbababbbaaaabbbbaabbbbb"
           "abbbbbabaab"
           "bbaaabbbbaabaabbbabbaaaaaabbbbabababbaabbabbbbabbabbaabbb"
           "aaabaaabbab"
           "abbbabbbbaabaaababbabbaababbbabbaababbabbbbabbbbabaabaaaa"
           "baaaabababa"
           "abababbaaabbabbbbbbaaaaaabbbbabbabbabaaaaabaabbaababbbbaa"
           "baaabbabaaa"
           "abaaabbbabbbaababababaaaabbbbaaabbababababaabbaaaabaabbab"
           "abbabbaaaba"
           "bbaaabbbbbabbbaababaaabbababbbbbaabbbabaaaaabbbbabbabaaaa"
           "babbabbabab"
           "aabbaababbbbbaababbbabbbaaabbabbbaabbababbaabbaaaaaabaaab"
           "bbaababbaaa"
           "ababaabbaaabbbaabababbbbbababbbbbbbaabbbbaabababbbaabbbbb"
           "bbaabbbbaaa"
           "babaaaabaabaaaabbabaabaaabaaaaabaabbbbabbabaaabbabbaabbaa"
           "bbabaaabbbb"
           "baaabababbaaaaabbabbababaababbaabbabaaaaabaaabaaababaabab"
           "aaaaaababaa"
           "aaaaaabaababbbbaabaabbabbabaaaaaabaabbabbbabbaabbbbbbbaaa"
           "ababababbbb"
           "ababbbabbbaaabbabbabbaabbbbbbbababaabaabababbaaabbaabbbaa"
           "bbbabbbbbab"
           "aaabbbabaaaabbabaaabaabbaaaabbabbbabababbaaabbbbaabaabbab"
           "abaaaabbbaa"
           "aabbbaabaa",
           "bbbaabaabbbbabaaaaababbbaababbabbabbaabaaabbaaabbabbbabbb"
           "aaaababaaab"
           "baaabbabbbbaaabbabaaaaaaababbaaabbaabbaabaabbabbaabaabbab"
           "abbbbbbbbaa"
           "aaaaaabbabbbabaaababbbbbabababbbaaabbaaaaaabbbbbbabbabbba"
           "aaaabbabbab"
           "bbbaaaaabbabbabbbbababbbababbbaaabaabbabaabbaaaaabbababba"
           "abbbababbaa"
           "abbabaaabbabaaaaaaabbaababbaabbbabbabaaaabaabaaabbbbaaaab"
           "bbaaaaaaabb"
           "aabaaabbbaababbaaabbbbaabbabbbbabbbababbabbbbababbbbbbaaa"
           "baabaababab"
           "aabbabbbaaabbabbaaabaabbbbaabbaabaabaababbabbaabaabbabbbb"
           "baaabbaaabb"
           "abbbbababbaaabbabbbaabaaabaaaaaababbaaabbbbbababbaabbaaaa"
           "bbaaaaabaaa"
           "aaabbbaaaaaaaabbabbbaabaaaabaababbaaabbbbbabaaaabbbabaaaa"
           "abbaabbaabb"
           "bbaaabbbbaabaabbaaabbbbaabbbaaaaaabbbabbaabbaabbabbbabaab"
           "bbbaabababa"
           "abbbbbbaaaabbabbbbabbaaabbbabbabaaabbabbabbbabbbaaaabbbaa"
           "abbbaabaabb"
           "aaabaabbabbbbaabaaabaabbaaababaabbabaaabaabbaaabaaababbaa"
           "bbbbbababba"
           "abbabbabbbaaabbabaaaabbbaaaaabbbbbbbabbbabbbababbbabaaaba"
           "bababaaaaba"
           "aaaaaaaabbabaaabbabbbabbaaababababaaabbabbbbababbbaaaaaba"
           "baaaabbabaa"
           "babbaaaaabaaaaabbabbbbbbbbbaabbaabaabbabbaabbabaabaaaabaa"
           "babaababbaa"
           "aabaabaababbaaaaabbabbababbabbbaabbbbbaaabbbaabaaaaabaaab"
           "bbaaabbbaba"
           "bbbbbabbabbaaaabbbaababbababbabaabaabbbbaaabaaabbbabbbbba"
           "baaaabaabaa"
           "bbbabbbbaabbbaaabbbbaabaababbaabbabbabaaabbaaaababbabbaab"
           "bbabaabbbba"
           "aaabbbaaaaabaaabab",
           "aaabbababbbbabaaaaababbbaababbabbabbaabaaabbaaabbabbbabbb"
           "aaaababaaab"
           "baaabbabbbbaaabbabaaaaaaababbaaabbaabbaabaabbabbaabaabbab"
           "abbbbbbbbaa"
           "aaabbbaababbabaaababbbbbabababbbaaabbaaaaaabbbbbbabbabbba"
           "aaaabbabbab"
           "bbbaaaaabbabbabbbbababbbababbbaaabaabbabaabbaaaaabbababba"
           "abbbababbaa"
           "abbabbbbaabaaaaaaaabbaababbaabbbabbabaaaabaabaaabbbbaaaab"
           "bbaaaaaaabb"
           "aabaaabbbaababbaaabbbbaabbabbbbabbbababbabbbbababbbbbbaaa"
           "baabaababab"
           "aabbabbbaaabbabbaaabaabbbbaabbaabaabaababbabbaabaabbabbbb"
           "baaabbaaabb"
           "abbbbababbbbbaababbaabaaabaaaaaababbaaabbbbbababbaabbaaaa"
           "bbaaaaabaaa"
           "aaabbbaaaaaaaabbabbbaabaaaabaababbaaabbbbbabaaaabbbabaaaa"
           "abbaabbaabb"
           "bbaaabbbbaabaabbaaabbbbaabbbaaaaaabbbabbaabbaabbabbbabaab"
           "bbbaabababa"
           "abbbbbbabbbaababbbabbaaabbbabbabaaabbabbabbbabbbaaaabbbaa"
           "abbbaabaabb"
           "aaabaabbabbbbaabaaabaabbaaababaabbabaaabaabbaaabaaababbaa"
           "bbbbbababba"
           "abbabbabbbbbbaabaaaaabbbaaaaabbbbbbbabbbabbbababbbabaaaba"
           "bababaaaaba"
           "aaaaaaaabbabaaabbabbbabbaaababababaaabbabbbbababbbaaaaaba"
           "baaaabbabaa"
           "babbaaaaabaabbbaababbbbbbbbaabbaabaabbabbaabbabaabaaaabaa"
           "babaababbaa"
           "aabaabaababbaaaaabbabbababbabbbaabbbbbaaabbbaabaaaaabaaab"
           "bbaaabbbaba"
           "bbbbbabbabbaaaabbbaababbababbabaabaabbbbaaabaaabbbabbbbba"
           "baaaabaabaa"
           "bbbabbbbaabbbaaabbbbaabaababbaabbabbabaaabbaaaababbabbaab"
           "bbabaabbbba"
           "aaabbbaaaaabaaabab",
           "bbbaababbaabbababbbaabbbbaaaaaaabbaabbbbbabaababaababbbba"
           "baabbbaabbb"
           "aabaaabbbaabbbabbabbbbabbbabbbbbaaaaaaabaabbbbaabbbbbbaab"
           "baabaabaaba"
           "aabbabbaababbbbababaaaabaababbaababbbbabaabbbabbabaababaa"
           "abaaabbbaba"
           "bbbaabaababbbbaaaaabaaaababaababbababaaabaaaaaabbaabaabab"
           "bbbaaabaaaa"
           "bbaaabbabaaabbababbbabbbbbbababbaabbaaaababbbbaabbbaababb"
           "aabaababbbb"
           "aabbbbaabababbbabaabbaaaabaabbbabbbaabaabbabbaababbbbbbba"
           "bbbbbbbabaa"
           "bbbaaaaabbabbbbabbbbabbbaaabbbbaabbbabaabaabaabbaaaaabbba"
           "babaaabbaaa"
           "bbbbbabaaabbabbaabbbaaabbabbbbbbabbabaaabbbabbbabaabbabba"
           "bababbabbaa"
           "ababaabbbbbbaababbbbbbbaaaaaaabaababbaaababbbbbaaaaaaaabb"
           "bbabaabbbab"
           "babaabababaaaabbababbabaabbaababaabbbbbabaaabbbbabaababaa"
           "aaaaababbbb"
           "bbbbbbbbbaaabbabbbbaaabaabbbabaabaabaaaabaabbbbbbabbaaabb"
           "abaaabbbaba"
           "abaaabbbbabbbaababaaabbaaabaabababbabababaaabbabaabbabbaa"
           "aabbbbabbab"
           "abbabbababbbbbaababbaabbabaabbaaabaaaababbbbaaaabbabbaaaa"
           "baaabbbbaba"
           "bbbbbaaabbaaaabbabbabaaaabbabbaaaababbbaababbabbbaababaaa"
           "bababbabbab"
           "babbbabbbaababbbaababbbbbbbbababbbabababbababbbaaabbaabab"
           "aabbbaaabbb"
           "bbaaabababaaabbbbbaabaaababababaabbbbbbabbbabaaabaabababb"
           "babaaabaabb"
           "bbaabbaababbbabaaabbabaaaaaabbaaaababbaabbbaababbaaababbb"
           "aabaabbbbbb"
           "ababbbbbbbbaabaabbbaabaaaabababbaaabaabaababaabababbabbab"
           "bbaabbbbaba"
           "baaababbbbabbaaa",
           "aaabbabbbaabbababbbaabbbbaaaaaaabbaabbbbbabaababaababbbba"
           "baabbbaaaaa"
           "bbabaabbbaabbbabbabbbbabbbabbbbbaaaaaaabaabbbbaabbbbbbaab"
           "baabaabaabb"
           "bbaababaababbbbababaaaabaababbaababbbbabaabbbabbabaababaa"
           "abaaabbbaba"
           "bbbaabaababbbbaaaaabaaaababaababbababaaabaaaaaabbaabaabab"
           "bbbaaabaaaa"
           "bbaaabbabaaabbababbbabbbbbbababbaabbaaaababbbbaabbbaababb"
           "aabaababbbb"
           "aabbbbaabababbbabaabbaaaabaabbbabbbaabaabbabbaababbbbbbba"
           "bbbbbbbabaa"
           "bbbaabbbaababbbabbbbabbbaaabbbbaabbbabaabaabaabbaaaaabbba"
           "babaaabbaaa"
           "bbbbbabbbbaababaabbbaaabbabbbbbbabbabaaabbbabbbabaabbabba"
           "bababbabbaa"
           "ababaabbbaaabbabbbbbbbbaaaaaaabaababbaaababbbbbaaaaaaaabb"
           "bbabaabbbab"
           "babaababababbbaabaabbabaabbaababaabbbbbabaaabbbbabaababaa"
           "aaaaababbbb"
           "bbbbbbbbbaaabbabbbbaaabaabbbabaabaabaaaabaabbbbbbabbaaabb"
           "abaaabbbaba"
           "abaaabbbbaaaabbabbaaabbaaabaabababbabababaaabbabaabbabbaa"
           "aabbbbabbab"
           "abbabbababbaaabbabbbaabbabaabbaaabaaaababbbbaaaabbabbaaaa"
           "baaabbbbaba"
           "bbbbbaaabbabbbaabababaaaabbabbaaaababbbaababbabbbaababaaa"
           "bababbabbab"
           "babbbabbbaabaaaabbabbbbbbbbbababbbabababbababbbaaabbaabab"
           "aabbbaaabbb"
           "bbaaabababaaabbbbbaabaaababababaabbbbbbabbbabaaabaabababb"
           "babaaabaabb"
           "bbaabbaababbbabaaabbabaaaaaabbaaaababbaabbbaababbaaababbb"
           "aabaabbbbbb"
           "ababbbbbbbbaabaaaaabbabaaabababbaaabaabaababaabababbabbab"
           "bbaabbbbaba"
           "baaababbbbabbaaa",
           "bbbaabaaaabababaabbbbbbbabbbaaabbbabbabbbbbabaaaabaaaabaa"
           "bbbaabbbbbb"
           "bbaaabbabbabaaabbaaaaaabbbabaaaaabababbbbabbbaaaabbbabbaa"
           "abbbabbbabb"
           "aababbbaababaaaaabaaaaababaabbaaaaaaabbbaaaaaaaaaaaaaabba"
           "abbababbabb"
           "bababaaaabbababbabbabbbaaaabbaaaababaabaababaabbaababaaaa"
           "bbbbbbbbaba"
           "babbbbbabbbaabaabaabaaababbababaababaaaaaababbabaabaabbba"
           "baaaabbbabb"
           "aaabbbaabbaaabbabbbabababbabbbaaaaabbaaabaaabaabbaabbbbbb"
           "bbaaabaaaab"
           "babbbbbbaaabaaabbabbbbabbbbbaabbabaabbbaaaaababaaaaababbb"
           "abbabbabbbb"
           "bbababaaabbaaabbbaababaabaaabbaabababbbbaabbaabbabaaaabbb"
           "abbbaabaabb"
           "baaababbbbbbbbaababbaabbbbaaaaaabababababbaababbbabaaaabb"
           "baaabbbbaba"
           "baaaaaabbbabbbbbaabaaaaabbabbaabaaaabbbaaabaaabbabaabaabb"
           "bababaaaabb"
           "babbabaabababaaaaabbabbbaabbbaababbaaaababbbabbaaabababbb"
           "aaabbababab"
           "baaabbbbbbbbaaabbbbaabababaaaaaabaaabbabaabbabbababbaabaa"
           "abaababaaab"
           "babaabbbbbbbbbbbbbbaabaababbbababaaaaaaabababbbbababbaaba"
           "bababbbabbb"
           "abbaabaaaabbabaaaaaaabbabbabaaabaaabbabbababbaaaaaababbab"
           "abbaababbbb"
           "aababbbbbbaabbbabaabaaabbabaababbabaaaaabbbabaabaaababaaa"
           "aaaaaabaaab"
           "bbabbbbabaaabaaaabbaabbbaabaaabbaabbbbaaabbbbbbaabbbabbab"
           "abbbabaaabb"
           "baaaabbabababbababbabbabbbaababaaabaaabbabaaaabbbbabaaaba"
           "ababbbaabba"
           "babbbbbbabbababaabaabbbabaaabbabababbbbbabaaababbbabaabbb"
           "baabbbbabba"
           "abaabbaabaaaaabaaabaabbbaaa",
           "aaabbabaaabababaabbbbbbbabbbaaabbbabbabbbbbabaaaabaaaabaa"
           "bbbaabbbbbb"
           "bbbbbaabababaaabbaaaaaabbbabaaaaabababbbbabbbaaaabbbabbaa"
           "abbbabbbabb"
           "aababbbaababaaaaabaaaaababaabbaaaaaaabbbaaaaaaaaaaaaaabba"
           "abbababbabb"
           "babababbbaabaabbabbabbbaaaabbaaaababaabaababaabbaababaaaa"
           "bbbbbbbbaba"
           "babbbbbaaaabbababaabaaababbababaababaaaaaababbabaabaabbba"
           "baaaabbbabb"
           "aaabbbaabbbbbaababbabababbabbbaaaaabbaaabaaabaabbaabbbbbb"
           "bbaaabaaaab"
           "babbbbbbaaabbbbaababbbabbbbbaabbabaabbbaaaaababaaaaababbb"
           "abbabbabbbb"
           "bbababaaabbaaaaaabbabbaabaaabbaabababbbbaabbaabbabaaaabbb"
           "abbbaabaabb"
           "baaababbbbbaaabbabbbaabbbbaaaaaabababababbaababbbabaaaabb"
           "baaabbbbaba"
           "baaaaaabbbabbbbbaabaaaaabbabbaabaaaabbbaaabaaabbabaabaabb"
           "bababaaaabb"
           "babbabaabababaaaaabbabbbaabbbaababbaaaababbbabbaaabababbb"
           "aaabbababab"
           "baaabbbbbbbbaaabbbbaabababaaaaaabaaabbabaabbabbababbaabaa"
           "abaababaaab"
           "babaabbbbbbbbbbbbbbaabaababbbababaaaaaaabababbbbababbaaba"
           "bababbbabbb"
           "abbaabaaaabbabaaaabbbaabababaaabaaabbabbababbaaaaaababbab"
           "abbaababbbb"
           "aababbbbbbaabbbabaabbbbaabaaababbabaaaaabbbabaabaaababaaa"
           "aaaaaabaaab"
           "bbabbbbabaaabaaaabbaaaaabbabaabbaabbbbaaabbbbbbaabbbabbab"
           "abbbabaaabb"
           "baaaabbabababbababbabbaaaabbabbaaabaaabbabaaaabbbbabaaaba"
           "ababbbaabba"
           "babbbbbbabbababaabaabbbabaaabbabababbbbbabaaababbbabaabbb"
           "baabbbbabba"
           "abaabbaabaaaaabaaabaabbbaaa",
           "bbbaababbababbaaaabbaabbabbbbaababbabbbabbbababbbbbaaabab"
           "babbababaaa"
           "abbabbabbbbaaaaaaaaabbbabaaabbbbababaaaabaabbbbbbaababbaa"
           "aaabbababab"
           "aaabbabbbbbbabbbbbaababbbbaabaabaaabbababaaabaaabbbaaaaab"
           "bbaababbbba"
           "abaaabbababaabababbababbbababbbabbababbabaabbbabbabbaaaab"
           "bbbabbbbbbb"
           "ababbbaabaaaabaaaaaaaaabbabbabaaaabbbaabbaababababaaabbba"
           "bbaabbbbaba"
           "aaabbabbabbaaaababababaabbbbaabbababbbbbabbabbbbbbaabbaba"
           "abbabbbbabb"
           "abbbaabaaabbbbbbabbaaabbaaabbbababaabababbabbbaababaabbaa"
           "bbabbbbaabb"
           "babbbbaabaabaaaabaaabbbbaaaaaaaabbbbbbabaabbabbbaaabababb"
           "babaaababbb"
           "bbabaaabbabaabbbbbabbabaababbabbabbabaabbbaaaaabbbaabbbaa"
           "aabaabababb"
           "bbabaaaaabbabaaaabbbaabbbbbbbabbabbababbaaaaabababaaabbbb"
           "babaabbbabb"
           "bbaabaaaaabbabaabbabbaabbbabaabbbaaaaabbababbbaaaabaababa"
           "baaabbbbbaa"
           "abbaaababbbaabaaaaaaababbbbabbbabaaaababbaababaababaaabab"
           "babbbabbaba"
           "ababbbaabbaaabbabaabaaaabbbbbababbbbabbababbbabaabaabbbab"
           "babbabaaaaa"
           "abbabaababaaaaabbabbaaaaaaabaaaabbaaabaababbaababaaabbbba"
           "aaababaaaba"
           "abbabababaababaaabbabbbaababbbabbbbabbaaaabaabbbababbbbbb"
           "abaabbbbaba"
           "abbbaabbbbbabbbbbaabababbbbaabbbbbabbbbabbaaababbabaabbab"
           "ababaababab"
           "bbbbaaaabbaaabaaaaabbabaaaaaaaabbbaababbaaabbbabbbbaaabaa"
           "babbaababbb"
           "ababbaaaabbbbaaaaaaaabbabbbbbababaabbababbaabbaaaaaaaabba"
           "bbbabbbbaab"
           "aabbbaabbbaaabaaaabb",
           "bbbaababbababbaaaabbaabbabbbbaababbabbbabbbababbbbbaaabab"
           "babbababaaa"
           "abbabbabbbbaaaaaaaaabbbabaaabbbbababaaaabaabbbbbbaababbaa"
           "aaabbababab"
           "aaabbabbbbbbabbbbbaababbbbaabaabaaabbababaaabaaabbbaaaaab"
           "bbaababbbba"
           "abaaabbababaabababbababbbababbbabbababbabaabbbabbabbaaaab"
           "bbbabbbbbbb"
           "ababbbaabaaaabaaaaaaaaabbabbabaaaabbbaabbaababababaaabbba"
           "bbaabbbbaba"
           "bbbaabababbaaaababababaabbbbaabbababbbbbabbabbbbbbaabbaba"
           "abbabbbbabb"
           "abbbaabaaabbbbbbabbaaabbaaabbbababaabababbabbbaababaabbaa"
           "bbabbbbaabb"
           "babbbbaabaabaaaabaaabbbbaaaaaaaabbbbbbabaabbabbbaaabababb"
           "babaaababbb"
           "bbabbbbaabaaabbbbbabbabaababbabbabbabaabbbaaaaabbbaabbbaa"
           "aabaabababb"
           "bbabaaaaabbabaaaabbbaabbbbbbbabbabbababbaaaaabababaaabbbb"
           "babaabbbabb"
           "bbaabaabbbaabaaabbabbaabbbabaabbbaaaaabbababbbaaaabaababa"
           "baaabbbbbaa"
           "abbaaabaaaabbabaaaaaababbbbabbbabaaaababbaababaababaaabab"
           "babbbabbaba"
           "ababbbaabbbbbaabaaabaaaabbbbbababbbbabbababbbabaabaabbbab"
           "babbabaaaaa"
           "abbabaababaabbbaababaaaaaaabaaaabbaaabaababbaababaaabbbba"
           "aaababaaaba"
           "abbabababaababaaabbabbbaababbbabbbbabbaaaabaabbbababbbbbb"
           "abaabbbbaba"
           "abbbaabbbbbabbaaabbabbabbbbaabbbbbabbbbabbaaababbabaabbab"
           "ababaababab"
           "bbbbaaaabbaaabaaaaabbabaaaaaaaabbbaababbaaabbbabbbbaaabaa"
           "babbaababbb"
           "ababbaaaabbbbaaaaaaaabbabbbbbababaabbababbaabbaaaaaaaabba"
           "bbbabbbbaab"
           "aabbbaabbbaaabaaaabb",
           "aaabbabababbbbbbbabbbbaaaabbbabbabaaabbaaaabbbbbababbabbb"
           "bbbbbbabbab"
           "abbbbaabaabababaabbababaababbbaaaabbbbbbbbaaabbbaaabaaabb"
           "bbbaaaaabba"
           "babaaabbabbbbabaabbabaababaabababaaabbbbbaaabaabbbbaabbbb"
           "bbabaaabbbb"
           "bbaabaababaabbbbaabaabbabbbbbababaaababababbababaabaabbbb"
           "bbbbabaabaa"
           "baaabbabaababbabbabbbbbbaaabababbabbbbbbababaabbaaabaabaa"
           "abababbbaba"
           "babbbbaabaababaababababaabbbabababbbbabbbbabbbaaaabaaaaaa"
           "abbbbabbabb"
           "abbbabbbaabaabbaabbbbaaabbaabbaabaabaababbabababbbbbabaaa"
           "aaaababaaba"
           "bbababbbbbaababbaaaabaaaabbbbbbabbbabbbaabbabababbbabbbbb"
           "bbbabaabaab"
           "bbaababaaaaabbabbabbbbabbbbbaababbbbbbbbaabbaabbababbbaba"
           "aabbbababaa"
           "aaaaabbabbbbaabaabbabbbbabaabababbaaabbbbbaaabaaabbbaaabb"
           "babaaabaaab"
           "aabbbbaabaaaaaaaaabbbbbaabbaabbbabbaaabaabbbbababaaaaabaa"
           "abbaaababbb"
           "bbbbbaaabbababbabaabaabababaabaabaaabaabbbaabaabbaabaaaab"
           "aabbbbbbbaa"
           "bbaaabaaaabbabbabbabbaaabbabaaaabbbbababbbaabaaaabbbababb"
           "bbababbbaaa"
           "baababbbaaaabbabababbbbbbaaaabaabaaababbaaabbaaaaabaaaaab"
           "babbaababab"
           "abaabbbabbaaabbababaaaababbbbabbabbabababaabbbbabbaabaaab"
           "bbabbabaaab"
           "abbabaaaabbbbbbaabaaaabaaaaaababbbbbaaaabbabbbbbbbbabbbab"
           "bababbbabaa"
           "bbbaaaaaaabaaaaaabbababbbaabbaaabaaaaaabbbababbaabbbaaaab"
           "baabaaaaaab"
           "ababbabbabbababbbbbaabaaabbabababbbabbbabbabbbabaababbbbb"
           "abbabbabaab"
           "abababbabbbab",
           "bbbaabaababbbbbbbabbbbaaaabbbabbabaaabbaaaabbbbbababbabbb"
           "bbbbbbabbab"
           "abaaabbababababaabbababaababbbaaaabbbbbbbbaaabbbaaabaaabb"
           "bbbaaaaabba"
           "babbbbaababbbabaabbabaababaabababaaabbbbbaaabaabbbbaabbbb"
           "bbabaaabbba"
           "aabbabababaabbbbaabaabbabbbbbababaaababababbababaabaabbbb"
           "bbbbabaabaa"
           "bbbbaabaaababbabbabbbbbbaaabababbabbbbbbababaabbaaabaabaa"
           "abababbbaba"
           "babbbbaabaababaababababaabbbabababbbbabbbbabbbaaaabaaaaaa"
           "abbbbabbabb"
           "abbbaaaabbababbaabbbbaaabbaabbaabaabaababbabababbbbbabaaa"
           "aaaababaaba"
           "bbababbaaabbabbbaaaabaaaabbbbbbabbbabbbaabbabababbbabbbbb"
           "bbbabaabaab"
           "bbaababaabbbaabababbbbabbbbbaababbbbbbbbaabbaabbababbbaba"
           "aabbbababaa"
           "aaaaabbabbbbaabaabbabbbbabaabababbaaabbbbbaaabaaabbbaaabb"
           "babaaabaaab"
           "aabaaabbabaaaaaaaabbbbbaabbaabbbabbaaabaabbbbababaaaaabaa"
           "abbaaababbb"
           "bbbbbbbbaabaabbabaabaabababaabaabaaabaabbbaabaabbaabaaaab"
           "aabbbbbbbaa"
           "bbaaababbbaabababbabbaaabbabaaaabbbbababbbaabaaaabbbababb"
           "bbababbbaaa"
           "baababbbabbbaabaababbbbbbaaaabaabaaababbaaabbaaaaabaaaaab"
           "babbaababab"
           "abaabbbabbaaabbababaaaababbbbabbabbabababaabbbbabbaabaaab"
           "bbabbabaaab"
           "abbabaaaabbbaaabbabaaabaaaaaababbbbbaaaabbabbbbbbbbabbbab"
           "bababbbabaa"
           "bbbaaaaaaabaaabbbaabaabbbaabbaaabaaaaaabbbababbaabbbaaaab"
           "baabaaaaaab"
           "ababbabbabbababbbbbaabaaabbabababbbabbbabbabbbabaababbbbb"
           "abbabbabaab"
           "abababbabbbab",
           "aaabbabaabbabbbbabbaabbaabaaaabbababbbbaaababbbbabbbaaabb"
           "abaaabbabba"
           "babbbaababbbaabbbbaabbbbbbbbbaaabbaaabaababbabaaabaabaaba"
           "aabaabaaaba"
           "abbabbbaabaababbbbabbbaaababbababaaaaaaabbbabbbbbaaaabbaa"
           "abbbbbbabab"
           "bababbbbbaabababbbabbabaaabbabbabaaabbbbabaaaaababbbbabbb"
           "babbabaabba"
           "aaaaabbbbbaabaababbbabbabaabbaababbabaaaaaaabbbbbabbbbbbb"
           "bbbbaababab"
           "ababbbbbbbaabababababaabbbbbaabbabbbbbaabbabbbbbaabaabbbb"
           "babaaaaaaab"
           "aabbbababbbbaabaaabbbaaaaaabbbabbabaaabbbabaababbabbbaaab"
           "ababbabaaba"
           "ababaabbaaaaaabbababbaaabbbabbaabaababbabaabaabaababababa"
           "aaaaaaaaaba"
           "aababbaababbaaaabbabbbaabaaababaaabaabaaabbbaabbababbabaa"
           "aaaaabababb"
           "abbaabbbbabbaabbbbaabaaabbaabaaaaababbabbaaaabbaabaabbaba"
           "ababaabaaaa"
           "aabbbbaabababbbabaaabbabaabbaababbaabaaabaababbabbbbaabba"
           "aabbbbbabba"
           "abbbaabaabaaaaaabbaaabbabaabbbbabbababaabbaabbbabbaaabaaa"
           "ababbaabbab"
           "babbbaabaaaabbababaabbbaababaaaabbbaaabaaaaaaaaaaaaabbaba"
           "abaaabbabbb"
           "bbabbababaababaaabbbbbbaabababbabbbbbbbbabbbaaaaabbbababa"
           "abaaabbbaba"
           "bbaaabaaabaaaababbaaabbabbbabbbbbbabababbbaaabbabaabbabba"
           "baaaabbabaa"
           "aaaaaabbbbbabaabbbaaaabbababbbbbabbbbbaaaaabbabbbabaabaaa"
           "baaaabbaaba"
           "baabbaababaabaaabbabbbbbaabaabaaaaabbaabaababbabbabbbbbba"
           "bbbaabbbbab"
           "aaababbbbbbbababbbbbbabbbaabaabaaaaaaaaaaaaabbbabbbabbbaa"
           "babbbababaa"
           "abaaaaaabbabbaaabaaa"
           "bbbaabaaabbabbbbabbaabbaabaaaabbababbbbaaababbbbabbbaaabb"
           "abaaabbabba"
           "baaaabbabbbbaabbbbaabbbbbbbbbaaabbaaabaababbabaaabaabaaba"
           "aabaabaaaba"
           "abbaaaabbabababbbbabbbaaababbababaaaaaaabbbabbbbbaaaabbaa"
           "abbbbbbabab"
           "bababbaaabbabbabbbabbabaaabbabbabaaabbbbabaaaaababbbbabbb"
           "babbabaabba"
           "aaaaabbbbbaabaababbbabbabaabbaababbabaaaaaaabbbbbabbbbbbb"
           "bbbbaababab"
           "ababbbbaaabbabbabababaabbbbbaabbabbbbbaabbabbbbbaabaabbbb"
           "babaaaaaaab"
           "aabbbababaaabbabaabbbaaaaaabbbabbabaaabbbabaababbabbbaaab"
           "ababbabaaba"
           "ababaabbaaaaaabbababbaaabbbabbaabaababbabaabaabaababababa"
           "aaaaaaaaaba"
           "aababbaababbabbbaababbaabaaababaaabaabaaabbbaabbababbabaa"
           "aaaaabababb"
           "abbaabbbbabbaabaaabbabaabbaabaaaaababbabbaaaabbaabaabbaba"
           "ababaabaaaa"
           "aabbbbaabababbbabbbbaabaaabbaababbaabaaabaababbabbbbaabba"
           "aabbbbbabba"
           "abbbaabaabaaaaaabbaaabbabaabbbbabbababaabbaabbbabbaaabaaa"
           "ababbaabbab"
           "babbbaabaaaabbababaaaaabbabbaaaabbbaaabaaaaaaaaaaaaabbaba"
           "abaaabbabbb"
           "bbabbababaababaaabbbaaabbabbabbabbbbbbbbabbbaaaaabbbababa"
           "abaaabbbaba"
           "bbaaabaaabaaaababbaaabbabbbabbbbbbabababbbaaabbabaabbabba"
           "baaaabbabaa"
           "aaaaaabbbbbabaabbbaaaabbababbbbbabbbbbaaaaabbabbbabaabaaa"
           "baaaabbaaba"
           "baabbaababaabaaabbabbbbbaabaabaaaaabbaabaababbabbabbbbbba"
           "bbbaabbbbab"
           "aaababbbbbbbababbbbbbabbbaabaabaaaaaaaaaaaaabbbabbbabbbaa"
           "babbbababaa"
           "abaaaaaabbabbaaabaaa",
           "bbbaabaabbaabbababbbbabaabaaaaabaabbbbaabbbbbbbabaababbaa"
           "baabaaabaaa"
           "abbbaabaaaabbaabbaaaabababbaaaaabbbbabbaabababbbbbabbaaaa"
           "abbabbbbabb"
           "babbbbaababaaaaabbbbaaaabababbaaabbabaaaabaabbabaababbbab"
           "bbaaabaabba"
           "abbbbaaabbababbbbabababbaabbabbaaabbbbabbabababbbbbbabbba"
           "bbbbaaabaab"
           "aababbbaaabbababbbaabbaaabaabbabbaaaaaaaaaaabbbbabbaaabaa"
           "baaaababaaa"
           "aabbbabaaabbababaaaaabaaaababbabaabbabbababbaabbbabbabaab"
           "babaaaababb"
           "babbbaaaabbbaabaaababbabaaaababbbbaaaaabaabbabaababaaaaaa"
           "aaabbabbbba"
           "baabaaaaaaabbababbbaabbbbaabbbbaabbbbaabaababbaabbbaaaaab"
           "baabaabbaaa"
           "abaaaabbabaabbbbbabababaababbbbbabbbabbaabaabbaaaaaabbaaa"
           "bbaabbbbbbb"
           "baabaaaabbabbbabbbaabbababaaabbbbbbbabbaaabbbabbaaaaaabaa"
           "babbaababba"
           "aaaababaaabbabbaababbabbababbabaaaaabbbababbababaabaaabab"
           "abbbaabaaab"
           "aabbbabbbbbbaabaaaaababbbabbbabaabababbababbbabaaabbbbbbb"
           "abbaaaaaaaa"
           "babbbaabaaabbabaaabaabaaabbbaaaaaaaabbbbaaabaaaabaaabaabb"
           "abbaaaabbaa"
           "bbabbaaaabaaabbababbbbaababaabbbbbbababaabababbbabbbaaabb"
           "babbbaabaab"
           "bbabaabbbababbbaababaabaababaaabbbaabbabbaaaaabbbababbaba"
           "bbaaaaababa"
           "bbbaabbbaababbbbbaababaaababbbaabaaabaabbbaaabbbbabaaabbb"
           "babbaaabaab"
           "babaaabbaaabbaaaabbabaaabbbbaabaabbbabaabbbaaabbbabaaabbb"
           "aabaaaababa"
           "bbbbaabaaabbbaabaabaaabbaaaabaabbabbabaabbbaaababbbaababa"
           "aaabbaaabba"
           "baaaababbab",
           "aaabbababbaabbababbbbabaabaaaaabaabbbbaabbbbbbbabaababbaa"
           "baabaaabaaa"
           "abbbaabaaaabbaabbaaaabababbaaaaabbbbabbaabababbbbbabbaaaa"
           "abbabbbbabb"
           "babbbbaababaaaaabbbbaaaabababbaaabbabaaaabaabbabaababbbab"
           "bbaaabaabba"
           "abbbbbbbaabaabbbbabababbaabbabbaaabbbbabbabababbbbbbabbba"
           "bbbbaaabaab"
           "aababbbaaabbababbbaabbaaabaabbabbaaaaaaaaaaabbbbabbaaabaa"
           "baaaababaaa"
           "aabbbabaaabbababaaaaabaaaababbabaabbabbababbaabbbabbabaab"
           "babaaaababb"
           "babbbaaaabbbaabaaababbabaaaababbbbaaaaabaabbabaababaaaaaa"
           "aaabbabbbba"
           "baabaaaabbbaabaabbbaabbbbaabbbbaabbbbaabaababbaabbbaaaaab"
           "baabaabbaaa"
           "ababbbaabaaabbbbbabababaababbbbbabbbabbaabaabbaaaaaabbaaa"
           "bbaabbbbbbb"
           "baabaaaabbabbbabbbaabbababaaabbbbbbbabbaaabbbabbaaaaaabaa"
           "babbaababba"
           "aaaababbbbaababaababbabbababbabaaaaabbbababbababaabaaabab"
           "abbbaabaaab"
           "aabbbabbbbbbaabaaaaababbbabbbabaabababbababbbabaaabbbbbbb"
           "abbaaaaaaaa"
           "babbbaabbbbaabaaaabaabaaabbbaaaaaaaabbbbaaabaaaabaaabaabb"
           "abbaaaabbaa"
           "bbabbaaaabbbbaabaabbbbaababaabbbbbbababaabababbbabbbaaabb"
           "babbbaabaab"
           "bbabaabbbababbbaababaabaababaaabbbaabbabbaaaaabbbababbaba"
           "bbaaaaababa"
           "bbbaabbbaababbaaabbabbaaababbbaabaaabaabbbaaabbbbabaaabbb"
           "babbaaabaab"
           "babaaabbaaabbabbbaabaaaabbbbaabaabbbabaabbbaaabbbabaaabbb"
           "aabaaaababa"
           "bbbbaabaaabbbaabaabaaabbaaaabaabbabbabaabbbaaababbbaababa"
           "aaabbaaabba"
           "baaaababbab",
           "aaabbabbbbaaabaaaabaabbaaabbabaabababaaaaabbaabbabaabbaaa"
           "bbbbabbbbbb"
           "baababaaababbbbbababababaaabbbaabbbabaaabbbbbbbaaaaabbbba"
           "babbbabaaab"
           "bbaabababababaaabbbaabbbbbaaabbaababbbaabbbaababaaaababba"
           "abbaaababba"
           "baaabbababbbaaaaababaabbbbbaaaabbbaaabaabaababaaabbabbbbb"
           "babbbbbaaaa"
           "bbaaabbabaabbbbbaabbbbbbbaabaabbabbbbbabbbaabbbaababababb"
           "babaaaaabbb"
           "bbaaabbabbabaabaaaaaabbaabbbaabbbaabbbbaababaaababbbaabab"
           "aabaaabaaba"
           "aaaaabbababaaabbbbaaabbabbabbbbbbabbaaababbbabaababbbbaaa"
           "abababbabba"
           "aabbaaabbabbbbbabbaaabaaaabbbaaabbabbababababbbbbabbbbbbb"
           "babaaababba"
           "aaaabbaaabbabbababbbbbaabbaaaabaaababbbbabaabaabbbbabaaaa"
           "bbbbbbbabab"
           "abababaaabbabbabaaababaaabbaaababbbabaabaaaabaaaaabbabaab"
           "babbaabbabb"
           "bbabaabbbbaabaaaabbabbabbabbaaaaaababaaaaaabbabbaabaaabab"
           "aaaabaaaaaa"
           "aabbaabbaaaabbabaababbabbababbaabbbbababaabaaaabaaaaabaaa"
           "bbabbaaabaa"
           "aaaaaaaabbbaabaababaabbbbbaababaabbaaaabbbbabaabbabaabbab"
           "aabaaaaaabb"
           "aababbbabaaaabbababbbbbaababbaabbbabbaabaabbbabababbaabbb"
           "aaaaaaabaaa"
           "aaaababbaaabbbaabaaababaaabbbaaaaabaaaabbbbbbbabbbaaaabab"
           "aababbababb"
           "bbaaaabbbababbbbaabaaabbababbbabaaabbbbbaabbababbaabbbbaa"
           "ababbbbbbab"
           "babbababaabaaabbbbaababbbaababbabbbbabbbbbbabbbaaabbbabaa"
           "aaaabbbbaba"
           "bbbbabaaaaabaababbbaababbabbbabaaababbaaabaabbbabaaaabbaa"
           "bbbbaababaa"
           "baabbaabaaabbaa",
           "bbbaababbbaaabaaaabaabbaaabbabaabababaaaaabbaabbabaabbaaa"
           "bbbbabbbbbb"
           "baababaaababbbbbababababaaabbbaabbbabaaabbbbbbbaaaaabbbba"
           "babbbabaaab"
           "bbaabababababaaabbbaabbbbbaaabbaababbbaabbbaababaaaababba"
           "abbaaababba"
           "baaabbababbbaaaaababaabbbbbaaaabbbaaabaabaababaaabbabbbbb"
           "babbbbbaaaa"
           "bbaaabbabaabbbbbaabbbbbbbaabaabbabbbbbabbbaabbbaababababb"
           "babaaaaabbb"
           "bbaaabbabbabaabaaaaaabbaabbbaabbbaabbbbaababaaababbbaabab"
           "aabaaabaaba"
           "aabbbaabaabaaabbbbaaabbabbabbbbbbabbaaababbbabaababbbbaaa"
           "abababbabba"
           "aabbbbbaababbbbabbaaabaaaabbbaaabbabbababababbbbbabbbbbbb"
           "babaaababba"
           "aaaabbbbbaababababbbbbaabbaaaabaaababbbbabaabaabbbbabaaaa"
           "bbbbbbbabab"
           "abababaaabbabbabaaababaaabbaaababbbabaabaaaabaaaaabbabaab"
           "babbaabbabb"
           "bbabaabbbbaabaaaabbabbabbabbaaaaaababaaaaaabbabbaabaaabab"
           "aaaabaaaaaa"
           "aabbaabbaaaabbabaababbabbababbaabbbbababaabaaaabaaaaabaaa"
           "bbabbaaabaa"
           "aaaaaaaabbbaabaababaabbbbbaababaabbaaaabbbbabaabbabaabbab"
           "aabaaaaaabb"
           "aababbbabaaaabbababbbbbaababbaabbbabbaabaabbbabababbaabbb"
           "aaaaaaabaaa"
           "aaaababbaaaaaabbabaababaaabbbaaaaabaaaabbbbbbbabbbaaaabab"
           "aababbababb"
           "bbaaaabbbababbbbaabaaabbababbbabaaabbbbbaabbababbaabbbbaa"
           "ababbbbbbab"
           "babbababaabaaabaaabbabbbbaababbabbbbabbbbbbabbbaaabbbabaa"
           "aaaabbbbaba"
           "bbbbabaaaaabaabaaaabbabbbabbbabaaababbaaabaabbbabaaaabbaa"
           "bbbbaababaa"
           "baabbaabaaabbaa",
           "bbbaababababaabbbabbaabbbbabbabbbbbbbaaabbaabbbbbbbaaabba"
           "abaabaaabbb"
           "abbbbaabaabbbabbaaabababbaabaaaaaaaabbbbbabbbbbbababaabab"
           "ababbbbabba"
           "baaabbbaabaabbbababbabbbaaababbbabbbbbaababbaababaaaaabab"
           "aaabababbbb"
           "ababaaaaabbababaabaabbaaaaabbbbbbabaabbabaaabaaaaabbabbbb"
           "baaaaaabaaa"
           "babbbbabaaabbabbbaaaabaaaabababbbbaaaabbabbbabbbbababaabb"
           "aaabbaababa"
           "baaaabbbbaaaabbabbbbabbbbbbbaaabbaabbbbbabaabbbbababbabaa"
           "ababbabbaab"
           "bbbbbabbaaabaaabbababaabbbbaaaaababaaabaaaababaaaabbaabba"
           "abbaaaabbbb"
           "abaabaabaaaaaabbbaababababaabbbbbabaaabbbbaabaabbabaababb"
           "ababbabbbbb"
           "bbbabaabbabaaabaaabbabbabbbabbbabababbaaabaaabbbbbbabaabb"
           "aababbabbaa"
           "aaaabaaabbbabbbbbaababbbbbabaababbababaabaaaaabbbbbaaabbb"
           "aaaabababba"
           "abaaabaabbbabbaaabbabaabaabbbaaaaaabbbbaabbaabbbaababaabb"
           "aabaaaaaabb"
           "bbaaababaabbaaabbbaabaabaabbbbbbbbababaaaaaabbaabaabbaabb"
           "babaaaaabaa"
           "babbbabaababababbbbaabaaaabbababbaabababbbababbabbbbbaaab"
           "aaaaabababb"
           "babaaaababbabbbbabbbaababaababbaaaaaaaaaaababbababbbabaab"
           "baaaaaabaaa"
           "aaaaaaaaaabaaaabbabbbbaababaabaababbbbbbbaababaaaaaababab"
           "babbbabbaba"
           "baaabaababaaaabaabbaaaaabbababaabbabaaaabbbbaabaabbabbaab"
           "aabaabbabbb"
           "abaabbbbbbaababbaabaaaabbabbababaabbaabaabbabbbbbabaababb"
           "bbaabbabaaa"
           "baabaababbaaabababaabbbbaababaaababbbbbbbaaaaabbbbbaababb"
           "baaabbbbbaa"
           "bbaabbaaaabbbabaababb",
           "aaabbabbababaabbbabbaabbbbabbabbbbbbbaaabbaabbbbbbbaaabba"
           "abaabaaabbb"
           "abaaabbababbbabbaaabababbaabaaaaaaaabbbbbabbbbbbababaabab"
           "ababbbbabba"
           "baaabbbaabaabbbababbabbbaaababbbabbbbbaababbaababaaaaabab"
           "aaabababbbb"
           "ababaabbbaabaabaabaabbaaaaabbbbbbabaabbabaaabaaaaabbabbbb"
           "baaaaaabaaa"
           "babbbbabaaabbabbbaaaabaaaabababbbbaaaabbabbbabbbbababaabb"
           "aaabbaababa"
           "baaaabbbbabbbaababbbabbbbbbbaaabbaabbbbbabaabbbbababbabaa"
           "ababbabbaab"
           "bbbbbabbaaabbbbaabaabaabbbbaaaaababaaabaaaababaaaabbaabba"
           "abbaaaabbbb"
           "abaabaabaaaaaabbbaababababaabbbbbabaaabbbbaabaabbabaababb"
           "ababbabbbbb"
           "bbbabaabbabaaabaaabbabbabbbabbbabababbaaabaaabbbbbbabaabb"
           "aababbabbaa"
           "aaaabaaabbbabbbbbaababbbbbabaababbababaabaaaaabbbbbaaabbb"
           "aaaabababba"
           "abaaabaabbbabbbbbaabaaabaabbbaaaaaabbbbaabbaabbbaababaabb"
           "aabaaaaaabb"
           "bbaaababaabbaaabbbaabaabaabbbbbbbbababaaaaaabbaabaabbaabb"
           "babaaaaabaa"
           "babbbabaababababaaabbabaaabbababbaabababbbababbabbbbbaaab"
           "aaaaabababb"
           "babaaaababbabbbbabbbaababaababbaaaaaaaaaaababbababbbabaab"
           "baaaaaabaaa"
           "aaaaaaaaaabaaaabbabaaabbabbaabaababbbbbbbaababaaaaaababab"
           "babbbabbaba"
           "baaabaababaaaabaabbaaaaabbababaabbabaaaabbbbaabaabbabbaab"
           "aabaabbabbb"
           "abaabbbbbbaababbaababbbaababababaabbaabaabbabbbbbabaababb"
           "bbaabbabaaa"
           "baabaababbaaabababaabaaabbabbaaababbbbbbbaaaaabbbbbaababb"
           "baaabbbbbaa"
           "bbaabbaaaabbbabaababb"};

    for (auto const& w : words) {
      REQUIRE(kambites::contains(k, kambites::reduce(k, w), w));
      REQUIRE(kambites::contains(k, w, kambites::reduce(k, w)));
    }
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites", "075", "example 1", "[quick][kambites]") {
    auto                    rg = ReportGuard(REPORT);
    Presentation<word_type> p;
    p.alphabet(2);
    presentation::add_rule(p, 000_w, 0_w);
    presentation::add_rule(p, 0_w, 11_w);

    Kambites k(twosided, p);

    REQUIRE_THROWS_AS(k.number_of_classes(), LibsemigroupsException);
    REQUIRE(k.small_overlap_class() == 1);
    REQUIRE(!is_obviously_infinite(k));
    REQUIRE_THROWS_AS(kambites::contains(k, 00_w, 0_w), LibsemigroupsException);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites", "076", "example 2", "[quick][kambites]") {
    auto                    rg = ReportGuard(REPORT);
    Presentation<word_type> p;
    p.alphabet(7);
    presentation::add_rule(p, 0123_w, 000400_w);
    presentation::add_rule(p, 45_w, 36_w);

    Kambites k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);
    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);

    REQUIRE(kambites::contains(k, 0123_w, 000400_w));
    REQUIRE(kambites::contains(k, 45_w, 36_w));
    REQUIRE(kambites::contains(k, 0000045_w, 0000036_w));
    REQUIRE(kambites::contains(k, 4501010_w, 3601010_w));

    // Skip 9000 to ensure that this reruns enumerate on the underlying
    // FroidurePinBase
    REQUIRE((normal_forms(k) | rx::take(50) | rx::to_vector())
            == std::vector<word_type>(
                {0_w,  1_w,  2_w,  3_w,  4_w,  5_w,  6_w,  00_w, 01_w, 02_w,
                 03_w, 04_w, 05_w, 06_w, 10_w, 11_w, 12_w, 13_w, 14_w, 15_w,
                 16_w, 20_w, 21_w, 22_w, 23_w, 24_w, 25_w, 26_w, 30_w, 31_w,
                 32_w, 33_w, 34_w, 35_w, 36_w, 40_w, 41_w, 42_w, 43_w, 44_w,
                 46_w, 50_w, 51_w, 52_w, 53_w, 54_w, 55_w, 56_w, 60_w, 61_w}));
    REQUIRE(
        (normal_forms(k) | rx::skip_n(9'000) | rx::take(50) | rx::to_vector())
        == std::vector<word_type>(
            {25530_w, 25531_w, 25532_w, 25533_w, 25534_w, 25535_w, 25536_w,
             25540_w, 25541_w, 25542_w, 25543_w, 25544_w, 25546_w, 25550_w,
             25551_w, 25552_w, 25553_w, 25554_w, 25555_w, 25556_w, 25560_w,
             25561_w, 25562_w, 25563_w, 25564_w, 25565_w, 25566_w, 25600_w,
             25601_w, 25602_w, 25603_w, 25604_w, 25605_w, 25606_w, 25610_w,
             25611_w, 25612_w, 25613_w, 25614_w, 25615_w, 25616_w, 25620_w,
             25621_w, 25622_w, 25623_w, 25624_w, 25625_w, 25626_w, 25630_w,
             25631_w}));
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "077",
                          "code coverage",
                          "[quick][kambites][no-valgrind]") {
    Presentation<word_type> p;
    p.alphabet(4);
    presentation::add_rule(
        p, 01011011101111_w, 011111011111101111111011111111_w);
    presentation::add_rule(
        p, 23233233323333_w, 233333233333323333333233333333_w);

    Kambites k(twosided, p);
    REQUIRE(k.small_overlap_class() == 4);
    REQUIRE(kambites::contains(k, 01110_w, 01110_w));
    REQUIRE(kambites::contains(
        k, 01110233333233333323333333233333333_w, 0111023233233323333_w));
    REQUIRE(k.finished());
    REQUIRE(is_obviously_infinite(k));
    REQUIRE(k.number_of_classes() == POSITIVE_INFINITY);

    auto s = to_froidure_pin(k);
    REQUIRE(froidure_pin::minimal_factorisation(*s, 100) == 0100_w);
    // REQUIRE(s->position(element_type(k, 0100_w)) == 100);
    REQUIRE(s->current_size() == 8196);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "078",
                          "large number of rules",
                          "[quick][kambites][no-valgrind]") {
    auto S = to_froidure_pin({LeastTransf<6>({1, 2, 3, 4, 5, 0}),
                              LeastTransf<6>({1, 0, 2, 3, 4, 5}),
                              LeastTransf<6>({0, 1, 2, 3, 4, 0})});
    REQUIRE(S.size() == 46'656);
    auto     p = to_presentation<word_type>(S);
    Kambites k(twosided, p);
    REQUIRE(k.small_overlap_class() == 1);
    REQUIRE(k.kind() == twosided);
  }

  LIBSEMIGROUPS_TEST_CASE("Kambites",
                          "074",
                          "code coverage for constructors/init",
                          "[quick][kambites]") {
    Kambites k;

    REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);

    Kambites l = std::move(k);
    REQUIRE(l.small_overlap_class() == POSITIVE_INFINITY);

    k = l;
    REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);
    REQUIRE(l.small_overlap_class() == POSITIVE_INFINITY);

    k = std::move(l);
    REQUIRE(k.small_overlap_class() == POSITIVE_INFINITY);

    l.init();
    REQUIRE(l.small_overlap_class() == POSITIVE_INFINITY);

    Presentation<std::string> p;
    p.alphabet("abcdefg");
    ToWord to_word(p.alphabet());

    presentation::add_rule(p, "abcd", "aaaeaa");
    presentation::add_rule(p, "ef", "dg");

    Kambites kk(twosided, std::move(p));
    REQUIRE(kk.presentation().alphabet() == "abcdefg");
    REQUIRE(kk.presentation().rules
            == std::vector<std::string>({"abcd", "aaaeaa", "ef", "dg"}));
    REQUIRE(kk.small_overlap_class() == 4);

    p.alphabet("abcdefg");
    presentation::add_rule(p, "abcd", "aaaeaa");
    presentation::add_rule(p, "ef", "dg");

    REQUIRE_THROWS_AS(
        kambites::add_generating_pair(kk, to_word("abababab"), to_word("aba")),
        LibsemigroupsException);

    kk.init(twosided, std::move(p));
    REQUIRE(!kk.started());
    REQUIRE(kk.presentation().alphabet() == "abcdefg");

    p.alphabet("abcdefg");
    kambites::add_generating_pair(kk, "abababab", "aba");
    REQUIRE(kk.small_overlap_class() == 1);

    Presentation<word_type> pp;
    pp.alphabet(7);
    presentation::add_rule(pp, to_word("abcd"), to_word("aaaeaa"));
    presentation::add_rule(pp, to_word("ef"), to_word("dg"));

    kk.init(twosided, pp);
    REQUIRE(kk.generating_pairs().empty());
    REQUIRE(kk.small_overlap_class() == 4);

    Kambites<std::string> kkk(twosided, pp);
    REQUIRE(kkk.generating_pairs().empty());
    REQUIRE(kkk.small_overlap_class() == 4);

    Kambites<word_type> k2;
    REQUIRE_THROWS_AS(
        kambites::add_generating_pair(k2, 01011011101111_w, 0123_w),
        LibsemigroupsException);
  }

}  // namespace libsemigroups
