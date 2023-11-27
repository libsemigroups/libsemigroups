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

#include <cxxabi.h>  // for abi::

#include <array>          // for array
#include <atomic>         // for atomic
#include <cstddef>        // for size_t
#include <cstdint>        // for uint64_t
#include <cstdlib>        // for free
#include <mutex>          // for mutex, lock_guard
#include <string>         // for string
#include <thread>         // for get_id, thread, thread::id
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_FMT_ENABLED

#include "containers.hpp"  // for StaticVector1
#include "string.hpp"      // for string_format, to_string
#include "timer.hpp"       // for string_format, to_string

#include "textflowcpp/TextFlow.hpp"
#include <magic_enum/magic_enum.hpp>

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/printf.h>

namespace libsemigroups {
  namespace detail {

    class ThreadIdManager {
     public:
      ThreadIdManager();
      ThreadIdManager(ThreadIdManager const&)            = delete;
      ThreadIdManager(ThreadIdManager&&)                 = delete;
      ThreadIdManager& operator=(ThreadIdManager const&) = delete;
      ThreadIdManager& operator=(ThreadIdManager&&)      = delete;

      void   reset();
      size_t tid(std::thread::id t);

     private:
      std::mutex                                  _mtx;
      size_t                                      _next_tid;
      std::unordered_map<std::thread::id, size_t> _thread_map;
    };
  }  // namespace detail

  extern detail::ThreadIdManager THREAD_ID_MANAGER;

  namespace report {
    bool should_report() noexcept;
    bool suppress(std::string_view const&);
    bool is_suppressed(std::string_view const&);
    bool stop_suppressing(std::string_view const&);
  }  // namespace report

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

  template <typename... Args>
  void report_no_prefix(char const* s, Args&&... args) {
    static std::mutex mtx;

    if (report::should_report()) {
      std::lock_guard<std::mutex> lg(mtx);
      fmt::print(s, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  void report_no_prefix(std::string const& s, Args&&... args) {
    report_no_prefix(s.c_str(), std::forward<Args>(args)...);
  }

  // TODO use fmt_default
  // TODO write a this_thread_id function
  template <typename... Args>
  void report_default(char const* s, Args&&... args) {
    if (report::should_report()) {
      std::string_view sv(s);
      auto             pos = sv.find(":");
      if (pos != std::string::npos) {
        sv.remove_suffix(sv.size() - sv.find(":"));
        if (report::is_suppressed(sv)) {
          return;
        }
      }

      uint64_t    tid    = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
      std::string prefix = fmt::format("#{}: ", tid);
      report_no_prefix(prefix + s, std::forward<Args>(args)...);
    }
  }

  template <typename... Args>
  void report_default(std::string const& s, Args&&... args) {
    report_default(s.c_str(), std::forward<Args>(args)...);
  }

  template <typename... Args>
  std::string fmt_default(char const* s, Args&&... args) {
    if (report::should_report()) {
      uint64_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
      return fmt::format(
          std::string("#{}: ") + s, tid, std::forward<Args>(args)...);
    }
    return "";
  }

  class SuppressReportFor {
    std::string_view _name;

   public:
    SuppressReportFor(std::string_view const& name) : _name(name) {
      report::suppress(_name);
    }

    ~SuppressReportFor() {
      report::stop_suppressing(_name);
    }
  };

  namespace detail {
    template <typename T>
    bool string_time_incremental(std::string&              result,
                                 std::chrono::nanoseconds& elapsed) {
      T x = std::chrono::duration_cast<T>(elapsed);
      if (x.count() > 0) {
        result += fmt::format("{}", x);
        elapsed -= std::chrono::nanoseconds(x);
        return true;
      }
      return false;
    }

    bool string_time_incremental(std::string&              result,
                                 std::chrono::nanoseconds& elapsed,
                                 bool                      use_float = false);
  }  // namespace detail

  template <typename Time>
  std::string string_time(Time elapsed_arg) {
    using detail::string_time_incremental;

    std::string out;

    std::chrono::nanoseconds elapsed = elapsed_arg;

    // TODO add day, months etc
    if (string_time_incremental<std::chrono::hours>(out, elapsed)) {
      string_time_incremental<std::chrono::minutes>(out, elapsed);
      string_time_incremental(out, elapsed, false);
    } else if (string_time_incremental<std::chrono::minutes>(out, elapsed)) {
      string_time_incremental(out, elapsed, false);
    } else if (string_time_incremental<std::chrono::seconds>(out, elapsed)) {
    } else if (string_time_incremental<std::chrono::milliseconds>(out,
                                                                  elapsed)) {
    } else if (string_time_incremental<std::chrono::microseconds>(out,
                                                                  elapsed)) {
    } else if (string_time_incremental<std::chrono::nanoseconds>(out,
                                                                 elapsed)) {
    }
    return out;
  }

  template <typename Arg>
  std::string italic(Arg&& arg) {
    if (report::should_report()) {
      return fmt::format(fmt::emphasis::italic, "{}", std::forward<Arg>(arg));
    }
    return "";
  }

  template <typename Arg>
  std::string italic(char const* s, Arg&& arg) {
    if (report::should_report()) {
      return fmt::format(fmt::emphasis::italic, s, std::forward<Arg>(arg));
    }
    return "";
  }

  namespace report {
    static inline void elapsed_time(std::string_view                    prefix,
                                    libsemigroups::detail::Timer const& tmr) {
      report_default("{} elapsed time {}", prefix, tmr);
    }
  }  // namespace report
}  // namespace libsemigroups

template <typename T, typename Char>
struct fmt::formatter<T, Char, std::enable_if_t<std::is_enum_v<T>>>
    : fmt::formatter<std::string> {
  template <typename FormatContext>
  auto format(T knd, FormatContext& ctx) const {
    auto name = magic_enum::enum_name(knd);
    return formatter<string_view>::format(name, ctx);
  }
};

template <typename T, typename Char>
struct fmt::formatter<std::vector<T>, Char> : fmt::formatter<std::string> {
  template <typename FormatContext>
  auto format(std::vector<T> const& v, FormatContext& ctx) const {
    return formatter<string_view>::format(libsemigroups::detail::to_string(v),
                                          ctx);
  }
};

template <typename T, size_t N, typename Char>
struct fmt::formatter<libsemigroups::detail::StaticVector1<T, N>, Char>
    : fmt::formatter<std::string> {
  template <typename FormatContext>
  auto format(libsemigroups::detail::StaticVector1<T, N> const& v,
              FormatContext&                                    ctx) const {
    return formatter<string_view>::format(libsemigroups::detail::to_string(v),
                                          ctx);
  }
};

template <>
struct fmt::formatter<libsemigroups::detail::Timer>
    : fmt::formatter<std::string> {
  template <typename FormatContext>
  auto format(libsemigroups::detail::Timer const& v, FormatContext& ctx) const {
    return formatter<string_view>::format(v.string(), ctx);
  }
};

#endif  // LIBSEMIGROUPS_DETAIL_REPORT_HPP_
