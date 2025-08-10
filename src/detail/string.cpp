// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2025 James D. Mitchell + Maria Tsalakou
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

#include "libsemigroups/detail/string.hpp"

#include <random>  // for mt19937

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#include "libsemigroups/detail/fmt.hpp"  // for group_digits

namespace libsemigroups {
  namespace detail {

    void throw_if_nullptr(char const* w, std::string_view arg) {
      if (w == nullptr) {
        LIBSEMIGROUPS_EXCEPTION(
            "the {} argument (char const*) must not be nullptr", arg);
      }
    }

    // Returns the string s to the power N, not optimized, complexity is O(N *
    // |s|)
    std::string power_string(std::string const& s, size_t N) {
      std::string result = s;
      for (size_t i = 0; i < N; ++i) {
        result += s;
      }
      return result;
    }

    namespace {
      std::string group_digits(uint64_t num) {
        // Although we ignore -Winline in detail/fmt.hpp, there is still an
        // warning issued here (with g++-14), which we suppress again.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
#endif
        return fmt::to_string(fmt::group_digits(num));
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
      }
    }  // namespace

    std::string group_digits(int64_t num) {
      if (num < 0) {
        return "-" + group_digits(static_cast<uint64_t>(-num));
      }
      return group_digits(static_cast<uint64_t>(num));
    }

    std::string signed_group_digits(int64_t num) {
      if (num < 0) {
        return "-" + group_digits(static_cast<uint64_t>(-num));
      }
      return "+" + group_digits(static_cast<uint64_t>(num));
    }

    size_t visible_length(std::string_view s) {
      size_t count = 0;

      for (std::size_t i = 0; i < s.size();) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c == 0x1B) {  // ESC
          // Skip the ESC
          ++i;
          if (i < s.size() && s[i] == '[') {
            // CSI sequence: skip until a letter in @A-Za-z~
            ++i;
            while (i < s.size()) {
              unsigned char d = static_cast<unsigned char>(s[i]);
              if (d >= '@' && d <= '~') {  // final byte
                ++i;
                break;
              }
              ++i;
            }
          } else {
            // Some other escape sequence: skip next char
            if (i < s.size())
              ++i;
          }
        } else {
          // visible character
          ++count;
          ++i;
        }
      }
      return count;
    }

    std::string subscript(size_t val) {
      static const char* subscripts[] = {
          u8"₀", u8"₁", u8"₂", u8"₃", u8"₄", u8"₅", u8"₆", u8"₇", u8"₈", u8"₉"};

      if (val == 0) {
        return subscripts[0];
      }
      std::string result;
      while (val > 0) {
        result.insert(0, subscripts[val % 10]);
        val /= 10;
      }
      return result;
    }

  }  // namespace detail
}  // namespace libsemigroups
