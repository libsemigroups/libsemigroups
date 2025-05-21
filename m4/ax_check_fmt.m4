dnl handle fmt checks

AC_DEFUN([AX_CHECK_FMT], [
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
        AC_SUBST(FMT_CFLAGS, ['-I$(srcdir)/third_party/fmt-11.1.4/include'])
  fi
  AC_SUBST([fmt_PCDEP])

  dnl The following makes LIBSEMIGROUPS_WITH_INTERNAL_FMT usable in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_WITH_INTERNAL_FMT], [test "x$with_external_fmt" != xyes])

  # The next variable becomes the preprocessor macro LIBSEMIGROUPS_WITH_INTERNAL_FMT
  AS_IF([test "x$with_external_fmt" != xyes],
        [AC_DEFINE([WITH_INTERNAL_FMT], [1], [define if building with the vendored fmt])])
])
