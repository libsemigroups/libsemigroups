//****************************************************************************//
//     Copyright (C) 2014-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
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

#include <iomanip>
#include <sstream>
#include <tuple>
#include <vector>

#include "hpcombi/perm16.hpp"
#include "hpcombi/perm_generic.hpp"

#include "test_main.hpp"
#include <catch2/catch_template_test_macros.hpp>

namespace HPCombi {
namespace {

template <class Perm_> struct Fixture1 {
    using VectType = typename Perm_::vect;
    using PermType = Perm_;

    Fixture1()
        : zero({0}), V01({0, 1}), V10({1, 0}), V11({1, 1}), V1({}, 1),
          PPa({1, 2, 3, 4, 0, 5}), PPb({1, 2, 3, 6, 0, 5}), czero(zero),
          cV01(V01), RandPerm({3, 1, 0, 5, 10, 2, 6, 7, 4, 8, 9}),
          Plist({PPa, PPb, RandPerm}),
          Vlist({zero, V01, V10, V11, V1, PPa, PPb, RandPerm}) {}
    ~Fixture1() {}

    VectType zero, V01, V10, V11, V1;
    PermType PPa, PPb;
    const VectType czero, cV01;
    const PermType RandPerm;
    const std::vector<PermType> Plist;
    const std::vector<VectType> Vlist;

    // some tests assume that the size is at least 6
    static_assert(VectType::size() >= 6, "Minimum size for tests");
};

}  // namespace

// Better than std::tuple because we can see the actual types in the output
// with a macro but not with the tuple.
#define PermTypes                                                              \
    Perm16, PermGeneric<12>, PermGeneric<16>, PermGeneric<32>,                 \
        PermGeneric<42>, PermGeneric<49>, (PermGeneric<350, uint32_t>)

TEMPLATE_TEST_CASE_METHOD(Fixture1, "sizeof", "[PermAll][000]", PermTypes) {
    CHECK(sizeof(Fixture1<TestType>::zero) ==
          Fixture1<TestType>::VectType::size() *
              sizeof(Fixture1<TestType>::zero[0]));
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "operator==", "[PermAll][001]", PermTypes) {
    CHECK(Fixture1<TestType>::zero == Fixture1<TestType>::zero);
    CHECK(Fixture1<TestType>::zero != Fixture1<TestType>::V01);
    for (unsigned i = 0; i < Fixture1<TestType>::Plist.size(); i++) {
        for (unsigned j = 0; j < Fixture1<TestType>::Plist.size(); j++) {
            if (i == j) {
                CHECK(Fixture1<TestType>::Plist[i] ==
                      Fixture1<TestType>::Plist[j]);
            } else {
                CHECK(Fixture1<TestType>::Plist[i] !=
                      Fixture1<TestType>::Plist[j]);
            }
        }
    }
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "operator[] const", "[PermAll][002]",
                          PermTypes) {
    CHECK(Fixture1<TestType>::czero[0] == 0u);
    CHECK(Fixture1<TestType>::czero[1] == 0u);
    if (Fixture1<TestType>::czero.size() > 12) {
        CHECK(Fixture1<TestType>::czero[12] == 0u);
    }
    if (Fixture1<TestType>::czero.size() > 15) {
        CHECK(Fixture1<TestType>::czero[15] == 0u);
    }
    CHECK(Fixture1<TestType>::cV01[0] == 0u);
    CHECK(Fixture1<TestType>::cV01[1] == 1u);
    CHECK(Fixture1<TestType>::cV01[2] == 0u);
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "operator[]", "[PermAll][003]", PermTypes) {
    CHECK(Fixture1<TestType>::zero[0] == 0u);
    CHECK(Fixture1<TestType>::zero[1] == 0u);
    if (Fixture1<TestType>::czero.size() > 12) {
        CHECK(Fixture1<TestType>::zero[12] == 0u);
    }
    if (Fixture1<TestType>::czero.size() > 15) {
        CHECK(Fixture1<TestType>::zero[15] == 0u);
    }
    CHECK(Fixture1<TestType>::V01[0] == 0u);
    CHECK(Fixture1<TestType>::V01[1] == 1u);
    CHECK(Fixture1<TestType>::V01[2] == 0u);
    CHECK(Fixture1<TestType>::PPa[4] == 0u);
    CHECK(Fixture1<TestType>::PPa[5] == 5u);
    Fixture1<TestType>::zero[0] = 3;
    CHECK(Fixture1<TestType>::zero[0] == 3u);
    CHECK(Fixture1<TestType>::zero[1] == 0u);
    if (Fixture1<TestType>::czero.size() > 12) {
        CHECK(Fixture1<TestType>::zero[12] == 0u);
    }
    if (Fixture1<TestType>::czero.size() > 15) {
        CHECK(Fixture1<TestType>::zero[15] == 0u);
    }
    Fixture1<TestType>::PPa[2] = 0;
    CHECK(Fixture1<TestType>::PPa[1] == 2u);
    CHECK(Fixture1<TestType>::PPa[2] == 0u);
    CHECK(Fixture1<TestType>::PPa[3] == 4u);
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "operator<", "[AllPerm][004]", PermTypes) {
    for (unsigned i = 0; i < Fixture1<TestType>::Plist.size(); i++) {
        for (unsigned j = 0; j < Fixture1<TestType>::Plist.size(); j++) {
            if (i < j) {
                CHECK(Fixture1<TestType>::Plist[i] <
                      Fixture1<TestType>::Plist[j]);
            } else {
                CHECK(!(Fixture1<TestType>::Plist[i] <
                        Fixture1<TestType>::Plist[j]));
            }
        }
    }
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "less_partial", "[AllPerm][005]",
                          PermTypes) {
    for (auto p : Fixture1<TestType>::Plist) {
        for (unsigned k = 0; k < Fixture1<TestType>::PermType::size(); k++) {
            CHECK(p.less_partial(p, k) == 0);
        }
    }
    for (auto p : Fixture1<TestType>::Plist) {
        for (auto q : Fixture1<TestType>::Plist) {
            CHECK(p.less_partial(q, 0) == 0);
        }
    }

    CHECK(Fixture1<TestType>::zero.less_partial(Fixture1<TestType>::V01, 1) ==
          0);
    CHECK(Fixture1<TestType>::V01.less_partial(Fixture1<TestType>::zero, 1) ==
          0);
    CHECK(Fixture1<TestType>::zero.less_partial(Fixture1<TestType>::V01, 2) <
          0);
    CHECK(Fixture1<TestType>::V01.less_partial(Fixture1<TestType>::zero, 2) >
          0);

    CHECK(Fixture1<TestType>::zero.less_partial(Fixture1<TestType>::V10, 1) <
          0);
    CHECK(Fixture1<TestType>::zero.less_partial(Fixture1<TestType>::V10, 2) <
          0);
    CHECK(Fixture1<TestType>::V10.less_partial(Fixture1<TestType>::zero, 1) >
          0);
    CHECK(Fixture1<TestType>::V10.less_partial(Fixture1<TestType>::zero, 2) >
          0);

    CHECK(Fixture1<TestType>::PPa.less_partial(Fixture1<TestType>::PPb, 1) ==
          0);
    CHECK(Fixture1<TestType>::PPa.less_partial(Fixture1<TestType>::PPb, 2) ==
          0);
    CHECK(Fixture1<TestType>::PPa.less_partial(Fixture1<TestType>::PPb, 3) ==
          0);
    CHECK(Fixture1<TestType>::PPa.less_partial(Fixture1<TestType>::PPb, 4) < 0);
    CHECK(Fixture1<TestType>::PPa.less_partial(Fixture1<TestType>::PPb, 5) < 0);
    CHECK(Fixture1<TestType>::PPb.less_partial(Fixture1<TestType>::PPa, 4) > 0);
    CHECK(Fixture1<TestType>::PPb.less_partial(Fixture1<TestType>::PPa, 5) > 0);
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "first_zero", "[AllPerm][006]", PermTypes) {
    CHECK(Fixture1<TestType>::zero.first_zero() == 0u);
    CHECK(Fixture1<TestType>::V01.first_zero() == 0u);
    CHECK(Fixture1<TestType>::PPa.first_zero() == 4u);
    CHECK(Fixture1<TestType>::V10.first_zero() == 1u);
    CHECK(Fixture1<TestType>::V1.first_zero() ==
          Fixture1<TestType>::VectType::size());
    CHECK(Fixture1<TestType>::V10.first_zero(1) ==
          Fixture1<TestType>::VectType::size());
    CHECK(Fixture1<TestType>::PPa.first_zero(5) == 4u);
    CHECK(Fixture1<TestType>::PPa.first_zero(3) ==
          Fixture1<TestType>::VectType::size());
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "last_zero", "[AllPerm][007]", PermTypes) {
    CHECK(Fixture1<TestType>::zero.last_zero() ==
          Fixture1<TestType>::VectType::size() - 1);
    CHECK(Fixture1<TestType>::V01.last_zero() ==
          Fixture1<TestType>::VectType::size() - 1);
    CHECK(Fixture1<TestType>::PPa.last_zero() == 4u);
    CHECK(Fixture1<TestType>::V1.last_zero() ==
          Fixture1<TestType>::VectType::size());
    CHECK(Fixture1<TestType>::V01.last_zero(1) == 0u);
    CHECK(Fixture1<TestType>::V10.last_zero(1) ==
          Fixture1<TestType>::VectType::size());
    CHECK(Fixture1<TestType>::PPa.last_zero(5) == 4u);
    CHECK(Fixture1<TestType>::PPa.last_zero(3) ==
          Fixture1<TestType>::VectType::size());
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "first_non_zero", "[AllPerm][008]",
                          PermTypes) {
    CHECK(Fixture1<TestType>::zero.first_non_zero() ==
          Fixture1<TestType>::VectType::size());
    CHECK(Fixture1<TestType>::V01.first_non_zero() == 1u);
    CHECK(Fixture1<TestType>::PPa.first_non_zero() == 0u);
    CHECK(Fixture1<TestType>::V01.first_non_zero() == 1u);
    CHECK(Fixture1<TestType>::V01.first_non_zero(1) ==
          Fixture1<TestType>::VectType::size());
    CHECK(Fixture1<TestType>::PPa.first_non_zero(5) == 0u);
    CHECK(Fixture1<TestType>::PPa.first_non_zero(3) == 0u);
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "last_non_zero", "[AllPerm][009]",
                          PermTypes) {
    CHECK(Fixture1<TestType>::zero.last_non_zero() ==
          Fixture1<TestType>::VectType::size());
    CHECK(Fixture1<TestType>::V01.last_non_zero() == 1u);
    CHECK(Fixture1<TestType>::PPa.last_non_zero() ==
          Fixture1<TestType>::VectType::size() - 1);
    CHECK(Fixture1<TestType>::V01.last_non_zero() == 1u);
    CHECK(Fixture1<TestType>::V01.last_non_zero(1) ==
          Fixture1<TestType>::VectType::size());
    CHECK(Fixture1<TestType>::PPa.last_non_zero(5) == 3u);
    CHECK(Fixture1<TestType>::PPa.last_non_zero(3) == 2u);
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "permuted", "[AllPerm][010]", PermTypes) {
    CHECK(Fixture1<TestType>::zero.permuted(Fixture1<TestType>::zero) ==
          Fixture1<TestType>::zero);
    CHECK(Fixture1<TestType>::V01.permuted(Fixture1<TestType>::V01) ==
          Fixture1<TestType>::V01);
    CHECK(Fixture1<TestType>::V10.permuted(Fixture1<TestType>::V10) ==
          typename Fixture1<TestType>::VectType({0, 1}, 1));
    CHECK(Fixture1<TestType>::V10.permuted(Fixture1<TestType>::V01) ==
          typename Fixture1<TestType>::VectType({1, 0}, 1));
    CHECK(Fixture1<TestType>::V01.permuted(Fixture1<TestType>::V10) ==
          Fixture1<TestType>::V10);
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "operator_insert", "[AllPerm][011]",
                          PermTypes) {
    std::ostringstream out, out2;
    out << Fixture1<TestType>::zero;
    out2 << "{ 0";
    for (size_t i = 1; i < Fixture1<TestType>::VectType::size(); i++)
        out2 << ", 0";
    out2 << "}";
    CHECK(out.str() == out2.str());

    out.str("");
    out2.str("");
    out << Fixture1<TestType>::V01;
    out2 << "{ 0, 1";
    for (size_t i = 2; i < Fixture1<TestType>::VectType::size(); i++)
        out2 << ", 0";
    out2 << "}";
    CHECK(out.str() == out2.str());

    out.str("");
    out2.str("");
    out << Fixture1<TestType>::PPa;
    out2 << "{ 1, 2, 3, 4, 0";
    for (size_t i = 5; i < Fixture1<TestType>::VectType::size(); i++)
        out2 << "," << std::setw(2) << i;
    out2 << "}";
    CHECK(out.str() == out2.str());
}

TEMPLATE_TEST_CASE_METHOD(Fixture1, "is_permutation", "[AllPerm][012]",
                          PermTypes) {
    CHECK(!Fixture1<TestType>::zero.is_permutation());
    CHECK(Fixture1<TestType>::PPa.is_permutation());
    CHECK(!Fixture1<TestType>::PPb.is_permutation());
    CHECK(Fixture1<TestType>::RandPerm.is_permutation());
    CHECK(!typename Fixture1<TestType>::VectType(
               {3, 1, 0, 9, 3, 10, 2, 11, 6, 7, 4, 8})
               .is_permutation());
    CHECK(Fixture1<TestType>::PPa.is_permutation(16));
    CHECK(!Fixture1<TestType>::RandPerm.is_permutation(4));
    CHECK(Fixture1<TestType>::PPa.is_permutation(5));
    CHECK(!Fixture1<TestType>::PPa.is_permutation(4));
}

template <class Perm_> struct Fixture2 {
    using PermType = Perm_;
    Fixture2()
        : id(PermType::one()), RandPerm({3, 1, 0, 5, 10, 2, 11, 6, 7, 4, 8, 9}),
          Plist({id, RandPerm}) {
        for (uint64_t i = 0; i < std::min<size_t>(PermType::size(), 30) - 1;
             i++) {
            Plist.push_back(PermType::elementary_transposition(i));
        }
        for (uint64_t i = std::max<size_t>(30, PermType::size() - 20);
             i < PermType::size() - 1; i++) {
            Plist.push_back(PermType::elementary_transposition(i));
        }
        for (uint64_t i = 0; i < 10; i++) {
            Plist.push_back(PermType::random());
        }
    }

    ~Fixture2() = default;

    PermType id, s1, s2, s3;
    const PermType RandPerm;
    std::vector<PermType> Plist;
};

TEMPLATE_TEST_CASE_METHOD(Fixture2,
                          "is_permutation for freshly constructed perms",
                          "[AllPerm][013]", PermTypes) {
    using Perm = typename Fixture2<TestType>::PermType;
    for (auto x : Fixture2<TestType>::Plist) {
        CHECK(Perm(x).is_permutation());
    }
    CHECK(Perm({}).is_permutation());
    CHECK(Perm({1, 0}).is_permutation());
    CHECK(Perm({1, 2, 0}).is_permutation());
    CHECK(!Perm({1, 2}).is_permutation());
}

TEMPLATE_TEST_CASE_METHOD(Fixture2, "std::hash", "[AllPerm][014]", PermTypes) {
    for (auto x : Fixture2<TestType>::Plist) {
        CHECK(std::hash<typename Fixture2<TestType>::PermType>()(x) != 0);
    }
}

TEMPLATE_TEST_CASE_METHOD(Fixture2, "mult_coxeter", "[AllPerm][015]",
                          PermTypes) {
    const size_t n = TestType::size() - 1;

    for (uint64_t i = 0; i < n; i++) {
        auto si = TestType::elementary_transposition(i);
        CHECK(si != Fixture2<TestType>::id);
        CHECK(si * si == Fixture2<TestType>::id);
        if (i + 1 < n) {
            auto si1 = TestType::elementary_transposition(i + 1);
            CHECK(si * si1 * si == si1 * si * si1);
        }
        for (uint64_t j = i + 2; j < n; j++) {
            auto sj = TestType::elementary_transposition(j);
            CHECK(sj * si == si * sj);
        }
    }
}

TEMPLATE_TEST_CASE_METHOD(Fixture2, "mult", "[AllPerm][016]", PermTypes) {
    for (auto x : Fixture2<TestType>::Plist) {
        CHECK(Fixture2<TestType>::id * x == x);
        CHECK(x * Fixture2<TestType>::id == x);
    }
    CHECK(Fixture2<TestType>::RandPerm * Fixture2<TestType>::RandPerm ==
          TestType({5, 1, 3, 2, 8, 0, 9, 11, 6, 10, 7, 4}));

    for (auto x : Fixture2<TestType>::Plist) {
        for (auto y : Fixture2<TestType>::Plist) {
            for (auto z : Fixture2<TestType>::Plist) {
                CHECK((x * y) * z == x * (y * z));
            }
        }
    }
}

TEMPLATE_TEST_CASE_METHOD(Fixture2, "inverse", "[AllPerm][017]", PermTypes) {
    for (auto x : Fixture2<TestType>::Plist) {
        CHECK(x.inverse() * x == Fixture2<TestType>::id);
        CHECK(x * x.inverse() == Fixture2<TestType>::id);
        CHECK(x.inverse().inverse() == x);
    }
}

TEMPLATE_TEST_CASE_METHOD(Fixture2, "random", "[AllPerm][018]", PermTypes) {
    for (int i = 0; i < 100; i++) {
        CHECK(Fixture2<TestType>::PermType::random().is_permutation());
    }
}

}  // namespace HPCombi
