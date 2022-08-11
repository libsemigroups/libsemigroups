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

#ifndef LIBSEMIGROUPS_REPORT_HPP_
#define LIBSEMIGROUPS_REPORT_HPP_

#include <cxxabi.h>  // for abi::

#include <atomic>         // for atomic
#include <cstddef>        // for size_t
#include <cstdint>        // for uint64_t
#include <cstdlib>        // for free
#include <iosfwd>         // for string, operator<<, cout, ost...
#include <mutex>          // for mutex, lock_guard
#include <string>         // for string
#include <thread>         // for get_id, thread, thread::id
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "config.hpp"  // for LIBSEMIGROUPS_FMT_ENABLED
#include "string.hpp"  // for string_format, to_string

#include "textflowcpp/TextFlow.hpp"

#ifdef LIBSEMIGROUPS_FMT_ENABLED
#include "fmt/color.h"
#include "fmt/printf.h"
#include <array>  // for array
#endif

// To avoid computing the parameters __VA_ARGS__ when we aren't even
// reporting, we check if we are reporting before calling REPORTER.
#define REPORT(...) \
  (REPORTER.report() ? REPORTER(__VA_ARGS__).prefix(this) : REPORTER)

#ifdef LIBSEMIGROUPS_FMT_ENABLED
#define REPORT_DEFAULT(...) REPORT(__VA_ARGS__).thread_color().flush();
#else
#define REPORT_DEFAULT(...) REPORT(__VA_ARGS__).flush();
#endif

#ifdef LIBSEMIGROUPS_DEBUG
#ifdef LIBSEMIGROUPS_FMT_ENABLED
#define REPORT_DEBUG(...) REPORT(__VA_ARGS__).color(fmt::color::dim_gray)
#else
#define REPORT_DEBUG(...) REPORT(__VA_ARGS__)
#endif
#define REPORT_DEBUG_DEFAULT(...) REPORT_DEBUG(__VA_ARGS__).flush();
#else
#define REPORT_DEBUG(...)
#define REPORT_DEBUG_DEFAULT(...)
#endif

#ifdef LIBSEMIGROUPS_VERBOSE
#ifdef LIBSEMIGROUPS_FMT_ENABLED
#define REPORT_VERBOSE(...) REPORT(__VA_ARGS__).color(fmt::color::cyan)
#else
#define REPORT_VERBOSE(...) REPORT(__VA_ARGS__)
#endif
#define REPORT_VERBOSE_DEFAULT(...) REPORT_VERBOSE(__VA_ARGS__).flush();
#else
#define REPORT_VERBOSE(...)
#define REPORT_VERBOSE_DEFAULT(...)
#endif

#define REPORT_TIME(var) \
  REPORT_DEFAULT("elapsed time (%s): %s\n", __func__, var.string().c_str());

namespace libsemigroups {
#ifndef LIBSEMIGROUPS_FMT_ENABLED
#define FORMAT(...) __VA_ARGS__
#else
#define FORMAT(...) fmt::format(__VA_ARGS__)
#endif

  namespace detail {

    class ThreadIdManager final {
     public:
      ThreadIdManager();
      ThreadIdManager(ThreadIdManager const&) = delete;
      ThreadIdManager(ThreadIdManager&&)      = delete;
      ThreadIdManager& operator=(ThreadIdManager const&) = delete;
      ThreadIdManager& operator=(ThreadIdManager&&) = delete;

      void   reset();
      size_t tid(std::thread::id t);

     private:
      std::mutex                                  _mtx;
      size_t                                      _next_tid;
      std::unordered_map<std::thread::id, size_t> _thread_map;
    };
  }  // namespace detail

  extern detail::ThreadIdManager THREAD_ID_MANAGER;

  namespace detail {
    template <typename T>
    static std::string string_class_name(T const* o) {
      static std::unordered_map<size_t, std::string> _class_name_map;
      auto it = _class_name_map.find(typeid(*o).hash_code());
      if (it != _class_name_map.end()) {
        return (*it).second;
      }
      int         status;
      char*       ptr = abi::__cxa_demangle(typeid(*o).name(), 0, 0, &status);
      std::string out = "";
      if (status == 0) {  // successfully demangled
        std::string full = std::string(ptr);
        size_t      last = full.size();
        if (full.back() == '>') {
          size_t bracket_count = 0;
          do {
            last = full.find_last_of("<>", last - 1);
            if (last != std::string::npos) {
              if (full.at(last) == '>') {
                bracket_count++;
              } else if (full.at(last) == '<') {
                bracket_count--;
              }
            }
          } while (bracket_count != 0);
        }
        size_t first = full.rfind("::", last - 1);
        first        = (first == std::string::npos ? 0 : first + 2);
        out          = full.substr(first, last - first);
      }
      free(ptr);
      // If we couldn't demangle the name, then just add the empty string to
      // the map
      _class_name_map.emplace(typeid(*o).hash_code(), out);
      return out;
    }

#ifdef LIBSEMIGROUPS_FMT_ENABLED
    static std::array<fmt::color, 146> const thread_colors
        = {fmt::color::white,
           fmt::color::red,
           fmt::color::green,
           fmt::color::blue,
           fmt::color::yellow,
           fmt::color::aquamarine,
           fmt::color::antique_white,
           fmt::color::aqua,
           fmt::color::azure,
           fmt::color::beige,
           fmt::color::bisque,
           fmt::color::blanched_almond,
           fmt::color::blue_violet,
           fmt::color::brown,
           fmt::color::burly_wood,
           fmt::color::cadet_blue,
           fmt::color::chartreuse,
           fmt::color::chocolate,
           fmt::color::coral,
           fmt::color::cornflower_blue,
           fmt::color::cornsilk,
           fmt::color::crimson,
           fmt::color::cyan,
           fmt::color::dark_blue,
           fmt::color::dark_cyan,
           fmt::color::dark_golden_rod,
           fmt::color::dark_gray,
           fmt::color::dark_green,
           fmt::color::dark_khaki,
           fmt::color::dark_magenta,
           fmt::color::dark_olive_green,
           fmt::color::dark_orange,
           fmt::color::dark_orchid,
           fmt::color::dark_red,
           fmt::color::dark_salmon,
           fmt::color::dark_sea_green,
           fmt::color::dark_slate_blue,
           fmt::color::dark_slate_gray,
           fmt::color::dark_turquoise,
           fmt::color::dark_violet,
           fmt::color::deep_pink,
           fmt::color::deep_sky_blue,
           fmt::color::dim_gray,
           fmt::color::dodger_blue,
           fmt::color::fire_brick,
           fmt::color::floral_white,
           fmt::color::forest_green,
           fmt::color::fuchsia,
           fmt::color::gainsboro,
           fmt::color::ghost_white,
           fmt::color::gold,
           fmt::color::golden_rod,
           fmt::color::gray,
           fmt::color::green_yellow,
           fmt::color::honey_dew,
           fmt::color::hot_pink,
           fmt::color::indian_red,
           fmt::color::indigo,
           fmt::color::ivory,
           fmt::color::khaki,
           fmt::color::lavender,
           fmt::color::lavender_blush,
           fmt::color::lawn_green,
           fmt::color::lemon_chiffon,
           fmt::color::light_blue,
           fmt::color::light_coral,
           fmt::color::light_cyan,
           fmt::color::light_golden_rod_yellow,
           fmt::color::light_gray,
           fmt::color::light_green,
           fmt::color::light_pink,
           fmt::color::light_salmon,
           fmt::color::light_sea_green,
           fmt::color::light_sky_blue,
           fmt::color::light_slate_gray,
           fmt::color::light_steel_blue,
           fmt::color::light_yellow,
           fmt::color::lime,
           fmt::color::lime_green,
           fmt::color::linen,
           fmt::color::magenta,
           fmt::color::maroon,
           fmt::color::medium_aquamarine,
           fmt::color::medium_blue,
           fmt::color::medium_orchid,
           fmt::color::medium_purple,
           fmt::color::medium_sea_green,
           fmt::color::medium_slate_blue,
           fmt::color::medium_spring_green,
           fmt::color::medium_turquoise,
           fmt::color::medium_violet_red,
           fmt::color::midnight_blue,
           fmt::color::mint_cream,
           fmt::color::misty_rose,
           fmt::color::moccasin,
           fmt::color::navajo_white,
           fmt::color::navy,
           fmt::color::old_lace,
           fmt::color::olive,
           fmt::color::olive_drab,
           fmt::color::orange,
           fmt::color::orange_red,
           fmt::color::orchid,
           fmt::color::pale_golden_rod,
           fmt::color::pale_green,
           fmt::color::pale_turquoise,
           fmt::color::pale_violet_red,
           fmt::color::papaya_whip,
           fmt::color::peach_puff,
           fmt::color::peru,
           fmt::color::pink,
           fmt::color::plum,
           fmt::color::powder_blue,
           fmt::color::purple,
           fmt::color::rebecca_purple,
           fmt::color::rosy_brown,
           fmt::color::royal_blue,
           fmt::color::saddle_brown,
           fmt::color::salmon,
           fmt::color::sandy_brown,
           fmt::color::sea_green,
           fmt::color::sea_shell,
           fmt::color::sienna,
           fmt::color::silver,
           fmt::color::sky_blue,
           fmt::color::slate_blue,
           fmt::color::slate_gray,
           fmt::color::snow,
           fmt::color::spring_green,
           fmt::color::steel_blue,
           fmt::color::tan,
           fmt::color::teal,
           fmt::color::thistle,
           fmt::color::tomato,
           fmt::color::turquoise,
           fmt::color::violet,
           fmt::color::wheat,
           fmt::color::white_smoke,
           fmt::color::yellow_green};
#endif

    class Reporter final {
     public:
      explicit Reporter(bool report = true);

      Reporter(Reporter const&) = delete;
      Reporter(Reporter&&)      = delete;
      Reporter& operator=(Reporter const&) = delete;
      Reporter& operator=(Reporter&&) = delete;

      template <typename TClass>
      Reporter& prefix(TClass const* const ptr, bool overide = false) {
        if (_report || overide) {
          std::lock_guard<std::mutex> lg(_mtx);
          uint64_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
          resize(tid + 1);
          _options[tid].prefix
#ifdef LIBSEMIGROUPS_FMT_ENABLED
              = fmt::sprintf("#%llu: %s: ", tid, string_class_name(ptr));
#else
              = string_format(
                  "#%llu: %s: ", tid, string_class_name(ptr).c_str());
#endif
        }
        return *this;
      }

#ifdef LIBSEMIGROUPS_FMT_ENABLED
      Reporter& color(fmt::color);
      Reporter& thread_color();
#endif

      Reporter& prefix();

      std::string get_prefix() const noexcept {
        uint64_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
        return _options[tid].prefix;
      }

      Reporter& flush_right();

      void flush();

      void report(bool val) {
        _report = val;
      }

      bool report() const noexcept {
        return _report;
      }

      template <typename... TArgs>
      Reporter& operator()(TArgs... args) {
        if (_report) {
          std::lock_guard<std::mutex> lg(_mtx);
          size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
          if (tid >= _last_msg.size()) {
            resize(tid + 1);
          }
          _last_msg[tid] = _msg[tid];

#ifdef LIBSEMIGROUPS_FMT_ENABLED
          _msg[tid] = fmt::sprintf(args...);
          color(thread_colors[tid % thread_colors.size()]);
#else
          _msg[tid] = string_format(args...);
#endif
        }
        return *this;
      }

     private:
      void resize(size_t);

      struct Options {
        Options()
            :
#ifdef LIBSEMIGROUPS_FMT_ENABLED
              color(fmt::color::alice_blue),
#endif
              flush_right(false),
              prefix() {
        }
#ifdef LIBSEMIGROUPS_FMT_ENABLED
        fmt::color color;
#endif
        bool        flush_right;
        std::string prefix;
      };
      std::vector<std::string> _last_msg;
      std::mutex               _mtx;
      std::vector<std::string> _msg;
      std::vector<Options>     _options;
      std::atomic<bool>        _report;
    };
  }  // namespace detail

  extern detail::Reporter REPORTER;

  namespace detail {
    class PrintTable {
     public:
      explicit PrintTable(size_t width = 72)
          : _rows(), _header(), _footer(), _width(width) {}

      std::string emit() {
        std::string result = lineohash() + _header + lineohash();
        for (auto const& row : _rows) {
          size_t n = 0;
          // Check if we contain a \infty
          if (!row.second.empty()) {
            if (row.second.find("\u221E") == std::string::npos) {
              n = _width - row.first.size() - row.second.size();
            } else {
              n = _width - row.first.size() - row.second.size() + 2;
            }
          }
          result += row.first + std::string(n, ' ') + row.second + "\n";
        }
        result += lineohash() + _footer + lineohash();
        return result;
      }

      void header(char const* text) {
        _header = wrap(text);
        _header += "\n";
      }

      void footer(char const* text) {
        _footer = wrap(text);
        _footer += "\n";
      }

      void divider() {
        _rows.emplace_back(lineohash(false), "");
      }

      void operator()(char const* col0) {
        _rows.emplace_back(wrap(col0), std::string());
      }

      template <typename T>
      void operator()(char const* col0, T const& col1) {
        _rows.emplace_back(col0, detail::to_string(col1));
      }

      template <typename T>
      void operator()(char const* col0,
                      char const* col1_format,
                      T const&    col1) {
        _rows.emplace_back(col0, string_format(col1_format, col1));
      }

     private:
      std::string wrap(char const* text) {
        return TextFlow::Column(text).width(_width).toString();
      }

      std::string lineohash(bool newline = true) {
        return std::string(_width, '#') + (newline ? "\n" : "");
      }

      std::vector<std::pair<std::string, std::string>> _rows;
      std::string                                      _header;
      std::string                                      _footer;
      size_t                                           _width;
    };
  }  // namespace detail

  namespace report {
    bool should_report() noexcept;
  }

  //! This struct can be used to enable printing of some information during
  //! various of the computation in ``libsemigroups``. Reporting is enable (or
  //! not) at construction time, and disable when the ReportGuard goes out of
  //! scope.
  struct ReportGuard {
    //! Constructs a ReportGuard with reporting enabled by default.
    //!
    //! \param val whether to report or not (default: \c true).
    explicit ReportGuard(bool val = true) {
      REPORTER.report(val);
    }

    ~ReportGuard() {
      REPORTER.report(false);
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_REPORT_HPP_
