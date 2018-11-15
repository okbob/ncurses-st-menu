# -*- makefile-gmake -*-

prefix = /usr
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
mandir = ${datarootdir}/man
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
datarootdir = ${prefix}/share
sysconfdir = /etc

COMPILE_MENU = 

CC = gcc
CFLAGS = -g -O2   -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600  -DPACKAGE_NAME=\"st_menu\" -DPACKAGE_TARNAME=\"st_menu\" -DPACKAGE_VERSION=\"0\" -DPACKAGE_STRING=\"st_menu\ 0\" -DPACKAGE_BUGREPORT=\"pavel.stehule@gmail.com\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_NCURSESW=1 -DHAVE_CURSES=1 -DHAVE_CURSES_ENHANCED=1 -DHAVE_CURSES_COLOR=1 -DHAVE_CURSES_OBSOLETE=1 -DHAVE_NCURSESW_CURSES_H=1 -DHAVE_CURSES_ENHANCED=1 -DHAVE_CURSES_COLOR=1 -DHAVE_CURSES_OBSOLETE=1 -DHAVE_NCURSES_H=1 -DHAVE_PANEL=1 -DHAVE_NCURSESW_PANEL_H=1 -DHAVE_ICONV=1 -DICONV_CONST= -DHAVE_LIBUNISTRING=1
LDFLAGS = 
LDLIBS =  -lpanelw -lncursesw -ltinfo  -L/usr/lib64 -lunistring
LIBDIR = ${exec_prefix}/lib64
INCLUDEDIR = ${prefix}/include
PKG_CONFIG = /usr/bin/pkg-config
HAVE_LIBUNISTRING = yes

config.status: configure
	./config.status --recheck

config.make: config.status
	./config.status $@

config.make: config.make.in
