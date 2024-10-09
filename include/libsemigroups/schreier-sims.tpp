//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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

// This file contains the implementation of the Schreier-Sims algorithm,
// in the class template SchreierSims.
//
// It also contains the implementation of a backtrack search for computing the
// intersection of two permutations given by Schreier-Sims algorithm by Reinis
// Cirpons.

namespace libsemigroups {

  template <size_t N, typename Point, typename Element, typename Traits>
  SchreierSims<N, Point, Element, Traits>::SchreierSims()
      : _base(),
        _base_size(0),
        _domain(0, N),
        _finished(false),
        _one(this->to_internal(One()(N))),
        _orbits(),
        _orbits_lookup(),
        _strong_gens(),
        _tmp_element1(this->internal_copy(_one)),
        _tmp_element2(this->internal_copy(_one)),
        _transversal(),
        _inversal() {
    init();
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  SchreierSims<N, Point, Element, Traits>&
  SchreierSims<N, Point, Element, Traits>::init() {
    clear();
    _base_size = 0;
    _finished  = false;
    _orbits_lookup.fill(false);
    return *this;
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  SchreierSims<N, Point, Element, Traits>::~SchreierSims() {
    clear();
    this->internal_free(_one);
    this->internal_free(_tmp_element1);
    this->internal_free(_tmp_element2);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  SchreierSims<N, Point, Element, Traits>::SchreierSims(
      SchreierSims const& that)
      : _base(that._base),
        _base_size(that._base_size),
        _domain(that._domain),
        _finished(that._finished),
        _one(this->internal_copy(that._one)),
        _orbits(that._orbits),
        _orbits_lookup(that._orbits_lookup),
        _strong_gens(),
        _tmp_element1(this->internal_copy(_one)),
        _tmp_element2(this->internal_copy(_one)),
        _transversal(),
        _inversal() {
    init_strong_gens_traversal_inversal(that);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  SchreierSims<N, Point, Element, Traits>&
  SchreierSims<N, Point, Element, Traits>::operator=(SchreierSims const& that) {
    _base          = that._base;
    _base_size     = that._base_size;
    _domain        = that._domain;
    _finished      = that._finished;
    _one           = this->internal_copy(that._one);
    _orbits        = that._orbits;
    _orbits_lookup = that._orbits_lookup;
    _tmp_element1  = this->internal_copy(_one);
    _tmp_element2  = this->internal_copy(_one);

    _strong_gens.clear();
    _transversal.clear();
    _inversal.clear();

    init_strong_gens_traversal_inversal(that);

    return *this;
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  bool SchreierSims<N, Point, Element, Traits>::add_generator_no_checks(
      const_element_reference x) {
    if (contains(x)) {
      return false;
    }
    _finished = false;
    _strong_gens.push_back(0, this->internal_copy(this->to_internal_const(x)));
    return true;
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  bool SchreierSims<N, Point, Element, Traits>::add_generator(
      const_element_reference x) {
    throw_if_bad_degree(x);
    if (_strong_gens.size(0) >= _strong_gens.max_size(0)) {
      LIBSEMIGROUPS_EXCEPTION("there are already the maximum number {} of "
                              "generators, cannot add any more!",
                              _strong_gens.size(0));
    } else if (contains(x)) {
      return false;
    }
    _finished = false;
    _strong_gens.push_back(0, this->internal_copy(this->to_internal_const(x)));
    return true;
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  size_t
  SchreierSims<N, Point, Element, Traits>::number_of_generators_no_checks()
      const noexcept {
    if (_base_size == 0) {
      return 0;
    }
    return number_of_strong_generators_no_checks(0);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  size_t SchreierSims<N, Point, Element, Traits>::number_of_generators()
      const noexcept {
    throw_if_bad_depth(0);
    return number_of_generators_no_checks();
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  typename SchreierSims<N, Point, Element, Traits>::const_element_reference
  SchreierSims<N, Point, Element, Traits>::strong_generator(
      index_type depth,
      index_type index) const {
    throw_if_bad_depth(depth);
    if (index >= _strong_gens.size(depth)) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument is out of bounds, expected "
                              "value in range (0, {}], got {}",
                              _strong_gens.size(depth),
                              index);
    }
    return strong_generator_no_checks(depth, index);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  typename SchreierSims<N, Point, Element, Traits>::const_element_reference
  SchreierSims<N, Point, Element, Traits>::transversal_element(
      index_type depth,
      point_type pt) const {
    throw_if_bad_depth(depth);
    throw_if_point_gt_degree(pt);
    throw_if_point_not_in_orbit(depth, pt);
    return transversal_element_no_checks(depth, pt);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  typename SchreierSims<N, Point, Element, Traits>::const_element_reference
  SchreierSims<N, Point, Element, Traits>::inverse_transversal_element(
      index_type depth,
      point_type pt) const {
    throw_if_bad_depth(depth);
    throw_if_point_gt_degree(pt);
    throw_if_point_not_in_orbit(depth, pt);
    return inverse_transversal_element_no_checks(depth, pt);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  uint64_t SchreierSims<N, Point, Element, Traits>::size() {
    // TODO(later) check if product overflows?
    if (empty()) {
      return 1;
    }
    run();
    uint64_t out = 1;
    for (index_type i = 0; i < _base_size; i++) {
      out *= _orbits.size(i);
    }
    return out;
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  bool SchreierSims<N, Point, Element, Traits>::const_contains(
      const_element_reference x) const {
    if (!is_valid_degree(Degree()(x))) {
      return false;
    }
    auto const& y = sift_no_checks(x);
    return internal_equal_to(this->to_internal_const(y), _one);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  bool
  SchreierSims<N, Point, Element, Traits>::contains(const_element_reference x) {
    if (is_valid_degree(Degree()(x))) {
      run();
      return const_contains(x);
    } else {
      return false;
    }
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::add_base_point(point_type pt) {
    throw_if_point_gt_degree(pt);
    if (finished()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the Schreier-Sims algorithm has been run to completion already, "
          "cannot add further base points");
    }
    size_t m = std::distance(_base.cbegin(),
                             std::find(_base.cbegin(), _base.cend(), pt));
    if (m < _base_size) {
      LIBSEMIGROUPS_EXCEPTION("the argument {} (a point) equals item {} in "
                              "the existing base, cannot add it again",
                              pt,
                              m);
    }
    internal_add_base_point(pt);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::run() {
    if (_finished || _strong_gens.size(0) == 0) {
      return;
    }

    for (index_type j = 0; j < _strong_gens.size(0); j++) {
      internal_const_element_type x = _strong_gens.at(0, j);
      index_type                  k = 0;
      while (k < _base_size
             && Action()(_base[k], this->to_external_const(x)) == _base[k]) {
        ++k;
      }
      if (k == _base_size) {  // all base points fixed
        point_type pt = *first_non_fixed_point(x);
        internal_add_base_point(pt);
      }
    }

    index_type first = _strong_gens.size(0) - 1;
    LIBSEMIGROUPS_ASSERT(first < N);
    for (index_type i = 1; i < _base_size + 1; i++) {
      point_type beta      = _base[i - 1];
      index_type old_first = _strong_gens.size(i);
      // set up the strong generators
      for (index_type j = first; j < _strong_gens.size(i - 1); j++) {
        internal_element_type x = _strong_gens.at(i - 1, j);
        if (beta == Action()(beta, this->to_external_const(x))) {
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
        LIBSEMIGROUPS_ASSERT(_orbits_lookup[i][beta]);
        for (index_type m = 0; m < _strong_gens.size(i); m++) {
          Product()(this->to_external(_tmp_element1),
                    this->to_external_const(_transversal[i][beta]),
                    this->to_external_const(_strong_gens.at(i, m)));
          point_type delta
              = Action()(beta, this->to_external_const(_strong_gens.at(i, m)));
          LIBSEMIGROUPS_ASSERT(_orbits_lookup[i][delta]);
          LIBSEMIGROUPS_ASSERT(
              delta
              == Action()(_base[i], this->to_external_const(_tmp_element1)));
          if (!internal_equal_to(_tmp_element1, _transversal[i][delta])) {
            Product()(this->to_external(_tmp_element2),
                      this->to_external_const(_tmp_element1),
                      this->to_external_const(_inversal[i][delta]));
            LIBSEMIGROUPS_ASSERT(
                _base[i]
                == Action()(_base[i], this->to_external_const(_tmp_element2)));
            // internal_sift changes _tmp_element2 in-place
            index_type depth     = internal_sift(_tmp_element2);
            bool       propagate = false;
            if (depth < _base_size) {
              propagate = true;
            } else if (!internal_equal_to(_tmp_element2, _one)) {
              propagate = true;
              internal_add_base_point(*first_non_fixed_point(_tmp_element2));
            }
            if (propagate) {
              LIBSEMIGROUPS_ASSERT(i + 1 <= static_cast<int>(depth));
              _strong_gens.push_back(i + 1, this->internal_copy(_tmp_element2));
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

  ////////////////////////////////////////////////////////////////////////
  // SchreierSims - validation - private
  ////////////////////////////////////////////////////////////////////////

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::throw_if_bad_degree(
      const_element_reference x,
      std::string_view        arg_pos) const {
    auto M = Degree()(x);
    if (!is_valid_degree(M)) {
      LIBSEMIGROUPS_EXCEPTION("the degree of the {} argument (an element) is "
                              "incorrect, expected {} got {}",
                              arg_pos,
                              N,
                              M);
    }
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::throw_if_bad_depth(
      size_t           depth,
      std::string_view arg_pos) const {
    if (depth >= _base_size) {
      LIBSEMIGROUPS_EXCEPTION("the {} argument (depth) is out of bounds, "
                              "expected a value in range [0, {}) got {}",
                              arg_pos,
                              _base_size,
                              depth);
    }
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::throw_if_point_gt_degree(
      point_type       pt,
      std::string_view arg_pos) const {
    if (pt >= N) {
      LIBSEMIGROUPS_EXCEPTION("the {} argument (a point) is out of bounds, "
                              "expected a value in range [0, {}) got {}",
                              arg_pos,
                              N,
                              pt);
    }
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::throw_if_point_not_in_orbit(
      index_type       depth,
      point_type       pt,
      std::string_view depth_arg_pos,
      std::string_view pt_arg_pos) const {
    LIBSEMIGROUPS_ASSERT(depth < N);
    LIBSEMIGROUPS_ASSERT(pt < N);
    if (!_orbits_lookup[depth][pt]) {
      LIBSEMIGROUPS_EXCEPTION(
          "the {} argument {} (a point) does not belong "
          "to the orbit specified by the {} argument {} (depth)",
          pt_arg_pos,
          pt,
          depth_arg_pos,
          depth);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // SchreierSims - member functions - private
  ////////////////////////////////////////////////////////////////////////

  template <size_t N, typename Point, typename Element, typename Traits>
  void
  SchreierSims<N, Point, Element, Traits>::init_strong_gens_traversal_inversal(
      SchreierSims const& that) {
    for (size_t depth = 0; depth < N; ++depth) {
      for (size_t index = 0; index < N; ++index) {
        if (that._orbits_lookup[depth][index]) {
          _transversal[depth][index]
              = this->internal_copy(that._transversal[depth][index]);
          _inversal[depth][index]
              = this->internal_copy(that._inversal[depth][index]);
        }
      }
    }
    for (size_t depth = 0; depth < N; ++depth) {
      for (size_t index = 0; index < that._strong_gens.size(depth); ++index) {
        _strong_gens.push_back(
            depth, this->internal_copy(that._strong_gens.at(depth, index)));
      }
    }
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  // TODO(later): this could be better, especially when use in init() above,
  // we could recycle the memory allocated, instead of freeing everything as
  // below.
  void SchreierSims<N, Point, Element, Traits>::clear() {
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
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::internal_add_base_point(
      point_type pt) {
    LIBSEMIGROUPS_ASSERT(_base_size < N);
    LIBSEMIGROUPS_ASSERT(
        std::find(_base.begin(), _base.begin() + _base_size, pt)
        >= _base.begin() + _base_size);
    _base[_base_size] = pt;
    _orbits.push_back(_base_size, pt);
    _orbits_lookup[_base_size][pt] = true;
    _transversal[_base_size][pt]   = this->internal_copy(_one);
    _inversal[_base_size][pt]      = this->internal_copy(_one);
    _base_size++;
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void
  SchreierSims<N, Point, Element, Traits>::orbit_enumerate(index_type depth,
                                                           index_type first) {
    LIBSEMIGROUPS_ASSERT(depth < _base_size);
    for (index_type i = first; i < _orbits.size(depth); i++) {
      for (auto it = _strong_gens.cbegin(depth); it < _strong_gens.cend(depth);
           ++it) {
        orbit_add_point(depth, *it, _orbits.at(depth, i));
      }
    }
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::orbit_add_gen(
      index_type            depth,
      internal_element_type gen) {
    LIBSEMIGROUPS_ASSERT(depth < _base_size);
    // Apply the new generator to existing points in orbits[depth].
    index_type old_size_orbit = _orbits.size(depth);
    for (index_type i = 0; i < old_size_orbit; i++) {
      orbit_add_point(depth, gen, _orbits.at(depth, i));
    }
    orbit_enumerate(depth, old_size_orbit);
  }

  template <size_t N, typename Point, typename Element, typename Traits>
  void SchreierSims<N, Point, Element, Traits>::orbit_add_point(
      index_type            depth,
      internal_element_type x,
      point_type            pt) {
    point_type img = Action()(pt, this->to_external_const(x));
    if (!_orbits_lookup[depth][img]) {
      _orbits.push_back(depth, img);
      _orbits_lookup[depth][img] = true;
      _transversal[depth][img]   = this->internal_copy(_one);
      Product()(this->to_external(_transversal[depth][img]),
                this->to_external_const(_transversal[depth][pt]),
                this->to_external_const(x));
      _inversal[depth][img] = this->to_internal(
          Inverse()(this->to_external_const(_transversal[depth][img])));
    }
  }

  // Changes _tmp_element2 in-place, and returns the depth reached in the
  // sifting.
  template <size_t N, typename Point, typename Element, typename Traits>
  typename SchreierSims<N, Point, Element, Traits>::index_type
  SchreierSims<N, Point, Element, Traits>::internal_sift(
      internal_reference x) const {
    LIBSEMIGROUPS_ASSERT(&x != &_tmp_element1);
    for (index_type depth = 0; depth < _base_size; ++depth) {
      point_type beta = Action()(_base[depth], this->to_external_const(x));
      if (!_orbits_lookup[depth][beta]) {
        return depth;
      }
      Product()(this->to_external(_tmp_element1),
                this->to_external_const(x),
                this->to_external_const(_inversal[depth][beta]));
      Swap()(this->to_external(x), this->to_external(_tmp_element1));
    }
    return _base_size;
  }

  namespace schreier_sims {
    template <size_t N>
    void intersection(SchreierSims<N>& T,
                      SchreierSims<N>& S1,
                      SchreierSims<N>& S2) {
      // This might not be correct for general traits, i.e. only works for
      // permutations for now.
      using point_type   = typename SchreierSims<N>::point_type;
      using element_type = typename SchreierSims<N>::element_type;
      using One          = typename SchreierSims<N>::One;
      using Product      = typename SchreierSims<N>::Product;

      if (!T.empty()) {
        LIBSEMIGROUPS_EXCEPTION("the parameter T must be empty");
      }

      S1.run();
      S2.run();
      if (S2.base_size() < S1.base_size()) {
        intersection(T, S2, S1);
        return;
      }

      // If N <= 1 then both S1, S2 are trivial.
      if (N <= 1) {
        T.run();
        return;
      }

      // Note that if N-1 points are fixed then the N-th point is also fixed.
      // So if base contains all N points, then we lose nothing by discarding
      // the last point in the base.
      size_t base_size = S1.base_size();
      if (base_size == N) {
        base_size = N - 1;
      }

      auto S2B = std::make_unique<SchreierSims<N>>();
      for (size_t depth = 0; depth < base_size; ++depth) {
        S2B->add_base_point(S1.base(depth));
      }
      for (size_t i = 0; i < S2.number_of_generators(); ++i) {
        S2B->add_generator(S2.generator(i));
      }
      S2B->run();
#ifdef LIBSEMIGROUPS_DEBUG
      for (size_t depth = 0; depth < base_size; ++depth) {
        LIBSEMIGROUPS_ASSERT(S1.base(depth) == S2B->base(depth));
      }
#endif
      // Only need to consider points reachable by both groups.
      // Note that as we traverse the tree these points change!
      // In general, if we are at a node corresponding to elements g and h
      // in the tree and orbits O and P respectively, then the only points we
      // need to consider are O^g intersect P^h.
      // This is not currently implemented! We just use all of the points
      // in the orbits of S1. Implementing it probably requires refactoring
      // the code.
      detail::StaticTriVector2<point_type, N> refined_orbit;
      for (size_t depth = 0; depth < base_size; ++depth) {
        // First point is always base point to make algorithm simpler
        LIBSEMIGROUPS_ASSERT(S1.base(depth) == S2B->base(depth));
        refined_orbit.push_back(depth, S1.base(depth));
        for (point_type pt = 0; pt < N; ++pt) {
          if ((pt != S1.base(depth)) && S1.orbit_lookup(depth, pt)) {
            refined_orbit.push_back(depth, pt);
          }
        }
      }

      // Initially assume that we have traversed the tree to the leaf
      // corresponding to the base and identity element.
      // stab_depth tracks the largest stabiliser we have found thus far.
      size_t                      stab_depth = base_size;
      size_t                      depth      = 0;
      std::array<size_t, N>       state_index;
      std::array<element_type, N> state_elem;
      state_index.fill(0);
      state_elem.fill(One()(N));

      while (stab_depth > 0) {
        for (; depth < base_size; ++depth) {
          // This is a safe memory access as base_size <= N-1, so depth < N-1
          // during the loop and so depth + 1 <= N-1
          LIBSEMIGROUPS_ASSERT(depth + 1 < N);
          Product()(state_elem[depth + 1],
                    S1.transversal_element(
                        depth, refined_orbit.at(depth, state_index[depth])),
                    state_elem[depth]);
        }
        if (S2B->contains(state_elem[depth])) {
          LIBSEMIGROUPS_ASSERT(S1.contains(state_elem[depth]));
          LIBSEMIGROUPS_ASSERT(S2.contains(state_elem[depth]));
          T.add_generator(state_elem[depth]);
          // As soon as we find one, the rest are in a coset of stabiliser, so
          // dont need to look at any more nodes.
          depth = stab_depth;
        }
        // If previous if statement passes then depth = stab_depth > 0 by the
        // while loop invariant. If not, then depth = base_size > 0 due to the
        // for loop before the if statement.
        LIBSEMIGROUPS_ASSERT(depth != 0);
        depth--;

        // Find largest depth that has an unvisited node and increment its
        // index. Adjust stabilizer depth as depths are exhausted.
        for (;; --depth) {
          LIBSEMIGROUPS_ASSERT(depth < base_size);
          state_index[depth]++;
          if (state_index[depth] < refined_orbit.size(depth)) {
            break;
          }
          if (depth < stab_depth) {
            stab_depth = depth;
          }
          state_index[depth] = 0;
          state_elem[depth]  = One()(N);
          if (depth == 0) {
            break;
          }
        }
      }

      T.run();
    }
  }  // namespace schreier_sims
}  // namespace libsemigroups
