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

// This file contains some function for creating semigroups of certain types
// primarily for the purposes of testing and benchmarking.

#ifndef LIBSEMIGROUPS_BENCHMARK_SRC_EXAMPLES_H_
#define LIBSEMIGROUPS_BENCHMARK_SRC_EXAMPLES_H_

#include <libsemigroups/semigroups.h>

namespace libsemigroups {
  typedef std::vector<std::vector<bool>> BooleanMatArg;
  static BooleanMatArg id(size_t n) {
    BooleanMatArg x(n, std::vector<bool>(n, false));
    for (size_t k = 0; k < n; ++k) {
      x[k][k] = true;
    }
    return x;
  }

  std::vector<Element*>* uni_triangular_boolean_mat(size_t n) {
    std::vector<Element*>* gens = new std::vector<Element*>();
    gens->reserve((n * (n - 1)) / 2);
    for (size_t i = 0; i < n - 1; ++i) {
      for (size_t j = i + 1; j < n; ++j) {
        BooleanMatArg x = id(n);
        x[i][j]         = true;
        gens->push_back(new BooleanMat(x));
      }
    }
    gens->push_back(gens->at(0)->identity());
    return gens;
  }

  std::vector<Element*>* upper_triangular_boolean_mat(size_t n) {
    if (n == 1) {
      return new std::vector<Element*>(
          {new BooleanMat(std::vector<std::vector<bool>>({true}))});
    }
    std::vector<Element*>* gens = uni_triangular_boolean_mat(n);
    gens->reserve(gens->size() + n);
    for (size_t i = 0; i < n; ++i) {
      BooleanMatArg x = id(n);
      x[i][i]         = false;
      gens->push_back(new BooleanMat(x));
    }
    return gens;
  }

  std::vector<Element*>* gossip(size_t n) {
    if (n == 1) {
      return new std::vector<Element*>(
          {new BooleanMat(std::vector<std::vector<bool>>({true}))});
    }
    std::vector<Element*>* gens = new std::vector<Element*>();
    for (size_t i = 0; i < n - 1; ++i) {
      for (size_t j = i + 1; j < n; ++j) {
        BooleanMatArg x = id(n);
        x[i][j]         = true;
        x[j][i]         = true;
        gens->push_back(new BooleanMat(x));
      }
    }
    gens->push_back(gens->at(0)->identity());
    return gens;
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BENCHMARK_SRC_EXAMPLES_H_
