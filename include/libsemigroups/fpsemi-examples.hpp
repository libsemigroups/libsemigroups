//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 Florent Hivert
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

#ifndef LIBSEMIGROUPS_FPSEMI_EXAMPLES_HPP_
#define LIBSEMIGROUPS_FPSEMI_EXAMPLES_HPP_

#include <cctype>         // for toupper
#include <cstddef>        // for size_t
#include <string>         // for basic_string, operator+
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "libsemigroups/present.hpp"  // for Presentation
#include "libsemigroups/types.hpp"    // for relation_type

namespace libsemigroups {
  enum class author : uint64_t {
    Machine    = 0,
    Aizenstat  = 1,
    Burnside   = 2,
    Carmichael = 4,
    Coxeter    = 8,
    Easdown    = 16,
    East       = 32,
    FitzGerald = 64,
    Godelle    = 128,
    Guralnick  = 256,
    Iwahori    = 512,
    Kantor     = 1024,
    Kassabov   = 2048,
    Lubotzky   = 4096,
    Miller     = 8192,
    Moore      = 16384,
    Moser      = 32768,
    Sutov      = 65536
  };

  inline author operator+(author auth1, author auth2) {
    return static_cast<author>(static_cast<uint64_t>(auth1)
                               + static_cast<uint64_t>(auth2));
  }

  std::vector<relation_type> rook_monoid(size_t l, int q);

  // Construct a presentation for the Renner common type B monoid.
  //
  // Returns a vector of relations defining the Renner common type B monoid with
  // parameters `l` and `q`.
  //
  // \param l the degree
  // \param q the parameter `q`
  //
  // \returns A `std::vector<relation_type>`
  //
  // \noexcept
  std::vector<relation_type> renner_common_type_B_monoid(size_t l, int q);

  std::vector<relation_type> RennerTypeBMonoid(size_t l, int q);

  // Construct a presentation for the Renner type B monoid.
  //
  // Returns a vector of relations defining the Renner type B monoid with
  // parameters `l` and `q`.
  //
  // The argument `val` determines the specific presentation
  // which is returned. The options are:
  // * `author::Godelle` (described
  // [here](https://doi.org/10.48550/arXiv.0904.0926), and is possibly
  // incorrect)

  // \param l the parameter `l`
  // \param q the parameter `q`
  // \param val the author value
  //
  // \returns A `std::vector<relation_type>`
  //
  // \throws LibsemigroupsException if the author value is not
  // `author::Godelle`.
  std::vector<relation_type> renner_type_B_monoid(size_t l, int q, author val);

  // Construct a presentation for the Renner common type D monoid.
  //
  // Returns a vector of relations defining the Renner common type D monoid with
  // parameters `l` and `q`.
  //
  // \param l the parameter `l`
  // \param q the parameter `q`
  //
  // \returns A `std::vector<relation_type>`
  //
  // \noexcept
  std::vector<relation_type> renner_common_type_D_monoid(size_t l, int q);

  // Construct a presentation for the Renner type D monoid.
  //
  // Returns a vector of relations defining the Renner type D monoid with
  // parameters `l` and `q`.
  //
  // The argument `val` determines the specific presentation
  // which is returned. The options are:
  // * `author::Godelle` (described
  // in [10.48550/arXiv.0904.0926](https://doi.org/10.48550/arXiv.0904.0926),
  // and is possibly incorrect)

  // \param l the parameter `l`
  // \param q the parameter `q`
  // \param val the author value
  //
  // \returns A `std::vector<relation_type>`
  //
  // \throws LibsemigroupsException if the author value is not
  // `author::Godelle`.
  std::vector<relation_type> renner_type_D_monoid(size_t l, int q, author val);

  std::vector<relation_type> RennerTypeDMonoid(size_t l, int q);

  //! Construct a presentation for the stellar monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! stellar monoid with `n` generators, as in Theorem 4.39 of
  //! [10.48550/arXiv.1910.11740](https://doi.org/10.48550/arXiv.1910.11740).
  //!
  //! \param n the number of generators
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \throws LibsemigroupsException if `n < 3`, or the author value is not
  //! `author::Easdown + author::East + author::FitzGerald`.
  std::vector<relation_type> stellar_monoid(size_t l);

  //! Construct a presentation for the dual symmetric inverse monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! dual symmetric inverse monoid of degree `n`. The argument `val` determines
  //! the specific presentation which is returned. The options are:
  //! * `author::Easdown + author::East + author::FitzGerald` (from Section 3 of
  //! [10.48550/arxiv.0707.2439](https://doi.org/10.48550/arxiv.0707.2439))
  //!
  //! \param n the degree
  //! \param val the author of the presentation
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \throws LibsemigroupsException if `n < 3`, or the author value is not
  //! `author::Easdown + author::East + author::FitzGerald`.
  std::vector<relation_type> dual_symmetric_inverse_monoid(size_t n,
                                                           author val);

  //! Construct a presentation for the uniform block bijection monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! uniform block bijection monoid of degree `n`. The argument `val`
  //! determines the specific presentation which is returned. The only option
  //! is:
  //! * `author::FitzGerald` (see
  //! [10.1017/s0004972700037692](https://doi.org/10.1017/s0004972700037692))
  //!
  //! \param n the degree
  //! \param val the author of the presentation
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \throws LibsemigroupsException if `n < 3`, or the author value is not
  //! `author::FitzGerald`.
  std::vector<relation_type> uniform_block_bijection_monoid(size_t n,
                                                            author val);

  //! Construct a presentation for the partition monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! partition monoid of degree `n`. The argument `val` determines the specific
  //! presentation which is returned. The options are:
  //! * `author::Machine`
  //! * `author::East` (see Theorem 41 of
  //! [10.1016/j.jalgebra.2011.04.008](https://doi.org/10.1016/j.jalgebra.2011.04.008))
  //!
  //! \param n the degree
  //! \param val the author of the presentation
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \throws LibsemigroupsException if (`val = author::Machine` and `n != 3`),
  //! or `(val = author::East` and `n < 4`).
  std::vector<relation_type> partition_monoid(size_t n, author val);

  //! Construct a presentation for the singular part of the Brauer monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation for the
  //! singular part of the Brauer monoid of degree `n`, as in Theorem 5 of
  //! [10.21136/MB.2007.134125](https://doi.org/10.21136/MB.2007.134125).
  //!
  //! \param n the degree
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> singular_brauer_monoid(size_t n);

  //! Construct a presentation for the monoid of orientation preserving
  //! mappings.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! monoid of orientation preserving mappings on a finite chain of order `n`,
  //! as described in
  //! [10.1007/s10012-000-0001-1](https://doi.org/10.1007/s10012-000-0001-1).
  //!
  //! \param n the order
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> orientation_preserving_monoid(size_t n);

  //! Construct a presentation for the monoid of orientation reversing mappings.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! monoid of orientation reversing mappings on a finite chain of order `n`,
  //! as described in
  //! [10.1007/s10012-000-0001-1](https://doi.org/10.1007/s10012-000-0001-1).
  //!
  //! \param n the order
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> orientation_reversing_monoid(size_t n);

  //! Construct a presentation for the Temperley-Lieb monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! Temperley-Lieb monoid with `n` generators, as described in Theorem 2.2 of
  //! [10.1093/qmath/haab001](https://doi.org/10.1093/qmath/haab001).
  //!
  //! \param n the number of generators
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> temperley_lieb_monoid(size_t n);

  //! Construct a presentation for the Brauer monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! Brauer monoid of degree `n`, as described in Theorem 3.1 of the paper
  //! [10.2478/s11533-006-0017-6](https://doi.org/10.2478/s11533-006-0017-6).
  //!
  //! \param n the degree
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> brauer_monoid(size_t n);

  //! Construct a presentation for a Fibonacci semigroup.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! Fibonacci semigroup \f$F(r, n)\f$, as described in
  //! [10.1016/0022-4049(94)90005-1](https://doi.org/10.1016/0022-4049(94)90005-1).
  //!
  //! \param r the parameter `r`
  //! \param n the number of generators
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> fibonacci_semigroup(size_t r, size_t n);

  //! Construct a presentation for the plactic monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! plactic monoid with `n` generators (see Section 3 of
  //! [10.1007/s00233-022-10285-3](https://doi.org/10.1007/s00233-022-10285-3)).
  //!
  //! \param n the number of generators
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> plactic_monoid(size_t n);

  //! Construct a presentation for the stylic monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! stylic monoid with `n` generators (see Theorem 8.1 of
  //! [10.1007/s00233-022-10285-3](https://doi.org/10.1007/s00233-022-10285-3)).
  //!
  //! \param n the number of generators
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> stylic_monoid(size_t n);

  //! Construct a presentation for the symmetric group.
  //!
  //! Returns a vector of relations giving a monoid presentation for the
  //! symmetric group. The argument `val` determines the specific presentation
  //! which is returned. The options are:
  //! * `author::Burnside + author::Miller` (given on p.464 of
  //! [10.1017/CBO9781139237253](https://doi.org/10.1017/CBO9781139237253))
  //! * `author::Carmichael` (given in comment 9.5.2 of
  //! [10.1007/978-1-84800-281-4](https://doi.org/10.1007/978-1-84800-281-4))
  //! * `author::Coxeter + author::Moser` (see Ch. 3, Prop 1.2 of
  //! [hdl.handle.net/10023/2821](http://hdl.handle.net/10023/2821))
  //! * `author::Moore` (given in comment 9.5.3 of
  //! [10.1007/978-1-84800-281-4](https://doi.org/10.1007/978-1-84800-281-4))
  //!
  //! \param n the degree of the symmetric group
  //! \param val the author of the presentation
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \throws LibsemigroupsException if `val` is not listed above (modulo order
  //! of author), or `n < 4`.
  std::vector<relation_type> symmetric_group(size_t n, author val);

  //! Construct a presentation for a rectangular band.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! `m` by `n` rectangular band, as given in Proposition 4.2 of
  //! [10.1007/s002339910016](https://doi.org/10.1007/s002339910016).
  //!
  //! \param m the value `m`
  //! \param n the value `n`
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> rectangular_band(size_t m, size_t n);

  //! Construct a presentation for the full transformation monoid.
  //!
  //! Returns a vector of relations giving a monoid presentation defining the
  //! full transformation monoid. The argument `val` determines the specific
  //! presentation which is returned. The options are:
  //! * `author::Aizenstat` (see Ch. 3, Prop 1.7 of
  //! [http://hdl.handle.net/10023/2821](http://hdl.handle.net/10023/2821))
  //! * `author::Iwahori` (see Theorem 9.3.1 of
  //! [10.1007/978-1-84800-281-4](https://doi.org/10.1007/978-1-84800-281-4))
  //!
  //!
  //! \param n the degree of the full transformation monoid
  //! \param val the author of the presentation
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \throws LibsemigroupsException if `val` is not listed above (modulo order
  //! of author).
  std::vector<relation_type> full_transformation_monoid(size_t n, author val);

  //! Construct a presentation for the partial transformation monoid.
  //!
  //! Returns a vector of relations giving a monoid presentation defining the
  //! partial transformation monoid. The argument `val` determines the specific
  //! presentation which is returned. The options are:
  //! * `author::Machine`
  //! * `author::Sutov` (see Theorem 9.4.1 of
  //! [10.1007/978-1-84800-281-4](https://doi.org/10.1007/978-1-84800-281-4))
  //!
  //! \param n the degree of the partial transformation monoid
  //! \param val the author of the presentation
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \throws LibsemigroupsException if `val` is not listed above (modulo order
  //! of author).
  std::vector<relation_type> partial_transformation_monoid(size_t n,
                                                           author val);

  //! Construct a presentation for the symmetric inverse monoid.
  //!
  //! Returns a vector of relations giving a monoid presentation defining the
  //! symmetric inverse monoid. The argument `val` determines the specific
  //! presentation which is returned. The options are:
  //! * `author::Sutov` (see Theorem 9.2.2 of
  //! [10.1007/978-1-84800-281-4](https://doi.org/10.1007/978-1-84800-281-4))
  //!
  //! \param n the degree of the symmetric inverse monoid
  //! \param val the author of the presentation
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \throws LibsemigroupsException if `val` is not listed above (modulo order
  //! of author).
  std::vector<relation_type> symmetric_inverse_monoid(size_t n, author val);

  //! Construct a presentation for the Chinese monoid.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! Chinese monoid, as described in
  //! [10.1142/S0218196701000425](https://doi.org/10.1142/S0218196701000425).
  //!
  //! \param n the number of generators
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> chinese_monoid(size_t n);

  //! Construct a presentation for a monogenic semigroup.
  //!
  //! Returns a vector of relations giving a semigroup presentation defining the
  //! monogenic semigroup defined by the presentation \f$\langle a \mid a^{m +
  //! r} = a^m \rangle\f$.
  //!
  //! \param m the parameter `m`
  //! \param r the parameter `r`
  //!
  //! \returns A `std::vector<relation_type>`
  //!
  //! \noexcept
  std::vector<relation_type> monogenic_semigroup(size_t m, size_t r);

  void
  add_full_transformation_monoid_relations(std::vector<relation_type>& result,
                                           size_t                      n,
                                           size_t                      pi_start,
                                           size_t e12_value);

  template <typename T, typename F, typename... Args>
  void setup(T& tc, size_t num_gens, F func, Args... args) {
    tc.set_number_of_generators(num_gens);
    for (auto const& w : func(args...)) {
      tc.add_pair(w.first, w.second);
    }
  }

  template <typename T>
  T make_group(T& M) {
    std::string              id = "e";
    std::unordered_set<char> self_inverse;
    self_inverse.emplace('e');

    for (auto it = M.cbegin_rules(); it != M.cend_rules(); ++it) {
      if (it->second == id && it->first.size() == 2
          && it->first[0] == it->first[1]) {
        // Remove self-inverse elements
        self_inverse.emplace(it->first[0]);
      } else if (it->first == id && it->second.size() == 2
                 && it->second[0] == it->second[1]) {
        // Remove self-inverse elements
        self_inverse.emplace(it->second[0]);
      }
    }

    std::string alphabet;
    std::string inverse;

    for (auto const& letter : self_inverse) {
      alphabet += letter;
      inverse += letter;
    }

    for (auto const& letter : M.alphabet()) {
      if (self_inverse.find(letter) == self_inverse.cend()) {
        alphabet += letter;
        alphabet += std::toupper(letter);
        inverse += std::toupper(letter);
        inverse += letter;
      }
    }

    T G;
    G.set_alphabet(alphabet);
    G.set_identity(id);
    G.set_inverses(inverse);
    for (auto it = M.cbegin_rules(); it < M.cend_rules(); ++it) {
      if ((it->second != id || it->first.size() != 2
           || it->first[0] != it->first[1])
          && (it->first != id || it->second.size() != 2
              || it->second[0] != it->second[1])) {
        G.add_rule(*it);
      }
    }
    return G;
  }

  template <typename T>
  T AlternatingGroupMoore(size_t n) {
    static const std::string alphabet = "abcdfghijklmnopqrstuvwxyz";
    T                        x;
    std::string              id = "e";
    x.set_alphabet(id
                   + std::string(alphabet.cbegin(), alphabet.cbegin() + n - 2));

    std::string const& a = x.alphabet();
    x.add_rule(std::string({a[1]}) + a[1] + a[1], id);

    for (size_t i = 2; i <= n - 2; ++i) {
      x.add_rule(std::string({a[i - 1]}) + a[i] + a[i - 1] + a[i] + a[i - 1]
                     + a[i],
                 id);
    }

    for (size_t j = 1; j <= n - 4; ++j) {
      x.add_rule(std::string({a[j]}) + a[j], id);
    }

    for (size_t k = 3; k <= n - 3; ++k) {
      for (size_t j = 1; j < k - 1; ++j) {
        x.add_rule(std::string({a[j]}) + a[k] + a[j] + a[k], id);
      }
    }
    return make_group(x);
  }

  template <typename T,
            typename
            = std::enable_if_t<std::is_base_of<PresentationBase, T>::value>>
  T make(std::vector<relation_type> const& rels) {
    Presentation<word_type> p;
    for (auto const& rel : rels) {
      p.add_rule(rel.first.cbegin(),
                 rel.first.cend(),
                 rel.second.cbegin(),
                 rel.second.cend());
    }
    p.alphabet_from_rules();
    p.validate();
    return p;
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_FPSEMI_EXAMPLES_HPP_
