//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_INCLUDE_INTERNAL_REPORT_HPP_
#define LIBSEMIGROUPS_INCLUDE_INTERNAL_REPORT_HPP_

#include <cxxabi.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "libsemigroups-debug.hpp"
#include "libsemigroups-exception.hpp"

#define REPORT(...) REPORTER(this, __VA_ARGS__)

/*#define REPORT_FROM_FUNC(message)                                  \
  if (REPORTER.get_report()) {                                     \
    size_t __tid = REPORTER.thread_id(std::this_thread::get_id()); \
    std::lock_guard<std::mutex> __lg(REPORTER.mutex());            \
    REPORTER(__func__, __tid) << message << '\n';                  \
  }*/

namespace libsemigroups {
  // The following is based on Catch v2.1.0
  class ColourGuard {
   public:
    enum class Code {
      None = 0,

      White,
      Red,
      Green,
      Blue,
      Cyan,
      Yellow,
      Grey,

      Bright = 0x10,

      BrightRed   = Bright | Red,
      BrightGreen = Bright | Green,
      LightGrey   = Bright | Grey,
      BrightWhite = Bright | White,
    };

    // Use constructed object for RAII guard
    explicit ColourGuard(Code colourCode) {
      use(colourCode);
    }

    explicit ColourGuard(size_t tid) {
      static const std::vector<Code> colours = {Code::White,
                                                Code::Green,
                                                Code::Yellow,
                                                Code::Blue,
                                                Code::Red,
                                                Code::Cyan,
                                                Code::BrightWhite,
                                                Code::Grey,
                                                Code::BrightGreen,
                                                Code::BrightRed,
                                                Code::LightGrey};
      use(colours[tid % colours.size()]);
    }

    ColourGuard(ColourGuard&& other) noexcept {
      _moved       = other._moved;
      other._moved = true;
    }

    ColourGuard& operator=(ColourGuard&& other) noexcept {
      _moved       = other._moved;
      other._moved = true;
      return *this;
    }

    ~ColourGuard() {
      if (!_moved) {
        use(Code::None);
      }
    }

    friend std::ostream& operator<<(std::ostream& os, ColourGuard const&) {
      return os;
    }

   private:
    void use(Code colourCode) {
      switch (colourCode) {
        case Code::None:
          // intentional fall through
        case Code::White:
          return set_colour("[0m");
        case Code::Red:
          return set_colour("[0;31m");
        case Code::Green:
          return set_colour("[0;32m");
        case Code::Blue:
          return set_colour("[0;34m");
        case Code::Cyan:
          return set_colour("[0;36m");
        case Code::Yellow:
          return set_colour("[0;33m");
        case Code::Grey:
          return set_colour("[1;30m");

        case Code::LightGrey:
          return set_colour("[0;37m");
        case Code::BrightRed:
          return set_colour("[1;31m");
        case Code::BrightGreen:
          return set_colour("[1;32m");
        case Code::BrightWhite:
          return set_colour("[1;37m");

        case Code::Bright:
          LIBSEMIGROUPS_EXCEPTION("not a colour");
      }
    }

    void set_colour(const char* escapeCode) {
      std::cout << '\033' << escapeCode;
    }

    bool _moved = false;
  };

  class Reporter {
    using Code = ColourGuard::Code;

   public:
    Reporter() : _next_tid(0), _report(false), _thread_map() {
      // Get thread id 0 for the main thread
      thread_id(std::this_thread::get_id());
    }

    ~Reporter() {}

    template <typename TThisType, typename... TParams>
    void operator()(TThisType const* ptr, TParams... args) {
      if (_report) {
        size_t                      tid = thread_id(std::this_thread::get_id());
        std::lock_guard<std::mutex> lg(_mtx);
        ColourGuard                 cg(tid);
        std::cout << cg << "#" << tid << ": " << class_name(ptr) << ": ";
        int dummy[sizeof...(TParams)] = {(std::cout << args, 0)...};
        (void) dummy;
        std::cout << '\n';
      }
    }

    // Reporter& operator()(char const* func, size_t tid) {
    //  *_ostream << color_prefix(tid) << "#" << tid << ": " << func << ": ";
    //  return *this;
    //}

    void set_report(bool val) {
      _report = val;
    }

    void reset_thread_ids() {
      // Only do this from the main thread
      LIBSEMIGROUPS_ASSERT(thread_id(std::this_thread::get_id()) == 0);
      // Delete all thread_ids
      _thread_map.clear();
      _next_tid = 0;
      // Reinsert the main thread's id
      thread_id(std::this_thread::get_id());
    }

    size_t thread_id(std::thread::id tid) {
      std::lock_guard<std::mutex> lg(_mtx);
      auto                        it = _thread_map.find(tid);
      if (it != _thread_map.end()) {
        return (*it).second;
      } else {
        // Don't check the assert below because on a single thread machine
        // (such as those used by appveyor), for an fp-semigroup more than 1
        // thread will be used, and this assertion will fail.
        // LIBSEMIGROUPS_ASSERT(_next_tid <=
        // std::thread::hardware_concurrency());
        _thread_map.emplace(tid, _next_tid++);
        return _next_tid - 1;
      }
    }

   private:
    template <class T> std::string class_name(T const* o) {
      auto it = _class_name_map.find(typeid(*o).hash_code());
      if (it != _class_name_map.end()) {
        return (*it).second;
      }
      int   status;
      char* ptr = abi::__cxa_demangle(typeid(*o).name(), 0, 0, &status);
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
        free(ptr);
        std::string out = full.substr(first, last - first);
        _class_name_map.emplace(typeid(*o).hash_code(), out);
        return out;
      }
      free(ptr);
      std::string out;
      _class_name_map.emplace(typeid(*o).hash_code(), out);
      return out;
    }

    std::unordered_map<size_t, std::string>     _class_name_map;
    std::mutex                                  _mtx;
    size_t                                      _next_tid;
    std::atomic<bool>                           _report;
    std::unordered_map<std::thread::id, size_t> _thread_map;
  };

  extern Reporter REPORTER;
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_INTERNAL_REPORT_HPP_
