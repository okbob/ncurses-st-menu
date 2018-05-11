#include <langinfo.h>
#include <locale.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <unicase.h>

#include "st_menu.h"

/*
 * Read event. When event is mouse event, read mouse data
 *
 * Use wide char function when it is available. Then shortcuts
 * can be any wide char, else shortcut can be only 8bit char.
 * ncurses divides multibyte chars to separate events when a function
 * getch is used.
 */
static inline int
get_event(MEVENT *mevent, bool *alt)
{
	bool	first_event = true;
	int		c;

#if NCURSES_WIDECHAR > 0

	wint_t	ch;
	int		ret;

#endif

	*alt = false;

repeat:

#if NCURSES_WIDECHAR > 0

	ret = get_wch(&ch);
	(void) ret;

	c = ch;

#else

	c = getch();

#endif

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
	int		maxx, maxy;
	PANEL *mainpanel;
	ST_MENU_CONFIG  config;
	ST_MENU		   *active_item;
	struct ST_MENU_STATE *mstate;
	bool	press_accelerator;
	bool	button1_clicked;
	int		c;
	MEVENT	mevent;
	int		i;
	bool	alt;

	const char *demo = 
			"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore "
			"et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
			"aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum "
			"dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia"
			"deserunt mollit anim id est laborum.";

	ST_MENU _left[] = {
		{"~F~ile listing", 1, NULL},
		{"~Q~uick view", 2, "C-x q"},
		{"~I~nfo", 3, "C-x i"},
		{"~T~ree", 4, NULL},
		{"--", -1, NULL},
		{"~L~isting mode...", 5, NULL},
		{"~S~ort order...", 6, NULL},
		{"~F~ilter...", 7, NULL},
		{"~E~ncoding", 8, "M-e"},
		{"--", -1, NULL},
		{"FT~P~ link...", 9, NULL},
		{"S~h~ell link...", 10, NULL},
		{"S~F~TP link...", 11, NULL},
		{"SM~B~ link...", 12, NULL},
		{"Paneli~z~e", 13, NULL},
		{"--", -1, NULL},
		{"~R~escan", 14, "C-r"},
		{NULL, -1, NULL}
	};

	ST_MENU _file[] = {
		{"~V~iew", 15, "F3"},
		{"Vie~w~ file...",16, NULL},
		{"~F~iltered view", 17, "M-!"},
		{"~E~dit", 18, "F4"},
		{"~C~opy", 19, "F5"},
		{"C~h~mod", 20, "C-x c"},
		{"~L~ink", 21, "C-x l"},
		{"~S~ymlink", 22, "C-x s"},
		{"Relative symlin~k~", 23, "C-x v"},
		{"Edit s~y~mlink", 24, "C-x C-s"},
		{"Ch~o~wn", 25, "C-x o"},
		{"~A~dvanced chown", 26, NULL},
		{"~R~ename/Move", 27, "F6"},
		{"~M~kdir", 28, "F7"},
		{"~D~elete", 29, "F8"},
		{"~Q~uick cd", 30, "M-c"},
		{"--", -1, NULL},
		{"Select ~g~roup", 31, "+"},
		{"U~n~select group", 32, "-"},
		{"~I~nvert selection", 33, "*"},
		{"--", -1, NULL},
		{"E~x~it", 34, "F10"},
		{NULL, -1, NULL}
	};

	ST_MENU _styles[] = {
		{"_1_Midnight black", 70},
		{"_2_Midnight", 71},
		{"_3_Vision", 72},
		{"_4_Dos", 73},
		{"_5_FAND 1", 74},
		{"_6_FAND 2", 75},
		{"_7_Fox", 76},
		{"_8_Perfect", 77},
		{"_9_No color", 78},
		{"_0_One color", 79},
		{"_t_Turbo", 80},
		{"_p_Pdmenu", 81, NULL, ST_MENU_OPTION_DEFAULT},
		{NULL, -1, NULL}
	};

	ST_MENU _command[] = {
		{"~U~ser menu", 35, "F2"},
		{"~D~irectory tree", 36, NULL},
		{"~F~ind file", 37, "M-?"},
		{"S~w~ap panels", 38, "C-u"},
		{"Switch ~p~anels on/off", 39, "C-o"},
		{"~C~ompare directories", 40, "C-x d"},
		{"C~o~mpare files", 41, "C-x C-d"},
		{"E~x~ternal panelize", 42, "C-x !"},
		{"~S~how directory sizes", 43, "C-Space"},
		{"--", -1, NULL},
		{"Command ~h~istory", 44, "M-h"},
		{"Di~r~ectory hotlist", 45, "C-\\"},
		{"~A~ctive VFS list", 46, "C-x a"},
		{"~B~ackground jobs", 47, "C-x j"},
		{"Screen lis~t~", 48, "M-`"},
		{"--", -1, NULL},
		{"Edit ~e~xtension file", 49, NULL},
		{"Edit ~m~enu file", 50, NULL},
		{"Edit highli~g~hting group file", 51, NULL},
		{"--", -1, NULL},
		{"Set style", 52, NULL, 0, _styles},
		{"_@_Do something on current file", 53, NULL},
		{NULL, -1, NULL}
	};

	ST_MENU menubar[] = {
		{"~L~eft", 60, NULL, 0, _left},
		{"~F~ile", 61, NULL, 0, _file},
		{"~C~ommand", 62, NULL, 0, _command},
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

	refresh();

	init_pair(1, COLOR_WHITE, COLOR_BLUE);

	/* load style, possible alternatives: ST_MENU_STYLE_MC, ST_MENU_STYLE_DOS */
	st_menu_load_style(&config, 11, 2);

#ifdef NCURSES_EXT_FUNCS

	set_escdelay(25);

#endif

#if NCURSES_MOUSE_VERSION > 1

	/* BUTTON1_PRESSED | BUTTON1_RELEASED are mandatory enabled */
	mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED | BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);

#else

	mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED, NULL);

#endif

	mouseinterval(0);

	/* prepare main window */
	getmaxyx(stdscr, maxy, maxx);
	wbkgd(stdscr, COLOR_PAIR(1));

	for (i = 0; i <= maxy; i++)
	{
		wmove(stdscr, i, 0);

		waddnstr(stdscr, demo + i , maxx);
	}

	wrefresh(stdscr);

	/*
	 * main window should be panelized. Only panels can be
	 * overlapped without unwanted effects.
	 */
	mainpanel = new_panel(stdscr);

	/* pass desktop panel to lib to show shadows correctly */
	st_menu_set_desktop_panel(mainpanel);

	/* prepare state variable for menubar */
	mstate = st_menu_new_menubar(&config, menubar);

	/* post meubar (display it) */
	st_menu_post(mstate);

	c = get_event(&mevent, &alt);

	refresh();

	while (1)
	{
		bool	processed;

		/* when submenu is not active, then enter activate submenu,
		 * else end
		 */
		if (c == 10 && st_menu_is_active_submenu(mstate))
		{
			active_item = st_menu_active_item(&press_accelerator, &button1_clicked);
			if (active_item && !active_item->submenu)
			{
				goto process_code;
			}
		}

		if (c == KEY_RESIZE)
		{
			int		cursor_store[1024];

			getmaxyx(stdscr, maxy, maxx);
			wbkgd(stdscr, COLOR_PAIR(1));

			for (i = 0; i <= maxy; i++)
			{
				wmove(stdscr, i, 0);

				waddnstr(stdscr, demo + i , maxx);
			}

			wnoutrefresh(stdscr);

			st_menu_save(mstate, cursor_store, 1023);

			st_menu_delete(mstate);
			mstate = st_menu_new_menubar(&config, menubar);

			st_menu_load(mstate, cursor_store);

			st_menu_post(mstate);

			doupdate();
		}
		else
		{
			processed = st_menu_driver(mstate, c, alt, &mevent);
		}

		doupdate();

		active_item = st_menu_active_item(&press_accelerator, &button1_clicked);
		if (processed && (press_accelerator || button1_clicked))
		{

process_code:

			if (active_item && active_item->code >= 70 && active_item->code <= 81)
			{
				int		style = active_item->code - 70;
				int		cursor_store[1024];

				st_menu_save(mstate, cursor_store, 1023);

				st_menu_delete(mstate);

				st_menu_load_style(&config,
					style, style == ST_MENU_STYLE_ONECOLOR ? 1 : 2);

				mstate = st_menu_new_menubar(&config, menubar);

				st_menu_load(mstate, cursor_store);

				st_menu_post(mstate);

				refresh();
			}
			else
			{
				break;
			}
		}

		if (c == 'q' && !press_accelerator)
			break;

		/* get new event */
		c = get_event(&mevent, &alt);
	}

	endwin();

	st_menu_unpost(mstate, true);
	st_menu_delete(mstate);

	if (active_item != NULL && !(c == 'q' && !press_accelerator))
		printf("selected text: %s, code: %d\n", active_item->text, active_item->code);
	else
		printf("ending without select menu item\n");

	return 0;
}
