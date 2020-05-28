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

#ifndef LIBSEMIGROUPS_SEMIRING_HPP_
#define LIBSEMIGROUPS_SEMIRING_HPP_

#include <algorithm>  // for min, max
#include <cinttypes>  // for int64_t

#include "constants.hpp"            // for NEGATIVE_INFINITY
#include "libsemigroups-debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {

  //! Defined in ``semiring.hpp``.
  //!
  //! This class its subclasses provide very basic functionality for creating
  //! semirings.
  //!
  //! A *semiring* is a set \f$R\f$ together with two binary operations \f$+\f$
  //! and \f$\times\f$ (called *addition* and *multiplication*) such that
  //! \f$(R, +)\f$ is a commutative monoid with identity \f$0\f$,
  //! \f$(R, \times)\f$ is a monoid with identity \f$1\f$, and the
  //! following hold:
  //!
  //! * Multiplication is left and right distributive over addition, i.e.
  //! \f$a \times (b + c) = a \times b + a \times c\f$ and
  //! \f$(a + b) \times c = (a \times c) + (b \times c)\f$ for all
  //! \f$a,b,c\in R\f$;
  //!
  //! * Multiplication by \f$0\f$ annihilates \f$R\f$:
  //!   \f$0 \times a = a \times 0\f$ for all \f$R\f$.
  //!
  //! More information about semirings can be found on
  //! [Wikipedia](https://en.wikipedia.org/wiki/Semiring).
  template <typename T>
  class Semiring {
   public:
    //! Default constructor.
    Semiring() = default;

    //! Default copy constructor.
    Semiring(Semiring const&) = default;

    //! Default move constructor.
    Semiring(Semiring&&) = default;

    //! Default copy assignment constructor.
    Semiring& operator=(Semiring const&) = default;

    //! Default move assignment constructor.
    Semiring& operator=(Semiring&&) = default;

    virtual ~Semiring() = default;

    //! Returns the multiplicative identity, or one, of the semiring.
    virtual T one() const = 0;

    //! Returns the additive identity, or zero, of the semiring.
    virtual T zero() const = 0;

    //! Returns the sum, in the semiring, of the parameters.
    virtual T plus(T, T) const = 0;

    //! Returns the product, in the semiring, of the parameters.
    virtual T prod(T, T) const = 0;

    //! Returns \c true if \c this contain the argument.
    //!
    //! This member function returns \c true if the argument is mathematically
    //! contained in \c this. For semirings without threshold, this will always
    //! return true.
    virtual bool contains(T) const {
      return true;
    }
  };

  //! Defined in ``semiring.hpp``.
  //!
  //! The usual Boolean semiring.
  struct BooleanSemiring final : public Semiring<bool> {
    //! Default constructor.
    BooleanSemiring() = default;

    //! Default copy constructor.
    BooleanSemiring(BooleanSemiring const&) = default;

    //! Default move constructor.
    BooleanSemiring(BooleanSemiring&&) = default;

    //! Default copy assignment constructor.
    BooleanSemiring& operator=(BooleanSemiring const&) = default;

    //! Default move assignment constructor.
    BooleanSemiring& operator=(BooleanSemiring&&) = default;

    ~BooleanSemiring() = default;

    bool one() const override {
      return true;
    }

    bool zero() const override {
      return false;
    }

    bool prod(bool x, bool y) const override {
      return x && y;
    }

    bool plus(bool x, bool y) const override {
      return x || y;
    }
  };

  //! Defined in ``semiring.hpp``.
  //!
  //! The usual ring of integers.
  struct Integers final : public Semiring<int64_t> {
    //! Default constructor.
    Integers() = default;

    //! Default copy constructor.
    Integers(Integers const&) = default;

    //! Default move constructor.
    Integers(Integers&&) = default;

    //! Default copy assignment constructor.
    Integers& operator=(Integers const&) = default;

    //! Default move assignment constructor.
    Integers& operator=(Integers&&) = default;

    ~Integers() = default;

    int64_t one() const override {
      return 1;
    }

    int64_t zero() const override {
      return 0;
    }

    int64_t prod(int64_t x, int64_t y) const override {
      return x * y;
    }

    int64_t plus(int64_t x, int64_t y) const override {
      return x + y;
    }
  };

  //! Defined in ``semiring.hpp``.
  //!
  //! The *max-plus semiring* consists of the integers together with negative
  //! infinity with operations max and plus. Negative infinity is represented
  //! by libsemigroups::NEGATIVE_INFINITY.
  struct MaxPlusSemiring final : public Semiring<int64_t> {
    //! Default constructor.
    MaxPlusSemiring() = default;

    //! Default copy constructor.
    MaxPlusSemiring(MaxPlusSemiring const&) = default;

    //! Default move constructor.
    MaxPlusSemiring(MaxPlusSemiring&&) = default;

    //! Default copy assignment constructor.
    MaxPlusSemiring& operator=(MaxPlusSemiring const&) = default;

    //! Default move assignment constructor.
    MaxPlusSemiring& operator=(MaxPlusSemiring&&) = default;

    ~MaxPlusSemiring() = default;

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns libsemigroups::NEGATIVE_INFINITY.
    int64_t zero() const override {
      return NEGATIVE_INFINITY;
    }

    //! Returns libsemigroups::NEGATIVE_INFINITY if either parameter equals
    //! libsemigroups::NEGATIVE_INFINITY, otherwise returns the usual integer
    //! sum of the parameters.
    int64_t prod(int64_t x, int64_t y) const override {
      if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
        return NEGATIVE_INFINITY;
      }
      return x + y;  // TODO(later) could define addition of NEGATIVE_INFINITY
    }

    //! Returns the maximum of the parameters.
    int64_t plus(int64_t x, int64_t y) const override {
      if (x == NEGATIVE_INFINITY) {
        return y;
      } else if (y == NEGATIVE_INFINITY) {
        return x;
      }
      return std::max(x, y);
    }
  };

  //! Defined in ``semiring.hpp``.
  //!
  //! The *min-plus semiring* consists of the integers together
  //! with infinity with operations min and plus. Infinity is represented by
  //! libsemigroups::POSITIVE_INFINITY.
  struct MinPlusSemiring final : public Semiring<int64_t> {
    //! Default constructor.
    MinPlusSemiring() = default;

    //! Default copy constructor.
    MinPlusSemiring(MinPlusSemiring const&) = default;

    //! Default move constructor.
    MinPlusSemiring(MinPlusSemiring&&) = default;

    //! Default copy assignment constructor.
    MinPlusSemiring& operator=(MinPlusSemiring const&) = default;

    //! Default move assignment constructor.
    MinPlusSemiring& operator=(MinPlusSemiring&&) = default;

    ~MinPlusSemiring() = default;

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns libsemigroups::POSITIVE_INFINITY.
    int64_t zero() const override {
      return POSITIVE_INFINITY;
    }

    //! Returns libsemigroups::POSITIVE_INFINITY if either parameter equals
    //! libsemigroups::POSITIVE_INFINITY, otherwise returns the usual integer
    //! sum of the parameters.
    int64_t prod(int64_t x, int64_t y) const override {
      if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return x + y;
    }

    //! Returns the minimum of the parameters
    int64_t plus(int64_t x, int64_t y) const override {
      if (x == POSITIVE_INFINITY) {
        return y;
      } else if (y == POSITIVE_INFINITY) {
        return x;
      }
      return std::min(x, y);
    }
  };

  //! Defined in ``semiring.hpp``.
  //!
  //! This abstract class provides common member functions for its subclasses
  //! TropicalMaxPlusSemiring, TropicalMinPlusSemiring, and NaturalSemiring.
  class SemiringWithThreshold : public Semiring<int64_t> {
   public:
    //! Default constructor.
    SemiringWithThreshold() = delete;

    //! Default copy constructor.
    SemiringWithThreshold(SemiringWithThreshold const&) = default;

    //! Default move constructor.
    SemiringWithThreshold(SemiringWithThreshold&&) = default;

    //! Default copy assignment constructor.
    SemiringWithThreshold& operator=(SemiringWithThreshold const&) = default;

    //! Default move assignment constructor.
    SemiringWithThreshold& operator=(SemiringWithThreshold&&) = default;

    ~SemiringWithThreshold() = default;

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

  //! Defined in ``semiring.hpp``.
  //!
  //! The **tropical max-plus semiring** consists of the integers
  //! \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the
  //! **threshold** of the semiring) and libsemigroups::POSITIVE_INFINITY.
  struct TropicalMaxPlusSemiring final : public SemiringWithThreshold {
    //! Default constructor.
    TropicalMaxPlusSemiring() = delete;

    //! Default copy constructor.
    TropicalMaxPlusSemiring(TropicalMaxPlusSemiring const&) = default;

    //! Default move constructor.
    TropicalMaxPlusSemiring(TropicalMaxPlusSemiring&&) = default;

    //! Default copy assignment constructor.
    TropicalMaxPlusSemiring& operator=(TropicalMaxPlusSemiring const&)
        = default;

    //! Default move assignment constructor.
    TropicalMaxPlusSemiring& operator=(TropicalMaxPlusSemiring&&) = default;

    ~TropicalMaxPlusSemiring() = default;

    //! Construct from threshold.
    //!
    //! The threshold is the largest integer in the semiring.
    explicit TropicalMaxPlusSemiring(int64_t threshold)
        : SemiringWithThreshold(threshold) {}

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns libsemigroups::NEGATIVE_INFINITY.
    int64_t zero() const override {
      return NEGATIVE_INFINITY;
    }

    //! Returns libsemigroups::NEGATIVE_INFINITY if either parameter equal
    //! libsemgroups::NEGATIVE_INFINITY, otherwise returns the minimum of the
    //! usual integer sum of the parameters and the threshold of the semiring.
    int64_t prod(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                           || x == NEGATIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                           || y == NEGATIVE_INFINITY);
      if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
        return NEGATIVE_INFINITY;
      }
      return std::min(x + y, threshold());
    }

    //! Returns the minimum of (the maximum of \p x and \p y) and the threshold
    //! of the semiring.
    int64_t plus(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                           || x == NEGATIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                           || y == NEGATIVE_INFINITY);
      if (x == NEGATIVE_INFINITY) {
        return y;
      } else if (y == NEGATIVE_INFINITY) {
        return x;
      }
      return std::max(x, y);
    }

    bool contains(int64_t x) const override {
      return ((x >= 0 && x <= this->threshold()) || x == NEGATIVE_INFINITY);
    }
  };

  //! Defined in ``semiring.hpp``.
  //!
  //! The **tropical min-plus semiring** consists of the integers
  //! \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the **threshold**
  //! of the semiring) and libsemigroups::POSITIVE_INFINITY.
  struct TropicalMinPlusSemiring final : public SemiringWithThreshold {
    //! Default constructor.
    TropicalMinPlusSemiring() = delete;

    //! Default copy constructor.
    TropicalMinPlusSemiring(TropicalMinPlusSemiring const&) = default;

    //! Default move constructor.
    TropicalMinPlusSemiring(TropicalMinPlusSemiring&&) = default;

    //! Default copy assignment constructor.
    TropicalMinPlusSemiring& operator=(TropicalMinPlusSemiring const&)
        = default;

    //! Default move assignment constructor.
    TropicalMinPlusSemiring& operator=(TropicalMinPlusSemiring&&) = default;

    ~TropicalMinPlusSemiring() = default;

    //! Construct from threshold.
    //!
    //! The threshold is the largest integer in the semiring.
    explicit TropicalMinPlusSemiring(int64_t threshold)
        : SemiringWithThreshold(threshold) {}

    //! Returns the integer 0.
    int64_t one() const override {
      return 0;
    }

    //! Returns libsemigroups::POSITIVE_INFINITY.
    int64_t zero() const override {
      return POSITIVE_INFINITY;
    }

    //! Returns POSITIVE_INFINITY if either parameter equals POSITIVE_INFINITY,
    //! otherwise return the minimum of the usual integer sum of the parameters
    //! and the threshold of the semiring.
    int64_t prod(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                           || x == POSITIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                           || y == POSITIVE_INFINITY);
      if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return std::min(x + y, threshold());
    }

    //! Returns the minimum of the parameters.
    int64_t plus(int64_t x, int64_t y) const override {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= this->threshold())
                           || x == POSITIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= this->threshold())
                           || y == POSITIVE_INFINITY);
      if (x == POSITIVE_INFINITY) {
        return y;
      } else if (y == POSITIVE_INFINITY) {
        return x;
      }
      return std::min(x, y);
    }

    bool contains(int64_t x) const override {
      return ((x >= 0 && x <= this->threshold()) || x == POSITIVE_INFINITY);
    }
  };

  //! Defined in ``semiring.hpp``.
  //!
  //! This class implements the *semiring* consisting of
  //! \f$\{0, 1, ..., t, t +  1, ..., t + p - 1\}\f$ for some **threshold**
  //! \f$t\f$ and **period** \f$p\f$ with operations addition and
  //! multiplication modulo the congruence \f$t = t + p\f$.
  class NaturalSemiring final : public SemiringWithThreshold {
   public:
    //! Default constructor.
    NaturalSemiring() = delete;

    //! Default copy constructor.
    NaturalSemiring(NaturalSemiring const&) = default;

    //! Default move constructor.
    NaturalSemiring(NaturalSemiring&&) = default;

    //! Default copy assignment constructor.
    NaturalSemiring& operator=(NaturalSemiring const&) = default;

    //! Default move assignment constructor.
    NaturalSemiring& operator=(NaturalSemiring&&) = default;

    ~NaturalSemiring() = default;

    //! Construct from threshold and period.
    //!
    //! This member function constructs a semiring whose elements are
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

    bool contains(int64_t x) const override {
      return x >= 0 && x < _period + this->threshold();
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

#endif  // LIBSEMIGROUPS_SEMIRING_HPP_
