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

const int N = 8;


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

bool try_row(BMat8 a, BMat8 b, size_t card, uint8_t row) {
//    cout << std::bitset<8>(row) << " "
//         << unshuffles[row] << endl << endl;
    b = b.col_permuted(unshuffles[row]);

    for (Perm16iter it(0, card); it; ++it) {
        for (Perm16iter it2(card, N, *it); it2; ++it2) {
            BMat8 ap = a.col_permuted(*it2);
            if (ap.row_space_included(b)) {
                cout << "Found" << endl << ap << endl << endl << b << endl;
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

void init_rsincl() {
    init_cycles();
    init_rows_card();
    init_unshuffle();
}

int main(int argc, char *argv[]) {
    init_rsincl();
    /*
    BMat8 bm({{1, 1, 1, 1, 0, 0, 0, 0},
              {0, 1, 0, 1, 0, 0, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}}),
         bm1({{0, 0, 0, 1, 0, 0, 1, 1},
              {0, 0, 1, 0, 0, 1, 0, 1},
              {1, 1, 0, 0, 1, 1, 0, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 0, 1, 1, 1, 1, 1},
              {0, 1, 0, 1, 0, 1, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 0}});
    */
    BMat8 bm({{1, 1, 1, 0, 0, 0, 0, 0},
              {0, 1, 0, 1, 0, 0, 0, 1},
              {0, 1, 1, 1, 0, 1, 0, 1},
              {1, 1, 0, 1, 1, 1, 1, 1},
              {0, 0, 1, 0, 0, 1, 1, 1},
              {1, 1, 0, 0, 0, 0, 0, 1},
              {0, 1, 0, 0, 0, 0, 1, 1},
              {0, 1, 1, 1, 1, 0, 1, 0}}),
         bm1({{1, 0, 0, 0, 0, 0, 0, 0},
              {0, 1, 0, 0, 0, 1, 0, 0},
              {0, 0, 0, 0, 1, 0, 0, 0},
              {0, 0, 0, 0, 1, 0, 0, 0},
              {0, 0, 0, 0, 0, 0, 1, 0},
              {0, 0, 0, 0, 1, 0, 1, 0},
              {0, 0, 0, 0, 0, 1, 0, 0},
              {0, 0, 0, 0, 0, 0, 1, 0}});

    cout << permute_row_space_included(bm, bm1) << endl;
}
