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

#include "test-main.hpp"

#include <algorithm>         // for transform, all_of, find_if
#include <cctype>            // for isdigit, tolower
#include <chrono>            // for nanoseconds
#include <cmath>             // for pow
#include <cstdint>           // for uint64_t
#include <cstdlib>           // for size_t
#include <initializer_list>  // for initializer_list
#include <string>            // for basic_string, allocator
#include <string_view>       // for basic_string_view, string_...
#include <vector>            // for vector

#include "catch_amalgamated.hpp"  // for Colour, Colour::Code::BrightRed, CATCH_REGISTER...

#include "libsemigroups/paths.hpp"   // for paths::algorithm
#include "libsemigroups/runner.hpp"  // for Runner
#include "libsemigroups/types.hpp"   // for tril

#include "libsemigroups/detail/fmt.hpp"    // for fmt
#include "libsemigroups/detail/timer.hpp"  // for string_time

CATCH_REGISTER_ENUM(libsemigroups::tril,
                    libsemigroups::tril::TRUE,
                    libsemigroups::tril::FALSE,
                    libsemigroups::tril::unknown);

CATCH_REGISTER_ENUM(libsemigroups::paths::algorithm,
                    libsemigroups::paths::algorithm::dfs,
                    libsemigroups::paths::algorithm::matrix,
                    libsemigroups::paths::algorithm::acyclic,
                    libsemigroups::paths::algorithm::automatic,
                    libsemigroups::paths::algorithm::trivial)

CATCH_REGISTER_ENUM(libsemigroups::Runner::state,
                    libsemigroups::Runner::state::never_run,
                    libsemigroups::Runner::state::running_to_finish,
                    libsemigroups::Runner::state::running_for,
                    libsemigroups::Runner::state::running_until,
                    libsemigroups::Runner::state::timed_out,
                    libsemigroups::Runner::state::stopped_by_predicate,
                    libsemigroups::Runner::state::not_running,
                    libsemigroups::Runner::state::dead);

#define CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS
#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_MAIN

namespace {
  bool find_tag(Catch::TestCaseInfo const& testInfo, std::string tag) {
    std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

    return std::find_if(testInfo.tags.cbegin(),
                        testInfo.tags.cend(),
                        [&tag](Catch::Tag const& t) -> bool {
                          auto t_copy = std::string(t.original);
                          std::transform(t_copy.begin(),
                                         t_copy.end(),
                                         t_copy.begin(),
                                         ::tolower);
                          return t_copy == tag;
                        })
           != testInfo.tags.cend();
  }

  std::string find_tag_starting_with(Catch::TestCaseInfo const& testInfo,
                                     char const*                prefix) {
    for (auto const& orig_tag : testInfo.tags) {
      auto tag = std::string(orig_tag.original);
      if (tag.rfind(prefix, 0) == 0) {
        tag.erase(tag.begin(), tag.begin() + std::strlen(prefix));
        return tag;
      }
    }
    return "";
  }

  std::string& rtrim(std::string&& s) {
    s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
    return s;
  }
}  // namespace

struct LibsemigroupsListener : Catch::EventListenerBase {
  using EventListenerBase::EventListenerBase;  // inherit constructor

  // Catch::TestCaseInfo is non-copyable so we have our own version here.
  struct TestCaseInfo {
    std::string category;
    std::string name;
    std::string number;

    TestCaseInfo() = default;

    explicit TestCaseInfo(Catch::TestCaseInfo const& testInfo)
        : TestCaseInfo() {
      *this = testInfo;
    }

    TestCaseInfo& operator=(Catch::TestCaseInfo const& testInfo) {
      name   = testInfo.name;
      number = find_tag_starting_with(testInfo, LIBSEMIGROUPS_TEST_NUM);

      for (char const* cat : {"quick", "standard", "extreme", "fail"}) {
        if (find_tag(testInfo, cat)) {
          category = cat;
          break;
        }
      }
      check_category(testInfo);
      check_unique_number(testInfo);
      check_title_length(testInfo);
      return *this;
    }

    void check_category(Catch::TestCaseInfo const& testInfo) const {
      if (category.empty()) {
        Catch::cerr() << fmt::format(_warn_emph,
                                     "WARNING - Missing category tag: "
                                     "[quick|standard|extreme|fail]!\n"
                                     "  in: \"{}\" \n  at: {}:{}\n",
                                     rtrim(to_string(*this)),
                                     testInfo.lineInfo.file,
                                     testInfo.lineInfo.line);
      }
    }

    void check_unique_number(Catch::TestCaseInfo const& testInfo) const {
      static std::unordered_map<std::string, std::string> test_numbers;

      std::string pos = fmt::format(
          "{}:{}", testInfo.lineInfo.file, testInfo.lineInfo.line);
      auto id = find_tag_starting_with(testInfo, LIBSEMIGROUPS_TEST_PREFIX);
      auto [it, inserted] = test_numbers.emplace(id, pos);

      if (!inserted && pos != it->second) {
        // the 2nd condition in the prev. line stops warnings about template
        // test cases
        Catch::cerr() << fmt::format(
            _warn_emph,
            "WARNING - Duplicate test case number: \"{}\"\n"
            "  previous: {}\n  current:  {}\n",
            id,
            it->second,
            pos);
      }
    }

    void check_title_length(Catch::TestCaseInfo const& testInfo) const {
      auto const   prefix = fmt::format("[{}]: ", number);
      size_t const N
          = ::libsemigroups::detail::unicode_string_length(testInfo.name);
      if (prefix.size() + N + _time_cols > _line_cols) {
        Catch::cerr() << fmt::format(
            _warn_emph,
            "WARNING - Test case name too long:\n"
            "  {}:{}\n"
            "  \"{}\"\n"
            "  OMITTING {: <{}}{:^<{}}\n",
            testInfo.lineInfo.file,
            testInfo.lineInfo.line,
            testInfo.name,
            "",
            _line_cols - prefix.size() - _time_cols - 9,
            "",
            N - _line_cols + prefix.size() + _time_cols + 1);
      }
    }
  };

  // Catch::SectionInfo is non-copyable so we have our own version here.
  struct SectionInfo {
    std::string name;

    SectionInfo& operator=(Catch::SectionInfo const& si) {
      name = si.name;
      return *this;
    }
  };

  // Catch::SectionStats is non-copyable so we have our own version here.
  struct SectionStats {
    std::chrono::nanoseconds duration;
    std::string              name;
    // We could store the SectionStats.sectionInfo here instead of "name", but
    // the name we want is the most nested one, and the
    // SectionStats.sectionInfo.name is the least nested one.

    SectionStats(Catch::SectionStats const& ss, SectionInfo const& si)
        : duration(std::chrono::nanoseconds(
              static_cast<uint64_t>(ss.durationInSeconds * std::pow(10, 9)))),
          name(si.name) {}
  };

  void print_extreme_test_divider(std::string_view sv) const {
    auto msg  = fmt::format(_extreme_emph,
                           "[{}]: {} - {}\n",
                           current_test_case_info().number,
                           current_test_case_info().name,
                           sv);
    auto rule = fmt::format(
        _extreme_emph, "{:=>{}}\n", "", std::max(_line_cols, msg.size() - 9));
    fmt::print(rule + msg + rule);
  }

  void set_current_test_case_info(Catch::TestCaseInfo const& testInfo) {
    _current_test_case_info = testInfo;
    _section_depth          = 0;
  }

  TestCaseInfo const& current_test_case_info() const noexcept {
    return _current_test_case_info;
  }

  void set_most_recent_section_info(Catch::SectionInfo const& sectionInfo) {
    _most_recent_section_info = sectionInfo;
  }

  SectionInfo const& most_recent_section_info() const {
    return _most_recent_section_info;
  }

  static std::string to_string(TestCaseInfo const& testCaseInfo) {
    auto const prefix     = fmt::format("[{}]: ", testCaseInfo.number);
    auto const prefix_pad = _prefix_cols - prefix.size() - 1;

    std::string const trunc_name(
        testCaseInfo.name.begin(),
        testCaseInfo.name.begin()
            + std::min(prefix_pad, testCaseInfo.name.size()));
    // This is the prefix of length (_line_cols - _time_cols)
    return fmt::format("{}{:<{}} ", prefix, trunc_name, prefix_pad);
  }

  void testCaseStarting(Catch::TestCaseInfo const& testInfo) override {
    _current_section_name = testInfo.name;
    _test_case_time       = std::chrono::nanoseconds(0);
    set_current_test_case_info(testInfo);
    if (current_test_case_info().category != "extreme") {
      fmt::print("{}", to_string(current_test_case_info()));
    } else {
      print_extreme_test_divider("START");
    }
  }

  void sectionStarting(Catch::SectionInfo const& sectionInfo) override {
    // TODO handle arbitrary depth subsections
    set_most_recent_section_info(sectionInfo);
    if (current_test_case_info().category != "extreme") {
      if (_section_depth == 1 && _current_section_name != sectionInfo.name) {
        _current_section_name             = sectionInfo.name;
        constexpr std::string_view prefix = "\n-- with ";
        auto const prefix_pad             = _prefix_cols - prefix.size() + 1;
        fmt::print("{}{: <{}}", prefix, sectionInfo.name, prefix_pad);
      } else if (_section_depth == 2) {
        constexpr std::string_view prefix = "\n---- ";
        auto const prefix_pad             = _prefix_cols - prefix.size() + 1;
        fmt::print("{}{: <{}}", prefix, sectionInfo.name, prefix_pad);
      }
    } else {
      if (_section_depth > 0) {
        print_extreme_test_divider(sectionInfo.name + " - START");
      }
    }
    _section_depth++;
  }

  void sectionEnded(Catch::SectionStats const& sectionStats) override {
    using libsemigroups::detail::string_time;

    _section_depth--;

    if (_section_depth == 0) {
      _section_stats.emplace_back(sectionStats, most_recent_section_info());
      _test_run_time += _section_stats.back().duration;
      _test_case_time += _section_stats.back().duration;
      std::string section_duration
          = string_time(_section_stats.back().duration);
      if (current_test_case_info().category != "extreme") {
        fmt::print("{:.>{}}", section_duration, _time_cols);
      } else {
        if (most_recent_section_info().name != current_test_case_info().name) {
          // In this case the leaf section that was run was a proper
          // subsection, and not just the entire test case, so we print the
          // end of that proper subsection.
          print_extreme_test_divider(most_recent_section_info().name + " - "
                                     + section_duration + " - STOP");
        }
      }
    }
  }

  void testCaseEnded(Catch::TestCaseStats const&) override {
    using libsemigroups::detail::string_time;

    if (current_test_case_info().category == "extreme") {
      print_extreme_test_divider(string_time(_test_case_time) + " - STOP");
      if (_section_stats.size() > 1) {
        fmt::print(_extreme_emph, "{:=>{}}\n", "", _line_cols);
        fmt::print(_extreme_emph,
                   "Summary for {}",
                   to_string(current_test_case_info()));
        constexpr std::string_view prefix = "\n-- with ";
        for (SectionStats const& ss : _section_stats) {
          auto const prefix_pad = _prefix_cols - prefix.size() + 1;
          fmt::print(_extreme_emph, "{}{: <{}}", prefix, ss.name, prefix_pad);
          fmt::print(
              _extreme_emph, "{:.>{}}", string_time(ss.duration), _time_cols);
        }
        fmt::print(_extreme_emph, "\n{:=>{}}", "", _line_cols);
        fmt::print("\n");
      }
    } else {
      fmt::print("\n");
    }
    _section_stats.clear();
  }

  void testRunEnded(Catch::TestRunStats const&) override {
    using libsemigroups::detail::string_time;

    constexpr std::string_view prefix     = "Total time ";
    auto const                 t          = string_time(_test_run_time);
    auto const                 prefix_pad = _line_cols - prefix.size();

    fmt::print(_run_end_emph, "{:=>{}}\n", "", _line_cols);
    fmt::print(_run_end_emph, "{}{:.>{}}\n", prefix, t, prefix_pad);
    // The following =s fill in the line printed by catch to make it the same
    // width as _line_cols
    fmt::print(_run_end_emph, "{:=>{}}", "", _line_cols - 79);
  }

  std::chrono::nanoseconds _test_run_time = std::chrono::nanoseconds(0);
  std::chrono::nanoseconds _test_case_time;

  size_t      _section_depth;
  std::string _current_section_name;

  TestCaseInfo              _current_test_case_info;
  SectionInfo               _most_recent_section_info;
  std::vector<SectionStats> _section_stats;

  static constexpr size_t _line_cols   = 90;
  static constexpr size_t _time_cols   = 12;
  static constexpr size_t _prefix_cols = _line_cols - _time_cols;

  static constexpr auto _extreme_emph = fmt::emphasis::bold;
  static constexpr auto _run_end_emph
      = fmt::emphasis::bold | fg(fmt::terminal_color::bright_green);
  static constexpr auto _warn_emph
      = fmt::emphasis::bold | fg(fmt::terminal_color::red);
};

CATCH_REGISTER_LISTENER(LibsemigroupsListener)
