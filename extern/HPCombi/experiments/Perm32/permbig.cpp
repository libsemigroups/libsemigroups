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

const uint64_t NBlock = 16;
const uint64_t Size = 16 * NBlock;

/**********************************************************************/
/************** Défnitions des types et convertisseurs ****************/
/**********************************************************************/

/** Variable vectorielle
 * vecteur de 16 byte représentant une permutation
 * supporte les commandees vectorielles du processeur
 **/
using epu8 = uint8_t __attribute__((vector_size(16)));
using perm = std::array<epu8, NBlock>;

inline uint8_t &set(perm &p, uint64_t i) { return *(&p[0][0] + i); }
inline uint8_t get(perm p, uint64_t i) { return *(&p[0][0] + i); }

/**********************************************************************/
/***************** Fonctions d'affichages *****************************/
/**********************************************************************/

/** Affichage perm
 * Définition de l'opérateur d'affichage << pour le type perm
 **/
std::ostream &operator<<(std::ostream &stream, perm const &p) {
    using namespace std;
    stream << "[" << setw(2) << hex << unsigned(get(p, 0));
    for (unsigned i = 1; i < 16; ++i)
        stream << "," << setw(2) << unsigned(get(p, i));
    stream << dec << "...]";
    return stream;
}

/**********************************************************************/
/****** Permutations Variables globales et fonctions de base **********/
/**********************************************************************/

const perm make_permid() {
    perm res;
    for (uint64_t i = 0; i < Size; i++)
        set(res, i) = i;
    return res;
}
/** Permutation identité **/
const perm permid = make_permid();
// constexpr perm permid = A<Size>();

/**********************************************************************/
/************************ Utilitaires *********************************/
/**********************************************************************/

perm random_perm() {
    perm res = permid;
    std::random_shuffle(&set(res, 0), &set(res, Size));
    return res;
}

/** Construit un vecteurs d'ar16 au hasard
 * @param sz le nombre d'élements
 * @return le vecteur correspondant
 **/
std::vector<perm> rand_perms(int sz) {
    std::vector<perm> res(sz);
    std::srand(std::time(0));
    for (int i = 0; i < sz; i++)
        res[i] = random_perm();
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

inline bool eqperm(perm p1, perm p2) {
    for (uint64_t i = 0; i < NBlock; i++)
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(p1[i], p2[i])) != 0xffff)
            return false;
    return true;
}

perm permute_1(const perm &v1, perm v2) {
    perm res = {};
    for (uint64_t i = 0; i < NBlock; i++) {
        for (uint64_t j = 0; j < NBlock; j++) {
            res[j] = _mm_blendv_epi8(res[j], _mm_shuffle_epi8(v1[i], v2[j]),
                                     v2[j] <= 15);
            v2[j] -= 16;
        }
    }
    return res;
}

perm permute_2(const perm &v1, perm v2) {
    perm res;
    for (uint64_t j = 0; j < NBlock; j++) {
        res[j] = _mm_shuffle_epi8(v1[0], v2[j]);
        v2[j] -= 16;
    }
    for (uint64_t i = 1; i < NBlock; i++) {
        for (uint64_t j = 0; j < NBlock; j++) {
            res[j] = _mm_blendv_epi8(res[j], _mm_shuffle_epi8(v1[i], v2[j]),
                                     v2[j] <= 15);
            v2[j] -= 16;
        }
    }
    return res;
}

perm permute_3(const perm &v1, const perm &v2) {
    perm res;
    for (uint64_t j = 0; j < NBlock; j++) {
        epu8 v2j = v2[j];
        res[j] = _mm_shuffle_epi8(v1[0], v2j);
        for (uint64_t i = 1; i < NBlock; i++) {
            v2j -= 16;
            res[j] = _mm_blendv_epi8(res[j], _mm_shuffle_epi8(v1[i], v2j),
                                     v2j <= 15);
        }
    }
    return res;
}

perm permute_ref(const perm &v1, const perm &v2) {
    perm res;
    for (uint64_t i = 0; i < Size; i++)
        set(res, i) = get(v1, get(v2, i));
    return res;
}

int main() {
    using namespace std;

    const int repl = 256;
    const int samplesz = 100000 / NBlock;

    srand(time(0));

    /*
    perm v1 = random_perm();
    perm v2 = random_perm();
    cout << permid << endl;
    cout << v1 << endl;
    cout << v2 << endl << endl;
    cout << permute_ref(v1, v2) << endl << endl;
    cout << permute_1(v1, v2) << endl;
    cout << permute_2(v1, v2) << endl;
    */

    cout << "NBlock = " << NBlock << endl;

    cout << "Sampling : ";
    cout.flush();
    auto vrand = rand_perms(samplesz);
    cout << "Done !" << endl;
    vector<perm> check_ref(vrand.size());
    vector<perm> check_1(vrand.size());
    vector<perm> check_2(vrand.size());
    vector<perm> check_3(vrand.size());

    cout << "Ref  :  ";
    double sp_ref = timethat(
        [&vrand, &check_ref]() {
            transform(vrand.begin(), vrand.end(), check_ref.begin(),
                      [](perm p) {
                          for (int i = 0; i < repl; i++)
                              p = permute_ref(p, p);
                          return p;
                      });
        },
        0.0);

    cout << "Fast : ";
    timethat(
        [&vrand, &check_1]() {
            transform(vrand.begin(), vrand.end(), check_1.begin(), [](perm p) {
                for (int i = 0; i < repl; i++)
                    p = permute_1(p, p);
                return p;
            });
        },
        sp_ref);

    cout << "Fast2:  ";
    timethat(
        [&vrand, &check_2]() {
            transform(vrand.begin(), vrand.end(), check_2.begin(), [](perm p) {
                for (int i = 0; i < repl; i++)
                    p = permute_2(p, p);
                return p;
            });
        },
        sp_ref);

    cout << "Fast3:  ";
    timethat(
        [&vrand, &check_3]() {
            transform(vrand.begin(), vrand.end(), check_3.begin(), [](perm p) {
                for (int i = 0; i < repl; i++)
                    p = permute_3(p, p);
                return p;
            });
        },
        sp_ref);

    cout << "Checking : ";
    cout.flush();
    assert(mismatch(check_ref.begin(), check_ref.end(), check_1.begin(),
                    eqperm) == make_pair(check_ref.end(), check_1.end()));
    assert(mismatch(check_ref.begin(), check_ref.end(), check_2.begin(),
                    eqperm) == make_pair(check_ref.end(), check_2.end()));
    assert(mismatch(check_ref.begin(), check_ref.end(), check_3.begin(),
                    eqperm) == make_pair(check_ref.end(), check_3.end()));
    cout << "Ok !" << endl;
}
