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
#include <iomanip>
#include <iostream>
#include <x86intrin.h>

using namespace std;

/**********************************************************************/
/************** Défnitions des types et convertisseurs ****************/
/**********************************************************************/

/** Variable vectorielle
 * vecteur de 16 byte représentant une permutation
 * supporte les commandees vectorielles du processeur
 **/
using perm = uint8_t __attribute__((vector_size(16), __may_alias__));

/**********************************************************************/
/***************** Fonctions d'affichages *****************************/
/**********************************************************************/

/** Affichage perm
 * Définition de l'opérateur d'affichage << pour le type perm
 **/
ostream &operator<<(ostream &stream, perm const &p) {
    stream << "[" << setw(2) << hex << unsigned(p[0]);
    for (unsigned i = 1; i < 16; ++i)
        stream << "," << setw(2) << hex << unsigned(p[i]) << dec;
    stream << "]";
    return stream;
}

/**********************************************************************/
/****** Permutations Variables globales et fonctions de base **********/
/**********************************************************************/

/** Permutation identité **/
const perm permid{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

/** Permutation décalée d'un cran à gauche **/
const perm decal{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15};

int main() {
    const perm v1{2, 1, 7, 4, 9, 15, 12, 0, 5, 3, 6, 8, 11, 10, 14, 13};
    const perm v2{2, 1, 32, 4, 8, 1, 12, 0, 4, 4, 4, 4, 41, 10, 14, 13};
    perm v3;
    v3 = v1 <= v2;

    cout << v1 << endl;
    cout << v2 << endl;

#define FIND_IN_VECT_MASK                                                      \
    (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_UNIT_MASK)
#define FIND_IN_VECT (_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY)

    cout << _mm_cmpestrm(v1, 16, v2, 16, FIND_IN_VECT_MASK) << endl;
    cout << hex
         << _mm_movemask_epi8(_mm_cmpestrm(v1, 16, v2, 16, FIND_IN_VECT_MASK))
         << endl;
    cout << _mm_cmpestrm(v1, 16, v2, 16, FIND_IN_VECT) << endl;
    cout << "=====" << endl;

    cout << _mm_cmpistrm(v1, v2, FIND_IN_VECT_MASK) << endl;
    cout << hex << _mm_movemask_epi8(_mm_cmpistrm(v1, v2, FIND_IN_VECT_MASK))
         << endl;
    cout << _mm_cmpistrm(v1, v2, FIND_IN_VECT) << endl;
}
