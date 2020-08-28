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

#include "catch.hpp"                           // for REQUIRE
#include "libsemigroups/element-adapters.hpp"  // for Product
#include "libsemigroups/element.hpp"           // for Transformation
#include "libsemigroups/pool.hpp"              // for Pool
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  namespace detail {
    LIBSEMIGROUPS_TEST_CASE("Pool",
                            "000",
                            "initial",
                            "[quick][transformation]") {
      Pool<Transformation<size_t>*> cache;
      REQUIRE_THROWS_AS(cache.acquire(), LibsemigroupsException);
      Transformation<size_t> t({0, 1, 3, 2});
      cache.push(&t, 1);
      Transformation<size_t>& x = *cache.acquire();
      REQUIRE_THROWS_AS(cache.acquire(), LibsemigroupsException);
      cache.push(&t, 1);
      REQUIRE_NOTHROW(cache.acquire());
      x.increase_degree_by(10);
      cache.release(&x);
    }

    LIBSEMIGROUPS_TEST_CASE("Pool",
                            "001",
                            "boolean matrices",
                            "[quick][boolmat]") {
      Pool<BooleanMat*> cache;
      REQUIRE_THROWS_AS(cache.acquire(), LibsemigroupsException);
      BooleanMat* b = new BooleanMat({{0, 1, 0}, {1, 1, 1}, {0, 0, 1}});
      cache.push(b, 1);
      BooleanMat* tmp = cache.acquire();
      REQUIRE_THROWS_AS(cache.acquire(), LibsemigroupsException);
      cache.release(tmp);
      tmp = cache.acquire();
      REQUIRE_THROWS_AS(cache.acquire(), LibsemigroupsException);
      cache.release(tmp);
      delete b;
    }

    LIBSEMIGROUPS_TEST_CASE("Pool", "002", "PoolGuard", "[quick][boolmat]") {
      Pool<BooleanMat*> cache;
      REQUIRE_THROWS_AS(cache.acquire(), LibsemigroupsException);
      BooleanMat* b = new BooleanMat({{0, 1, 0}, {1, 1, 1}, {0, 0, 1}});
      cache.push(b, 2);
      {
        PoolGuard<BooleanMat*> cg1(cache);
        BooleanMat*            tmp1 = cg1.get();
        REQUIRE(b != tmp1);
        {
          PoolGuard<BooleanMat*> cg2(cache);
          BooleanMat*            tmp2 = cg2.get();
          REQUIRE_THROWS_AS(cache.acquire(), LibsemigroupsException);
          REQUIRE_THROWS_AS(PoolGuard<BooleanMat*>(cache),
                            LibsemigroupsException);
          REQUIRE(tmp1 != tmp2);
          REQUIRE(b != tmp2);
        }
      }
      delete b;
    }

    LIBSEMIGROUPS_TEST_CASE("Pool",
                            "003",
                            "transformation products",
                            "[quick][transformation]") {
      Pool<Transformation<size_t>*> cache;
      Transformation<size_t>        t({0, 1, 3, 2, 5, 7, 3, 4});
      cache.push(&t, 5);
      Transformation<size_t>* x = cache.acquire();
      Transformation<size_t>* y = cache.acquire();
      REQUIRE(x != y);
      REQUIRE(&t != x);
      REQUIRE(&t != y);
      *y = t;
      Product<Transformation<size_t>*>()(x, &t, y);
      REQUIRE(*x == t * t);
      REQUIRE(&t != x);
      REQUIRE(&t != y);
      Product<Transformation<size_t>*>()(y, &t, x);
      REQUIRE(*y == t * t * t);
    }
  }  // namespace detail
}  // namespace libsemigroups
