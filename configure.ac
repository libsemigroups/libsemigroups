dnl Process this file with autoconf to produce a configure script.

dnl Autoconf and automake initialization.

AC_INIT([semigroupsplusplus],[0.0.3],[The Semigroupsplusplus team])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(semigroups.cc)
AM_INIT_AUTOMAKE([gnu check-news 1.7])
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
AC_LANG([C++])
AX_PTHREAD()

dnl *
dnl * Output configured files
dnl *

AC_CONFIG_FILES([
	Makefile
        test/Makefile
	])
AC_OUTPUT dnl

dnl * Local Variables: 
dnl * mode: m4
dnl * End: 
