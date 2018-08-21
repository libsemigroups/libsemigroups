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

#ifndef LIBSEMIGROUPS_INCLUDE_SEMIRING_HPP_
#define LIBSEMIGROUPS_INCLUDE_SEMIRING_HPP_

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
  template <typename T>
  class Semiring {
   public:
    //! A default destructor.
    virtual ~Semiring() {}

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
    //! This method returns \c true if the argument is mathematically contained
    //! in \c this. For semirings without threshold, this will always return
    //! true.
    virtual bool contains(T) const {
      return true;
    }
  };

  //! The usual Boolean semiring.
  struct BooleanSemiring : public Semiring<bool> {
    BooleanSemiring();
    bool one() const override;
    bool zero() const override;
    bool prod(bool, bool) const override;
    bool plus(bool, bool) const override;
  };

  //! The usual ring of integers.
  struct Integers : public Semiring<int64_t> {
    Integers();
    int64_t one() const override;
    int64_t zero() const override;
    int64_t prod(int64_t, int64_t) const override;
    int64_t plus(int64_t, int64_t) const override;
  };

  //! The *max-plus semiring* consists of the integers together with negative
  //! infinity with operations max and plus. Negative infinity is represented
  //! by NEGATIVE_INFINITY.
  struct MaxPlusSemiring : public Semiring<int64_t> {
    MaxPlusSemiring();
    //! Returns the integer 0.
    int64_t one() const override;
    //! Returns NEGATIVE_INFINITY.
    int64_t zero() const override;
    //! Returns NEGATIVE_INFINITY if either parameter equals NEGATIVE_INFINITY,
    //! otherwise returns the usual integer sum of the parameters.
    int64_t prod(int64_t, int64_t) const override;
    //! Returns the maximum of the parameters.
    int64_t plus(int64_t, int64_t) const override;
  };

  //! The *min-plus semiring* consists of the integers together
  //! with infinity with operations min and plus. Infinity is represented by
  //! POSITIVE_INFINITY.
  struct MinPlusSemiring : public Semiring<int64_t> {
    MinPlusSemiring();

    //! Returns the integer 0.
    int64_t one() const override;

    //! Returns POSITIVE_INFINITY.
    int64_t zero() const override;

    //! Returns POSITIVE_INFINITY if either parameter equals POSITIVE_INFINITY,
    //! otherwise returns the usual integer sum of the parameters.
    int64_t prod(int64_t, int64_t) const override;

    //! Returns the minimum of the parameters
    int64_t plus(int64_t, int64_t) const override;
  };

  //! This abstract class provides common methods for its subclasses
  //! TropicalMaxPlusSemiring, TropicalMinPlusSemiring, and NaturalSemiring.
  class SemiringWithThreshold : public Semiring<int64_t> {
   public:
    //! A class for semirings with a threshold.
    //!
    //! The threshold of a semiring is related to the largest finite value in
    //! the semiring.
    explicit SemiringWithThreshold(int64_t);

    //! Returns the threshold of a semiring with threshold.
    int64_t threshold() const;

   private:
    int64_t _threshold;
  };

  //! The **tropical max-plus semiring** consists of the integers
  //! \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the
  //! **threshold** of the semiring) and \f$-\POSITIVE_INFINITY\f$. Negative
  //! infinity is represented by NEGATIVE_INFINITY.
  struct TropicalMaxPlusSemiring : public SemiringWithThreshold {
    //! Construct from threshold.
    //!
    //! The threshold is the largest integer in the semiring.
    explicit TropicalMaxPlusSemiring(int64_t);

    //! Returns the integer 0.
    int64_t one() const override;

    //! Returns the NEGATIVE_INFINITY.
    int64_t zero() const override;

    //! Returns NEGATIVE_INFINITY if either parameter equal NEGATIVE_INFINITY,
    //! otherwise returns the minimum of the usual integer sum of the
    //! parameters and the threshold of the semiring.
    int64_t prod(int64_t, int64_t) const override;

    //! Returns the minimum of (the maximum of \p x and \p y) and the threshold
    //! of the semiring.
    int64_t plus(int64_t, int64_t) const override;

    bool contains(int64_t) const override;
  };

  //! The **tropical min-plus semiring** consists of the integers
  //! \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the **threshold**
  //! of the semiring) and \f$\POSITIVE_INFINITY\f$. Infinity is represented
  //! by POSITIVE_INFINITY.
  struct TropicalMinPlusSemiring : public SemiringWithThreshold {
    //! Construct from threshold.
    //!
    //! The threshold is the largest integer in the semiring.
    explicit TropicalMinPlusSemiring(int64_t);

    //! Returns the integer 0.
    int64_t one() const override;

    //! Returns POSITIVE_INFINITY.
    int64_t zero() const override;

    //! Returns POSITIVE_INFINITY if either parameter equals POSITIVE_INFINITY,
    //! otherwise return the minimum of the usual integer sum of the parameters
    //! and the threshold of the semiring.
    int64_t prod(int64_t, int64_t) const override;

    //! Returns the minimum of the parameters.
    int64_t plus(int64_t, int64_t) const override;
    bool    contains(int64_t) const override;
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
    NaturalSemiring(int64_t, int64_t);

    //! Return the integer 1.
    int64_t one() const override;

    //! Return the integer 0.
    int64_t zero() const override;

    //! Returns \p x * \p y modulo the congruence \f$t = t + p\f$ where \f$t\f$
    //! and \f$p\f$ are the threshold and period of the semiring, respectively.
    int64_t prod(int64_t, int64_t) const override;

    //! Returns \p x + \p y modulo the congruence \f$t = t + p\f$ where \f$t\f$
    //! and \f$p\f$ are the threshold and period of the semiring, respectively.
    int64_t plus(int64_t, int64_t) const override;

    //! Returns the period of the semiring.
    int64_t period() const;

    bool contains(int64_t) const override;

   private:
    int64_t thresholdperiod(int64_t x) const;
    int64_t _period;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_SEMIRING_HPP_
