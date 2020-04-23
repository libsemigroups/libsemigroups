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

#include "generators.hpp"
#include "../bench-main.hpp"  // for to_hex_string
#include "common.hpp"

#include "libsemigroups/element-helper.hpp"
#include "libsemigroups/libsemigroups-config.hpp"

namespace libsemigroups {
#ifndef LIBSEMIGROUPS_HPCOMBI
  static_assert(false, "HPCombi must be enabled for benchmarks");
#endif

  using Transf = typename TransfHelper<16>::type;

  namespace {
    template <typename T>
    size_t create_id(Generators<T> const& x) {
      return detail::hash_combine(x.gens);
    }
  }  // namespace

  Generators<Transf> full_transf_monoid(size_t n) {
    if (n > 16 || n < 2) {
      LIBSEMIGROUPS_EXCEPTION(
          "the first argument must be in the range [2, 17), found %d", n);
    }

    Transf transpo;
    transpo[0] = 1;
    transpo[1] = 0;
    std::iota(transpo.begin() + 2, transpo.end(), 2);

    Transf cycle;
    std::iota(cycle.begin(), cycle.begin() + n - 1, 1);
    cycle[n - 1] = 0;
    std::iota(cycle.begin() + n, cycle.end(), n);

    Transf singular;
    singular[0] = 0;
    std::iota(singular.begin() + 1, singular.begin() + n, 0);
    std::iota(singular.begin() + n, singular.end(), n);

    Generators<Transf> out
        = {.id   = 0,
           .name = "full transformation monoid n = " + std::to_string(n),
           .gens = {transpo, cycle, singular}};
    out.id = create_id(out);
    return out;
  }

  std::vector<Generators<Transf>> const& transf_examples() {
    static std::vector<Generators<Transf>> examples = {
        full_transf_monoid(3),
        full_transf_monoid(4),
        full_transf_monoid(5),
        full_transf_monoid(6),
        {.id   = 0,
         .name = "FroidurePin 029",
         .gens = {Transf({0, 1, 2, 3, 4, 5}),
                  Transf({1, 0, 2, 3, 4, 5}),
                  Transf({4, 0, 1, 2, 3, 5}),
                  Transf({5, 1, 2, 3, 4, 5}),
                  Transf({1, 1, 2, 3, 4, 5})}},
        {.id   = 1,
         .name = "sporadic example 1",
         .gens = {Transf({9, 7, 3, 5, 3, 4, 2, 7, 7, 1}),
                  Transf({2, 4, 4, 7, 3, 0, 7, 0, 8, 6})}},
        {.id   = 2,
         .name = "sporadic example 2",
         .gens = {Transf({0, 7, 0, 9, 2, 4, 3, 2, 9, 5}),
                  Transf({3, 1, 0, 6, 5, 7, 8, 5, 0, 2})}},
        {.id   = 3,
         .name = "sporadic example 3",
         .gens = {Transf({8, 6, 1, 9, 7, 9, 4, 7, 4, 5}),
                  Transf({2, 4, 4, 8, 5, 0, 6, 8, 9, 2})}},
        {.id   = 4,
         .name = "sporadic example 4",
         .gens = {Transf({8, 1, 9, 7, 1, 1, 6, 4, 5, 7}),
                  Transf({3, 5, 4, 5, 0, 9, 7, 6, 4, 0})}},
        {.id   = 5,
         .name = "sporadic example 5",
         .gens = {Transf({7, 9, 1, 4, 6, 7, 9, 9, 8, 3}),
                  Transf({4, 2, 6, 4, 7, 7, 7, 8, 3, 1})}},
        {.id   = 6,
         .name = "sporadic example 6",
         .gens = {Transf({3, 5, 6, 0, 5, 8, 7, 6, 4, 2}),
                  Transf({0, 6, 2, 1, 8, 5, 1, 2, 6, 5})}},
        {.id   = 7,
         .name = "sporadic example 7",
         .gens = {Transf({0, 9, 6, 8, 7, 8, 1, 9, 2, 7}),
                  Transf({3, 1, 2, 9, 8, 7, 9, 8, 6, 6})}},
        {.id   = 8,
         .name = "sporadic example 8",
         .gens = {Transf({3, 0, 9, 9, 8, 1, 5, 4, 7, 7}),
                  Transf({2, 8, 9, 3, 0, 6, 6, 1, 4, 8})}},
        // {.id   = 9,
        //  .name = "sporadic example 9",
        //  .gens = {Transf({4, 2, 9, 3, 6, 7, 8, 1, 1, 0}),
        //           Transf({6, 8, 1, 9, 7, 8, 1, 3, 5, 2})}},
        {.id   = 10,
         .name = "sporadic example 10",
         .gens = {Transf({0, 8, 1, 5, 7, 2, 5, 6, 9, 4}),
                  Transf({7, 2, 9, 5, 7, 7, 7, 6, 9, 6})}},
        {.id   = 0x9806816B9D761476,
         .name = "JDM favourite",
         .gens = {Transf({1, 7, 2, 6, 0, 4, 1, 5}),
                  Transf({2, 4, 6, 1, 4, 5, 2, 7}),
                  Transf({3, 0, 7, 2, 4, 6, 2, 4}),
                  Transf({3, 2, 3, 4, 5, 3, 0, 1}),
                  Transf({4, 3, 7, 7, 4, 5, 0, 4}),
                  Transf({5, 6, 3, 0, 3, 0, 5, 1}),
                  Transf({6, 0, 1, 1, 1, 6, 3, 4}),
                  Transf({7, 7, 4, 0, 6, 4, 1, 7})}},
        full_transf_monoid(7),
    };
    return examples;
  }

  Generators<Transf> const& transf_examples(size_t id) {
    for (auto const& ex : transf_examples()) {
      if (id == ex.id) {
        return ex;
      }
    }
    LIBSEMIGROUPS_EXCEPTION("no example with id %d", id);
  }
}  // namespace libsemigroups
