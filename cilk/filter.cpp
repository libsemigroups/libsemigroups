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

using namespace std;
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
    a = a.transpose();
    Perm16 p = Perm16::one();
/*    for (uint64_t i7=0; i7 <= 7; i7 ++) {
      p = p * cycles[8]; */
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
        BMat8 ap1 = a.row_permuted(p);
        BMat8 ap2 = a.row_permuted(p * cycles[2]);
        BMat8::transpose2(ap1, ap2);
        auto res = BMat8::row_space_included2(ap1, b, ap2, b);
        if (res.first || res.second) {
            return true;
        }
    }
    }
    }
    }
    }
/*      } */
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

int writefile(std::string filename,
              std::vector<BMat8> & bmat_enum,
              std::vector<bool> & removed) {
    std::ofstream  outf(filename);
    int count = 0;
    for (unsigned int i=0; i < bmat_enum.size(); i++) {
        if (not removed[i]) {
            outf << bmat_enum[i].to_int() << std::endl;
            count ++;
        }
    }
    outf.close();
    return count;
}

int main(int argc, char *argv[]) {
    /*
    cout << "extra " << endl
         << BMat8(13853107707017724416) << endl << endl
         << BMat8(4620710844295184384) << endl << endl
         << BMat8(4647750068672397824) << endl << endl
         << BMat8(9241421688590303744) << endl;
    
    return 0;
    */
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
    for (unsigned int i=0; i < bmat_enum.size(); i++) {
        if (bmat_enum[i].nr_rows() != N ||
            bmat_enum[i].transpose().row_space_basis().nr_rows() != N) {
            removed[i] = true;
        }
    }

    std::cout << "Vector size = " << bmat_enum.size() << std::endl;
    std::atomic<int> done (0);
    cilk_for (unsigned int i=0; i < bmat_enum.size(); i++) {
        auto v = bmat_enum[i];
        cilk_for (unsigned int j=0; j < bmat_enum.size(); j++) {
            if (i != j && not removed[j]) {
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

    std::ostringstream outfilename;
    outfilename << "bmat_filter_new_enum_" << N << ".txt";
    int count = writefile(outfilename.str(), bmat_enum, removed);
    std::cout << "Kept= " << count << " / " << bmat_enum.size() << std::endl;
}
