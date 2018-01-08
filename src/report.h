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

#ifndef LIBSEMIGROUPS_SRC_REPORT_H_
#define LIBSEMIGROUPS_SRC_REPORT_H_

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

#define REPORT(message)                                                 \
  if (glob_reporter.get_report()) {                                     \
    size_t __tid = glob_reporter.thread_id(std::this_thread::get_id()); \
    glob_reporter.lock();                                               \
    glob_reporter(this, __func__, __tid) << message << std::endl;       \
    glob_reporter.unlock();                                             \
  }

#define REPORT_FROM_FUNC(message)                                       \
  if (glob_reporter.get_report()) {                                     \
    size_t __tid = glob_reporter.thread_id(std::this_thread::get_id()); \
    glob_reporter.lock();                                               \
    glob_reporter(__func__, __tid) << message << std::endl;             \
    glob_reporter.unlock();                                             \
  }

namespace libsemigroups {

  //
  // This is a simple class which can be used to print information to
  // the standard output, reporting the class and function name (if set), and
  // thread_id.
  //
  // It can be used like std::cout and if its call operator is used, like:
  //
  //    reporter("bananas", 2) << "the dvd player is broken";
  //    reporter("apples") << "the window is open";
  //
  // Then it will put the following to std::cout, if the class name is not set
  // when the <Reporter> is constructed or by <Reporter::set_class_name>:
  //
  //    Thread #2: "bananas": the dvd player is broken
  //    Thread #0: "apples": the window is open
  //
  // (where 0 is the default value of for _thread_id) or if the class name has
  // been set:
  //
  //    Thread #2: class_name::bananas: the dvd player is broken
  //    Thread #0: class_name::"apples": the window is open
  //
  // If the call operator has not been called before, then the prefix is not
  // printed (and consequently the value of the thread_id is not set).

  class Reporter {
   public:
    // 0 parameters
    //
    // The default constructor. Note that by default this will output nothing,
    // see <Reporter::set_report> and <Reporter::set_class_name>.
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

    // non-const, template
    // @rep a <Reporter> instance
    // @tt  something
    //
    // If <rep> is set to report (see <Reporter::set_report>), then << puts tt
    // to std::cout, otherwise this does nothing.
    //
    // If << is used immediately after the call operator of <rep>, then a prefix
    // will be put to std::cout, see the top of this section.
    //
    // If this is used in multiple threads for the same instance of a <Reporter>
    // then you should probably lock the reporter first, see <Reporter::lock()>.
    // (This is not done within the method for << since if, for example, we did:
    //
    //    rep << "aaa" << "bbb";
    //
    // and
    //
    //    rep << "ccc" << "ddd";
    //
    // in separate threads then we might see:
    //
    //    aaacccbbbddd
    //
    // when we wanted to see aaabbbcccddd.
    template <class T> friend Reporter& operator<<(Reporter& rep, const T& tt) {
      *(rep._ostream) << tt;
      return rep;
    }

    // non-const, template
    //
    // This method exists to allow std::endl to be put to a <Reporter>.
    Reporter& operator<<(std::ostream& (*function)(std::ostream&) ) {
      *_ostream << "\033[0m" << function;
      return *this;
    }

    // non-const
    // @func the function name part of the prefix put to std::cout
    // @thread_id the number put to std::cout to identify the thread which is
    //            printing
    template <class T>
    Reporter& operator()(T const* obj, char const* func, size_t tid) {
      *_ostream << get_color_prefix(tid) << "Thread #" << tid << ": "
                << get_class_name(obj) << "::" << func << ": ";
      return *this;
    }

    Reporter& operator()(char const* func, size_t tid) {
      *_ostream << get_color_prefix(tid) << "Thread #" << tid << ": " << func
                << ": ";
      return *this;
    }

    // non-const
    //
    // This method locks the reporter so that if it is called by multiple
    // threads
    // it does not give garbled output.
    void lock() {
      _mtx.lock();
    }

    // non-const
    //
    // This method unlocks the reporter so that if it is called by multiple
    // threads
    // another thread will be free to put to std::cout.
    void unlock() {
      _mtx.unlock();
    }

    // non-const
    // @val if false, then nothing is put to std::out.
    //
    // This method set whether anything should be output, this is atomic.
    void set_report(bool val) {
      _report = val;
    }

    // const
    //
    // This method returns true or false depending on whether or not we are
    // reporting.
    bool get_report() const {
      return _report;
    }

    // non-const, template
    // @obj the value to set the class name to.
    //
    // This method can be used to set the class name used in the output. This
    // can be used for example when a reporter class is static, and so there
    // may be no instance of the class to use as a parameter for the
    // constructor. It only prints the last part of the name, i.e. the part
    // after the last ::.

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
    template <class T> std::string get_class_name(T const* obj) {
      int   status;
      char* ptr = abi::__cxa_demangle(typeid(*obj).name(), 0, 0, &status);
      if (status == 0) {  // successfully demangled
        std::string full = std::string(ptr);
        // find the last :: in the class name <full>
        size_t prev = 0, pos = 0;
        while (pos != std::string::npos) {
          prev = pos;
          pos  = full.find("::", pos + 1);
        }
        if (prev != 0) {
          return full.substr(prev + 2, std::string::npos);
        } else {
          return full;
        }
      }
      free(ptr);
      return std::string();
    }

    std::string get_color_prefix(size_t tid) {
      return _color_prefix[tid % _color_prefix.size()];
    }

    std::vector<std::string> _color_prefix;
    std::unordered_map<std::thread::id, size_t> _map;
    std::mutex        _mtx;
    size_t            _next_tid;
    std::ostream*     _ostream;  // For testing
    std::atomic<bool> _report;
  };

  extern Reporter glob_reporter;
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_REPORT_H_
