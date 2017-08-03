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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This file contains ...
//
#ifndef LIBSEMIGROUPS_SRC_SIMS_H_
#define LIBSEMIGROUPS_SRC_SIMS_H_

#include "semigroups.h"  // TODO can this be removed?

namespace libsemigroups {
  // Schreier-Sims set up

  // deletes the contents of a Permutation container
  template <typename T, typename PointType>
  static inline void really_delete_cont(T* cont) {
    for (Permutation<PointType>* x : *cont) {
      x->really_delete();
      delete x;
    }
  }

  // copies a vector of permutations
  template <typename PointType>
  static inline std::vector<Permutation<PointType>*>*
  really_copy_vec(std::vector<Permutation<PointType>*>* const cont) {
    std::vector<Permutation<PointType>*>* out
        = new std::vector<Permutation<PointType>*>({});
    for (Permutation<PointType>* x : *cont)
      out->push_back(static_cast<Permutation<PointType>*>(x->really_copy()));
    return out;
  }

  // adds a permutaion to the posth position of strong_gens
  template <typename PointType>
  static inline void add_strong_gens(
      uint64_t const                                            pos,
      Permutation<PointType>* const                             value,
      std::vector<std::vector<Permutation<PointType>*>*>* const strong_gens) {
    if ((*strong_gens)[pos] == nullptr)
      (*strong_gens)[pos] = new std::vector<Permutation<PointType>*>({});
    (*strong_gens)[pos]->push_back(
        static_cast<Permutation<PointType>*>((value->really_copy())));
  }

  // stores a permutation in the transversal vector
  template <typename PointType>
  static inline void
  set_transversal(uint64_t const                              i,
                  uint64_t const                              j,
                  Permutation<PointType>* const               value,
                  std::vector<Permutation<PointType>*>* const transversal,
                  std::vector<Permutation<PointType>*>* const transversal_inv,
                  uint64_t const                              deg) {
    if ((*transversal)[i * deg + j] != nullptr) {
      (*transversal)[i * deg + j]->really_delete();
      (*transversal_inv)[i * deg + j]->really_delete();
      delete (*transversal)[i * deg + j];
      delete (*transversal_inv)[i * deg + j];
    }
    (*transversal)[i * deg + j]     = value;
    (*transversal_inv)[i * deg + j] = value->inverse();
  }

  // checks if x fixes all points in the base
  template <typename PointType>
  static bool perm_fixes_points(Permutation<PointType>* const x,
                                std::vector<PointType>* const base) {
    for (uint64_t i = 0; i < base->size(); i++)
      if ((*x)[(*base)[i]] != (*base)[i])
        return false;
    return true;
  }

  // adds a point to the base (and initialises orbit stuff)
  template <typename PointType>
  static void
  add_base_point(PointType const                             p,
                 std::vector<PointType>* const               base,
                 std::vector<PointType>* const               orbits,
                 std::vector<uint64_t>* const                size_orbits,
                 std::vector<Permutation<PointType>*>* const transversal,
                 std::vector<Permutation<PointType>*>* const transversal_inv,
                 uint64_t const                              deg) {
    std::vector<PointType> e(deg);
    for (uint64_t i = 0; i < deg; i++)
      e[i] = i;
    (*size_orbits)[base->size()]               = 1;
    (*orbits)[base->size() * deg]              = p;
    (*transversal)[base->size() * deg + p]     = new Permutation<PointType>(e);
    (*transversal_inv)[base->size() * deg + p] = new Permutation<PointType>(e);
    base->push_back(p);
  }

  template <typename PointType>
  static void free_stab_chain(
      std::vector<Permutation<PointType>*>* const               transversal,
      std::vector<Permutation<PointType>*>* const               transversal_inv,
      std::vector<std::vector<Permutation<PointType>*>*>* const strong_gens,
      uint64_t const                                            deg) {
    uint64_t i, j, k;
    for (i = 0; i < deg; i++)
      for (j = 0; j < deg; j++) {
        k = i * deg + j;
        if ((*transversal)[k] != nullptr) {
          (*transversal)[k]->really_delete();
          delete (*transversal)[k];
          (*transversal_inv)[k]->really_delete();
          delete (*transversal_inv)[k];
        }
      }

    delete transversal;
    delete transversal_inv;

    // free the strong_gens
    for (i = 0; i < deg + 1; i++)
      if ((*strong_gens)[i] != nullptr) {
        really_delete_cont<std::vector<Permutation<PointType>*>, PointType>(
            (*strong_gens)[i]);
        delete (*strong_gens)[i];
      }
    delete strong_gens;
  }

  // calculates the orbit of init_pt in the `depth`th stabiliser
  template <typename PointType>
  static void orbit_stab_chain(
      uint64_t const                                            depth,
      std::vector<PointType>* const                             orbits,
      std::vector<uint64_t>* const                              size_orbits,
      std::vector<Permutation<PointType>*>* const               transversal,
      std::vector<Permutation<PointType>*>* const               transversal_inv,
      std::vector<std::vector<Permutation<PointType>*>*>* const strong_gens,
      uint64_t const                                            deg) {
    PointType               pt;
    Permutation<PointType>* x;
    Permutation<PointType>* prod;

    for (uint64_t i = 0; i < (*size_orbits)[depth]; i++) {
      pt = (*orbits)[depth * deg + i];
      for (uint64_t j = 0; j < (*strong_gens)[depth]->size(); j++) {
        x = (*((*strong_gens)[depth]))[j];
        if ((*transversal)[depth * deg + (*x)[pt]] == nullptr) {
          (*orbits)[depth * deg + (*size_orbits)[depth]] = (*x)[pt];
          (*size_orbits)[depth]++;
          prod = new Permutation<PointType>(new std::vector<PointType>(deg));
          prod->redefine((*transversal)[depth * deg + pt], x);
          set_transversal<PointType>(
              depth, (*x)[pt], prod, transversal, transversal_inv, deg);
        }
      }
    }
  }

  // apply the new generator to existing points in orbits[depth]
  template <typename PointType>
  static void add_gen_orbit_stab_chain(
      uint64_t const                                            depth,
      Permutation<PointType>* const                             gen,
      std::vector<PointType>* const                             orbits,
      std::vector<uint64_t>* const                              size_orbits,
      std::vector<Permutation<PointType>*>* const               transversal,
      std::vector<Permutation<PointType>*>* const               transversal_inv,
      std::vector<std::vector<Permutation<PointType>*>*>* const strong_gens,
      uint64_t const                                            deg) {
    uint64_t                i, j;
    PointType               pt;
    Permutation<PointType>* x;
    Permutation<PointType>* prod;

    // apply the new generator to existing points in orbits[depth]
    for (i = 0; i < (*size_orbits)[depth]; i++) {
      pt = (*orbits)[depth * deg + i];
      if ((*transversal)[depth * deg + (*gen)[pt]] == nullptr) {
        (*orbits)[depth * deg + (*size_orbits)[depth]] = (*gen)[pt];
        (*size_orbits)[depth]++;
        prod = new Permutation<PointType>(new std::vector<PointType>(deg));
        prod->redefine((*transversal)[depth * deg + pt], gen);
        set_transversal<PointType>(
            depth, (*gen)[pt], prod, transversal, transversal_inv, deg);
      }
    }

    for (i = (*size_orbits)[depth]; i < (*size_orbits)[depth]; i++) {
      pt = (*orbits)[depth * deg + i];
      for (j = 0; j < (*strong_gens)[depth]->size(); j++) {
        x = (*((*strong_gens)[depth]))[j];
        if ((*transversal)[depth * deg + (*x)[pt]] == nullptr) {
          (*orbits)[depth * deg + (*size_orbits)[depth]] = (*x)[pt];
          (*size_orbits)[depth]++;
          prod = new Permutation<PointType>(new std::vector<PointType>(deg));
          prod->redefine((*transversal)[depth * deg + pt], x);
          set_transversal<PointType>(
              depth, (*x)[pt], prod, transversal, transversal_inv, deg);
        }
      }
    }
  }

  // changes g so that it fixes the current base
  template <typename PointType>
  static void
  sift_stab_chain(Permutation<PointType>* const               g,
                  uint64_t* const                             depth,
                  std::vector<PointType>* const               base,
                  std::vector<Permutation<PointType>*>* const transversal_inv,
                  uint64_t const                              deg) {
    PointType beta;
    Element*  g_copy;
    assert(*depth == 0);

    for (; *depth < base->size(); (*depth)++) {
      beta = (*g)[(*base)[*depth]];
      if ((*transversal_inv)[*depth * deg + beta] == nullptr)
        return;
      g_copy = g->really_copy();
      g->redefine(g_copy, (*transversal_inv)[*depth * deg + beta]);
      g_copy->really_delete();
      delete g_copy;
    }
    assert(perm_fixes_points<PointType>(g, base));
  }

  // builds a strong base and generating set for the group generated by
  // strong_gens[0] and its stabilisers up to depth
  template <typename PointType>
  static void schreier_sims_stab_chain(
      uint64_t const                                            depth,
      std::vector<PointType>* const                             base,
      std::vector<PointType>* const                             orbits,
      std::vector<uint64_t>* const                              size_orbits,
      std::vector<Permutation<PointType>*>* const               transversal,
      std::vector<Permutation<PointType>*>* const               transversal_inv,
      std::vector<std::vector<Permutation<PointType>*>*>* const strong_gens,
      uint64_t const                                            deg) {
    Permutation<PointType>* x;
    Permutation<PointType>* h;
    Permutation<PointType>* prod;
    bool                    escape, h_fixes_all_base_pts;
    int                     i;
    uint64_t                j, no_base_pts_h_fixes, k, l, m;
    PointType               beta, betax;

    for (i = 0; i <= static_cast<int>(depth); i++) {
      if ((*strong_gens)[i] != nullptr) {
        for (Permutation<PointType>* x : *((*strong_gens)[i])) {
          if (perm_fixes_points<PointType>(x, base)) {
            for (k = 0; k < deg; k++) {
              if (k != (*x)[k]) {
                add_base_point<PointType>(k,
                                          base,
                                          orbits,
                                          size_orbits,
                                          transversal,
                                          transversal_inv,
                                          deg);
                break;
              }
            }
          }
        }
      }
    }

    for (i = depth + 1; i <= static_cast<int>(base->size()); i++) {
      beta = (*base)[i - 1];
      // set up the strong generators
      for (j = 0; j < (*strong_gens)[i - 1]->size(); j++)
        if (beta == (*(*((*strong_gens)[i - 1]))[j])[beta]) {
          add_strong_gens<PointType>(
              i, (*((*strong_gens)[i - 1]))[j], strong_gens);
        }

      // find the orbit of base[i -1] under strong_gens[i - 1]
      orbit_stab_chain<PointType>(i - 1,
                                  orbits,
                                  size_orbits,
                                  transversal,
                                  transversal_inv,
                                  strong_gens,
                                  deg);
    }

    i = base->size() - 1;
    while (i >= static_cast<int>(depth)) {
      escape = false;
      for (j = 0; j < (*size_orbits)[i] && !escape; j++) {
        beta = (*orbits)[i * deg + j];
        for (m = 0; m < (*strong_gens)[i]->size() && !escape; m++) {
          x    = (*((*strong_gens)[i]))[m];
          prod = new Permutation<PointType>(std::vector<PointType>(deg));
          prod->redefine((*transversal)[i * deg + beta], x);
          betax = (*x)[beta];

          // TODO Not sure why this is needed (not in Wilf's C code)
          add_gen_orbit_stab_chain<PointType>(i,
                                              x,
                                              orbits,
                                              size_orbits,
                                              transversal,
                                              transversal_inv,
                                              strong_gens,
                                              deg);
          if (!(*prod == *((*transversal)[i * deg + betax]))) {
            h_fixes_all_base_pts = true;
            h = new Permutation<PointType>(std::vector<PointType>(deg));
            h->redefine(prod, (*transversal_inv)[i * deg + betax]);
            no_base_pts_h_fixes = 0;
            sift_stab_chain<PointType>(
                h, &no_base_pts_h_fixes, base, transversal_inv, deg);
            if (no_base_pts_h_fixes < base->size()) {
              h_fixes_all_base_pts = false;
            } else if (!(h->is_identity())) {
              h_fixes_all_base_pts = false;
              for (k = 0; k < deg; k++)
                if (k != (*h)[k]) {
                  add_base_point<PointType>(k,
                                            base,
                                            orbits,
                                            size_orbits,
                                            transversal,
                                            transversal_inv,
                                            deg);
                  break;
                }
            }

            if (!h_fixes_all_base_pts) {
              for (l = i + 1; l <= no_base_pts_h_fixes; l++) {
                add_strong_gens<PointType>(l, h, strong_gens);
                add_gen_orbit_stab_chain<PointType>(l,
                                                    h,
                                                    orbits,
                                                    size_orbits,
                                                    transversal,
                                                    transversal_inv,
                                                    strong_gens,
                                                    deg);
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

  //! Returns the size of the permutation group generated by a vector of
  //! permutations.
  template <typename PointType>
  extern uint64_t group_size(std::vector<Permutation<PointType>*>* const gens) {
    uint64_t               deg  = ((*gens)[0])->degree();
    std::vector<PointType> base = {};
    std::vector<uint64_t>  size_orbits(deg);
    std::vector<PointType> orbits(deg * deg);
    uint64_t               out = 1;

    std::vector<std::vector<Permutation<PointType>*>*>* strong_gens
        = new std::vector<std::vector<Permutation<PointType>*>*>(
            {really_copy_vec<PointType>(gens)});
    std::vector<Permutation<PointType>*>* transversal
        = new std::vector<Permutation<PointType>*>({});
    std::vector<Permutation<PointType>*>* transversal_inv
        = new std::vector<Permutation<PointType>*>({});

    strong_gens->resize(deg + 1);
    transversal->resize(deg * deg);
    transversal_inv->resize(deg * deg);

    add_base_point<PointType>(
        0, &base, &orbits, &size_orbits, transversal, transversal_inv, deg);
    schreier_sims_stab_chain<PointType>(0,
                                        &base,
                                        &orbits,
                                        &size_orbits,
                                        transversal,
                                        transversal_inv,
                                        strong_gens,
                                        deg);

    free_stab_chain<PointType>(transversal, transversal_inv, strong_gens, deg);
    for (uint64_t i = 0; i < base.size(); i++)
      out = out * size_orbits[i];
    return out;
  }
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SRC_SIMS_H_
