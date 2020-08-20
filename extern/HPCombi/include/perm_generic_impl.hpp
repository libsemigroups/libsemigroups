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

namespace HPCombi {

template <size_t _Size, typename Expo>
PermGeneric<_Size, Expo>::PermGeneric(std::initializer_list<Expo> il) {
    assert(il.size() <= _Size);
    std::copy(il.begin(), il.end(), this->v.begin());
    for (Expo i = il.size(); i < _Size; i++)
        this->v[i] = i;
}

template <size_t _Size, typename Expo>
PermGeneric<_Size, Expo>
PermGeneric<_Size, Expo>::elementary_transposition(uint64_t i) {
    assert(i < _Size);
    PermGeneric res{{}};
    res[i] = i + 1;
    res[i + 1] = i;
    return res;
}

template <size_t _Size, typename Expo>
PermGeneric<_Size, Expo>
PermGeneric<_Size, Expo>::inverse() const {
    PermGeneric res;
    for (uint64_t i = 0; i < _Size; i++) res[this->v[i]] = i;
    return res;
}

template <size_t _Size, typename Expo>
PermGeneric<_Size, Expo>
PermGeneric<_Size, Expo>::random() {
    PermGeneric res{{}};
    std::random_shuffle(res.v.begin(), res.v.end());
    return res;
}

template <size_t _Size, typename Expo>
typename PermGeneric<_Size, Expo>::vect
PermGeneric<_Size, Expo>::lehmer() const {
    vect res{};
    for (size_t i = 0; i < _Size; i++)
        for (size_t j = i + 1; j < _Size; j++)
            if (this->v[i] > this->v[j])
                res[i]++;
    return res;
}

template <size_t _Size, typename Expo>
uint64_t PermGeneric<_Size, Expo>::length() const {
    uint64_t res = 0;
    for (size_t i = 0; i < _Size; i++)
        for (size_t j = i + 1; j < _Size; j++)
            if (this->v[i] > this->v[j])
                res++;
    return res;
}

template <size_t _Size, typename Expo>
uint64_t PermGeneric<_Size, Expo>::nb_descents() const {
    uint64_t res = 0;
    for (size_t i = 0; i < _Size - 1; i++)
        if (this->v[i] > this->v[i + 1])
            res++;
    return res;
}

template <size_t _Size, typename Expo>
uint64_t PermGeneric<_Size, Expo>::nb_cycles() const {
    std::array<bool, _Size> b{};
    uint64_t c = 0;
    for (size_t i = 0; i < _Size; i++) {
        if (not b[i]) {
            for (size_t j = i; not b[j]; j = this->v[j])
                b[j] = true;
            c++;
        }
    }
    return c;
}

template <size_t _Size, typename Expo>
bool PermGeneric<_Size, Expo>::left_weak_leq(PermGeneric other) const {
    for (size_t i = 0; i < _Size; i++) {
        for (size_t j = i + 1; j < _Size; j++) {
            if ((this->v[i] > this->v[j]) && (other[i] < other[j]))
                return false;
        }
    }
    return true;
}

};  //  namespace HPCombi

namespace std {

template <size_t _Size, typename Expo>
struct hash<HPCombi::PermGeneric<_Size, Expo>> {
    size_t operator()(const HPCombi::PermGeneric<_Size, Expo> &ar) const {
        return hash<HPCombi::VectGeneric<_Size, Expo>>()(ar);
    }
};

}  // namespace std

