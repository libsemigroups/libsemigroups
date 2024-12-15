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
#include <cctype>         // for isprint
#include <cstddef>        // for size_t
#include <string>         // for basic_string, operator+
#include <unordered_map>  // for operator==
#include <vector>         // for vector

#include "libsemigroups/exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/presentation.hpp"  // for Presentation, to_string, to_word
#include "libsemigroups/to-presentation.hpp"  // for to_presentation
#include "libsemigroups/types.hpp"            // for word_type
#include "libsemigroups/word-range.hpp"       // for human_readable_letter

#include "libsemigroups/detail/fmt.hpp"  // for format

namespace libsemigroups {
  namespace detail {

    bool isprint(std::string const& alphabet) {
      return std::all_of(alphabet.cbegin(), alphabet.cend(), [](auto c) {
        return std::isprint(c);
      });
    }

    std::string to_printable(char c) {
      if (std::isprint(c)) {
        return fmt::format("\'{:c}\'", c);
      } else {
        return fmt::format("(char with value) {}", static_cast<int>(c));
      }
    }

    std::string to_printable(std::string const& alphabet) {
      if (isprint(alphabet)) {
        return fmt::format("\"{}\"", alphabet);
      } else {
        return fmt::format("(char values) {}",
                           std::vector<int>(alphabet.begin(), alphabet.end()));
      }
    }
  }  // namespace detail

  namespace presentation {
    std::string to_gap_string(Presentation<word_type> const& p,
                              std::string const&             var_name) {
      p.validate();
      if (p.alphabet().size() > 49) {
        LIBSEMIGROUPS_EXCEPTION("expected at most 49 generators, found {}!",
                                p.alphabet().size());
      }

      auto to_gap_word = [](word_type const& w) -> std::string {
        if (w.empty()) {
          return "One(F)";
        }
        std::string out;
        std::string sep = "";
        for (auto it = w.cbegin(); it < w.cend(); ++it) {
          out += sep + words::human_readable_letter<>(*it);
          sep = " * ";
        }
        return out;
      };

      std::string out = "F := Free";
      if (p.contains_empty_word()) {
        out += "Monoid(";
      } else {
        out += "Semigroup(";
      }

      std::string sep = "";
      for (auto it = p.alphabet().cbegin(); it != p.alphabet().cend(); ++it) {
        out += fmt::format(
            "{}\"{}\"", sep, words::human_readable_letter<>(*it));
        sep = ", ";
      }
      out += ");\n";

      out += "AssignGeneratorVariables(F);;\n";

      out += "R := [";
      sep = "";
      for (auto it = p.rules.cbegin(); it < p.rules.cend(); it += 2) {
        out += fmt::format("{}\n          [{}, {}]",
                           sep,
                           to_gap_word(*it),
                           to_gap_word(*(it + 1)));
        sep = ", ";
      }
      out += "\n         ];\n";
      out += var_name + " := F / R;\n";
      return out;
    }

    std::string to_gap_string(Presentation<std::string> const& p,
                              std::string const&               var_name) {
      return to_gap_string(to_presentation<word_type>(p), var_name);
    }

  }  // namespace presentation
}  // namespace libsemigroups
