//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 Finn Smith
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
// This file contains an implementation of Konieczny's algorithm for computing
// subsemigroups of the boolean matrix monoid.

// TODO: exception safety!

#ifndef LIBSEMIGROUPS_INCLUDE_KONIECZNY_HPP_
#define LIBSEMIGROUPS_INCLUDE_KONIECZNY_HPP_

#include <algorithm>
#include <map>
#include <set>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "bmat8.hpp"
#include "constants.hpp"
#include "digraph.hpp"
#include "orb.hpp"
#include "schreier-sims.hpp"

//
namespace libsemigroups {

  //! Provides a call operator returning a hash value for a pair of size_t.
  //!
  //! This struct provides a call operator for obtaining a hash value for the
  //! pair.
  struct pair_hash {
    //! Hashes a pair of size_t.
    size_t operator()(std::pair<size_t, size_t> x) const {
      return std::get<0>(x) + std::get<1>(x) + 0x9e3779b97f4a7c16;
    }
  };

  //! Provides a call operator returning a hash value for a vector of BMat8s.
  //!
  //! This struct provides a call operator for obtaining a hash value for the
  //! vector.
  struct vec_bmat8_hash {
    //! Hashes a vector of BMat8s.
    size_t operator()(std::vector<BMat8> vec) const {
      size_t hash = 0;
      for (BMat8 x : vec) {
        hash ^= x.to_int() + 0x9e3779b97f4a7c16 + (hash << 6) + (hash >> 2);
      }
      return hash;
    }
  };

  using row_action_type = right_action<BMat8, BMat8>;
  using col_action_type = left_action<BMat8, BMat8>;
  using row_orb_type    = Orb<BMat8, BMat8, row_action_type, Side::RIGHT>;
  using col_orb_type    = Orb<BMat8, BMat8, col_action_type, Side::LEFT>;

  BMat8 group_inverse(BMat8 id, BMat8 bm) {
    BMat8 tmp = bm;
    BMat8 y;
    do {
      y   = tmp;
      tmp = bm * y;
    } while (tmp != id);
    return y;
  }

  class Konieczny {
   public:
    explicit Konieczny(std::vector<BMat8> const& gens)
        : _col_orb(),
          _D_classes(),
          _D_rels(),
          _dim(1),
          _gens(gens),
          _group_indices(),
          _group_indices_alt(),
          _perm_in_gens(false),
          _regular_D_classes(),
          _row_orb() {
      compute_D_classes();
    }

    ~Konieczny();

    //! Finds a group index of a H class in the R class of \p bm
    size_t find_group_index(BMat8 bm) {
      BMat8  col_space_basis        = bm.col_space_basis();
      size_t pos                    = _row_orb.position(bm.row_space_basis());
      size_t row_scc_id             = _row_orb.action_digraph().scc_id(pos);
      std::pair<size_t, size_t> key = std::make_pair(
          col_space_basis.to_int(), _row_orb.action_digraph().scc_id(pos));

      if (_group_indices.find(key) == _group_indices.end()) {
        for (auto it = _row_orb.cbegin_scc(row_scc_id);
             it < _row_orb.cend_scc(row_scc_id);
             it++) {
          if (BMat8::is_group_index(col_space_basis, _row_orb.at(*it))) {
            _group_indices.emplace(key, *it);
            return *it;
          }
        }
      } else {
        return _group_indices.at(key);
      }
      _group_indices.emplace(key, UNDEFINED);
      return UNDEFINED;
    }

    bool is_regular_element(BMat8 bm) {
      if (find_group_index(bm) != UNDEFINED) {
        return true;
      }
      return false;
    }

    // TODO: it must be possible to do better than this
    BMat8 idem_in_H_class(BMat8 bm) {
      BMat8 tmp = bm;
      while (tmp * tmp != tmp) {
        tmp = tmp * bm;
      }
      return tmp;
    }

    //! Finds an idempotent in the D class of \c bm, if \c bm is regular
    BMat8 find_idem(BMat8 bm) {
      if (bm * bm == bm) {
        return bm;
      }
      if (!is_regular_element(bm)) {
        return BMat8(static_cast<size_t>(UNDEFINED));
      }
      size_t i   = find_group_index(bm);
      size_t pos = _row_orb.position(bm.row_space_basis());
      BMat8  x   = bm * _row_orb.multiplier_to_scc_root(pos)
                * _row_orb.multiplier_from_scc_root(i);
      // BMat8(UNDEFINED) happens to be idempotent...
      return idem_in_H_class(x);
    }

    class BaseDClass;
    class RegularDClass;
    class NonRegularDClass;

    std::vector<RegularDClass*> regular_D_classes() {
      return _regular_D_classes;
    }

    std::vector<BaseDClass*> D_classes() {
      return _D_classes;
    }

    size_t size() const;

   private:
    void add_D_class(Konieczny::RegularDClass* D);
    void add_D_class(Konieczny::NonRegularDClass* D);

    //! Finds the minimum dimension \c dim such that all generators have
    //! dimension less than or equal to \c dim, and sets \c _dim.
    void compute_min_possible_dim() {
      _dim = 1;
      for (BMat8 x : _gens) {
        size_t d = x.min_possible_dim();
        if (d > _dim) {
          _dim = d;
        }
      }
    }

    //! Adds the identity BMat8 (of degree max of the degrees of the generators)
    //! if there is no permutation already in the generators, and sets the value
    //! of \c _perm_in_gens.
    void conditional_add_identity() {
      // TODO: this isn't quite right - could be 0 generators etc.
      compute_min_possible_dim();
      for (BMat8 x : _gens) {
        if (x * x.transpose() == BMat8::one(_dim)) {
          _perm_in_gens = true;
        }
      }
      if (!_perm_in_gens) {
        _gens.push_back(BMat8::one(_dim));
      }
    }

    void compute_orbs() {
      _row_orb.add_seed(BMat8::one(_dim));
      _col_orb.add_seed(BMat8::one(_dim));
      for (BMat8 g : _gens) {
        _row_orb.add_generator(g);
        _col_orb.add_generator(g);
      }
      _row_orb.enumerate();
      _col_orb.enumerate();
      // std::cout << _row_orb.size() << std::endl;
      // std::cout << _col_orb.size() << std::endl;
    }

    void compute_D_classes();

    col_orb_type             _col_orb;
    std::vector<BaseDClass*> _D_classes;
    // contains in _D_rels[i] the indices of the D classes which lie above
    // _D_classes[i]
    std::vector<std::vector<size_t>> _D_rels;
    size_t                           _dim;
    std::vector<BMat8>               _gens;
    std::unordered_map<std::pair<size_t, size_t>, size_t, pair_hash>
        _group_indices;
    std::unordered_map<std::pair<size_t, size_t>, size_t, pair_hash>
                                _group_indices_alt;
    bool                        _perm_in_gens;
    std::vector<RegularDClass*> _regular_D_classes;
    row_orb_type                _row_orb;
  };

  class Konieczny::BaseDClass {
    friend class Konieczny;

   public:
    BaseDClass(Konieczny* parent, BMat8 rep)
        : _card(rep.row_space_size()),
          _computed(false),
          _H_class(),
          _left_mults(),
          _left_mults_inv(),
          _left_reps(),
          _parent(parent),
          _rep(rep),
          _right_mults(),
          _right_mults_inv(),
          _right_reps() {}

    virtual ~BaseDClass() {}

    BMat8 rep() const {
      return _rep;
    }

    std::vector<BMat8>::const_iterator cbegin_left_reps() {
      init();
      return _left_reps.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_left_reps() {
      init();
      return _left_reps.cend();
    }

    std::vector<BMat8>::const_iterator cbegin_right_reps() {
      init();
      return _right_reps.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_right_reps() {
      init();
      return _right_reps.cend();
    }

    std::vector<BMat8>::const_iterator cbegin_left_mults() {
      init();
      return _left_mults.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_left_mults() {
      init();
      return _left_mults.cend();
    }

    std::vector<BMat8>::const_iterator cbegin_left_mults_inv() {
      init();
      return _left_mults_inv.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_left_mults_inv() {
      init();
      return _left_mults_inv.cend();
    }

    std::vector<BMat8>::const_iterator cbegin_right_mults() {
      init();
      return _right_mults.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_right_mults() {
      init();
      return _right_mults.cend();
    }

    std::vector<BMat8>::const_iterator cbegin_right_mults_inv() {
      init();
      return _right_mults_inv.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_right_mults_inv() {
      init();
      return _right_mults_inv.cend();
    }

    std::vector<BMat8>::const_iterator cbegin_H_class() {
      init();
      return _H_class.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_H_class() {
      init();
      return _H_class.cend();
    }

    virtual bool contains(BMat8 bm) = 0;

    bool contains(BMat8 bm, size_t card) {
      return (card == _card && contains(bm));
    }

    virtual size_t size() {
      init();
      return _H_class.size() * _left_reps.size() * _right_reps.size();
    }

    // TODO: is NRVO getting rid of the copies on calling this function? I
    // think so...
    std::vector<BMat8> covering_reps() {
      init();
      std::vector<BMat8> out;
      // TODO: how to decide which side to calculate? One is often faster
      if (_parent->_row_orb.size() < _parent->_col_orb.size()) {
        for (BMat8 w : _left_reps) {
          for (BMat8 g : _parent->_gens) {
            BMat8 x = w * g;
            // std::cout << "found " << std::endl << x << std::endl;
            if (!contains(x)) {
              // std::cout << "not contained in this D class!" << std::endl;
              out.push_back(x);
            } else {
              // std::cout << "contained in this D class!" << std::endl;
            }
          }
        }
      } else {
        for (BMat8 z : _right_reps) {
          for (BMat8 g : _parent->_gens) {
            BMat8 x = g * z;
            if (!contains(x)) {
              out.push_back(x);
            }
          }
        }
      }
      std::sort(out.begin(), out.end());
      auto it = std::unique(out.begin(), out.end());
      out.erase(it, out.end());
      // std::cout << "found " << internal::to_string(out.size())
      //          << " covering reps this time" << std::endl;
      return out;
    }

   protected:
    virtual void init() = 0;

    size_t             _card;
    bool               _computed;
    std::vector<BMat8> _H_class;
    std::vector<BMat8> _left_mults;
    std::vector<BMat8> _left_mults_inv;
    std::vector<BMat8> _left_reps;
    Konieczny*         _parent;
    BMat8              _rep;
    std::vector<BMat8> _right_mults;
    std::vector<BMat8> _right_mults_inv;
    std::vector<BMat8> _right_reps;
  };

  class Konieczny::RegularDClass : public Konieczny::BaseDClass {
   public:
    RegularDClass(Konieczny* parent, BMat8 idem_rep)
        : Konieczny::BaseDClass(parent, idem_rep),
          _col_basis_positions(),
          _H_gens(),
          _left_idem_reps(),
          _left_indices(),
          _right_idem_reps(),
          _right_indices(),
          _row_basis_positions(),
          _stab_chain() {
      if (idem_rep * idem_rep != idem_rep) {
        throw LIBSEMIGROUPS_EXCEPTION(
            "RegularDClass: the representative given should be idempotent");
      }
    }

    std::vector<size_t>::const_iterator cbegin_left_indices() {
      init();
      return _left_indices.cbegin();
    }

    std::vector<size_t>::const_iterator cend_left_indices() {
      init();
      return _left_indices.cend();
    }

    std::vector<size_t>::const_iterator cbegin_right_indices() {
      init();
      return _right_indices.cbegin();
    }

    std::vector<size_t>::const_iterator cend_right_indices() {
      init();
      return _right_indices.cend();
    }

    std::vector<BMat8>::const_iterator cbegin_left_idem_reps() {
      init();
      return _left_idem_reps.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_left_idem_reps() {
      init();
      return _left_idem_reps.cend();
    }

    std::vector<BMat8>::const_iterator cbegin_right_idem_reps() {
      init();
      return _right_idem_reps.cbegin();
    }

    std::vector<BMat8>::const_iterator cend_right_idem_reps() {
      init();
      return _right_idem_reps.cend();
    }

    SchreierSims<8, uint8_t, Permutation<uint8_t>> stab_chain() {
      init();
      return _stab_chain;
    }

    // TODO: this is the wrong function! contains shouldn't assume argument is
    //        in semigroup!
    //! Tests whether an element \p bm of the semigroup is in this D class
    //!
    //! Watch out! The element \bm must be known to be in the semigroup for this
    //! to be valid!
    bool contains(BMat8 bm) override {
      init();
      std::pair<size_t, size_t> x = index_positions(bm);
      return x.first != UNDEFINED;
    }

    // returns the position of [the positions of the row and column bases in
    // the row and column orbits] in the left and right index vector of \c this
    // i.e. the position of the L and R classes that \p bm is in,
    // unless bm is not in \c this, in which case returns the pair
    // (UNDEFINED, UNDEFINED)
    std::pair<size_t, size_t> index_positions(BMat8 bm) {
      init();
      auto row_it = _row_basis_positions.find(bm.row_space_basis().to_int());
      if (row_it != _row_basis_positions.end()) {
        auto col_it = _col_basis_positions.find(bm.col_space_basis().to_int());
        if (col_it != _col_basis_positions.end()) {
          return std::make_pair((*row_it).second, (*col_it).second);
        }
      }
      return std::make_pair(UNDEFINED, UNDEFINED);
    }

   private:
    // this is annoyingly a bit more complicated than the right indices
    // because the find_group_index method fixes the column basis and loops
    // through the scc of the row basis
    void compute_left_indices() {
      if (_left_indices.size() > 0) {
        return;
      }
      size_t row_basis_pos = _parent->_row_orb.position(_rep.row_space_basis());
      size_t col_basis_pos = _parent->_col_orb.position(_rep.col_space_basis());
      size_t row_scc_id
          = _parent->_row_orb.action_digraph().scc_id(row_basis_pos);
      size_t col_scc_id
          = _parent->_col_orb.action_digraph().scc_id(col_basis_pos);

      std::pair<size_t, size_t> key = std::make_pair(col_scc_id, 0);
      for (auto it = _parent->_row_orb.cbegin_scc(row_scc_id);
           it < _parent->_row_orb.cend_scc(row_scc_id);
           it++) {
        std::get<1>(key) = *it;
        if (_parent->_group_indices_alt.find(key)
            == _parent->_group_indices_alt.end()) {
          bool found = false;
          for (auto it2 = _parent->_col_orb.cbegin_scc(col_scc_id);
               !found && it2 < _parent->_col_orb.cend_scc(col_scc_id);
               it2++) {
            if (BMat8::is_group_index(_parent->_col_orb.at(*it2),
                                      _parent->_row_orb.at(*it))) {
              _parent->_group_indices_alt.emplace(key, *it2);
              found = true;
            }
          }
          if (!found) {
            _parent->_group_indices_alt.emplace(key, UNDEFINED);
          }
        }
        if (_parent->_group_indices_alt.at(key) != UNDEFINED) {
          _row_basis_positions.emplace(_parent->_row_orb.at(*it).to_int(),
                                       _left_indices.size());
          _left_indices.push_back(*it);
        }
      }
#ifdef LIBSEMIGROUPS_DEBUG
      for (size_t i : _left_indices) {
        LIBSEMIGROUPS_ASSERT(i < _parent->_row_orb.size());
      }
#endif
    }

    void compute_right_indices() {
      if (_right_indices.size() > 0) {
        return;
      }
      size_t col_basis_pos = _parent->_col_orb.position(_rep.col_space_basis());
      size_t col_scc_id
          = _parent->_col_orb.action_digraph().scc_id(col_basis_pos);
      for (auto it = _parent->_col_orb.cbegin_scc(col_scc_id);
           it < _parent->_col_orb.cend_scc(col_scc_id);
           it++) {
        BMat8 x = _parent->_col_orb.multiplier_from_scc_root(*it)
                  * _parent->_col_orb.multiplier_to_scc_root(col_basis_pos)
                  * _rep;
        if (_parent->find_group_index(x) != UNDEFINED) {
          _col_basis_positions.emplace(_parent->_col_orb.at(*it).to_int(),
                                       _right_indices.size());
          _right_indices.push_back(*it);
        }
      }
#ifdef LIBSEMIGROUPS_DEBUG
      for (size_t i : _right_indices) {
        LIBSEMIGROUPS_ASSERT(i < _parent->_col_orb.size());
      }
#endif
    }

    void compute_mults() {
      if (_left_mults.size() > 0) {
        return;
      }
      BMat8  row_basis     = _rep.row_space_basis();
      size_t row_basis_pos = _parent->_row_orb.position(row_basis);
      BMat8  col_basis     = _rep.col_space_basis();
      size_t col_basis_pos = _parent->_col_orb.position(col_basis);

      for (size_t i = 0; i < _left_indices.size(); ++i) {
        BMat8 b
            = _parent->_row_orb.multiplier_to_scc_root(row_basis_pos)
              * _parent->_row_orb.multiplier_from_scc_root(_left_indices[i]);
        BMat8 c = _parent->_row_orb.multiplier_to_scc_root(_left_indices[i])
                  * _parent->_row_orb.multiplier_from_scc_root(row_basis_pos);

        _left_mults.push_back(b);
        _left_mults_inv.push_back(c);
      }

      for (size_t i = 0; i < _right_indices.size(); ++i) {
        BMat8 c = _parent->_col_orb.multiplier_from_scc_root(_right_indices[i])
                  * _parent->_col_orb.multiplier_to_scc_root(col_basis_pos);
        BMat8 d = _parent->_col_orb.multiplier_from_scc_root(col_basis_pos)
                  * _parent->_col_orb.multiplier_to_scc_root(_right_indices[i]);

        _right_mults.push_back(c);
        _right_mults_inv.push_back(d);
      }
    }

    void compute_reps() {
      compute_mults();

      _left_reps.clear();
      _right_reps.clear();

      for (BMat8 b : _left_mults) {
        _left_reps.push_back(_rep * b);
      }

      for (BMat8 c : _right_mults) {
        _right_reps.push_back(c * _rep);
      }
    }

    void compute_H_gens() {
      _H_gens.clear();
      BMat8  col_basis     = _rep.col_space_basis();
      size_t col_basis_pos = _parent->_col_orb.position(col_basis);
      size_t col_basis_scc_id
          = _parent->_col_orb.action_digraph().scc_id(col_basis_pos);
      std::vector<BMat8> right_invs;

      for (size_t i = 0; i < _left_indices.size(); ++i) {
        BMat8                     p = _left_reps[i];
        std::pair<size_t, size_t> key
            = std::make_pair(col_basis_scc_id, _left_indices[i]);

        size_t k = _parent->_group_indices_alt.at(key);
        size_t j = _col_basis_positions.at(_parent->_col_orb.at(k).to_int());
        BMat8  q = _right_reps[j];
        // find the inverse of pq in H_rep
        BMat8 y = group_inverse(_rep, p * q);
        right_invs.push_back(q * y);
      }

      for (size_t i = 0; i < _left_indices.size(); ++i) {
        BMat8 p = _left_reps[i];
        for (BMat8 g : _parent->_gens) {
          BMat8 x = p * g;
          BMat8 s = x.row_space_basis();
          for (size_t j = 0; j < _left_indices.size(); ++j) {
            if (_parent->_row_orb.at(_left_indices[j]) == s) {
              _H_gens.push_back(x * right_invs[j]);
              break;
            }
          }
        }
      }
      std::unordered_set<BMat8> set(_H_gens.begin(), _H_gens.end());
      _H_gens.assign(set.begin(), set.end());
    }

    /*
    void compute_stab_chain() {
      BMat8 row_basis = _rep.row_space_basis();
      for (BMat8 x : _H_gens) {
        _stab_chain.add_generator(BMat8::perm_action_on_basis(row_basis, x));
      }
    }
    */

    void compute_idem_reps() {
      BMat8  row_basis     = _rep.row_space_basis();
      BMat8  col_basis     = _rep.col_space_basis();
      size_t row_basis_pos = _parent->_row_orb.position(row_basis);
      size_t col_basis_pos = _parent->_col_orb.position(col_basis);
      size_t row_scc_id
          = _parent->_row_orb.action_digraph().scc_id(row_basis_pos);
      size_t col_scc_id
          = _parent->_col_orb.action_digraph().scc_id(col_basis_pos);

      // this all relies on the indices having been computed already

      // TODO: use information from the looping through the left indices in
      // the loop through the right indices
      for (size_t i = 0; i < _left_indices.size(); ++i) {
        std::pair<size_t, size_t> key
            = std::make_pair(col_scc_id, _left_indices[i]);
        size_t k = _parent->_group_indices_alt.at(key);
        size_t j = 0;
        while (_right_indices[j] != k) {
          ++j;
        }
        BMat8 x = _right_mults[j] * _rep * _left_mults[i];
        BMat8 y = x;
        // BMat8(UNDEFINED) happens to be idempotent...
        while (x * x != x) {
          x = x * y;
        }
        _left_idem_reps.push_back(x);
      }

      for (size_t j = 0; j < _right_indices.size(); ++j) {
        // TODO: make comprehensible
        std::pair<size_t, size_t> key = std::make_pair(
            _parent->_col_orb.at(_right_indices[j]).to_int(), row_scc_id);
        size_t k = _parent->_group_indices.at(key);
        size_t i = 0;
        while (_left_indices[i] != k) {
          ++i;
        }
        BMat8 x = _right_mults[j] * _rep * _left_mults[i];
        BMat8 y = x;
        // BMat8(UNDEFINED) happens to be idempotent...
        while (x * x != x) {
          x = x * y;
        }
        _right_idem_reps.push_back(x);
      }
    }

    // there should be some way of getting rid of this
    void compute_H_class() {
      _H_class = std::vector<BMat8>(_H_gens.begin(), _H_gens.end());
      std::unordered_set<BMat8> set(_H_class.begin(), _H_class.end());
      for (size_t i = 0; i < _H_class.size(); ++i) {
        for (BMat8 g : _H_gens) {
          BMat8 y = _H_class[i] * g;
          if (set.find(y) == set.end()) {
            set.insert(y);
            _H_class.push_back(y);
          }
        }
      }
    }

    void init() override {
      if (_computed) {
        return;
      }
      compute_left_indices();
      compute_right_indices();
      compute_mults();
      compute_reps();
      compute_idem_reps();
      compute_H_gens();
      compute_H_class();
      // compute_stab_chain();
      _computed = true;
    }

    std::unordered_map<size_t, size_t> _col_basis_positions;
    std::vector<BMat8>                 _H_gens;
    // TODO: why have these and the other reps?
    std::vector<BMat8>                             _left_idem_reps;
    std::vector<size_t>                            _left_indices;
    std::vector<BMat8>                             _right_idem_reps;
    std::vector<size_t>                            _right_indices;
    std::unordered_map<size_t, size_t>             _row_basis_positions;
    SchreierSims<8, uint8_t, Permutation<uint8_t>> _stab_chain;
  };

  class Konieczny::NonRegularDClass : public Konieczny::BaseDClass {
    friend class Konieczny;

   public:
    NonRegularDClass(Konieczny* parent, BMat8 rep)
        : Konieczny::BaseDClass(parent, rep),
          _col_basis_positions(),
          _left_idem_above(),
          _left_idem_class(),
          _H_set(),
          _right_idem_above(),
          _right_idem_class(),
          _row_basis_positions() {
      if (rep * rep == rep) {
        throw LIBSEMIGROUPS_EXCEPTION("NonRegularDClass: the representative "
                                      "given should not be idempotent");
      }
    }

    bool contains(BMat8 bm) override {
      init();
      size_t x = bm.row_space_basis().to_int();
      if (_row_basis_positions[x].size() == 0) {
        return false;
      }
      size_t y = bm.col_space_basis().to_int();
      for (size_t i : _row_basis_positions[x]) {
        for (size_t j : _col_basis_positions[y]) {
          if (_H_set.find(_right_mults_inv[j] * bm * _left_mults_inv[i])
              != _H_set.end()) {
            return true;
          }
        }
      }
      return false;
    }

   private:
    void init() override {
      if (_computed) {
        return;
      }
      find_idems_above();
      compute_H_class();
      _computed = true;
    }

    void find_idems_above() {
      // assumes that all D classes above this have already been calculated!
      bool left_found  = false;
      bool right_found = false;
      for (auto it = _parent->_regular_D_classes.rbegin();
           (!left_found || !right_found)
           && it != _parent->_regular_D_classes.rend();
           it++) {
        RegularDClass* D = *it;
        if (!left_found) {
          for (auto idem_it = D->cbegin_left_idem_reps();
               idem_it < D->cend_left_idem_reps();
               idem_it++) {
            if (_rep * (*idem_it) == _rep) {
              _left_idem_above = *idem_it;
              _left_idem_class = D;
              left_found       = true;
              break;
            }
          }
        }

        if (!right_found) {
          for (auto idem_it = D->cbegin_right_idem_reps();
               idem_it < D->cend_right_idem_reps();
               idem_it++) {
            if ((*idem_it) * _rep == _rep) {
              _right_idem_above = (*idem_it);
              _right_idem_class = D;
              right_found       = true;
              break;
            }
          }
        }
      }

#ifdef LIBSEMIGROUPS_DEBUG
      LIBSEMIGROUPS_ASSERT(_left_idem_class->contains(_left_idem_above));
      LIBSEMIGROUPS_ASSERT(_right_idem_class->contains(_right_idem_above));
      LIBSEMIGROUPS_ASSERT(left_found && right_found);
      LIBSEMIGROUPS_ASSERT(_rep * _left_idem_above == _rep);
      LIBSEMIGROUPS_ASSERT(_right_idem_above * _rep == _rep);
#endif
    }

    // TODO: this computes more than just the H class, and should be split
    void compute_H_class() {
      _H_class = std::vector<BMat8>();
      std::pair<size_t, size_t> left_idem_indices
          = _left_idem_class->index_positions(_left_idem_above);
      BMat8 left_idem_left_mult
          = _left_idem_class
                ->cbegin_left_mults()[std::get<0>(left_idem_indices)];
      BMat8 left_idem_right_mult
          = _left_idem_class
                ->cbegin_right_mults()[std::get<1>(left_idem_indices)];

      std::pair<size_t, size_t> right_idem_indices
          = _right_idem_class->index_positions(_right_idem_above);
      BMat8 right_idem_left_mult
          = _right_idem_class
                ->cbegin_left_mults()[std::get<0>(right_idem_indices)];
      BMat8 right_idem_right_mult
          = _right_idem_class
                ->cbegin_right_mults()[std::get<1>(right_idem_indices)];

      std::vector<BMat8> _left_idem_H_class;
      std::vector<BMat8> _right_idem_H_class;

      for (auto it = _left_idem_class->cbegin_H_class();
           it < _left_idem_class->cend_H_class();
           it++) {
        _left_idem_H_class.push_back(left_idem_right_mult * (*it)
                                     * left_idem_left_mult);
      }

      for (auto it = _right_idem_class->cbegin_H_class();
           it < _right_idem_class->cend_H_class();
           it++) {
        _right_idem_H_class.push_back(right_idem_right_mult * (*it)
                                      * right_idem_left_mult);
      }

      std::vector<BMat8> left_idem_left_reps;
      std::vector<BMat8> right_idem_right_reps;

      for (auto it = _left_idem_class->cbegin_left_mults();
           it < _left_idem_class->cend_left_mults();
           it++) {
        left_idem_left_reps.push_back(left_idem_right_mult
                                      * _left_idem_class->rep() * (*it));
      }

      for (auto it = _right_idem_class->cbegin_right_mults();
           it < _right_idem_class->cend_right_mults();
           it++) {
        right_idem_right_reps.push_back((*it) * _right_idem_class->rep()
                                        * right_idem_left_mult);
      }

      std::vector<BMat8> Hex;
      std::vector<BMat8> xHf;

      for (BMat8 s : _left_idem_H_class) {
        xHf.push_back(_rep * s);
      }

      for (BMat8 t : _right_idem_H_class) {
        Hex.push_back(t * _rep);
      }

      std::unordered_set<BMat8> s(Hex.begin(), Hex.end());
      Hex.assign(s.begin(), s.end());

      s = std::unordered_set<BMat8>(xHf.begin(), xHf.end());
      xHf.assign(s.begin(), s.end());

      std::sort(Hex.begin(), Hex.end());
      std::sort(xHf.begin(), xHf.end());

      std::set_intersection(Hex.begin(),
                            Hex.end(),
                            xHf.begin(),
                            xHf.end(),
                            std::back_inserter(_H_class));
      for (BMat8 x : _H_class) {
        _H_set.insert(x);
      }

      _left_reps.clear();
      _left_mults.clear();
      _right_reps.clear();
      _right_mults.clear();

      std::unordered_set<std::vector<BMat8>, vec_bmat8_hash> Hxhw_set;
      std::unordered_set<std::vector<BMat8>, vec_bmat8_hash> zhHx_set;

      for (BMat8 h : _left_idem_H_class) {
        for (size_t i = 0; i < left_idem_left_reps.size(); ++i) {
          BMat8 w = left_idem_left_reps[i];
          // TODO: enforce uniqueness here?
          std::vector<BMat8> Hxhw;
          for (BMat8 s : _H_class) {
            Hxhw.push_back(s * h * w);
          }
          std::sort(Hxhw.begin(), Hxhw.end());
          if (Hxhw_set.find(Hxhw) == Hxhw_set.end()) {
            Hxhw_set.insert(Hxhw);
            BMat8 A = _rep * h * w;
            BMat8 inv
                = group_inverse(_left_idem_above,
                                w * _left_idem_class->cbegin_left_mults_inv()[i]
                                    * left_idem_left_mult)
                  * group_inverse(_left_idem_above, h);

            size_t x  = A.row_space_basis().to_int();
            auto   it = _row_basis_positions.find(x);
            if (it == _row_basis_positions.end()) {
              _row_basis_positions.emplace(x, std::vector<size_t>());
            }
            _row_basis_positions[x].push_back(_left_reps.size());
            _left_reps.push_back(A);
            _left_mults.push_back(h * w);
            _left_mults_inv.push_back(
                _left_idem_class->cbegin_left_mults_inv()[i]
                * left_idem_left_mult * inv);
          }
        }
      }

      for (BMat8 h : _right_idem_H_class) {
        for (size_t i = 0; i < right_idem_right_reps.size(); ++i) {
          BMat8              z = right_idem_right_reps[i];
          std::vector<BMat8> zhHx;
          for (BMat8 s : _H_class) {
            zhHx.push_back(z * h * s);
          }
          std::sort(zhHx.begin(), zhHx.end());
          if (zhHx_set.find(zhHx) == zhHx_set.end()) {
            zhHx_set.insert(zhHx);
            BMat8 B = z * h * _rep;
            BMat8 inv
                = group_inverse(_right_idem_above, h)
                  * group_inverse(
                        _right_idem_above,
                        right_idem_right_mult
                            * _right_idem_class->cbegin_right_mults_inv()[i]
                            * z);

            size_t x  = B.col_space_basis().to_int();
            auto   it = _col_basis_positions.find(x);
            if (it == _col_basis_positions.end()) {
              _col_basis_positions.emplace(x, std::vector<size_t>());
            }
            _col_basis_positions[x].push_back(_right_reps.size());
            _right_reps.push_back(B);
            _right_mults.push_back(z * h);
            _right_mults_inv.push_back(
                inv * right_idem_right_mult
                * _right_idem_class->cbegin_right_mults_inv()[i]);
          }
        }
      }
    }

    std::unordered_map<size_t, std::vector<size_t>> _col_basis_positions;
    BMat8                                           _left_idem_above;
    RegularDClass*                                  _left_idem_class;
    std::unordered_set<BMat8>                       _H_set;
    BMat8                                           _right_idem_above;
    RegularDClass*                                  _right_idem_class;
    std::unordered_map<size_t, std::vector<size_t>> _row_basis_positions;
  };

  Konieczny::~Konieczny() {
    for (BaseDClass* D : _D_classes) {
      delete D;
    }
  }

  void Konieczny::add_D_class(Konieczny::RegularDClass* D) {
    _regular_D_classes.push_back(D);
    _D_classes.push_back(D);
    _D_rels.push_back(std::vector<size_t>());
  }

  void Konieczny::add_D_class(Konieczny::NonRegularDClass* D) {
    _D_classes.push_back(D);
    _D_rels.push_back(std::vector<size_t>());
  }

  size_t Konieczny::size() const {
    size_t out = 0;
    auto   it  = _D_classes.begin();
    if (!_perm_in_gens) {
      it++;
    }
    for (; it < _D_classes.end(); it++) {
      out += (*it)->size();
    }
    return out;
  }

  void Konieczny::compute_D_classes() {
    conditional_add_identity();
    compute_orbs();

    std::vector<std::vector<std::pair<BMat8, size_t>>> reg_reps(
        257, std::vector<std::pair<BMat8, size_t>>());
    std::vector<std::vector<std::pair<BMat8, size_t>>> non_reg_reps(
        257, std::vector<std::pair<BMat8, size_t>>());
    // TODO: reserve?
    std::set<size_t> cards;
    size_t           max_card = 0;
    cards.insert(0);

    RegularDClass* top = new RegularDClass(this, BMat8::one(_dim));
    add_D_class(top);
    for (BMat8 x : top->covering_reps()) {
      size_t card = x.row_space_size();
      cards.insert(card);
      if (is_regular_element(x)) {
        reg_reps[card].push_back(std::make_pair(x, 0));
      } else {
        non_reg_reps[card].push_back(std::make_pair(x, 0));
      }
    }

    // std::cout << "next card: " << internal::to_string(*cards.rbegin())
    //          << std::endl;

    while (*cards.rbegin() > 0) {
      size_t                                reps_are_reg = false;
      std::vector<std::pair<BMat8, size_t>> next_reps;

      max_card = *cards.rbegin();
      if (!reg_reps[max_card].empty()) {
        reps_are_reg = true;
        next_reps    = std::move(reg_reps[max_card]);
        reg_reps[max_card].clear();
      } else {
        next_reps = std::move(non_reg_reps[max_card]);
        non_reg_reps[max_card].clear();
      }

      std::vector<std::pair<BMat8, size_t>> tmp_next;
      for (auto it = next_reps.begin(); it < next_reps.end(); it++) {
        bool contained = false;
        for (size_t i = 0; i < _D_classes.size(); ++i) {
          if (_D_classes[i]->contains(std::get<0>(*it), max_card)) {
            _D_rels[i].push_back(std::get<1>(*it));
            contained = true;
            break;
          }
        }
        if (!contained) {
          tmp_next.push_back(*it);
        }
      }
      next_reps = std::move(tmp_next);

      while (!next_reps.empty()) {
        BaseDClass*               D;
        std::tuple<BMat8, size_t> tup;

        if (reps_are_reg) {
          tup = next_reps.back();
          D   = new RegularDClass(this, find_idem(std::get<0>(tup)));
          add_D_class(static_cast<RegularDClass*>(D));
          for (BMat8 x : D->covering_reps()) {
            size_t card = x.row_space_size();
            cards.insert(card);
            if (is_regular_element(x)) {
              reg_reps[card].push_back(
                  std::make_pair(x, _D_classes.size() - 1));
            } else {
              non_reg_reps[card].push_back(
                  std::make_pair(x, _D_classes.size() - 1));
            }
          }
          next_reps.pop_back();

        } else {
          tup = next_reps.back();
          D   = new NonRegularDClass(this, std::get<0>(tup));
          add_D_class(static_cast<NonRegularDClass*>(D));
          for (BMat8 x : D->covering_reps()) {
            size_t card = x.row_space_size();
            cards.insert(card);
            if (is_regular_element(x)) {
              reg_reps[card].push_back(
                  std::make_pair(x, _D_classes.size() - 1));
            } else {
              non_reg_reps[card].push_back(
                  std::make_pair(x, _D_classes.size() - 1));
            }
          }
          next_reps.pop_back();
        }

        std::vector<std::pair<BMat8, size_t>> tmp;
        for (std::pair<BMat8, size_t> x : next_reps) {
          if (D->contains(std::get<0>(x))) {
            _D_rels[_D_classes.size() - 1].push_back(std::get<1>(x));
          } else {
            tmp.push_back(x);
          }
        }
        next_reps = std::move(tmp);
      }
#ifdef LIBSEMIGROUPS_DEBUG
      LIBSEMIGROUPS_ASSERT(reg_reps[max_card].empty());
#endif
      if (non_reg_reps[max_card].empty()) {
        cards.erase(max_card);
        max_card = *cards.rbegin();
      }
    }
  }
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_KONIECZNY_HPP_
