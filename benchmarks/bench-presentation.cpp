//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#include "libsemigroups/config.hpp"        // for LIBSEMIGROUPS_CATCH_ALL_HEADER
#include "libsemigroups/presentation.hpp"  // for Presentation

#include LIBSEMIGROUPS_CATCH_ALL_HEADER  // for BENCHMARK, TEST_CASE

namespace libsemigroups {

  namespace old {
    template <typename Word, typename Iterator1, typename Iterator2>
    void replace_subword(Presentation<Word>& p,
                         Iterator1           first_existing,
                         Iterator1           last_existing,
                         Iterator2           first_replacement,
                         Iterator2           last_replacement) {
      if (first_existing == last_existing) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd and 3rd argument must not be equal");
      }
      auto rplc_sbwrd = [&first_existing,
                         &last_existing,
                         &first_replacement,
                         &last_replacement](Word& word) {
        size_t const M  = std::distance(first_existing, last_existing);
        size_t const N  = std::distance(first_replacement, last_replacement);
        auto         it = std::search(
            word.begin(), word.end(), first_existing, last_existing);
        while (it != word.end()) {
          // found existing
          auto replacement_first = it - word.begin();
          word.erase(it, it + M);
          word.insert(word.begin() + replacement_first,
                      first_replacement,
                      last_replacement);
          it = std::search(word.begin() + replacement_first + N,
                           word.end(),
                           first_existing,
                           last_existing);
        }
      };
      std::for_each(p.rules.begin(), p.rules.end(), rplc_sbwrd);
    }

    template <typename Word>
    void replace_subword(Presentation<Word>& p,
                         Word const&         existing,
                         Word const&         replacement) {
      replace_subword(p,
                      existing.cbegin(),
                      existing.cend(),
                      replacement.cbegin(),
                      replacement.cend());
    }
  }  // namespace old

  using std::literals::operator""s;

  TEST_CASE("replace_subword (M = existing.size(), N = "
            "replacement.size())",
            "[quick]") {
    Presentation<std::string> p;
    p.alphabet("ab"s).contains_empty_word(true);

    size_t const num_rules = 1'000'000;
    p.rules = random_strings("ab"s, num_rules, 8, 9) | rx::to_vector();
    REQUIRE(p.rules.size() == num_rules);

    auto compare = [&](char const* name,
                       auto        replace,
                       auto const& existing,
                       auto const& replacement) {
      BENCHMARK_ADVANCED(name)
      (Catch::Benchmark::Chronometer meter) {
        std::vector<Presentation<std::string>> inputs(meter.runs(), p);

        meter.measure(
            [&](int i) { replace(inputs[i], existing, replacement); });
      };  // NOLINT
    };

    compare(
        "old version (1 = M < N = 2)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "a"s,
        "ab"s);

    compare(
        "new version (1 = M < N = 2)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "a"s,
        "ab"s);

    compare(
        "old version (1 = M < N = 4)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "a"s,
        "abab"s);

    compare(
        "new version (1 = M < N = 4)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "a"s,
        "abab"s);

    compare(
        "old version (2 = M < N = 4)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "ab"s,
        "abab"s);

    compare(
        "new version (2 = M < N = 4)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "ab"s,
        "abab"s);

    compare(
        "old version (2 = M > N = 1)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "ab"s,
        "b"s);

    compare(
        "new version (2 = M > N = 1)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "ab"s,
        "b"s);

    compare(
        "old version (4 = M > N = 1)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "abab"s,
        "b"s);

    compare(
        "new version (4 = M > N = 1)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "abab"s,
        "b"s);

    compare(
        "old version (4 = M > N = 2)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "abab"s,
        "ba"s);

    compare(
        "new version (4 = M > N = 2)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "abab"s,
        "ba"s);

    compare(
        "old version (M = N = 1)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "a"s,
        "b"s);

    compare(
        "new version (M = N = 1)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "a"s,
        "b"s);

    compare(
        "old version (M = N = 2)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "ab"s,
        "ba"s);

    compare(
        "new version (M = N = 2)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "ab"s,
        "ba"s);

    compare(
        "old version (M = N = 4)",
        [](auto& p, auto const& from, auto const& to) {
          old::replace_subword(p, from, to);
        },
        "abaa"s,
        "babb"s);

    compare(
        "new version (M = N = 4)",
        [](auto& p, auto const& from, auto const& to) {
          presentation::replace_subword(p, from, to);
        },
        "abaa"s,
        "babb"s);
  }

}  // namespace libsemigroups
