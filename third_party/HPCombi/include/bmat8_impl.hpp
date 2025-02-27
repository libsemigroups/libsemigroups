/******************************************************************************/
/*       Copyright (C) 2018 Finn Smith <fls3@st-andrews.ac.uk>                */
/*       Copyright (C) 2018 James Mitchell <jdm3@st-andrews.ac.uk>            */
/*       Copyright (C) 2018 Florent Hivert <Florent.Hivert@lri.fr>,           */
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

// This file contains an implementation of fast boolean matrices up to
// dimension 8 x 8.

namespace HPCombi {
static_assert(std::is_trivial<BMat8>(), "BMat8 is not a trivial class!");

// clang-format off
static const constexpr std::array<uint64_t, 8> ROW_MASK = {{
    0xff00000000000000, 0xff000000000000, 0xff0000000000, 0xff00000000,
    0xff000000,         0xff0000,         0xff00,         0xff}};

static const constexpr std::array<uint64_t, 8> COL_MASK = {{
    0x8080808080808080, 0x4040404040404040, 0x2020202020202020, 0x1010101010101010,
    0x808080808080808,  0x404040404040404,  0x202020202020202,  0x101010101010101}};

static const constexpr std::array<uint64_t, 64> BIT_MASK = {{
                                        0x8000000000000000,
                                        0x4000000000000000,
                                        0x2000000000000000,
                                        0x1000000000000000,
                                        0x800000000000000,
                                        0x400000000000000,
                                        0x200000000000000,
                                        0x100000000000000,
                                        0x80000000000000,
                                        0x40000000000000,
                                        0x20000000000000,
                                        0x10000000000000,
                                        0x8000000000000,
                                        0x4000000000000,
                                        0x2000000000000,
                                        0x1000000000000,
                                        0x800000000000,
                                        0x400000000000,
                                        0x200000000000,
                                        0x100000000000,
                                        0x80000000000,
                                        0x40000000000,
                                        0x20000000000,
                                        0x10000000000,
                                        0x8000000000,
                                        0x4000000000,
                                        0x2000000000,
                                        0x1000000000,
                                        0x800000000,
                                        0x400000000,
                                        0x200000000,
                                        0x100000000,
                                        0x80000000,
                                        0x40000000,
                                        0x20000000,
                                        0x10000000,
                                        0x8000000,
                                        0x4000000,
                                        0x2000000,
                                        0x1000000,
                                        0x800000,
                                        0x400000,
                                        0x200000,
                                        0x100000,
                                        0x80000,
                                        0x40000,
                                        0x20000,
                                        0x10000,
                                        0x8000,
                                        0x4000,
                                        0x2000,
                                        0x1000,
                                        0x800,
                                        0x400,
                                        0x200,
                                        0x100,
                                        0x80,
                                        0x40,
                                        0x20,
                                        0x10,
                                        0x8,
                                        0x4,
                                        0x2,
                                        0x1}};

// clang-format on

inline bool BMat8::operator()(size_t i, size_t j) const {
    HPCOMBI_ASSERT(i < 8);
    HPCOMBI_ASSERT(j < 8);
    return (_data << (8 * i + j)) >> 63;
}

inline void BMat8::set(size_t i, size_t j, bool val) {
    HPCOMBI_ASSERT(i < 8);
    HPCOMBI_ASSERT(j < 8);
    _data ^= (-val ^ _data) & BIT_MASK[8 * i + j];
}

inline BMat8::BMat8(std::vector<std::vector<bool>> const &mat) {
    // FIXME exceptions
    HPCOMBI_ASSERT(mat.size() <= 8);
    HPCOMBI_ASSERT(0 < mat.size());
    _data = 0;
    uint64_t pow = 1;
    pow = pow << 63;
    for (auto row : mat) {
        HPCOMBI_ASSERT(row.size() == mat.size());
        for (auto entry : row) {
            if (entry) {
                _data ^= pow;
            }
            pow = pow >> 1;
        }
        pow = pow >> (8 - mat.size());
    }
}

static std::random_device _rd;
static std::mt19937 _gen(_rd());
static std::uniform_int_distribution<uint64_t> _dist(0, 0xffffffffffffffff);

inline BMat8 BMat8::random() { return BMat8(_dist(_gen)); }

inline BMat8 BMat8::random(size_t const dim) {
    HPCOMBI_ASSERT(0 < dim && dim <= 8);
    BMat8 bm = BMat8::random();
    for (size_t i = dim; i < 8; ++i) {
        bm._data &= ~ROW_MASK[i];
        bm._data &= ~COL_MASK[i];
    }
    return bm;
}

inline BMat8 BMat8::transpose() const {
    uint64_t x = _data;
    uint64_t y = (x ^ (x >> 7)) & 0xAA00AA00AA00AA;
    x = x ^ y ^ (y << 7);
    y = (x ^ (x >> 14)) & 0xCCCC0000CCCC;
    x = x ^ y ^ (y << 14);
    y = (x ^ (x >> 28)) & 0xF0F0F0F0;
    x = x ^ y ^ (y << 28);
    return BMat8(x);
}


inline BMat8 BMat8::transpose_mask() const {
    epu8 x = _mm_set_epi64x(_data, _data << 1);
    uint64_t res = _mm_movemask_epi8(x);
    x = x << Epu8(2);
    res = res << 16 | _mm_movemask_epi8(x);
    x = x << Epu8(2);
    res = res << 16 | _mm_movemask_epi8(x);
    x = x << Epu8(2);
    res = res << 16 | _mm_movemask_epi8(x);
    return BMat8(res);
}

inline BMat8 BMat8::transpose_maskd() const {
    uint64_t res = _mm_movemask_epi8(_mm_set_epi64x(_data, _data << 1));
    res = res << 16 | _mm_movemask_epi8(_mm_set_epi64x(_data << 2, _data << 3));
    res = res << 16 | _mm_movemask_epi8(_mm_set_epi64x(_data << 4, _data << 5));
    res = res << 16 | _mm_movemask_epi8(_mm_set_epi64x(_data << 6, _data << 7));
    return BMat8(res);
}


using epu64 = uint64_t __attribute__ ((__vector_size__ (16), __may_alias__));

inline void BMat8::transpose2(BMat8 &a, BMat8 &b) {
    epu64 x = _mm_set_epi64x(a._data, b._data);
    epu64 y = (x ^ (x >> 7)) & (epu64 {0xAA00AA00AA00AA, 0xAA00AA00AA00AA});
    x = x ^ y ^ (y << 7);
    y = (x ^ (x >> 14)) & (epu64 {0xCCCC0000CCCC, 0xCCCC0000CCCC});
    x = x ^ y ^ (y << 14);
    y = (x ^ (x >> 28)) & (epu64 {0xF0F0F0F0, 0xF0F0F0F0});
    x = x ^ y ^ (y << 28);
    a._data = _mm_extract_epi64(x, 1);
    b._data = _mm_extract_epi64(x, 0);
}

static constexpr epu8 rotlow { 7, 0, 1, 2, 3, 4, 5, 6};
static constexpr epu8 rothigh
    { 0, 1, 2, 3, 4, 5, 6, 7,15, 8, 9,10,11,12,13,14};
static constexpr epu8 rotboth
    { 7, 0, 1, 2, 3, 4, 5, 6,15, 8, 9,10,11,12,13,14};
static constexpr epu8 rot2
    { 6, 7, 0, 1, 2, 3, 4, 5,14,15, 8, 9,10,11,12,13};

inline BMat8 BMat8::mult_transpose(BMat8 const &that) const {
    epu8 x = _mm_set_epi64x(_data, _data);
    epu8 y = _mm_shuffle_epi8(_mm_set_epi64x(that._data, that._data), rothigh);
    epu8 data {};
    epu8 diag {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
               0x80, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};
    for (int i = 0; i < 4; ++i) {
        data |= ((x & y) != epu8{}) & diag;
        y = _mm_shuffle_epi8(y, rot2);
        diag = _mm_shuffle_epi8(diag, rot2);
    }
    return BMat8(_mm_extract_epi64(data, 0) | _mm_extract_epi64(data, 1));
}

inline epu8 BMat8::row_space_basis_internal() const {
    epu8 res = remove_dups(revsorted8(_mm_set_epi64x(0, _data)));
    epu8 rescy = res;
    // We now compute the union of all the included different rows
    epu8 orincl {};
    for (int i = 0; i < 7; i++) {
        rescy = permuted(rescy, rotlow);
        orincl |= ((rescy | res) == res) & rescy;
    }
    res = (res != orincl) & res;
    return res;
}

inline BMat8 BMat8::row_space_basis() const {
    return BMat8(_mm_extract_epi64(sorted8(row_space_basis_internal()), 0));
}

#if defined(FF)
#error FF is defined !
#endif /* FF */
#define FF 0xff

constexpr std::array<epu8, 4> masks {{
// clang-format off
        {FF, 0,FF, 0,FF, 0,FF, 0,FF, 0,FF, 0,FF, 0,FF, 0},
        {FF,FF, 1, 1,FF,FF, 1, 1,FF,FF, 1, 1,FF,FF, 1, 1},
        {FF,FF,FF,FF, 2, 2, 2, 2,FF,FF,FF,FF, 2, 2, 2, 2},
        {FF,FF,FF,FF,FF,FF,FF,FF, 3, 3, 3, 3, 3, 3, 3, 3}
    }};
#undef FF

static const epu8 bound08 = _mm_slli_epi32(epu8id, 3); // shift for *8
static const epu8 bound18 = bound08 + Epu8(0x80);
static const epu8 shiftres {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};

inline void update_bitset(epu8 block, epu8 &set0, epu8 &set1) {
    for (size_t slice8 = 0; slice8 < 16; slice8++) {
        epu8 bm5 = Epu8(0xf8) & block; /* 11111000 */
        epu8 shft = _mm_shuffle_epi8(shiftres, block - bm5);
        set0 |= (bm5 == bound08) & shft;
        set1 |= (bm5 == bound18) & shft;
        block = _mm_shuffle_epi8(block, right_cycle);
    }
}

inline void BMat8::row_space_bitset(epu8 &res0, epu8 &res1) const {
    epu8 in = _mm_set_epi64x(0, _data);
    epu8 block0 {}, block1 {};
    for (epu8 m : masks) {
        block0 |= static_cast<epu8>(_mm_shuffle_epi8(in, m));
        block1 |= static_cast<epu8>(_mm_shuffle_epi8(in, m | Epu8(4)));
    }
    res0 = epu8 {}; res1 = epu8 {};
    for (size_t r = 0; r < 16; r++) {
        update_bitset(block0 | block1, res0, res1);
        block1 = _mm_shuffle_epi8(block1, right_cycle);
    }
}

inline uint64_t BMat8::row_space_size_bitset() const {
    epu8 res0 {}, res1 {};
    row_space_bitset(res0, res1);
    return (_mm_popcnt_u64(_mm_extract_epi64(res0, 0)) +
            _mm_popcnt_u64(_mm_extract_epi64(res1, 0)) +
            _mm_popcnt_u64(_mm_extract_epi64(res0, 1)) +
            _mm_popcnt_u64(_mm_extract_epi64(res1, 1)));
}

inline uint64_t BMat8::row_space_size_incl1() const {
    epu8 in = _mm_set_epi64x(_data, _data);
    epu8 block = epu8id;
    uint64_t res = 0;
    for (size_t r = 0; r < 16; r++) {
        epu8 orincl {};
        for (int i = 0; i < 8; i++) {
            orincl |= ((in | block) == block) & in;
            in = permuted(in, rotboth);
        }
        res += _mm_popcnt_u64(_mm_movemask_epi8(block == orincl));
        block += Epu8(16);
    }
    return res;
}

inline uint64_t BMat8::row_space_size_incl() const {
    epu8 in = _mm_set_epi64x(_data, _data);
    epu8 block = epu8id;
    uint64_t res = 0;
    for (size_t r = 0; r < 16; r++) {
        epu8 orincl = ((in | block) == block) & in;
        for (int i = 0; i < 7; i++) {    // Only rotating
            in = permuted(in, rotboth);
            orincl |= ((in | block) == block) & in;
        }
        res += _mm_popcnt_u64(_mm_movemask_epi8(block == orincl));
        block += Epu8(16);
    }
    return res;
}

inline bool BMat8::row_space_included_bitset(BMat8 other) const {
    epu8 this0, this1, other0, other1;
    this->row_space_bitset(this0, this1);
    other.row_space_bitset(other0, other1);
    return equal(this0 | other0, other0) && equal(this1 | other1, other1);
}

inline bool BMat8::row_space_included(BMat8 other) const {
    epu8 in = _mm_set_epi64x(0, other._data);
    epu8 block = _mm_set_epi64x(0, _data);
    epu8 orincl = ((in | block) == block) & in;
    for (int i = 0; i < 7; i++) {    // Only rotating
        in = permuted(in, rotlow);
        orincl |= ((in | block) == block) & in;
    }
    return equal(block, orincl);
}

inline epu8 BMat8::row_space_mask(epu8 block) const {
    epu8 in = _mm_set_epi64x(_data, _data);
    epu8 orincl = ((in | block) == block) & in;
    for (int i = 0; i < 7; i++) {    // Only rotating
        in = permuted(in, rotboth);
        orincl |= ((in | block) == block) & in;
    }
    return block == orincl;
}

inline std::pair<bool, bool> BMat8::row_space_included2(BMat8 a0, BMat8 b0,
                                                        BMat8 a1, BMat8 b1) {
    epu8 in = _mm_set_epi64x(b1._data, b0._data);
    epu8 block = _mm_set_epi64x(a1._data, a0._data);
    epu8 orincl = ((in | block) == block) & in;
    for (int i = 0; i < 7; i++) {    // Only rotating
        in = permuted(in, rotboth);
        orincl |= ((in | block) == block) & in;
    }
    epu8 res = (block == orincl);
    return std::make_pair(_mm_extract_epi64(res, 0) == -1,
                          _mm_extract_epi64(res, 1) == -1);
}

inline std::bitset<256> BMat8::row_space_bitset_ref() const {
    std::bitset<256>     lookup;
    std::vector<uint8_t> row_vec = row_space_basis().rows();
    auto                 last = std::remove(row_vec.begin(), row_vec.end(), 0);
    row_vec.erase(last, row_vec.end());
    for (uint8_t x : row_vec) {
        lookup.set(x);
    }
    lookup.set(0);
    std::vector<uint8_t> row_space(row_vec.begin(), row_vec.end());
    for (size_t i = 0; i < row_space.size(); ++i) {
        for (uint8_t row : row_vec) {
            uint8_t x = row_space[i] | row;
            if (!lookup[x]) {
                row_space.push_back(x);
                lookup.set(x);
            }
        }
    }
    return lookup;
}

inline bool BMat8::row_space_included_ref(BMat8 other) const {
    std::bitset<256> thisspace = row_space_bitset_ref();
    std::bitset<256> otherspace = other.row_space_bitset_ref();
    return (thisspace | otherspace) == otherspace;
}

inline uint64_t BMat8::row_space_size_ref() const {
    return row_space_bitset_ref().count();
}

inline std::vector<uint8_t> BMat8::rows() const {
    std::vector<uint8_t> rows;
    for (size_t i = 0; i < 8; ++i) {
        uint8_t row = static_cast<uint8_t>(_data << (8 * i) >> 56);
        rows.push_back(row);
    }
    return rows;
}

inline size_t BMat8::nr_rows() const {
    epu8 x = _mm_set_epi64x(_data, 0);
    return _mm_popcnt_u64(_mm_movemask_epi8(x != epu8 {}));
}

static HPCOMBI_CONSTEXPR epu8
  rev8 {7,6,5,4,3,2,1,0,8,9,10,11,12,13,14,15};
inline BMat8 BMat8::row_permuted(Perm16 p) const {
    epu8 x = _mm_set_epi64x(0, _data);
    x = permuted(x, rev8);
    x = permuted(x, p);
    x = permuted(x, rev8);
    return BMat8(_mm_extract_epi64(x, 0));
}
inline BMat8 BMat8::col_permuted(Perm16 p) const {
    return transpose().row_permuted(p).transpose();
}
inline BMat8 BMat8::row_permutation_matrix(Perm16 p) {
    return one().row_permuted(p);
}
inline BMat8 BMat8::col_permutation_matrix(Perm16 p) {
    return one().row_permuted(p).transpose();
}

inline Perm16 BMat8::right_perm_action_on_basis_ref(BMat8 bm) const {
    // LIBSEMIGROUPS_ASSERT(bm.row_space_basis() == bm);
    std::vector<uint8_t> rows = this->rows();
    BMat8                product   = *this * bm;
    std::vector<uint8_t> prod_rows = product.rows();

    // LIBSEMIGROUPS_ASSERT(product.row_space_basis() == bm);

    std::vector<uint8_t> perm(8);
    for (size_t i = 0; i < nr_rows(); ++i) {
        uint8_t row = rows[i];
        perm[i]
            = std::distance(prod_rows.begin(),
                            std::find(prod_rows.begin(), prod_rows.end(), row));
    }
    std::iota(perm.begin() + nr_rows(), perm.end(), nr_rows());

    Perm16 res = Perm16::one();
    for (size_t i=0; i < 8; i++)
        res[i] = perm[i];
    return res;
 }

inline Perm16 BMat8::right_perm_action_on_basis(BMat8 other) const {
    epu8 x = permuted(_mm_set_epi64x(_data, 0), epu8rev);
    epu8 y = permuted(_mm_set_epi64x((*this * other)._data, 0), epu8rev);
    // Vector ternary operator is not supported by clang.
    // return (x != (epu8 {})) ? permutation_of(y, x) : epu8id;
    return _mm_blendv_epi8(epu8id, permutation_of(y, x), x != epu8 {});
}



inline std::ostream &BMat8::write(std::ostream &os) const {
    uint64_t x = _data;
    uint64_t pow = 1;
    pow = pow << 63;
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            if (pow & x) {
                os << "1";
            } else {
                os << "0";
            }
            x = x << 1;
        }
        os << "\n";
    }
    return os;
}

}  // namespace HPCombi

namespace std {

inline std::ostream &operator<<(std::ostream &os, HPCombi::BMat8 const &bm) {
    return bm.write(os);
}

}  // namespace std
