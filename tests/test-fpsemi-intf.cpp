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

#include <cstddef>

#include "catch.hpp"           // for REQUIRE, SECTION, ...
#include "cong-pair.hpp"       // for FpSemigroupByPairs
#include "element-helper.hpp"  // for FpSemigroupByPairs
#include "fpsemi-intf.hpp"     // for FpSemigroupInterface
#include "fpsemi.hpp"          // for FpSemigroup
#include "knuth-bendix.hpp"    // for fpsemigroup::KnuthBendix
#include "order.hpp"           // for shortlex_words
#include "string.hpp"          // for to_string of rule_type for debugging
#include "test-main.hpp"       // for LIBSEMIGROUPS_TEST_CASE
#include "todd-coxeter.hpp"    // for fpsemigroup::ToddCoxeter

// The following is required to get catch to print rules
namespace std {
  using rule_type = libsemigroups::FpSemigroupInterface::rule_type;
  std::ostream& operator<<(std::ostream& os, rule_type const& value) {
    os << "{ " << value.first << ", " << value.second << " }";
    return os;
  }
}  // namespace std

namespace libsemigroups {
  struct LibsemigroupsException;  // Forward declaration

  constexpr bool REPORT = false;

  namespace fpsemigroup {
    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "000",
                            "run with no alphabet",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      REQUIRE_THROWS_AS(fp->run(), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "001",
                            "equal_to",
                            "[quick][no-valgrind]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;
      size_t                                nr_words = 0;
      SECTION("human readable alphabet") {
        SECTION("ToddCoxeter") {
          fp = detail::make_unique<ToddCoxeter>();
        }
        SECTION("KnuthBendix") {
          fp = detail::make_unique<KnuthBendix>();
        }
        SECTION("FpSemigroup") {
          fp = detail::make_unique<FpSemigroup>();
        }
        fp->set_alphabet("ab");
        fp->add_rule("aaa", "a");
        fp->add_rule("bbbb", "b");
        fp->add_rule("abab", "aa");
        REQUIRE(!fp->finished());
        REQUIRE(fp->size() == 27);
        nr_words = 171;
      }
      SECTION("FpSemigroupByPairs") {
        using Transf = typename TransfHelper<5>::type;
        FroidurePin<Transf> S(
            {Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
        fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);
        fp->add_rule({0, 0, 0}, {0});
        fp->add_rule({1, 1, 1, 1}, {1});
        fp->add_rule({0, 1, 0, 1}, {1, 1});
        REQUIRE(!fp->finished());
        REQUIRE(fp->size() == 2);
        nr_words = 10;
      }
      REQUIRE(fp->equal_to({0, 0, 0}, {0}));
      REQUIRE(!fp->equal_to({1, 1, 1, 1, 1, 1}, {0}));
      auto words = shortlex_words(2, 10);
      REQUIRE(words.size() == 2046);
      REQUIRE(std::count_if(words.cbegin(),
                            words.cend(),
                            [&fp](word_type const& w) -> bool {
                              return fp->equal_to(w, {0});
                            })
              == nr_words);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "002",
                            "normal_form",
                            "[quick]") {
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      fp->set_alphabet("ab");
      fp->add_rule("aaa", "a");
      fp->add_rule("bbbb", "b");
      fp->add_rule("abab", "aa");
      REQUIRE(!fp->finished());
      REQUIRE(fp->size() == 27);
      // Not yet implemented
      // SECTION("FpSemigroupByPairs") {
      // }
      REQUIRE(fp->normal_form({0, 0, 0}) == word_type({0}));
      REQUIRE(fp->normal_form({1, 1, 1, 1, 1, 1}) == word_type({1, 1, 1}));
      auto words = shortlex_words(2, 5);
      REQUIRE(words.size() == 62);
      std::transform(words.cbegin(),
                     words.cend(),
                     words.begin(),
                     [&fp](word_type const& w) { return fp->normal_form(w); });
      REQUIRE(words
              == std::vector<word_type>({{0},
                                         {1},
                                         {0, 0},
                                         {0, 1},
                                         {1, 0},
                                         {1, 1},
                                         {0},
                                         {0, 0, 1},
                                         {0, 1, 0},
                                         {0, 1, 1},
                                         {1, 0, 0},
                                         {1, 0, 1},
                                         {1, 1, 0},
                                         {1, 1, 1},
                                         {0, 0},
                                         {0, 1},
                                         {0, 1, 1},
                                         {0, 1, 0},
                                         {0, 1},
                                         {0, 0},
                                         {0, 0, 1},
                                         {0},
                                         {1, 0},
                                         {1, 0, 0, 1},
                                         {1, 0, 1, 0},
                                         {1, 0, 1, 1},
                                         {1, 1, 0, 0},
                                         {1, 1, 0, 1},
                                         {1, 1, 1, 0},
                                         {1},
                                         {0},
                                         {0, 0, 1},
                                         {0, 1, 0},
                                         {0, 1, 1},
                                         {0, 0, 1},
                                         {0},
                                         {0, 1},
                                         {0, 0},
                                         {0, 1, 0},
                                         {0, 1, 1},
                                         {0},
                                         {0, 0, 1},
                                         {0, 1, 1},
                                         {0, 1, 0},
                                         {0, 0},
                                         {0, 1},
                                         {1, 0, 0},
                                         {1, 0, 1},
                                         {1, 0, 1, 1},
                                         {1, 0, 1, 0},
                                         {1, 0, 1},
                                         {1, 0, 0},
                                         {1, 0, 0, 1},
                                         {1, 0},
                                         {1, 1, 0},
                                         {1, 1, 0, 0, 1},
                                         {1, 1, 0, 1, 0},
                                         {1, 1, 0, 1, 1},
                                         {1, 1, 1, 0, 0},
                                         {1, 1, 1, 0, 1},
                                         {1, 0},
                                         {1, 1}}));
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "003",
                            "set_alphabet (1/2)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      // Duplicates
      REQUIRE_THROWS_AS(fp->set_alphabet("aa"), LibsemigroupsException);

      // Empty
      REQUIRE_THROWS_AS(fp->set_alphabet(""), LibsemigroupsException);
      REQUIRE_THROWS_AS(fp->set_alphabet(0), LibsemigroupsException);

      // Too many
      REQUIRE_THROWS_AS(fp->set_alphabet(300), LibsemigroupsException);

      fp->set_alphabet("ab");
      // Set more than once
      REQUIRE_THROWS_AS(fp->set_alphabet("ab"), LibsemigroupsException);
      REQUIRE_THROWS_AS(fp->set_alphabet(2), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "004",
                            "set_alphabet (2/2)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      using Transf = typename TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);
      fp->add_rule({0, 0, 0}, {0});
      fp->add_rule({1, 1, 1, 1}, {1});
      fp->add_rule({0, 1, 0, 1}, {1, 1});
      REQUIRE(!fp->finished());
      REQUIRE(fp->size() == 2);

      // Duplicates
      REQUIRE_THROWS_AS(fp->set_alphabet("aa"), LibsemigroupsException);

      // Empty
      REQUIRE_THROWS_AS(fp->set_alphabet(""), LibsemigroupsException);
      REQUIRE_THROWS_AS(fp->set_alphabet(0), LibsemigroupsException);

      // Too many
      REQUIRE_THROWS_AS(fp->set_alphabet(300), LibsemigroupsException);

      // Set more than once
      REQUIRE_THROWS_AS(fp->set_alphabet("ab"), LibsemigroupsException);
      REQUIRE_THROWS_AS(fp->set_alphabet(2), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "005",
                            "add_rule after finished",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("human readable alphabet") {
        SECTION("ToddCoxeter") {
          fp = detail::make_unique<ToddCoxeter>();
        }
        SECTION("KnuthBendix") {
          fp = detail::make_unique<KnuthBendix>();
        }
        SECTION("FpSemigroup") {
          fp = detail::make_unique<FpSemigroup>();
        }
        fp->set_alphabet("ab");
        fp->add_rule("aaa", "a");
        fp->add_rule("bbbb", "b");
        fp->add_rule("abab", "aa");
        REQUIRE(!fp->finished());
        REQUIRE(fp->size() == 27);
      }
      SECTION("FpSemigroupByPairs") {
        using Transf = typename TransfHelper<5>::type;
        FroidurePin<Transf> S(
            {Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
        fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);
        fp->add_rule({0, 0, 0}, {0});
        fp->add_rule({1, 1, 1, 1}, {1});
        fp->add_rule({0, 1, 0, 1}, {1, 1});
        REQUIRE(!fp->finished());
        REQUIRE(fp->size() == 2);
      }

      REQUIRE(fp->finished());
      REQUIRE(fp->started());
      // Add rule after finished
      REQUIRE_THROWS_AS(fp->add_rule({0}, {1}), LibsemigroupsException);
      REQUIRE_THROWS_AS(fp->add_rule({"a"}, {"b"}), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "006",
                            "add_rule with equal words (1/2)",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;
      using Transf = typename TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      SECTION("human readable alphabet") {
        SECTION("ToddCoxeter") {
          fp = detail::make_unique<ToddCoxeter>(S);
        }
        SECTION("KnuthBendix") {
          fp = detail::make_unique<KnuthBendix>(S);
        }
        SECTION("FpSemigroup") {
          fp = detail::make_unique<FpSemigroup>(S);
        }
      }
      SECTION("FpSemigroupByPairs") {
        using Transf = typename TransfHelper<5>::type;
        FroidurePin<Transf> S(
            {Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
        fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);
      }
      size_t expected = fp->nr_rules();
      REQUIRE_NOTHROW(fp->add_rule({0}, {0}));
      REQUIRE(fp->nr_rules() == expected);
      REQUIRE_NOTHROW(fp->add_rule(std::pair<word_type, word_type>({0}, {0})));
      REQUIRE_NOTHROW(
          fp->add_rule(std::pair<word_type, word_type>({{1, 1}, {0, 1}})));
      REQUIRE(fp->nr_rules() == expected + 1);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "007",
                            "add_rule with equal words (2/2)",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      fp->set_alphabet("ab");
      size_t expected = fp->nr_rules();
      REQUIRE_NOTHROW(fp->add_rule("a", "a"));
      REQUIRE_NOTHROW(fp->add_rule("ab", "ab"));
      REQUIRE_NOTHROW(fp->add_rule("abaaaaaaaa", "abaaaaaaaa"));
      REQUIRE(fp->nr_rules() == expected);
      REQUIRE_NOTHROW(fp->add_rule(std::make_pair("a", "a")));
      REQUIRE_NOTHROW(fp->add_rule(std::make_pair("ab", "ab")));
      REQUIRE(fp->nr_rules() == expected);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "008",
                            "add_rule with word_type",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      fp->set_alphabet(2);
      size_t expected = fp->nr_rules();
      REQUIRE_NOTHROW(fp->add_rule({0}, {0}));
      REQUIRE_NOTHROW(fp->add_rule({0, 1}, {0, 1}));
      REQUIRE(fp->nr_rules() == expected);
      REQUIRE_NOTHROW(fp->add_rule({0, 0, 0}, {0}));
      REQUIRE_NOTHROW(fp->add_rule({0, 1, 0}, {0, 1}));
      REQUIRE(fp->nr_rules() == expected + 2);
      REQUIRE_THROWS_AS(fp->add_rule({0, 1, 0}, {}), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "009",
                            "add_rule with empty word (1/2)",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      fp->set_alphabet("ab");
      REQUIRE_THROWS_AS(fp->add_rule("abaaaaaaaa", ""), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "010",
                            "add_rule with empty word (1/2)",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;
      fp = detail::make_unique<KnuthBendix>();
      fp->set_alphabet("ab");
      REQUIRE_NOTHROW(fp->add_rule("abaaaaaaaa", ""));
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "011",
                            "add_rules (1/3)",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      std::unique_ptr<FpSemigroupInterface> fp;

      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      fp->set_alphabet("a");
      using Transf = typename TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE_THROWS_AS(fp->add_rules(S), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "012",
                            "add_rules (2/3)",
                            "[quick]") {
      auto rg = ReportGuard(REPORT);

      using Transf = typename TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      FpSemigroupByPairs<Transf> fp(S);
      REQUIRE(fp.nr_rules() == 18);
      REQUIRE(fp.congruence().nr_generating_pairs() == 0);
      // Generating pairs are the extra generating pairs added, whereas
      // the nr_rules is the number of rules of defining the semigroup over
      // which the congruence is defined.
      REQUIRE(fp.congruence().nr_generating_pairs() == 0);

      using Transf = typename TransfHelper<5>::type;
      FroidurePin<Transf> T({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      REQUIRE_NOTHROW(fp.add_rules(T));
      REQUIRE(fp.nr_rules() == 36);
      REQUIRE(fp.size() == S.size());
      REQUIRE(fp.congruence().nr_generating_pairs() == 0);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "013",
                            "add_rules (3/3)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      REQUIRE_NOTHROW(fp->set_alphabet("ab"));
      size_t const expected = fp->nr_rules() + 3;
      std::vector<std::pair<std::string, std::string>> v
          = {{"aaa", "a"}, {"ab", "ba"}, {"bbbb", "b"}};
      REQUIRE_NOTHROW(fp->add_rules(v));
      REQUIRE(fp->nr_rules() == expected);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "014",
                            "set_identity (1/3)",
                            "[quick]") {
      using rule_type = typename FpSemigroupInterface::rule_type;
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }

      // No alphabet
      REQUIRE_THROWS_AS(fp->set_identity("a"), LibsemigroupsException);
      // Too long
      REQUIRE_THROWS_AS(fp->set_identity("aa"), LibsemigroupsException);

      REQUIRE_NOTHROW(fp->set_alphabet("ab"));

      // Letter out of range
      REQUIRE_THROWS_AS(fp->set_identity("x"), LibsemigroupsException);
      // Too long
      REQUIRE_THROWS_AS(fp->set_identity("aa"), LibsemigroupsException);

      REQUIRE_NOTHROW(fp->set_identity("a"));
      REQUIRE(fp->identity() == "a");

      REQUIRE(std::vector<rule_type>(fp->cbegin_rules(), fp->cend_rules())
              == std::vector<rule_type>({rule_type({"aa", "a"}),
                                         rule_type({"ba", "b"}),
                                         rule_type({"ab", "b"})}));
      REQUIRE_NOTHROW(fp->set_identity("b"));
      REQUIRE(fp->identity() == "b");
      REQUIRE(std::vector<rule_type>(fp->cbegin_rules(), fp->cend_rules())
              == std::vector<rule_type>({rule_type({"aa", "a"}),
                                         rule_type({"ba", "b"}),
                                         rule_type({"ab", "b"}),
                                         rule_type({"ab", "a"}),
                                         rule_type({"ba", "a"}),
                                         rule_type({"bb", "b"})}));
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "015",
                            "set_identity (2/3)",
                            "[quick]") {
      using rule_type = typename FpSemigroupInterface::rule_type;
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      using Transf = typename TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);

      auto a = std::string(1, fp->alphabet()[0]);
      auto b = std::string(1, fp->alphabet()[1]);
      REQUIRE_NOTHROW(fp->set_identity(0));
      REQUIRE(fp->identity() == a);

      // Letter out of range
      REQUIRE_THROWS_AS(fp->set_identity(letter_type(10)),
                        LibsemigroupsException);
      REQUIRE(fp->identity() == a);

      REQUIRE(std::vector<rule_type>(fp->cbegin_rules(), fp->cend_rules())
              == std::vector<rule_type>(
                  {rule_type({b + b + b, b}),
                   rule_type({b + b + a + b, b + a + b}),
                   rule_type({a + a + a + a + a, a + a}),
                   rule_type({a + b + a + a + b, a + a + a + a + b}),
                   rule_type({b + a + a + a + a, b + a}),
                   rule_type({b + b + a + a + b, b + a + a + a + b}),
                   rule_type({a + a + b + a + b + a, a + a + b + b}),
                   rule_type({a + a + b + a + b + b, a + a + b + a}),
                   rule_type({b + a + b + a + b + a, b + a + b + b}),
                   rule_type({b + a + b + a + b + b, b + a + b + a}),
                   rule_type({b + b + a + a + a + b, b + a + a + b}),
                   rule_type({a + a + b + b + a + a + a, a + a + b + b}),
                   rule_type(
                       {b + a + b + a + a + a + b, a + a + b + a + a + a + b}),
                   rule_type({b + a + b + b + a + a + a, b + a + b + b}),
                   rule_type({a + a + a + b + a + a + a + b,
                              a + a + b + a + a + a + b}),
                   rule_type({a + a + b + a + a + a + b + b,
                              a + a + b + a + a + a + b}),
                   rule_type({b + a + a + b + a + a + a + b,
                              a + a + b + a + a + a + b}),
                   rule_type({a + a + b + a + a + a + b + a + a + a,
                              a + a + b + a + a + a + b}),
                   rule_type({a + a, a}),
                   rule_type({b + a, b}),
                   rule_type({a + b, b})}));
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "016",
                            "set_identity (3/3)",
                            "[quick]") {
      using rule_type = typename FpSemigroupInterface::rule_type;
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      // No alphabet
      REQUIRE_THROWS_AS(fp->set_identity(0), LibsemigroupsException);

      REQUIRE_NOTHROW(fp->set_alphabet("ab"));

      // Letter out of range
      REQUIRE_THROWS_AS(fp->set_identity(10), LibsemigroupsException);

      REQUIRE_NOTHROW(fp->set_identity(0));
      REQUIRE(fp->identity() == "a");

      REQUIRE(std::vector<rule_type>(fp->cbegin_rules(), fp->cend_rules())
              == std::vector<rule_type>({rule_type({"aa", "a"}),
                                         rule_type({"ba", "b"}),
                                         rule_type({"ab", "b"})}));
      REQUIRE_NOTHROW(fp->set_identity(1));
      REQUIRE(fp->identity() == "b");
      REQUIRE(std::vector<rule_type>(fp->cbegin_rules(), fp->cend_rules())
              == std::vector<rule_type>({rule_type({"aa", "a"}),
                                         rule_type({"ba", "b"}),
                                         rule_type({"ab", "b"}),
                                         rule_type({"ab", "a"}),
                                         rule_type({"ba", "a"}),
                                         rule_type({"bb", "b"})}));
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "017",
                            "identity",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      REQUIRE_THROWS_AS(fp->identity(), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "018",
                            "set_inverses + inverses (1/2)",
                            "[quick]") {
      using rule_type = typename FpSemigroupInterface::rule_type;
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      // No alphabet
      REQUIRE_THROWS_AS(fp->set_inverses("bac"), LibsemigroupsException);
      // Not set
      REQUIRE_THROWS_AS(fp->inverses(), LibsemigroupsException);

      // No identity
      fp->set_alphabet("abc");
      REQUIRE_THROWS_AS(fp->set_inverses("bac"), LibsemigroupsException);
      // Not set
      REQUIRE_THROWS_AS(fp->inverses(), LibsemigroupsException);

      fp->set_identity("c");
      // Duplicates
      REQUIRE_THROWS_AS(fp->set_inverses("bbc"), LibsemigroupsException);
      // Not set
      REQUIRE_THROWS_AS(fp->inverses(), LibsemigroupsException);
      // Wrong size
      REQUIRE_THROWS_AS(fp->set_inverses("bc"), LibsemigroupsException);
      // Not set
      REQUIRE_THROWS_AS(fp->inverses(), LibsemigroupsException);

      fp->set_inverses("bac");
      // Can't set inverses more than once
      REQUIRE_THROWS_AS(fp->set_inverses("abc"), LibsemigroupsException);
      REQUIRE(std::vector<rule_type>(fp->cbegin_rules(), fp->cend_rules())
              == std::vector<rule_type>({rule_type({"ac", "a"}),
                                         rule_type({"ca", "a"}),
                                         rule_type({"bc", "b"}),
                                         rule_type({"cb", "b"}),
                                         rule_type({"cc", "c"}),
                                         rule_type({"ab", "c"}),
                                         rule_type({"ba", "c"}),
                                         rule_type({"ba", "c"}),
                                         rule_type({"ab", "c"}),
                                         rule_type({"cc", "c"}),
                                         rule_type({"cc", "c"})}));
      REQUIRE(fp->inverses() == "bac");
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "019",
                            "set_inverses + inverses (2/2)",
                            "[quick]") {
      using rule_type = typename FpSemigroupInterface::rule_type;
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      using Transf = typename TransfHelper<5>::type;
      FroidurePin<Transf> S({Transf({1, 3, 4, 2, 3}), Transf({3, 2, 1, 3, 3})});
      fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);

      auto a = std::string(1, fp->alphabet()[0]);
      auto b = std::string(1, fp->alphabet()[1]);

      // Not set
      REQUIRE_THROWS_AS(fp->inverses(), LibsemigroupsException);

      // No identity
      REQUIRE_THROWS_AS(fp->set_inverses(b + a), LibsemigroupsException);
      // Not set
      REQUIRE_THROWS_AS(fp->inverses(), LibsemigroupsException);

      fp->set_identity(a);
      // Duplicates
      REQUIRE_THROWS_AS(fp->set_inverses(b + b), LibsemigroupsException);
      // Not set
      REQUIRE_THROWS_AS(fp->inverses(), LibsemigroupsException);
      // Wrong size
      REQUIRE_THROWS_AS(fp->set_inverses(a), LibsemigroupsException);
      // Not set
      REQUIRE_THROWS_AS(fp->inverses(), LibsemigroupsException);

      fp->set_inverses(b + a);

      // Can't set inverses more than once
      REQUIRE_THROWS_AS(fp->set_inverses(b + a), LibsemigroupsException);

      REQUIRE(std::vector<rule_type>(fp->cbegin_rules(), fp->cend_rules())
              == std::vector<rule_type>(
                  {rule_type({b + b + b, b}),
                   rule_type({b + b + a + b, b + a + b}),
                   rule_type({a + a + a + a + a, a + a}),
                   rule_type({a + b + a + a + b, a + a + a + a + b}),
                   rule_type({b + a + a + a + a, b + a}),
                   rule_type({b + b + a + a + b, b + a + a + a + b}),
                   rule_type({a + a + b + a + b + a, a + a + b + b}),
                   rule_type({a + a + b + a + b + b, a + a + b + a}),
                   rule_type({b + a + b + a + b + a, b + a + b + b}),
                   rule_type({b + a + b + a + b + b, b + a + b + a}),
                   rule_type({b + b + a + a + a + b, b + a + a + b}),
                   rule_type({a + a + b + b + a + a + a, a + a + b + b}),
                   rule_type(
                       {b + a + b + a + a + a + b, a + a + b + a + a + a + b}),
                   rule_type({b + a + b + b + a + a + a, b + a + b + b}),
                   rule_type({a + a + a + b + a + a + a + b,
                              a + a + b + a + a + a + b}),
                   rule_type({a + a + b + a + a + a + b + b,
                              a + a + b + a + a + a + b}),
                   rule_type({b + a + a + b + a + a + a + b,
                              a + a + b + a + a + a + b}),
                   rule_type({a + a + b + a + a + a + b + a + a + a,
                              a + a + b + a + a + a + b}),
                   rule_type({a + a, a}),
                   rule_type({b + a, b}),
                   rule_type({a + b, b}),
                   rule_type({a + b, a}),
                   rule_type({b + a, a}),
                   rule_type({b + a, a}),
                   rule_type({a + b, a})}));
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "020",
                            "is_obviously_infinite (1/2)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      // No alphabet
      REQUIRE(!fp->is_obviously_infinite());
      fp->set_alphabet("ab");

      // More generators than rules
      REQUIRE(fp->is_obviously_infinite());
      fp->add_rule("aaa", "a");
      REQUIRE(fp->is_obviously_infinite());

      fp->add_rule("bbbb", "b");
      fp->add_rule("abab", "aa");
      REQUIRE(!fp->is_obviously_infinite());

      REQUIRE(fp->froidure_pin()->size() == 27);
      REQUIRE(!fp->is_obviously_infinite());
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "021",
                            "is_obviously_infinite (2/2)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      using Transf = typename TransfHelper<1>::type;
      FroidurePin<Transf> S({Transf({0})});
      fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);

      REQUIRE(!fp->is_obviously_infinite());
      REQUIRE(!fp->is_obviously_infinite());
      fp->add_rule({0, 0, 0}, {0});
      REQUIRE(!fp->is_obviously_infinite());

      REQUIRE(!fp->is_obviously_infinite());
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "022",
                            "is_obviously_finite (1/2)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      // No alphabet
      REQUIRE(fp->is_obviously_finite());
      fp->set_alphabet("ab");

      // More generators than rules
      REQUIRE(!fp->is_obviously_finite());
      fp->add_rule("aaa", "a");
      REQUIRE(!fp->is_obviously_finite());

      fp->add_rule("bbbb", "b");
      fp->add_rule("abab", "aa");
      REQUIRE(!fp->is_obviously_finite());

      REQUIRE(fp->froidure_pin()->size() == 27);
      REQUIRE(fp->is_obviously_finite());
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "023",
                            "is_obviously_finite (2/2)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      using Transf = typename TransfHelper<1>::type;
      FroidurePin<Transf> S({Transf({0})});
      fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);
      REQUIRE(fp->is_obviously_finite());
      REQUIRE(fp->is_obviously_finite());
      fp->add_rule({0, 0, 0}, {0});
      REQUIRE(fp->is_obviously_finite());
      REQUIRE(fp->is_obviously_finite());
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "024",
                            "to_gap_string (1/3)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      SECTION("ToddCoxeter") {
        fp = detail::make_unique<ToddCoxeter>();
      }
      SECTION("KnuthBendix") {
        fp = detail::make_unique<KnuthBendix>();
      }
      SECTION("FpSemigroup") {
        fp = detail::make_unique<FpSemigroup>();
      }
      fp->set_alphabet("ab");
      fp->add_rule("aaa", "a");
      fp->add_rule("bbbb", "b");
      fp->add_rule("abab", "aa");

      REQUIRE(fp->to_gap_string()
              == "free := FreeMonoid(\"a\", \"b\");\n"
                 "AssignGeneratorVariables(free);\n"
                 "rules := [\n"
                 "          [a * a * a, a],\n"
                 "          [b * b * b * b, b],\n"
                 "          [a * b * a * b, a * a]\n"
                 "         ];\n"
                 "S := free / rules;\n");
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "025",
                            "to_gap_string (2/3)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      fp = detail::make_unique<KnuthBendix>();
      fp->set_alphabet("ab");
      fp->add_rule("abab", "");

      REQUIRE(fp->to_gap_string()
              == "free := FreeMonoid(\"a\", \"b\");\n"
                 "AssignGeneratorVariables(free);\n"
                 "rules := [\n"
                 "          [a * b * a * b, One(free)]\n"
                 "         ];\n"
                 "S := free / rules;\n");
    }

    LIBSEMIGROUPS_TEST_CASE("FpSemigroupInterface",
                            "026",
                            "to_gap_string (3/3)",
                            "[quick]") {
      auto                                  rg = ReportGuard(REPORT);
      std::unique_ptr<FpSemigroupInterface> fp;
      using Transf = typename TransfHelper<1>::type;
      FroidurePin<Transf> S({Transf({0})});
      fp = detail::make_unique<FpSemigroupByPairs<Transf>>(S);

      REQUIRE(fp->to_gap_string()
              == "free := FreeMonoid(\"a\");\n"
                 "AssignGeneratorVariables(free);\n"
                 "rules := [\n"
                 "          [a * a, a]\n"
                 "         ];\n"
                 "S := free / rules;\n");
    }

  }  // namespace fpsemigroup
}  // namespace libsemigroups
