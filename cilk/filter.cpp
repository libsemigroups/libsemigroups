#include <x86intrin.h>
#include <cassert>
#include <cstdint>
#include <algorithm>
#include <array>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <atomic>

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>

#include <hpcombi.hpp>

using namespace HPCombi;

const int N = 7;


/**** Cycle for permutation enumeration *************************************/
Perm16 cycle(uint64_t n) {
  assert(n <= 16);
  epu8 res = epu8id;
  for (uint64_t j=1; j < n; j++) res[j] = j-1;
  res[0] = n-1;
  return res;
}
std::array<Perm16, 17> cycles;

void init_cycles() {
  for (uint64_t i = 0; i <= 16; i++) cycles[i] = cycle(i);
}


bool permute_row_space_included(BMat8 a, BMat8 b) {
    Perm16 p = Perm16::one();
    for (uint64_t i6=0; i6 <= 6; i6 ++) {
        p = p * cycles[7];
    for (uint64_t i5=0; i5 <= 5; i5 ++) {
        p = p * cycles[6];
    for (uint64_t i4=0; i4 <= 4; i4 ++) {
        p = p * cycles[5];
    for (uint64_t i3=0; i3 <= 3; i3 ++) {
        p = p * cycles[4];
    for (uint64_t i2=0; i2 <= 2; i2 ++) {
        p = p * cycles[3];
    for (uint64_t i1=0; i1 <= 1; i1 ++) {
        p = p * cycles[2];
        BMat8 ap = a.col_permuted(p);
        if (ap.row_space_included(b)) {
            return true;
        }
    }
    }
    }
    }
    }
    }
    return false;
}

std::vector<BMat8> readfile(std::string filename) {
    std::ifstream      f(filename);
    std::string        line;

    std::vector<BMat8> res;
    while (std::getline(f, line)) {
        BMat8 m(std::stoul(line));
        if (m.row_space_size() != (1 << N)) { // get rid of the identity
            res.push_back(m);
        }
    }
    f.close();
    return res;
}

int main(int argc, char *argv[]) {
    std::ostringstream filename;
    filename << "bmat_trim_enum_" << N << ".txt";
    std::vector<BMat8> bmat_enum = readfile(filename.str());

    init_cycles();
    for (int i=0; i<8; i++) std::cout << cycles[i] << std::endl;

    int sz = bmat_enum.size();
    BMat8 v1 = bmat_enum[sz-1];
    BMat8 v2 = bmat_enum[sz-2];
    std::cout << v1.row_space_included(v2) << " "
              << v2.row_space_included(v1) << std::endl;

    std::vector<bool> removed(bmat_enum.size(), false);

    std::cout << "Vector size = " << bmat_enum.size() << std::endl;
    std::atomic<int> done (0);
    cilk_for (unsigned int i=0; i < bmat_enum.size(); i++) {
        auto v = bmat_enum[i];
        cilk_for (unsigned int j=0; j < bmat_enum.size(); j++) {
            if (i != j) {
                auto v1 = bmat_enum[j];
//                 if (not removed[i] && v1.row_space_included(v)) {
                if (not removed[i] && permute_row_space_included(v1, v)) {
                    removed[j] = true;
                }
            }
        }
        done++;
        std::cout << "Done= " << done << " / " << bmat_enum.size() << std::endl;
    }
    int count = 0;
    for (unsigned int i=0; i < bmat_enum.size(); i++) {
        if (not removed[i]) {
//            std::cout << bmat_enum[i] << std::endl;
            count ++;
        }
    }
    std::cout << "Kept= " << count << " / " << bmat_enum.size() << std::endl;
}
