# handle magic_enum checks
#
# magic_enum ships a pkg-config file, but the $includedir doesn't
# contain the leading "magic_enum", so unless you are doing something
# really weird, it's easier to just try magic_enum/magic_enum.hpp
# since that's what you'll be including anyway.
#
AC_DEFUN([AX_CHECK_MAGIC_ENUM], [
  AC_ARG_WITH(
    [external-magic-enum],
    [AS_HELP_STRING([--with-external-magic-enum], [use external magic_enum (default: no)])],
    [],
    [with_external_magic_enum=no]
  )
  AC_MSG_CHECKING([whether to use external magic_enum])
  AC_MSG_RESULT([$with_external_magic_enum])

  AS_IF([test "x$with_external_magic_enum" = xyes], [
    # Check if we can use magic_enum from the system. If not, error.
    AC_CHECK_HEADER([magic_enum/magic_enum.hpp], [
      AC_MSG_NOTICE([external magic_enum will be used])
    ], [
      AC_MSG_ERROR([external magic_enum not found])
    ])
  ])
  AM_CONDITIONAL([USE_BUNDLED_MAGIC_ENUM], [test "x$with_external_magic_enum" != xyes])
])
