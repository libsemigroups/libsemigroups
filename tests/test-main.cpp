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

#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_MAIN

#include <cstdlib>        // for exit, size_t
#include <string>         // for string
#include <unordered_map>  // for unordered_map

#include "catch.hpp"  // for Colour, Colour::Code::BrightRed, CATCH_REGISTER...

#include "include/stl.hpp"     // for detail::to_string
#include "include/string.hpp"  // for detail::wrap
#include "include/timer.hpp"   // for detail::Timer

struct LibsemigroupsLineInfo {
  explicit LibsemigroupsLineInfo(Catch::TestCaseInfo const& testInfo)
      : _file(testInfo.lineInfo.file),
        _line(libsemigroups::detail::to_string(testInfo.lineInfo.line)) {}
  std::string _file;
  std::string _line;
};

struct LibsemigroupsListener : Catch::TestEventListenerBase {
  using TestEventListenerBase::TestEventListenerBase;  // inherit constructor

  void testCaseStarting(Catch::TestCaseInfo const& testInfo) override {
    _first_call = true;
    _one        = testInfo.name;
    _two        = " . . .";
    _three      = "";
    _pad        = 0;
    _depth      = 0;
    static std::unordered_map<std::string, LibsemigroupsLineInfo> map;
    auto pos = testInfo.name.find(":");
    if (pos == std::string::npos) {
      // Just run the test, and ignore the registration of the id
      return;
    }
    auto exp_tag
        = std::string(testInfo.name.cbegin(),
                      testInfo.name.cbegin() + testInfo.name.find(":"));
    if (!find_tag(testInfo, exp_tag)) {
      // Just run the test, and ignore the registration of the id
      return;
    }
    auto it = map.find(exp_tag);
    if (it != map.end()) {
      {
        Catch::Colour colourGuard(Catch::Colour::BrightRed);
        Catch::cerr() << "Duplicate test case: [" << exp_tag << "]!\n"
                      << "  first seen at: " << (*it).second._file << ":"
                      << (*it).second._line << "\n"
                      << "  redefined at:  " << testInfo.lineInfo.file << ":"
                      << testInfo.lineInfo.line << "\n";
      }
      std::exit(1);
    } else {
      map.emplace(exp_tag, LibsemigroupsLineInfo(testInfo));
    }
    check_category(testInfo);
  }

  void sectionEnded(Catch::SectionStats const& sectionStats) override {
    if (_one != sectionStats.sectionInfo.name) {
      _two = " . . .\n";
      if (_three.empty()) {
        _three = "  -- with " + sectionStats.sectionInfo.name;
      } else {
        _three += " and " + sectionStats.sectionInfo.name;
      }
      _pad = 79 - _three.size();
    } else {
      size_t t = static_cast<size_t>(sectionStats.durationInSeconds
                                     * std::pow(10, 9));
      if (_first_call) {
        if (_one.size() + _two.size() > 72) {
          _one = _one.substr(0, 66 - _two.size()) + " [...]";
        }
        Catch::Colour colourGuard(Catch::Colour::BrightWhite);
        std::cout << _one << _two;
        _first_call = false;
      }
      if (_three.empty()) {
        _pad = 79 - (_one.size() + _two.size());
      }
      auto ts
          = libsemigroups::detail::Timer::string(std::chrono::nanoseconds(t));
      _pad -= libsemigroups::detail::unicode_string_length(ts);

      std::cout << _three << std::setw(_pad) << "";
      {
        Catch::Colour colourGuard(Catch::Colour::BrightWhite);
        std::cout << ts << "\n";
      }
      _three.clear();
    }
  }

  // The way the total time below is displayed is unattractive, so decided to
  // leave this out for the time being.
  // void testRunEnded(Catch::TestRunStats const&) override {
  // std::cout << libsemigroups::detail::Timer::string(
  //                 std::chrono::nanoseconds(_total_time))
  //          << std::endl;
  //}

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

  void check_category(Catch::TestCaseInfo const& testInfo) {
    if (!(find_tag(testInfo, "quick") || find_tag(testInfo, "standard")
          || find_tag(testInfo, "extreme") || find_tag(testInfo, "fail"))) {
      {
        Catch::Colour colourGuard(Catch::Colour::BrightRed);
        Catch::cerr()
            << "Missing category tag: [quick|standard|extreme|fail]!\n"
            << "  in test case at " << testInfo.lineInfo.file << ":"
            << testInfo.lineInfo.line << "\n";
      }
      std::exit(1);
    }
  }
  bool        _first_call;
  size_t      _total_time = 0;
  std::string _one;
  std::string _two;
  std::string _three;
  size_t      _pad;
  size_t      _depth;
};

CATCH_REGISTER_LISTENER(LibsemigroupsListener)
