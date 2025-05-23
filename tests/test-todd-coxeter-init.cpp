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

#include "catch.hpp"      // for LIBSEMIGROUPS_TEST_CASE
#include "test-main.hpp"

#include "libsemigroups/constants.hpp"              // for UNDEFINED
#include "libsemigroups/presentation.hpp"           // for Presentation
#include "libsemigroups/todd-coxeter.hpp"           // for ToddCoxeter
#include "libsemigroups/word-graph.hpp"             // for WordGraph
#include "libsemigroups/word-range.hpp"             // for operator"" _w

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  struct LibsemigroupsException;
  
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeterImpl", 
                          "001", 
                          "initialization from WordGraph and Presentation", 
                          "[quick][todd-coxeter]") {
    auto                    rg = detail::ReportGuard(false);
    Presentation<word_type> p;
    p.alphabet(2);
    p.contains_empty_word(true);
    
    // Create a word graph with some structure
    WordGraph<ForestGraph> wg(p);
    wg.add_edge(0, 1, 0);
    wg.add_edge(0, 2, 1);
    wg.add_edge(1, 3, 0);
    wg.add_edge(2, 3, 1);
    
    // Initialize TC with the word graph
    ToddCoxeter tc(congruence_kind::twosided, wg);
    
    // Check that the word graph structure is preserved
    REQUIRE(tc.current_word_graph().number_of_nodes() == 4);
    REQUIRE(tc.current_word_graph().number_of_edges() == 4);
    
    // Create presentation with a rule
    Presentation<word_type> p2;
    p2.alphabet(2);
    p2.contains_empty_word(true);
    presentation::add_rule(p2, 01_w, 10_w);
    
    // Initialize TC with both presentation and word graph
    tc.init(congruence_kind::twosided, p2, wg);
    
    // Check the presentation and word graph are correctly set
    REQUIRE(tc.presentation().rules == std::vector<word_type>({01_w, 10_w}));
    REQUIRE(tc.current_word_graph().number_of_nodes() == 4);
  }
  
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeterImpl", 
                          "002", 
                          "reset_settings_stack", 
                          "[quick][todd-coxeter]") {
    auto rg = detail::ReportGuard(false);
    
    Presentation<word_type> p;
    p.alphabet(2);
    
    ToddCoxeter tc(congruence_kind::twosided, p);
    
    // Change some settings from default
    auto original_min = tc.lookahead_min();
    tc.lookahead_min(1000);
    
    // Re-initialize to check settings reset
    tc.init(congruence_kind::twosided, p);
    
    // Settings should be reset to default
    REQUIRE(tc.lookahead_min() == original_min);
  }
  
  LIBSEMIGROUPS_TEST_CASE("ToddCoxeterImpl", 
                          "003", 
                          "init with HLT on free semigroup", 
                          "[quick][todd-coxeter]") {
    auto rg = detail::ReportGuard(false);
    
    Presentation<word_type> p;
    p.alphabet(2);
    
    ToddCoxeter tc(congruence_kind::twosided, p);
    tc.strategy(options::strategy::hlt);
    
    // Should throw because HLT can't be used with a free presentation
    REQUIRE_THROWS_AS(tc.run(), LibsemigroupsException);
    REQUIRE_THROWS_AS(tc.run_for(std::chrono::milliseconds(10)), 
                      LibsemigroupsException);
    
    size_t val = 0;
    REQUIRE_THROWS_AS(tc.run_until([&val]() { return val++ > 10; }),
                      LibsemigroupsException);
  }
}  // namespace libsemigroups