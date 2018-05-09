# ncurses-st-menu 

ncurses based library for CUA look menu

I didn't find any library for Unix terminal applications for creating pull down and menu bar menus. 
My short target is library for menu with midnight commander look. The origin ncurses menu are not designed
for CUA applications, and cannot be used for these types of applications.

# Features

* CUA look pulldown and menu bar support

* Support inner accelerator ~x~ and outer accelerator _x_

* Menubar are accessable via Alt x accelerators

* Nested pulldown menu are suppported

* Possible to set style (14 attributes, 12 styles are prepared)

* Possible to set shadow around pulldown menu

* Mouse is supported

* Possible serialize and load state of menu objects

# Screenshots

![Screeshot](screenshots/scr1.png)
![Screeshot](screenshots/scr2.png)
![Screeshot](screenshots/scr4.png)
![Screeshot](screenshots/scr6.png)
![Screeshot](screenshots/scr8.png)


# Demo

    gcc demo-st-menu.c -Wall -lncursesw -lpanel -lunistring -DNCURSES_WIDECHAR=1

# Dependency

* libunistring - LGPL Unicode library

* define NCURSES_WIDECHAR when wide char ncurses support is available.

