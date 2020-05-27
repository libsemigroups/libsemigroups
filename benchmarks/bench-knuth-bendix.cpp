//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

#include "bench-main.hpp"  // for LIBSEMIGROUPS_TEST_CASE
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "examples/fpsemi-intf.hpp"
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"  // for KnuthBendix, operator<<

namespace libsemigroups {
  namespace fpsemigroup {
    namespace {
      KnuthBendix* before_normal_forms(FpSemiIntfArgs const& p) {
        auto kb = make<KnuthBendix>(p);
        kb->run();
        REQUIRE(kb->confluent());
        // REQUIRE(!kb->is_finite());
        return kb;
      }

      void bench_normal_forms(KnuthBendix* kb, size_t id) {
        static std::unordered_map<size_t, size_t> data
            = {{0x6BA1D9229DC22AFB, 4000}, {0x222CC398A1FBBC47, 170},
               {0xB558B78731C80849, 140},  {0xB3E816A734D092E0, 12},
               {0x88F91469E1FD9427, 220},  {0xE98EAA9E59EADB12, 2000},
               {0x9AB44C55E7EF3FE6, 2000}, {0x108096C3079B0302, 9},
               {0x327F33C8DC363DEA, 19},   {0x9E13A24681757CC, 6},
               {0x84B2E77D4EA9AEFF, 6},    {0x5AFDDD8E430D1787, 6},
               {0xC24BAB32F318E8B9, 6},    {0x6578EB84FB90291, 5},
               {0xB31C21EC18298FC5, 8},    {0x777131204E94A930, 11},
               {0x7FC4713FF479BDD8, 3000}, {0xA5CA113505A6EEFB, 9},
               {0xCEA8D789D5C595C2, 6},    {0x1565E7D947EC2828, 23}};
        auto ptr = kb->froidure_pin();
        ptr->run_until([&ptr, &id]() -> bool {
          return ptr->current_max_word_length() >= data.find(id)->second;
        });
      }
      void after_normal_forms(KnuthBendix* tc) {
        delete tc;
      }
    }  // namespace

    // std::vector<FpSemiIntfArgs> subset =
    // {string_infinite_examples(0x1565E7D947EC2828)};

    LIBSEMIGROUPS_BENCHMARK("Shortlex normal forms 1",
                            "[KnuthBendix][normal_forms_short_lex][quick]",
                            before_normal_forms,
                            bench_normal_forms,
                            after_normal_forms,
                            fpsemigroup::infinite_examples());
    // subset);

  }  // namespace fpsemigroup
}  // namespace libsemigroups
