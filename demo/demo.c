
#ifdef HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#include <locale.h>
#include "st_curses.h"
#include "st_panel.h"
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#ifdef HAVE_LIBUNISTRING

#include <unicase.h>

#endif

#include "st_menu.h"

/*
 * #define DEBUG_PIPE "/home/pavel/debug"
 */

#ifdef DEBUG_PIPE

/*
 * When you would to use named pipe for debugging, then there should
 * be active reader from this pipe before start demo application.
 * In this case "tail -f ~/debug" inside other terminal.
 */

FILE   *debug_pipe = NULL;
int		debug_eventno = 0;

#endif

bool	active_xterm_mouse_mode = false;
bool	active_ncurses = false;

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

#ifdef DEBUG_PIPE

	char	buffer[20];

	fflush(debug_pipe);

#endif

#if defined(NCURSES_WIDECHAR) && (defined(HAVE_NCURSESW_CURSES_H) || defined(HAVE_NCURSESW_H))

	wint_t	ch;
	int		ret;

#endif

	*alt = false;

repeat:

#if defined(NCURSES_WIDECHAR) && (defined(HAVE_NCURSESW_CURSES_H) || defined(HAVE_NCURSESW_H))

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

#ifdef DEBUG_PIPE

	debug_eventno += 1;
	if (c == KEY_MOUSE)
	{
		sprintf(buffer, ", bstate: %08x", mevent->bstate);
	}
	else
		buffer[0] = '\0';

	fprintf(debug_pipe, "*** eventno: %d, key: %s%s%s ***\n",
			  debug_eventno,
			  *alt ? "Alt " : "",
			  keyname(c),
			  buffer);

	fflush(debug_pipe);

#endif

	return c;
}

static void
exit_demo(void)
{
	if (active_xterm_mouse_mode)
	{
		printf("\033[?1002l");
		fflush(stdout);
	}

	if (active_ncurses)
		endwin();
}

static void
SigintHandler(int sig_num)
{
	(void) sig_num;

	exit(1);
}

/*
 * Application demo
 */
int
main()
{
	int		maxx, maxy;
	ST_MENU_CONFIG  config;
	ST_MENU_CONFIG config_b;
	ST_MENU_ITEM	   *active_item = NULL;
	ST_CMDBAR_ITEM	   *active_command = NULL;
	struct ST_MENU *menu;
	struct ST_CMDBAR *cmdbar;
	bool	activated;
	int		c;
	MEVENT	mevent;
	int		i;
	bool	alt;
	bool	requested_exit = false;
	int		style = 1;
	bool	xterm_mouse_mode = false;
	char   *term_str;

	WINDOW *w1 = NULL;
	WINDOW *w2 = NULL;
	WINDOW *w3 = NULL;
	WINDOW *w4 = NULL;

	WINDOW **bgwins[4] = {&w1, &w2, &w3, &w4};

	const char *demo = 
			"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore "
			"et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
			"aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum "
			"dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia"
			"deserunt mollit anim id est laborum.";

	ST_CMDBAR_ITEM	bottombar[] = {
		{"Help", false, 1, 1, 0},
		{"Menu", false, 2, 2, 0},
		{"View", false, 3, 3, 0},
		{"Edit", false, 4, 4, 0},
		{"Copy", false, 5, 5, 0},
		{"PullDn", false, 9, 99, 0},
		{"Quit", false, 10, 100, 0},
		{NULL}
	};

	ST_MENU_ITEM _left[] = {
		{"~F~ile listing", 1, NULL, ST_MENU_OPTION_MARKED, 0, 0, NULL},
		{"~Q~uick view", 2, "C-x q", 0, 0, 0, NULL},
		{"~I~nfo", 3, "C-x i", 0, 0, 0, NULL},
		{"~T~ree", 4, NULL, 0, 0, 0, NULL},
		{"--", -1, NULL, 0, 0, 0, NULL},
		{"~L~isting mode...", 5, NULL, 0, 0, 0, NULL},
		{"~S~ort order...", 6, NULL, 0, 0, 0, NULL},
		{"~F~ilter...", 7, NULL, 0, 0, 0, NULL},
		{"~E~ncoding", 8, "M-e", 0, 0, 0, NULL},
		{"--", -1, NULL, 0, 0, 0, NULL},
		{"FT~P~ link...", 9, NULL, 0, 0, 0, NULL},
		{"S~h~ell link...", 10, NULL, 0, 0, 0, NULL},
		{"S~F~TP link...", 11, NULL, 0, 0, 0, NULL},
		{"SM~B~ link...", 12, NULL, 0, 0, 0, NULL},
		{"Paneli~z~e", 13, NULL, 0, 0, 0, NULL},
		{"--", -1, NULL, 0, 0, 0, NULL},
		{"~R~escan", 14, "C-r", 0, 0, 0, NULL},
		{NULL, -1, NULL, 0, 0, 0, NULL}
	};

	ST_MENU_ITEM _file[] = {
		{"~V~iew", 15, "F3", 0, 0, 0, NULL},
		{"Vie~w~ file...", 16, NULL, 0, 0, 0, NULL},
		{"~F~iltered view", 17, "M-!", 0, 0, 0, NULL},
		{"~E~dit", 18, "F4", 0, 0, 0, NULL},
		{"~C~opy", 19, "F5", 0, 0, 0, NULL},
		{"C~h~mod", 20, "C-x c", 0, 0, 0, NULL},
		{"~L~ink", 21, "C-x l", 0, 0, 0, NULL},
		{"~S~ymlink", 22, "C-x s", 0, 0, 0, NULL},
		{"Relative symlin~k~", 23, "C-x v", 0, 0, 0, NULL},
		{"Edit s~y~mlink", 24, "C-x C-s", 0, 0, 0, NULL},
		{"Ch~o~wn", 25, "C-x o", 0, 0, 0, NULL},
		{"~A~dvanced chown", 26, NULL, 0, 0, 0, NULL},
		{"~R~ename/Move", 27, "F6", 0, 0, 0, NULL},
		{"~M~kdir", 28, "F7", 0, 0, 0, NULL},
		{"~D~elete", 29, "F8", 0, 0, 0, NULL},
		{"~Q~uick cd", 30, "M-c", ST_MENU_OPTION_DISABLED, 0, 0, NULL},
		{"--", -1, NULL, 0, 0, 0, NULL},
		{"Select ~g~roup", 31, "+", 0, 0, 0, NULL},
		{"U~n~select group", 32, "-", 0, 0, 0, NULL},
		{"~I~nvert selection", 33, "*", 0, 0, 0, NULL},
		{"--", -1, NULL, 0, 0, 0, NULL},
		{"E~x~it", 34, "F10", 0, 0, 0, NULL},
		{NULL, -1, NULL, 0, 0, 0, NULL}
	};

#define THEME_GROUP			1
#define MENU_ITEM_SET_STYLE		52

	ST_MENU_ITEM _styles[] = {
		{"_1_Midnight black", 70, NULL, ST_MENU_STYLE_MCB, THEME_GROUP, 0, NULL},
		{"_2_Midnight", 71, NULL, ST_MENU_STYLE_MC, THEME_GROUP, 0, NULL},
		{"_3_Vision", 72, NULL, ST_MENU_STYLE_VISION, THEME_GROUP, 0, NULL},
		{"_4_Dos", 73, NULL, ST_MENU_STYLE_DOS, THEME_GROUP, 0, NULL},
		{"_5_FAND 1", 74, NULL, ST_MENU_STYLE_FAND_1, THEME_GROUP, 0, NULL},
		{"_6_FAND 2", 75, NULL, ST_MENU_STYLE_FAND_2, THEME_GROUP, 0, NULL},
		{"_7_Fox", 76, NULL, ST_MENU_STYLE_FOXPRO, THEME_GROUP, 0, NULL},
		{"_8_Perfect", 77, NULL, ST_MENU_STYLE_PERFECT, THEME_GROUP, 0, NULL},
		{"_9_No color", 78, NULL, ST_MENU_STYLE_NOCOLOR, THEME_GROUP, 0, NULL},
		{"_0_One color", 79, NULL, ST_MENU_STYLE_ONECOLOR, THEME_GROUP, 0, NULL},
		{"_t_Turbo", 80, NULL, ST_MENU_STYLE_TURBO, THEME_GROUP, 0, NULL},
		{"_p_Pdmenu", 81, NULL, ST_MENU_STYLE_PDMENU, THEME_GROUP, ST_MENU_OPTION_DEFAULT, NULL},
		{"_o_Old Turbo", 82, NULL, ST_MENU_STYLE_OLD_TURBO, THEME_GROUP, 0, NULL},
		{"_f_Free Dos", 83, NULL, ST_MENU_STYLE_FREE_DOS, THEME_GROUP, 0, NULL},
		{"_m_Midnight46", 84, NULL, ST_MENU_STYLE_MC46, THEME_GROUP, 0, NULL},
		{"_d_Dbase", 85, NULL, ST_MENU_STYLE_DBASE, THEME_GROUP, 0, NULL},
		{"_w_MenuWorks", 86, NULL, ST_MENU_STYLE_MENUWORKS, THEME_GROUP, 0, NULL},
		{"_a_Tao", 87, NULL, ST_MENU_STYLE_TAO, THEME_GROUP, 0, NULL},
		{"_g_Gold", 88, NULL, ST_MENU_STYLE_XGOLD, THEME_GROUP, 0, NULL},
		{"_b_Gold (black)", 89, NULL, ST_MENU_STYLE_XGOLD_BLACK, THEME_GROUP, 0, NULL},
		{NULL, -1, NULL, 0, 0, 0, NULL}
	};

	ST_MENU_ITEM _command[] = {
		{"~U~ser menu", 35, "F2", 0, 0, 0, NULL},
		{"~D~irectory tree", 36, NULL, 0, 0, 0, NULL},
		{"~F~ind file", 37, "M-?", 0, 0, 0, NULL},
		{"S~w~ap panels", 38, "C-u", 0, 0, 0, NULL},
		{"Switch ~p~anels on/off", 39, "C-o", 0, 0, 0, NULL},
		{"~C~ompare directories", 40, "C-x d", 0, 0, 0, NULL},
		{"C~o~mpare files", 41, "C-x C-d", 0, 0, 0, NULL},
		{"E~x~ternal panelize", 42, "C-x !", 0, 0, 0, NULL},
		{"~S~how directory sizes", 43, "C-Space", 0, 0, 0, NULL},
		{"--", -1, NULL, 0, 0, 0, NULL},
		{"Command ~h~istory", 44, "M-h", 0, 0, 0, NULL},
		{"Di~r~ectory hotlist", 45, "C-\\", 0, 0, 0, NULL},
		{"~A~ctive VFS list", 46, "C-x a", 0, 0, 0, NULL},
		{"~B~ackground jobs", 47, "C-x j", 0, 0, 0, NULL},
		{"Screen lis~t~", 48, "M-`", 0, 0, 0, NULL},
		{"--", -1, NULL, 0, 0, 0, NULL},
		{"Edit ~e~xtension file", 49, NULL, 0, 0, 0, NULL},
		{"Edit ~m~enu file", 50, NULL, 0, 0, 0, NULL},
		{"Edit highli~g~hting group file", 51, NULL, 0, 0, 0, NULL},
		{"--", -1, NULL, 0, 0, 0, NULL},
		{"Set st~y~le", MENU_ITEM_SET_STYLE, NULL, 0, 0, 0, _styles},
		{"_@_Do something on current file", 53, NULL, 0, 0, 0, NULL},
		{NULL, -1, NULL, 0, 0, 0, NULL}
	};

	ST_MENU_ITEM menubar[] = {
		{"~L~eft", 60, NULL, 0, 0, 0, _left},
		{"~F~ile", 61, NULL, 0, 0, 0,  _file},
		{"~C~ommand", 62, NULL, 0, 0, 0,  _command},
		{"~O~ptions", 63, NULL, 0, 0, ST_MENU_OPTION_DISABLED, NULL},
		{NULL, -1, NULL, 0, 0, 0, NULL}
	};

#ifdef DEBUG_PIPE

	debug_pipe = fopen(DEBUG_PIPE, "w");
	setlinebuf(debug_pipe);
	fprintf(debug_pipe, "demo application start\n");

#endif

	signal(SIGINT, SigintHandler);
	atexit(exit_demo);

	setlocale(LC_ALL, "");

	#ifdef HAVE_LANGINFO_CODESET
	/* Don't use UTF when terminal doesn't use UTF */
	config.encoding = nl_langinfo(CODESET);
	#else
	config.encoding = "";
	#endif

#ifdef LIBUNISTRING

	config.language = uc_locale_language();

#else

	config.language = NULL;

#endif

	config.force8bit = strcmp(config.encoding, "UTF-8") != 0;

	initscr();

	active_ncurses = true;

	start_color();
	clear();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	refresh();

	init_pair(1, COLOR_WHITE, COLOR_BLUE);

	/* load style, possible alternatives: ST_MENU_STYLE_MC, ST_MENU_STYLE_DOS */
	st_menu_load_style(&config, ST_MENU_STYLE_VISION, 2, false, false);

#ifdef NCURSES_EXT_FUNCS

	set_escdelay(25);

#endif

#if NCURSES_MOUSE_VERSION > 1

	term_str = getenv("TERM");
	if (term_str && strstr(term_str, "xterm"))
		xterm_mouse_mode = true;
	else
	{
		char   *kmous_str = tigetstr("kmous");

		if (kmous_str != NULL && kmous_str != (char *) -1)
		{
			if (strcmp(kmous_str, "\033[M") == 0)
				xterm_mouse_mode = true;
		}
	}

	/* BUTTON1_PRESSED | BUTTON1_RELEASED are mandatory enabled */
	mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED | BUTTON4_PRESSED | BUTTON5_PRESSED |
			  (xterm_mouse_mode ? REPORT_MOUSE_POSITION : 0), NULL);

	if (xterm_mouse_mode)
	{
		printf("\033[?1002h");
		fflush(stdout);

		active_xterm_mouse_mode = true;
	}

#else

	mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED, NULL);

	(void) xterm_mouse_mode;
	(void) term_str;

#endif

	mouseinterval(0);

	/* prepare main window */
	getmaxyx(stdscr, maxy, maxx);

	w1 = subwin(stdscr, 10, 15, 1, 0);
	w2 = subwin(stdscr, 10, maxx - 15, 1, 15);
	w3 = subwin(stdscr, maxy - 12, 15, 11, 0);
	w4 = subwin(stdscr, maxy - 12, maxx - 15, 11, 15);

	for (i = 0; i < 4; i++)
	{
		WINDOW *w = *bgwins[i];
		int		j;

		wbkgd(w, COLOR_PAIR(1));

		getmaxyx(w, maxy, maxx);

		for (j = 0; j < maxy; j++)
		{
			wmove(w, j, 0);
			waddnstr(w, demo + j , maxx);
		}

		box(w, 0, 0);
		wrefresh(w);
	}

	/* prepare state variable for cmdbar */
	cmdbar = st_cmdbar_new(&config, bottombar);
	/* display cmdbar */

	/* prepare state variable for menubar */
	menu = st_menu_new_menubar(&config, menubar);

	/* default theme is Vision, mark it in menu list */
	st_menu_enable_option(menu, 72, ST_MENU_OPTION_MARKED);


	st_menu_enable_option(menu, 50, ST_MENU_OPTION_DISABLED);
	st_menu_enable_option(menu, 51, ST_MENU_OPTION_DISABLED);

	st_menu_set_focus(menu, ST_MENU_FOCUS_ALT_MOUSE);

	/* pass desktop panel to lib to show shadows correctly */
	st_menu_set_desktop_window(stdscr);

	/* post menubar (display it) */
	st_menu_post(menu);
	st_cmdbar_post(cmdbar);

	doupdate();

	c = get_event(&mevent, &alt);

	while (!requested_exit)
	{
		bool	processed = false;

		if (c == KEY_RESIZE)
		{
			int		cursor_store[1024];
			int		*refvals_store[1024];

			getmaxyx(stdscr, maxy, maxx);
			wbkgd(stdscr, COLOR_PAIR(1));

			for (i = 0; i < maxy; i++)
			{
				wmove(stdscr, i, 0);

				waddnstr(stdscr, demo + i , maxx);
			}

			wnoutrefresh(stdscr);

			st_menu_save(menu, cursor_store, refvals_store, 1023);

			st_menu_free(menu);
			menu = st_menu_new_menubar2(&config,
						style != ST_MENU_STYLE_FREE_DOS ? NULL : &config_b, menubar);

			st_menu_load(menu, cursor_store, refvals_store);

			st_cmdbar_free(cmdbar);
			cmdbar = st_cmdbar_new(&config, bottombar);

			st_cmdbar_post(cmdbar);
			st_menu_post(menu);

			doupdate();
		}
		else
		{
			processed = st_menu_driver(menu, c, alt, &mevent);
		}

		doupdate();

		active_item = st_menu_selected_item(&activated);
		if (processed && activated)
		{
			if (active_item->group == THEME_GROUP)
			{
				int		cursor_store[1024];
				int	   *refvals_store[1024];
				int		fcp = 2;
				int		menu_code = active_item->code;
				int		start_from_rgb = 200;

				style = active_item->data;

				st_menu_save(menu, cursor_store, refvals_store, 1023);

				st_menu_free(menu);
				st_cmdbar_free(cmdbar);

				/* Better to start using default colors in applications instead in lib */
				if (style == ST_MENU_STYLE_ONECOLOR ||
						style == ST_MENU_STYLE_NOCOLOR ||
						style == ST_MENU_STYLE_MCB)
					use_default_colors();

				if (style == ST_MENU_STYLE_FREE_DOS)
					fcp = st_menu_load_style(&config_b,
												ST_MENU_STYLE_FREE_DOS_P,
												style == ST_MENU_STYLE_ONECOLOR ? 1 : fcp,
												false, false);

				st_menu_load_style_rgb(&config,
										style,
										style == ST_MENU_STYLE_ONECOLOR ? 1 : fcp,
										&start_from_rgb,
										false, false);

				menu = st_menu_new_menubar2(&config, style != ST_MENU_STYLE_FREE_DOS ? NULL : &config_b, menubar);

				st_menu_load(menu, cursor_store, refvals_store);

				cmdbar = st_cmdbar_new(&config, bottombar);

				st_menu_reset_all_submenu_options(menu, MENU_ITEM_SET_STYLE, ST_MENU_OPTION_MARKED);
				st_menu_enable_option(menu, menu_code, ST_MENU_OPTION_MARKED);

				st_cmdbar_post(cmdbar);
				st_menu_post(menu);

				refresh();
			}
			/* here is processing of menucode related to Exit menu item */
			else if (active_item->code == 34)
			{
				requested_exit = true;
				break;
			}
			else
			{
				break;
			}
		}
		else if (processed)
		{
			active_command = st_menu_selected_command(&activated);

			if (activated)
			{
				if (active_command->code == 100)
				{
					requested_exit = true;
					break;
				}
				else if (active_command->code == 99)
				{
					st_menu_set_focus(menu, ST_MENU_FOCUS_FULL);
					st_menu_post(menu);
					doupdate();
				}
				else
					break;
			}
		}

		if (!processed && c == ST_MENU_ESCAPE)
		{
			st_menu_set_focus(menu, ST_MENU_FOCUS_ALT_MOUSE);
			st_menu_post(menu);
			doupdate();
		}

		if (!processed && (c == KEY_MOUSE || c == KEY_F(10)))
		{
			st_menu_set_focus(menu, ST_MENU_FOCUS_ALT_MOUSE);
			st_menu_post(menu);
			doupdate();
		}

		/* q is common command for exit (when it is not used like accelerator */
		if (!processed && (c == 'q'))
		{
			requested_exit = true;
			break;
		}

#ifdef DEBUG_PIPE

	fflush(debug_pipe);

#endif

		/* get new event */
		c = get_event(&mevent, &alt);
	}

	endwin();

	if (xterm_mouse_mode)
	{
		printf("\033[?1002l");
		fflush(stdout);
	}

	st_menu_unpost(menu, true);
	st_menu_free(menu);

	if (requested_exit)
		printf("exiting...\n");
	else if (active_item)
		printf("selected text: %s, code: %d\n", active_item->text, active_item->code);
	else if (active_command)
		printf("selected command: %s, code: %d\n", active_command->text, active_command->code);
	else
		printf("ending without select menu item\n");

#ifdef DEBUG_PIPE

	fprintf(debug_pipe, "demo application end\n");
	fclose(debug_pipe);

#endif

	return 0;
}
