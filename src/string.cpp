// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

#include "libsemigroups/string.hpp"

#include <random>  // for mt19937

#include "libsemigroups/config.hpp"     // for LIBSEMIGROUPS_FMT_ENABLED
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#ifdef LIBSEMIGROUPS_FMT_ENABLED
#include "fmt/format.h"  // for group_digits
#endif

namespace libsemigroups {
  namespace detail {
    std::string random_string(std::string const& alphabet, size_t length) {
      static std::random_device       rd;
      static std::mt19937             generator(rd());
      std::uniform_int_distribution<> distribution(0, alphabet.size() - 1);

      std::string result;

      for (size_t i = 0; i < length; ++i) {
        result += alphabet[distribution(generator)];
      }

      return result;
    }

    // Random string with random length in the range [min, max) over <alphabet>
    std::string random_string(std::string const& alphabet,
                              size_t             min,
                              size_t             max) {
      if (min >= max) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 2nd argument (min) must be less than the 3rd argument (max)");
      } else if (alphabet.empty() && min != 0) {
        LIBSEMIGROUPS_EXCEPTION("expected non-empty 1st argument (alphabet)");
      }
      if (max == min + 1) {
        return random_string(alphabet, min);
      }
      static std::random_device       rd;
      static std::mt19937             generator(rd());
      std::uniform_int_distribution<> distribution(min, max - 1);
      return random_string(alphabet, distribution(generator));
    }

    std::vector<std::string> random_strings(std::string const& alphabet,
                                            size_t             number,
                                            size_t             min,
                                            size_t             max) {
      std::vector<std::string> result;
      for (size_t j = 0; j < number; ++j) {
        result.push_back(random_string(alphabet, min, max));
      }
      return result;
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

#ifdef LIBSEMIGROUPS_FMT_ENABLED
    std::string group_digits(size_t num) {
      return fmt::to_string(fmt::group_digits(num));
    }
#else
    std::string group_digits(size_t num) {
      std::string result = detail::to_string(num);
      if (result.size() < 4) {
        return result;
      }
      for (size_t i = 3; i < result.size(); i += 4) {
        result.insert(result.size() - i, ",");
      }
      return result;
    }
#endif
  }  // namespace detail
}  // namespace libsemigroups
