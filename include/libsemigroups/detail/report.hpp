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

#include <chrono>      // for std::chrono
#include <cstddef>     // for size_t
#include <functional>  // for std::function
#include <mutex>       // for mutex, lock_guard
#include <string>      // for string
#include <thread>      // for get_id, thread, thread::id
#include <utility>     // for pair

#include "fmt.hpp"         // for fmtlib includes
#include "formatters.hpp"  // for custom formatters
#include "timer.hpp"       // for string_format, to_strin

namespace libsemigroups {

  namespace detail {
    using t_id = size_t;

    t_id this_threads_id();
    t_id thread_id(std::thread::id);
    void reset_thread_ids();

    bool is_report_suppressed_for(std::string_view);

    // This class provides a thread-safe means of calling a function every
    // second in a detached thread. The purpose of this class is so that the
    // TickerImpl, which contains the data required by the function to be
    // called, inside the Ticker, can outlive the Ticker, the TickerImpl is
    // deleted by the function called in the thread.
    // TODO(0) prevent too many Tickers being created at a time
    class Ticker {
      class TickerImpl;

      TickerImpl* _ticker_impl;

     public:
      // Construct a Ticker that calls \c func every \c time, until the Ticker
      // object is destructed.
      template <typename Func, typename Time = std::chrono::seconds>
      explicit Ticker(Func&& func, Time time = std::chrono::seconds(1));

      Ticker()                         = delete;
      Ticker(Ticker const&)            = delete;
      Ticker(Ticker&&)                 = delete;
      Ticker& operator=(Ticker const&) = delete;
      Ticker& operator=(Ticker&&)      = delete;

      ~Ticker();
    };

    // This object is a helper for formatting information reported by various
    // classes in libsemigroups such as ToddCoxeter, KnuthBendix, etc.
    //
    // The idea is to store the rows in the _rows, and to properly align the
    // values in each column. This is done by storing the rows and their widths,
    // then using std::apply to call report_default on the properly aligned
    // columns. This happens when the ReportCell object is destroyed.
    template <size_t C>
    class ReportCell {
     private:
      using Row = std::array<std::string, C + 1>;

      std::array<size_t, C + 1> _col_widths;
      std::vector<Row>          _rows;

     public:
      ReportCell() : _col_widths(), _rows() {
        _col_widths.fill(0);
      }

      ReportCell(ReportCell const&)            = delete;
      ReportCell(ReportCell&&)                 = delete;
      ReportCell& operator=(ReportCell const&) = delete;
      ReportCell& operator=(ReportCell&&)      = delete;

      ~ReportCell() {
        emit();
      }

      // Set the minimum width of every column
      ReportCell& min_width(size_t val) {
        _col_widths.fill(val);
        return *this;
      }

      // Set the minimum width of a specific column
      ReportCell& min_width(size_t col, size_t val) {
        LIBSEMIGROUPS_ASSERT(col < C);
        _col_widths[col + 1] = val;
        return *this;
      }

      // Insert a row using a format string and arguments
      template <typename... Args>
      void operator()(std::string_view fmt_str, Args&&... args);

      // Insert a row using a function, format string, and arguments. The
      // function is called on each of the arguments.
      template <typename Func, typename... Args>
      void operator()(Func&& f, char const* fmt_str, Args&&... args) {
        operator()(fmt_str, f(args)...);
      }

     private:
      size_t line_width() const;

      void emit();
    };  // ReportCell
  }  // namespace detail

  bool reporting_enabled() noexcept;

  template <typename... Args>
  std::string fmt_default(std::string_view sv, Args&&... args) {
    auto        tid     = detail::this_threads_id();
    std::string fmt_str = "#{}: ";
    fmt_str.append(sv.begin(), sv.end());
    return fmt::format(std::move(fmt_str), tid, std::forward<Args>(args)...);
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
    explicit SuppressReportFor(std::string_view);
    ~SuppressReportFor();
  };
}  // namespace libsemigroups

#include "report.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_REPORT_HPP_
