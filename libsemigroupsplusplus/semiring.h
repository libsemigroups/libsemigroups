//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#ifndef SEMIGROUPSPLUSPLUS_SEMIRING_H_
#define SEMIGROUPSPLUSPLUS_SEMIRING_H_

#include <limits.h>

#include <algorithm>
#include <cstdint>

namespace semigroupsplusplus {

  // Abstract
  // A *semiring* is a set *R* together with two binary operations + and x
  // (called *addition* and *multiplication*) such that *(R, +)* is a
  // commutative monoid with identity *0*, *(R, x)* is a monoid with identity
  // *1*,
  // and the following hold:
  //
  // * Mulitplication is left and right distributive over addition, i.e.
  //   *a x (b + c) = a x b + a x c* and *(a + b) x c = (a x c) + (b x c)*
  //   for all *a,b,c* in *R*;
  //
  // * Multiplication by *0* annihilates *R*:
  //   *0 x a = a x 0* for all *R*.
  //
  // More information about semirings can be found on
  // [Wikipedia](https://en.wikipedia.org/wiki/Semiring).
  //
  // This class its subclasses provide very basic functionality for creating
  // semirings.

  class Semiring {
   public:
    //
    // A default destructor.
    virtual ~Semiring() {}

    // Semiring multiplicative identity.
    // Method for finding the multiplicative identity, or one, of the
    // semiring.
    //
    // @return the one of the semiring.
    virtual int64_t one() const = 0;

    // Semiring additive identity.
    // Method for finding the additive identity, or zero, of the
    // semiring.
    //
    // @return the zero of the semiring.
    virtual int64_t zero() const = 0;

    // Addition in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // Method for finding the sum of two elements in the
    // semiring.
    // @return the sum of x and y in the semiring.
    virtual int64_t plus(int64_t x, int64_t y) const = 0;

    // Multiplication in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // Method for finding the product of two elements in the
    // semiring.
    // @return the product of x and y in the semiring.
    virtual int64_t prod(int64_t x, int64_t y) const = 0;

    // Threshold of the semiring.
    //
    // Method for finding the threshold of a semiring. The default value is
    // -1 (undefined).
    // @return -1
    virtual int64_t threshold() const {
      return -1;
    }

    // Period of the semiring.
    //
    // Method for finding the period of a semiring. The default value is
    // -1 (undefined).
    // @return -1
    virtual int64_t period() const {
      return -1;
    }
  };

  // Non-abstract
  // The usual ring of integers.
  //
  // This class implements ring of integers.
  class Integers : public Semiring {
   public:
    // Default
    Integers() : Semiring() {}

    // Multiplicative identity.
    // This method returns the multiplicative identity, or one, of the ring.
    //
    // @return the integer 1.
    int64_t one() const override {
      return 1;
    }

    // Additive identity.
    // This method returns the additive identity, or zero, of the ring.
    //
    // @return the integer 0.
    int64_t zero() const override {
      return 0;
    }

    // Multiplication in the integers.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return the product x \* y.
    int64_t prod(int64_t x, int64_t y) const override {
      return x * y;
    }

    // Addition in the prime field.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return sum x + y.
    int64_t plus(int64_t x, int64_t y) const override {
      return x + y;
    }
  };

  // Non-abstract
  //
  // The *max-plus semiring* consists of the set of natural numbers together
  // with negative infinity with operations max and plus. Negative infinity is
  // represented by int64_t_MIN.
  class MaxPlusSemiring : public Semiring {
   public:
    // Default
    MaxPlusSemiring() : Semiring() {}

    // Semiring multiplicative identity.
    // This method returns the multiplicative identity, or one, of the
    // max-plus semiring.
    //
    // @return the integer 0.
    int64_t one() const override {
      return 0;
    }

    // Semiring additive identity.
    // This method returns the additive identity, or zero, of the
    // max-plus semiring.
    //
    // @return int64_t_MIN.
    int64_t zero() const override {
      return LONG_MIN;
    }

    // Multiplication in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return LONG_MIN if x or y equals LONG_MIN, otherwise return x + y.
    int64_t prod(int64_t x, int64_t y) const override {
      if (x == LONG_MIN || y == LONG_MIN) {
        return LONG_MIN;
      }
      return x + y;
    }

    // Addition in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return the maximum of x and y.
    int64_t plus(int64_t x, int64_t y) const override {
      return std::max(x, y);
    }
  };

  // Non-abstract
  //
  // The *min-plus semiring* consists of the set of natural numbers together
  // with infinity with operations min and plus. Infinity is
  // represented by LONG_MAX.

  class MinPlusSemiring : public Semiring {
   public:
    // Default
    MinPlusSemiring() : Semiring() {}

    // Semiring multiplicative identity.
    // This method returns the multiplicative identity, or one, of the
    // min-plus semiring.
    //
    // @return the integer 0.
    int64_t one() const override {
      return 0;
    }

    // Semiring additive identity.
    // This method returns the additive identity, or zero, of the
    // min-plus semiring.
    //
    // @return LONG_MAX.
    int64_t zero() const override {
      return LONG_MAX;
    }

    // Multiplication in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return LONG_MAX if x or y equals LONG_MAX, otherwise return x + y.
    int64_t prod(int64_t x, int64_t y) const override {
      if (x == LONG_MAX || y == LONG_MAX) {
        return LONG_MAX;
      }
      return x + y;
    }

    // Addition in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return the minimum of x and y.
    int64_t plus(int64_t x, int64_t y) const override {
      return std::min(x, y);
    }
  };

  // Abstract
  // Tropical semiring base class.
  //
  // This class provides common methods for its subclasses
  // <TropicalMaxPlusSemiring> and <TropicalMinPlusSemiring>.
  class TropicalSemiring : public Semiring {
   public:
    // Default
    // @threshold the largest integer in the semiring (or equivalently the
    // size of the semiring minus 2).
    explicit TropicalSemiring(int64_t threshold)
        : Semiring(), _threshold(threshold) {}

    // Threshold of a tropical semiring.
    // This is the largest non-negative integer in the semiring, called the
    // **threshold**.
    //
    // @return the threshold of the semiring.
    int64_t threshold() const override {
      return _threshold;
    }

   private:
    int64_t _threshold;
  };

  // Non-abstract
  // Tropical max-plus semiring.
  //
  // The **tropical max-plus semiring** consists of the integers *0, ... , t*
  // for some value *t* (called the **threshold** of the semiring) and negative
  // infinity. Negative infinity is represented by LONG_MIN.
  class TropicalMaxPlusSemiring : public TropicalSemiring {
   public:
    // Default
    // @threshold the largest integer in the semiring (or equivalently the
    // size of the semiring minus 2).
    explicit TropicalMaxPlusSemiring(int64_t threshold)
        : TropicalSemiring(threshold) {}

    // Semiring multiplicative identity.
    // This method returns the multiplicative identity, or one, of the
    // tropical max-plus semiring.
    //
    // @return the integer 0.
    int64_t one() const override {
      return 0;
    }

    // Semiring additive identity.
    // This method returns the additive identity, or zero, of the
    // tropical max-plus semiring.
    //
    // @return the integer LONG_MIN.
    int64_t zero() const override {
      return LONG_MIN;
    }

    // Multiplication in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return LONG_MIN if x or y equals LONG_MIN, otherwise return the
    // minimum of x + y and the threshold of the semiring.
    int64_t prod(int64_t x, int64_t y) const override {
      if (x == LONG_MIN || y == LONG_MIN) {
        return LONG_MIN;
      }
      return std::min(x + y, threshold());
    }

    // Multiplication in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return the minimum of (the maximum of x and y) and the threshold of
    // the semiring.
    int64_t plus(int64_t x, int64_t y) const override {
      return std::min((std::max(x, y)), threshold());
    }
  };

  // Non-abstract
  // Tropical min-plus semiring.
  //
  // The **tropical min-plus semiring** consists of the integers *0, ... , t*
  // for some value *t* (called the **threshold** of the semiring) and
  // infinity. Infinity is represented by LONG_MAX.
  class TropicalMinPlusSemiring : public TropicalSemiring {
   public:
    // Default
    // @threshold the largest integer in the semiring (or equivalently the
    // size of the semiring minus 2).
    explicit TropicalMinPlusSemiring(int64_t threshold)
        : TropicalSemiring(threshold) {}

    // Semiring multiplicative identity.
    // This method returns the multiplicative identity, or one, of the
    // tropical min-plus semiring.
    //
    // @return the integer 0.
    int64_t one() const override {
      return 0;
    }

    // Semiring additive identity.
    // This method returns the additive identity, or zero, of the
    // tropical min-plus semiring.
    //
    // @return the integer LONG_MAX.
    int64_t zero() const override {
      return LONG_MAX;
    }

    // Multiplication in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return LONG_MAX if x or y equals LONG_MAX, otherwise return the
    // minimum of x + y and the threshold of the semiring.
    int64_t prod(int64_t x, int64_t y) const override {
      if (x == LONG_MAX || y == LONG_MAX) {
        return LONG_MAX;
      }
      return std::min(x + y, threshold());
    }

    // Multiplication in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return LONG_MAX if either of x and y is LONG_MAX, and otherwise the
    // minimum of x, y, and the threshold of the semiring.
    int64_t plus(int64_t x, int64_t y) const override {
      if (x == LONG_MAX && y == LONG_MAX) {
        return LONG_MAX;
      }
      return std::min((std::min(x, y)), threshold());
    }
  };

  // Non-abstract
  // Semiring of natural numbers mod *t*, *p*.
  //
  // This class implements the *semiring* consisting of *0, 1, ..., t, t + 1,
  // ..., t + p - 1* for some **threshold** *t* and **period** *p* with
  // operations addition and multiplication modulo the congruence *t = t + p*.

  class NaturalSemiring : public Semiring {
   public:
    // Default
    // @threshold the threshold, should be positive, this is not checked.
    // @period    the period, should be non-negative, this is not checked.
    NaturalSemiring(int64_t threshold, int64_t period)
        : Semiring(), _threshold(threshold), _period(period) {}

    // Semiring multiplicative identity.
    // This method returns the multiplicative identity, or one, of the
    // semiring.
    //
    // @return the integer 1.
    int64_t one() const override {
      return 1;
    }

    // Semiring additive identity.
    // This method returns the additive identity, or zero, of the
    // semiring.
    //
    // @return the integer 0.
    int64_t zero() const override {
      return 0;
    }

    // Multiplication in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return x \* y modulo the congruence *t = t + p* where *t* and *p* are
    // the threshold and period of the semiring, respectively.
    int64_t prod(int64_t x, int64_t y) const override {
      return thresholdperiod(x * y);
    }

    // Addition in the semiring.
    // @x any int64_t int
    // @y any int64_t int
    //
    // @return x + y modulo the congruence *t = t + p* where *t* and *p* are
    // the threshold and period of the semiring, respectively.
    int64_t plus(int64_t x, int64_t y) const override {
      return thresholdperiod(x + y);
    }

    // Threshold of the semiring.
    // The semiring consists of *0, 1, ..., t, t + 1,
    // ..., t + p - 1* for some **threshold** *t* and **period** *p* with
    // operations addition and multiplication modulo the congruence *t = t + p*.
    //
    // @return the threshold of the semiring.
    int64_t threshold() const override {
      return _threshold;
    }

    // Period of the semiring.
    // The semiring consists of *0, 1, ..., t, t + 1,
    // ..., t + p - 1* for some **threshold** *t* and **period** *p* with
    // operations addition and multiplication modulo the congruence *t = t + p*.
    //
    // @return the period of the semiring.
    int64_t period() const override {
      return _period;
    }

   private:
    int64_t thresholdperiod(int64_t x) const {
      if (x > _threshold) {
        return _threshold + (x - _threshold) % _period;
      }
      return x;
    }

    int64_t _threshold;
    int64_t _period;
  };
}  // namespace semigroupsplusplus

#endif  // SEMIGROUPSPLUSPLUS_SEMIRING_H_
