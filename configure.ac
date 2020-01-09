dnl Process this file with autoconf to produce a configure script.

dnl Autoconf and automake initialization.

AC_PREREQ([2.69])
AC_INIT([libsemigroups],m4_esyscmd([tr -d '\n' < .VERSION]),[jdm3@st-andrews.ac.uk])
AC_CONFIG_SRCDIR(include/libsemigroups.hpp)
AC_CONFIG_HEADERS(config/config.h)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)

dnl Set default CFLAGS/CXXFLAGS to nothing, to avoid passing -g -O2 as
dnl the default C/CXXFLAGS when compiling in, e.g. debug mode
: ${CFLAGS=""}
: ${CXXFLAGS=""}

AM_INIT_AUTOMAKE([1.11 -Wall foreign subdir-objects])
AM_SILENT_RULES([yes])
AM_PROG_AR

AX_PREFIX_CONFIG_H([include/libsemigroups-config.hpp],[],[config/config.h])

AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11(,[mandatory])

dnl ## Check for pthread, this seems to be required to compile with GCC
AX_PTHREAD(,[AC_MSG_ERROR([pthread is required])])
AC_CHECK_LIB([pthread], [pthread_create])

AM_MAINTAINER_MODE

AC_SUBST(ac_aux_dir)

dnl Checks for programs.

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdint.h stdlib.h sys/time.h unistd.h pthread.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T
AC_CHECK_TYPES([ptrdiff_t])
AC_CHECK_SIZEOF([void *])

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset pow sqrt])

# Check if debug mode is enabled
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug], [enable debug mode])],
    [AC_DEFINE([DEBUG], [1], [define if building in debug mode])],
    [enable_debug=no]
    )
AC_MSG_CHECKING([whether to enable debug mode])
AC_MSG_RESULT([$enable_debug])

AM_CONDITIONAL([LIBSEMIGROUPS_DEBUG], [test "x$enable_debug" = xyes])

AC_ARG_ENABLE([verbose],
    [AS_HELP_STRING([--enable-verbose], [enable verbose mode])],
    [AC_DEFINE([VERBOSE], [1], [define if building in verbose mode])],
    [enable_verbose=no]
    )
AC_MSG_CHECKING([whether to enable verbose mode])
AC_MSG_RESULT([$enable_verbose])

# Check if we should use google's dense_hash_map
# AC_ARG_ENABLE([densehashmap],
#     [AS_HELP_STRING([--enable-densehashmap], 
#                     [enable google's dense_hash_map])],
#     [enable_densehashmap=yes],
#     [enable_densehashmap=no])
# 
# AC_MSG_CHECKING([whether to use google's dense_hash_map])
# AC_MSG_RESULT([$enable_densehashmap])
# 
# AS_IF([test "x$enable_densehashmap" = xyes],
#       [AC_CHECK_FILE([extern/sparsehash-c11/sparsehash/dense_hash_map], 
#                      [], 
#                      [AC_MSG_WARN([extern/sparsehash-c11 is not available, see https://github.com/sparsehash/sparsehash-c11])
#                      [enable_densehashmap=no]])])
# 
# AS_IF([test "x$enable_densehashmap" = xyes], 
#        [AC_DEFINE([DENSEHASHMAP], 
#                   [1], 
#                   [define if google's dense_hash_map is available])])
# 
# AM_CONDITIONAL([LIBSEMIGROUPS_DENSEHASHMAP], 
#                [test "x$enable_densehashmap" = xyes])

AC_ARG_ENABLE([compile-warnings],
    [AS_HELP_STRING([--enable-compile-warnings], [enable compiler warnings])],
    [enable_compile_warnings=yes],
    [enable_compile_warnings=no])
AC_MSG_CHECKING([whether enable compiler warnings])
AC_MSG_RESULT([$enable_compile_warnings])

AS_IF([test "x$enable_compile_warnings" != "xno"],
  [ax_enable_compile_warnings=yes
   AX_COMPILER_FLAGS_CXXFLAGS([WARNING_CXXFLAGS], [], [ dnl
       -pedantic dnl
       -Wold-style-cast])
   AC_SUBST([WARNING_CXXFLAGS])])


# Check if code coverage mode is enabled
AX_CODE_COVERAGE()

# Check if HPCombi is enable, and available
AX_CHECK_HPCOMBI()

# Check if the vendored fmt should be used or not
AX_CHECK_FMT()

dnl Output configured files

AC_CONFIG_FILES([Makefile libsemigroups.pc])
AC_OUTPUT
