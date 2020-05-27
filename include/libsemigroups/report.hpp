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

#include <array>          // for array
#include <atomic>         // for atomic
#include <cstddef>        // for size_t
#include <iostream>       // for string, operator<<, cout, ost...
#include <mutex>          // for mutex, lock_guard
#include <thread>         // for get_id, thread, thread::id
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT
#include "string.hpp"               // for wrap

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Winline"
#include "fmt/color.h"
#include "fmt/printf.h"
#pragma GCC diagnostic pop

// To avoid computing the parameters __VA_ARGS__ when we aren't even
// reporting, we check if we are reporting before calling REPORTER.
#define REPORT(...) \
  (REPORTER.report() ? REPORTER(__VA_ARGS__).prefix(this) : REPORTER)

#define REPORT_DEFAULT(...) REPORT(__VA_ARGS__).thread_color().flush();

#ifdef LIBSEMIGROUPS_DEBUG
#define REPORT_DEBUG(...) REPORT(__VA_ARGS__).color(fmt::color::dim_gray)
#define REPORT_DEBUG_DEFAULT(...) REPORT_DEBUG(__VA_ARGS__).flush();
#else
#define REPORT_DEBUG(...)
#define REPORT_DEBUG_DEFAULT(...)
#endif

#ifdef LIBSEMIGROUPS_VERBOSE
#define REPORT_VERBOSE(...) REPORT(__VA_ARGS__).color(fmt::color::cyan)
#define REPORT_VERBOSE_DEFAULT(...) REPORT_VERBOSE(__VA_ARGS__).flush();
#else
#define REPORT_VERBOSE(...)
#define REPORT_VERBOSE_DEFAULT(...)
#endif

#define REPORT_TIME(var) \
  REPORT_DEFAULT("elapsed time (%s): %s\n", __func__, var.string());

namespace libsemigroups {

  namespace detail {

    class ThreadIdManager final {
     public:
      ThreadIdManager() : _mtx(), _next_tid(0), _thread_map() {
        tid(std::this_thread::get_id());
      }

      ThreadIdManager(ThreadIdManager const&) = delete;
      ThreadIdManager(ThreadIdManager&&)      = delete;
      ThreadIdManager& operator=(ThreadIdManager const&) = delete;
      ThreadIdManager& operator=(ThreadIdManager&&) = delete;

      void reset() {
        // Only do this from the main thread
        LIBSEMIGROUPS_ASSERT(tid(std::this_thread::get_id()) == 0);
        // Delete all thread_ids
        _thread_map.clear();
        _next_tid = 0;
        // Reinsert the main thread's id
        tid(std::this_thread::get_id());
      }

      size_t tid(std::thread::id t) {
        std::lock_guard<std::mutex> lg(_mtx);
        auto                        it = _thread_map.find(t);
        if (it != _thread_map.end()) {
          return (*it).second;
        } else {
          // Don't check the assert below because on a single thread machine
          // (such as those used by appveyor), for an fp-semigroup more than 1
          // thread will be used, and this assertion will fail.
          // LIBSEMIGROUPS_ASSERT(_next_tid <=
          // std::thread::hardware_concurrency());
          _thread_map.emplace(t, _next_tid++);
          return _next_tid - 1;
        }
      }

     private:
      std::mutex                                  _mtx;
      size_t                                      _next_tid;
      std::unordered_map<std::thread::id, size_t> _thread_map;
    };
  }  // namespace detail

  extern detail::ThreadIdManager THREAD_ID_MANAGER;

  namespace detail {
    class ClassNamer final {
      ClassNamer()                  = delete;
      ClassNamer(ClassNamer const&) = delete;
      ClassNamer(ClassNamer&&)      = delete;
      ClassNamer& operator=(ClassNamer const&) = delete;
      ClassNamer& operator=(ClassNamer&&) = delete;

     public:
      template <typename T>
      static std::string get(T const* o) {
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
    };

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

    class Reporter final {
     public:
      explicit Reporter(bool report = true)
          : _last_msg(), _mtx(), _msg(), _options(), _report(report) {}

      Reporter(Reporter const&) = delete;
      Reporter(Reporter&&)      = delete;
      Reporter& operator=(Reporter const&) = delete;
      Reporter& operator=(Reporter&&) = delete;

      template <typename TClass>
      Reporter& prefix(TClass const* const ptr) {
        if (_report) {
          std::lock_guard<std::mutex> lg(_mtx);
          size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
          resize(tid + 1);
          _options[tid].prefix
              = fmt::sprintf("#%llu: %s: ", tid, ClassNamer::get(ptr));
        }
        return *this;
      }

      Reporter& prefix() {
        if (_report) {
          std::lock_guard<std::mutex> lg(_mtx);
          size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
          resize(tid + 1);
          _options[tid].prefix = "";
        }
        return *this;
      }

      Reporter& color(fmt::color c) {
        if (_report) {
          size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
          resize(tid + 1);
          _options[tid].color = c;
        }
        return *this;
      }

      Reporter& thread_color() {
        if (_report) {
          std::lock_guard<std::mutex> lg(_mtx);
          size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
          resize(tid + 1);
          _options[tid].color = thread_colors[tid % thread_colors.size()];
        }
        return *this;
      }

      Reporter& flush_right() {
        if (_report) {
          std::lock_guard<std::mutex> lg(_mtx);
          size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
          resize(tid + 1);
          _options[tid].flush_right = true;
        }
        return *this;
      }

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
          _msg[tid]      = fmt::sprintf(args...);
          color(thread_colors[tid % thread_colors.size()]);
        }
        return *this;
      }

      void flush() {
        if (_report) {
          std::lock_guard<std::mutex> lg(_mtx);
          size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
          size_t pad = 0;
          _msg[tid]  = _options[tid].prefix + _msg[tid];
          if (_options[tid].flush_right
              && _last_msg[tid].size() + unicode_string_length(_msg[tid])
                     < 80) {
            pad = (80 - _last_msg[tid].size())
                  - unicode_string_length(_msg[tid]);
            _msg[tid] = std::string(pad, ' ') + _msg[tid];
          }
#ifdef LIBSEMIGROUPS_VERBOSE
          if (_msg[tid].back() != '\n') {
            _msg[tid] += "\n";
          }
#endif
          _msg[tid] = wrap(_options[tid].prefix.length(), _msg[tid]);
          fmt::print(fg(_options[tid].color), _msg[tid]);
          _options[tid] = Options();
        }
      }

     private:
      void resize(size_t n) {
        if (n > _msg.size()) {
          _last_msg.resize(n);
          _msg.resize(n);
          _options.resize(n);
        }
      }

      struct Options {
        Options()
            : color(fmt::color::alice_blue), flush_right(false), prefix() {}
        fmt::color  color;
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

  //! This struct can be used to enable printing of some information during
  //! various of the computation in libsemigroups. Reporting is enable (or not)
  //! at construction time, and disable when the ReportGuard goes out of scope.
  struct ReportGuard {
    //! Constructs a ReportGuard with reporting enabled by default.
    //!
    //! \param val whether to report or not (default: \c true).
    explicit ReportGuard(bool val = true) {
      REPORTER.report(val);
    }

    ~ReportGuard() {
      REPORTER.report(false);
      // REPORTER.set_ostream(std::cout);
    }
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_REPORT_HPP_
