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

#ifndef LIBSEMIGROUPS_INCLUDE_INTERNAL_REPORT_H_
#define LIBSEMIGROUPS_INCLUDE_INTERNAL_REPORT_H_

#include <cxxabi.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "libsemigroups-debug.h"
#include "timer.h"

#define REPORT(message)                                            \
  if (REPORTER.get_report()) {                                     \
    size_t __tid = REPORTER.thread_id(std::this_thread::get_id()); \
    std::lock_guard<std::mutex> __lg(REPORTER.mutex());            \
    REPORTER.report_from(__tid, this) << message << std::endl;     \
  }

#define REPORT_FROM_FUNC(message)                                  \
  if (REPORTER.get_report()) {                                     \
    size_t __tid = REPORTER.thread_id(std::this_thread::get_id()); \
    std::lock_guard<std::mutex> __lg(REPORTER.mutex());            \
    REPORTER(__func__, __tid) << message << std::endl;             \
  }

namespace libsemigroups {
  class Reporter {
   public:
    Reporter()
        : _color_prefix({"",
                         "\033[40;38;5;82m",
                         "\033[33m",
                         "\033[40;38;5;208m",
                         "\033[38;5;27m"}),
          _map(),
          _next_tid(0),
          _ostream(&std::cout),
          _report(false) {
      // Get thread id 0 for the main thread
      thread_id(std::this_thread::get_id());
    }

    ~Reporter() {}

    template <class T> friend Reporter& operator<<(Reporter& rep, const T& tt) {
      *(rep._ostream) << tt;
      return rep;
    }

    Reporter& operator<<(std::ostream& (*function)(std::ostream&) ) {
      *_ostream << "\033[0m" << function;
      return *this;
    }

    template <class T> Reporter& report_from(size_t tid, T const* ptr) {
      *_ostream << get_color_prefix(tid) << "#" << tid << ": "
                << get_class_name(ptr);
      *_ostream << ": ";
      return *this;
    }

    Reporter& operator()(char const* func, size_t tid) {
      *_ostream << get_color_prefix(tid) << "#" << tid << ": " << func << ": ";
      return *this;
    }

    std::mutex& mutex() {
      return _mtx;
    }

    void set_report(bool val) {
      _report = val;
    }

    bool get_report() const {
      return _report;
    }

    void set_ostream(std::ostream* os) {
      _ostream = os;
    }

    void reset_thread_ids() {
      // Only do this from the main thread
      LIBSEMIGROUPS_ASSERT(thread_id(std::this_thread::get_id()) == 0);
      // Delete all thread_ids
      _map.clear();
      _next_tid = 0;
      // Reinsert the main thread's id
      thread_id(std::this_thread::get_id());
    }

    // Caution should only use this method when the reporter is locked!
    size_t thread_id(std::thread::id tid) {
      std::lock_guard<std::mutex> lg(_mtx);
      auto                        it = _map.find(tid);
      if (it != _map.end()) {
        return (*it).second;
      } else {
        // Don't check the assert below because on a single thread machine
        // (such as those used by appveyor), for an fp-semigroup more than 1
        // thread will be used, and this assertion will fail.
        // LIBSEMIGROUPS_ASSERT(_next_tid <=
        // std::thread::hardware_concurrency());
        _map.emplace(tid, _next_tid++);
        return _next_tid - 1;
      }
    }

   private:
    template <class T> std::string get_class_name(T const* o) {
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
        return full.substr(first, last - first);
      }
      free(ptr);
      return std::string();
    }

    std::string get_color_prefix(size_t tid) {
      return _color_prefix[tid % _color_prefix.size()];
    }

    std::vector<std::string>                    _color_prefix;
    std::unordered_map<std::thread::id, size_t> _map;
    std::mutex                                  _mtx;
    size_t                                      _next_tid;
    std::ostream*                               _ostream;  // For testing
    std::atomic<bool>                           _report;
  };

  extern Reporter REPORTER;
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_INTERNAL_REPORT_H_
