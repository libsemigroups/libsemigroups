#include <sstream>

std::string ver_string(std::string a, int b, int c, int d) {
    std::ostringstream ss;
    ss << a << '-' << b << '.' << c << '.' << d;
    return ss.str();
}
std::string ver_string(std::string a, int b) {
    std::ostringstream ss;
    ss << a << '-' << b;
    return ss.str();
}

std::string CXX_VER =
#if defined(__clang__)
    ver_string("clang", __clang_major__, __clang_minor__, __clang_patchlevel__);

#elif defined(__ICC) || defined(__INTEL_COMPILER)
    ver_string("icc", __INTEL_COMPILER);

#elif defined(__GNUC__) || defined(__GNUG__)
    ver_string("g++", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);

#elif defined(__HP_cc) || defined(__HP_aCC)
    ver_string("hp", __HP_cc);

#elif defined(__IBMC__) || defined(__IBMCPP__)
    ver_string("xl", __IBMCPP__);

#elif defined(_MSC_VER)
    ver_string("mvs", _MSC_FULL_VER);

#elif defined(__PGI)
    ver_string("pgi", __PGIC__, __PGIC_MINOR, __PGIC_PATCHLEVEL__);

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
    ver_string("sun", __SUNPRO_CC);

#endif
