/******************************************************************************/
/*       Copyright (C) 2017 Florent Hivert <Florent.Hivert@lri.fr>,           */
/*                                                                            */
/*  Distributed under the terms of the GNU General Public License (GPL)       */
/*                                                                            */
/*    This code is distributed in the hope that it will be useful,            */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*   General Public License for more details.                                 */
/*                                                                            */
/*  The full text of the GPL is available at:                                 */
/*                                                                            */
/*                  http://www.gnu.org/licenses/                              */
/******************************************************************************/

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>
#include <x86intrin.h>

/**********************************************************************/
/************** Défnitions des types et convertisseurs ****************/
/**********************************************************************/

/** Variable vectorielle
 * vecteur de 16 byte représentant une permutation
 * supporte les commandees vectorielles du processeur
 **/
using epu8 = uint8_t __attribute__((vector_size(16)));
using perm64 = std::array<epu8, 4>;

inline uint8_t &set(perm64 &p, uint64_t i) { return *(&p[0][0] + i); }
inline uint8_t get(perm64 p, uint64_t i) { return *(&p[0][0] + i); }

/**********************************************************************/
/***************** Fonctions d'affichages *****************************/
/**********************************************************************/

/** Affichage perm64
 * Définition de l'opérateur d'affichage << pour le type perm64
 **/
std::ostream &operator<<(std::ostream &stream, perm64 const &p) {
    using namespace std;
    stream << "[" << setw(2) << hex << unsigned(get(p, 0));
    for (unsigned i = 1; i < 32; ++i)
        stream << "," << setw(2) << unsigned(get(p, i));
    stream << dec << "...]";
    return stream;
}

/**********************************************************************/
/****** Permutations Variables globales et fonctions de base **********/
/**********************************************************************/

/** Permutation identité **/
const perm64 permid{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
                    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};

/**********************************************************************/
/************************ Utilitaires *********************************/
/**********************************************************************/

perm64 random_perm64() {
    perm64 res = permid;
    std::random_shuffle(&set(res, 0), &set(res, 64));
    return res;
}

/** Construit un vecteurs d'ar16 au hasard
 * @param sz le nombre d'élements
 * @return le vecteur correspondant
 **/
std::vector<perm64> rand_perms(int sz) {
    std::vector<perm64> res(sz);
    std::srand(std::time(0));
    for (int i = 0; i < sz; i++)
        res[i] = random_perm64();
    return res;
}

/** Calcul et affiche le temps de calcul d'une fonction
 * @param func la fonction à executer
 * @param reftime le temps de référence
 * @return le temps d'exécution
 **/
template <typename Func> double timethat(Func fun, double reftime = 0) {
    using namespace std::chrono;
    auto tstart = high_resolution_clock::now();
    fun();
    auto tfin = high_resolution_clock::now();

    auto tm = duration_cast<duration<double>>(tfin - tstart);
    std::cout << "time = " << std::setprecision(3) << tm.count() << "s";
    if (reftime != 0)
        std::cout << ", speedup = " << reftime / tm.count();
    std::cout << std::endl;
    return tm.count();
}

/**********************************************************************/
/************************ Primitives  *********************************/
/**********************************************************************/

inline bool eqperm64(perm64 p1, perm64 p2) {
    for (uint64_t i = 0; i < 4; i++)
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(p1[i], p2[i])) != 0xffff)
            return false;
    return true;
}

perm64 permute_1(perm64 v1, perm64 v2) {
    perm64 res = {};
    for (uint64_t i = 0; i < 4; i++) {
        for (uint64_t j = 0; j < 4; j++) {
            res[j] = _mm_blendv_epi8(res[j], _mm_shuffle_epi8(v1[i], v2[j]),
                                     v2[j] <= 15);
            v2[j] -= 16;
        }
    }
    return res;
}

perm64 permute_2(perm64 v1, perm64 v2) {
    perm64 res;
    for (uint64_t j = 0; j < 4; j++) {
        res[j] = _mm_shuffle_epi8(v1[0], v2[j]);
        v2[j] -= 16;
    }
    for (uint64_t i = 1; i < 4; i++) {
        for (uint64_t j = 0; j < 4; j++) {
            res[j] = _mm_blendv_epi8(res[j], _mm_shuffle_epi8(v1[i], v2[j]),
                                     v2[j] <= 15);
            v2[j] -= 16;
        }
    }
    return res;
}

perm64 permute_3(perm64 v1, perm64 v2) {
    perm64 res;
    for (uint64_t j = 0; j < 4; j++) {
        res[j] = _mm_shuffle_epi8(v1[0], v2[j]);
        v2[j] -= 16;
        res[j] = _mm_blendv_epi8(res[j], _mm_shuffle_epi8(v1[1], v2[j]),
                                 v2[j] <= 15);
        v2[j] -= 16;
        res[j] = _mm_blendv_epi8(res[j], _mm_shuffle_epi8(v1[2], v2[j]),
                                 v2[j] <= 15);
        v2[j] -= 16;
        res[j] = _mm_blendv_epi8(res[j], _mm_shuffle_epi8(v1[3], v2[j]),
                                 v2[j] <= 15);
    }
    return res;
}

perm64 permute_ref(perm64 v1, perm64 v2) {
    perm64 res;
    for (uint64_t i = 0; i < 64; i++)
        set(res, i) = get(v1, get(v2, i));
    return res;
}

int main() {
    using namespace std;
    srand(time(0));
    perm64 v1 = random_perm64();
    perm64 v2 = random_perm64();
    cout << permid << endl;
    cout << v1 << endl;
    cout << v2 << endl << endl;
    cout << permute_ref(v1, v2) << endl << endl;
    cout << permute_1(v1, v2) << endl;
    cout << permute_2(v1, v2) << endl;
    cout << permute_3(v1, v2) << endl;

    cout << "Sampling : ";
    cout.flush();
    auto vrand = rand_perms(100000);
    cout << "Done !" << endl;
    std::vector<perm64> check_ref(vrand.size());
    std::vector<perm64> check_1(vrand.size());
    std::vector<perm64> check_2(vrand.size());
    std::vector<perm64> check_3(vrand.size());

    cout << "Ref  :  ";
    double sp_ref = timethat(
        [&vrand, &check_ref]() {
            std::transform(vrand.begin(), vrand.end(), check_ref.begin(),
                           [](perm64 p) {
                               for (int i = 0; i < 800; i++)
                                   p = permute_ref(p, p);
                               return p;
                           });
        },
        0.0);

    cout << "Fast : ";
    timethat(
        [&vrand, &check_1]() {
            std::transform(vrand.begin(), vrand.end(), check_1.begin(),
                           [](perm64 p) {
                               for (int i = 0; i < 800; i++)
                                   p = permute_1(p, p);
                               return p;
                           });
        },
        sp_ref);

    cout << "Fast2:  ";
    timethat(
        [&vrand, &check_2]() {
            std::transform(vrand.begin(), vrand.end(), check_2.begin(),
                           [](perm64 p) {
                               for (int i = 0; i < 800; i++)
                                   p = permute_2(p, p);
                               return p;
                           });
        },
        sp_ref);

    cout << "Fast3:  ";
    timethat(
        [&vrand, &check_3]() {
            std::transform(vrand.begin(), vrand.end(), check_3.begin(),
                           [](perm64 p) {
                               for (int i = 0; i < 800; i++)
                                   p = permute_3(p, p);
                               return p;
                           });
        },
        sp_ref);

    cout << "Checking : ";
    cout.flush();
    assert(std::mismatch(check_ref.begin(), check_ref.end(), check_1.begin(),
                         eqperm64) ==
           std::make_pair(check_ref.end(), check_1.end()));
    assert(std::mismatch(check_ref.begin(), check_ref.end(), check_2.begin(),
                         eqperm64) ==
           std::make_pair(check_ref.end(), check_2.end()));
    assert(std::mismatch(check_ref.begin(), check_ref.end(), check_3.begin(),
                         eqperm64) ==
           std::make_pair(check_ref.end(), check_3.end()));
    cout << "Ok !" << endl;
}
