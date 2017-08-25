//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Michael Torpey
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

#include <utility>

#include "../src/partition.h"
#include "../src/semigroups.h"
#include "catch.hpp"

using namespace libsemigroups;

TEST_CASE("Partition 01: 0-argument constructor", "[partition][quick][01]") {
  Partition<size_t>* part = new Partition<size_t>();
  REQUIRE(part->size() == 0);
  delete part;
}

TEST_CASE("Partition 02: 1-argument constructor", "[partition][quick][02]") {
  std::vector<std::vector<word_t*>*>* vector
      = new std::vector<std::vector<word_t*>*>();

  for (size_t i = 0; i < 3; i++) {
    vector->push_back(new std::vector<word_t*>());
  }

  vector->at(0)->push_back(new word_t({1}));
  vector->at(0)->push_back(new word_t({0, 1}));
  vector->at(1)->push_back(new word_t({0, 0, 1}));
  vector->at(1)->push_back(new word_t({1, 3, 2, 2}));
  vector->at(2)->push_back(new word_t({3}));

  Partition<word_t>* part = new Partition<word_t>(vector);

  REQUIRE(part->size() == 3);
  REQUIRE(part->at(0)->size() == 2);
  REQUIRE((*part)[1]->size() == 2);
  REQUIRE((*part)[2]->size() == 1);
  REQUIRE(*part->at(1, 1) == word_t({1, 3, 2, 2}));

  delete part;
}
