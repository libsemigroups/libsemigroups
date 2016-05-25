/*
 * Semigroups GAP package
 *
 * This file contains a simple timer class.
 *
 */

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

#include <assert.h>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

class Timer {

  typedef duration<long long int, std::nano>  nano_t;
  typedef steady_clock::time_point  time_point_t;

 public:

    Timer () : _start(), _running(false) {}

    void start () {
      if (!_running) {
        _running = true;
        _start = steady_clock::now();
      } else {
        assert(false);
      }
    }

    void stop (bool report = true) {
      if (_running) {
        time_point_t end     = steady_clock::now();
        nano_t       elapsed = duration_cast<nano_t>(end - _start);
        _running = false;
        if (report) {
          std::cout << "elapsed time = ";
          if (print_it<hours>(elapsed, "h ", 0)) {
            print_it<minutes>(elapsed, "m", 0);
            std::cout << std::endl;
            return;
          } else if (print_it<minutes>(elapsed, "m ", 0)) {
            print_it<seconds>(elapsed, "s", 0);
            std::cout << std::endl;
            return;
          /*} else if (print_it<seconds>(elapsed, "s ", 0)) {
            print_it<milliseconds>(elapsed, "ms", 0);
            std::cout << std::endl;
            return;*/
          } else if (print_it<milliseconds>(elapsed, "ms ", 9)) {
            std::cout << std::endl;
            return;
          } else if (print_it<microseconds>(elapsed, "us ", 9)) {
            std::cout << std::endl;
            return;
          } else if (print_it<nanoseconds>(elapsed, "ns ", 0)) {
            std::cout << std::endl;
            return;
          }
        }
      }
    }

 private:
   steady_clock::time_point _start;
   bool                     _running;

   template<typename T>
     bool print_it (nano_t& elapsed, std::string str, size_t threshold) {
       T x = duration_cast<T>(elapsed);
       if (x > T(threshold)) {
         std::cout << x.count() << str;
         elapsed -= x;
         return true;
       }
       return false;
     }
};

#endif //SRC_TIMER_H_
