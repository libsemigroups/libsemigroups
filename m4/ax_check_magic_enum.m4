# handle magic_enum checks
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
    # The version constraint is to ensure that the headers relative to
    # $includedir are magic_enum/foo.hpp. In 0.9.6, the magic_enum/
    # prefix was not used.
    PKG_CHECK_MODULES(MAGICENUM, [magic_enum >= 0.9.7], [
      AC_MSG_NOTICE([external magic_enum will be used])
    ], [
      AC_MSG_ERROR([external magic_enum not found])
    ])
  ])
  AM_CONDITIONAL([USE_BUNDLED_MAGIC_ENUM], [test "x$with_external_magic_enum" != xyes])
])
