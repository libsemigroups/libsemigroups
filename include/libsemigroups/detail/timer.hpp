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

#ifndef LIBSEMIGROUPS_DETAIL_TIMER_HPP_
#define LIBSEMIGROUPS_DETAIL_TIMER_HPP_

#include <chrono>  // for nanoseconds, duration_cast, minutes, operator>
#include <iosfwd>  // for ostream
#include <string>  // for string

namespace libsemigroups {
  namespace detail {

    // String containing the somewhat human readable amount of time, this is
    // primarily intended for testing purposes
    std::string string_time(std::chrono::nanoseconds elapsed);

    template <typename Unit>
    std::string string_time(Unit elapsed) {
      using std::chrono::duration_cast;
      using std::chrono::nanoseconds;
      return string_time(duration_cast<nanoseconds>(elapsed));
    }

    // This is a simple class which can be used to send timing information in a
    // somewhat human readable format to the standard output.
    class Timer {
     public:
      // Default constructor, timer starts when object is created
      Timer() : _start(std::chrono::high_resolution_clock::now()) {}

      Timer(Timer const&)            = default;
      Timer(Timer&&)                 = default;
      Timer& operator=(Timer const&) = default;
      Timer& operator=(Timer&&)      = default;
      ~Timer()                       = default;

      // Reset the timer (i.e. time from this point on)
      void reset() {
        _start = std::chrono::high_resolution_clock::now();
      }

      // The elapsed time in nanoseconds since last reset
      std::chrono::nanoseconds elapsed() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now() - _start);
      }

      // String containing the somewhat human readable amount of time since the
      // last reset
      std::string string() const {
        return string_time(elapsed());
      }

      // Left shift the string containing the somewhat human readable amount of
      // time since last reset to an ostream
      friend std::ostream& operator<<(std::ostream&        os,
                                      detail::Timer const& t) {
        os << t.string();
        return os;
      }

      template <typename T>
      bool operator>(T val) {
        return elapsed() > std::chrono::nanoseconds(val);
      }

     private:
      std::chrono::high_resolution_clock::time_point _start;
    };
  }  // namespace detail

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_TIMER_HPP_
