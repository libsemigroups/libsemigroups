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
#include <iostream>   // for string, operator<<, basic_ostream
#include <string>     // for char_traits, allocator, hash
#include <vector>     // for vector

#include "catch.hpp"  // for Colour, Colour::Code::BrightRed, CATCH_REGISTER...

#include "libsemigroups/detail/string.hpp"  // for to_string, unicode_string_length
#include "libsemigroups/detail/timer.hpp"  // for Timer

#include <fmt/color.h>
#include <fmt/format.h>

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
    return libsemigroups::detail::Timer::string(std::chrono::nanoseconds(t));
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

  void init(Catch::TestCaseInfo const& testInfo) {
    test_name(testInfo);
    test_number(testInfo);
    test_category(testInfo);
  }

  void testCaseStarting(Catch::TestCaseInfo const& testInfo) override {
    init(testInfo);
    if (test_category() != "extreme") {
      std::string short_name(
          testInfo.name.begin(),
          testInfo.name.begin()
              + std::min(static_cast<size_t>(59), testInfo.name.size()));
      fmt::print("[{}]: {:<59} ", test_number(), short_name);
    } else {
      extreme_test_divider("START");
    }
  }

  void sectionEnded(Catch::SectionStats const& sectionStats) override {
    _total_time += static_cast<uint64_t>(sectionStats.durationInSeconds
                                         * std::pow(10, 9));
    if (test_category() == "extreme") {
      extreme_test_divider("END - " + section_time(sectionStats));
    } else {
      fmt::print("{:.>12}\n", section_time(sectionStats));
      return;
    }
  }

  void testRunEnded(Catch::TestRunStats const&) override {
    using Timer_ = libsemigroups::detail::Timer;
    using std::chrono::nanoseconds;

    auto const emph
        = fmt::emphasis::bold | fg(fmt::terminal_color::bright_green);
    std::string_view const prefix = "Total time ";
    auto const             t      = Timer_::string(nanoseconds(_total_time));

    fmt::print(emph, "{:=>79}\n", "");
    fmt::print(
        emph, "{}{:.>{w}}\n", prefix, t, fmt::arg("w", 79 - prefix.size()));
  }

  size_t _total_time = 0;

  std::string _test_name;
  std::string _test_number;
  std::string _test_category;
  std::string _test_starting_string;
};

CATCH_REGISTER_LISTENER(LibsemigroupsListener)
