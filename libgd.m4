dnl The option stuff below is based on the similar code from Automake

# _LIBGD_MANGLE_OPTION(NAME)
# -------------------------
# Convert NAME to a valid m4 identifier, by replacing invalid characters
# with underscores, and prepend the _LIBGD_OPTION_ suffix to it.
AC_DEFUN([_LIBGD_MANGLE_OPTION],
[[_LIBGD_OPTION_]m4_bpatsubst($1, [[^a-zA-Z0-9_]], [_])])

# _LIBGD_SET_OPTION(NAME)
# ----------------------
# Set option NAME.  If NAME begins with a digit, treat it as a requested
# Guile version number, and define _LIBGD_GUILE_VERSION to that number.
# Otherwise, define the option using _LIBGD_MANGLE_OPTION.
AC_DEFUN([_LIBGD_SET_OPTION],
[m4_define(_LIBGD_MANGLE_OPTION([$1]), 1)])

# _LIBGD_SET_OPTIONS(OPTIONS)
# ----------------------------------
# OPTIONS is a space-separated list of libgd options.
AC_DEFUN([_LIBGD_SET_OPTIONS],
[m4_foreach_w([_LIBGD_Option], [$1], [_LIBGD_SET_OPTION(_LIBGD_Option)])])

# _LIBGD_IF_OPTION_SET(NAME,IF-SET,IF-NOT-SET)
# -------------------------------------------
# Check if option NAME is set.
AC_DEFUN([_LIBGD_IF_OPTION_SET],
[m4_ifset(_LIBGD_MANGLE_OPTION([$1]),[$2],[$3])])

dnl LIBGD_INIT([OPTIONS], [DIR])
dnl ----------------------------
dnl OPTIONS      A whitespace-seperated list of options.
dnl DIR          libgd submodule directory (defaults to 'libgd')
AC_DEFUN([LIBGD_INIT], [
    _LIBGD_SET_OPTIONS([$1])
    AC_SUBST([LIBGD_MODULE_DIR],[m4_if([$2],,[libgd],[$2])])

    AC_REQUIRE([LT_INIT])
    AC_REQUIRE([AC_CHECK_LIBM])
    AC_SUBST(LIBM)
    LIBGD_MODULES="gtk+-3.0 >= 3.4"
    LIBGD_GIR_INCLUDES="Gtk-3.0"
    LIBGD_SOURCES=""

    # tagged-entry: Gtk+ widget
    AM_CONDITIONAL([LIBGD_TAGGED_ENTRY],[_LIBGD_IF_OPTION_SET([tagged-entry],[true],[false])])
    _LIBGD_IF_OPTION_SET([tagged-entry],[
        AC_DEFINE([LIBGD_TAGGED_ENTRY], [1], [Description])
    ])

    # vapi: vala bindings support
    AM_CONDITIONAL([LIBGD_VAPI],[ _LIBGD_IF_OPTION_SET([vapi],[true],[false])])
    _LIBGD_IF_OPTION_SET([vapi],[
        _LIBGD_SET_OPTION([gir])
        dnl check for vapigen
        AC_PATH_PROG(VAPIGEN, vapigen, no)
        AS_IF([test x$VAPIGEN = "xno"],
              [AC_MSG_ERROR([Cannot find the "vapigen compiler in your PATH])])
    ])

    # gir: gobject introspection support
    AM_CONDITIONAL([LIBGD_GIR],[ _LIBGD_IF_OPTION_SET([gir],[true],[false])])
    _LIBGD_IF_OPTION_SET([gir],[
        GOBJECT_INTROSPECTION_REQUIRE([0.9.6])
    ])

    PKG_CHECK_MODULES(LIBGD, [ $LIBGD_MODULES ])
    AC_SUBST(LIBGD_GIR_INCLUDES)
    AC_SUBST(LIBGD_SOURCES)
])
