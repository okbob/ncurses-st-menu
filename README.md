# ncurses-st-menu

ncurses based library for CUA look menu

I didn't find any library for Unix terminal applications for creating pull down and menu bar menus. 
My short target is library for menu with midnight commander look. The origin ncurses menu are not designed
for CUA applications, and cannot be used for these types of applications.

# demo

    gcc demo2.c unicode.c   -lncursesw -ltinfo -Wall -lmenuw -lform -lpanel

