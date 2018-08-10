//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

#include <unordered_map>

#include "include/internal/stl.h"
#include "include/internal/timer.h"

#include "libsemigroups.tests.hpp"

struct LibsemigroupsLineInfo {
  explicit LibsemigroupsLineInfo(Catch::TestCaseInfo const& testInfo)
      : _file(testInfo.lineInfo.file),
        _line(libsemigroups::to_string(testInfo.lineInfo.line)) {}
  std::string _file;
  std::string _line;
};

struct LibsemigroupsListener : Catch::TestEventListenerBase {
  using TestEventListenerBase::TestEventListenerBase;  // inherit constructor

  void testCaseStarting(Catch::TestCaseInfo const& testInfo) override {
    _msg = testInfo.name;
    static std::unordered_map<std::string, LibsemigroupsLineInfo>
      map;
    auto pos = testInfo.name.find(":");
    if (pos == std::string::npos) {
      // Just run the test, and ignore the registration of the id
      return;
    }
    auto exp_tag
        = std::string(testInfo.name.cbegin(),
                      testInfo.name.cbegin() + testInfo.name.find(":"));

    auto fnd_tag
        = std::find_if(testInfo.tags.cbegin(), testInfo.tags.cend(),
            [&exp_tag](std::string const& tag) -> bool {
              return tag == exp_tag;
            });
    if (fnd_tag == testInfo.tags.end()) {
      // Just run the test, and ignore the registration of the id
      return;
    }
    auto it = map.find(*fnd_tag);
    if (it != map.end()) {
      {
        Catch::Colour colourGuard(Catch::Colour::Red);
        Catch::cerr() << "Duplicate test case id: [" << *fnd_tag << "]!\n"
                      << "  first seen at: " << (*it).second._file << ":"
                      << (*it).second._line << "\n"
                      << "  redefined at:  " << testInfo.lineInfo.file << ":"
                      << testInfo.lineInfo.line << "\n";
      }
      std::exit(1);
    } else {
      map.emplace(testInfo.tags.at(1), testInfo);
    }
  }

  void sectionEnded(Catch::SectionStats const& sectionStats) override {
    std::cout << _msg << " ("
              << libsemigroups::Timer::string(
                     std::chrono::nanoseconds(static_cast<size_t>(
                         sectionStats.durationInSeconds * std::pow(10, 9))))
              << ")" << std::endl;
  }

  std::string _msg;
};

CATCH_REGISTER_LISTENER(LibsemigroupsListener)
