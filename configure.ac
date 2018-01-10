dnl Process this file with autoconf to produce a configure script.

dnl Autoconf and automake initialization.

AC_PREREQ([2.69])
AC_INIT([libsemigroups],m4_esyscmd([tr -d '\n' < VERSION]),[jdm3@st-andrews.ac.uk])
AC_CONFIG_SRCDIR(src/semigroups.h)
AC_CONFIG_HEADERS(config/config.h)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE([1.11 -Wall foreign subdir-objects])
AM_SILENT_RULES([yes])
AM_PROG_AR

AX_PREFIX_CONFIG_H([src/libsemigroups-config.h],[],[config/config.h])

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
AC_CHECK_HEADER([sparsehash/dense_hash_map], 
                [AC_DEFINE([HAVE_DENSEHASHMAP], 
                           [1], 
                           [define if google dense_hash_map is available])])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T
AC_CHECK_TYPES([ptrdiff_t])

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

AS_IF([test "x$enable_debug" = xyes],
    [AC_SUBST(CXXFLAGS, '-g3 -O0 -Wall -Wextra -Wno-unused-parameter -Wtype-limits -Wformat-security -Wpointer-arith -Wno-div-by-zero -Wreturn-type -Wswitch -Wunused-label -fno-omit-frame-pointer -Wno-c++11-extensions -Wold-style-cast -Wshadow')])

AS_IF([test "x$enable_debug" = xyes],
    [AC_SUBST(CFLAGS, '-g3 -O0 -Wall -Wextra -Wno-unused-parameter -Wtype-limits -Wformat-security -Wpointer-arith -Wno-div-by-zero -Wreturn-type -Wswitch -Wunused-label -fno-omit-frame-pointer -Wold-style-cast -Wshadow')])

AM_CONDITIONAL([LIBSEMIGROUPS_DEBUG], [test "x$enable_debug" = xyes])

# Check if stats mode is enabled
AC_ARG_ENABLE([stats],
    [AS_HELP_STRING([--enable-stats], [enable stats mode])],
    [AC_DEFINE([STATS], [1], [define if building in stats mode])],
    [enable_stats=no]
    )
AC_MSG_CHECKING([whether to enable stats mode])
AC_MSG_RESULT([$enable_stats])

# Check if we should use google's dense_hash_map
AC_ARG_ENABLE([densehashmap],
    [AS_HELP_STRING([--enable-densehashmap], 
                    [enable google's dense_hash_map])],
                    [AC_DEFINE([USE_DENSEHASHMAP], 
                               [1], 
                               [define if building with google's dense_hash_map])],
    [enable_densehashmap=no])

AC_MSG_CHECKING([whether to use google's dense_hash_map])
AC_MSG_RESULT([$enable_densehashmap])

# Check if HPCombi is enabled
AC_ARG_ENABLE([hpcombi],
    [AS_HELP_STRING([--enable-hpcombi], [enable HPCombi])],
    [],
    [enable_hpcombi=no]
    )
AC_MSG_CHECKING([whether to enable HPCombi])
AC_MSG_RESULT([$enable_hpcombi])

AS_IF([test "x$enable_hpcombi" = xyes],
      [AC_CHECK_FILE([extern/HPCombi/VERSION], 
                     [], 
                     [AC_MSG_WARN([HPCombi is not available, ignoring --enable-hpcombi])
                      [enable_hpcombi=no]])])

AS_IF([test "x$enable_hpcombi" = xyes],
      [AC_MSG_CHECKING([the present version of HPCombi])
      [cat extern/HPCombi/VERSION]])

AS_IF([test "x$enable_hpcombi" = xyes],
      [AC_MSG_CHECKING([the required version of HPCombi])
      [cat extern/.HPCombi_VERSION]])

AS_IF([test "x$enable_hpcombi" = xyes],
[AS_IF([! cmp -s extern/HPCombi/VERSION extern/.HPCombi_VERSION],
       [AC_MSG_WARN([the version of HPCombi that is present is incorrect, ignoring --enable-hpcombi])
        [enable_hpcombi=no]])])

SAVED_CXXFLAGS=$CXXFLAGS
CXXFLAGS="-g -O2 -mavx -flax-vector-conversions"
AC_DEFUN([CHECK_COMPILER_BUILTIN],
[AC_MSG_CHECKING([for $1])
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM(
            [[#include <x86intrin.h>]],
            [$1[($2)];
            ]
        )],
        [],
        [enable_hpcombi=no]
        )
    AC_MSG_RESULT([$enable_hpcombi])
        ])

dnl # TODO check that x86intrin.h exists and that __m128i exists
AS_IF([test "x$enable_hpcombi" = xyes], 
      [AC_MSG_NOTICE([compiler builtins required by HPCombi])
       enable_hpcombi_before_builtin_check=yes])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_blendv_epi8],[__m128i{},__m128i{},__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_cmpeq_epi8],[__m128i{},__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_cmpestri],[__m128i{},1,__m128i{},1,1])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_cmpestrm],[__m128i{},1,__m128i{},1,1])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_cmplt_epi8],[__m128i{},__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_extract_epi64],[__m128i{},1])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_max_epi8],[__m128i{},__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_max_epu8],[__m128i{},__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_min_epi8],[__m128i{},__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_min_epu8],[__m128i{},__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_movemask_epi8],[__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_popcnt_u32],[{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_set_epi64x],[1,1])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_shuffle_epi8],[__m128i{},__m128i{}])])
AS_IF([test "x$enable_hpcombi" = xyes], [CHECK_COMPILER_BUILTIN([_mm_slli_epi32],[__m128i{},1])])
CXXFLAGS=$SAVED_CXXFLAGS

AS_IF([test "x$enable_hpcombi_before_builtin_check" = xyes],
      [AS_IF([test "x$enable_hpcombi" = xno],
       [AC_MSG_WARN([compiler builtin not supported, ignoring --enable-hpcombi])])])

AS_IF([test "x$enable_hpcombi" = xyes],
      [AC_DEFINE([USE_HPCOMBI], [1], [define if building with HPCombi])])

dnl # the following is used in Makefile.am
AM_CONDITIONAL([LIBSEMIGROUPS_USE_HPCOMBI], [test "x$enable_hpcombi" = xyes])

# Check if code coverage mode is enabled
AX_CODE_COVERAGE()

dnl Output configured files

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
