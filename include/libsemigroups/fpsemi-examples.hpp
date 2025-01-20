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
#include "types.hpp"         // for relation_type

// TODO: Run include-what-you-use on this file (I (Murray) couldn't)

namespace libsemigroups {

  //! This namespace contains functions which give presentations for some
  //! finitely presented semigroups and monoids.
  namespace fpsemigroup {

    // The values in this enum class are used to specify the authors of a
    // presentation. Where there are different presentations by different
    // authors, values of this type can be passed as an argument to disambiguate
    // which presentation is wanted.
    enum class author : uint64_t {
      Any        = 0,
      Machine    = 1,
      Aizenstat  = 1 << 1,
      Burnside   = 1 << 2,
      Carmichael = 1 << 3,
      Coxeter    = 1 << 4,
      Easdown    = 1 << 5,
      East       = 1 << 6,
      Fernandes  = 1 << 7,
      FitzGerald = 1 << 8,
      Gay        = 1 << 9,
      Godelle    = 1 << 10,
      Guralnick  = 1 << 11,
      Halverson  = 1 << 12,
      Iwahori    = 1 << 13,
      Kantor     = 1 << 14,
      Kassabov   = 1 << 15,
      Lubotzky   = 1 << 16,
      Mitchell   = 1 << 17,
      Miller     = 1 << 18,
      Moore      = 1 << 19,
      Moser      = 1 << 20,
      Sutov      = 1 << 21,
      Ram        = 1 << 22,
      Whyte      = 1 << 23
    };

    //! This operator can be used arbitrarily to combine author values (see \ref
    //! author).
    [[nodiscard]] inline author operator+(author auth1, author auth2) {
      return static_cast<author>(static_cast<uint64_t>(auth1)
                                 + static_cast<uint64_t>(auth2));
    }

    //! A presentation for the stellar monoid.
    //!

    //! the stellar monoid with `l` generators, as in Theorem 4.39 of
    //! [10.48550/arXiv.1910.11740][].
    //!
    //! \param l the number of generators
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `l < 2`
    //!
    //! [10.48550/arXiv.1910.11740]: https://doi.org/10.48550/arXiv.1910.11740
    [[nodiscard]] Presentation<word_type> stellar_monoid(size_t l,
                                                         author val
                                                         = author::Any);

    //! A presentation for the dual symmetric inverse monoid.
    //!
    //! Returns a monoid presentation defining
    //! the dual symmetric inverse monoid of degree `n`. The argument `val`
    //! determines the specific presentation which is returned. The options are:
    //! * `author::Easdown + author::East + author::FitzGerald` (from Section 3
    //! of [10.48550/arxiv.0707.2439][])
    //!
    //! The default for `val` is the only option above.
    //!
    //! \param n the degree
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //! \throws LibsemigroupsException if `val` is not
    //! `author::Easdown + author::East + author::FitzGerald`
    //!
    //! [10.48550/arxiv.0707.2439]: https://doi.org/10.48550/arxiv.0707.2439
    [[nodiscard]] Presentation<word_type> dual_symmetric_inverse_monoid(
        size_t n,
        author val = author::Easdown + author::East + author::FitzGerald);

    //! A presentation for the uniform block bijection monoid.
    //!
    //! Returns a monoid presentation defining
    //! the uniform block bijection monoid of degree `n`. The argument `val`
    //! determines the specific presentation which is returned. The only option
    //! is:
    //! * `author::FitzGerald` (see [10.1017/s0004972700037692][])
    //!
    //! The default for `val` is the only option above.
    //!
    //! \param n the degree
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //! \throws LibsemigroupsException if `val` is not `author::FitzGerald`
    //!
    //! [10.1017/s0004972700037692]: https://doi.org/10.1017/s0004972700037692
    [[nodiscard]] Presentation<word_type>
    uniform_block_bijection_monoid(size_t n, author val = author::FitzGerald);

    //! A presentation for the partition monoid.
    //!
    //! Returns a presentation defining
    //! the partition monoid of degree `n`. The argument `val` determines the
    //! specific presentation which is returned. The options are:
    //! * `author::Machine`
    //! * `author::East` (see Theorem 41 of
    //! [10.1016/j.jalgebra.2011.04.008][])
    //! * `author::Halverson + author::Ram` (see Theorem 1.11 (b) of
    //! [10.1016/j.ejc.2004.06.005][])
    //!
    //! The default for `val` is `author::East`.
    //!
    //! Note that `author::East` returns a monoid presentation, and
    //! `author::Machine` returns a semigroup presentation.
    //!
    //! \param n the degree
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `val = author::Machine and n != 3`
    //! \throws LibsemigroupsException if `val = author::East and n < 4`
    //! \throws LibsemigroupsException if `val = author::Halverson + author::Ram
    //! and n < 1`
    //!
    //! [10.1016/j.jalgebra.2011.04.008]:
    //! https://doi.org/10.1016/j.jalgebra.2011.04.008
    //! [10.1016/j.ejc.2004.06.005]:
    //! https://doi.org/10.1016/j.ejc.2004.06.005
    [[nodiscard]] Presentation<word_type> partition_monoid(size_t n,
                                                           author val
                                                           = author::East);

    //! A presentation for the singular part of the Brauer monoid.
    //!
    //! Returns a monoid presentation for the
    //! singular part of the Brauer monoid of degree `n`, as in Theorem 5 of
    //! [10.21136/MB.2007.134125][]).
    //!
    //! \param n the degree
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //!
    //! [10.21136/MB.2007.134125]: https://doi.org/10.21136/MB.2007.134125
    [[nodiscard]] Presentation<word_type> singular_brauer_monoid(size_t n,
                                                                 author val
                                                                 = author::Any);

    //! A presentation for the monoid of orientation preserving
    //! mappings.
    //!
    //! Returns a monoid presentation defining
    //! the monoid of orientation preserving mappings on a finite chain of order
    //! `n`, as described in [10.1007/s10012-000-0001-1][].
    //!
    //! \param n the order of the chain
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //!
    //! [10.1007/s10012-000-0001-1]: https://doi.org/10.1007/s10012-000-0001-1
    [[nodiscard]] Presentation<word_type>
    orientation_preserving_monoid(size_t n, author val = author::Any);

    //! A presentation for the monoid of orientation preserving or reversing
    //! mappings.
    //!
    //! Returns a monoid presentation defining
    //! the monoid of orientation preserving or reversing mappings on a finite
    //! chain of order `n`, as described in [10.1007/s10012-000-0001-1][].
    //!
    //! \param n the order of the chain
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //!
    //! [10.1007/s10012-000-0001-1]: https://doi.org/10.1007/s10012-000-0001-1
    [[nodiscard]] Presentation<word_type>
    orientation_preserving_reversing_monoid(size_t n, author val = author::Any);

    //! A presentation for the Temperley-Lieb monoid.
    //!
    //! Returns a monoid presentation defining
    //! the Temperley-Lieb monoid with `n` generators, as described in
    //! Theorem 2.2 of [10.1093/qmath/haab001][].
    //!
    //! \param n the number of generators
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //!
    //! [10.1093/qmath/haab001]: https://doi.org/10.1093/qmath/haab001
    [[nodiscard]] Presentation<word_type> temperley_lieb_monoid(size_t n,
                                                                author val
                                                                = author::Any);

    //! A presentation for the Brauer monoid.
    //!
    //! Returns a monoid presentation defining
    //! the Brauer monoid of degree `n`, as described in Theorem 3.1 of the
    //! paper [10.2478/s11533-006-0017-6][].
    //!
    //! \param n the degree
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! [10.2478/s11533-006-0017-6]:
    //! https://doi.org/10.2478/s11533-006-0017-6
    [[nodiscard]] Presentation<word_type> brauer_monoid(size_t n,
                                                        author val
                                                        = author::Any);

    //! A presentation for the partial Brauer monoid.
    //!
    //! Returns a monoid presentation defining
    //! the partial Brauer monoid of degree `n`, as described in Theorem 5.1 of
    //! the paper [10.2478/s11533-006-0017-6][].
    //!
    //! \param n the degree
    //!
    //! \returns A `std::vector<relation_type>`
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

    //! A presentation for a Fibonacci semigroup.
    //!
    //! Returns a semigroup presentation defining
    //! the Fibonacci semigroup \f$F(r, n)\f$, as described in
    //! [10.1016/0022-4049(94)90005-1][].
    //!
    //! \param r the length of the left hand sides of the relations
    //! \param n the number of generators
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n = 0`
    //! \throws LibsemigroupsException if `r = 0`
    //!
    //! [10.1016/0022-4049(94)90005-1]:
    //! https://doi.org/10.1016/0022-4049(94)90005-1
    [[nodiscard]] Presentation<word_type>
    fibonacci_semigroup(size_t r, size_t n, author val = author::Any);

    //! A presentation for the plactic monoid.
    //!
    //! Returns a monoid presentation defining
    //! the plactic monoid with `n` generators (see Section 3 of
    //! [10.1007/s00233-022-10285-3][]).
    //!
    //! \param n the number of generators
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 2`
    //!
    //! [10.1007/s00233-022-10285-3]: https://doi.org/10.1007/s00233-022-10285-3
    [[nodiscard]] Presentation<word_type> plactic_monoid(size_t n,
                                                         author val
                                                         = author::Any);

    //! A presentation for the stylic monoid.
    //!
    //! Returns a monoid presentation defining
    //! the stylic monoid with `n` generators (see Theorem 8.1 of
    //! [10.1007/s00233-022-10285-3]).
    //!
    //! \param n the number of generators
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 2`
    //!
    //! [10.1007/s00233-022-10285-3]: https://doi.org/10.1007/s00233-022-10285-3
    [[nodiscard]] Presentation<word_type> stylic_monoid(size_t n,
                                                        author val
                                                        = author::Any);

    //! A presentation for the symmetric group.
    //!
    //! Returns a monoid presentation for the
    //! symmetric group. The argument `val` determines the specific presentation
    //! which is returned. The options are:
    //!
    // clang-format off
    //!
    //! Author | Index | No. generators  | No. relations | Reference
    //! ------ | ----- | --------------- | ------------- | ----------
    //! `author::Burnside + author::Miller`| `0` | \f$n - 1\f$ | \f$n^3 - 5n^2 + 9n - 5\f$ | p.464 of [10.1017/CBO9781139237253][]   <!-- NOLINT -->
    //! `author::Carmichael`               | `0` | \f$n - 1\f$ | \f$(n - 1)^2\f$  | Comment 9.5.2 of [10.1007/978-1-84800-281-4][]   <!-- NOLINT -->
    //! `author::Coxeter + author::Moser`  | `0` | \f$n - 1\f$ | \f$n(n + 1)/2\f$ | Ch.3, Prop 1.2 of [hdl.handle.net/10023/2821][]  <!-- NOLINT -->
    //! `author::Moore`                    | `0` | \f$2\f$     | \f$n + 1\f$      | Ch. 3, Prop 1.1 of [hdl.handle.net/10023/2821][] <!-- NOLINT -->
    //! ^                                  | `1` | \f$n - 1\f$ | \f$n(n + 1)/2\f$ | Comment 9.5.3 of [10.1007/978-1-84800-281-4][]   <!-- NOLINT -->
    //!
    //! [10.1017/CBO9781139237253]: https://doi.org/10.1017/CBO9781139237253
    //! [10.1007/978-1-84800-281-4]: https://doi.org/10.1007/978-1-84800-281-4
    //! [hdl.handle.net/10023/2821]: http://hdl.handle.net/10023/2821
    //!
    // clang-format on
    //!
    //! The default for `val` is `author::Carmichael`. The default for `index`
    //! is `0`.
    //!
    //! \param n the degree of the symmetric group
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 4`
    //! \throws LibsemigroupsException if the author-index combination is
    //! invalid
    //!
    [[nodiscard]] Presentation<word_type> symmetric_group(size_t n,
                                                          author val
                                                          = author::Carmichael,
                                                          size_t index = 0);

    //! A presentation for the alternating group.
    //!
    //! Returns a monoid presentation defining the
    //! alternating group of degree `n`. The argument `val` determines the
    //! specific presentation which is returned. The options are:
    //! * `author::Moore` (see Ch. 3, Prop 1.3 of [hdl.handle.net/10023/2821][])
    //!
    //! The default for `val` is `author::Moore`.
    //!
    //! \param n the degree of the alternating group
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `val` is not `author::Moore`
    //! \throws LibsemigroupsException if `n < 4`
    //!
    //! [hdl.handle.net/10023/2821]: http://hdl.handle.net/10023/2821
    [[nodiscard]] Presentation<word_type> alternating_group(size_t n,
                                                            author val
                                                            = author::Moore);

    //! A presentation for a rectangular band.
    //!
    //! Returns a semigroup presentation defining
    //! the `m` by `n` rectangular band, as given in Proposition 4.2 of
    //! [10.1007/s002339910016][].
    //!
    //! \param m the number of rows
    //! \param n the number of columns
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `m = 0`
    //! \throws LibsemigroupsException if `n = 0`
    //!
    //! [10.1007/s002339910016]: https://doi.org/10.1007/s002339910016
    [[nodiscard]] Presentation<word_type>
    rectangular_band(size_t m, size_t n, author val = author::Any);

    //! A presentation for the full transformation monoid.
    //!
    //! Returns a monoid presentation defining the
    //! full transformation monoid. The argument `val` determines the specific
    //! presentation which is returned. The options are:
    //! * `author::Aizenstat` (see Ch. 3, Prop 1.7 of
    //! [http://hdl.handle.net/10023/2821][])
    //! * `author::Iwahori` (see Theorem 9.3.1 of
    //! [10.1007/978-1-84800-281-4][])
    //! * `author::Mitchell + author::Whyte` (see Theorem 1.5 of
    //! [10.48550/arXiv.2406.19294][])
    //!    * `index = 0` for the presentation with five non-symmetric-group
    //!    relations
    //!    * `index = 1` for the presentation with four non-symmetric-group
    //!    relations, valid for odd degree
    //!
    //! The default for `val` is `author::Mitchell + author::Whyte`.
    //!
    //! \param n the degree of the full transformation monoid
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `val` is not listed above (modulo
    //! order of author) \throws LibsemigroupsException if `n < 4`
    //!
    //! [http://hdl.handle.net/10023/2821]: http://hdl.handle.net/10023/2821
    //! [10.1007/978-1-84800-281-4]: https://doi.org/10.1007/978-1-84800-281-4
    //! [10.48550/arXiv.2406.19294]: https://doi.org/10.48550/arXiv.2406.19294
    [[nodiscard]] Presentation<word_type>
    full_transformation_monoid(size_t n,
                               author val   = author::Mitchell + author::Whyte,
                               size_t index = 0);

    //! A presentation for the partial transformation monoid.
    //!
    //! Returns a monoid presentation defining the
    //! partial transformation monoid. The argument `val` determines the
    //! specific presentation which is returned. The options are:
    //! * `author::Machine`
    //! * `author::Sutov` (see Theorem 9.4.1 of
    //! [10.1007/978-1-84800-281-4][])
    //! * `author::Mitchell + author::Whyte`  (See Theorem 1.5 of
    //! [10.48550/arXiv.2406.19294])
    //!
    //! The default for `val` is `author::Mitchell + author::Whyte`.
    //!
    //! \param n the degree of the partial transformation monoid
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `val` is not listed above (modulo
    //! order of author)
    //!
    //! [10.1007/978-1-84800-281-4]: https://doi.org/10.1007/978-1-84800-281-4
    //! [10.48550/arXiv.2406.19294]: https://doi.org/10.48550/arXiv.2406.19294
    [[nodiscard]] Presentation<word_type> partial_transformation_monoid(
        size_t n,
        author val = author::Mitchell + author::Whyte);

    //! A presentation for the symmetric inverse monoid.
    //!
    //! Returns a monoid presentation defining the
    //! symmetric inverse monoid. The argument `val` determines the specific
    //! presentation which is returned. The options are:
    //! * `author::Sutov` (see Theorem 9.2.2 of
    //! [10.1007/978-1-84800-281-4][])
    //! * `author::Mitchell + author::Whyte` (see Theorem 1.4 of
    //! [10.48550/arXiv.2406.19294][])
    //!
    //!
    //! The default value for `val` is `author::Mitchell + author::Whyte`.
    //!
    //! \param n the degree of the symmetric inverse monoid
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `val` is not listed above (modulo
    //! order of author)
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

    //! A presentation for the Chinese monoid.
    //!
    //! Returns a monoid presentation defining
    //! the Chinese monoid, as described in [10.1142/S0218196701000425][].
    //!
    //! \param n the number of generators
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 2`
    //!
    //! [10.1142/S0218196701000425]: https://doi.org/10.1142/S0218196701000425
    [[nodiscard]] Presentation<word_type> chinese_monoid(size_t n,
                                                         author val
                                                         = author::Any);

    //! A presentation for a monogenic semigroup.
    //!
    //! Returns a presentation defining
    //! the monogenic semigroup defined by the presentation \f$\langle a \mid
    //! a^{m + r} = a^m \rangle\f$.
    //!
    //! If \param m = 0, the presentation returned is a monoid presentation;
    //! otherwise, a semigroup presentation is returned.
    //!
    //! \param m the index
    //! \param r the period
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `m < 0`
    //! \throws LibsemigroupsException if `r = 0`
    [[nodiscard]] Presentation<word_type>
    monogenic_semigroup(size_t m, size_t r, author val = author::Any);

    //! A presentation for the monoid of order-preserving mappings.
    //!
    //! Returns a monoid presentation defining
    //! the monoid of order-preserving transformations of degree `n`, as
    //! described in Section 2 of the paper [10.1007/s10012-000-0001-1][].
    //!
    //! This presentation has \f$2n - 2\f$ generators and \f$n^2\f$ relations.
    //!
    //! \param n the degree
    //! \param val the author
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //!
    //! [10.1007/s10012-000-0001-1]: https://doi.org/10.1007/s10012-000-0001-1
    [[nodiscard]] Presentation<word_type>
    order_preserving_monoid(size_t n, author val = author::Any);

    //! A presentation for the cyclic inverse monoid.
    //!
    //! Returns a monoid presentation defining
    //! the cyclic inverse monoid of degree `n`.
    //!
    //! The combination of `val` and `index` determines the specific
    //! presentation which is returned. The options for these are:
    //! * `val = author::Fernandes`
    //!    * `index = 0` (see Theorem 2.6 of [10.48550/arxiv.2211.02155][])
    //!    * `index = 1` (see Theorem 2.7 of [10.48550/arxiv.2211.02155][])
    //!
    //! \param n the degree
    //! \param val the author
    //! \param index the index
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //! \throws LibsemigroupsException if `val` is not `author::Fernandes`
    //! \throws LibsemigroupsException if `val = author::Fernandes` and `index`
    //! is not `0` or `1`
    //!
    //! The presentation with `val = author::Fernandes` and `index = 0` has
    //! \f$n + 1\f$ generators and \f$\frac{1}{2} \left(n^2 + 3n + 4\right)\f$
    //! relations.
    //!
    //! The presentation with `val = author::Fernandes` and `index = 1` has
    //! \f$2\f$ generators and \f$\frac{1}{2}\left(n^2 - n + 6\right)\f$
    //! relations.
    //!
    //! [10.48550/arxiv.2211.02155]: https://doi.org/10.48550/arxiv.2211.02155
    [[nodiscard]] Presentation<word_type>
    cyclic_inverse_monoid(size_t n,
                          author val   = author::Fernandes,
                          size_t index = 1);

    //! A presentation for the order-preserving part of the cyclic inverse
    //! monoid.
    //!
    //! Returns a monoid presentation defining
    //! the order-preserving part of the cyclic inverse monoid of degree `n`, as
    //! described in Theorem 2.17 of
    //! the paper [10.48550/arxiv.2211.02155][].
    //!
    //! \param n the degree
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //!
    //! [10.48550/arxiv.2211.02155]: https://doi.org/10.48550/arxiv.2211.02155
    [[nodiscard]] Presentation<word_type>
    order_preserving_cyclic_inverse_monoid(size_t n, author val = author::Any);

    //! A presentation for the monoid of partial isometries of a cycle graph.
    //!
    //! Returns a monoid presentation defining
    //! the monoid of partial isometries of an \f$n\f$-cycle graph, as described
    //! in Theorem 2.8 of [10.48550/arxiv.2205.02196][].
    //!
    //! \param n the number of vertices of the cycle graph
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `n < 3`
    //!
    //! [10.48550/arxiv.2205.02196]: https://doi.org/10.48550/arxiv.2205.02196
    [[nodiscard]] Presentation<word_type>
    partial_isometries_cycle_graph_monoid(size_t n, author val = author::Any);

    //! A non-presentation for the symmetric group.
    //!
    //! Returns a monoid presentation which is
    //! claimed to define the symmetric group of degree `n`, but does not. The
    //! argument `val` determines the specific presentation which is returned.
    //! The options are:
    //! * `author::Guralnick + author::Kantor + author::Kassabov +
    //! author::Lubotzky` [doi.org/10.1090/S0894-0347-08-00590-0][]
    //!
    //! The default for `val` is the only option above.
    //!
    //! \param n the claimed degree of the symmetric group
    //! \param val the author of the presentation
    //!
    //! \returns A `std::vector<relation_type>`
    //!
    //! \throws LibsemigroupsException if `val` is not listed above (modulo
    //! order of author)
    //! \throws LibsemigroupsException if `n < 4`
    //!
    //! [doi.org/10.1090/S0894-0347-08-00590-0]:
    //! https://doi.org/10.1090/S0894-0347-08-00590-0
    [[nodiscard]] Presentation<word_type>
    not_symmetric_group(size_t n,
                        author val = author::Guralnick + author::Kantor
                                     + author::Kassabov + author::Lubotzky);

    [[nodiscard]] Presentation<word_type> special_linear_group_2(size_t n,
                                                                 author val
                                                                 = author::Any);

    // TODO (doc)
    [[nodiscard]] Presentation<word_type> hypo_plactic_monoid(size_t n,
                                                              author val
                                                              = author::Any);

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
