/*
 * Semigroups++
 *
 * This file contains declarations for semirings.
 *
 */

#ifndef SEMIGROUPS_SEMIRING_H
#define SEMIGROUPS_SEMIRING_H

#include <algorithm>
#include <limits.h>

// Namespace for containing everything related to the class <Semiring>.

namespace semiring {

  // Abstract base class.
  // A *semiring* is a set *R* together with two binary operations + and x
  // (called *addition* and *multiplication*) such that *(R, +)* is a
  // commutative monoid with identity *0*, *(R, x)* is a monoid with identity *1*,
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
  // [wikipedia](https://en.wikipedia.org/wiki/Semiring).
  //
  // This class its subclasses provide very basic functionality for creating
  // semirings.

  class Semiring {

    public:
      virtual ~Semiring () {};

      // Semiring multiplicative identity.
      // Method for finding the multiplicative identity, or one, of the
      // semiring.
      //
      // @return the one of the semiring.
      virtual long one () const            = 0;

      // Semiring additive identity.
      // Method for finding the additive identity, or zero, of the
      // semiring.
      //
      // @return the zero of the semiring.
      virtual long zero () const           = 0;

      // Addition in the semiring.
      // @x any long int
      // @y any long int
      //
      // Method for finding the sum of two elements in the
      // semiring.
      // @return the sum of x and y in the semiring.
      virtual long plus (long x, long y) const = 0;

      // Multiplication in the semiring.
      // @x any long int
      // @y any long int
      //
      // Method for finding the product of two elements in the
      // semiring.
      // @return the product of x and y in the semiring.
      virtual long prod (long x, long y) const = 0;

      // Threshold of the semiring.
      //
      // Method for finding the threshold of a semiring. The default value is
      // -1 (undefined).
      // @return -1
      virtual long threshold () const {
        return -1;
      }

      // Period of the semiring.
      //
      // Method for finding the period of a semiring. The default value is
      // -1 (undefined).
      // @return -1
      virtual long period () const {
        return -1;
      }
  };

  // The usual ring of integers.
  //
  // This class implements ring of integers.
  class Integers : public Semiring {

    public:

      // Default constructor.
      Integers () : Semiring() {}

      // Multiplicative identity.
      // This method returns the multiplicative identity, or one, of the ring.
      //
      // @return the integer 1.
      long one () const override {
        return 1;
      }

      // Additive identity.
      // This method returns the additive identity, or zero, of the ring.
      //
      // @return the integer 0.
      long zero () const override {
        return 0;
      }

      // Multiplication in the integers.
      // @x any long int
      // @y any long int
      //
      // @return the product x \* y.
      long prod (long x, long y) const override {
        return x * y;
      }

      // Addition in the prime field.
      // @x any long int
      // @y any long int
      //
      // @return sum x + y.
      long plus (long x, long y) const override {
        return x + y;
      }
  };

  // Finite field of prime order.
  //
  // This class implements finite fields of prime order only.
  class PrimeField : public Semiring {

    public:

      // Default constructor.
      // @n the size of the finite field, this must be a prime number but this
      // is not checked.
      PrimeField (long n) : Semiring(), _n(n) {}

      // Semiring multiplicative identity.
      // This method returns the multiplicative identity, or one, of the prime field.
      //
      // @return the integer 1.
      long one () const override {
        return 1;
      }

      // Semiring additive identity.
      // This method returns the additive identity, or zero, of the prime field.
      //
      // @return the integer 0.
      long zero () const override {
        return 0;
      }

      // Multiplication in the prime field.
      // @x any long int
      // @y any long int
      //
      // @return the integer (x \* y) mod the size of the prime field.
      long prod (long x, long y) const override {
        return (x * y) % _n;
      }

      // Addition in the prime field.
      // @x any long int
      // @y any long int
      //
      // @return the integer (x + y) mod the size of the prime field.
      long plus (long x, long y) const override {
        return (x + y) % _n;
      }

      // Finite field size.
      //
      // @return the size of the prime field.
      long size () const {
        return _n;
      }

    private:
      long _n;
  };

  // Max-plus semiring.
  //
  // The *max-plus semiring* consists of the set of natural numbers together
  // with negative infinity with operations max and plus. Negative infinity is
  // represented by LONG_MIN.
  class MaxPlusSemiring : public Semiring {

    public:

      // Default constructor.
      MaxPlusSemiring () : Semiring() {}

      // Semiring multiplicative identity.
      // This method returns the multiplicative identity, or one, of the
      // max-plus semiring.
      //
      // @return the integer 0.
      long one () const override {
        return 0;
      }

      // Semiring additive identity.
      // This method returns the additive identity, or zero, of the
      // max-plus semiring.
      //
      // @return LONG_MIN.
      long zero () const override {
        return LONG_MIN;
      }

      // Multiplication in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return LONG_MIN if x or y equals LONG_MIN, otherwise return x + y.
      long prod (long x, long y) const override {
        if (x == LONG_MIN || y == LONG_MIN) {
          return LONG_MIN;
        }
        return x + y;
      }

      // Addition in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return the maximum of x and y.
      long plus (long x, long y) const override {
        return std::max(x, y);
      }
  };

  // Min-plus semiring.
  //
  // The *min-plus semiring* consists of the set of natural numbers together
  // with infinity with operations min and plus. Infinity is
  // represented by LONG_MAX.

  class MinPlusSemiring : public Semiring {

    public:

      // Default constructor.
      MinPlusSemiring () : Semiring() {}

      // Semiring multiplicative identity.
      // This method returns the multiplicative identity, or one, of the
      // min-plus semiring.
      //
      // @return the integer 0.
      long one () const override {
        return 0;
      }

      // Semiring additive identity.
      // This method returns the additive identity, or zero, of the
      // min-plus semiring.
      //
      // @return LONG_MAX.
      long zero () const override {
        return LONG_MAX;
      }

      // Multiplication in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return LONG_MAX if x or y equals LONG_MAX, otherwise return x + y.
      long prod (long x, long y) const override {
        if (x == LONG_MAX || y == LONG_MAX) {
          return LONG_MAX;
        }
        return x + y;
      }

      // Addition in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return the minimum of x and y.
      long plus (long x, long y) const override {
        return std::min(x, y);
      }
  };

  // Tropical semiring base class.
  //
  // This class provides common methods for its subclasses
  // <TropicalMaxPlusSemiring> and <TropicalMinPlusSemiring>.
  class TropicalSemiring : public Semiring {

    public:

      // Default constructor.
      // @threshold the largest integer in the semiring (or equivalently the
      // size of the semiring minus 2).
      TropicalSemiring (long threshold) : Semiring(), _threshold(threshold) {}

      // Threshold of a tropical semiring.
      // This is the largest non-negative integer in the semiring, called the
      // **threshold**.
      //
      // @return the threshold of the semiring.
      long threshold () const override {
        return _threshold;
      }

    private:

      long _threshold;
  };

  // Tropical max-plus semiring.
  //
  // The **tropical max-plus semiring** consists of the integers *0, ... , t*
  // for some value *t* (called the **threshold** of the semiring) and negative
  // infinity. Negative infinity is represented by LONG_MIN.
  class TropicalMaxPlusSemiring : public TropicalSemiring {

    public:

      // Default constructor.
      // @threshold the largest integer in the semiring (or equivalently the
      // size of the semiring minus 2).
      TropicalMaxPlusSemiring (long threshold) : TropicalSemiring(threshold) {}

      // Semiring multiplicative identity.
      // This method returns the multiplicative identity, or one, of the
      // tropical max-plus semiring.
      //
      // @return the integer 0.
      long one () const override {
        return 0;
      }

      // Semiring additive identity.
      // This method returns the additive identity, or zero, of the
      // tropical max-plus semiring.
      //
      // @return the integer LONG_MIN.
      long zero () const override {
        return LONG_MIN;
      }

      // Multiplication in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return LONG_MIN if x or y equals LONG_MIN, otherwise return the
      // minimum of x + y and the threshold of the semiring.
      long prod (long x, long y) const override {
        if (x == LONG_MIN || y == LONG_MIN) {
          return LONG_MIN;
        }
        return std::min(x + y, threshold());
      }

      // Multiplication in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return the minimum of (the maximum of x and y) and the threshold of
      // the semiring.
      long plus (long x, long y) const override {
        return std::min((std::max(x, y)), threshold());
      }
  };

  // Tropical min-plus semiring.
  //
  // The **tropical min-plus semiring** consists of the integers *0, ... , t*
  // for some value *t* (called the **threshold** of the semiring) and
  // infinity. Infinity is represented by LONG_MAX.
  class TropicalMinPlusSemiring : public TropicalSemiring {

    public:

      // Default constructor.
      // @threshold the largest integer in the semiring (or equivalently the
      // size of the semiring minus 2).
      TropicalMinPlusSemiring (long threshold) : TropicalSemiring(threshold) {}

      // Semiring multiplicative identity.
      // This method returns the multiplicative identity, or one, of the
      // tropical min-plus semiring.
      //
      // @return the integer 0.
      long one () const override {
        return 0;
      }

      // Semiring additive identity.
      // This method returns the additive identity, or zero, of the
      // tropical min-plus semiring.
      //
      // @return the integer LONG_MAX.
      long zero () const override {
        return LONG_MAX;
      }

      // Multiplication in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return LONG_MAX if x or y equals LONG_MAX, otherwise return the
      // minimum of x + y and the threshold of the semiring.
      long prod (long x, long y) const override {
        if (x == LONG_MAX || y == LONG_MAX) {
          return LONG_MAX;
        }
        return std::min(x + y, threshold());
      }

      // Multiplication in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return LONG_MAX if either of x and y is LONG_MAX, and otherwise the
      // minimum of x, y, and the threshold of the semiring.
      long plus (long x, long y) const override {
        if (x == LONG_MAX && y == LONG_MAX) {
          return LONG_MAX;
        }
        return std::min((std::min(x, y)), threshold());
      }
  };

  // Semiring of natural numbers mod *t*, *p*.
  //
  // This class implements the *semiring* consisting of *0, 1, ..., t, t + 1,
  // ..., t + p - 1* for some **threshold** *t* and **period** *p* with
  // operations addition and multiplication modulo the congruence *t = t + p*.

  class NaturalSemiring : public Semiring {

    public:

      // Default constructor.
      // @threshold the threshold, should be positive, this is not checked.
      // @period    the period, should be non-negative, this is not checked.
      NaturalSemiring (long threshold, long period)
        : Semiring(),
          _threshold(threshold),
          _period(period)
         {}

      // Semiring multiplicative identity.
      // This method returns the multiplicative identity, or one, of the
      // semiring.
      //
      // @return the integer 1.
      long one () const override {
        return 1;
      }

      // Semiring additive identity.
      // This method returns the additive identity, or zero, of the
      // semiring.
      //
      // @return the integer 0.
      long zero () const override {
        return 0;
      }

      // Multiplication in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return x \* y modulo the congruence *t = t + p* where *t* and *p* are
      // the threshold and period of the semiring, respectively.
      long prod (long x, long y) const override {
        return thresholdperiod(x * y);
      }

      // Addition in the semiring.
      // @x any long int
      // @y any long int
      //
      // @return x + y modulo the congruence *t = t + p* where *t* and *p* are
      // the threshold and period of the semiring, respectively.
      long plus (long x, long y) const override {
        return thresholdperiod(x + y);
      }

      // Threshold of the semiring.
      // The semiring consists of *0, 1, ..., t, t + 1,
      // ..., t + p - 1* for some **threshold** *t* and **period** *p* with
      // operations addition and multiplication modulo the congruence *t = t + p*.
      //
      // @return the threshold of the semiring.
      long threshold () const override {
        return _threshold;
      }

      // Period of the semiring.
      // The semiring consists of *0, 1, ..., t, t + 1,
      // ..., t + p - 1* for some **threshold** *t* and **period** *p* with
      // operations addition and multiplication modulo the congruence *t = t + p*.
      //
      // @return the period of the semiring.
      long period () const override {
        return _period;
      }

    private:

      long thresholdperiod (long x) const {
        if (x > _threshold) {
          return _threshold + (x - _threshold) % _period;
        }
        return x;
      }

      long _threshold;
      long _period;
  };

}
#endif
