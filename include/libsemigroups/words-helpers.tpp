//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2026 James Mitchell + Joseph Edwards
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

// This file contains implementations of functions related to words (counting,
// and converting) in libsemigroups.

namespace libsemigroups {
  namespace words {
    template <typename Word>
    std::vector<Word> parse_relations(std::string const& w) {
      std::vector<Word> rules;
      size_t                             start           = 0;
      size_t                             p               = 0;
      size_t                             equal_sign      = std::string::npos;
      bool                               whitespace_only = true;
      int                                bracket_depth   = 0;

      while (p <= w.size()) {
        if (w[p] == '(') {
          bracket_depth++;
        } else if (w[p] == ')') {
          bracket_depth--;
          if (bracket_depth < 0) {
            LIBSEMIGROUPS_EXCEPTION("Relation starting at string position {} "
                                    "includes mismatched brackets",
                                    start);
          }
        }
        else if (w[p] == '=') {
          if (bracket_depth != 0) {
            LIBSEMIGROUPS_EXCEPTION("Relation starting at string position {} "
                                    "includes mismatched brackets",
                                    start);
          }
          equal_sign = p;  // save equal sign index as marker for lhs and rhs
          whitespace_only = false;
        } else if ((w[p] == ',' || p == w.size()) && bracket_depth == 0) {
          // handle empty equation
          if (start == p || whitespace_only) {
            start           = p + 1;
            whitespace_only = true;
            p++;
            continue;
          }
          if (equal_sign == std::string::npos) {
            LIBSEMIGROUPS_EXCEPTION("Relation starting at string position {} "
                                    "is incomplete, missing internal \'=\'",
                                    start);
          }

          std::string left = parse(w.substr(start, equal_sign - start));
          std::string right
              = parse(w.substr(equal_sign + 1, p - equal_sign - 1));

          if (left == "" || right == "") {
            LIBSEMIGROUPS_EXCEPTION(
                "Relation starting at string position {} is incomplete, "
                "missing one side of the equation",
                start);
          }

          // bridge to template
          Word left_word(left.begin(), left.end());
          Word right_word(right.begin(), right.end());

          rules.emplace_back(std::move(left_word));
          rules.emplace_back(std::move(right_word));

          start           = p + 1;
          equal_sign      = std::string::npos;
          whitespace_only = true;
        } else if (w[p] != ' ' && w[p] != '\t' && w[p] != '\n'
                   && w[p] != '\r') {
          whitespace_only = false;
        }
        p++;
      }
      if (bracket_depth != 0) {
        LIBSEMIGROUPS_EXCEPTION("Final relation is missing closing bracket");
      }
      return rules;
    }

    ////////////////////////////////////////////////////////////////////////
    // pow
    ////////////////////////////////////////////////////////////////////////

    template <typename Word>
    void pow_inplace(Word& x, size_t n) {
      Word y(x);
      x.reserve(x.size() * n);
      if (n % 2 == 0) {
        x = Word({});
      }

      while (n > 1) {
        y += y;
        n /= 2;
        if (n % 2 == 1) {
          x += y;
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // prod
    ////////////////////////////////////////////////////////////////////////

    // Note: we could do a version of the below using insert on words, where
    // the step is +/- 1.
    template <typename Container, typename Word>
    Word prod(Container const& elts, int first, int last, int step) {
      if (step == 0) {
        LIBSEMIGROUPS_EXCEPTION("the 4th argument must not be 0");
      } else if (((first < last && step > 0) || (first > last && step < 0))
                 && elts.size() == 0) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument must not be empty if the "
                                "given range is not empty");
        // TODO Is int signed? Should this also contain
        // std::numeric_limits<int>::min?
      } else if (elts.size() > std::numeric_limits<int>::max()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument must have size less than or equal to {}",
            std::numeric_limits<int>::max());
      }
      Word      result;
      int const s = elts.size();

      if (first < last) {
        if (step < 0) {
          return result;
        }
        result.reserve((last - first) / step);

        for (int i = first; i < last; i += step) {
          size_t const a = (i % s + s) % s;
          LIBSEMIGROUPS_ASSERT(static_cast<int>(a) < s);
          result += elts[a];
        }
      } else {
        if (step > 0) {
          return result;
        }
        size_t steppos = static_cast<size_t>(-step);
        result.reserve((first - last) / steppos);
        for (int i = first; i > last; i += step) {
          size_t const a = (i % s + s) % s;
          LIBSEMIGROUPS_ASSERT(static_cast<int>(a) < s);
          result += elts[a];
        }
      }
      return result;
    }
  }  // namespace words
}  // namespace libsemigroups
