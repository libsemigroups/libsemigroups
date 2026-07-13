dnl handle alglib checks

AC_DEFUN([AX_CHECK_ALGLIB], [
  AC_ARG_ENABLE(
    [alglib],
    [AS_HELP_STRING([--enable-alglib], [enable alglib (default: yes)])],
    [],
    [enable_alglib=yes]
  )
  AC_MSG_CHECKING([whether to enable alglib])
  AC_MSG_RESULT([$enable_alglib])
  
  dnl The next variable is used in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_ALGLIB_ENABLED], [test "x$enable_alglib" = xyes])
  dnl The following defines the LIBSEMIGROUPS_ALGLIB_ENABLED preprocessor macro
  dnl (the LIBSEMIGROUPS_ prefix is added by another part of the build system)
  AS_IF([test "x$enable_alglib" = xyes],
        [AC_DEFINE([ALGLIB_ENABLED], [1], [define if building with alglib])])
  
  if test "x$enable_alglib" = xyes; then
    AC_ARG_WITH(
      [external-alglib],
      [AS_HELP_STRING([--with-external-alglib], [use the external alglib (default: no)])],
      [], 
      [with_external_alglib=no]
    )
    AC_MSG_CHECKING([whether to use external alglib])
    AC_MSG_RESULT([$with_external_alglib])
   
    MIN_ALGLIB_VERSION="4.08.0"
    alglib_PCDEP=""

    if test "x$with_external_alglib" = xyes;  then
          m4_ifdef([PKG_CHECK_MODULES], [
              PKG_CHECK_MODULES([ALGLIB], [libalglib >= $MIN_ALGLIB_VERSION])
              alglib_PCDEP="libalglib"
          ],
          [AC_MSG_ERROR([cannot use flag --with-external-alglib, the libsemigroups configure file was created on a system without m4 macros for pkg-config available...])])
    else
          AC_SUBST(ALGLIB_CFLAGS, ['-I$(srcdir)/third_party/alglib-4.08.0/src'])
    fi
    AC_SUBST([alglib_PCDEP])
  fi

  dnl The following makes LIBSEMIGROUPS_WITH_INTERNAL_ALGLIB usable in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_WITH_INTERNAL_ALGLIB], [test "x$enable_alglib" = xyes && test "x$with_external_alglib" != xyes])
])