//****************************************************************************//
//       Copyright (C) 2016 Florent Hivert <Florent.Hivert@lri.fr>,           //
//                                                                            //
//  Distributed under the terms of the GNU General Public License (GPL)       //
//                                                                            //
//    This code is distributed in the hope that it will be useful,            //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
//   General Public License for more details.                                 //
//                                                                            //
//  The full text of the GPL is available at:                                 //
//                                                                            //
//                  http://www.gnu.org/licenses/                              //
//****************************************************************************//

#include "hpcombi/perm16.hpp"
#include "testtools.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace HPCombi;

#define ASSERT(test)                                                           \
    if (!(test))                                                               \
    cout << "Test failed in file " << __FILE__ << " line " << __LINE__         \
         << ": " #test << endl

int main() {
    auto vrand = rand_perms(1000);
    auto rep = 10000;

    cout << "Loop   : ";
    double reftime = timethat(
        [vrand]() {
            for (Perm16 v : vrand)
                ASSERT(v.sum_ref() == 120);
        },
        rep);
    cout << "4 rnds : ";
    timethat(
        [vrand]() {
            for (Perm16 v : vrand)
                ASSERT(v.sum4() == 120);
        },
        rep, reftime);
    cout << "3 rnds : ";
    timethat(
        [vrand]() {
            for (Perm16 v : vrand)
                ASSERT(v.sum3() == 120);
        },
        rep, reftime);
    return EXIT_SUCCESS;
}
