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

AC_ARG_ENABLE([stats],
    [AS_HELP_STRING([--enable-stats], [enable stats mode])],
    [AC_DEFINE([STATS], [1], [define if building in stats mode])],
    [enable_stats=no]
    )
AC_MSG_CHECKING([whether to enable stats mode])
AC_MSG_RESULT([$enable_stats])

# Check if code coverage mode is enabled
AX_CODE_COVERAGE()

dnl Output configured files

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
