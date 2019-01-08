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
#include <sparsehash/dense_hash_set>
#include <unordered_set>

#include <hpcombi.hpp>

#define USE_GOOGLE_SET

using namespace std;
using namespace HPCombi;


#include"idemp7.hpp"


vector<BMat8> make_gens(size_t n) {
    vector<BMat8> res;
    for (size_t i = 0; i<n-1; i++)
        for (size_t j = i+1; j<n; j++) {
            BMat8 m = BMat8::one();
            m.set(i,j,1);
            m.set(j,i,1);
            res.push_back(m);
        }
    return res;
}

vector<BMat8> gens = make_gens(N);

struct MyHash {
    inline size_t operator()(BMat8 const &m) const {
        size_t key = m.to_int();
        key ^= key >> 33;
        key *= 0xff51afd7ed558ccd;
        key ^= key >> 33;
        key *= 0xc4ceb9fe1a85ec53;
        key ^= key >> 33;
        return key;
    }
};

#ifdef  USE_GOOGLE_SET
using BMat8set = google::dense_hash_set<BMat8, MyHash, equal_to<BMat8>>;
#else
using BMat8set = unordered_set<BMat8>;
#endif

BMat8set ideal(pair<BMat8, vector<BMat8>> pr) {
    int lg = 0;
    BMat8set res;

#ifdef  USE_GOOGLE_SET
    res.set_empty_key(BMat8(0));
#endif

    res.insert(pr.first);
    std::vector<BMat8> todo, newtodo;
    todo.push_back(pr.first);
    while (todo.size()) {
         newtodo.clear();
        lg++;
        for (auto v : todo) {
            for (auto g : gens) {
                auto el = v * g;
                bool out = false;
                for (auto nidm : pr.second) {
                    if (nidm * el == el) {
                        out = true;
                        break;
                    }
                }
                if (not out and res.insert(el).second)
                    newtodo.push_back(el);
            }
        }
        std::swap(todo, newtodo);
        std::cout << lg << ", todo = " << todo.size() << ", res = " << res.size()
                  << ", #Bucks = " << res.bucket_count() << std::endl;
    }
    return res;
}

int main() {
    vector<int> result;
    for (auto pr : input) {
        cout << pr.first;
        result.push_back(ideal(pr).size());
    }
    for (auto i : result) {
        cout << i << ",";
    }
    cout << endl;
}
