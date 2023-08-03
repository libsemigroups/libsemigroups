//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2023 James D. Mitchell
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

// This file contains  ?

namespace libsemigroups {

  namespace words {
    template <typename Word, typename>
    Word pow(Word const& x, size_t n) {
      Word y(x);
      pow_inplace(y, n);
      return y;
    }

    template <typename Word, typename>
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

    // Note: we could do a version of the below using insert on words, where
    // the step is +/- 1.
    template <typename Container, typename Word, typename>
    Word prod(Container const& elts, int first, int last, int step) {
      if (step == 0) {
        LIBSEMIGROUPS_EXCEPTION("the 4th argument must not be 0");
      } else if (((first < last && step > 0) || (first > last && step < 0))
                 && elts.size() == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "1st argument must be empty if the given range is not empty");
      }
      Word result;

      if (first < last) {
        if (step < 0) {
          return result;
        }
        result.reserve((last - first) / step);

        int i = first;
        for (; i < last && i < 0; i += step) {
          size_t a = ((-i / elts.size()) + 1) * elts.size() + i;
          result += elts[a];
        }
        for (; i < last; i += step) {
          result += elts[i % elts.size()];
        }
      } else {
        if (step > 0) {
          return result;
        }
        size_t steppos = static_cast<size_t>(-step);
        result.reserve((first - last) / steppos);
        int i = first;
        for (; i > last && i >= 0; i += step) {
          result += elts[i % elts.size()];
        }
        for (; i > last; i += step) {
          size_t a = ((-i / elts.size()) + 1) * elts.size() + i;
          result += elts[a];
        }
      }
      return result;
    }
  }  // namespace words
}  // namespace libsemigroups
