/*******************************************************************************

 Copyright (C) 2016 James D. Mitchell

 This work is licensed under a Creative Commons Attribution-ShareAlike 4.0
 International License. See
 http://creativecommons.org/licenses/by-sa/4.0/

 For a discussion about this code and latest version:

*******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include <assert.h>
#include <iostream>
#include <chrono>

class Timer {

  typedef std::chrono::duration<long long int, std::nano>  nano_t;
  typedef std::chrono::steady_clock::time_point            time_point_t;

 public:
    Timer () : _start(), _running(false) {}

    void start () {
      if (!_running) {
        _running = true;
        _start = std::chrono::steady_clock::now();
      } else {
        assert(false);
      }
    }

    void stop () {
      print();
      _running = false;
    }

    void print () {
      if (_running) {
        time_point_t end     = std::chrono::steady_clock::now();
        nano_t       elapsed = std::chrono::duration_cast<nano_t>(end - _start);
        std::cout << "elapsed time = ";
        if (print_it<std::chrono::hours>(elapsed, "h ", 0)) {
          print_it<std::chrono::minutes>(elapsed, "m", 0);
          std::cout << std::endl;
          return;
        } else if (print_it<std::chrono::minutes>(elapsed, "m ", 0)) {
          print_it<std::chrono::seconds>(elapsed, "s", 0);
          std::cout << std::endl;
          return;
        } else if (print_it<std::chrono::milliseconds>(elapsed, "ms ", 9)) {
          std::cout << std::endl;
          return;
        } else if (print_it<std::chrono::microseconds>(elapsed, "us ", 9)) {
          std::cout << std::endl;
          return;
        } else if (print_it<std::chrono::nanoseconds>(elapsed, "ns ", 0)) {
          std::cout << std::endl;
          return;
        }
      } else {
        assert(false);
      }
    }

 private:
    std::chrono::steady_clock::time_point _start;
    bool                                  _running;

    template<typename T>
      bool print_it (nano_t& elapsed, std::string str, size_t threshold) {
        T x = std::chrono::duration_cast<T>(elapsed);
        if (x > T(threshold)) {
          std::cout << x.count() << str;
          elapsed -= x;
          return true;
        }
        return false;
      }
};

#endif // TIMER_H_
