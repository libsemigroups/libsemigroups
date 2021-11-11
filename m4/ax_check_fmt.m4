dnl handle fmt checks

AC_DEFUN([AX_CHECK_FMT], [
  AC_ARG_ENABLE(
    [fmt],
    [AS_HELP_STRING([--enable-fmt], [enable fmt])],
    [],
    [enable_fmt=no]
  )
  AC_MSG_CHECKING([whether to enable fmt])
  AC_MSG_RESULT([$enable_fmt])

  dnl The following defines the LIBSEMIGROUPS_FMT_ENABLED preprocessor macro
  dnl (the LIBSEMIGROUPS_ prefix is added by another part of the build system)
  AS_IF([test "x$enable_fmt" = xyes], 
    [AC_DEFINE([FMT_ENABLED], [1], [define if building with fmt])])

  if test "x$enable_fmt" = xyes;  then
    AC_ARG_WITH(
      [external-fmt],
      [AS_HELP_STRING([--with-external-fmt],[use the external fmt])],
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
          AC_SUBST(FMT_CFLAGS, ['-I$(srcdir)/extern/fmt-8.0.1/include'])
    fi
  fi

  dnl The following makes LIBSEMIGROUPS_WITH_INTERNAL_FMT usable in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_WITH_INTERNAL_FMT], [test "x$enable_fmt" = xyes && test "x$with_external_fmt" != xyes])
])

