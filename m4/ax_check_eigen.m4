dnl handle eigen checks

AC_DEFUN([AX_CHECK_EIGEN], [
  AC_ARG_ENABLE(
    [eigen],
    [AS_HELP_STRING([--enable-eigen], [enable eigen])],
    [],
    [enable_eigen=yes]
  )
  AC_MSG_CHECKING([whether to enable eigen])
  AC_MSG_RESULT([$enable_eigen])
  
  dnl The following defines the LIBSEMIGROUPS_EIGEN_ENABLED preprocessor macro
  dnl (the LIBSEMIGROUPS_ prefix is added by another part of the build system)
  AS_IF([test "x$enable_eigen" = xyes],
        [AC_DEFINE([EIGEN_ENABLED], [1], [define if building with eigen])])
  
  if test "x$enable_eigen" = xyes; then
    AC_ARG_WITH(
      [external-eigen],
      [AS_HELP_STRING([--with-external-eigen],[use the external eigen])],
      [], 
      [with_external_eigen=no]
    )
    AC_MSG_CHECKING([whether to use external eigen])
    AC_MSG_RESULT([$with_external_eigen])
   
    MIN_EIGEN_VERSION="3.3.7"

    if test "x$with_external_eigen" = xyes;  then
          m4_ifdef([PKG_CHECK_MODULES], [
          PKG_CHECK_MODULES([EIGEN3], 
                            [eigen3 >= $MIN_EIGEN_VERSION])],
          [AC_MSG_ERROR([cannot use flag --with-external-eigen, the libsemigroups configure file was created on a system without m4 macros for pkg-config available...])])
    else
          AC_SUBST(EIGEN3_CFLAGS, ['-I$(srcdir)/extern/eigen-3.3.9/'])
    fi
  fi

  dnl The following makes LIBSEMIGROUPS_WITH_INTERNAL_EIGEN usable in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_WITH_INTERNAL_EIGEN], [test "x$enable_eigen" = xyes && test "x$with_external_eigen" != xyes])
])



