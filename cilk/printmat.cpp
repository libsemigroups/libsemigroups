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
#include <hpcombi.hpp>

using namespace std;
using namespace HPCombi;

int main(int argc, char *argv[]) {
    while (1) {
        uint64_t in;
        cin >> in;
        cout << BMat8(in) << endl;
    }
    return 0;
}
