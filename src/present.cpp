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

// This file contains implementations of the functions declared in
// presentation.hpp

#include <algorithm>      // for transform
#include <cstddef>        // for size_t
#include <string>         // for basic_string, operator+
#include <unordered_map>  // for operator==
#include <vector>         // for vector

#include "fmt/core.h"  // for format

#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/present.hpp"    // for Presentation, to_string, to_word
#include "libsemigroups/types.hpp"      // for word_type
#include "libsemigroups/words.hpp"      // for human_readable_char

namespace libsemigroups {
  namespace presentation {

    typename Presentation<std::string>::letter_type
    human_readable_letter(Presentation<std::string> const&, size_t i) {
      return human_readable_char(i);
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
          out += sep + human_readable_char(*it);
          sep = " * ";
        }
        return out;
      };

      std::string out = "free := FreeSemigroup(";
      std::string sep = "";
      for (auto it = p.alphabet().cbegin(); it != p.alphabet().cend(); ++it) {
        out += fmt::format("{}\"{}\"", sep, human_readable_char(*it));
        sep = ", ";
      }
      out += ");\n";

      for (size_t i = 0; i != p.alphabet().size(); ++i) {
        out += fmt::format("{} := free.{};\n", human_readable_char(i), i + 1);
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

  void to_word(Presentation<std::string> const& p,
               std::string const&               input,
               word_type&                       output) {
    output.resize(input.size(), 0);
    std::transform(input.cbegin(), input.cend(), output.begin(), [&p](auto i) {
      return p.index(i);
    });
  }

  word_type to_word(Presentation<std::string> const& p, std::string const& s) {
    word_type w;
    to_word(p, s, w);
    return w;
  }

  void to_string(Presentation<std::string> const& p,
                 word_type const&                 input,
                 std::string&                     output) {
    output.resize(input.size(), 0);
    std::transform(input.cbegin(), input.cend(), output.begin(), [&p](auto i) {
      return p.letter(i);
    });
  }

  std::string to_string(Presentation<std::string> const& p,
                        word_type const&                 w) {
    std::string s;
    to_string(p, w, s);
    return s;
  }
}  // namespace libsemigroups
