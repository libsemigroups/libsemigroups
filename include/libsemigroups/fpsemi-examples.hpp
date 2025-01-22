//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 Murray T. Whyte
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
// This file contains declarations of functions for constructing various
// standard examples of presentations.

#ifndef LIBSEMIGROUPS_FPSEMI_EXAMPLES_HPP_
#define LIBSEMIGROUPS_FPSEMI_EXAMPLES_HPP_

#include <cstddef>  // for size_t
#include <cstdint>  // for uint64_t
#include <vector>   // for vector

#include "presentation.hpp"  // for Presentation
#include "types.hpp"         // for word_type

namespace libsemigroups {

  //! \ingroup presentations_group
  //!
  //! \brief Namespace for presentations of some finitely presented semigroups.
  //!
  //! Defined in `fpsemi-examples.hpp`.
  //!
  //! This namespace contains functions which give presentations for some common
  //! finitely presented semigroups and monoids.
  namespace fpsemigroup {
    //! \brief Enum class containing various authors.
    //!
    //! The values in this enum class are used to specify the authors of a
    //! presentation. Where there are different presentations by different
    //! authors, values of this type can be passed as an argument to
    //! disambiguate which presentation is wanted.
    enum class author : uint64_t {

      //! encoded as the `uint64_t` value \f$0\f$.
      Any = 0,

      //! encoded as the `uint64_t` value \f$1\f$.
      Machine = 1,

      //! encoded as the `uint64_t` value \f$2^1\f$.
      Aizenstat = 1 << 1,

      //! encoded as the `uint64_t` value \f$2^2\f$.
      Burnside = 1 << 2,

      //! encoded as the `uint64_t` value \f$2^3\f$.
      Carmichael = 1 << 3,

      //! encoded as the `uint64_t` value \f$2^4\f$.
      Coxeter = 1 << 4,

      //! encoded as the `uint64_t` value \f$2^5\f$.
      Easdown = 1 << 5,

      //! encoded as the `uint64_t` value \f$2^6\f$.
      East = 1 << 6,

      //! encoded as the `uint64_t` value \f$2^7\f$.
      Fernandes = 1 << 7,

      //! encoded as the `uint64_t` value \f$2^8\f$.
      FitzGerald = 1 << 8,

      //! encoded as the `uint64_t` value \f$2^9\f$.
      Gay = 1 << 9,

      //! encoded as the `uint64_t` value \f$2^{10}\f$.
      Godelle = 1 << 10,

      //! encoded as the `uint64_t` value \f$2^{11}\f$.
      Guralnick = 1 << 11,

      //! encoded as the `uint64_t` value \f$2^{12}\f$.
      Halverson = 1 << 12,

      //! encoded as the `uint64_t` value \f$2^{13}\f$.
      Iwahori = 1 << 13,

      //! encoded as the `uint64_t` value \f$2^{14}\f$.
      Kantor = 1 << 14,

      //! encoded as the `uint64_t` value \f$2^{15}\f$.
      Kassabov = 1 << 15,

      //! encoded as the `uint64_t` value \f$2^{16}\f$.
      Lubotzky = 1 << 16,

      //! encoded as the `uint64_t` value \f$2^{17}\f$.
      Mitchell = 1 << 17,

      //! encoded as the `uint64_t` value \f$2^{18}\f$.
      Miller = 1 << 18,

      //! encoded as the `uint64_t` value \f$2^{19}\f$.
      Moore = 1 << 19,

      //! encoded as the `uint64_t` value \f$2^{20}\f$.
      Moser = 1 << 20,

      //! encoded as the `uint64_t` value \f$2^{21}\f$.
      Sutov = 1 << 21,

      //! encoded as the `uint64_t` value \f$2^{22}\f$.
      Ram = 1 << 22,

      //! encoded as the `uint64_t` value \f$2^{23}\f$.
      Whyte = 1 << 23
    };

    //! \brief Operator to combine \ref author values.
    //!
    //! This operator can be used arbitrarily to combine \ref author values.
    [[nodiscard]] inline author operator+(author auth1, author auth2) {
      return static_cast<author>(static_cast<uint64_t>(auth1)
                                 + static_cast<uint64_t>(auth2));
    }

    //! \brief A presentation for the stellar monoid.
    //!
    //! This function returns a monoid presentation defining the stellar monoid
    //! with \p l generators, as in Theorem 4.39 of \cite Gay1999aa.
    //!
    //! \param l the number of generators.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `l < 2`.
    [[nodiscard]] Presentation<word_type> stellar_monoid(size_t l);

    //! \brief A presentation for the dual symmetric inverse monoid.
    //!
    //! This function returns a monoid presentation defining the dual symmetric
    //! inverse monoid of degree \p n, as in Section 3 of \cite Easdown2007aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    dual_symmetric_inverse_monoid(size_t n);

    //! \brief A presentation for the uniform block bijection monoid.
    //!
    //! This function returns a monoid presentation defining the uniform block
    //! bijection monoid of degree \p n, as in \cite FitzGerald2003aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    uniform_block_bijection_monoid(size_t n);

    //! \brief A presentation for the partition monoid.
    //!
    //! This function returns a monoid presentation defining the partition
    //! monoid of degree \p n, as in \cite Halverson2005aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 1`.
    //!
    //! \sa
    //! `partition_monoid`, `partition_monoid_Eas11` and
    //! `partition_monoid_Machine`.
    [[nodiscard]] Presentation<word_type> partition_monoid_HR05(size_t n);

    //! \brief A presentation for the partition monoid.
    //!
    //! This function returns a monoid presentation defining the partition
    //! monoid of degree \p n, as in Theorem 41 of \cite East2011aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    //!
    //! \sa
    //! `partition_monoid`, `partition_monoid_HR05` and
    //! `partition_monoid_Machine`.
    [[nodiscard]] Presentation<word_type> partition_monoid_Eas11(size_t n);

    //! \brief A presentation for the partition monoid.
    //!
    //! This function returns a monoid presentation defining the partition
    //! monoid of degree \p n, where \p n is either `2` or `3`. This
    //! presentation was found using `libsemigroups`.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n != 2` or `n != 3`.
    //!
    //! \sa
    //! `partition_monoid`, `partition_monoid_Eas11`, and
    //! `partition_monoid_HR05`.
    [[nodiscard]] Presentation<word_type> partition_monoid_Machine(size_t n);

    //! \brief A presentation for the partition monoid.
    //!
    //! This function returns a monoid presentation defining the partition
    //! monoid of degree \p n, as in Theorem 41 of \cite East2011aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    //!
    //! \note
    //! This function performs exactly the same as `partition_monoid_Eas11`, and
    //! exists as a convenience function for when a presentation for the
    //! partition monoid is required, but the relations of the presentation are
    //! not important.
    //!
    //! \sa
    //! `partition_monoid_Eas11`, `partition_monoid_HR05` and
    //! `partition_monoid_Machine`.
    [[nodiscard]] inline Presentation<word_type> partition_monoid(size_t n) {
      return partition_monoid_Eas11(n);
    }

    //! \brief A presentation for the singular part of the Brauer monoid.
    //!
    //! This function returns a monoid presentation for the singular part of the
    //! Brauer monoid of degree \p n, as in Theorem 5 of \cite Maltcev2007aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type> singular_brauer_monoid(size_t n);

    //! \brief A presentation for the monoid of orientation preserving
    //! mappings.
    //!
    //! This function returns a monoid presentation defining the monoid of
    //! orientation preserving mappings on a finite chain of order \p n, as
    //! described in \cite Arthur2000aa.
    //!
    //! \param n the order of the chain.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    orientation_preserving_monoid(size_t n);

    //! \brief A presentation for the monoid of orientation preserving or
    //! reversing mappings.
    //!
    //! This function returns a monoid presentation defining the monoid of
    //! orientation preserving or reversing mappings on a finite chain of order
    //! \p n, as described in \cite Arthur2000aa.
    //!
    //! \param n the order of the chain.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    orientation_preserving_reversing_monoid(size_t n);

    //! \brief A presentation for the Temperley-Lieb monoid.
    //!
    //! This function returns a monoid presentation defining the Temperley-Lieb
    //! monoid with \p n generators, as described in Theorem 2.2 of
    //! \cite East2022aa.
    //!
    //! \param n the number of generators.
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type> temperley_lieb_monoid(size_t n);

    //! \brief A presentation for the Brauer monoid.
    //!
    //! This function returns a monoid presentation defining the Brauer monoid
    //! of degree \p n, as described in Theorem 3.1 of the paper
    //! [10.2478/s11533-006-0017-6][].
    //!
    //! \param n the degree.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.2478/s11533-006-0017-6]:
    //! https://doi.org/10.2478/s11533-006-0017-6
    [[nodiscard]] Presentation<word_type> brauer_monoid(size_t n,
                                                        author val
                                                        = author::Any);

    //! A presentation for the partial Brauer monoid.
    //!
    //! The function returns a monoid presentation defining the partial Brauer
    //! monoid of degree `n`, as described in Theorem 5.1 of
    //! the paper [10.2478/s11533-006-0017-6][].
    //!
    //! \param n the degree
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.2478/s11533-006-0017-6]:
    //! https://doi.org/10.2478/s11533-006-0017-6
    [[nodiscard]] Presentation<word_type> partial_brauer_monoid(size_t n,
                                                                author val
                                                                = author::Any);

    //! A presentation for the Motzkin monoid.
    //!
    //! Returns a monoid presentation defining
    //! the Motzkin monoid of degree `n`, as described in Theorem 4.1 of the
    //! paper [10.48550/arXiv.1301.4518][], with the additional relations
    //! \f$ r_i t_i l_i = r_i ^ 2 \f$ added to fix a hole in Lemma 4.10 which
    //! rendered the presentation as stated in the paper incorrect.
    //!
    //! \param n the degree
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! [10.48550/arXiv.1301.4518]:
    //! https://doi.org/10.48550/arXiv.1301.4518
    [[nodiscard]] Presentation<word_type> motzkin_monoid(size_t n,
                                                         author val
                                                         = author::Any);

    //! \brief A presentation for a Fibonacci semigroup.
    //!
    //! This function returns a semigroup presentation defining the Fibonacci
    //! semigroup \f$F(r, n)\f$, where \f$r\f$ is \p r and \f$n\f$ is \p n, as
    //! described in [10.1016/0022-4049(94)90005-1][].
    //!
    //! \param r the length of the left hand sides of the relations.
    //! \param n the number of generators.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n == 0`.
    //! \throws LibsemigroupsException if `r == 0`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.1016/0022-4049(94)90005-1]:
    //! https://doi.org/10.1016/0022-4049(94)90005-1
    [[nodiscard]] Presentation<word_type>
    fibonacci_semigroup(size_t r, size_t n, author val = author::Any);

    //! \brief A presentation for the plactic monoid.
    //!
    //! This function returns a monoid presentation defining the plactic monoid
    //! with \p n generators (see Section 3 of [10.1007/s00233-022-10285-3][]).
    //!
    //! \param n the number of generators.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.1007/s00233-022-10285-3]: https://doi.org/10.1007/s00233-022-10285-3
    [[nodiscard]] Presentation<word_type> plactic_monoid(size_t n,
                                                         author val
                                                         = author::Any);

    //! \brief A presentation for the stylic monoid.
    //!
    //! This function returns a monoid presentation defining the stylic monoid
    //! with \p n generators (see Theorem 8.1 of [10.1007/s00233-022-10285-3]).
    //!
    //! \param n the number of generators.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.1007/s00233-022-10285-3]: https://doi.org/10.1007/s00233-022-10285-3
    [[nodiscard]] Presentation<word_type> stylic_monoid(size_t n,
                                                        author val
                                                        = author::Any);

    //! \brief A presentation for the symmetric group.
    //!
    //! This function returns a monoid presentation for the symmetric group of
    //! degree \p n. The arguments \p val and \p index determine the specific
    //! presentation which is returned. The options are:
    //!
    // clang-format off
    //!
    //! Author                              | Index | No. generators  | No. relations             | Reference
    //! ----------------------------------- | ----- | --------------- | ------------------------- | ---------------------------------------------------------------
    //! `author::Burnside + author::Miller` | `0`   | \f$n - 1\f$     | \f$n^3 - 5n^2 + 9n - 5\f$ | p.464 of [10.1017/CBO9781139237253][]   <!-- NOLINT -->
    //! `author::Carmichael`                | `0`   | \f$n - 1\f$     | \f$(n - 1)^2\f$           | Comment 9.5.2 of [10.1007/978-1-84800-281-4][]   <!-- NOLINT -->
    //! `author::Coxeter + author::Moser`   | `0`   | \f$n - 1\f$     | \f$n(n + 1)/2\f$          | Ch.3, Prop 1.2 of [hdl.handle.net/10023/2821][]  <!-- NOLINT -->
    //! `author::Moore`                     | `0`   | \f$2\f$         | \f$n + 1\f$               | Ch. 3, Prop 1.1 of [hdl.handle.net/10023/2821][] <!-- NOLINT -->
    //! ^                                   | `1`   | \f$n - 1\f$     | \f$n(n + 1)/2\f$          | Comment 9.5.3 of [10.1007/978-1-84800-281-4][]   <!-- NOLINT -->
    //!
    //! [10.1017/CBO9781139237253]: https://doi.org/10.1017/CBO9781139237253
    //! [10.1007/978-1-84800-281-4]: https://doi.org/10.1007/978-1-84800-281-4
    //! [hdl.handle.net/10023/2821]: http://hdl.handle.net/10023/2821
    //!
    // clang-format on
    //!
    //! \param n the degree of the symmetric group.
    //! \param val the author of the presentation (default:
    //! `author::Carmichael`).
    //! \param index which of the author's presentations to return, if multiple
    //! exist (default: `0`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    //! \throws LibsemigroupsException if the author-index combination is
    //! not present in the table above.
    //!
    [[nodiscard]] Presentation<word_type> symmetric_group(size_t n,
                                                          author val
                                                          = author::Carmichael,
                                                          size_t index = 0);

    //! \brief A presentation for the alternating group.
    //!
    //! This function returns a monoid presentation defining the alternating
    //! group of degree \p n. The argument \p val determines the specific
    //! presentation which is returned. The options are:
    //! * `author::Moore` (see Ch. 3, Prop 1.3 of
    //! [hdl.handle.net/10023/2821][]).
    //!
    //! \param n the degree of the alternating group.
    //! \param val the author of the presentation (default: `author::Moore`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    //! \throws LibsemigroupsException if `val` is not `author::Moore`.
    //!
    //! [hdl.handle.net/10023/2821]: http://hdl.handle.net/10023/2821
    [[nodiscard]] Presentation<word_type> alternating_group(size_t n,
                                                            author val
                                                            = author::Moore);

    //! \brief A presentation for a rectangular band.
    //!
    //! This function returns a semigroup presentation defining the \p m by \p n
    //! rectangular band, as given in Proposition 4.2 of
    //! [10.1007/s002339910016][].
    //!
    //! \param m the number of rows.
    //! \param n the number of columns.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `m == 0`.
    //! \throws LibsemigroupsException if `n == 0`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.1007/s002339910016]: https://doi.org/10.1007/s002339910016
    [[nodiscard]] Presentation<word_type>
    rectangular_band(size_t m, size_t n, author val = author::Any);

    //! \brief A presentation for the full transformation monoid.
    //!
    //! This function returns a monoid presentation defining the full
    //! transformation monoid. The arguments \p val and \p index determine the
    //! specific presentation which is returned. The options are:
    //! * `author::Aizenstat` and `index = 0` (see Ch. 3, Prop 1.7 of
    //! [http://hdl.handle.net/10023/2821][]);
    //! * `author::Iwahori` and `index = 0` (see Theorem 9.3.1 of
    //! [10.1007/978-1-84800-281-4][]);
    //! * `author::Mitchell + author::Whyte` (see Theorem 1.5 of
    //! [10.48550/arXiv.2406.19294][]):
    //!    * `index = 0` for the presentation with five non-symmetric-group
    //!    relations;
    //!    * `index = 1` for the presentation with four non-symmetric-group
    //!    relations, valid for odd degree.
    //!
    //! \param n the degree of the full transformation monoid.
    //! \param val the author of the presentation (default: `author::Mitchell +
    //! author::Whyte`).
    //! \param index which of the author's presentations to
    //! return, if multiple exist (default: `0`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    //! \throws LibsemigroupsException if the author-index combination is
    //! not present in the table above.
    //!
    //! [http://hdl.handle.net/10023/2821]: http://hdl.handle.net/10023/2821
    //! [10.1007/978-1-84800-281-4]: https://doi.org/10.1007/978-1-84800-281-4
    //! [10.48550/arXiv.2406.19294]: https://doi.org/10.48550/arXiv.2406.19294
    [[nodiscard]] Presentation<word_type>
    full_transformation_monoid(size_t n,
                               author val   = author::Mitchell + author::Whyte,
                               size_t index = 0);

    //! \brief A presentation for the partial transformation monoid.
    //!
    //! This function returns a monoid presentation defining the partial
    //! transformation monoid. The argument \p val determines the specific
    //! presentation which is returned. The options are:
    //! * `author::Machine`;
    //! * `author::Sutov` (see Theorem 9.4.1 of
    //! [10.1007/978-1-84800-281-4][]);
    //! * `author::Mitchell + author::Whyte`  (See Theorem 1.5 of
    //! [10.48550/arXiv.2406.19294]).
    //!
    //! \param n the degree of the partial transformation monoid.
    //! \param val the author of the presentation (default: `author::Mitchell +
    //! author::Whyte`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if \p val is not one of the options
    //! specified above.
    //!
    //! [10.1007/978-1-84800-281-4]: https://doi.org/10.1007/978-1-84800-281-4
    //! [10.48550/arXiv.2406.19294]: https://doi.org/10.48550/arXiv.2406.19294
    [[nodiscard]] Presentation<word_type> partial_transformation_monoid(
        size_t n,
        author val = author::Mitchell + author::Whyte);

    //! \brief A presentation for the symmetric inverse monoid.
    //!
    //! This function returns a monoid presentation defining the symmetric
    //! inverse monoid. The argument \brief val determines the specific
    //! presentation which is returned. The options are:
    //! * `author::Gay`;
    //! * `author::Sutov` (see Theorem 9.2.2 of
    //! [10.1007/978-1-84800-281-4][]);
    //! * `author::Mitchell + author::Whyte` (see Theorem 1.4 of
    //! [10.48550/arXiv.2406.19294][]).
    //!
    //!
    //! \param n the degree of the symmetric inverse monoid.
    //! \param val the author of the presentation (default: `author::Mitchell +
    //! author::Whyte`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if \p val is not one of the options
    //! specified above.
    //!
    //! [10.1007/978-1-84800-281-4]: https://doi.org/10.1007/978-1-84800-281-4
    //! [10.48550/arXiv.2406.19294]: https://doi.org/10.48550/arXiv.2406.19294
    //
    // When val == author::Gay, this is just a presentation for the symmetric
    // inverse monoid, a slightly modified version from Solomon (so that
    // contains the Coxeter+Moser presentation for the symmetric group),
    // Example 7.1.2 in Joel gay's thesis (JDM the presentation in Example 7.1.2
    // seems to have 2n - 1 generators whereas this function returns a monoid on
    // n generators. TODO ask Florent again if this reference is correct
    // Maybe should be Solomon:
    // https://www.sciencedirect.com/science/article/pii/S0021869303005933/pdf
    [[nodiscard]] Presentation<word_type>
    symmetric_inverse_monoid(size_t n,
                             author val = author::Mitchell + author::Whyte);

    //! \brief A presentation for the Chinese monoid.
    //!
    //! This function returns a monoid presentation defining the Chinese monoid,
    //! as described in [10.1142/S0218196701000425][].
    //!
    //! \param n the number of generators.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.1142/S0218196701000425]: https://doi.org/10.1142/S0218196701000425
    [[nodiscard]] Presentation<word_type> chinese_monoid(size_t n,
                                                         author val
                                                         = author::Any);

    //! \brief A presentation for a monogenic semigroup.
    //!
    //! This function returns a presentation defining the monogenic semigroup
    //! defined by the presentation \f$\langle a \mid a^{m + r} = a^m
    //! \rangle\f$.
    //!
    //! If \p m is 0, the presentation returned is a monoid presentation;
    //! otherwise, a semigroup presentation is returned.
    //!
    //! \param m the index.
    //! \param r the period.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `m < 0`.
    //! \throws LibsemigroupsException if `r = 0`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    [[nodiscard]] Presentation<word_type>
    monogenic_semigroup(size_t m, size_t r, author val = author::Any);

    //! \brief A presentation for the monoid of order-preserving mappings.
    //!
    //! This function returns a monoid presentation defining the monoid of
    //! order-preserving transformations of degree \p n, as described in Section
    //! 2 of the paper [10.1007/s10012-000-0001-1][].
    //!
    //! This presentation has \f$2n - 2\f$ generators and \f$n^2\f$ relations.
    //!
    //! \param n the degree.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.1007/s10012-000-0001-1]: https://doi.org/10.1007/s10012-000-0001-1
    [[nodiscard]] Presentation<word_type>
    order_preserving_monoid(size_t n, author val = author::Any);

    //! \brief A presentation for the cyclic inverse monoid.
    //!
    //! This function returns a monoid presentation defining the cyclic inverse
    //! monoid of degree \p n.
    //!
    //! The arguments \p val and \p index determine the specific presentation
    //! which is returned. The options are:
    //! * `val = author::Fernandes`:
    //!    * `index = 0` (see Theorem 2.6 of [10.48550/arxiv.2211.02155][]);
    //!    * `index = 1` (see Theorem 2.7 of [10.48550/arxiv.2211.02155][]).
    //!
    //! The presentation with `val = author::Fernandes` and `index = 0` has
    //! \f$n + 1\f$ generators and \f$\frac{1}{2} \left(n^2 + 3n + 4\right)\f$
    //! relations.
    //!
    //! The presentation with `val = author::Fernandes` and `index = 1` has
    //! \f$2\f$ generators and \f$\frac{1}{2}\left(n^2 - n + 6\right)\f$
    //! relations.
    //!
    //! \param n the degree.
    //! \param val the author (default: `author::Fernandes`).
    //! \param index the index (default: `1`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    //! \throws LibsemigroupsException if `val` is not `author::Fernandes`.
    //! \throws LibsemigroupsException if `val == author::Fernandes` and \p
    //! index is not `0` or `1`.
    //!
    //! [10.48550/arxiv.2211.02155]: https://doi.org/10.48550/arxiv.2211.02155
    [[nodiscard]] Presentation<word_type>
    cyclic_inverse_monoid(size_t n,
                          author val   = author::Fernandes,
                          size_t index = 1);

    //! \p A presentation for the order-preserving part of the cyclic inverse
    //! monoid.
    //!
    //! This function returns a monoid presentation defining the
    //! order-preserving part of the cyclic inverse monoid of degree \p n, as
    //! described in Theorem 2.17 of the paper [10.48550/arxiv.2211.02155][].
    //!
    //! \param n the degree.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.48550/arxiv.2211.02155]: https://doi.org/10.48550/arxiv.2211.02155
    [[nodiscard]] Presentation<word_type>
    order_preserving_cyclic_inverse_monoid(size_t n, author val = author::Any);

    //! \brief A presentation for the monoid of partial isometries of a cycle
    //! graph.
    //!
    //! This function returns a monoid presentation defining the monoid of
    //! partial isometries of an \f$n\f$-cycle graph, as described in
    //! Theorem 2.8 of [10.48550/arxiv.2205.02196][].
    //!
    //! \param n the number of vertices of the cycle graph.
    //! \param val the author of the presentation (default: `author::Any`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    //! \throws LibsemigroupsException if \p val is not `author::Any`.
    //!
    //! [10.48550/arxiv.2205.02196]: https://doi.org/10.48550/arxiv.2205.02196
    [[nodiscard]] Presentation<word_type>
    partial_isometries_cycle_graph_monoid(size_t n, author val = author::Any);

    //! \brief A non-presentation for the symmetric group.
    //!
    //! This function returns a monoid presentation which is claimed to define
    //! the symmetric group of degree \p n, but does not. The argument \p val
    //! determines the specific non-presentation which is returned. The options
    //! are:
    //! * `author::Guralnick + author::Kantor + author::Kassabov +
    //! author::Lubotzky` [doi.org/10.1090/S0894-0347-08-00590-0][].
    //!
    //! \param n the claimed degree of the symmetric group.
    //! \param val the author of the presentation (default: `author::Guralnick +
    //! author::Kantor + author::Kassabov + author::Lubotzky`).
    //!
    //! \returns A value of type `Presentation<libsemigroups::word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    //! \throws LibsemigroupsException if `val` is not listed above.
    //!
    //! [doi.org/10.1090/S0894-0347-08-00590-0]:
    //! https://doi.org/10.1090/S0894-0347-08-00590-0
    [[nodiscard]] Presentation<word_type>
    not_symmetric_group(size_t n,
                        author val = author::Guralnick + author::Kantor
                                     + author::Kassabov + author::Lubotzky);

    // TODO (doc)
    // n should be prime for this presentation to actually defined the claimed
    // group.
    [[nodiscard]] Presentation<word_type> special_linear_group_2(size_t n,
                                                                 author val
                                                                 = author::Any);

    // TODO (doc)
    [[nodiscard]] Presentation<word_type> hypo_plactic_monoid(size_t n,
                                                              author val
                                                              = author::Any);

    // TODO (doc)
    [[nodiscard]] Presentation<word_type>
    sigma_stylic_monoid(std::vector<size_t> const& sigma,
                        author                     val = author::Any);

    // TODO add okada_monoid
    // TODO add free_semilattice

    // TODO update doc
    // When q = 0, the a_i^2 = 1 relations from the C+M symmetric group
    // presentation are replaced with a_i^2 = a_i. See Definition 4.1.1 in Joel
    // Gay's thesis
    // https://theses.hal.science/tel-01861199
    //
    //  This could also be called the RennerTypeAMonoid
    [[nodiscard]] Presentation<word_type> zero_rook_monoid(size_t n,
                                                           author val
                                                           = author::Any);

    // TODO rename renner_type_b_monoid:
    // when q = 0: Definition 8.4.1 + Example 8.4.2 in Joel's thesis.
    // when q = 1: same presentation as for q=0 except pi_i ^ 2 = pi_i replaced
    // with pi_i ^ 2 = 1 (usual Renner monoid) (q is the Iwahori-Hecke
    // deformation of the Renner monoid). q = 1From Joel's thesis,
    // Theorem 8.4.19.
    [[nodiscard]] Presentation<word_type>
    renner_type_B_monoid(size_t l, int q, author val = author::Any);

    // not_renner_type_b_monoid
    // when q = 0, Example 7.1.2, of Joel's thesis
    // when q = 1, is Godelle's presentation from: p40 of
    // https://www.cambridge.org/core/services/aop-cambridge-core/content/view/B6BAB75BD3463916FEDEC15BEDA724FF/S0004972710000365a.pdf/presentation_for_renner_monoids.pdf
    // or
    // https://arxiv.org/abs/0904.0926
    [[nodiscard]] Presentation<word_type>
    not_renner_type_B_monoid(size_t l, int q, author val = author::Any);

    // See Theorem 8.4.43 in Joel's thesis q = 1,
    // for q = 0, Definition 8.4.22 (author = Gay)
    [[nodiscard]] Presentation<word_type>
    renner_type_D_monoid(size_t l, int q, author val = author::Any);

    // Godelle's:
    // when q = 1, p41 of
    // https://www.cambridge.org/core/services/aop-cambridge-core/content/view/B6BAB75BD3463916FEDEC15BEDA724FF/S0004972710000365a.pdf/presentation_for_renner_monoids.pdf
    // q = 0, no reference
    [[nodiscard]] Presentation<word_type>
    not_renner_type_D_monoid(size_t l, int q, author val = author::Any);

  }  // namespace fpsemigroup
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FPSEMI_EXAMPLES_HPP_
