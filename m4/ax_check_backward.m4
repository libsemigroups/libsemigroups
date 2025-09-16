dnl handle backward checks

AC_DEFUN([AX_CHECK_BACKWARD], [
  AC_MSG_CHECKING([whether to enable backward mode])

  # Check if --enable-backward or --disable-backward was specified, and provide
  # the help string for the flag. If a flag was specified, set the value of
  # backward accordingly. Otherwise, if a flag was not specified, set backward to
  # yes.
  AC_ARG_ENABLE([backward],
    [AS_HELP_STRING([--disable-backward], [disable backward (default: no)])],
    [case "${enableval}" in
      yes) backward=yes ;;
      no)  backward=no ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-backward]) ;;
    esac],
    [backward=yes]
  )
  AC_MSG_RESULT([$backward])

  AS_IF([test "x$backward" = "xyes"], [
    AC_CHECK_HEADER(execinfo.h,
      [AC_DEFINE([BACKWARD_ENABLED], [1], [define if building with backward enabled])],
      [AC_MSG_WARN([backward enabled but execinfo.h not found, disabling backward!])])
  ])
  
  if test "x$backward" = xyes; then
    AC_MSG_CHECKING([whether to use external backward])
    AC_ARG_WITH(
      [external-backward],
      [AS_HELP_STRING([--with-external-backward], [use the external backward (default: no)])],
      [], 
      [with_external_backward=no]
    )
    AC_MSG_RESULT([$with_external_backward])
   
    if test "x$with_external_backward" = xyes;  then
      TMP_CPPFLAGS="$CPPFLAGS"
      CPPFLAGS+="$BACKWARD_CPPFLAGS"
      AC_CHECK_HEADER(backward.hpp,
        [],
        [AC_MSG_ERROR([--with-external-backward has been specified, but backward.hpp cannot be found. Consider specifying BACKWARD_CPPFLAGS to include the path to backward.hpp.])]
      )
      CPPFLAGS="$TMP_CPPFLAGS"
    else
    # The next variable becomes the preprocessor macro LIBSEMIGROUPS_WITH_INTERNAL_BACKWARD
      AC_DEFINE([WITH_INTERNAL_BACKWARD], [1], [define if building with the vendored backward])
      AC_SUBST(BACKWARD_CFLAGS, ['-I$(srcdir)/third_party/backward-cpp/'])
    fi
    AC_SUBST([BACKWARD_CPPFLAGS])
  fi

  dnl The following makes LIBSEMIGROUPS_WITH_INTERNAL_BACKWARD usable in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_WITH_INTERNAL_BACKWARD], [test "x$enable_backward" = xyes && test "x$with_external_backward" != xyes])
])



