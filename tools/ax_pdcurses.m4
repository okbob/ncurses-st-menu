# ===========================================================================
#      http://www.gnu.org/software/autoconf-archive/ax_with_curses.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PDCURSES
#
# DESCRIPTION
#   This script will look for PDCurses libraries and setup the varaibles
#   for use in other parts of the build.
#
#   configure options:
#   PDCURSES_INSTALL    If you have the PDCurses libs/headers installed you can 
#                       simple use just this flag.  The other options will be
#                       filled in for you.  Currently support: "x11"
#   PDCURSES_INCDIR     Directory containing the curses/panel headers
#   PDCURSES_LIBDIR     Directory containing the PDCurses library
#   PDCURSES_LIB        Name of the PDCurses library, defaults to 'XCurses'
#   PDCURSES_DEP_LIBS   List of the PDCurses dependant library (exclude PDCURSES_LIB),
#                       'XCurses' is already handled.
# 
#   NOTE: If you are pointing to the PDCurses github directory built for
#         a specific OS/terminal you can omit the PDCURSES_INCDIR.
#
# EXAMPLES:
#   
#   This example builds from PDCurses source for X11:
#   NOTE: PDCURSES_LIB defaults to XCurses so we don't need to specify it
#   PDCURSES_LIBDIR=/home/username/github/PDCurses/x11 ./configure
#
#   This is a complete example (ignores the fact that some variables are not needed:
#   PDCURSES_LIBDIR=/home/username/github/PDCurses/x11 \
#   PDCURSES_INCDIR=/home/username/github/PDCurses \
#   PDCURSES_LIB=XCurses \
#   PDCURSES_DEP_LIBS=Xaw Xmu Xt X11 Xpm SM ICE Xext \
#   ./configure
#
#   Build when X11 PDCurses is installed:
#   PDCURSES_INSTALL=x11 ./configure
# 

AC_DEFUN([AX_PDCURSES], [

    AC_ARG_VAR([PDCURSES_INSTALL], [If you have the PDCurses libs/headers installed you can simple use just this flag.  The other options will be filled in for you.  Currently support: 'x11'])
    AC_ARG_VAR([PDCURSES_INCDIR], [Directory containing the curses/panel headers])
    AC_ARG_VAR([PDCURSES_LIBDIR], [Directory containing the PDCurses library])
    AC_ARG_VAR([PDCURSES_LIB], [Name of the PDCurses library, defaults to 'XCurses'])
    AC_ARG_VAR([PDCURSES_DEP_LIBS], [List of the PDCurses dependant library (exclude PDCURSES_LIB), 'XCurses' is already handled.])
    
    # For use outside this script in configure.ac
    ax_cv_pdcurses=no

    # Available in the makefile
    AC_SUBST(HAVE_PDCURSES, "no")

    AS_IF([test "$PDCURSES_INSTALL" = "x11"], [ 

        # See if xcurses-config exists, if so, build for X11
        AC_PATH_TOOL([XCURSES_CONFIG], [xcurses-config])
        if test -n "$XCURSES_CONFIG"; then
            AC_MSG_NOTICE([Building for PDCurses X11])
            PDCURSES_INSTALL=x11
            # NOTE: We don't use xcurses-config output only because 
            #       the project was setup and working prior to this 
            #       change.  If others want to do this that is fine.
            xcurses_prefix=`xcurses-config --prefix` 

            AS_IF([test "x$PDCURSES_LIBDIR" = x], [ 
                # Not set so we will do so
                PDCURSES_LIBDIR=$xcurses_prefix/lib
            ])
            AS_IF([test "x$PDCURSES_INCDIR" = x], [ 
                # Not set so we will do so
                PDCURSES_INCDIR=$xcurses_prefix/include/xcurses
            ])
        fi

        PDCURSES_LIB="XCurses"
        PDCURSES_DEP_LIBS="Xaw Xmu Xt X11 Xpm SM ICE Xext"
        AC_DEFINE([XCURSES], [1], [Should be enabled for X11 build])
    ],[
        
    ])

    # Assign a default value if missing
    AS_IF([test "x$PDCURSES_LIBDIR" != x], [ 
        AS_IF([test "x$PDCURSES_INCDIR" = x], [ 
            # This is typical if build from source and not installed
            PDCURSES_INCDIR="$PDCURSES_LIBDIR/.."
        ])
    ])
    AS_IF([test "x$PDCURSES_LIB" = x], [ 
        PDCURSES_LIB="XCurses" 
    ])

    # PDCURSES_DEP_LIBS not set set set this up if known
    AS_IF([test "x$PDCURSES_DEP_LIBS" = x], [ 
        # These are the ones we know...
        AS_IF([test "$PDCURSES_LIB" = "XCurses"], [ 
            PDCURSES_DEP_LIBS="Xaw Xmu Xt X11 Xpm SM ICE Xext"
            AC_DEFINE([XCURSES], [1], [Should be enabled for X11 build])
        ],[
            AC_MSG_ERROR([PDCURSES_DEP_LIBS must be set])
        ])
    ])

    AS_IF([test "x$PDCURSES_LIBDIR" != x], [ 

        ax_cv_pdcurses=yes
        AC_SUBST(HAVE_PDCURSES, "yes")
        
        # Adds a CFLAG (-DPDCURSES=1 etc)
        AC_DEFINE([PDCURSES], [1], [Building for PDCurses])
        AC_DEFINE([_BSD_SOURCE], [1], [Needed for strdup])
    
        # Check for the PDCurses library
        AS_IF([test "x$PDCURSES_LIBDIR" != x], [
            AC_CHECK_FILE(["$PDCURSES_LIBDIR/lib$PDCURSES_LIB.a"],
              [],[AC_MSG_ERROR([could not find a $PDCURSES_LIBDIR/lib$PDCURSES_LIB.a])]
            )
        ])

        # Check for the PDCurses headers
        AS_IF([test "x$PDCURSES_INCDIR" != x], [
            AC_CHECK_FILE(["$PDCURSES_INCDIR/curses.h"],[
                AC_DEFINE([HAVE_CURSES_H], [1], [Curses header available])
            ],[
                AC_MSG_ERROR([could not find the PDcurses header $PDCURSES_INCDIR/curses.h])
            ])
            AC_CHECK_FILE(["$PDCURSES_INCDIR/panel.h"],[
                AC_DEFINE([HAVE_PANEL_H], [1], [Panel header available])
            ],[
                AC_MSG_ERROR([could not find the PDcurses header $PDCURSES_INCDIR/panel.h])
            ])
        ])
    ])
    
    # If not using PDCurses just clear the PDCURSES_LIB
    AS_IF([test "$HAVE_PDCURSES" = "no"], [ 
        PDCURSES_LIB="" 
    ])

])dnl
