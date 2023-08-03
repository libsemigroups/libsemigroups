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
#include "libsemigroups/words.hpp"

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
               word_type&                       w,
               std::string const&               s) {
    w.resize(s.size(), 0);
    std::transform(
        s.cbegin(), s.cend(), w.begin(), [&p](auto i) { return p.index(i); });
  }

  word_type to_word(Presentation<std::string> const& p, std::string const& s) {
    word_type w;
    to_word(p, w, s);
    return w;
  }

  std::string to_string(Presentation<std::string> const& p,
                        word_type const&                 w) {
    std::string s(w.size(), 0);
    std::transform(w.cbegin(), w.cend(), s.begin(), [&p](auto i) {
      return p.letter_no_checks(i);
    });
    return s;
  }
}  // namespace libsemigroups
