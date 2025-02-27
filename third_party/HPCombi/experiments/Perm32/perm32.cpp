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
using xpu8 = uint8_t __attribute__((vector_size(32)));
using perm32 = std::array<epu8, 2>;

// xpu8 &to_xpu8(perm32 &p) { return reinterpret_cast<xpu8 &>(p); }
// perm32 &from_xpu8(xpu8 &p) { return reinterpret_cast<perm32 &>(p); }
xpu8 to_xpu8(perm32 p) { return reinterpret_cast<xpu8 &>(p); }
perm32 from_xpu8(xpu8 p) { return reinterpret_cast<perm32 &>(p); }
__m256d to_m256d(perm32 p) { return reinterpret_cast<__m256d &>(p); }
perm32 from_m256d(__m256d p) { return reinterpret_cast<perm32 &>(p); }

inline uint8_t &set(perm32 &p, uint64_t i) { return *(&p[0][0] + i); }
inline uint8_t get(perm32 p, uint64_t i) { return *(&p[0][0] + i); }

/**********************************************************************/
/***************** Fonctions d'affichages *****************************/
/**********************************************************************/

/** Affichage perm32
 * Définition de l'opérateur d'affichage << pour le type perm32
 **/
std::ostream &operator<<(std::ostream &stream, perm32 const &p) {
    using namespace std;
    stream << "[" << setw(2) << hex << unsigned(get(p, 0));
    for (unsigned i = 1; i < 32; ++i)
        stream << "," << setw(2) << unsigned(get(p, i));
    stream << dec << "]";
    return stream;
}

/**********************************************************************/
/****** Permutations Variables globales et fonctions de base **********/
/**********************************************************************/

/** Permutation identité **/
const perm32 permid{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                    22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

/**********************************************************************/
/************************ Utilitaires *********************************/
/**********************************************************************/

perm32 random_perm32() {
    perm32 res = permid;
    std::random_shuffle(&set(res, 0), &set(res, 32));
    return res;
}

/** Construit un vecteurs d'ar16 au hasard
 * @param sz le nombre d'élements
 * @return le vecteur correspondant
 **/
std::vector<perm32> rand_perms(int sz) {
    std::vector<perm32> res(sz);
    std::srand(std::time(0));
    for (int i = 0; i < sz; i++)
        res[i] = random_perm32();
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

inline bool eqperm32(perm32 p1, perm32 p2) {
    return (_mm_movemask_epi8(_mm_cmpeq_epi8(p1[0], p2[0])) == 0xffff) &
           (_mm_movemask_epi8(_mm_cmpeq_epi8(p1[1], p2[1])) == 0xffff);
}

perm32 permute(perm32 v1, perm32 v2) {
    return {_mm_blendv_epi8(_mm_shuffle_epi8(v1[1], v2[0]),
                            _mm_shuffle_epi8(v1[0], v2[0]), v2[0] < 16),
            _mm_blendv_epi8(_mm_shuffle_epi8(v1[1], v2[1]),
                            _mm_shuffle_epi8(v1[0], v2[1]), v2[1] < 16)};
}

perm32 permute_ref(perm32 v1, perm32 v2) {
    perm32 res;
    for (uint64_t i = 0; i < 32; i++)
        set(res, i) = get(v1, get(v2, i));
    return res;
}

int main() {
    using namespace std;
    srand(time(0));
    perm32 v1 = random_perm32();
    perm32 v2 = random_perm32();
    cout << permid << endl;
    cout << v1 << endl;
    cout << v2 << endl;
    cout << permute(v1, v2) << endl;
    cout << permute_ref(v1, v2) << endl;

    cout << from_xpu8(to_xpu8(v1) & to_xpu8(v2)) << endl;
    //  cout << from_xpu8(_mm256_and_pd((__m256d) to_xpu8(v1), (__m256d)
    //  to_xpu8(v2))) << endl;
    cout << from_m256d(
                _mm256_and_pd((__m256d)to_m256d(v1), (__m256d)to_m256d(v2)))
         << endl;

    cout << "Sampling : ";
    cout.flush();
    auto vrand = rand_perms(100000);
    cout << "Done !" << endl;
    std::vector<perm32> check_ref(vrand.size());
    std::vector<perm32> check(vrand.size());

    cout << "Ref :  ";
    double sp_ref = timethat(
        [&vrand, &check_ref]() {
            std::transform(vrand.begin(), vrand.end(), check_ref.begin(),
                           [](perm32 p) {
                               for (int i = 0; i < 800; i++)
                                   p = permute_ref(p, p);
                               return p;
                           });
        },
        0.0);

    cout << "Fast : ";
    timethat(
        [&vrand, &check]() {
            std::transform(vrand.begin(), vrand.end(), check.begin(),
                           [](perm32 p) {
                               for (int i = 0; i < 800; i++)
                                   p = permute(p, p);
                               return p;
                           });
        },
        sp_ref);

    cout << "Checking : ";
    cout.flush();
    assert(std::mismatch(check_ref.begin(), check_ref.end(), check.begin(),
                         eqperm32) ==
           std::make_pair(check_ref.end(), check.end()));
    cout << "Ok !" << endl;
}
