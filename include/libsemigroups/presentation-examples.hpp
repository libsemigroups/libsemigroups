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
#include <vector>   // for vector

#include "presentation.hpp"  // for Presentation
#include "types.hpp"         // for word_type

namespace libsemigroups {
  //! \ingroup presentations_group
  //!
  //! \brief Namespace for presentations of some finitely presented
  //! semigroups.
  //!
  //! Defined in `presentation-examples.hpp`.
  //!
  //! This namespace contains functions which give presentations for some
  //! common finitely presented semigroups and monoids.
  namespace presentation::examples {

    //! \name Default presentations
    ///@{
    // Forward declarations of the functions so they appear first in the
    // documentation
    Presentation<word_type> alternating_group(size_t n);
    Presentation<word_type> brauer_monoid(size_t n);
    Presentation<word_type> chinese_monoid(size_t n);
    Presentation<word_type> cyclic_inverse_monoid(size_t n);
    Presentation<word_type> dual_symmetric_inverse_monoid(size_t n);
    Presentation<word_type> fibonacci_semigroup(size_t r, size_t n);
    Presentation<word_type> full_transformation_monoid(size_t n);
    Presentation<word_type> hypo_plactic_monoid(size_t n);
    Presentation<word_type> monogenic_semigroup(size_t m, size_t r);
    Presentation<word_type> motzkin_monoid(size_t n);
    Presentation<word_type> not_renner_type_B_monoid(size_t l, int q);
    Presentation<word_type> not_renner_type_D_monoid(size_t l, int q);
    Presentation<word_type> not_symmetric_group(size_t n);
    Presentation<word_type> order_preserving_cyclic_inverse_monoid(size_t n);
    Presentation<word_type> order_preserving_monoid(size_t n);
    Presentation<word_type> orientation_preserving_monoid(size_t n);
    Presentation<word_type> orientation_preserving_reversing_monoid(size_t n);
    Presentation<word_type> partial_brauer_monoid(size_t n);
    Presentation<word_type> partial_isometries_cycle_graph_monoid(size_t n);
    Presentation<word_type> partial_transformation_monoid(size_t n);
    Presentation<word_type> partition_monoid(size_t n);
    Presentation<word_type> plactic_monoid(size_t n);
    Presentation<word_type> rectangular_band(size_t m, size_t n);
    Presentation<word_type> renner_type_B_monoid(size_t l, int q);
    Presentation<word_type> renner_type_D_monoid(size_t l, int q);
    Presentation<word_type>
    sigma_plactic_monoid(std::vector<size_t> const& sigma);
    Presentation<word_type> singular_brauer_monoid(size_t n);
    Presentation<word_type> special_linear_group_2(size_t q);
    Presentation<word_type> stellar_monoid(size_t l);
    Presentation<word_type> stylic_monoid(size_t n);
    Presentation<word_type> symmetric_group(size_t n);
    Presentation<word_type> symmetric_inverse_monoid(size_t n);
    Presentation<word_type> temperley_lieb_monoid(size_t n);
    Presentation<word_type> uniform_block_bijection_monoid(size_t n);
    Presentation<word_type> zero_rook_monoid(size_t n);
    ///@}

    //! \name Specific presentations
    //!
    //! The functions documented below provide specific presentations for
    //! various semigroups and monoids, usually accompanied by a reference to
    //! the source of the presentation. There may be several presentations
    //! for any semigroup or monoid.
    //!
    //! For each semigroup or monoid, there is a corresponding default
    //! function that is documented in the \ref
    //! default_presentations_member_group "Default presentations" section.
    //!
    ///@{

    //! \brief A presentation for the alternating group.
    //!
    //! This function returns a monoid presentation defining the alternating
    //! group of degree \p n, as in Theorem B of \cite Moore1897aa.
    //!
    //! \param n the degree of the alternating group.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    [[nodiscard]] Presentation<word_type> alternating_group_Moo97(size_t n);

    //! \brief A presentation for the Brauer monoid.
    //!
    //! This function returns a monoid presentation defining the Brauer monoid
    //! of degree \p n, as described in Theorem 3.1 of
    //! \cite Kudryavtseva2006aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 1`.
    [[nodiscard]] Presentation<word_type> brauer_monoid_KM07(size_t n);

    //! \brief A presentation for the Chinese monoid.
    //!
    //! This function returns a monoid presentation defining the Chinese
    //! monoid with \p n generators, as in \cite Cassaigne2001aa.
    //!
    //! \param n the number of generators.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type> chinese_monoid_CEKNH01(size_t n);

    //! \brief A presentation for the cyclic inverse monoid.
    //!
    //! This function returns a monoid presentation defining the cyclic
    //! inverse monoid of degree \p n, as in Theorem 2.6 of
    //! \cite Fernandes2022aa.
    //!
    //! This has \f$n + 1\f$ generators and \f$\frac{1}{2} \left(n^2 + 3n +
    //! 4\right)\f$ relations.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    cyclic_inverse_monoid_Fer22_a(size_t n);

    //! \brief A presentation for the cyclic inverse monoid.
    //!
    //! This function returns a monoid presentation defining the cyclic
    //! inverse monoid of degree \p n, as in Theorem 2.7 of
    //! \cite Fernandes2022aa.
    //!
    //! This presentation has \f$2\f$ generators and \f$\frac{1}{2}\left(n^2 -
    //! n
    //! + 6\right)\f$ relations.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    cyclic_inverse_monoid_Fer22_b(size_t n);

    //! \brief A presentation for the dual symmetric inverse monoid.
    //!
    //! This function returns a monoid presentation defining the dual
    //! symmetric inverse monoid of degree \p n, as in Section 3 of
    //! \cite Easdown2007aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    dual_symmetric_inverse_monoid_EEF07(size_t n);

    //! \brief A presentation for a Fibonacci semigroup.
    //!
    //! This function returns a semigroup presentation defining the Fibonacci
    //! semigroup \f$F(r, n)\f$, where \f$r\f$ is \p r and \f$n\f$ is \p n, as
    //! described in \cite Campbell1994aa.
    //!
    //! \param r the length of the left hand sides of the relations.
    //! \param n the number of generators.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 1`.
    //! \throws LibsemigroupsException if `r < 1`.
    [[nodiscard]] Presentation<word_type> fibonacci_semigroup_CRRT94(size_t r,
                                                                     size_t n);

    //! \brief A presentation for the full transformation monoid.
    //!
    //! This function returns a monoid presentation defining the full
    //! transformation monoid of degree \p n, as in Section 5, Theorem 2 of
    //! \cite Aizenstat1958aa (Russian) and Chapter 3, Proposition 1.7 of
    //! \cite Ruskuc1995aa (English).
    //!
    //! \param n the degree of the full transformation monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    [[nodiscard]] Presentation<word_type>
    full_transformation_monoid_Aiz58(size_t n);

    //! \brief A presentation for the full transformation monoid.
    //!
    //! This function returns a monoid presentation defining the full
    //! transformation monoid of degree \p n due to Iwahori and Iwahori
    //! \cite Iwahori1974aa, as in Theorem 9.3.1 of \cite Ganyushkin2009aa.
    //!
    //! \param n the degree of the full transformation monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    [[nodiscard]] Presentation<word_type>
    full_transformation_monoid_II74(size_t n);

    //! \brief A presentation for the full transformation monoid.
    //!
    //! This function returns a monoid presentation defining the full
    //! transformation monoid of degree \p n, corresponding to
    //! \f$\mathcal{T}\f$ in Theorem 1.5 of \cite Mitchell2024aa. For `n >= 4`
    //! this presentation has five non-symmetric-group relations.
    //!
    //! \param n the degree of the full transformation monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type>
    full_transformation_monoid_MW24_a(size_t n);

    //! \brief A presentation for the full transformation monoid.
    //!
    //! This function returns a monoid presentation defining the full
    //! transformation monoid of degree \p n, corresponding to
    //! \f$\mathcal{T}'\f$ in Theorem 1.5 of \cite Mitchell2024aa. This
    //! presentation is only valid for odd values of \p n, and for `n >= 5`
    //! this presentation has four non-symmetric-group relations.
    //!
    //! \param n the degree of the full transformation monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    //! \throws LibsemigroupsException if \p n is not odd.
    [[nodiscard]] Presentation<word_type>
    full_transformation_monoid_MW24_b(size_t n);

    //! \brief A presentation for the hypoplactic monoid.
    //!
    //! This function returns a presentation for the hypoplactic monoid with
    //! \p n generators, as in Definition 4.2 of \cite Novelli2000aa.
    //!
    //! This monoid is a quotient monoid of the plactic monoid, and this
    //! presentation includes the rules from \ref plactic_monoid_Knu70.
    //!
    //! \param n the number of generators.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 1`.
    [[nodiscard]] Presentation<word_type> hypo_plactic_monoid_Nov00(size_t n);

    //! \brief A presentation for the Motzkin monoid.
    //!
    //! This function returns a monoid presentation defining
    //! the Motzkin monoid of degree \p n, as described in Theorem 4.1 of
    //! \cite Posner2013aa, with the additional relations \f$ r_i t_i l_i =
    //! r_i ^ 2 \f$ added to fix a hole in Lemma 4.10 which rendered the
    //! presentation as stated in the paper incorrect.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 1`.
    [[nodiscard]] Presentation<word_type> motzkin_monoid_PHL13(size_t n);

    //! \brief A presentation that incorrectly claims to be the Renner monoid
    //! of type B.
    //!
    //! This functions returns a presentation that incorrectly claims to be
    //! the Renner monoid of type B with size \p l and Iwahori-Hecke
    //! deformation \p q.
    //!
    //! When `q == 0`, this corresponds to Example 7.1.2 of \cite Gay2018aa.
    //!
    //! When `q == 1`, this corresponds to Section 3.2 of \cite Godelle2009aa.
    //!
    //! \param l the size of the monoid.
    //! \param q the Iwahori-Hecke deformation.
    //!
    //! \returns a value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `q != 0` or `q != 1`.
    [[nodiscard]] Presentation<word_type>
    not_renner_type_B_monoid_Gay18(size_t l, int q);

    //! \brief A presentation that incorrectly claims to be the Renner monoid
    //! of type D.
    //!
    //! This functions returns a presentation that incorrectly claims to be
    //! the Renner monoid of type D with size \p l and Iwahori-Hecke
    //! deformation \p q.
    //!
    //! When `q == 1`, this corresponds to Section 3.3 of \cite Godelle2009aa.
    //!
    //! \param l the size of the monoid.
    //! \param q the Iwahori-Hecke deformation.
    //!
    //! \returns a value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `q != 0` or `q != 1`.
    [[nodiscard]] Presentation<word_type>
    not_renner_type_D_monoid_God09(size_t l, int q);

    //! \brief A non-presentation for the symmetric group.
    //!
    //! This function returns a monoid presentation which is claimed to define
    //! the symmetric group of degree \p n, but does not, as in Section 2.2 of
    //! \cite Guralnick2008aa.
    //!
    //! \param n the claimed degree of the symmetric group.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    [[nodiscard]] Presentation<word_type> not_symmetric_group_GKKL08(size_t n);

    //! \brief A presentation for the order preserving part of the cyclic
    //! inverse monoid.
    //!
    //! This function returns a monoid presentation defining the
    //! order preserving part of the cyclic inverse monoid of degree \p n, as
    //! in Theorem 2.17 of \cite Fernandes2022aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    order_preserving_cyclic_inverse_monoid_Fer22(size_t n);

    //! \brief A presentation for the monoid of order preserving mappings.
    //!
    //! This function returns a monoid presentation defining the monoid of
    //! order preserving transformations of degree \p n, as described in
    //! Section 2 of \cite Arthur2000aa.
    //!
    //! This presentation has \f$2n - 2\f$ generators and \f$n^2\f$ relations.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    order_preserving_monoid_AR00(size_t n);

    //! \brief A presentation for the monoid of orientation preserving
    //! mappings.
    //!
    //! This function returns a monoid presentation defining the monoid of
    //! orientation preserving mappings on a finite chain of order \p n, as
    //! described in \cite Arthur2000aa.
    //!
    //! \param n the order of the chain.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    orientation_preserving_monoid_AR00(size_t n);

    //! \brief A presentation for the monoid of orientation preserving or
    //! reversing mappings.
    //!
    //! This function returns a monoid presentation defining the monoid of
    //! orientation preserving or reversing mappings on a finite chain of
    //! order \p n, as described in \cite Arthur2000aa.
    //!
    //! \param n the order of the chain.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    orientation_preserving_reversing_monoid_AR00(size_t n);

    //! \brief A presentation for the partial Brauer monoid.
    //!
    //! This function returns a monoid presentation defining the partial
    //! Brauer monoid of degree \p n, as described in Theorem 5.1 of
    //! \cite Kudryavtseva2006aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 1`.
    [[nodiscard]] Presentation<word_type> partial_brauer_monoid_KM07(size_t n);

    //! \brief A presentation for the monoid of partial isometries of a cycle
    //! graph.
    //!
    //! This function returns a monoid presentation defining the monoid of
    //! partial isometries of an \f$n\f$-cycle graph, as in Theorem 2.8 of
    //! \cite Fernandes2022ab
    //!
    //! \param n the number of vertices of the cycle graph.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    partial_isometries_cycle_graph_monoid_FP22(size_t n);

    //! \brief A presentation for the partial transformation monoid.
    //!
    //! This function returns a monoid presentation defining the partial
    //! transformation monoid of degree \p n, as in Theorem 1.6 of
    //! \cite Mitchell2024aa.
    //!
    //! \param n the degree of the partial transformation monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type>
    partial_transformation_monoid_MW24(size_t n);

    //! \brief A presentation for the partial transformation monoid.
    //!
    //! This function returns a monoid presentation defining the partial
    //! transformation monoid of degree \p n due to Shutov \cite Shutov1960aa,
    //! as in Theorem 9.4.1 of \cite Ganyushkin2009aa.
    //!
    //! \param n the degree of the partial transformation monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    [[nodiscard]] Presentation<word_type>
    partial_transformation_monoid_Shu60(size_t n);

    //! \brief A presentation for the partition monoid.
    //!
    //! This function returns a monoid presentation defining the partition
    //! monoid of degree \p n, as in Theorem 41 of \cite East2011aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    [[nodiscard]] Presentation<word_type> partition_monoid_Eas11(size_t n);

    //! \brief A presentation for the partition monoid.
    //!
    //! This function returns a monoid presentation defining the partition
    //! monoid of degree \p n, as in \cite Halverson2005aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 1`.
    [[nodiscard]] Presentation<word_type> partition_monoid_HR05(size_t n);

    //! \brief A presentation for the plactic monoid.
    //!
    //! This function returns a monoid presentation defining the plactic
    //! monoid with \p n generators, as in see Theorem 6 of \cite Knuth1970aa.
    //!
    //! \param n the number of generators.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type> plactic_monoid_Knu70(size_t n);

    //! \brief A presentation for a rectangular band.
    //!
    //! This function returns a semigroup presentation defining the \p m by \p
    //! n rectangular band, as given in Proposition 4.2 of \cite Ayik2000aa.
    //!
    //! \param m the number of rows.
    //! \param n the number of columns.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `m == 0`.
    //! \throws LibsemigroupsException if `n == 0`.
    [[nodiscard]] Presentation<word_type> rectangular_band_ACOR00(size_t m,
                                                                  size_t n);

    //! \brief A presentation for the Renner monoid of type B.
    //!
    //! This functions returns a presentation for the Renner monoid of type B
    //! with size \p l and Iwahori-Hecke deformation \p q.
    //!
    //! When `q == 0`, this corresponds to Definition 8.4.1 and Example 8.4.2
    //! of \cite Gay2018aa.
    //!
    //! When `q == 1`, this corresponds to Theorem 8.4.19 of \cite Gay2018aa.
    //!
    //! \param l the size of the monoid.
    //! \param q the Iwahori-Hecke deformation.
    //!
    //! \returns a value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `q != 0` or `q != 1`.
    [[nodiscard]] Presentation<word_type> renner_type_B_monoid_Gay18(size_t l,
                                                                     int    q);

    //! \brief A presentation for the Renner monoid of type D.
    //!
    //! This functions returns a presentation for the Renner monoid of type D
    //! with size \p l and Iwahori-Hecke deformation \p q.
    //!
    //! When `q == 0`, this corresponds Definition 8.4.22 of \cite Gay2018aa.
    //!
    //! When `q == 1`, this corresponds to Theorem 8.4.43 of \cite Gay2018aa.
    //!
    //! \param l the size of the monoid.
    //! \param q the Iwahori-Hecke deformation.
    //!
    //! \returns a value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `q != 0` or `q != 1`.
    [[nodiscard]] Presentation<word_type> renner_type_D_monoid_Gay18(size_t l,
                                                                     int    q);

    //! \brief A presentation for the \f$\sigma\f$-plactic monoid.
    //!
    //! This function returns a presentation for the \f$\sigma\f$-plactic
    //! monoid with `sigma.size()` generators as in Section 3.1
    //! \cite Abram2024aa.
    //! The image of \f$\sigma\f$ is given by the values in \p sigma.
    //!
    //! The \f$\sigma\f$-plactic monoid is the quotient of the plactic monoid
    //! by the least congruence containing the relation \f$a^{\sigma(a)} =
    //! a\f$ for each \f$a\f$ in the alphabet. When \f$\sigma(a) = 2\f$ for
    //! all \f$a\f$, the resultant \f$\sigma\f$-plactic monoid is known as the
    //! stylic monoid, and is given in \ref stylic_monoid.
    //!
    //! \param sigma a vector representing the image of \f$\sigma\f$.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `sigma.size() < 1`.
    [[nodiscard]] Presentation<word_type>
    sigma_plactic_monoid_AHMNT24(std::vector<size_t> const& sigma);

    //! \brief A presentation for the singular part of the Brauer monoid.
    //!
    //! This function returns a monoid presentation for the singular part of
    //! the Brauer monoid of degree \p n, as in Theorem 5 of
    //! \cite Maltcev2007aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type> singular_brauer_monoid_MM07(size_t n);

    //! \brief A presentation for the special linear group
    //! \f$\mathrm{SL}(2, q)\f$.
    //!
    //! This function returns a presentation for the special linear group
    //! \f$\mathrm{SL}(2, q)\f$ (also written \f$\mathrm{SL(2,
    //! \mathbb{Z}_q)}\f$), where \p q is an odd prime, as in Theorem 4 of
    //! \cite Campbell1980aa.
    //!
    //! \param q the order of the finite field over which the special linear
    //! group is constructed. This should be an odd prime for the returned
    //! presentation to define claimed group.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `q < 3`.
    [[nodiscard]] Presentation<word_type> special_linear_group_2_CR80(size_t q);

    //! \brief A presentation for the stellar monoid.
    //!
    //! This function returns a monoid presentation defining the stellar
    //! monoid with \p l generators, as in Theorem 4.39 of \cite Gay1999aa.
    //!
    //! \param l the number of generators.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `l < 2`.
    [[nodiscard]] Presentation<word_type> stellar_monoid_GH19(size_t l);

    //! \brief A presentation for the stylic monoid.
    //!
    //! This function returns a monoid presentation defining the stylic monoid
    //! with \p n generators, as in Theorem 8.1 of \cite Abram2022aa.
    //!
    //! \param n the number of generators.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type> stylic_monoid_AR22(size_t n);

    //! \brief A presentation for the symmetric group.
    //!
    //! This function returns a monoid presentation for the symmetric group of
    //! degree \p n, as in p.464 of \cite Burnside2012aa. This presentation
    //! has \f$n - 1\f$ generators and \f$n^3 - 5n^2 + 9n - 5\f$ relations.
    //!
    //! \param n the degree of the symmetric group.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type> symmetric_group_Bur12(size_t n);

    //! \brief A presentation for the symmetric group.
    //!
    //! This function returns a monoid presentation for the symmetric group of
    //! degree \p n, as on page 169 of \cite Carmichael1956aa. This
    //! presentation has \f$n - 1\f$ generators and \f$(n - 1)^2\f$ relations.
    //!
    //! \param n the degree of the symmetric group.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type> symmetric_group_Car56(size_t n);

    //! \brief A presentation for the symmetric group.
    //!
    //! This function returns a monoid presentation for the symmetric group of
    //! degree \p n, as in Theorem A of \cite Moore1897aa. This  presentation
    //! has \f$n - 1\f$ generators and \f$\frac{1}{2}n(n - 1)\f$
    //! relations.
    //!
    //! \param n the degree of the symmetric group.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type> symmetric_group_Moo97_a(size_t n);

    //! \brief A presentation for the symmetric group.
    //!
    //! This function returns a monoid presentation for the symmetric group of
    //! degree \p n, as in in Theorem A' of \cite Moore1897aa. This
    //! presentation has \f$2\f$ generators and \f$n + 1\f$ relations for \f$n
    //! \geq 4\f$. If \f$n<4\f$ then there are \f$4\f$ relations.
    //!
    //! \param n the degree of the symmetric group.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    [[nodiscard]] Presentation<word_type> symmetric_group_Moo97_b(size_t n);

    //! \brief A presentation for the symmetric inverse monoid.
    //!
    //! This function returns a monoid presentation defining the symmetric
    //! inverse monoid of degree \p n, as in Example 7.1.2 of \cite Gay2018aa.
    //!
    //!
    //! \param n the degree of the symmetric inverse monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    // This is just a presentation for the symmetric inverse monoid, a
    // slightly modified version from Solomon (so that contains the
    // Coxeter+Moser presentation for the symmetric group), Example 7.1.2 in
    // Joel gay's thesis (JDM the presentation in Example 7.1.2 seems to have
    // 2n - 1 generators whereas this function returns a monoid on n
    // generators. TODO ask Florent again if this reference is correct Maybe
    // should be Solomon:
    // https://www.sciencedirect.com/science/article/pii/S0021869303005933/pdf
    [[nodiscard]] Presentation<word_type>
    symmetric_inverse_monoid_Gay18(size_t n);

    //! \brief A presentation for the symmetric inverse monoid.
    //!
    //! This function returns a monoid presentation defining the partial
    //! transformation monoid of degree \p n, as in Theorem 1.4 of
    //! \cite Mitchell2024aa.
    //!
    //!
    //! \param n the degree of the symmetric inverse monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    [[nodiscard]] Presentation<word_type>
    symmetric_inverse_monoid_MW24(size_t n);

    //! \brief A presentation for the symmetric inverse monoid.
    //!
    //! This function returns a monoid presentation defining the symmetric
    //! inverse monoid of degree \p n due to Shutov \cite Shutov1960aa, as in
    //! Theorem 9.2.2 of \cite Ganyushkin2009aa.
    //!
    //! \param n the degree of the symmetric inverse monoid.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    [[nodiscard]] Presentation<word_type>
    symmetric_inverse_monoid_Shu60(size_t n);

    //! \brief A presentation for the Temperley-Lieb monoid.
    //!
    //! This function returns a monoid presentation defining the
    //! Temperley-Lieb monoid with \p n generators, as described in
    //! Theorem 2.2 of \cite East2022aa.
    //!
    //! \param n the number of generators.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type> temperley_lieb_monoid_Eas21(size_t n);

    //! \brief A presentation for the uniform block bijection monoid.
    //!
    //! This function returns a monoid presentation defining the uniform block
    //! bijection monoid of degree \p n, as in \cite FitzGerald2003aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 3`.
    [[nodiscard]] Presentation<word_type>
    uniform_block_bijection_monoid_Fit03(size_t n);

    // TODO add okada_monoid
    // TODO add free_semilattice

    //! \brief A presentation for the \f$0\f$-rook monoid.
    //!
    //! This function returns a presentation for the \f$0\f$-rook monoid of
    //! degree \p n, as in Definition 4.1.1 in \cite Gay2018aa
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 2`.
    // When q = 0, the a_i^2 = 1 relations from the C+M symmetric group
    // presentation are replaced with a_i^2 = a_i. See Definition 4.1.1 in
    // Joel Gay's thesis https://theses.hal.science/tel-01861199
    //
    //  This could also be called the RennerTypeAMonoid
    [[nodiscard]] Presentation<word_type> zero_rook_monoid_Gay18(size_t n);

    ///@}

    //! \anchor default_presentations_member_group
    //!
    //! \name Default presentations
    //!
    //! For some semigroups and monoids presented on this page, there are
    //! multiple presentations. The section below defines some default
    //! functions; functions that should be used when the particular relations
    //! aren't important, only the semigroup or monoid they define.
    ///@{

    //! \copydoc alternating_group_Moo97
    //!
    //! \note
    //! This function performs exactly the same as `alternating_group_Moo97`,
    //! and exists as a convenience function for when a presentation for the
    //! alternating group is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! `alternating_group_Moo97`.
    [[nodiscard]] inline Presentation<word_type> alternating_group(size_t n) {
      return alternating_group_Moo97(n);
    }

    //! \copydoc brauer_monoid_KM07
    //!
    //! \note
    //! This function performs exactly the same as `brauer_monoid_KM07`, and
    //! exists as a convenience function for when a presentation for the
    //! Brauer monoid is required, but the relations of the presentation are
    //! not important.
    //!
    //! \sa
    //! `brauer_monoid_KM07`.
    [[nodiscard]] inline Presentation<word_type> brauer_monoid(size_t n) {
      return brauer_monoid_KM07(n);
    }

    //! \copydoc chinese_monoid_CEKNH01
    //!
    //! \note
    //! This function performs exactly the same as
    //! `chinese_monoid_CEKNH01`, and exists as a convenience
    //! function for when a presentation for the Chinese monoid is required,
    //! but the relations of the presentation are not important.
    //!
    //! \sa
    //! `chinese_monoid_CEKNH01`.
    [[nodiscard]] inline Presentation<word_type> chinese_monoid(size_t n) {
      return chinese_monoid_CEKNH01(n);
    }

    //! \copydoc cyclic_inverse_monoid_Fer22_b
    //!
    //! \note
    //! This function performs exactly the same as
    //! `cyclic_inverse_monoid_Fer22_b`, and exists as a convenience
    //! function for when a presentation for the cyclic inverse monoid
    //! is required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! For a specific %presentation of the full transformation monoid, see
    //! %one of the following functions:
    //! * `cyclic_inverse_monoid_Fer22_a`;
    //! * `cyclic_inverse_monoid_Fer22_b`.
    [[nodiscard]] inline Presentation<word_type>
    cyclic_inverse_monoid(size_t n) {
      return cyclic_inverse_monoid_Fer22_b(n);
    }

    //! \copydoc dual_symmetric_inverse_monoid_EEF07
    //!
    //! \note
    //! This function performs exactly the same as
    //! `dual_symmetric_inverse_monoid_EEF07`, and exists as a convenience
    //! function for when a presentation for the dual symmetric inverse monoid
    //! is required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! `dual_symmetric_inverse_monoid`.
    [[nodiscard]] inline Presentation<word_type>
    dual_symmetric_inverse_monoid(size_t n) {
      return dual_symmetric_inverse_monoid_EEF07(n);
    }

    //! \copydoc fibonacci_semigroup_CRRT94
    //!
    //! \note
    //! This function performs exactly the same as
    //! `fibonacci_semigroup_CRRT94`, and exists as a convenience function for
    //! when a presentation for the Fibonacci semigroup is required, but the
    //! relations of the presentation are not important.
    //!
    //! \sa
    //! `fibonacci_semigroup_CRRT94`.
    [[nodiscard]] inline Presentation<word_type> fibonacci_semigroup(size_t r,
                                                                     size_t n) {
      return fibonacci_semigroup_CRRT94(r, n);
    }

    //! \copydoc full_transformation_monoid_MW24_a
    //!
    //! \note
    //! This function performs exactly the same as
    //! `full_transformation_monoid_MW24_a`, and exists as a convenience
    //! function for when a presentation for the full transformation monoid is
    //! required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! For a specific %presentation of the full transformation monoid, see
    //! %one of the following functions:
    //! * `full_transformation_monoid_Aiz58`;
    //! * `full_transformation_monoid_II74`;
    //! * `full_transformation_monoid_MW24_a`;
    //! * `full_transformation_monoid_MW24_b`.
    [[nodiscard]] inline Presentation<word_type>
    full_transformation_monoid(size_t n) {
      return full_transformation_monoid_MW24_a(n);
    }

    //! \copydoc hypo_plactic_monoid_Nov00
    //!
    //! \note
    //! This function performs exactly the same as
    //! `hypo_plactic_monoid_Nov00`, and exists as a convenience function for
    //! when a presentation for the hypoplactic monoid is required, but the
    //! relations of the presentation are not important.
    //!
    //! \sa
    //! `hypo_plactic_monoid_Nov00`.
    [[nodiscard]] inline Presentation<word_type> hypo_plactic_monoid(size_t n) {
      return hypo_plactic_monoid_Nov00(n);
    }

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
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `r == 0`.
    [[nodiscard]] Presentation<word_type> monogenic_semigroup(size_t m,
                                                              size_t r);

    //! \copydoc motzkin_monoid_PHL13
    //!
    //! \note
    //! This function performs exactly the same as `motzkin_monoid_PHL13`,
    //! and exists as a convenience function for when a presentation for the
    //! Motzkin monoid is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! `motzkin_monoid_PHL13`.
    [[nodiscard]] inline Presentation<word_type> motzkin_monoid(size_t n) {
      return motzkin_monoid_PHL13(n);
    }

    //! \copydoc not_renner_type_B_monoid_Gay18
    //!
    //! \note
    //! This function performs exactly the same as
    //! `not_renner_type_B_monoid_Gay18`, and exists as a convenience function
    //! for when a presentation that incorrectly claims to be the Renner
    //! monoid of type B is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! `not_renner_type_B_monoid_Gay18`.
    [[nodiscard]] inline Presentation<word_type>
    not_renner_type_B_monoid(size_t l, int q) {
      return not_renner_type_B_monoid_Gay18(l, q);
    }

    //! \copydoc not_renner_type_D_monoid_God09
    //!
    //! \note
    //! This function performs exactly the same as
    //! `not_renner_type_D_monoid_God09`, and exists as a convenience function
    //! for when a presentation that incorrectly claims to be the Renner
    //! monoid of type D is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! `not_renner_type_D_monoid_God09`
    [[nodiscard]] inline Presentation<word_type>
    not_renner_type_D_monoid(size_t l, int q) {
      return not_renner_type_D_monoid_God09(l, q);
    }

    //! \copydoc not_symmetric_group_GKKL08
    //!
    //! \note
    //! This function performs exactly the same as
    //! `not_symmetric_group_GKKL08`, and exists as a convenience function for
    //! when a presentation that incorrectly claims to be the symmetric group
    //! required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! `not_symmetric_group_GKKL08`.
    [[nodiscard]] inline Presentation<word_type> not_symmetric_group(size_t n) {
      return not_symmetric_group_GKKL08(n);
    }

    //! \copydoc order_preserving_cyclic_inverse_monoid_Fer22
    //!
    //! \note
    //! This function performs exactly the same as
    //! `order_preserving_cyclic_inverse_monoid_Fer22`, and exists as a
    //! convenience function for when a presentation for the order preserving
    //! part of the cyclic inverse monoid is required, but the relations of
    //! the presentation are not important.
    //!
    //! \sa
    //! `order_preserving_cyclic_inverse_monoid_Fer22`.
    [[nodiscard]] inline Presentation<word_type>
    order_preserving_cyclic_inverse_monoid(size_t n) {
      return order_preserving_cyclic_inverse_monoid_Fer22(n);
    }

    //! \copydoc order_preserving_monoid_AR00
    //!
    //! \note
    //! This function performs exactly the same as
    //! `order_preserving_monoid_AR00`, and exists as a convenience
    //! function for when a presentation for the order preserving monoid is
    //! required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! `order_preserving_monoid_AR00`.
    [[nodiscard]] inline Presentation<word_type>
    order_preserving_monoid(size_t n) {
      return order_preserving_monoid_AR00(n);
    }

    //! \copydoc orientation_preserving_monoid_AR00
    //!
    //! \note
    //! This function performs exactly the same as
    //! `orientation_preserving_monoid_AR00`, and exists as a convenience
    //! function for when a presentation for the the monoid of
    //! orientation preserving mappings on a finite chain is required,
    //! but the relations of the presentation are not important.
    //!
    //! \sa
    //! `orientation_preserving_monoid_AR00`.
    [[nodiscard]] inline Presentation<word_type>
    orientation_preserving_monoid(size_t n) {
      return orientation_preserving_monoid_AR00(n);
    }

    //! \copydoc orientation_preserving_reversing_monoid_AR00
    //!
    //! \note
    //! This function performs exactly the same as
    //! `orientation_preserving_reversing_monoid_AR00`, and exists as a
    //! convenience function for when a presentation for the monoid of
    //! orientation preserving or reversing mappings on a finite chain is
    //! required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! `orientation_preserving_reversing_monoid_AR00`.
    [[nodiscard]] inline Presentation<word_type>
    orientation_preserving_reversing_monoid(size_t n) {
      return orientation_preserving_reversing_monoid_AR00(n);
    }

    //! \copydoc partial_brauer_monoid_KM07
    //!
    //! \note
    //! This function performs exactly the same as
    //! `partial_brauer_monoid_KM07`, and exists as a convenience function for
    //! when a presentation for the partial Brauer monoid is required, but the
    //! relations of the presentation are not important.
    //!
    //! \sa
    //! `partial_brauer_monoid_KM07`.
    [[nodiscard]] inline Presentation<word_type>
    partial_brauer_monoid(size_t n) {
      return partial_brauer_monoid_KM07(n);
    }

    //! \copydoc partial_isometries_cycle_graph_monoid_FP22
    //!
    //! \note
    //! This function performs exactly the same as
    //! `partial_isometries_cycle_graph_monoid_FP22`, and exists as a
    //! convenience function for when a presentation for the monoid of
    //! partial isometries of an \f$n\f$-cycle graph is required, but the
    //! relations of the presentation are not important.
    //!
    //! \sa
    //! `partial_isometries_cycle_graph_monoid_FP22`.
    [[nodiscard]] inline Presentation<word_type>
    partial_isometries_cycle_graph_monoid(size_t n) {
      return partial_isometries_cycle_graph_monoid_FP22(n);
    }

    //! \copydoc partial_transformation_monoid_MW24
    //!
    //! \note
    //! This function performs exactly the same as
    //! `partial_transformation_monoid_MW24`, and exists as a convenience
    //! function for when a presentation for the partial transformation monoid
    //! is required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! For a specific %presentation of the full transformation monoid, see
    //! %one of the following functions:
    //! * `partial_transformation_monoid_Shu60`;
    //! * `partial_transformation_monoid_MW24`.
    [[nodiscard]] inline Presentation<word_type>
    partial_transformation_monoid(size_t n) {
      return partial_transformation_monoid_MW24(n);
    }

    //! \brief A presentation for the partition monoid.
    //!
    //! This function returns a monoid presentation defining the partition
    //! monoid of degree \p n, as in Theorem 41 of \cite East2011aa.
    //!
    //! \param n the degree.
    //!
    //! \returns A value of type `Presentation<word_type>`.
    //!
    //! \throws LibsemigroupsException if `n < 4`.
    //!
    //! \note
    //! This function performs exactly the same as `partition_monoid_Eas11`,
    //! and exists as a convenience function for when a presentation for the
    //! partition monoid is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! For a specific %presentation of the full transformation monoid, see
    //! %one of the following functions:
    //! * `partition_monoid_Eas11`;
    //! * `partition_monoid_HR05`.
    [[nodiscard]] inline Presentation<word_type> partition_monoid(size_t n) {
      return partition_monoid_Eas11(n);
    }

    //! \copydoc plactic_monoid_Knu70
    //!
    //! \note
    //! This function performs exactly the same as `plactic_monoid_Knu70`,
    //! and exists as a convenience function for when a presentation for the
    //! plactic monoid is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! `plactic_monoid_Knu70`.
    [[nodiscard]] inline Presentation<word_type> plactic_monoid(size_t n) {
      return plactic_monoid_Knu70(n);
    }

    //! \copydoc rectangular_band_ACOR00
    //!
    //! \note
    //! This function performs exactly the same as `rectangular_band_ACOR00`,
    //! and exists as a convenience function for when a presentation for a
    //! rectangular band is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! `rectangular_band_ACOR00`.
    [[nodiscard]] inline Presentation<word_type> rectangular_band(size_t m,
                                                                  size_t n) {
      return rectangular_band_ACOR00(m, n);
    }

    //! \copydoc renner_type_B_monoid_Gay18
    //!
    //! \note
    //! This function performs exactly the same as
    //! `renner_type_B_monoid_Gay18`, and exists as a convenience function for
    //! when a presentation for the Renner monoid of type B is required, but
    //! the relations of the presentation are not important.
    //!
    //! \sa
    //! `renner_type_B_monoid_Gay18`.
    [[nodiscard]] inline Presentation<word_type> renner_type_B_monoid(size_t l,
                                                                      int q) {
      return renner_type_B_monoid_Gay18(l, q);
    }

    //! \copydoc renner_type_D_monoid_Gay18
    //!
    //! \note
    //! This function performs exactly the same as
    //! `renner_type_D_monoid_Gay18`, and exists as a convenience function for
    //! when a presentation for the Renner monoid of type D is required, but
    //! the relations of the presentation are not important.
    //!
    //! \sa
    //! `renner_type_D_monoid_Gay18`
    [[nodiscard]] inline Presentation<word_type> renner_type_D_monoid(size_t l,
                                                                      int q) {
      return renner_type_D_monoid_Gay18(l, q);
    }

    //! \copydoc sigma_plactic_monoid_AHMNT24
    //!
    //! \note
    //! This function performs exactly the same as
    //! `sigma_plactic_monoid_AHMNT24`, and exists as a convenience function for
    //! when a presentation for the \f$\sigma\f$-plactic monoid is required, but
    //! the relations of the presentation are not important.
    //!
    //! \sa
    //! `sigma_plactic_monoid_AHMNT24`
    [[nodiscard]] inline Presentation<word_type>
    sigma_plactic_monoid(std::vector<size_t> const& sigma) {
      return sigma_plactic_monoid_AHMNT24(sigma);
    }

    //! \copydoc singular_brauer_monoid_MM07
    //!
    //! \note
    //! This function performs exactly the same as
    //! `singular_brauer_monoid_MM07`, and exists as a convenience function
    //! for when a presentation for the singular part of the brauer monoid is
    //! required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! `singular_brauer_monoid_MM07`.
    [[nodiscard]] inline Presentation<word_type>
    singular_brauer_monoid(size_t n) {
      return singular_brauer_monoid_MM07(n);
    }

    //! \copydoc special_linear_group_2_CR80
    //!
    //! \note
    //! This function performs exactly the same as
    //! `special_linear_group_2_CR80`, and exists as a convenience function
    //! for when a presentation for the special linear group
    //! \f$\mathrm{SL}(2,q)\f$ is required, but the relations of the
    //! presentation are not important.
    //!
    //! \sa
    //! `special_linear_group_2_CR80`.
    [[nodiscard]] inline Presentation<word_type>
    special_linear_group_2(size_t q) {
      return special_linear_group_2_CR80(q);
    }

    //! \copydoc stellar_monoid_GH19
    //!
    //! \note
    //! This function performs exactly the same as `stellar_monoid_GH19`, and
    //! exists as a convenience function for when a presentation for the
    //! stellar monoid is required, but the relations of the presentation are
    //! not important.
    //!
    //! \sa
    //! `stellar_monoid_GH19`.
    [[nodiscard]] inline Presentation<word_type> stellar_monoid(size_t l) {
      return stellar_monoid_GH19(l);
    }

    //! \copydoc stylic_monoid_AR22
    //!
    //! \note
    //! This function performs exactly the same as `stylic_monoid_AR22`,
    //! and exists as a convenience function for when a presentation for the
    //! stylic monoid is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! `stylic_monoid_AR22`.
    [[nodiscard]] inline Presentation<word_type> stylic_monoid(size_t n) {
      return stylic_monoid_AR22(n);
    }

    //! \copydoc symmetric_group_Car56
    //!
    //! \note
    //! This function performs exactly the same as `symmetric_group_Car56`,
    //! and exists as a convenience function for when a presentation for the
    //! symmetric group is required, but the relations of the presentation are
    //! not important.
    //!
    //! \sa
    //! For a specific %presentation of the symmetric group, see %one of the
    //! following functions:
    //! * `symmetric_group_Bur12`;
    //! * `symmetric_group_Car56`;
    //! * `symmetric_group_Moo97_a`;
    //! * `symmetric_group_Moo97_b`.
    [[nodiscard]] inline Presentation<word_type> symmetric_group(size_t n) {
      return symmetric_group_Car56(n);
    }

    //! \copydoc partial_transformation_monoid_MW24
    //!
    //! \note
    //! This function performs exactly the same as
    //! `symmetric_inverse_monoid_MW24`, and exists as a convenience
    //! function for when a presentation for the symmetric inverse monoid
    //! is required, but the relations of the presentation are not important.
    //!
    //! \sa
    //! For a specific %presentation of the full transformation monoid, see
    //! %one of the following functions:
    //! * `symmetric_inverse_monoid_Gay18`;
    //! * `symmetric_inverse_monoid_Shu60`;
    //! * `symmetric_inverse_monoid_MW24`.
    [[nodiscard]] inline Presentation<word_type>
    symmetric_inverse_monoid(size_t n) {
      return symmetric_inverse_monoid_MW24(n);
    }

    //! \copydoc temperley_lieb_monoid_Eas21
    //!
    //! \note
    //! This function performs exactly the same as
    //! `temperley_lieb_monoid_Eas21`, and exists as a convenience function
    //! for when a presentation for the Temperley-Lieb monoid is required, but
    //! the relations of the presentation are not important.
    //!
    //! \sa
    //! `temperley_lieb_monoid_Eas21`.
    [[nodiscard]] inline Presentation<word_type>
    temperley_lieb_monoid(size_t n) {
      return temperley_lieb_monoid_Eas21(n);
    }

    //! \copydoc uniform_block_bijection_monoid_Fit03
    //!
    //! \note
    //! This function performs exactly the same as
    //! `uniform_block_bijection_monoid_Fit03`, and exists as a convenience
    //! function for when a presentation for the uniform block bijection
    //! monoid is required, but the relations of the presentation are not
    //! important.
    //!
    //! \sa
    //! `uniform_block_bijection_monoid_Fit03`.
    [[nodiscard]] inline Presentation<word_type>
    uniform_block_bijection_monoid(size_t n) {
      return uniform_block_bijection_monoid_Fit03(n);
    }

    //! \copydoc zero_rook_monoid_Gay18
    //!
    //! \note
    //! This function performs exactly the same as `zero_rook_monoid_Gay18`,
    //! and exists as a convenience function for when a presentation for the
    //! \f$0\f$-rook monoid is required, but the relations of the presentation
    //! are not important.
    //!
    //! \sa
    //! `zero_rook_monoid_Gay18`.
    [[nodiscard]] inline Presentation<word_type> zero_rook_monoid(size_t n) {
      return zero_rook_monoid_Gay18(n);
    }
    ///@}

  }  // namespace presentation::examples
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FPSEMI_EXAMPLES_HPP_
