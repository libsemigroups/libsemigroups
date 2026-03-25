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

#ifndef LIBSEMIGROUPS_SRC_TIMER_H_
#define LIBSEMIGROUPS_SRC_TIMER_H_

#include <chrono>
#include <iostream>
#include <string>

// #include "to_string.h"

namespace libsemigroups {

// This is a simple class which can be used to send timing information in a
// somewhat human readable format to the standard output.
class Timer {
  public:
    // Default constructor, timer starts when object is created
    Timer() : _start(std::chrono::high_resolution_clock::now()) {}

    // Reset the timer (i.e. time from this point on)
    void reset() { _start = std::chrono::high_resolution_clock::now(); }

    // The elapsed time in nanoseconds since last reset
    std::chrono::nanoseconds elapsed() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - _start);
    }

    // String containing the somewhat human readable amount of time, this is
    // primarily intended for testing purposes
    std::string string(std::chrono::nanoseconds elapsed) const {
        std::string out;
        if (string_it<std::chrono::hours>(out, elapsed, "h ", 0)) {
            string_it<std::chrono::minutes>(out, elapsed, "m", 0);
            return out;
        } else if (string_it<std::chrono::minutes>(out, elapsed, "m ", 0)) {
            string_it<std::chrono::seconds>(out, elapsed, "s", 0);
            return out;
        } else if (string_it<std::chrono::milliseconds>(out, elapsed, "ms",
                                                        9)) {
            return out;
        } else if (string_it<std::chrono::microseconds>(out, elapsed, "\u03BCs",
                                                        9)) {
            return out;
        } else if (string_it<std::chrono::nanoseconds>(out, elapsed, "ns", 0)) {
            return out;
        }
        return out;
    }

    // String containing the somewhat human readable amount of time since the
    // last reset
    std::string string() const { return string(elapsed()); }

    // Left shift the string containing the somewhat human readable amount of
    // time since last reset to an ostream
    friend std::ostream &operator<<(std::ostream &os, Timer const &t) {
        os << t.string();
        return os;
    }

  private:
    std::chrono::high_resolution_clock::time_point _start;

    template <typename T>
    bool string_it(std::string &str, std::chrono::nanoseconds &elapsed,
                   std::string unit, size_t threshold) const {
        T x = std::chrono::duration_cast<T>(elapsed);
        if (x > T(threshold)) {
            str += std::to_string(x.count()) + unit;
            elapsed -= x;
            return true;
        }
        return false;
    }
};
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SRC_TIMER_H_
