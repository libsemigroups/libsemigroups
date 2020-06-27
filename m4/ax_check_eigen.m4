dnl handle eigen checks
dnl
dnl if --with-external-eigen is supplied,
dnl use it if it is known to pkg-config and is new enough;
dnl otherwise use the included version
dnl
AC_DEFUN([AX_CHECK_EIGEN], [
  
  AC_ARG_ENABLE([eigen],
      [AS_HELP_STRING([--enable-eigen], [enable eigen])],
      [],
      [enable_eigen=yes]
      )
  AC_MSG_CHECKING([whether to enable eigen])
  AC_MSG_RESULT([$enable_eigen])
  
  if test "$enable_eigen" = yes; then
    AC_ARG_WITH([external-eigen],
                [AC_HELP_STRING([--with-external-eigen],
                                [use the external eigen])],
        [with_external_eigen=yes], 
        [with_external_eigen=no])
    AC_MSG_CHECKING([whether to use external eigen])
    AC_MSG_RESULT([$with_external_eigen])

    REQUI_EIGEN_VERSION="$(cat $srcdir/extern/.EIGEN_VERSION)"
    if test "$with_external_eigen" = yes;  then
          m4_ifdef([PKG_CHECK_MODULES], [
          PKG_CHECK_MODULES([EIGEN3], 
                            [eigen3 >= $REQUI_EIGEN_VERSION])],
          [AC_MSG_ERROR([cannot use flag --with-external-eigen, the libsemigroups configure file was created on a system without m4 macros for pkg-config available...])])
    else
          AC_CHECK_FILE(
                  [$srcdir/extern/eigen-3.3.7/Eigen/QR],
                  [eigen_enabled=yes],
                  [AC_MSG_ERROR([eigen is required, clone or download the repo from https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.tar.gz into the libsemigroups/extern directory])])

          AC_SUBST(EIGEN3_CFLAGS, ['-I$(srcdir)/extern/eigen-3.3.7/'])
    fi
  fi
  dnl The following defines the LIBSEMIGROUPS_EIGEN_ENABLED preprocessor macro
  dnl (the LIBSEMIGROUPS_ prefix is added by another part of the build system)
  AS_IF([test "x$eigen_enabled" = xyes],
        [AC_DEFINE([EIGEN_ENABLED], [1], [define if building with eigen])])
  dnl The following makes LIBSEMIGROUPS_EIGEN_ENABLED usable in Makefile.am
  AM_CONDITIONAL([LIBSEMIGROUPS_EIGEN_ENABLED], [test x$eigen_enabled = xyes])
])

