# ncurses-st-menu 

ncurses based library for CUA look menu

I didn't find any library for Unix terminal applications for creating pull down and menu bar menus. 
My short target is library for menu with midnight commander look. The origin ncurses menu are not designed
for CUA applications, and cannot be used for these types of applications. This is library - small library.
It does only pulldown menus and menubars, nothing else. It is not complex framework with thousand functions.

# Features

* CUA look pulldown and menu bar support

* Support inner accelerator ~x~ and outer accelerator _x_

* Menubar are accessable via Alt x accelerators

* Nested pulldown menu are suppported

* Possible to set style (14 attributes, 12 styles are prepared)

* Possible to set shadow around pulldown menu

* Mouse is supported

* Possible serialize and load state of menu objects

* The usage pattern is close to original ncurses `menu` library. A menudata are
  created by functions `st_menu_new` or `st_menu_new_menubar`. A function `st_menu_post`
  displays menu, and function `st_menu_unpost` hides menu. Any event can be processed
  in menu library by function `st_menu_driver` and menu can be removed by function 
  `st_menu_delete`.

# Screenshots

![](screenshots/small/scr1.png)
![](screenshots/small/scr2.png)
![](screenshots/small/scr4.png)
![](screenshots/small/scr6.png)
![](screenshots/small/scr8.png)


# Demo

    make
    ./demo

Creates `demo` and `demo_sl`. `demo_sl` uses shared library - and it can be executed by
`LD_LIBRARY_PATH=. /.demo_sl`.

`Command|Set style` submenu is active - you can change styles interactivly.

When there are no `ncursesw` library, then modify Makefile and replace `ncursesw` by `necurses`,
and remove `-DNCURSES_WIDECHAR=1`

# Usage
```C
#include <langinfo.h>
#include <locale.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <unicase.h>

#include "st_menu.h"

/*
 * Read event. When event is mouse event, read mouse data
 */
static inline int
get_event(MEVENT *mevent, bool *alt)
{
	bool	first_event = true;
	int		c;

	*alt = false;

repeat:

	c = getch();

	/*
	 * Read mouse event when it is possible. Do it now, before st_meny_driver call,
	 * as protection to unwanted multiple call of getmouse function. For one mouse
	 * event, it returns data only once time.
	 */
	if (c == KEY_MOUSE)
	{
		int ok = getmouse(mevent);

		if (ok != OK)
			goto repeat;
	}

	if (c == ST_MENU_ESCAPE)
	{
		if (first_event)
		{
			first_event = false;
			goto repeat;
		}
	}

	*alt = !first_event;

	return c;
}

/*
 * Application demo
 */
int
main()
{
	PANEL *mainpanel;
	ST_MENU_CONFIG  config;
	ST_MENU_ITEM		   *active_item;
	struct ST_MENU *menu;
	bool	activated;
	int		c;
	MEVENT	mevent;
	bool	alt;

	ST_MENU_ITEM _file[] = {
		{"E~x~it", 34, "Alt-x"},
		{NULL, -1, NULL}
	};

	ST_MENU_ITEM menubar[] = {
		{"~F~ile", 61, NULL, 0, _file},
		{NULL, -1, NULL}
	};

	setlocale(LC_ALL, "");

	/* Don't use UTF when terminal doesn't use UTF */
	config.encoding = nl_langinfo(CODESET);
	config.language = uc_locale_language();
	config.force8bit = strcmp(config.encoding, "UTF-8") != 0;

	initscr();
	start_color();
	clear();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

#ifdef NCURSES_EXT_FUNCS

	set_escdelay(25);

#endif

	refresh();

	init_pair(1, COLOR_WHITE, COLOR_BLUE);

	/* load style, possible alternatives: ST_MENU_STYLE_MC, ST_MENU_STYLE_DOS */
	st_menu_load_style(&config, ST_MENU_STYLE_VISION, 2);

	mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED, NULL);
	mouseinterval(0);

	/* prepare main window */
	wbkgd(stdscr, COLOR_PAIR(1));
	wrefresh(stdscr);

	/*
	 * main window should be panelized. Only panels can be
	 * overlapped without unwanted effects.
	 */
	mainpanel = new_panel(stdscr);
	st_menu_set_desktop_panel(mainpanel);

	/* prepare state variable for menubar */
	menu = st_menu_new_menubar(&config, menubar);

	/* post meubar (display it) */
	st_menu_post(menu);

	c = get_event(&mevent, &alt);

	refresh();

	while (1)
	{
		bool	processed = false;

		processed = st_menu_driver(menu, c, alt, &mevent);

		doupdate();

		active_item = st_menu_selected_item(&activated);
		if (processed && activated)
		{
			/* here is processing of menucode related to Exit menu item */
			if (active_item->code == 34)
				break;
		}

		if (!processed && alt && c == 'x')
			break;

		/* get new event */
		c = get_event(&mevent, &alt);
	}

	endwin();

	st_menu_unpost(menu, true);
	st_menu_free(menu);

	return 0;
}
```

# Dependency

* libunistring - LGPL Unicode library

* define NCURSES_WIDECHAR when wide char ncurses support is available.

# Note

If you like it, send a postcard from your home country to my address, please:

    Pavel Stehule
    Skalice 12
    256 01 Benesov u Prahy
    Czech Republic


I invite any questions, comments, bug reports, patches on mail address pavel.stehule@gmail.com
