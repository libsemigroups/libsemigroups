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
//

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_MAIN

#include <algorithm>  // for transform, find_if
#include <cctype>     // for tolower
#include <chrono>     // for nanoseconds
#include <cmath>      // for pow
#include <cstdlib>    // for exit, size_t
#include <string>     // for char_traits, allocator, hash
#include <vector>     // for vector

#include "catch.hpp"  // for Colour, Colour::Code::BrightRed, CATCH_REGISTER...

#include "libsemigroups/detail/fmt.hpp"
#include "libsemigroups/detail/string.hpp"  // for to_string, unicode_string_length
#include "libsemigroups/detail/timer.hpp"  // for Timer

struct LibsemigroupsListener : Catch::TestEventListenerBase {
  using TestEventListenerBase::TestEventListenerBase;  // inherit constructor

  void extreme_test_divider(std::string_view sv) const {
    using libsemigroups::detail::unicode_string_length;
    auto msg  = fmt::format(fmt::emphasis::bold,
                           "* [{}]: {} - {} *\n",
                           test_number(),
                           test_name(),
                           sv);
    auto rule = fmt::format(
        fmt::emphasis::bold, "{:*^{w}}\n", "", fmt::arg("w", msg.size() - 9));
    fmt::print(rule + msg + rule);
  }

  std::string_view test_name(Catch::TestCaseInfo const& testInfo) {
    _test_name = testInfo.name;
    return _test_name;
  }

  std::string_view test_name() const {
    return _test_name;
  }

  std::string_view test_number(Catch::TestCaseInfo const& testInfo) {
    for (auto const& tag : testInfo.tags) {
      if (tag.size() == 3
          && std::all_of(tag.cbegin(), tag.cend(), [](auto const& c) {
               return std::isdigit(c);
             })) {
        _test_number = tag;
        return _test_number;
      }
    }
    return "";
  }

  std::string_view test_number() const {
    return _test_number;
  }

  std::string_view test_category(Catch::TestCaseInfo const& testInfo) {
    if (find_tag(testInfo, "quick")) {
      _test_category = "quick";
    } else if (find_tag(testInfo, "standard")) {
      _test_category = "standard";
    } else if (find_tag(testInfo, "extreme")) {
      _test_category = "extreme";
    } else if (find_tag(testInfo, "fail")) {
      _test_category = "fail";
    }
    return _test_category;
  }

  std::string_view test_category() const {
    return _test_category;
  }

  std::string section_time(Catch::SectionStats const& sectionStats) {
    auto t = static_cast<uint64_t>(sectionStats.durationInSeconds
                                   * std::pow(10, 9));
    return libsemigroups::detail::string_time(std::chrono::nanoseconds(t));
  }

  bool find_tag(Catch::TestCaseInfo const& testInfo, std::string tag) {
    std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

    return std::find_if(testInfo.tags.cbegin(),
                        testInfo.tags.cend(),
                        [&tag](std::string t) -> bool {
                          std::transform(
                              t.begin(), t.end(), t.begin(), ::tolower);
                          return t == tag;
                        })
           != testInfo.tags.cend();
  }

  // Not currently used
  // void check_category(Catch::TestCaseInfo const& testInfo) {
  //   if (!(find_tag(testInfo, "quick") || find_tag(testInfo, "standard")
  //         || find_tag(testInfo, "extreme") || find_tag(testInfo, "fail"))) {
  //     {
  //       Catch::Colour colourGuard(Catch::Colour::BrightRed);
  //       Catch::cerr()
  //           << "Missing category tag: [quick|standard|extreme|fail]!\n"
  //           << "  in test case at " << testInfo.lineInfo.file << ":"
  //           << testInfo.lineInfo.line << "\n";
  //     }
  //     std::exit(1);
  //   }
  // }

  void init_test_case(Catch::TestCaseInfo const& testInfo) {
    test_name(testInfo);
    test_number(testInfo);
    test_category(testInfo);
    _section_number = 0;
    _test_time      = 0;
  }

  void testCaseStarting(Catch::TestCaseInfo const& testInfo) override {
    init_test_case(testInfo);
    if (test_category() != "extreme") {
      auto const prefix_prefix = fmt::format("[{}]: ", test_number());
      auto const prefix_pad    = prefix_cols - prefix_prefix.size() - 1;

      std::string const trunc_name(
          testInfo.name.begin(),
          testInfo.name.begin() + std::min(prefix_pad, testInfo.name.size()));
      // This is the prefix of length (line_cols - time_cols)
      fmt::print("{}{:<{}} ", prefix_prefix, trunc_name, prefix_pad);
    } else {
      extreme_test_divider("START");
    }
  }

  void sectionStarting(Catch::SectionInfo const& sectionInfo) override {
    if (test_category() != "extreme" && _section_number == 1) {
      constexpr std::string_view prefix_prefix = "\n-- with ";
      auto const prefix_pad = prefix_cols - prefix_prefix.size() + 1;

      fmt::print("{}{: <{}}", prefix_prefix, sectionInfo.name, prefix_pad);
    }
    _section_number++;
  }

  void sectionEnded(Catch::SectionStats const& sectionStats) override {
    auto section_ns = static_cast<uint64_t>(sectionStats.durationInSeconds
                                            * std::pow(10, 9));
    _total_time += section_ns;
    _test_time += section_ns;
    if (test_category() != "extreme") {
      if (_section_number == 1) {
        fmt::print("{:.>{}}", section_time(sectionStats), time_cols);
      }
    } else {
      extreme_test_divider("END - " + section_time(sectionStats));
    }
    _section_number--;
  }

  void testCaseEnded(Catch::TestCaseStats const&) override {
    if (test_category() != "extreme") {
      fmt::print("\n");
    } else {
      extreme_test_divider("END - "
                           + libsemigroups::detail::string_time(
                               std::chrono::nanoseconds(_test_time)));
    }
  }

  void testRunEnded(Catch::TestRunStats const&) override {
    using libsemigroups::detail::string_time;
    using std::chrono::nanoseconds;

    auto const emph
        = fmt::emphasis::bold | fg(fmt::terminal_color::bright_green);
    constexpr std::string_view prefix = "Total time ";
    auto const                 t      = string_time(nanoseconds(_total_time));
    auto const                 prefix_pad = line_cols - prefix.size();

    fmt::print(emph, "{:=>{}}\n", "", line_cols);
    fmt::print(emph, "{}{:.>{}}\n", prefix, t, prefix_pad);
  }

  size_t _total_time = 0;

  size_t _section_number;

  std::string _test_name;
  std::string _test_number;
  std::string _test_category;
  uint64_t    _test_time;

  static constexpr size_t line_cols   = 79;
  static constexpr size_t time_cols   = 12;
  static constexpr size_t prefix_cols = line_cols - time_cols;
};

CATCH_REGISTER_LISTENER(LibsemigroupsListener)
