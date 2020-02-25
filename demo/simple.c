#include <langinfo.h>
#include <locale.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>

#ifdef HAVE_LIBUNISTRING

#include <unicase.h>

#endif

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
		{"~F~ile", 61, NULL, 0, 0, 0, _file},
		{NULL, -1, NULL}
	};

	setlocale(LC_ALL, "");

	/* Don't use UTF when terminal doesn't use UTF */
	config.encoding = nl_langinfo(CODESET);

#ifdef LIBUNISTRING

	config.language = uc_locale_language();

#else

	config.language = NULL;

#endif

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
	wclear(stdscr);
	wrefresh(stdscr);

	/*
	 * main window should be panelized. Only panels can be
	 * overlapped without unwanted effects.
	 */
	st_menu_set_desktop_window(stdscr);

	/* prepare state variable for menubar */
	menu = st_menu_new_menubar(&config, menubar);
	st_menu_set_focus(menu, ST_MENU_FOCUS_ALT_MOUSE);

	/* post meubar (display it) */
	st_menu_post(menu);

	/* refresh screen */
	doupdate();

	while (1)
	{
		bool	processed = false;

		c = get_event(&mevent, &alt);

		processed = st_menu_driver(menu, c, alt, &mevent);
		doupdate();

		active_item = st_menu_selected_item(&activated);
		if (processed && activated)
		{
			/* here is processing of menucode related to Exit menu item */
			if (active_item->code == 34)
				break;
		}

		if (!processed && (c == ST_MENU_ESCAPE || c == KEY_MOUSE))
		{
			/* Change focus type */
			st_menu_set_focus(menu, ST_MENU_FOCUS_ALT_MOUSE);

			/* Redraw menu */
			st_menu_post(menu);
			doupdate();
		}

		if (!processed && alt && c == 'x')
			break;
	}

	endwin();

	st_menu_unpost(menu, true);
	st_menu_free(menu);

	return 0;
}
