//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

// This file contains some performance benchmarks.

#include "../semigroups.h"
#include "catch.hpp"

using namespace libsemigroups;

template <typename T> static inline void really_delete_cont(T cont) {
  for (Element* x : cont) {
    x->really_delete();
    delete x;
  }
}

std::string vector_to_py_list(std::vector<size_t>& vec) {
  std::string py_list = "[";
  for (auto const& val : vec) {
    py_list += std::to_string(val);
    py_list += ", ";
  }
  py_list.erase(py_list.end() - 2, py_list.end() - 1);
  py_list += "]";
  return py_list;
}

std::vector<size_t> bench(std::vector<Element*>           gens,
                          std::vector<Element*>           coll,
                          size_t                          limit,
                          std::function<void(Semigroup*)> setup,
                          size_t                          nr_trials = 10) {
  std::vector<size_t> mean_time = {0, 0, 0};
  Timer               t;

  for (size_t j = 0; j < nr_trials; j++) {
    Semigroup* S = new Semigroup(gens);
    setup(S);

    t.start();
    S->enumerate(limit);
    t.stop();
    mean_time[2] += t.elapsed() / nr_trials;

    t.start();
    S->add_generators(coll);
    t.stop();
    mean_time[0] += t.elapsed() / nr_trials;
    mean_time[2] += mean_time[0];

    limit = S->current_size();
    delete S;

    S = new Semigroup(gens);
    setup(S);

    t.start();
    S->add_generators(coll);
    S->enumerate(limit);
    t.stop();

    mean_time[1] += t.elapsed() / nr_trials;
    delete S;
  }

  return mean_time;
}

size_t const example1_step = 5000;

void example1_setup(Semigroup* S) {
  S->reserve(650000);
  S->set_report(false);
  S->set_batch_size(example1_step);
}

// My favourite example

TEST_CASE("Benchmark 01: add_generators versus enumerate example 1",
          "[.][benchmark][01]") {
  std::cout << "Performing " << 597369 / example1_step << " runs . . ."
            << std::endl;

  std::vector<Element*> gens
      = {new Transformation<uint_fast8_t>({1, 7, 2, 6, 0, 4, 1, 5}),
         new Transformation<uint_fast8_t>({2, 4, 6, 1, 4, 5, 2, 7}),
         new Transformation<uint_fast8_t>({3, 0, 7, 2, 4, 6, 2, 4}),
         new Transformation<uint_fast8_t>({3, 2, 3, 4, 5, 3, 0, 1}),
         new Transformation<uint_fast8_t>({4, 3, 7, 7, 4, 5, 0, 4}),
         new Transformation<uint_fast8_t>({5, 6, 3, 0, 3, 0, 5, 1}),
         new Transformation<uint_fast8_t>({6, 0, 1, 1, 1, 6, 3, 4})};

  std::vector<Element*> coll
      = {new Transformation<uint_fast8_t>({7, 7, 4, 0, 6, 4, 1, 7})};

  std::vector<size_t> ag_times;
  std::vector<size_t> en_times;
  std::vector<size_t> cu_times;
  size_t              counter = 0;

  for (size_t limit = example1_step; limit < 597369; limit += example1_step) {
    std::vector<size_t> t = bench(gens, coll, limit, example1_setup, 3);
    ag_times.push_back(t[0]);
    en_times.push_back(t[1]);
    cu_times.push_back(t[2]);
    std::cout << "Run " << counter++ << " complete" << std::endl;
  }
  std::cout << vector_to_py_list(ag_times) << std::endl;
  std::cout << vector_to_py_list(en_times) << std::endl;
  really_delete_cont(gens);
  really_delete_cont(coll);
}

size_t const example2_step = 5000;

void example2_setup(Semigroup* S) {
  S->reserve(700000);
  S->set_report(false);
  S->set_batch_size(example1_step);
}

// S := HallMonoid(5);
// S := Semigroup(S.1, S.3, S.4, S.12);

TEST_CASE("Benchmark 02: add_generators versus enumerate example 2",
          "[.][benchmark][02]") {
  std::cout << "Performing " << 663336 / example1_step << " runs . . ."
            << std::endl;

  std::vector<Element*> gens = {new BooleanMat({{0, 1, 0, 0, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 0, 0, 1},
                                                {1, 0, 0, 0, 0}}),
                                new BooleanMat({{0, 0, 0, 0, 1},
                                                {0, 0, 0, 1, 0},
                                                {0, 0, 1, 0, 0},
                                                {0, 1, 0, 0, 0},
                                                {1, 0, 0, 0, 1}}),
                                new BooleanMat({{0, 0, 0, 0, 1},
                                                {0, 0, 0, 1, 0},
                                                {0, 1, 1, 0, 0},
                                                {1, 0, 1, 0, 0},
                                                {1, 1, 0, 0, 0}})};

  std::vector<Element*> coll = {new BooleanMat({{0, 0, 0, 1, 1},
                                                {0, 0, 1, 0, 1},
                                                {0, 1, 0, 1, 0},
                                                {1, 0, 1, 0, 0},
                                                {1, 1, 0, 0, 0}})};
  std::vector<size_t> ag_times;
  std::vector<size_t> en_times;
  std::vector<size_t> cu_times;
  size_t              counter = 0;

  for (size_t limit = example2_step; limit < 663336; limit += example2_step) {
    std::vector<size_t> t = bench(gens, coll, limit, example2_setup, 3);
    ag_times.push_back(t[0]);
    en_times.push_back(t[1]);
    cu_times.push_back(t[2]);
    std::cout << "Run " << counter++ << " complete" << std::endl;
  }
  std::cout << vector_to_py_list(ag_times) << std::endl;
  std::cout << vector_to_py_list(en_times) << std::endl;
  really_delete_cont(gens);
  really_delete_cont(coll);
}
