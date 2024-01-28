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

#include "creducer_opadd_array.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <x86intrin.h>
using namespace std;

// type tableau classique
using ar16 = array<uint8_t, 16>;
// Variable vectorielle
using perm = uint8_t __attribute__((vector_size(16), __may_alias__));

// conversion ar16 <-> perm
using converter = union {
    ar16 p;
    perm v8;
};

ar16 ar16_perm(perm v) {
    converter c;
    c.v8 = v;
    return c.p;
}

perm perm_ar16(ar16 p) {
    converter c;
    c.p = p;
    return c.v8;
}

// Affichage
ostream &operator<<(ostream &stream, ar16 const &ar) {
    stream << "[" << setw(2) << hex << unsigned(ar[0]);
    for (unsigned i = 1; i < 16; ++i)
        stream << "," << setw(2) << hex << unsigned(ar[i]) << dec;
    stream << "]";
    return stream;
}
ostream &operator<<(ostream &stream, perm const &p) {
    stream << ar16_perm(p);
    return stream;
}

// permutation identique
const ar16 ar16id = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const perm permid = perm_ar16(ar16id);

ar16 transposition(uint64_t i, uint64_t j) {
    assert(i < 16);
    assert(j < 16);
    ar16 res = ar16id;
    std::swap(res[i], res[j]);
    return res;
}
ar16 randomar16() {
    ar16 res = ar16id;
    random_shuffle(res.begin(), res.end());
    return res;
}

perm decal = perm_ar16({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15});

int nb_descent(perm p) {
    perm pdec = _mm_shuffle_epi8((__m128i)p, (__m128i)decal);
    pdec = (p > pdec);
    return _mm_popcnt_u32(_mm_movemask_epi8((__m128i)pdec));
}

array<array<perm, 16>, 16> transp;

void init_transp() {
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            transp[i][j] = perm_ar16(transposition(i, j));
}

perm permuteij(perm p, int i, int j) {
    return _mm_shuffle_epi8((__m128i)p, (__m128i)transp[i][j]);
}

cilkpub::creducer_opadd_array<unsigned long int> res_red(16);

/* parcours l'ensemble de toutes les permutations
   obtenues en permutant les n premières valeurs de perm */
void allperm(perm p, int n) {
    if (n == 0)
        res_red[nb_descent(p)] += 1;
    for (int i = 0; i < n; i++) {
        cilk_spawn allperm(permuteij(p, i, n - 1), n - 1);
    }
}

void allperm_iter7(perm p7) {
    unsigned long int res[16];
    for (int i = 0; i < 16; i++)
        res[i] = 0;
    for (int i6 = 0; i6 <= 6; i6++) {
        perm p6 = permuteij(p7, i6, 6);
        for (int i5 = 0; i5 <= 5; i5++) {
            perm p5 = permuteij(p6, i5, 5);
            for (int i4 = 0; i4 <= 4; i4++) {
                perm p4 = permuteij(p5, i4, 4);
                for (int i3 = 0; i3 <= 3; i3++) {
                    perm p3 = permuteij(p4, i3, 3);
                    for (int i2 = 0; i2 <= 2; i2++) {
                        perm p2 = permuteij(p3, i2, 2);
                        for (int i1 = 0; i1 <= 1; i1++)
                            res[nb_descent(permuteij(p2, i1, 1))]++;
                    }
                }
            }
        }
    }
    for (int i = 0; i < 16; i++)
        res_red[i] += res[i];
}

/* parcours l'ensemble de toutes les permutations
   obtenues en permutant les n premières valeurs de perm */
void allperm_derec(perm p, int n) {
    if (n == 7)
        allperm_iter7(p);
    else
        for (int i = 0; i < n; i++) {
            cilk_spawn allperm_derec(permuteij(p, i, n - 1), n - 1);
        }
}

void show_usage(std::string name) {
    cerr << "Usage: " << name << " [-n <proc_number>] size " << endl;
}

int main(int argc, char *argv[]) {
    int n;

    if (argc != 2 and argc != 4) {
        show_usage(argv[0]);
        return 1;
    }
    if (argc == 4) {
        if (std::string(argv[1]) != "-n") {
            show_usage(argv[0]);
            return 1;
        }
        std::string nproc = std::string(argv[2]);
        if (__cilkrts_set_param("nworkers", nproc.c_str()) !=
            __CILKRTS_SET_PARAM_SUCCESS)
            std::cerr << "Failed to set the number of Cilk workers"
                      << std::endl;
    }

    init_transp();
    n = atoi(argv[argc - 1]);

    unsigned long int res[16];
    for (int i = 0; i < 16; i++)
        res[i] = 0;

    res_red.move_in(res);
    allperm_derec(permid, n);
    res_red.move_out(res);

    std::cout << "Result: ";
    for (int i = 0; i < 16; i++)
        std::cout << res[i] << " ";
    std::cout << std::endl;
    return 0;
}
