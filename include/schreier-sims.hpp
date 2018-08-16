//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains an implementation of the Schreier-Sims algorithm, as
// described in Section 4.4.2 of:
//
// D. Holt (with B. Eick and E. O'Brien), Handbook of computational group
// theory, CRC Press, 2004.
//
// and as implemented in the kernel module of the Digraphs package for GAP in
// C:
//
// https://github.com/gap-packages/Digraphs/blob/master/src/schreier-sims.c
//
// by Wilf A. Wilson.

// TODO
//
// 0. check use of:
//    * element_type []
//    * const_element_type []
//    * reference        []
//    * const_reference  []
//    * internal_element_type []
//    * internal_const_element_type []
//    * internal_reference []
//    * internal_const_reference []
// 1. iterator to the elements (maybe)
// 2. stabilizer method
// 3. change base
// 4. random version
// 5. try it with Digraphs

#ifndef LIBSEMIGROUPS_INCLUDE_SCHREIER_SIMS_H_
#define LIBSEMIGROUPS_INCLUDE_SCHREIER_SIMS_H_

#include <array>

#include "internal/libsemigroups-config.hpp"
#include "internal/range.hpp"
#include "internal/square.hpp"

#include "adapters.hpp"
#include "elements.hpp"
#include "semigroup-traits.hpp"

namespace libsemigroups {
  template <size_t N,
            typename TPointType    = typename SmallestInteger<N>::type,
            typename TElementType  = typename Perm<N>::type,
            typename TDomainType   = IntegralRange<TPointType, 0, N>,
            typename TElementEqual = equal_to<TElementType>,
            class TTraits = SemigroupTraitsEqual<TElementType, TElementEqual>>
  class SchreierSims : private TTraits {
    using const_element_type = typename TTraits::const_element_type;
    using const_reference    = typename TTraits::const_reference;

    using internal_element_type = typename TTraits::internal_element_type;
    using internal_const_element_type =
        typename TTraits::internal_const_element_type;

    using internal_equal_to = typename TTraits::internal_equal_to;

    // FIXME make index_type a template parameter again
    using index_type = size_t;

   public:
    using element_type = typename TTraits::element_type;
    using point_type   = TPointType;

   private:
    // gcc apparently requires the extra qualification on the aliases below
    using action  = ::libsemigroups::action<internal_element_type, point_type>;
    using degree  = ::libsemigroups::degree<internal_element_type>;
    using inverse = ::libsemigroups::inverse<internal_element_type>;
    using one     = ::libsemigroups::one<internal_element_type>;
    using product = ::libsemigroups::product<internal_element_type>;
    using swap    = ::libsemigroups::swap<internal_element_type>;

   public:
    SchreierSims()
        : _base(),
          _base_size(0),
          _domain(),
          _finished(false),
          _one(one()(N)),
          _orbits(),
          _orbits_lookup(),
          _strong_gens(),
          _tmp_element1(this->internal_copy(_one)),
          _tmp_element2(this->internal_copy(_one)),
          _transversal(),
          _inversal() {
      init();
    }

    ~SchreierSims() {
      clear();
      this->internal_free(_one);
      this->internal_free(_tmp_element1);
      this->internal_free(_tmp_element2);
    }

    void add_generator(const_reference x) {
      if (!has_valid_degree(x)) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "the degree of the generator must be " + to_string(N) + ", not "
            + to_string(degree()(this->to_internal_const(x))));
      } else if (!contains(x)) {
        _finished = false;
        _strong_gens.push_back(0, this->internal_copy(_tmp_element2));
      }
    }

    const_reference generator(size_t index) {
      if (index >= _strong_gens.size(0)) {
        throw LIBSEMIGROUPS_EXCEPTION("the argument must be at most "
                                      + to_string(_strong_gens.size(0))
                                      + ", not " + to_string(index));
      }
      return this->to_external_const(_strong_gens.at(0, index));
    }

    bool empty() {
      return _strong_gens.size(0) == 0;
    }

    size_t size() {
      if (empty()) {
        return 1;
      }
      schreier_sims();
      size_t out = 1;
      for (index_type i = 0; i < _base_size; i++) {
        out *= _orbits.size(i);
      }
      return out;
    }

    element_type sift(const_reference x) {
      if (!has_valid_degree(x)) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "the degree of the generator must be " + to_string(N) + ", not "
            + to_string(degree()(this->to_internal_const(x))));
      }
      element_type cpy = this->external_copy(this->to_internal_const(x));
      swap()(this->to_internal(cpy), _tmp_element2);
      internal_sift();  // changes _tmp_element2 in place
      swap()(this->to_internal(cpy), _tmp_element2);
      return cpy;
    }

    bool contains(const_reference x) {
      if (!has_valid_degree(x)) {
        return false;
      }
      schreier_sims();
      internal_element_type cpy
          = this->internal_copy(this->to_internal_const(x));
      swap()(cpy, _tmp_element2);
      this->internal_free(cpy);
      internal_sift();  // changes _tmp_element2 in place
      return internal_equal_to()(_tmp_element2, _one);
    }

    const_reference identity() const {
      return this->to_external_const(_one);
    }

    void clear() {
      for (size_t depth = 0; depth < N; ++depth) {
        for (size_t index = 0; index < N; ++index) {
          if (_orbits_lookup[depth][index]) {
            this->internal_free(_transversal[depth][index]);
            this->internal_free(_inversal[depth][index]);
          }
        }
      }
      std::unordered_set<internal_element_type> deleted;
      for (size_t depth = 0; depth < N; ++depth) {
        for (size_t index = 0; index < _strong_gens.size(depth); ++index) {
          if (deleted.find(_strong_gens.at(depth, index)) == deleted.end()) {
            this->internal_free(_strong_gens.at(depth, index));
            deleted.insert(_strong_gens.at(depth, index));
          }
        }
      }
      _strong_gens.clear();
      _orbits.clear();
      init();
    }

    bool finished() const {
      return _finished;
    }

    void add_base_point(point_type const pt) {
      if (pt >= N - 1) {
        throw LIBSEMIGROUPS_EXCEPTION("the new base point must be at most "
                                      + to_string(N) + ", not "
                                      + to_string(pt));
      } else if (_base_size == N) {
        throw LIBSEMIGROUPS_EXCEPTION("the current base has size "
                                      + to_string(N)
                                      + ", cannot add further points");
      } else if (finished()) {
        throw LIBSEMIGROUPS_EXCEPTION("cannot add "
                                      "further base points");
      } else {
        for (size_t i = 0; i < _base_size; ++i) {
          if (_base[i] == pt) {
            throw LIBSEMIGROUPS_EXCEPTION(to_string(pt)
                                          + " is already a base point");
          }
        }
      }
      internal_add_base_point(pt);
    }

    point_type base(index_type const index) const {
      if (index >= _base_size) {
        throw LIBSEMIGROUPS_EXCEPTION("the index must be at most "
                                      + to_string(_base_size) + ", not "
                                      + to_string(index));
      }
      return _base[index];
    }

    size_t base_size() const {
      return _base_size;
    }

   private:
    void init() {
      _base_size = 0;
      _finished  = false;
      _orbits_lookup.fill(false);
    }

    bool has_valid_degree(const_reference x) const {
      return
#ifdef LIBSEMIGROUPS_HPCOMBI
          std::is_same<HPCombi::Perm16, element_type>::value ||
#endif
          degree()(this->to_internal_const(x)) == N;
    }

    void internal_add_base_point(point_type const pt) {
      LIBSEMIGROUPS_ASSERT(_base_size < N);
      _base[_base_size] = pt;
      _orbits.push_back(_base_size, pt);
      _orbits_lookup[_base_size][pt] = true;
      _transversal[_base_size][pt]   = this->internal_copy(_one);
      _inversal[_base_size][pt]      = this->internal_copy(_one);
      _base_size++;
    }

    void orbit_enumerate(index_type const depth, index_type first = 0) {
      LIBSEMIGROUPS_ASSERT(depth < _base_size);
      for (index_type i = first; i < _orbits.size(depth); i++) {
        for (auto it = _strong_gens.cbegin(depth);
             it < _strong_gens.cend(depth);
             ++it) {
          orbit_add_point(depth, *it, _orbits.at(depth, i));
        }
      }
    }

    void orbit_add_gen(index_type const depth, internal_element_type gen) {
      LIBSEMIGROUPS_ASSERT(depth < _base_size);
      // Apply the new generator to existing points in orbits[depth].
      index_type old_size_orbit = _orbits.size(depth);
      for (index_type i = 0; i < old_size_orbit; i++) {
        orbit_add_point(depth, gen, _orbits.at(depth, i));
      }
      orbit_enumerate(depth, old_size_orbit);
    }

    void orbit_add_point(index_type const      depth,
                         internal_element_type x,
                         point_type const      pt) {
      point_type img = action()(x, pt);
      if (!_orbits_lookup[depth][img]) {
        _orbits.push_back(depth, img);
        _orbits_lookup[depth][img] = true;
        _transversal[depth][img]   = this->internal_copy(_one);
        product()(_transversal[depth][img], _transversal[depth][pt], x);
        _inversal[depth][img] = inverse()(_transversal[depth][img]);
      }
    }

    // Changes _tmp_element2 in-place, and returns the depth reached in the
    // sifting.
    index_type internal_sift() {
      for (index_type depth = 0; depth < _base_size; ++depth) {
        point_type beta = action()(_tmp_element2, _base[depth]);
        if (!_orbits_lookup[depth][beta]) {
          return depth;
        }
        product()(_tmp_element1, _tmp_element2, _inversal[depth][beta]);
        swap()(_tmp_element2, _tmp_element1);
      }
      return _base_size;
    }

    void schreier_sims() {
      if (_finished) {
        return;
      }

      for (index_type j = 0; j < _strong_gens.size(0); j++) {
        internal_const_element_type x = _strong_gens.at(0, j);
        index_type                  k = 0;
        while (k < _base_size && action()(x, _base[k]) == _base[k]) {
          ++k;
        }
        if (k == _base_size) {  // all base points fixed
          point_type pt = first_non_fixed_point(x);
          if (pt != N) {
            internal_add_base_point(pt);
          }
        }
      }

      index_type first = 0;
      for (index_type i = 1; i < _base_size + 1; i++) {
        point_type beta      = _base[i - 1];
        index_type old_first = _strong_gens.size(i);
        // set up the strong generators
        for (index_type j = first; j < _strong_gens.size(i - 1); j++) {
          internal_element_type x = _strong_gens.at(i - 1, j);
          if (beta == action()(x, beta)) {
            _strong_gens.push_back(i, x);
          }
        }
        first = old_first;
        // find the orbit of <beta> under strong_gens[i - 1]
        orbit_enumerate(i - 1);
      }
      for (int i = _base_size - 1; i >= 0; --i) {
      start:
        for (auto it = _orbits.cbegin(i); it < _orbits.cend(i); ++it) {
          point_type beta = *it;
          for (index_type m = 0; m < _strong_gens.size(i); m++) {
            product()(
                _tmp_element1, _transversal[i][beta], _strong_gens.at(i, m));
            point_type delta = action()(_strong_gens.at(i, m), beta);
            LIBSEMIGROUPS_ASSERT(delta == action()(_tmp_element1, _base[i]));
            if (!internal_equal_to()(_tmp_element1, _transversal[i][delta])) {
              product()(_tmp_element2, _tmp_element1, _inversal[i][delta]);
              LIBSEMIGROUPS_ASSERT(_base[i]
                                   == action()(_tmp_element2, _base[i]));
              // internal_sift changes _tmp_element2 in-place
              index_type depth     = internal_sift();
              bool       propagate = false;
              if (depth < _base_size) {
                propagate = true;
              } else if (!internal_equal_to()(_tmp_element2, _one)) {
                propagate = true;
                internal_add_base_point(first_non_fixed_point(_tmp_element2));
              }
              if (propagate) {
                LIBSEMIGROUPS_ASSERT(i + 1 <= static_cast<int>(depth));
                _strong_gens.push_back(i + 1,
                                       this->internal_copy(_tmp_element2));
                orbit_add_gen(i + 1, _tmp_element2);
                for (index_type l = i + 2; l <= depth; l++) {
                  _strong_gens.push_back(l, _strong_gens.back(i + 1));
                  orbit_add_gen(l, _tmp_element2);
                  // add generator to orbit of base[l]
                }
                i = depth;
                goto start;
              }
            }
          }
        }
      }
      _finished = true;
    }

    point_type first_non_fixed_point(internal_const_element_type x) {
      for (auto it = _domain.cbegin(); it < _domain.cend(); ++it) {
        if (*it != action()(x, *it)) {
          return *it;
        }
      }
      return N;
    }

    std::array<point_type, N>              _base;
    index_type                             _base_size;
    TDomainType                            _domain;
    bool                                   _finished;
    internal_element_type                  _one;
    SquareVector<point_type, N>            _orbits;
    SquareArray<bool, N>                   _orbits_lookup;
    SquareVector<internal_element_type, N> _strong_gens;
    internal_element_type                  _tmp_element1;
    internal_element_type                  _tmp_element2;
    SquareArray<internal_element_type, N>  _transversal;
    SquareArray<internal_element_type, N>  _inversal;
  };

}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_INCLUDE_SCHREIER_SIMS_H_
