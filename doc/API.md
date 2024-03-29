Interface - API
===============

# Structures

## Structure `ST_MENU`

An array of this structures is used as imput template.

```c
typedef struct _ST_MENU_ITEM
{
	char	*text;						/* text of menu item, possible specify accelerator by ~ */
	int	 code;						/* code of menu item (optional) */
	char	*shortcut;					/* shortcut text, only printed (optional) */
	int	 options;					/* locked, marked, ... (optional) */
	struct _ST_MENU_ITEM *submenu;				/* reference to nested menu (optional) */
} ST_MENU_ITEM;
```

### Example

```c
ST_MENU menubar[] = {
	{"~L~eft", 60, NULL, 0, _left},
	{"~F~ile", 61, NULL, 0, _file},
	{"~C~ommand", 62, NULL, 0, _command},
	{"~O~ptions", 63, NULL, ST_MENU_OPTION_DISABLED, NULL},
	{NULL, -1, NULL}
};
```

## Structure `ST_MENU`

This is opaque structure - the content is protected, that holds state data
and some cached data used for faster visualization.

```c
struct ST_MENU;
```

## Structure `ST_MENU_CONFIG`

This structure holds configuration defined visual form and behave pull down and menubar menus.

```c
typedef struct
{
	bool	force8bit;
	char   *encoding;
	const char   *language;
	bool	wide_vborders;			/* wide vertical menu borders like Turbo Vision */
	bool	wide_hborders;			/* wide horizontal menu borders like custom menu mc */
	bool	draw_box;			/* when true, then box is created */
	bool	left_alligned_shortcuts;	/* when true, a shortcuts are left alligned */
	bool	extra_inner_space;		/* when true, then there 2 spaces between text and border */
	int	shadow_width;			/* when shadow_width is higher than zero, shadow is visible */
	int	menu_background_cpn;		/* draw area color pair number */
	int	menu_background_attr;		/* draw area attributte */
	int	menu_shadow_cpn;		/* draw area color pair number */
	int	menu_shadow_attr;		/* draw area attributte */
	int	accelerator_cpn;		/* color pair of accelerators */
	int	accelerator_attr;		/* accelerator attributes */
	int	cursor_cpn;			/* cursor color pair */
	int	cursor_attr;			/* cursor attributte */
	int	cursor_accel_cpn;		/* color pair of accelerator on cursor row */
	int	cursor_accel_attr;		/* attributte of of accelerator on cursor row */
	int	disabled_cpn;			/* color of disabled menu fields */
	int	disabled_attr;			/* attributes of disabled menu fields */
	int	shortcut_space;			/* spaces between text and shortcut */
	int	text_space;			/* spaces between text fields (menubar), when it is -1, then dynamic spaces (FAND) */
	int	init_text_space;		/* initial space for menu bar */
	int	menu_bar_menu_offset;		/* offset between menu bar and menu */
	int	inner_space;			/* space between draw area and border, FAND uses 2 spaces */
	int	extern_accel_text_space;	/* space between external accelerator and menu item text */
	int	submenu_tag;			/* symbol used for submenu tag */
	int	submenu_offset_y;		/* offset for submenu related to right border of parent menu window */
	int	submenu_offset_x;		/* offset for submenu related to cursor in parent menu window */
	int		switch_tag_n1;			/* symbol used for switch negative 1 */
	int		switch_tag_0;			/* symbol used for switch 0 */
	int		switch_tag_1;			/* symbol used for switch 1 */
} ST_MENU_CONFIG;
```

This structure can be simply initialized by function `st_menu_load_style()`.

```c
ST_MENU_CONFIG	config;

setlocale(LC_ALL, "");

/* Don't use UTF when terminal doesn't use UTF */
config.encoding = nl_langinfo(CODESET);
config.language = uc_locale_language();
config.force8bit = strcmp(config.encoding, "UTF-8") != 0;

/* load style, possible alternatives: ST_MENU_STYLE_MC, ST_MENU_STYLE_DOS */
st_menu_load_style(&config, 11, 2);
```

Available styles:
```c
#define ST_MENU_STYLE_MCB		0
#define ST_MENU_STYLE_MC		1
#define ST_MENU_STYLE_VISION		2
#define ST_MENU_STYLE_DOS		3
#define ST_MENU_STYLE_FAND_1		4
#define ST_MENU_STYLE_FAND_2		5
#define ST_MENU_STYLE_FOXPRO		6
#define ST_MENU_STYLE_PERFECT		7
#define ST_MENU_STYLE_NOCOLOR		8
#define ST_MENU_STYLE_ONECOLOR		9
#define ST_MENU_STYLE_TURBO		10
#define ST_MENU_STYLE_PDMENU		11
#define ST_MENU_STYLE_OLD_TURBO		12
#define ST_MENU_STYLE_FREE_DOS		13	/* menubar style */
#define ST_MENU_STYLE_FREE_DOS_P	14	/* pulldown menu style */
#define ST_MENU_STYLE_MC46			15
#define ST_MENU_STYLE_DBASE			16

```

# Functions

## API

```c
extern int st_menu_load_style(ST_MENU_CONFIG *config, int style, int start_from_cpn, int *start_from_rgb);
extern void st_menu_set_desktop_panel(PANEL *pan);

extern struct ST_MENU *st_menu_new(ST_MENU_CONFIG *config, ST_MENU_ITEM *items, int begin_y, int begin_x, char *title);
extern struct ST_MENU *st_menu_new_menubar(ST_MENU_CONFIG *config, ST_MENU_ITEM *items);
extern struct ST_MENU *st_menu_new_menubar2(ST_MENU_CONFIG *barcfg, ST_MENU_CONFIG *pdcfg, ST_MENU_ITEM *items);

extern void st_menu_post(struct ST_MENU *menu);
extern void st_menu_unpost(struct ST_MENU *menu, bool close_active_submenu);
extern bool st_menu_driver(struct ST_MENU *menu, int c, bool alt, MEVENT *mevent);
extern void st_menu_free(struct ST_MENU *menu);
extern void st_menu_save(struct ST_MENU *menu, int *cursor_rows, int **refvals, int max_rows);
extern void st_menu_load(struct ST_MENU *menu, int *cursor_rows, int **refvals);

extern ST_MENU *st_menu_selected_item(bool *activated);

extern bool st_menu_enable_option(struct ST_MENU *menu, int code, int option);
extern bool st_menu_reset_option(struct ST_MENU *menu, int code, int option);
extern bool st_menu_set_option(struct ST_MENU *menu, int code, int option, bool value);
extern bool st_menu_reset_all_submenu_options(struct ST_MENU *menu, int menu_code, int option);
extern bool st_menu_reset_all_options(struct ST_MENU *menu, int option);

extern bool st_menu_set_ref_option(struct ST_MENU *menu, int code, int option, int *refvalue);

extern void st_menu_set_direct_color(bool direct_color);

```

## Description

* `st_menu_load_style` - fill ST_MENU_CONFIG from predefined styles. `start_from_cpn` is first free
  color pair. This function returns next free color pair. `start_from_rgb` is pointer to int numer
  used for redefine own rgb colours (it's necessary for Hi colour themes).

* `st_menu_set_desktop_panel` - allow to specify some panel used like desktop. The content of 
   related window is used as source for shadow drawing.

* `st_menu_new` - create state variable for pull down menu from template. `st_menu_new_menubar` - same like
  previous function - creates menubar menu. `st_menu_new_menubar2` allow to specify different configurations
  (different styles) form menubar and pulldown menu (see FREE DOS style).

* `st_menu_post` shows menu, `st_menu_unpost` hides menu. The hide doesn't throw state
  data if `close_active_submenu` is false.

* `st_menu_driver` - main functionality - sends events to st_menu library. Any event has three
  parts - keycode, alt info (used or not used Alt key) and mouse event data.

* `st_menu_free` - remove state data of menu object from memory.

* We can store menu's state data before deleting to int array. The state data can be restored
  from this array by `st_menu_load` function. When menu objects are significantly changed, then
  good practice is saving state data, deleting menu object, creating new menu object based on new
  configuration, and load state data.

* The selected menu item can be accessed via function `st_menu_selected_item`. When `activated` flag
  is true, then selected item was touched by mouse, accelerator or enter key. The activated flag will
  be false every time when no item is selected.

* the menu state can be updated by function `st_menu_set_option` or function `st_menu_reset_option`.

* the state of item or group of items can be shared between application and st_menu library. You should
  to use function `st_menu_set_ref_option` and pass pointer to int variable as last parameter of this
  function. You can use options `ST_MENU_OPTION_MARKED_REF` of automatical marking one item of group
  items that shares same pointer. Or you can use `ST_MENU_OPTION_SWITCH2_REF` or `ST_MENU_OPTION_SWITCH3_REF`
  for 2 (0 - off, 1 - off) state or 3 (-1 - unknown, 0 - off, 1 - on) state value switcher. Look to
  example `simple.c`

* `st_menu_set_direct_color` allows to set direct color mode. It should be used, when `TERM` is
  `xterm-direct` or `tmux-direct`. The default is false.

## Example:
```c
#ifdef HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#include <locale.h>
#include "st_curses.h"
#include "st_panel.h"
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
	ST_MENU_ITEM		 *active_item;
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

	#ifdef HAVE_LANGINFO_CODESET
	/* Don't use UTF when terminal doesn't use UTF */
	config.encoding = nl_langinfo(CODESET);
	#else
	config.encoding = "";
	#endif
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
    wclear(stdscr);
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
