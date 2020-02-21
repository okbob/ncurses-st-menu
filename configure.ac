AC_INIT([st_menu],[0],[pavel.stehule@gmail.com],[st_menu])

AC_LANG([C])
AC_CONFIG_SRCDIR(src)
AC_CONFIG_AUX_DIR([tools])

AC_PREFIX_DEFAULT(/usr)

dnl  Document where we keep our .m4 file.
AC_CONFIG_MACRO_DIR([tools])
dnl  Make sure aclocal actually found it!
m4_pattern_forbid([^AX_])

AC_PROG_CC
AM_PROG_AR
AC_HEADER_STDC

AX_DEBUG_CFLAGS
AX_PDCURSES

if test "$ax_cv_pdcurses" = "no"; then
	AC_MSG_NOTICE([NCurses build...])

    AX_WITH_CURSES
    AX_WITH_CURSES_PANEL

    case "$ax_cv_ncurses" in "no")
    	AC_MSG_ERROR([ncurses not found])
    esac
    case "$ax_cv_ncursesw" in "no")
    	AC_MSG_WARN([The found ncurses library does not support wide-char.])
    	AC_MSG_WARN([This means that tig will not correctly render UTF-8.])
    esac

    if test "$ax_cv_panel" = "no"; then
    	AC_MSG_ERROR([ncurses panel library not found])
    fi
fi

# requires gettext-devel
gl_LIBUNISTRING

AC_SUBST(enable_debug)
AC_SUBST(CURSES_LIBS)
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)
AC_SUBST(COMPILE_MENU)
AC_SUBST(LTLIBUNISTRING)

AC_CONFIG_FILES([config.make])

# supply -g if --enable-debug
if test "$enable_debug" = yes && test "$ac_cv_prog_cc_g" = yes; then
  CFLAGS="$CFLAGS -g"
fi

AC_CONFIG_FILES([
        st_menu.pc
])

AC_OUTPUT
