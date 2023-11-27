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

// This file contains a class for reporting things during a computation.

#ifndef LIBSEMIGROUPS_DETAIL_REPORT_HPP_
#define LIBSEMIGROUPS_DETAIL_REPORT_HPP_

#include <cstddef>  // for size_t
#include <mutex>    // for mutex, lock_guard
#include <string>   // for string
#include <thread>   // for get_id, thread, thread::id
#include <utility>  // for pair

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/printf.h>

#include "formatters.hpp"  // for custom formatters
#include "timer.hpp"       // for string_format, to_strin

namespace libsemigroups {

  namespace detail {
    using t_id = size_t;

    t_id this_threads_id();
    t_id thread_id(std::thread::id);
    void reset_thread_ids();

    bool is_report_suppressed_for(std::string_view);
  }  // namespace detail

  bool reporting_enabled() noexcept;

  template <typename... Args>
  std::string fmt_default(std::string_view sv, Args&&... args) {
    auto        tid     = detail::this_threads_id();
    std::string fmt_str = "#{}: ";
    fmt_str.append(sv.begin(), sv.end());
    return fmt::format(fmt_str, tid, std::forward<Args>(args)...);
  }

  template <typename... Args>
  void report_no_prefix(std::string_view sv, Args&&... args) {
    static std::mutex mtx;

    if (reporting_enabled()) {
      std::lock_guard<std::mutex> lg(mtx);
      fmt::print(sv, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  void report_default(std::string_view sv, Args&&... args) {
    if (reporting_enabled()) {
      std::string_view prefix(sv);
      auto             pos = prefix.find(":");
      if (pos != std::string::npos) {
        prefix.remove_suffix(prefix.size() - prefix.find(":"));
        if (detail::is_report_suppressed_for((prefix))) {
          return;
        }
      }
      report_no_prefix(fmt_default(sv, std::forward<Args>(args)...));
    }
  }

  static inline void
  report_elapsed_time(std::string_view                    prefix,
                      libsemigroups::detail::Timer const& tmr) {
    report_default("{} elapsed time {}", prefix, tmr);
  }

  //! This struct can be used to enable printing of some information during
  //! various of the computation in ``libsemigroups``. Reporting is enable (or
  //! not) at construction time, and disable when the ReportGuard goes out of
  //! scope.
  struct ReportGuard {
    //! Constructs a ReportGuard with reporting enabled by default.
    //!
    //! \param val whether to report or not (default: \c true).
    explicit ReportGuard(bool val = true);
    ~ReportGuard();
  };

  class SuppressReportFor {
    std::string_view _prefix;

   public:
    SuppressReportFor(std::string_view const&);
    ~SuppressReportFor();
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_DETAIL_REPORT_HPP_
