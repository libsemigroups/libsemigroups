dnl Process this file with autoconf to produce a configure script.

dnl Autoconf and automake initialization.

AC_PREREQ([2.69])
AC_INIT([semigroupsplusplus],[0.0.3],[jdm3@st-andrews.ac.uk])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(libsemigroupsplusplus/semigroups.cc)
AM_INIT_AUTOMAKE(gnu check-news 1.7 subdir-objects)
AM_SILENT_RULES([yes])
AC_CONFIG_HEADERS([config/config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

AC_SUBST(ac_aux_dir)

dnl Checks for programs.

AC_PROG_INSTALL
AC_PROG_LN_S
dnl AC_DISABLE_SHARED
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_CC
AC_LANG([C++])
AX_PTHREAD()

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdint.h stdlib.h sys/time.h unistd.h])

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

dnl Output configured files

AC_CONFIG_FILES([
	Makefile
        test/Makefile
	])
AC_OUTPUT dnl
