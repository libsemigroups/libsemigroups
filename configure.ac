dnl Process this file with autoconf to produce a configure script.

dnl Autoconf and automake initialization.

AC_PREREQ([2.69])
AC_INIT([libsemigroups],[0.3.1],[jdm3@st-andrews.ac.uk])
AC_CONFIG_SRCDIR(src/semigroups.cc)
AC_CONFIG_HEADERS([config/config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE([1.11 -Wall foreign subdir-objects])
AM_SILENT_RULES([yes])
AM_PROG_AR

AC_PROG_CXX
AC_LANG([C++])
AX_PTHREAD() # remove this line

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

if `$CXX -v 2>&1 | grep 'gcc version' >/dev/null 2>&1` ; then
  GCC_SPECIFIC_CXXFLAGS='-pthread'
fi
AC_SUBST(GCC_SPECIFIC_CXXFLAGS, $GCC_SPECIFIC_CXXFLAGS)

dnl Output configured files

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
