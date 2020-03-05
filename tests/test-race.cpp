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

// The purpose of this file is to test the Race class.

#include <cstddef>  // for size_t

#include "catch.hpp"  // for REQUIRE, REQUIRE_THROWS_AS
#include "libsemigroups/libsemigroups-exception.hpp"  // for LibsemigroupsException (ptr o...
#include "libsemigroups/race.hpp"                     // for Race
#include "libsemigroups/report.hpp"                   // for ReportGuard
#include "libsemigroups/runner.hpp"                   // for Runner
#include "test-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

  namespace detail {

    class TestRunner1 : public Runner {
     private:
      void run_impl() override {
        while (!stopped()) {
        }
      }

      bool finished_impl() const override {
        return stopped();
      }
    };

    class TestRunner2 : public Runner {
     private:
      void run_impl() override {}
      bool finished_impl() const override {
        return true;
      }
    };

    class TestRunner3 : public Runner {
     private:
      void run_impl() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
      }

      bool finished_impl() const override {
        return started();
      }
    };

    LIBSEMIGROUPS_TEST_CASE("Race", "001", "run_for", "[quick]") {
      auto rg = ReportGuard(REPORT);
      Race rc;
      rc.max_threads(1);
      REQUIRE(rc.max_threads() == 1);
      rc.add_runner(std::make_shared<TestRunner1>());
      rc.run_for(std::chrono::milliseconds(10));
      rc.run_until([]() -> bool { return true; });
      REQUIRE(rc.winner() != nullptr);
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "002", "run_until", "[quick]") {
      auto rg = ReportGuard(REPORT);
      Race rc;
      rc.add_runner(std::make_shared<TestRunner1>());
      size_t nr  = 0;
      auto   foo = [&nr]() -> bool { return ++nr == 2; };
      rc.run_until(foo, std::chrono::milliseconds(10));
      REQUIRE(rc.winner() != nullptr);
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "003", "exceptions", "[quick]") {
      auto rg = ReportGuard(REPORT);
      Race rc;
      REQUIRE_THROWS_AS(rc.run_for(std::chrono::milliseconds(10)),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(rc.run_until([]() -> bool { return true; }),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(rc.run(), LibsemigroupsException);

      rc.add_runner(std::make_shared<TestRunner1>());
      rc.run_for(std::chrono::milliseconds(10));
      REQUIRE(rc.winner() != nullptr);
      auto tr = std::make_shared<TestRunner1>();
      REQUIRE_THROWS_AS(rc.add_runner(tr), LibsemigroupsException);
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "004", "iterators", "[quick]") {
      auto rg = ReportGuard(REPORT);
      Race rc;
      rc.max_threads(2);
      rc.add_runner(std::make_shared<TestRunner1>());
      rc.add_runner(std::make_shared<TestRunner1>());
      REQUIRE(rc.end() - rc.begin() == rc.number_runners());
      REQUIRE(rc.cend() - rc.cbegin() == rc.number_runners());
      REQUIRE(2 == rc.number_runners());
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "005", "find_runner", "[quick]") {
      auto rg = ReportGuard(REPORT);
      Race rc;
      rc.max_threads(2);
      rc.add_runner(std::make_shared<TestRunner1>());
      rc.add_runner(std::make_shared<TestRunner1>());
      REQUIRE(rc.find_runner<TestRunner1>() != nullptr);
      REQUIRE(rc.find_runner<TestRunner2>() == nullptr);
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "006", "run_func", "[quick]") {
      auto rg = ReportGuard(REPORT);
      Race rc;
      rc.max_threads(2);
      rc.add_runner(std::make_shared<TestRunner1>());
      rc.add_runner(std::make_shared<TestRunner3>());
      rc.run_for(std::chrono::milliseconds(10));
      REQUIRE(rc.winner() != nullptr);
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "007", "run_func", "[quick]") {
      auto rg = ReportGuard(REPORT);
      Race rc;
      rc.max_threads(2);
      TestRunner1* tr = new TestRunner1();
      tr->run_for(std::chrono::milliseconds(10));
      rc.add_runner(std::shared_ptr<TestRunner1>(tr));
      rc.add_runner(std::make_shared<TestRunner3>());
      rc.run_for(std::chrono::milliseconds(10));
      REQUIRE(rc.winner() != nullptr);
    }

    LIBSEMIGROUPS_TEST_CASE("Race", "008", "run_func", "[quick]") {
      auto rg = ReportGuard(REPORT);
      Race rc;
      rc.max_threads(4);
      rc.add_runner(std::make_shared<TestRunner1>());
      rc.add_runner(std::make_shared<TestRunner1>());
      rc.add_runner(std::make_shared<TestRunner1>());
      rc.add_runner(std::make_shared<TestRunner1>());
      rc.run_for(std::chrono::milliseconds(10));
      REQUIRE(rc.winner() != nullptr);
    }
  }  // namespace detail
}  // namespace libsemigroups
