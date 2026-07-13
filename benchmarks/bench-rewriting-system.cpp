//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 Joseph Edwards + James D. Mitchell
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

#include <random>

#include "libsemigroups/config.hpp"      // for LIBSEMIGROUPS_CATCH_ALL_HEADER
#include "libsemigroups/ranges.hpp"      // for rx::to_vector
#include "libsemigroups/word-range.hpp"  // for literals

#include "libsemigroups/detail/report.hpp"            // for ReportGuard
#include "libsemigroups/detail/rewriting-system.hpp"  // for LenLexTrie

#include LIBSEMIGROUPS_CATCH_ALL_HEADER  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

namespace libsemigroups {

  using LenLexTrie = detail::RewritingSystemTrie<LenLexCmp>;
  using LenLexSet  = detail::RewritingSystemSet<LenLexCmp>;

  using RPOTrie = detail::RewritingSystemTrie<RPOCmp>;
  using RPOSet  = detail::RewritingSystemSet<RPOCmp>;

  using literals::operator""_w;

  namespace detail {

    namespace {
      // Function to generate a bunch of rules that match "many" subwords of the
      // input sample words.
      template <typename Rewriter>
      Rewriter
      generate_rewriter_random_subwords(std::vector<std::string> const& sample,
                                        size_t num_rules,
                                        size_t min_length_rule,
                                        size_t max_length_rule,
                                        size_t num_letters = 3) {
        size_t min_sample_length
            = std::min_element(sample.begin(),
                               sample.end(),
                               [](auto const& x, auto const& y) {
                                 return x.size() < y.size();
                               })
                  ->size();
        if (max_length_rule > min_sample_length) {
          LIBSEMIGROUPS_EXCEPTION("nope");
        }

        static std::random_device rd;
        static std::mt19937       mt(rd());

        std::uniform_int_distribution<uint64_t> word_dist(0, sample.size() - 1);
        std::uniform_int_distribution<uint64_t> rule_length_dist(
            min_length_rule, max_length_rule);

        Rewriter rt;
        rt.increase_alphabet_size_by(num_letters);
        for (size_t i = 0; i < num_rules; ++i) {
          std::string lhs, rhs;
          {
            size_t      rule_length = rule_length_dist(mt);
            auto const& word        = sample[word_dist(mt)];
            std::uniform_int_distribution<uint64_t> start_dist(
                0, word.size() - rule_length);
            size_t start_index = start_dist(mt);

            lhs = std::string(word.begin() + start_index,
                              word.begin() + start_index + rule_length);
          }
          {
            size_t      rule_length = rule_length_dist(mt);
            auto const& word        = sample[word_dist(mt)];
            std::uniform_int_distribution<uint64_t> start_dist(
                0, word.size() - rule_length);
            size_t start_index = start_dist(mt);

            rhs = std::string(word.begin() + start_index,
                              word.begin() + start_index + rule_length);
          }
          rewriting_system::add_rule(rt, lhs, rhs);
        }
        return rt;
      }

      template <typename Rewriter>
      Rewriter generate_rewriter_all_words(size_t min_length_rule,
                                           size_t max_length_rule,
                                           size_t num_letters = 3) {
        StringRange words;
        words.alphabet({0, 1, 2}).min(min_length_rule).max(max_length_rule);
        Rewriter rt;
        rt.increase_alphabet_size_by(num_letters);
        std::string empty = "";
        for (auto const& word : words) {
          rewriting_system::add_rule(rt, word, empty);
        }
        return rt;
      }

    }  // namespace

    TEMPLATE_TEST_CASE("Length of words rewritten",
                       "[LenLexTrie][000]",
                       LenLexTrie,
                       LenLexSet) {
      auto     rg = ReportGuard(false);
      TestType rt;
      rt.increase_alphabet_size_by(3);
      rewriting_system::add_rule(rt, "aa"_w, "a"_w);
      rewriting_system::add_rule(rt, "bc"_w, "c"_w);
      rewriting_system::add_rule(rt, "bbb"_w, "b"_w);
      rewriting_system::add_rule(rt, "ababab"_w, "b"_w);

      REQUIRE(rt.reduce());

      for (size_t m = 500; m < 10'000; m += 500) {
        std::vector<std::string> sample
            = (random_strings({0, 1, 2}, 100, m, m + 1) | rx::to_vector());

        BENCHMARK(fmt::format("3-rules, rule length word length = [1, 6], word "
                              "length = {}, for {} rewrites",
                              m,
                              sample.size())
                      .c_str()) {
          for (auto& word : sample) {
            auto copy(word);
            REQUIRE(copy.size() == m);
            rt.rewrite(copy);
            REQUIRE(copy.size() != 0);
          }
        };
      }
    }

    TEMPLATE_TEST_CASE("Number of rules (no rewriting)",
                       "[LenLexTrie][number_of_rules][001]",
                       LenLexTrie,
                       LenLexSet) {
      auto rg = ReportGuard(false);

      size_t                   sample_size = 100;
      size_t                   sample_min  = 99;
      size_t                   sample_max  = 100;
      std::vector<std::string> sample
          = (random_strings({0, 1, 2}, sample_size, sample_min, sample_max)
             | rx::to_vector());

      for (size_t m = 50; m <= 1000; m += 50) {
        TestType rt;
        rt.increase_alphabet_size_by(3);
        size_t rule_min = 100;
        size_t rule_max = 101;

        for (auto words : random_strings({0, 1, 2}, 2 * m, rule_min, rule_max)
                              | rx::in_groups_of_exactly(2)) {
          auto lhs = words.get();
          words.next();
          auto rhs = words.get();
          rewriting_system::add_rule(rt, lhs, rhs);
        }
        rt.reduce();

        BENCHMARK(fmt::format("{}-rules, rule length = [{}, {}], word length = "
                              "[{}, {}), for {} rewrites",
                              m,
                              rule_min,
                              rule_max,
                              sample_min,
                              sample_max,
                              sample.size())
                      .c_str()) {
          for (auto& word : sample) {
            auto copy(word);
            rt.rewrite(copy);
            REQUIRE(copy.size() != 0);
          }
        };
      }
    }

    TEMPLATE_TEST_CASE(
        "Number of rules (approx. no rewriting, but accessing trie)",
        "[LenLexTrie][number_of_rules][002]",
        LenLexTrie,
        LenLexSet) {
      auto rg = ReportGuard(false);

      size_t                   sample_size = 100;
      size_t                   sample_min  = 100;
      size_t                   sample_max  = 101;
      std::vector<std::string> sample
          = (random_strings({0, 1, 2}, sample_size, sample_min, sample_max)
             | rx::to_vector());

      for (size_t m = 50; m <= 1000; m += 50) {
        TestType rt;
        rt.increase_alphabet_size_by(3);
        size_t rule_min = 4;
        size_t rule_max = 32;

        for (auto words : random_strings({0, 1, 2}, 2 * m, rule_min, rule_max)
                              | rx::in_groups_of_exactly(2)) {
          auto lhs = words.get();
          words.next();
          auto rhs = words.get();
          rewriting_system::add_rule(rt, lhs, rhs);
        }
        rt.reduce();

        BENCHMARK(fmt::format("{}-rules, rule length = [{}, {}], word length = "
                              "[{}, {}), for {} rewrites",
                              m,
                              rule_min,
                              rule_max,
                              sample_min,
                              sample_max,
                              sample.size())
                      .c_str()) {
          for (auto& word : sample) {
            auto copy(word);
            rt.rewrite(copy);
            REQUIRE(copy.size() != 0);
          }
        };
      }
    }

    TEMPLATE_TEST_CASE("Number of rules (more rewriting)",
                       "[number_of_rules][003]",
                       LenLexTrie,
                       LenLexSet) {
      auto rg = ReportGuard(false);

      size_t                   sample_size = 1000;
      size_t                   sample_min  = 100;
      size_t                   sample_max  = 200;
      std::vector<std::string> sample
          = (random_strings({0, 1, 2}, sample_size, sample_min, sample_max)
             | rx::to_vector());

      for (size_t m = 5 * sample_size; m <= 10 * sample_size;
           m += sample_size / 2) {
        size_t rule_min = 4;
        size_t rule_max = 32;
        auto   rt       = generate_rewriter_random_subwords<TestType>(
            sample, m, rule_min, rule_max);
        rt.reduce();

        BENCHMARK(fmt::format("{}-rules, rule length = [{}, {}], word length = "
                              "[{}, {}), for {} rewrites",
                              m,
                              rule_min,
                              rule_max,
                              sample_min,
                              sample_max,
                              sample.size())
                      .c_str()) {
          for (auto& word : sample) {
            auto copy(word);
            rt.rewrite(copy);
            REQUIRE(copy.size() != 0);
          }
        };
      }
    }

    TEST_CASE("Number of rules (lots of rewriting)", "[number_of_rules][004]") {
      auto rg = ReportGuard(false);

      size_t                   sample_size = 1000;
      size_t                   sample_min  = 1000;
      size_t                   sample_max  = 2000;
      std::vector<std::string> sample
          = (random_strings({0, 1, 2}, sample_size, sample_min, sample_max)
             | rx::to_vector());

      for (size_t rule_min = 1; rule_min < 11; ++rule_min) {
        size_t rule_max = rule_min + 1;
        auto   rt = generate_rewriter_all_words<LenLexTrie>(rule_min, rule_max);
        rt.reduce();

        BENCHMARK(fmt::format("LenLexTrie, {}-rules, rule length = "
                              "[{}, {}], word length = "
                              "[{}, {}), for {} rewrites",
                              rt.active_rules().size(),
                              rule_min,
                              rule_max,
                              sample_min,
                              sample_max,
                              sample_size)
                      .c_str()) {
          for (auto& word : sample) {
            auto copy(word);
            rt.rewrite(copy);
            REQUIRE(copy.size() < rule_max);
          }
        };

        auto rfl = generate_rewriter_all_words<LenLexSet>(rule_min, rule_max);
        rfl.reduce();

        BENCHMARK(fmt::format("LenLexSet, {}-rules, rule length = [{}, "
                              "{}], word length = "
                              "[{}, {}), for {} rewrites",
                              rfl.active_rules().size(),
                              rule_min,
                              rule_max,
                              sample_min,
                              sample_max,
                              sample_size)
                      .c_str()) {
          for (auto& word : sample) {
            auto copy(word);
            rfl.rewrite(copy);
            REQUIRE(copy.size() < rule_max);
          }
        };
      }
    }

  }  // namespace detail

}  // namespace libsemigroups
