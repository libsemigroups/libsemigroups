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

#include"idemp6.hpp"


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

int ideal(pair<BMat8, vector<BMat8>> pr) {
    int lg = 0, res = 0;
    BMat8set todo, newtodo;

#ifdef  USE_GOOGLE_SET
    todo.set_empty_key(BMat8(0));
    newtodo.set_empty_key(BMat8(0));
#endif

    todo.insert(pr.first);
    while (todo.size()) {
        res += todo.size();
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
                if (not out and todo.find(el) == todo.end())
                    newtodo.insert(el);
            }
        }
        std::cout << lg
                  << ", todo = " << todo.size()
                  << ", newtodo = " << newtodo.size()
                  << ", res = " << res
                  << ", #Bucks = " << todo.bucket_count() << std::endl;
        if (lg > 20) return 0;
        std::swap(todo, newtodo);
    }
    return res;
}

int main() {
    vector<int> result;
    for (auto pr : input) {
        cout << pr.first;
        result.push_back(ideal(pr));
    }
    for (auto i : result) {
        cout << i << ",";
    }
    cout << endl;
}
