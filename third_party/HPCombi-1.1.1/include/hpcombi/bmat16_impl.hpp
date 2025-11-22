//****************************************************************************//
//    Copyright (C) 2018-2024 Finn Smith <fls3@st-andrews.ac.uk>              //
//    Copyright (C) 2018-2024 James Mitchell <jdm3@st-andrews.ac.uk>          //
//    Copyright (C) 2018-2024 Florent Hivert <Florent.Hivert@lisn.fr>,        //
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

// This file contains an implementation of fast boolean matrices up to
// dimension 16 x 16.

// NOLINT(build/header_guard)

namespace HPCombi {
static_assert(std::is_trivial<BMat16>(), "BMat16 is not a trivial class!");

static constexpr xpu16 line{0x800, 0x901, 0xa02, 0xb03, 0xc04, 0xd05,
                            0xe06, 0xf07, 0x800, 0x901, 0xa02, 0xb03,
                            0xc04, 0xd05, 0xe06, 0xf07};
static constexpr xpu16 block{0x200, 0x604, 0xa08, 0xe0c, 0x301, 0x705,
                             0xb09, 0xf0d, 0x200, 0x604, 0xa08, 0xe0c,
                             0x301, 0x705, 0xb09, 0xf0d};

inline xpu64 to_line(xpu64 vect) {
    return simde_mm256_shuffle_epi8(vect, line);
}

inline xpu64 to_block(xpu64 vect) {
    return simde_mm256_shuffle_epi8(vect, block);
}

inline BMat16::BMat16(uint64_t n0, uint64_t n1, uint64_t n2,
                      uint64_t n3) noexcept {
    xpu64 tmp{n0, n1, n2, n3};
    _data = to_line(tmp);
}

inline BMat16::BMat16(std::vector<std::vector<bool>> const &mat) noexcept {
    HPCOMBI_ASSERT(mat.size() <= 16);
    HPCOMBI_ASSERT(0 < mat.size());
    std::array<uint64_t, 4> tmp = {0, 0, 0, 0};
    for (int i = mat.size() - 1; i >= 0; --i) {
        HPCOMBI_ASSERT(mat.size() == mat[i].size());
        tmp[i / 4] <<= 16 - mat.size();
        for (int j = mat[i].size() - 1; j >= 0; --j) {
            tmp[i / 4] = (tmp[i / 4] << 1) | mat[i][j];
        }
    }
    _data = xpu64{tmp[0], tmp[1], tmp[2], tmp[3]};
}

inline bool BMat16::operator()(size_t i, size_t j) const noexcept {
    return (_data[i / 4] >> (16 * (i % 4) + j)) & 1;
}

inline void BMat16::set(size_t i, size_t j, bool val) noexcept {
    HPCOMBI_ASSERT(i < 16);
    HPCOMBI_ASSERT(j < 16);
    uint64_t a = 1;
    a <<= 16 * (i % 4) + j;
    xpu64 mask{(i / 4 == 0) * a, (i / 4 == 1) * a, (i / 4 == 2) * a,
               (i / 4 == 3) * a};
    _data ^= (-val ^ _data) & mask;
}

inline bool BMat16::operator==(BMat16 const &that) const noexcept {
    xpu64 tmp = _data ^ that._data;
    return simde_mm256_testz_si256(tmp, tmp);
}

inline bool BMat16::operator<(BMat16 const &that) const noexcept {
    return _data[0] < that._data[0] ||
           (_data[0] == that._data[0] &&
            (_data[1] < that._data[1] ||
             (_data[1] == that._data[1] &&
              (_data[2] < that._data[2] ||
               (_data[2] == that._data[2] && (_data[3] < that._data[3]))))));
}

inline bool BMat16::operator>(BMat16 const &that) const noexcept {
    return _data[0] > that._data[0] ||
           (_data[0] == that._data[0] &&
            (_data[1] > that._data[1] ||
             (_data[1] == that._data[1] &&
              (_data[2] > that._data[2] ||
               (_data[2] == that._data[2] && (_data[3] > that._data[3]))))));
}

inline std::array<std::array<bool, 16>, 16> BMat16::to_array() const noexcept {
    xpu64 tmp = to_block(_data);
    uint64_t a = tmp[0], b = tmp[1], c = tmp[2], d = tmp[3];
    std::array<std::array<bool, 16>, 16> res;
    for (size_t i = 0; i < 64; ++i) {
        res[i / 8][i % 8] = a & 1;
        a >>= 1;
        res[i / 8][8 + i % 8] = b & 1;
        b >>= 1;
        res[8 + i / 8][i % 8] = c & 1;
        c >>= 1;
        res[8 + i / 8][8 + i % 8] = d & 1;
        d >>= 1;
    }
    return res;
}

inline BMat16 BMat16::transpose_naive() const noexcept {
    uint64_t a = 0, b = 0, c = 0, d = 0;
    for (int i = 7; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            a = (a << 1) | (*this)(j, i);
            b = (b << 1) | (*this)(j + 8, i);
            c = (c << 1) | (*this)(j, i + 8);
            d = (d << 1) | (*this)(j + 8, i + 8);
        }
    }
    return BMat16(a, b, c, d);
}

inline BMat16 BMat16::transpose() const noexcept {
    xpu64 tmp = to_block(_data);
    xpu64 x = simde_mm256_set_epi64x(tmp[3], tmp[1], tmp[2], tmp[0]);
    xpu64 y = (x ^ (x >> 7)) & (xpu64{0xAA00AA00AA00AA, 0xAA00AA00AA00AA,
                                      0xAA00AA00AA00AA, 0xAA00AA00AA00AA});
    x = x ^ y ^ (y << 7);
    y = (x ^ (x >> 14)) &
        (xpu64{0xCCCC0000CCCC, 0xCCCC0000CCCC, 0xCCCC0000CCCC, 0xCCCC0000CCCC});
    x = x ^ y ^ (y << 14);
    y = (x ^ (x >> 28)) &
        (xpu64{0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0, 0xF0F0F0F0});
    x = x ^ y ^ (y << 28);
    return BMat16(to_line(x));
}

static constexpr xpu16 rot{0x302, 0x504, 0x706, 0x908, 0xb0a, 0xd0c,
                           0xf0e, 0x100, 0x302, 0x504, 0x706, 0x908,
                           0xb0a, 0xd0c, 0xf0e, 0x100};

inline BMat16 BMat16::mult_transpose(BMat16 const &that) const noexcept {
    xpu16 x = _data;
    xpu16 y1 = that._data;
    xpu16 y2 = simde_mm256_set_epi64x(that._data[1], that._data[0],
                                      that._data[3], that._data[2]);
    xpu16 zero = simde_mm256_setzero_si256();
    xpu16 data = simde_mm256_setzero_si256();
    xpu16 diag1{0x1,   0x2,   0x4,   0x8,   0x10,   0x20,   0x40,   0x80,
                0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000};
    xpu16 diag2{0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
                0x1,   0x2,   0x4,   0x8,   0x10,   0x20,   0x40,   0x80};
    for (size_t i = 0; i < 8; ++i) {
        data |= ((x & y1) != zero) & diag1;
        data |= ((x & y2) != zero) & diag2;
        y1 = simde_mm256_shuffle_epi8(y1, rot);
        y2 = simde_mm256_shuffle_epi8(y2, rot);
        diag1 = simde_mm256_shuffle_epi8(diag1, rot);
        diag2 = simde_mm256_shuffle_epi8(diag2, rot);
    }
    return BMat16(data);
}

inline BMat16 BMat16::mult_4bmat8(BMat16 const &that) const noexcept {
    BMat16 tmp = that.transpose();
    xpu64 t1 = to_block(_data), t2 = to_block(tmp._data);
    BMat8 a1(t1[0]), b1(t1[1]), c1(t1[2]), d1(t1[3]), a2(t2[0]), b2(t2[1]),
        c2(t2[2]), d2(t2[3]);
    return BMat16((a1.mult_transpose(a2) | b1.mult_transpose(b2)).to_int(),
                  (a1.mult_transpose(c2) | b1.mult_transpose(d2)).to_int(),
                  (c1.mult_transpose(a2) | d1.mult_transpose(b2)).to_int(),
                  (c1.mult_transpose(c2) | d1.mult_transpose(d2)).to_int());
}

inline BMat16 BMat16::mult_naive(BMat16 const &that) const noexcept {
    uint64_t a = 0, b = 0, c = 0, d = 0;
    for (int i = 7; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            a <<= 1;
            b <<= 1;
            c <<= 1;
            d <<= 1;
            for (size_t k = 0; k < 8; ++k) {
                a |= ((*this)(i, k) && that(k, j)) ||
                     ((*this)(i, k + 8) && that(k + 8, j));
                b |= ((*this)(i, k) && that(k, j + 8)) ||
                     ((*this)(i, k + 8) && that(k + 8, j + 8));
                c |= ((*this)(i + 8, k) && that(k, j)) ||
                     ((*this)(i + 8, k + 8) && that(k + 8, j));
                d |= ((*this)(i + 8, k) && that(k, j + 8)) ||
                     ((*this)(i + 8, k + 8) && that(k + 8, j + 8));
            }
        }
    }
    return BMat16(a, b, c, d);
}

inline BMat16 BMat16::mult_naive_array(BMat16 const &that) const noexcept {
    std::array<std::array<bool, 16>, 16> tab1 = to_array(),
                                         tab2 = that.to_array();
    uint64_t a = 0, b = 0, c = 0, d = 0;
    for (int i = 7; i >= 0; --i) {
        for (int j = 7; j >= 0; --j) {
            a <<= 1;
            b <<= 1;
            c <<= 1;
            d <<= 1;
            for (size_t k = 0; k < 16; ++k) {
                a |= tab1[i][k] && tab2[k][j];
                b |= tab1[i][k] && tab2[k][j + 8];
                c |= tab1[i + 8][k] && tab2[k][j];
                d |= tab1[i + 8][k] && tab2[k][j + 8];
            }
        }
    }
    return BMat16(a, b, c, d);
}

inline size_t BMat16::nr_rows() const noexcept {
    size_t res = 0;
    for (size_t i = 0; i < 16; ++i)
        if ((_data[i / 4] << (16 * (i % 4)) >> 48) != 0)
            ++res;
    return res;

    //// Vectorized version which doesn't work due to the absence of popcnt in
    /// simde
    // xpu16 tmp = _data, zero = simde_mm256_setzero_si256();
    // xpu16 x = (tmp != zero);
    // return simde_mm256_popcnt_epi16(x);
}

inline std::vector<uint16_t> BMat16::rows() const {
    std::vector<uint16_t> rows;
    for (size_t i = 0; i < 16; ++i) {
        uint16_t row_rev = (_data[i / 4] << (16 * (3 - i % 4)) >> 48);

        // The row needs to be reversed
        uint16_t row = 0;
        for (size_t j = 0; j < 16; ++j) {
            row = (row << 1) | (row_rev & 1);
            row_rev >>= 1;
        }
        rows.push_back(row);
    }
    return rows;
}

inline BMat16 BMat16::random() {
    static std::random_device _rd;
    static std::mt19937 _gen(_rd());
    static std::uniform_int_distribution<uint64_t> _dist(0, 0xffffffffffffffff);

    return BMat16(_dist(_gen), _dist(_gen), _dist(_gen), _dist(_gen));
}

static const constexpr std::array<xpu64, 16> ROW_MASK16 = {
    static_cast<xpu64>(
        xpu16{0xffff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0xffff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0xffff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0xffff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0xffff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0xffff, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0xffff, 0, 0, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0xffff, 0, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0, 0xffff, 0, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffff, 0, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffff, 0, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffff, 0, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffff, 0, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffff, 0, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffff, 0}),
    static_cast<xpu64>(
        xpu16{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xffff})};

static const constexpr std::array<xpu64, 16> COL_MASK16 = {
    static_cast<xpu64>(xpu16{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}),
    static_cast<xpu64>(xpu16{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}),
    static_cast<xpu64>(xpu16{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}),
    static_cast<xpu64>(xpu16{8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}),
    static_cast<xpu64>(xpu16{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
                             0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10}),
    static_cast<xpu64>(xpu16{0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
                             0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}),
    static_cast<xpu64>(xpu16{0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
                             0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40}),
    static_cast<xpu64>(xpu16{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                             0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}),
    static_cast<xpu64>(xpu16{0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100,
                             0x100, 0x100, 0x100, 0x100, 0x100, 0x100, 0x100,
                             0x100, 0x100}),
    static_cast<xpu64>(xpu16{0x200, 0x200, 0x200, 0x200, 0x200, 0x200, 0x200,
                             0x200, 0x200, 0x200, 0x200, 0x200, 0x200, 0x200,
                             0x200, 0x200}),
    static_cast<xpu64>(xpu16{0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400,
                             0x400, 0x400, 0x400, 0x400, 0x400, 0x400, 0x400,
                             0x400, 0x400}),
    static_cast<xpu64>(xpu16{0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800,
                             0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800,
                             0x800, 0x800}),
    static_cast<xpu64>(xpu16{0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
                             0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
                             0x1000, 0x1000, 0x1000, 0x1000}),
    static_cast<xpu64>(xpu16{0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000,
                             0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000,
                             0x2000, 0x2000, 0x2000, 0x2000}),
    static_cast<xpu64>(xpu16{0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
                             0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
                             0x4000, 0x4000, 0x4000, 0x4000}),
    static_cast<xpu64>(xpu16{0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
                             0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
                             0x8000, 0x8000, 0x8000, 0x8000})};

inline BMat16 BMat16::random(size_t const dim) {
    // TO DO : Instead of nulling all the cols/rows one by one, one could do
    // that at once with the proper mask
    HPCOMBI_ASSERT(0 < dim && dim <= 16);
    BMat16 bm = BMat16::random();
    for (size_t i = dim; i < 16; ++i) {
        bm._data &= ~ROW_MASK16[i];
        bm._data &= ~COL_MASK16[i];
    }
    return bm;
}

inline std::ostream &BMat16::write(std::ostream &os) const {
    for (size_t i = 0; i < 16; ++i) {
        for (size_t j = 0; j < 16; ++j) {
            os << (*this)(i, j);
        }
        os << "\n";
    }
    return os;
}

}  // namespace HPCombi

namespace std {

// Not noexcept because BMat16::write isn't
inline std::ostream &operator<<(std::ostream &os, HPCombi::BMat16 const &bm) {
    return bm.write(os);
}

}  // namespace std
