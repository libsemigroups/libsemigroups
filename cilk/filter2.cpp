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
// cycles[i][j] contains the cycle (i i+1 ... j-1 j) ... when i <= j
std::array<std::array<Perm16, 17>, 17> cycles;

Perm16 cycle(uint8_t i, uint8_t j) {
  epu8 res = epu8id;
  for (uint8_t k=i; k < j; k++) res[k] = k+1;
  res[j] = i;
  return res;
}
void init_cycles() {
  for (uint8_t i = 0; i < 15; i++)
      for (uint8_t j = i+1; j < 16; j++)
          cycles[i][j] = cycle(i,j);
}

// An iterator for permutations of the interval [a..b-1]
class Perm16iter :
    public std::iterator<std::input_iterator_tag, Perm16> {
private:
    const size_t a, b;
    Perm16 p;
    size_t loops[16];
public:
    Perm16iter(size_t lo, size_t hi, Perm16 start = Perm16::one()) :
        a(lo), b(hi), p(start), loops{0} {};
    Perm16 operator*() const { return p; };
    Perm16iter& operator++() {
        size_t i;
        for (i = 1; loops[i] == i; i++) {
            p = p * cycles[a][a+i];
            loops[i] = 0;
        }
        p = p * cycles[a][a+i];
        loops[i]++;
        return *this;
    }
    // Only compare with end (compare with self
    operator bool() const {return loops[b-a] == 0;}
    bool not_at_end() const {return loops[b-a] == 0;}
};


/**** Row sorted by cardinalities *************************************/
const size_t Binom84 = 70;
std::array<std::array<uint8_t, Binom84>, 9> rows_card;
std::array<size_t, 9> sz_rows_card {};
void init_rows_card() {
    for (uint8_t i = 0; i < (1 << N) - 1; i++) {
        size_t card = _mm_popcnt_u32(i);
        rows_card[card][sz_rows_card[card]] = i;
        sz_rows_card[card]++;
    }
    rows_card[8][0] = 0xff;
    sz_rows_card[8] = 1;
}


/**** Unshuffling *************************************/
Perm16 unshuffle(uint8_t bset) {
    epu8 res = epu8id;
    size_t k=0, i;
    for (i=0; i<8; i++) {
        if (uint8_t(bset << i) >> 7)
            res[k++] = i;
    }
    for (i=0; i<8; i++) {
        if (!(uint8_t(bset << i) >> 7))
            res[k++] = i;
    }
    return res;
}
std::array<Perm16, 256> unshuffles;
void init_unshuffle() {
    for (uint8_t i=0; i<255; i++)
        unshuffles[i] = unshuffle(i);
    unshuffles[255] = epu8id;
}




bool try_row(BMat8 a, BMat8 b, size_t card, uint8_t row) {
//    cout << std::bitset<8>(row) << " "
//         << unshuffles[row] << endl << endl;
    b = b.col_permuted(unshuffles[row]);
    for (Perm16iter it(0, card); it; ++it) {
        for (Perm16iter it2(card, N, *it); it2; ++it2) {
            BMat8 ap = a.col_permuted(*it2);
            if (ap.row_space_included(b)) {
                // cout << "Found" << endl << ap << endl << endl << b << endl;
                return true;
            }
        }
    }
    return false;
}

bool permute_row_space_included(BMat8 a, BMat8 b) {
    // sort the columns of a
    a = a.transpose();
    a = BMat8(_mm_extract_epi64(sorted8(_mm_set_epi64x(0, a.to_int())), 0));
    a = a.transpose();
    size_t card = _mm_popcnt_u64(a.to_int() & 0xff00000000000000);
    cout << "Card = " << card << endl << endl;
    if (card == 0) {
        cout << a << endl;
        if (a.row_space_size() == 1) return true;
        a = a.row_permuted(cycles[0][N]);
        return permute_row_space_included(a, b);
    }

    std::array<uint8_t, Binom84> &rows = rows_card[card];
    size_t nbrows = sz_rows_card[card];
    // cout << "Number of rows " << nbrows << endl;
    for (size_t irc = 0; irc < nbrows; irc += 16) {
        epu8 rows16 = _mm_lddqu_si128((__m128i*)&(rows[irc]));
        epu8 mask = b.row_space_mask(rows16);
        //cout << hex << rows16 << b.row_space_mask(rows16) << endl;
        for (size_t i = 0; i < min(size_t(16), nbrows - irc); i++) {
            if (mask[i]) {
                if (try_row(a, b, card, rows16[i])) return true;
            }
        }
    }
    return false;
}


// Reference version
bool permute_row_space_included_ref(BMat8 a, BMat8 b) {
    a = a.transpose();
    Perm16 p = Perm16::one();
/*    for (uint64_t i7=0; i7 <= 7; i7 ++) {
      p = p * cycles[0][7]; */
    for (uint64_t i6=0; i6 <= 6; i6 ++) {
        p = p * cycles[0][6];
    for (uint64_t i5=0; i5 <= 5; i5 ++) {
        p = p * cycles[0][5];
    for (uint64_t i4=0; i4 <= 4; i4 ++) {
        p = p * cycles[0][4];
    for (uint64_t i3=0; i3 <= 3; i3 ++) {
        p = p * cycles[0][3];
    for (uint64_t i2=0; i2 <= 2; i2 ++) {
        p = p * cycles[0][2];
        BMat8 ap1 = a.row_permuted(p);
        BMat8 ap2 = a.row_permuted(p * cycles[0][1]);
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
/*    } */
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


void init_rsincl() {
    init_cycles();
    init_rows_card();
    init_unshuffle();
}

int main(int argc, char *argv[]) {
    init_rsincl();

    BMat8 m1 = BMat8(8416165773404205568u);
    BMat8 m = BMat8(9241421688657806848u);


    cout << m1 << endl << m << endl;
    
    cout << "Ref = " << permute_row_space_included_ref(m1, m)
         << " "  << permute_row_space_included(m1, m);

    return 0;
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
    for (unsigned int i=0; i < bmat_enum.size(); i++) {
        auto v = bmat_enum[i];
        for (unsigned int j=0; j < bmat_enum.size(); j++) {
            if (i != j && not removed[j]) {
                auto v1 = bmat_enum[j];
                if (permute_row_space_included_ref(v1, v) !=
                    permute_row_space_included(v1, v)) {
                    cout << v1.to_int() << endl << v.to_int() << endl;
                    return -1;
                }
                if (not removed[i] && permute_row_space_included_ref(v1, v)) {
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
