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

#include "libsemigroups/constants.hpp"              // for UNDEFINED
#include "libsemigroups/presentation.hpp"           // for Presentation
#include "libsemigroups/todd-coxeter.hpp"           // for ToddCoxeter
#include "libsemigroups/to-todd-coxeter.hpp"        // for reduce_no_run
#include "libsemigroups/word-range.hpp"             // for operator"" _w

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  using detail::tril;
  
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeterImpl", 
                          "010", 
                          "reduce_no_run_no_checks", 
                          "[quick][todd-coxeter]") {
    auto                    rg = detail::ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    
    // Add some rules
    presentation::add_rule(p, 00_w, 0_w);  // a^2 = a
    presentation::add_rule(p, 11_w, 1_w);  // b^2 = b
    presentation::add_rule(p, 010_w, 01_w); // aba = ab
    
    ToddCoxeter tc(congruence_kind::twosided, p);
    
    // Run the algorithm to establish some reduction structure
    tc.run();
    
    // Test reduce_no_run on various words
    REQUIRE(todd_coxeter::reduce_no_run(tc, 00000_w) == 0_w);
    REQUIRE(todd_coxeter::reduce_no_run(tc, 11111_w) == 1_w);
    REQUIRE(todd_coxeter::reduce_no_run(tc, 01010101_w) == 0101_w);
    REQUIRE(todd_coxeter::reduce_no_run(tc, 10101010_w) == 1010_w);
  }
  
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeterImpl", 
                          "011", 
                          "currently_contains_relation", 
                          "[quick][todd-coxeter]") {
    auto                      rg = detail::ReportGuard(false);
    Presentation<std::string> p;
    p.alphabet("ab");
    
    // Add some rules
    presentation::add_rule(p, "aa", "a");
    presentation::add_rule(p, "bb", "b");
    presentation::add_rule(p, "aba", "ab");
    
    ToddCoxeter tc(congruence_kind::twosided, p);
    
    // Before running the algorithm
    REQUIRE(todd_coxeter::currently_contains(tc, "aa", "a") == tril::TRUE);
    REQUIRE(todd_coxeter::currently_contains(tc, "bb", "b") == tril::TRUE);
    REQUIRE(todd_coxeter::currently_contains(tc, "aba", "ab") == tril::TRUE);
    
    // Word not in the presentation
    REQUIRE(todd_coxeter::currently_contains(tc, "aaa", "a") == tril::unknown);
    
    // Run the algorithm to establish more relations
    tc.run();
    
    // Now we can determine more relations
    REQUIRE(todd_coxeter::currently_contains(tc, "aaa", "a") == tril::TRUE);
    REQUIRE(todd_coxeter::currently_contains(tc, "abba", "ab") == tril::TRUE);
    
    // Words that are not equal should return FALSE
    REQUIRE(todd_coxeter::currently_contains(tc, "a", "b") == tril::FALSE);
    
    // Test with reduced words
    std::string word1 = "aaaabbbaaabbbaba";
    std::string word2 = "ababa";
    
    // Reduce both words and compare their reductions
    std::string reduced1 = todd_coxeter::reduce_no_run(tc, word1);
    std::string reduced2 = todd_coxeter::reduce_no_run(tc, word2);
    
    REQUIRE(todd_coxeter::currently_contains(tc, reduced1, reduced2) == tril::TRUE);
  }
}  // namespace libsemigroups