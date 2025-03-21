//****************************************************************************//
//     Copyright (C) 2017-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
//                                                                            //
//  This file is part of HP-Combi <https://github.com/libsemigroups/HPCombi>  //
//                                                                            //
//  HP-Combi is free software: you can redistribute it and/or modify it       //
//  under the terms of the GNU General Public License as published by the     //
//  Free Software Foundation, either version 3 of the License, or             //
//  (at your option) any later version.                                       //
//                                                                            //
//  HP-Combi is distributed in the hope that it will be useful, but WITHOUT   //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     //
//  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      //
//  for  more details.                                                        //
//                                                                            //
//  You should have received a copy of the GNU General Public License along   //
//  with HP-Combi. If not, see <https://www.gnu.org/licenses/>.               //
//****************************************************************************//

#include <algorithm>
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <x86intrin.h>

using namespace std;

const int SZ = 32;
/**********************************************************************/
/************** Défnitions des types et convertisseurs ****************/
/**********************************************************************/

/** Variable vectorielle
 * vecteur de 16 byte représentant une permutation
 * supporte les commandees vectorielles du processeur
 **/
using perm = uint8_t __attribute__((vector_size(SZ), __may_alias__));

/**********************************************************************/
/***************** Fonctions d'affichages *****************************/
/**********************************************************************/

/** Affichage perm
 * Définition de l'opérateur d'affichage << pour le type perm
 **/
ostream &operator<<(ostream &stream, perm const &p) {
    stream << "[" << setw(2) << hex << unsigned(p[0]);
    for (unsigned i = 1; i < SZ; ++i)
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
    v3 = v2 == v1;

    cout << permid << endl;
    cout << v1 << endl;
    cout << v2 << endl;
    cout << v3 << endl;

    cout << int(v1[0]) << " " << int(v1[1]) << endl;

    long int b = _mm256_movemask_epi8(v3);
    cout << "Application du masque : positions égales : " << hex << unsigned(b)
         << dec << endl;
    cout << "On compte les 1 avec une opération du processeur" << endl;
    cout << _mm_popcnt_u32(b) << endl;
}
