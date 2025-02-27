////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////

// NOLINT(build/header_guard)

namespace HPCombi {

template <size_t Size, typename Expo>
PermGeneric<Size, Expo>::PermGeneric(std::initializer_list<Expo> il) {
    HPCOMBI_ASSERT(il.size() <= Size);
    std::copy(il.begin(), il.end(), this->v.begin());
    for (Expo i = il.size(); i < Size; i++)
        this->v[i] = i;
}

template <size_t Size, typename Expo>
PermGeneric<Size, Expo>
PermGeneric<Size, Expo>::elementary_transposition(uint64_t i) {
    HPCOMBI_ASSERT(i < Size);
    PermGeneric res{{}};
    res[i] = i + 1;
    res[i + 1] = i;
    return res;
}

template <size_t Size, typename Expo>
PermGeneric<Size, Expo> PermGeneric<Size, Expo>::inverse() const {
    PermGeneric res;
    for (uint64_t i = 0; i < Size; i++)
        res[this->v[i]] = i;
    return res;
}

template <size_t Size, typename Expo>
PermGeneric<Size, Expo> PermGeneric<Size, Expo>::random() {
    static std::random_device rd;
    static std::mt19937 g(rd());

    PermGeneric res{{}};
    std::shuffle(res.v.begin(), res.v.end(), g);
    return res;
}

template <size_t Size, typename Expo>
typename PermGeneric<Size, Expo>::vect PermGeneric<Size, Expo>::lehmer() const {
    vect res{};
    for (size_t i = 0; i < Size; i++)
        for (size_t j = i + 1; j < Size; j++)
            if (this->v[i] > this->v[j])
                res[i]++;
    return res;
}

template <size_t Size, typename Expo>
uint64_t PermGeneric<Size, Expo>::length() const {
    uint64_t res = 0;
    for (size_t i = 0; i < Size; i++)
        for (size_t j = i + 1; j < Size; j++)
            if (this->v[i] > this->v[j])
                res++;
    return res;
}

template <size_t Size, typename Expo>
uint64_t PermGeneric<Size, Expo>::nb_descents() const {
    uint64_t res = 0;
    for (size_t i = 0; i < Size - 1; i++)
        if (this->v[i] > this->v[i + 1])
            res++;
    return res;
}

template <size_t Size, typename Expo>
uint64_t PermGeneric<Size, Expo>::nb_cycles() const {
    std::array<bool, Size> b{};
    uint64_t c = 0;
    for (size_t i = 0; i < Size; i++) {
        if (!b[i]) {
            for (size_t j = i; !b[j]; j = this->v[j])
                b[j] = true;
            c++;
        }
    }
    return c;
}

template <size_t Size, typename Expo>
bool PermGeneric<Size, Expo>::left_weak_leq(PermGeneric other) const {
    for (size_t i = 0; i < Size; i++) {
        for (size_t j = i + 1; j < Size; j++) {
            if ((this->v[i] > this->v[j]) && (other[i] < other[j]))
                return false;
        }
    }
    return true;
}

};  //  namespace HPCombi

namespace std {

template <size_t Size, typename Expo>
struct hash<HPCombi::PermGeneric<Size, Expo>> {
    size_t operator()(const HPCombi::PermGeneric<Size, Expo> &ar) const {
        return hash<HPCombi::VectGeneric<Size, Expo>>()(ar);
    }
};

}  // namespace std
