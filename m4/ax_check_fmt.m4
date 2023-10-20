dnl handle fmt checks

AC_DEFUN([AX_CHECK_FMT], [
  dnl The following defines the LIBSEMIGROUPS_FMT_ENABLED preprocessor macro
  dnl (the LIBSEMIGROUPS_ prefix is added by another part of the build system)
  AC_DEFINE([FMT_ENABLED], [1], [define if building with fmt])
  dnl TODO remove the previous line when LIBSEMIGROUPS_FMT_ENABLED is
  dnl completely removed

  AC_ARG_WITH(
    [external-fmt],
    [AS_HELP_STRING([--with-external-fmt], [use the external fmt (default: no)])],
    [],
    [with_external_fmt=no]
  )
  AC_MSG_CHECKING([whether to use external fmt])
  AC_MSG_RESULT([$with_external_fmt])

  MIN_FMT_VERSION="10.1.0"

  fmt_PCDEP=""
  if test "x$with_external_fmt" = xyes;  then
        m4_ifdef([PKG_CHECK_MODULES], [
            PKG_CHECK_MODULES([FMT], [fmt >= $MIN_FMT_VERSION])
            fmt_PCDEP="fmt"
        ],
        [AC_MSG_ERROR([cannot use flag --with-external-fmt, the libsemigroups configure file was created on a system without m4 macros for pkg-config available...])])
  else
        AC_SUBST(FMT_CFLAGS, ['-I$(srcdir)/extern/fmt-10.1.1/include'])
  fi
  AC_SUBST([fmt_PCDEP])

  dnl The following makes LIBSEMIGROUPS_WITH_INTERNAL_FMT usable in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_WITH_INTERNAL_FMT], [test "x$with_external_fmt" != xyes])
])
