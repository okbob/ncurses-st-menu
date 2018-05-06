# ncurses-st-menu

ncurses based library for CUA look menu

I didn't find any library for Unix terminal applications for creating pull down and menu bar menus. 
My short target is library for menu with midnight commander look. The origin ncurses menu are not designed
for CUA applications, and cannot be used for these types of applications.

# demo

    gcc demo-st-menu.c -Wall -lncursesw -lpanel -lunistring -DNCURSES_WIDECHAR=1

#dependency

* libunistring - LGPL Unicode library

* define NCURSES_WIDECHAR when wide char ncurses support is available.