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

#include "bench-main.hpp"  // for LIBSEMIGROUPS_BENCHMARK
#include "catch.hpp"       // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS

#include "libsemigroups/froidure-pin-base.hpp"
#include "libsemigroups/froidure-pin.hpp"
#include "libsemigroups/types.hpp"

#include "examples/generators.hpp"

namespace libsemigroups {
  template <typename T>
  FroidurePin<T>* before_bench(Generators<T> const& p) {
    auto fp = make<FroidurePin<T>>(p);
    return fp;
  }

  template <typename T>
  void bench_run(FroidurePin<T>* fp, size_t) {
    fp->run();
    REQUIRE(fp->finished());
  }

  template <typename T>
  void after_bench(FroidurePin<T>* fp) {
    delete fp;
  }

  template <typename T>
  FroidurePin<T>* before_bench_rules(Generators<T> const& p) {
    auto fp = before_bench(p);
    fp->run();
    REQUIRE(fp->finished());
    return fp;
  }

  template <typename T>
  void bench_const_rule_iterator(FroidurePin<T>* fp, size_t) {
    std::vector<relation_type> v(fp->cbegin_rules(), fp->cend_rules());
    REQUIRE(v.size() == fp->nr_rules());
  }

  template <typename T>
  void bench_relations(FroidurePin<T>* fp, size_t) {
    std::vector<relation_type> v;
    relations(*fp, [&v](word_type l, word_type r) -> void {
      v.emplace_back(std::move(l), std::move(r));
    });
    REQUIRE(v.size() == fp->nr_rules());
  }

  using Transf = typename TransfHelper<16>::type;

  LIBSEMIGROUPS_BENCHMARK("FroidurePin<Transf>",
                          "[FroidurePin][001]",
                          before_bench<Transf>,
                          bench_run<Transf>,
                          after_bench<Transf>,
                          transf_examples());

  LIBSEMIGROUPS_BENCHMARK("cbegin/end_rules",
                          "[FroidurePin][002]",
                          before_bench_rules<Transf>,
                          bench_const_rule_iterator<Transf>,
                          after_bench<Transf>,
                          {transf_examples(0x9806816B9D761476)});

  LIBSEMIGROUPS_BENCHMARK("relations",
                          "[FroidurePin][003]",
                          before_bench_rules<Transf>,
                          bench_relations<Transf>,
                          after_bench<Transf>,
                          {transf_examples(0x9806816B9D761476)});

}  // namespace libsemigroups
