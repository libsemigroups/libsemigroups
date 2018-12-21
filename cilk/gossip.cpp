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
#include <unordered_set>

#include <hpcombi.hpp>

using namespace std;
using namespace HPCombi;

const int N = 8;

vector<pair<BMat8, vector<BMat8>>> input = {
    #include"from_sage"
};

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

unordered_set<BMat8> ideal(pair<BMat8, vector<BMat8>> pr) {
    int lg = 0;
    unordered_set<BMat8> res;

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
        result.push_back(ideal(pr).size());
    }
    for (auto i : result) {
        cout << i << ",";
    }
    cout << endl;
}
