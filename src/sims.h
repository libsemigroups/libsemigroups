//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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
//
// This file contains ...
//
#ifndef LIBSEMIGROUPS_SRC_SIMS_H_
#define LIBSEMIGROUPS_SRC_SIMS_H_
#include "semigroups.h"  // TODO can this be removed?
namespace libsemigroups {
  // Schreier-Sims set up
  template <typename PointType> class StabChain {
   public:
    StabChain()
        : _gens(nullptr),
          _base({}),
          _orbits({}),
          _size_orbits({}),
          _transversal({}),
          _transversal_inv({}),
          _strong_gens(),
          _enumerated(false){};

    explicit StabChain(std::vector<Permutation<PointType>*>* gens)
        : StabChain() {
      LIBSEMIGROUPS_ASSERT(!gens->empty());
      _gens = gens;
      _deg  = ((*gens)[0])->degree();
      _size_orbits.resize(_deg);
      _orbits.resize(_deg * _deg);
      //_strong_gens = {*(really_copy_vec(gens))};
      std::vector<Permutation<PointType>*> init_gens = {};
      for (Permutation<PointType>* perm : *gens) {
        init_gens.push_back(
            static_cast<Permutation<PointType>*>(perm->really_copy()));
      }
      _strong_gens = {init_gens};

#ifdef LIBSEMIGROUPS_DEBUG
      for (auto x : *gens) {
        LIBSEMIGROUPS_ASSERT(x->degree() == gens->at(0)->degree());
      }
#endif
    }
    ~StabChain() {
      for (std::vector<Permutation<PointType>*> perm_list : _strong_gens) {
        really_delete_cont<std::vector<Permutation<PointType>*>>(perm_list);
      }

      for (Permutation<PointType>* perm : _transversal) {
        if (perm != nullptr) {
          perm->really_delete();
          delete perm;
        }
      }

      for (Permutation<PointType>* perm : _transversal_inv) {
        if (perm != nullptr) {
          perm->really_delete();
          delete perm;
        }
      }
    }

    uint64_t group_size() {
      uint64_t out = 1;

      if (!_enumerated) {
        _strong_gens.resize(_deg + 1);
        _transversal.resize(_deg * _deg);
        _transversal_inv.resize(_deg * _deg);

        add_base_point(0);
        schreier_sims_stab_chain(0);
        _enumerated = true;
      }

      for (uint64_t i = 0; i < _base.size(); i++)
        out = out * _size_orbits[i];
      return out;
    }

    uint64_t group_mem(Permutation<PointType>* const test) {
      if (test->degree() != _deg) {
        return false;
      }

      bool out = true;

      if (!_enumerated) {
        _strong_gens.resize(_deg + 1);
        _transversal.resize(_deg * _deg);
        _transversal_inv.resize(_deg * _deg);

        add_base_point(0);
        schreier_sims_stab_chain(0);
        _enumerated = true;
      }

      Permutation<PointType>* currenttest
          = static_cast<Permutation<PointType>*>(test->really_copy());

      Element* currenttest_copy;
      for (uint64_t i = 0; i < _base.size(); i++) {
        if (_transversal_inv[i * _deg + (*currenttest)[_base[i]]] == nullptr) {
          out = false;
          break;
        } else if (is_identity(currenttest)) {
          break;
        }
        currenttest_copy = currenttest->really_copy();
        currenttest->redefine(
            currenttest_copy,
            _transversal_inv[i * _deg + (*currenttest)[_base[i]]]);
        currenttest_copy->really_delete();
        delete currenttest_copy;
      }

      out = out && is_identity(currenttest);
      currenttest->really_delete();
      delete currenttest;
      return out;
    }

   private:
    std::vector<Permutation<PointType>*>*
    really_copy_vec(std::vector<Permutation<PointType>*>* const cont) {
      std::vector<Permutation<PointType>*>* out
          = new std::vector<Permutation<PointType>*>({});
      for (Permutation<PointType>* x : *cont)
        out->push_back(static_cast<Permutation<PointType>*>(x->really_copy()));
      return out;
    }

    bool is_identity(Permutation<PointType>* perm) {
      bool out = true;
      for (PointType pt = 0; pt < perm->degree(); pt++) {
        if ((*perm)[pt] != pt) {
          out = false;
          break;
        }
      }
      return out;
    }

    void add_strong_gens(uint64_t const                pos,
                         Permutation<PointType>* const value) {
      //   if (_strong_gens[pos].size() == 0)
      //     _strong_gens[pos] = {};
      LIBSEMIGROUPS_ASSERT(_strong_gens.size() > pos);
      _strong_gens[pos].push_back(
          static_cast<Permutation<PointType>*>((value->really_copy())));
    }

    void set_transversal(uint64_t const                i,
                         uint64_t const                j,
                         Permutation<PointType>* const value) {
      if (_transversal[i * _deg + j] != nullptr) {
        _transversal[i * _deg + j]->really_delete();
        _transversal_inv[i * _deg + j]->really_delete();
        delete _transversal[i * _deg + j];
        delete _transversal_inv[i * _deg + j];
      }
      _transversal[i * _deg + j]     = value;
      _transversal_inv[i * _deg + j] = value->inverse();
    }

    bool perm_fixes_points(Permutation<PointType>* const perm) {
      for (uint64_t i = 0; i < _base.size(); i++)
        if ((*perm)[_base[i]] != _base[i])
          return false;
      return true;
    }

    void add_base_point(uint64_t const p) {
      LIBSEMIGROUPS_ASSERT(!_enumerated);

      std::vector<PointType> id(_deg);
      for (uint64_t i = 0; i < _deg; i++)
        id[i] = i;
      _size_orbits[_base.size()]            = 1;
      _orbits[_base.size() * _deg]          = p;
      _transversal[_base.size() * _deg + p] = new Permutation<PointType>(id);
      _transversal_inv[_base.size() * _deg + p]
          = new Permutation<PointType>(id);
      _base.push_back(p);
    }

    void orbit_stab_chain(uint64_t const depth) {
      uint64_t                pt;
      Permutation<PointType>* x;
      Permutation<PointType>* prod;

      for (uint64_t i = 0; i < _size_orbits[depth]; i++) {
        pt = _orbits[depth * _deg + i];
        for (uint64_t j = 0; j < _strong_gens[depth].size(); j++) {
          x = _strong_gens[depth][j];
          if (_transversal[depth * _deg + (*x)[pt]] == nullptr) {
            _orbits[depth * _deg + _size_orbits[depth]] = (*x)[pt];
            _size_orbits[depth]++;
            prod = new Permutation<PointType>(new std::vector<PointType>(_deg));
            prod->redefine(_transversal[depth * _deg + pt], x);
            set_transversal(depth, (*x)[pt], prod);
          }
        }
      }
    }

    void add_gen_orbit_stab_chain(uint64_t const                depth,
                                  Permutation<PointType>* const gen) {
      uint64_t                i, j;
      PointType               pt;
      Permutation<PointType>* x;
      Permutation<PointType>* prod;
      uint64_t                inital_size_orbit(_size_orbits[depth]);

      // apply the new generator to existing points in orbits[depth]
      for (i = 0; i < inital_size_orbit; i++) {
        pt = _orbits[depth * _deg + i];
        if (_transversal[depth * _deg + (*gen)[pt]] == nullptr) {
          _orbits[depth * _deg + _size_orbits[depth]] = (*gen)[pt];
          _size_orbits[depth]++;
          prod = new Permutation<PointType>(new std::vector<PointType>(_deg));
          prod->redefine(_transversal[depth * _deg + pt], gen);
          set_transversal(depth, (*gen)[pt], prod);
        }
      }

      for (i = inital_size_orbit; i < _size_orbits[depth]; i++) {
        pt = _orbits[depth * _deg + i];
        for (j = 0; j < _strong_gens[depth].size(); j++) {
          x = _strong_gens[depth][j];
          if (_transversal[depth * _deg + (*x)[pt]] == nullptr) {
            _orbits[depth * _deg + _size_orbits[depth]] = (*x)[pt];
            _size_orbits[depth]++;
            prod = new Permutation<PointType>(new std::vector<PointType>(_deg));
            prod->redefine(_transversal[depth * _deg + pt], x);
            set_transversal(depth, (*x)[pt], prod);
          }
        }
      }
    }

    void sift_stab_chain(Permutation<PointType>* const g,
                         uint64_t* const               depth) {
      PointType beta;
      Element*  g_copy;
      LIBSEMIGROUPS_ASSERT(*depth == 0);

      for (; *depth < _base.size(); (*depth)++) {
        beta = (*g)[_base[*depth]];
        if (_transversal_inv[*depth * _deg + beta] == nullptr)
          return;
        g_copy = g->really_copy();
        g->redefine(g_copy, _transversal_inv[*depth * _deg + beta]);
        g_copy->really_delete();
        delete g_copy;
      }
      LIBSEMIGROUPS_ASSERT(perm_fixes_points(g));
    }

    // Builds a base and strong generating set and stabilisers, for the group
    // generated by strong_gens[0]
    void schreier_sims_stab_chain(uint64_t const depth) {
      Permutation<PointType>* prod;
      bool                    escape, h_fixes_all_base_pts;
      int                     i;
      uint64_t                j, no_base_pts_h_fixes, k, l, m;
      PointType               beta, betax;

      LIBSEMIGROUPS_ASSERT(depth < _strong_gens.size());

      for (i = 0; i <= static_cast<int>(depth); i++) {
        if (_strong_gens[i].size() != 0) {
          for (Permutation<PointType>* x : _strong_gens[i]) {
            if (perm_fixes_points(x)) {
              for (k = 0; k < _deg; k++) {
                if (k != (*x)[k]) {
                  add_base_point(k);
                  break;
                }
              }
            }
          }
        }
      }

      for (i = depth + 1; i <= static_cast<int>(_base.size()); i++) {
        beta = _base[i - 1];
        // set up the strong generators
        for (j = 0; j < _strong_gens[i - 1].size(); j++)
          if (beta == (*(_strong_gens[i - 1][j]))[beta]) {
            add_strong_gens(i, _strong_gens[i - 1][j]);
            // add_gen_orbit_stab_chain(i, _strong_gens[i - 1][j]);
          }

        // find the orbit of base[i -1] under strong_gens[i - 1]
        orbit_stab_chain(i - 1);
      }

      Permutation<PointType>* h;
      Permutation<PointType>* x;
      bool                    h_is_not_identity;

      i = _base.size() - 1;
      while (i >= static_cast<int>(depth)) {
        escape = false;

        for (j = 0; j < _size_orbits[i] && !escape; j++) {
          beta = _orbits[i * _deg + j];
          for (m = 0; m < _strong_gens[i].size() && !escape; m++) {
            x    = _strong_gens[i][m];
            prod = new Permutation<PointType>(std::vector<PointType>(_deg));
            prod->redefine(_transversal[i * _deg + beta], x);
            betax = (*x)[beta];

            if (!(*prod == *(_transversal[i * _deg + betax]))) {
              h = new Permutation<PointType>(std::vector<PointType>(_deg));
              h_fixes_all_base_pts = true;
              h->redefine(prod, _transversal_inv[i * _deg + betax]);
              no_base_pts_h_fixes = 0;
              sift_stab_chain(h, &no_base_pts_h_fixes);

              h_is_not_identity = false;
              for (uint64_t pt = 0; pt < h->degree(); pt++) {
                if ((*h)[pt] != pt) {
                  h_is_not_identity = true;
                  break;
                }
              }
              LIBSEMIGROUPS_ASSERT(no_base_pts_h_fixes <= _base.size());

              if (no_base_pts_h_fixes < _base.size()) {
                h_fixes_all_base_pts = false;
              } else if (h_is_not_identity) {
                h_fixes_all_base_pts = false;
                for (k = 0; k < _deg; k++)
                  if (k != (*h)[k]) {
                    add_base_point(k);
                    break;
                  }
              }

              if (!h_fixes_all_base_pts) {
                for (l = i + 1; l <= no_base_pts_h_fixes; l++) {
                  add_strong_gens(l, h);
                  add_gen_orbit_stab_chain(l, h);

                  // add generator to <h> to orbit of base[l]
                }
                i      = no_base_pts_h_fixes;
                escape = true;
              }
              h->really_delete();
              delete h;
            }
            prod->really_delete();
            delete prod;
          }
        }

        if (!escape)
          i--;
      }
    }

    std::vector<Permutation<PointType>*>*             _gens;
    std::vector<PointType>                            _base;
    std::vector<PointType>                            _orbits;
    std::vector<uint64_t>                             _size_orbits;
    std::vector<Permutation<PointType>*>              _transversal;
    std::vector<Permutation<PointType>*>              _transversal_inv;
    std::vector<std::vector<Permutation<PointType>*>> _strong_gens;
    uint64_t                                          _deg;
    bool                                              _enumerated;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_SIMS_H_
