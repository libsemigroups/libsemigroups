dnl handle fmt checks
dnl 
dnl if --enable-fmt and if --with-external-fmt is supplied, use it if it is
dnl known to pkg-config and is new enough; otherwise use the included version
dnl
AC_DEFUN([AX_CHECK_FMT], [
  AC_ARG_ENABLE(
    [fmt],
    [AS_HELP_STRING([--enable-fmt], [enable fmt])],
    [],
    [enable_fmt=no]
  )
  AC_MSG_CHECKING([whether to enable fmt])
  AC_MSG_RESULT([$enable_fmt])

  AS_IF([test "x$enable_fmt" = xyes], 
    [AC_DEFINE([FMT_ENABLED], [1], [define if building with fmt])])
  AM_CONDITIONAL([LIBSEMIGROUPS_FMT_ENABLED], [test "x$enable_fmt" = xyes])

  if test "x$enable_fmt" = xyes;  then
    AC_ARG_WITH(
      [external-fmt],
      [AC_HELP_STRING([--with-external-fmt], [use the external fmt])],
      [],
      [with_external_fmt=no]
    )
    AC_MSG_CHECKING([whether to use external fmt])
    AC_MSG_RESULT([$with_external_fmt])

    MIN_FMT_VERSION="5.3.0"

    if test "x$with_external_fmt" = xyes;  then
          m4_ifdef([PKG_CHECK_MODULES], [
          PKG_CHECK_MODULES([FMT], 
                            [fmt >= $MIN_FMT_VERSION])],
          [AC_MSG_ERROR([cannot use flag --with-external-fmt, the libsemigroups configure file was created on a system without m4 macros for pkg-config available...])])
    else
          AC_SUBST(FMT_CFLAGS, ['-I$(srcdir)/extern/fmt-5.3.0/include'])
    fi
  fi

  AM_CONDITIONAL([LIBSEMIGROUPS_WITH_INTERNAL_FMT], [test "x$enable_fmt" = xyes && test "x$with_external_fmt" != xyes])
])

