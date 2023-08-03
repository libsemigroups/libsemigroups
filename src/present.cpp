//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Murray T. Whyte
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

// This file contains

#include <cstddef>  // for size_t

#include "libsemigroups/present.hpp"

namespace libsemigroups {
  namespace presentation {
    namespace {
      std::string const& chars_in_human_readable_order() {
        // Choose visible characters a-zA-Z0-9 first before anything else
        // The ascii ranges for these characters are: [97, 123), [65, 91),
        // [48, 58) so the remaining range of chars that are appended to the end
        // after these chars are [0,48), [58, 65), [91, 97), [123, 255)
        static std::string letters
            = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        static bool first_call = true;
        if (first_call) {
          letters.resize(255);
          std::iota(letters.begin() + 62,
                    letters.begin() + 110,
                    static_cast<char>(0));
          std::iota(letters.begin() + 110,
                    letters.begin() + 117,
                    static_cast<char>(58));
          std::iota(letters.begin() + 117,
                    letters.begin() + 123,
                    static_cast<char>(91));
          std::iota(
              letters.begin() + 123, letters.end(), static_cast<char>(123));
          first_call = false;
          LIBSEMIGROUPS_ASSERT(letters.size()
                               == std::numeric_limits<char>::max()
                                      - std::numeric_limits<char>::min());
          LIBSEMIGROUPS_ASSERT(letters.end() == letters.begin() + 255);
        }
        return letters;
      }

    }  // namespace

    typename Presentation<std::string>::letter_type
    human_readable_letter(Presentation<std::string> const&, size_t i) {
      return human_readable_letter(i);
    }

    typename Presentation<std::string>::letter_type
    human_readable_letter(size_t i) {
      using letter_type = typename Presentation<std::string>::letter_type;
      // Choose visible characters a-zA-Z0-9 first before anything else
      // The ascii ranges for these characters are: [97, 123), [65, 91),
      // [48, 58) so the remaining range of chars that are appended to the end
      // after these chars are [0,48), [58, 65), [91, 97), [123, 255)
      if (i >= std::numeric_limits<letter_type>::max()
                   - std::numeric_limits<letter_type>::min()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a value in the range [0, {}) found {}",
            std::numeric_limits<letter_type>::max()
                - std::numeric_limits<letter_type>::min(),
            i);
      }
      return chars_in_human_readable_order()[i];
    }

    typename Presentation<word_type>::letter_type
    human_readable_index(typename Presentation<std::string>::letter_type c) {
      static bool first_call = true;
      static std::unordered_map<Presentation<std::string>::letter_type,
                                Presentation<word_type>::letter_type>
          map;
      if (first_call) {
        first_call        = false;
        auto const& chars = chars_in_human_readable_order();
        for (letter_type i = 0; i < chars.size(); ++i) {
          map.emplace(chars[i], i);
        }
      }

      auto it = map.find(c);
      if (it == map.cend()) {
        LIBSEMIGROUPS_EXCEPTION(
            "unexpected character, cannot convert \'{}\' to a letter", c);
      }

      return it->second;
    }

    std::string to_gap_string(Presentation<word_type> const& p,
                              std::string const&             var_name) {
      if (p.alphabet().size() > 49) {
        LIBSEMIGROUPS_EXCEPTION("expected at most 49 generators, found {}!",
                                p.alphabet().size());
      }

      auto to_gap_word = [](word_type const& w) -> std::string {
        std::string out;
        std::string sep = "";
        for (auto it = w.cbegin(); it < w.cend(); ++it) {
          out += sep + human_readable_letter(*it);
          sep = " * ";
        }
        return out;
      };

      std::string out = "free := FreeSemigroup(";
      std::string sep = "";
      for (auto it = p.alphabet().cbegin(); it != p.alphabet().cend(); ++it) {
        out += fmt::format("{}\"{}\"", sep, human_readable_letter(*it));
        sep = ", ";
      }
      out += ");\n";

      for (size_t i = 0; i != p.alphabet().size(); ++i) {
        out += fmt::format("{} := free.{};\n", human_readable_letter(i), i + 1);
      }
      out += "\n";

      out += "rules := [";
      sep = "";
      for (auto it = p.rules.cbegin(); it < p.rules.cend(); it += 2) {
        out += fmt::format("{}\n          [{}, {}]",
                           sep,
                           to_gap_word(*it),
                           to_gap_word(*(it + 1)));
        sep = ", ";
      }
      out += "\n         ];\n";
      out += var_name + " := free / rules;\n";
      return out;
    }
  }  // namespace presentation
}  // namespace libsemigroups
