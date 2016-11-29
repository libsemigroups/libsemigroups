//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

#ifndef SEMIGROUPSPLUSPLUS_UTIL_REPORT_H_
#define SEMIGROUPSPLUSPLUS_UTIL_REPORT_H_

#define NDEBUG
#include <assert.h>
#include <cxxabi.h>

#include <atomic>
#include <iostream>
#include <mutex>
#include <string>

#include "timer.h"

namespace semigroupsplusplus {

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
        : _operator_called(false),
          _report(false),
          _ostream(&std::cout),
          _thread_id(0) {}

    // 1 parameter
    // @obj the object whose name will be used by the reporter.
    //
    // Note that by default this will output nothing, see
    // <Reporter::set_report>.
    template <class T>
    explicit Reporter(T const& obj)
        : _operator_called(false),
          _report(false),
          _ostream(&std::cout),
          _thread_id(0) {
      set_class_name(obj);
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
      rep.output_prefix();
      if (rep._report) {
        *(rep._ostream) << tt;
      }
      return rep;
    }

    // non-const, template
    //
    // This method exists to allow std::endl to be put to a <Reporter>.
    Reporter& operator<<(std::ostream& (*function)(std::ostream&) ) {
      this->output_prefix();
      if (_report) {
        *_ostream << function;
      }
      return *this;
    }

    // non-const
    // @func the function name part of the prefix put to std::cout
    // @thread_id the number put to std::cout to identify the thread which is
    //            printing
    Reporter& operator()(std::string func, size_t thread_id = 0) {
      _thread_id       = thread_id;
      _func            = func;
      _operator_called = true;
      return *this;
    }

    Reporter& operator()(size_t thread_id = 0) {
      _thread_id       = thread_id;
      _operator_called = true;
      return *this;
    }

    // non-const
    //
    // This method locks the reporter so that if it is called by multiple
    // threads
    // it does not give garbled output.
    void lock() {
      if (_report) {
        _mtx.lock();
      }
    }

    // non-const
    //
    // This method unlocks the reporter so that if it is called by multiple
    // threads
    // another thread will be free to put to std::cout.
    void unlock() {
      if (_report) {
        _mtx.unlock();
      }
    }

    // non-const
    // @val if false, then nothing is put to std::out.
    //
    // This method set whether anything should be output, this is atomic.
    void set_report(bool val) {
      _report = val;
    }

    // non-const, template
    // @obj the value to set the class name to.
    //
    // This method can be used to set the class name used in the output. This
    // can
    // be used for example when a reporter class is static, and so there may be
    // no instance of the class to use as a parameter for the constructor.
    template <class T> void set_class_name(T const& obj) {
      _class = abi::__cxa_demangle(typeid(obj).name(), 0, 0, 0);
    }

    // non-const
    //
    // This method can be used to start a timer. The reporter is locked when the
    // <Timer> is started but this is not thread safe, since if started in one
    // thread and stop in another the output is not meaningful.
    void start_timer() {
      if (_report) {
        _mtx.lock();
        _timer.start();
        _mtx.unlock();
      }
    }

    // non-const
    // @prefix a string to prefix the time (defaults to "elapsed time = ")
    //
    // This method can be used to stop a timer, and report the amount of time.
    // The reporter is locked when the <Timer> is stopped but this is not thread
    // safe, since if started in one thread and stop in another the output is
    // not
    // meaningful.
    void stop_timer(std::string prefix = "elapsed time = ") {
      if (_report && _timer.is_running()) {
        _mtx.lock();
        (*this)(_func, _thread_id) << prefix << _timer.string() << std::endl;
        _timer.stop();
        _mtx.unlock();
      }
    }

    void set_ostream(std::ostream* os) {
      _ostream = os;
    }

   private:
    void output_prefix() {
      if (_report && _operator_called) {
        *_ostream << "Thread #" << _thread_id << ": ";

        if (_class != "") {
          *_ostream << _class;
          if (_func != "") {
            *_ostream << "::";
          } else {
            *_ostream << ": ";
          }
        }
        if (_func != "") {
          *_ostream << _func << ": ";
        }
      }
      _operator_called = false;
    }

    std::string       _class;
    std::string       _func;
    std::mutex        _mtx;
    bool              _operator_called;
    std::atomic<bool> _report;
    std::ostream*     _ostream;
    size_t            _thread_id;
    Timer             _timer;
  };
}  // namespace semigroupsplusplus
#endif  // SEMIGROUPSPLUSPLUS_UTIL_REPORT_H_
