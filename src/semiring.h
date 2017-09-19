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

#include <limits.h>

#include <algorithm>
#include <cstdint>

#include "libsemigroups-debug.h"

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
  template <typename T> class Semiring {
   public:
    //! Value representing an undefined quantity.
    static const T UNDEFINED;

    //! Value representing \f$-\infty\f$.
    static const T MINUS_INFTY;

    //! Value representing \f$\infty\f$.
    static const T INFTY;

    //! A default destructor.
    virtual ~Semiring() {}

    //! Returns the multiplicative identity, or one, of the semiring.
    virtual T one() const = 0;

    //! Returns the additive identity, or zero, of the semiring.
    virtual T zero() const = 0;

    //! Returns the sum of \p x and \p y.
    virtual T plus(T x, T y) const = 0;

    //! Returns the product of \p x and \p y.
    virtual T prod(T x, T y) const = 0;
  };

  template <typename T>
  const T Semiring<T>::UNDEFINED = std::numeric_limits<T>::max();

  template <typename T>
  const T Semiring<T>::MINUS_INFTY = std::numeric_limits<T>::min();

  template <typename T>
  const T Semiring<T>::INFTY = std::numeric_limits<T>::max();

  //! The usual Boolean semiring.
  class BooleanSemiring : public Semiring<bool> {
   public:
    BooleanSemiring() : Semiring() {}

    //! Returns the integer 1.
    bool one() const override {
      return true;
    }

    //! Returns the integer 0.
    bool zero() const override {
      return false;
    }

    //! Returns the product \f$xy\f$.
    bool prod(bool x, bool y) const override {
      return x && y;
    }

    //! Returns the sum \f$x + y\f$.
    bool plus(bool x, bool y) const override {
      return x || y;
    }
  };

  //! The usual ring of integers.
  class Integers : public Semiring<int64_t> {
   public:
    Integers() : Semiring<int64_t>() {}

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
  //! by Semiring<int64_t>::MINUS_INFTY.
  class MaxPlusSemiring : public Semiring<int64_t> {
   public:
    MaxPlusSemiring() : Semiring<int64_t>() {}

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns Semiring<int64_t>::MINUS_INFTY.
    int64_t zero() const override {
      return MINUS_INFTY;
    }

    //! Returns Semiring<int64_t>::MINUS_INFTY if \p x or \p y equals
    //! Semiring<int64_t>::MINUS_INFTY, otherwise returns \p x + \p y.
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
  //! Semiring<int64_t>::INFTY.
  class MinPlusSemiring : public Semiring<int64_t> {
   public:
    MinPlusSemiring() : Semiring<int64_t>() {}

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns Semiring<int64_t>::INFTY.
    int64_t zero() const override {
      return INFTY;
    }

    //! Returns Semiring<int64_t>::INFTY if \p x or \p y equals
    //! Semiring<int64_t>::INFTY,
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
  class SemiringWithThreshold : public Semiring<int64_t> {
   public:
    //! A class for semirings with a threshold.
    //!
    //! The threshold of a semiring is related to the largest finite value in
    //! the semiring.
    explicit SemiringWithThreshold(int64_t threshold)
        : Semiring<int64_t>(), _threshold(threshold) {}

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
  //! represented by Semiring<int64_t>::MINUS_INFTY.
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

    //! Returns the Semiring<int64_t>::MINUS_INFTY.
    int64_t zero() const override {
      return MINUS_INFTY;
    }

    //! Returns Semiring<int64_t>::MINUS_INFTY if \p x or \p y equals
    //! Semiring<int64_t>::MINUS_INFTY, otherwise returns the minimum of \p x +
    //! \p y and
    //! the threshold of the semiring.
    int64_t prod(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                           || x == Semiring<int64_t>::MINUS_INFTY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                           || y == Semiring<int64_t>::MINUS_INFTY);
      if (x == MINUS_INFTY || y == MINUS_INFTY) {
        return MINUS_INFTY;
      }
      return std::min(x + y, threshold());
    }

    //! Returns the minimum of (the maximum of \p x and \p y) and the threshold
    //! of the semiring.
    int64_t plus(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                           || x == Semiring<int64_t>::MINUS_INFTY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                           || y == Semiring<int64_t>::MINUS_INFTY);
      return std::max(x, y);
    }
  };

  //! The **tropical min-plus semiring** consists of the integers
  //! \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the **threshold**
  //! of the semiring) and \f$\infty\f$. Infinity is represented
  //! by Semiring<int64_t>::INFTY.
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

    //! Returns the Semiring<int64_t>::INFTY.
    int64_t zero() const override {
      return INFTY;
    }

    //! Returns Semiring<int64_t>::INFTY if \p x or \p y equals
    //! Semiring<int64_t>::INFTY, otherwise return the minimum of \p x + \p y
    //! and the threshold of the semiring.
    int64_t prod(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                           || x == Semiring<int64_t>::INFTY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                           || y == Semiring<int64_t>::INFTY);
      if (x == INFTY || y == INFTY) {
        return INFTY;
      }
      return std::min(x + y, threshold());
    }

    //! Returns Semiring<int64_t>::INFTY if either of \p x and \p y is
    //! Semiring<int64_t>::INFTY, and otherwise the minimum of x, y, and the
    //! threshold of the semiring.
    int64_t plus(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                           || x == Semiring<int64_t>::INFTY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                           || y == Semiring<int64_t>::INFTY);
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
    //! This method constructs a semiring whose elements are
    //! \f$\{0, 1, ..., t, t +  1, ..., t + p - 1\}\f$
    //! with operations addition and multiplication modulo the congruence
    //! \f$t = t + p\f$.
    //!
    //! The parameter \p t should be greater than or equal to 0, and the
    //! parameter \p p must be strictly greater than 0, both which are asserted
    //! in the constructor.
    NaturalSemiring(int64_t t, int64_t p)
        : SemiringWithThreshold(t), _period(p) {
      LIBSEMIGROUPS_ASSERT(_period > 0);
      LIBSEMIGROUPS_ASSERT(this->threshold() >= 0);
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
      LIBSEMIGROUPS_ASSERT(x >= 0 && x <= _period + this->threshold() - 1);
      LIBSEMIGROUPS_ASSERT(y >= 0 && y <= _period + this->threshold() - 1);
      return thresholdperiod(x * y);
    }

    //! Returns \p x + \p y modulo the congruence \f$t = t + p\f$ where \f$t\f$
    //! and \f$p\f$ are the threshold and period of the semiring, respectively.
    int64_t plus(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT(x >= 0 && x <= _period + this->threshold() - 1);
      LIBSEMIGROUPS_ASSERT(y >= 0 && y <= _period + this->threshold() - 1);
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
