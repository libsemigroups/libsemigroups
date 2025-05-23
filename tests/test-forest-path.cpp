//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

#include <algorithm>  // for reverse

#include "catch.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "test-main.hpp"

#include "libsemigroups/forest.hpp"
#include "libsemigroups/word-range.hpp"  // for operator"" _w

namespace libsemigroups {
  struct LibsemigroupsException;
  
  // Create a test forest with a specific structure for path testing
  namespace {
    Forest create_path_forest() {
      Forest forest(10);
      
      // Create a tree with multiple paths:
      //     0
      //    /|\
      //   1 5 8
      //  /|   |
      // 2 4   9
      // |
      // 3
      //
      // 6 and 7 are isolated nodes
      
      forest.set_parent_and_label_no_checks(1, 0, 1);
      forest.set_parent_and_label_no_checks(2, 1, 2);
      forest.set_parent_and_label_no_checks(3, 2, 3);
      forest.set_parent_and_label_no_checks(4, 1, 4);
      forest.set_parent_and_label_no_checks(5, 0, 5);
      forest.set_parent_and_label_no_checks(8, 0, 8);
      forest.set_parent_and_label_no_checks(9, 8, 9);
      
      return forest;
    }
  }
  
  LIBSEMIGROUPS_TEST_CASE("Forest", "020", "path_from_root functions", "[quick]") {
    using literals::operator""_w;
    
    Forest forest = create_path_forest();
    
    // Test path_from_root_no_checks with different nodes
    REQUIRE(forest::path_from_root_no_checks(forest, 0) == word_type());
    REQUIRE(forest::path_from_root_no_checks(forest, 1) == 1_w);
    REQUIRE(forest::path_from_root_no_checks(forest, 3) == 123_w);
    REQUIRE(forest::path_from_root_no_checks(forest, 9) == 89_w);
    
    // Test path_from_root that performs bounds checking
    REQUIRE(forest::path_from_root(forest, 4) == 14_w);
    REQUIRE_THROWS_AS(forest::path_from_root(forest, 20), LibsemigroupsException);
  }
  
  LIBSEMIGROUPS_TEST_CASE("Forest", "021", "depth functions", "[quick]") {
    Forest forest = create_path_forest();
    
    // Test depth_no_checks
    REQUIRE(forest.depth_no_checks(0) == 0);
    REQUIRE(forest.depth_no_checks(1) == 1);
    REQUIRE(forest.depth_no_checks(3) == 3);
    REQUIRE(forest.depth_no_checks(5) == 1);
    REQUIRE(forest.depth_no_checks(9) == 2);
    
    // Test depth with bounds checking
    REQUIRE(forest.depth(4) == 2);
    REQUIRE_THROWS_AS(forest.depth(20), LibsemigroupsException);
  }
  
  LIBSEMIGROUPS_TEST_CASE("Forest", "022", "path reversals", "[quick]") {
    using literals::operator""_w;
    
    Forest forest = create_path_forest();
    
    // Test that path_to_root and path_from_root are reversals of each other
    for (size_t n = 0; n < forest.number_of_nodes(); ++n) {
      auto to_root = forest::path_to_root(forest, n);
      auto from_root = forest::path_from_root(forest, n);
      
      std::reverse(from_root.begin(), from_root.end());
      REQUIRE(to_root == from_root);
    }
  }
}  // namespace libsemigroups