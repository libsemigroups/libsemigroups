//
// libsemigroups - C++ library for semigroups and monoids
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

#ifndef LIBSEMIGROUPS_SRC_SEMIRING_H_
#define LIBSEMIGROUPS_SRC_SEMIRING_H_

#include <assert.h>
#include <limits.h>

#include <algorithm>
#include <cstdint>

namespace libsemigroups {

  //! This class its subclasses provide very basic functionality for creating
  //! semirings.
  //!
  //! A *semiring* is a set \f$R\f$ together with two binary operations \f$+\f$
  //! and \f$\times\f$ (called *addition* and *multiplication*) such that
  //! \f$(R, +)\f$ is a commutative monoid with identity \f$0\f$, \f$(R,
  //! \times)\f$ is a monoid with identity \f$1\f$, and the following hold:
  //!
  //! * Mulitplication is left and right distributive over addition, i.e.
  //! \f$a \times (b + c) = a \times b + a \times c\f$ and \f$(a + b)
  //! \times c = (a \times c) + (b \times c)\f$ for all \f$a,b,c\in R\f$;
  //!
  //! * Multiplication by \f$0\f$ annihilates \f$R\f$:
  //!   \f$0 \times a = a \times 0\f$ for all \f$R\f$.
  //!
  //! More information about semirings can be found on
  //! [Wikipedia](https://en.wikipedia.org/wiki/Semiring).
  class Semiring {
   public:
    //! Value representing an undefined quantity.
    static const int64_t UNDEFINED = -1;

    //! Value representing \f$-\infty\f$.
    static const int64_t MINUS_INFTY = std::numeric_limits<int64_t>::min();

    //! Value representing \f$\infty\f$.
    static const int64_t INFTY = std::numeric_limits<int64_t>::max();

    //! A default destructor.
    virtual ~Semiring() {}

    //! Returns the multiplicative identity, or one, of the semiring.
    virtual int64_t one() const = 0;

    //! Returns the additive identity, or zero, of the semiring.
    virtual int64_t zero() const = 0;

    //! Returns the sum of \p x and \p y.
    virtual int64_t plus(int64_t x, int64_t y) const = 0;

    //! Returns the product of \p x and \p y.
    virtual int64_t prod(int64_t x, int64_t y) const = 0;
  };

  //! The usual ring of integers.
  class Integers : public Semiring {
   public:
    Integers() : Semiring() {}

    //! Returns the integer 1.
    int64_t one() const override {
      return 1;
    }

    //! Returns the integer 0.
    int64_t zero() const override {
      return 0;
    }

    //! Returns the product \f$xy\f$.
    int64_t prod(int64_t x, int64_t y) const override {
      return x * y;
    }

    //! Returns the sum \f$x + y\f$.
    int64_t plus(int64_t x, int64_t y) const override {
      return x + y;
    }
  };

  //! The *max-plus semiring* consists of the integers together with negative
  //! infinity with operations max and plus. Negative infinity is represented
  //! by Semiring::MINUS_INFTY.
  class MaxPlusSemiring : public Semiring {
   public:
    MaxPlusSemiring() : Semiring() {}

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns Semiring::MINUS_INFTY.
    int64_t zero() const override {
      return MINUS_INFTY;
    }

    //! Returns Semiring::MINUS_INFTY if \p x or \p y equals
    //! Semiring::MINUS_INFTY, otherwise returns \p x + \p y.
    int64_t prod(int64_t x, int64_t y) const override {
      if (x == MINUS_INFTY || y == MINUS_INFTY) {
        return MINUS_INFTY;
      }
      return x + y;
    }

    //! Returns the maximum of \p x and \p y.
    int64_t plus(int64_t x, int64_t y) const override {
      return std::max(x, y);
    }
  };

  //! The *min-plus semiring* consists of the integers together
  //! with infinity with operations min and plus. Infinity is represented by
  //! Semiring::INFTY.
  class MinPlusSemiring : public Semiring {
   public:
    MinPlusSemiring() : Semiring() {}

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns Semiring::INFTY.
    int64_t zero() const override {
      return INFTY;
    }

    //! Returns Semiring::INFTY if \p x or \p y equals Semiring::INFTY,
    //! otherwise returns \p x + \p y.
    int64_t prod(int64_t x, int64_t y) const override {
      if (x == INFTY || y == INFTY) {
        return INFTY;
      }
      return x + y;
    }

    //! Returns the minimum of \p x and \p y.
    int64_t plus(int64_t x, int64_t y) const override {
      return std::min(x, y);
    }
  };

  //! This abstract class provides common methods for its subclasses
  //! TropicalMaxPlusSemiring, TropicalMinPlusSemiring, and NaturalSemiring.
  class SemiringWithThreshold : public Semiring {
   public:
    //! A class for semirings with a threshold.
    //!
    //! The threshold of a semiring is related to the largest finite value in
    //! the semiring.
    explicit SemiringWithThreshold(int64_t threshold)
        : Semiring(), _threshold(threshold) {}

    //! Returns the threshold of a semiring with threshold.
    int64_t threshold() const {
      return _threshold;
    }

   private:
    int64_t _threshold;
  };

  //! The **tropical max-plus semiring** consists of the integers
  //! \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the
  //! **threshold** of the semiring) and \f$-\infty\f$. Negative infinity is
  //! represented by Semiring::MINUS_INFTY.
  class TropicalMaxPlusSemiring : public SemiringWithThreshold {
   public:
    //! Construct from threshold.
    //!
    //! The threshold is the largest integer in the semiring.
    explicit TropicalMaxPlusSemiring(int64_t threshold)
        : SemiringWithThreshold(threshold) {}

    // Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns the Semiring::MINUS_INFTY.
    int64_t zero() const override {
      return MINUS_INFTY;
    }

    //! Returns Semiring::MINUS_INFTY if \p x or \p y equals
    //! Semiring::MINUS_INFTY, otherwise returns the minimum of \p x + \p y and
    //! the threshold of the semiring.
    int64_t prod(int64_t x, int64_t y) const override {
      assert((x >= 0 && x <= this->threshold()) || x == Semiring::MINUS_INFTY);
      assert((y >= 0 && y <= this->threshold()) || y == Semiring::MINUS_INFTY);
      if (x == MINUS_INFTY || y == MINUS_INFTY) {
        return MINUS_INFTY;
      }
      return std::min(x + y, threshold());
    }

    //! Returns the minimum of (the maximum of \p x and \p y) and the threshold
    //! of the semiring.
    int64_t plus(int64_t x, int64_t y) const override {
      assert((x >= 0 && x <= this->threshold()) || x == Semiring::MINUS_INFTY);
      assert((y >= 0 && y <= this->threshold()) || y == Semiring::MINUS_INFTY);
      return std::max(x, y);
    }
  };

  //! The **tropical min-plus semiring** consists of the integers
  //! \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the **threshold**
  //! of the semiring) and \f$\infty\f$. Infinity is represented
  //! by Semiring::INFTY.
  class TropicalMinPlusSemiring : public SemiringWithThreshold {
   public:
    //! Construct from threshold.
    //!
    //! The threshold is the largest integer in the semiring.
    explicit TropicalMinPlusSemiring(int64_t threshold)
        : SemiringWithThreshold(threshold) {}

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns the Semiring::INFTY.
    int64_t zero() const override {
      return INFTY;
    }

    //! Returns Semiring::INFTY if \p x or \p y equals Semiring::INFTY,
    //! otherwise return the minimum of \p x + \p y and the threshold of the
    //! semiring.
    int64_t prod(int64_t x, int64_t y) const override {
      assert((x >= 0 && x <= this->threshold()) || x == Semiring::INFTY);
      assert((y >= 0 && y <= this->threshold()) || y == Semiring::INFTY);
      if (x == INFTY || y == INFTY) {
        return INFTY;
      }
      return std::min(x + y, threshold());
    }

    //! Returns Semiring::INFTY if either of \p x and \p y is Semiring::INFTY,
    //! and otherwise the minimum of x, y, and the threshold of the semiring.
    int64_t plus(int64_t x, int64_t y) const override {
      assert((x >= 0 && x <= this->threshold()) || x == Semiring::INFTY);
      assert((y >= 0 && y <= this->threshold()) || y == Semiring::INFTY);
      if (x == INFTY && y == INFTY) {
        return INFTY;
      }
      return std::min(x, y);
    }
  };

  //! This class implements the *semiring* consisting of
  //! \f$\{0, 1, ..., t, t +  1, ..., t + p - 1\}\f$ for some **threshold**
  //! \f$t\f$ and **period** \f$p\f$ with operations addition and
  //! multiplication modulo the congruence \f$t = t + p\f$.
  class NaturalSemiring : public SemiringWithThreshold {
   public:
    //! Construct from threshold and period.
    //!
    //! This method constructs a Semiring whose elements are
    //! \f$\{0, 1, ..., t, t +  1, ..., t + p - 1\}\f$
    //! with operations addition and multiplication modulo the congruence
    //! \f$t = t + p\f$.
    //!
    //! The parameter \p t should be greater than or equal to 0, and the
    //! parameter \p p must be strictly greater than 0, both which are asserted
    //! in the constructor.
    NaturalSemiring(int64_t t, int64_t p)
        : SemiringWithThreshold(t), _period(p) {
      assert(_period > 0);
      assert(this->threshold() >= 0);
    }

    //! Return the integer 1.
    int64_t one() const override {
      return 1;
    }

    //! Return the integer 0.
    int64_t zero() const override {
      return 0;
    }

    //! Returns \p x * \p y modulo the congruence \f$t = t + p\f$ where \f$t\f$
    //! and \f$p\f$ are the threshold and period of the semiring, respectively.
    int64_t prod(int64_t x, int64_t y) const override {
      assert(x >= 0 && x <= _period + this->threshold() - 1);
      assert(y >= 0 && y <= _period + this->threshold() - 1);
      return thresholdperiod(x * y);
    }

    //! Returns \p x + \p y modulo the congruence \f$t = t + p\f$ where \f$t\f$
    //! and \f$p\f$ are the threshold and period of the semiring, respectively.
    int64_t plus(int64_t x, int64_t y) const override {
      assert(x >= 0 && x <= _period + this->threshold() - 1);
      assert(y >= 0 && y <= _period + this->threshold() - 1);
      return thresholdperiod(x + y);
    }

    //! Returns the period of the semiring.
    int64_t period() const {
      return _period;
    }

   private:
    int64_t thresholdperiod(int64_t x) const {
      int64_t threshold = this->threshold();
      if (x > threshold) {
        return threshold + (x - threshold) % _period;
      }
      return x;
    }

    int64_t _period;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SRC_SEMIRING_H_
