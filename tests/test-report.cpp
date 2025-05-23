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

#include <chrono>    // for chrono
#include <functional> // for function
#include <thread>     // for sleep_for

#include "catch.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "test-main.hpp"

#include "libsemigroups/detail/report.hpp"  // for Ticker
#include "libsemigroups/exception.hpp"      // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {
  struct LibsemigroupsException;
  
  LIBSEMIGROUPS_TEST_CASE("Ticker", "001", "default constructor", "[quick]") {
    using detail::Ticker;
    
    // Test that default constructor works
    REQUIRE_NOTHROW(Ticker ticker);
  }
  
  LIBSEMIGROUPS_TEST_CASE("Ticker", "002", "function call operator", "[quick]") {
    using detail::Ticker;
    
    // Test counter for ticker callback
    size_t counter = 0;
    
    // Create ticker with default constructor
    Ticker ticker;
    
    // Use the function call operator to set the ticker
    ticker([&counter]() { counter++; }, std::chrono::milliseconds(10));
    
    // Sleep to let the ticker run
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Counter should have been incremented multiple times
    REQUIRE(counter > 0);
  }
  
  LIBSEMIGROUPS_TEST_CASE("Ticker", "003", "reuse exception", "[quick]") {
    using detail::Ticker;
    
    // Create ticker with default constructor
    Ticker ticker;
    
    // Use the function call operator to set the ticker
    ticker([]() {}, std::chrono::milliseconds(10));
    
    // Trying to use the operator again should throw an exception
    REQUIRE_THROWS_AS(
        ticker([]() {}, std::chrono::milliseconds(10)),
        LibsemigroupsException
    );
  }
  
}  // namespace libsemigroups