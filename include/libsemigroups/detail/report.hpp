//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

#include "libsemigroups/exception.hpp"  // LIBSEMIGROUPS_EXCEPTION

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
    // TODO(1) prevent too many Tickers being created at a time, really just
    // launch a single thread to do reporting for the duration.
    class Ticker {
      class TickerImpl;

      TickerImpl* _ticker_impl;

     public:
      // Construct a Ticker that calls \c func every \c time, until the Ticker
      // object is destructed.
      template <typename Func, typename Time = std::chrono::seconds>
      explicit Ticker(Func&& func, Time time = std::chrono::seconds(1));

      template <typename Func, typename Time = std::chrono::seconds>
      void operator()(Func&& func, Time time = std::chrono::seconds(1));

      Ticker() : _ticker_impl(nullptr) {}
      Ticker(Ticker const&)            = delete;
      Ticker(Ticker&&)                 = delete;
      Ticker& operator=(Ticker const&) = delete;
      Ticker& operator=(Ticker&&)      = delete;

      ~Ticker();
    };

    enum class Align : uint8_t { left, right };

    // This object is a helper for formatting information reported by various
    // classes in libsemigroups such as ToddCoxeterImpl, KnuthBendixImpl, etc.
    //
    // The idea is to store the rows in the _rows, and to properly align the
    // values in each column. This is done by storing the rows and their widths,
    // then using std::apply to call report_default on the properly aligned
    // columns. This happens when the ReportCell object is destroyed.
    template <size_t C>
    class ReportCell {
     private:
      using Row = std::array<std::string, C + 1>;
      std::array<Align, C + 1>  _align;
      std::array<size_t, C + 1> _col_widths;
      std::vector<Row>          _rows;

     public:
      ReportCell() : _align(), _col_widths(), _rows() {
        _align.fill(Align::right);
        _col_widths.fill(0);
      }

      ReportCell(ReportCell const&)            = default;
      ReportCell(ReportCell&&)                 = default;
      ReportCell& operator=(ReportCell const&) = default;
      ReportCell& operator=(ReportCell&&)      = default;

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

      ReportCell& align(size_t col, Align val) noexcept {
        LIBSEMIGROUPS_ASSERT(col < C);
        _align[col + 1] = val;
        return *this;
      }

      ReportCell& align(Align val) noexcept {
        _align.fill(val);
        return *this;
      }

      Align align(size_t col) const noexcept {
        LIBSEMIGROUPS_ASSERT(col < C);
        return _align[col + 1];
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
      void emit();
    };  // ReportCell

  }  // namespace detail

  //! No doc
  bool reporting_enabled() noexcept;

  //! No doc
  template <typename... Args>
  std::string fmt_default(std::string_view sv, Args&&... args) {
    std::string prefix = fmt::format("#{}: ", detail::this_threads_id());
    return prefix + fmt::format(sv, std::forward<Args>(args)...);
  }

  //! No doc
  std::mutex& report_mutex();

  //! No doc
  // This function is provided to allow multiple lines of output to block other
  // lines from being interspersed, by first locking the report_mutex.
  template <typename... Args>
  void report_no_lock_no_prefix(std::string_view sv, Args&&... args) {
    auto line = fmt::format(sv, std::forward<Args>(args)...);
    fmt::print("{}", line);
  }

  //! No doc
  template <typename... Args>
  void report_no_prefix(std::string_view sv, Args&&... args) {
    if (reporting_enabled()) {
      std::lock_guard<std::mutex> lg(report_mutex());
      report_no_lock_no_prefix(sv, std::forward<Args>(args)...);
    }
  }

  //! No doc
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

  //! \ingroup core_classes_group
  //!
  //! \brief Struct for specifying whether or not to report about an
  //! algorithm's performance.
  //!
  //! This struct can be used to enable printing of some information during
  //! various of the computation in `libsemigroups`. Reporting is enabled (or
  //! not) at construction time, and disabled when the ReportGuard goes out of
  //! scope.
  struct ReportGuard {
    //! \brief Constructs a ReportGuard with reporting enabled by default.
    //!
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
