#include <string.h>
#include <curses.h>
#include <panel.h>
#include <locale.h>
#include "unicode.h"
#include <stdlib.h>
#include <ctype.h>

typedef struct ST_menu
{
	char	*text;
	int		code;
	char	*help;
} ST_MENU;

typedef struct
{
	int		c;
	int		code;
} ST_MENU_ACCELERATOR;

typedef struct
{
	bool	force8bit;
	bool	wide_vborders;			/* wide vertical menu borders like Turbo Vision */
	bool	wide_hborders;			/* wide horizontal menu borders like custom menu mc */
	bool	draw_box;				/* when true, then box is created */
	bool	left_alligned_helpers;	/* when true, a helpers are left alligned */
	int		menu_background_cpn;	/* draw area color pair number */
	int		menu_background_attr;	/* draw area attributte */
	int		accelerator_cpn;		/* color pair of accelerators */
	int		accelerator_attr;		/* accelerator attributes */
	int		cursor_cpn;				/* cursor color pair */
	int		cursor_attr;			/* cursor attributte */
	int		cursor_accel_cpn;		/* color pair of accelerator on cursor row */
	int		cursor_accel_attr;		/* attributte of of accelerator on cursor row */
} ST_MENU_CONFIG;

#define ST_MENU_STYLE_MCB			0
#define ST_MENU_STYLE_MC			1
#define ST_MENU_STYLE_VISION		2
#define ST_MENU_STYLE_DOS			3
#define ST_MENU_STYLE_FAND_1		4
#define ST_MENU_STYLE_FAND_2		5
#define ST_MENU_STYLE_FOXPRO		6
#define ST_MENU_STYLE_PERFECT		7
#define ST_MENU_STYLE_NOCOLOR		8
#define ST_MENU_STYLE_ONECOLOR		9
#define ST_MENU_STYLE_TURBO			10
#define ST_MENU_STYLE_PDMENU		11

typedef struct
{
	ST_MENU	   *menu;
	WINDOW	   *draw_area;
	WINDOW	   *window;
	PANEL	   *panel;
	int			cursor_row;
	ST_MENU_ACCELERATOR		*accelerators;
	int			naccelerators;
	ST_MENU_CONFIG *config;
	int			help_x_pos;
	char	   *title;
	bool		pressed_accelerator;
	int			cursor_code;
} ST_MENU_STATE;

static void
st_menu_load_style(ST_MENU_CONFIG *config, int style, int start_from_cpn)
{
	switch (style)
	{
		case ST_MENU_STYLE_MCB:
			use_default_colors();
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = 0;
			init_pair(start_from_cpn++, -1, -1);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

			config->left_alligned_helpers = true;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_MC:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_CYAN);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_YELLOW, COLOR_CYAN);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_YELLOW, COLOR_BLACK);

			config->left_alligned_helpers = true;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_VISION:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = 0;
			init_pair(start_from_cpn++, COLOR_RED, COLOR_WHITE);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_GREEN);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = 0;
			init_pair(start_from_cpn++, COLOR_RED, COLOR_GREEN);

			config->left_alligned_helpers = false;
			config->wide_vborders = true;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_DOS:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_WHITE);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_FAND_1:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_CYAN);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = 0;
			init_pair(start_from_cpn++, COLOR_RED, COLOR_CYAN);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_YELLOW, COLOR_BLUE);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_YELLOW, COLOR_BLUE);

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_FAND_2:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_CYAN);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_CYAN, COLOR_CYAN);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_YELLOW, COLOR_BLUE);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_YELLOW, COLOR_BLUE);

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_FOXPRO:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_WHITE);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_CYAN);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_CYAN);

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_PERFECT:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLUE, COLOR_WHITE);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = 0;
			init_pair(start_from_cpn++, COLOR_RED, COLOR_WHITE);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_RED);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_RED);

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_NOCOLOR:
			use_default_colors();
			config->menu_background_cpn = 0;
			config->menu_background_attr = 0;

			config->accelerator_cpn = 0;
			config->accelerator_attr = A_UNDERLINE;

			config->cursor_cpn = 0;
			config->cursor_attr = A_REVERSE;

			config->cursor_accel_cpn = 0;
			config->cursor_accel_attr = A_UNDERLINE | A_REVERSE;

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_ONECOLOR:
			use_default_colors();
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = A_UNDERLINE;

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = A_REVERSE;

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_UNDERLINE | A_REVERSE;

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_TURBO:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = 0;
			init_pair(start_from_cpn++, COLOR_RED, COLOR_WHITE);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;

		case ST_MENU_STYLE_PDMENU:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_CYAN);

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_CYAN);

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = 0;
			init_pair(start_from_cpn++, COLOR_CYAN, COLOR_BLACK);

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			break;
	}

	config->draw_box = true;
}

static int
max_int(int a, int b)
{
	return a > b ? a : b;
}

/*
 * Returns display length of some text. ~ char is ignored.
 * ~~ is used as ~.
 */
static int
MenuTextDisplayWidth(ST_MENU_CONFIG *config, char *text, char **accelerator)
{
	int		result = 0;

	*accelerator = NULL;

	while (*text != '\0')
	{
		if (*text == '~')
		{
			if (text[1] == '~')
			{
				result += 1;
				text += 2;
			}
			else
			{
				text += 1;
				if (*accelerator == NULL)
					*accelerator = text;
			}

			continue;
		}

		if (config->force8bit)
		{
			result += 1;
			text += 1;
		}
		else
		{
			result += utf_dsplen(text);
			text += utf8charlen(*text);
		}
	}

	return result;
}

static void
PullDownMenuContentSize(ST_MENU_CONFIG *config, ST_MENU *menu, int *rows, int *columns, int *help_x_pos,
						ST_MENU_ACCELERATOR *accelerators, int *naccelerators, int *first_row, int *first_code)
{
	char	*accelerator;
	int	max_text_width = 0;
	int max_help_width = 0;
	int		naccel = 0;

	*rows = 0;
	*columns = 0;
	*help_x_pos = 0;
	*first_row = -1;
	*first_code = -1;

	*naccelerators = 0;

	while (menu->text != NULL)
	{
		*rows += 1;
		if (*menu->text && strncmp(menu->text, "--", 2) != 0)
		{
			int text_width = 0;
			int help_width = 0;

			if (*first_row == -1)
				*first_row = *rows;
			if (*first_code == -1)
				*first_code = menu->code;

			text_width = MenuTextDisplayWidth(config, menu->text, &accelerator);

			if (accelerator != NULL)
			{
				accelerators[naccel].c = 
					config->force8bit ? tolower(*accelerator) : utf8_tofold(accelerator);
				accelerators[naccel++].code = menu->code;
			}

			if (menu->help)
			{
				help_width = config->force8bit ? strlen(menu->help) : utf_string_dsplen(menu->help, SIZE_MAX);
			}

			if (config->left_alligned_helpers)
			{
				max_text_width = max_int(max_text_width, 1 + text_width + 2);
				max_help_width = max_int(max_help_width, help_width);
			}
			else
				*columns = max_int(*columns, 1 + text_width + 1 + (max_help_width > 0 ? help_width + 4 : 0));
		}

		menu += 1;
	}

	if (config->left_alligned_helpers)
	{
		*columns = max_text_width + (max_help_width > 0 ? max_help_width + 1 : 0);
		*help_x_pos = max_text_width;
	}
	else
		*help_x_pos = -1;

	*naccelerators = naccel;
}

static void
PullDownMenuDraw(ST_MENU_STATE *menustate)
{
	int		row = 1;
	int		maxy, maxx;
	int		text_min_x, text_max_x;
	bool	draw_box = menustate->config->draw_box;
	ST_MENU	   *menu = menustate->menu;
	ST_MENU_CONFIG	*config = menustate->config;
	WINDOW	   *draw_area = menustate->draw_area;

	getmaxyx(draw_area, maxy, maxx);

	werase(draw_area);

	if (draw_box)
		box(draw_area, 0, 0);

	text_min_x = draw_box ? 1 : 0;
	text_max_x = maxx - (draw_box ? 1 : 0);

	while (menu->text != NULL)
	{
		if (*menu->text == '\0' || strncmp(menu->text, "--", 2) == 0)
		{
			int		i;

			if (draw_box)
			{
				wmove(draw_area, row, 0);
				waddch(draw_area, ACS_LTEE);
			}
			else
				wmove(draw_area, row - 1, 0);

			for(i = 0; i < text_max_x - (draw_box ? 1 : 0); i++)
				waddch(draw_area, ACS_HLINE);

			if (draw_box)
				waddch(draw_area, ACS_RTEE);
		}
		else
		{
			char	*text = menu->text;
			bool	highlight = false;
			bool	is_cursor_row = menustate->cursor_row == row;

			if (is_cursor_row)
			{
				mvwchgat(draw_area, row - (draw_box ? 0 : 1), text_min_x, text_max_x - text_min_x,
						config->cursor_attr, config->cursor_cpn, NULL);
				wattron(draw_area, COLOR_PAIR(config->cursor_cpn) | config->cursor_attr);
			}

			wmove(draw_area, row - (draw_box ? 0 : 1), text_min_x + 1);

			while (*text)
			{
				if (*text == '~')
				{
					if (text[1] == '~')
					{
						waddstr(draw_area, "~");
						text += 2;
						continue;
					}

					if (!highlight)
					{
						wattron(draw_area,
							COLOR_PAIR(is_cursor_row ? config->cursor_accel_cpn : config->accelerator_cpn) |
									   (is_cursor_row ? config->cursor_accel_attr : config->accelerator_attr));
					}
					else
					{
						wattroff(draw_area,
							COLOR_PAIR(is_cursor_row ? config->cursor_accel_cpn : config->accelerator_cpn) |
									   (is_cursor_row ? config->cursor_accel_attr : config->accelerator_attr));
						if (is_cursor_row)
							wattron(draw_area, COLOR_PAIR(config->cursor_cpn) | config->cursor_attr);
					}

					highlight = !highlight;
					text += 1;
				}
				else
				{
					int chlen = menustate->config->force8bit ? 1 : utf8charlen(*text);

					waddnstr(draw_area, text, chlen);
					text += chlen;
				}
			}

			if (menu->help != NULL)
			{
				if (menustate->help_x_pos != -1)
				{
					wmove(draw_area, row - (draw_box ? 0 : 1), menustate->help_x_pos + (draw_box ? 1 : 0));
				}
				else
				{
					int dspl = menustate->config->force8bit ? strlen(menu->help) : utf_string_dsplen(menu->help, SIZE_MAX);

					wmove(draw_area, row - (draw_box ? 0 : 1), text_max_x - dspl - 1);
				}
				waddstr(draw_area, menu->help);
			}

			if (is_cursor_row)
				wattroff(draw_area, COLOR_PAIR(config->cursor_cpn) | config->cursor_attr);
		}

		menu += 1;
		row += 1;
	}

	wrefresh(draw_area);
}

ST_MENU_STATE *
st_menu_new(ST_MENU_CONFIG *config, ST_MENU *menu, int begin_y, int begin_x, char *title)
{
	ST_MENU_STATE *menustate;
	int		rows, cols;
	ST_MENU *aux_menu;
	int		menu_fields;

	menustate = malloc(sizeof(ST_MENU_STATE));

	menustate->menu = menu;
	menustate->config = config;
	menustate->title = title;
	menustate->naccelerators = 0;

	aux_menu = menu;
	while (aux_menu->text != NULL)
	{
		menu_fields += 1;
		aux_menu += 1;
	}

	/* preallocate good enough memory */
	menustate->accelerators = malloc(sizeof(ST_MENU_ACCELERATOR) * menu_fields);

	PullDownMenuContentSize(config, menu, &rows, &cols, &menustate->help_x_pos,
							menustate->accelerators, &menustate->naccelerators,
							&menustate->cursor_row, &menustate->cursor_code);

	if (config->draw_box)
	{
		rows += 2;
		cols += 2;
	}

	if (config->wide_vborders)
		cols += 2;
	if (config->wide_hborders)
		rows += 2;

	menustate->window = newwin(rows, cols, begin_y, begin_x);
	menustate->panel = new_panel(menustate->window);

	wbkgd(menustate->window, COLOR_PAIR(config->menu_background_cpn) | config->menu_background_attr);

	/* draw area can be same like window or smaller */
	if (config->wide_vborders || config->wide_hborders)
	{
		menustate->draw_area = derwin(menustate->window,
			rows - (config->wide_hborders ? 2 : 0),
			cols - (config->wide_vborders ? 2 : 0),
			config->wide_hborders ? 1 : 0,
			config->wide_vborders ? 1 : 0);

		wbkgd(menustate->window, COLOR_PAIR(config->menu_background_cpn) | config->menu_background_attr);
	}
	else
		menustate->draw_area = menustate->window;

	hide_panel(menustate->panel);

	return menustate;
}

void
st_menu_post(ST_MENU_STATE *menustate)
{
	show_panel(menustate->panel);
	top_panel(menustate->panel);

	update_panels();

	curs_set(0);
	noecho();

	PullDownMenuDraw(menustate);
}

void
st_menu_driver(ST_MENU_STATE *menustate, int c)
{
	ST_MENU *menu = menustate->menu;

	int		first_row = -1;
	int		last_row = -1;
	int		first_code = -1;
	int		last_code = -1;
	int		row = 1;
	int		cursor_row = menustate->cursor_row;
	bool	found_row = false;
	int		search_code = -1;

	menustate->pressed_accelerator = false;

	if (c != KEY_HOME && c != KEY_END && c != KEY_UP && c != KEY_DOWN)
	{
		/* ToDo to_fold(int) */
		int		accelerator = menustate->config->force8bit ? tolower(c) : c;
		int		i;

		for (i = 0; i < menustate->naccelerators; i++)
		{
			if (menustate->accelerators[i].c == accelerator)
			{
				search_code = menustate->accelerators[i].code;
				break;
			}
		}
	}

	while (menu->text != 0)
	{
		if (*menu->text != '\0' && strncmp(menu->text, "--", 2) != 0)
		{
			if (first_row == -1)
			{
				first_row = row;
				first_code = menu->code;

				if (c == KEY_HOME)
				{
					menustate->cursor_row = row;
					menustate->cursor_code = menu->code;
					found_row = true;
					break;
				}
			}

			if (row == cursor_row && c == KEY_UP)
			{
				if (last_row != -1)
				{
					menustate->cursor_row = last_row;
					menustate->cursor_code = last_code;
					found_row = true;
					break;
				}
				else
				{
					c = KEY_END;
				}
			}

			if (row > cursor_row && c == KEY_DOWN)
			{
				menustate->cursor_row = row;
				menustate->cursor_code = menu->code;
				found_row = true;
				break;
			}

			if (search_code == menu->code && search_code != -1)
			{
				menustate->pressed_accelerator = true;
				menustate->cursor_row = row;
				menustate->cursor_code = search_code;
				found_row = true;
				break;
			}

			last_row = row;
			last_code = menu->code;
		}
		menu += 1;
		row += 1;
	}

	if (c == KEY_END)
	{
		menustate->cursor_row = last_row;
		menustate->cursor_code = last_code;
	}
	else if (!found_row && c == KEY_DOWN)
	{
		menustate->cursor_row = first_row;
		menustate->cursor_code = first_code;
	}

	PullDownMenuDraw(menustate);
}

int
main()
{
	int		maxx, maxy;
	WINDOW *mainwin;
	PANEL *mainpanel;
	ST_MENU_CONFIG config;
	ST_MENU_STATE *menustate;
	int		c;

	ST_MENU testmenu[] = {
		{"~U~živatelské menu", 1, "F2"},
		{"Strom a~d~resářů", 2, NULL},
		{"~N~ajít soubor", 3, "M-?"},
		{"Proh~o~dit panely", 4, "C-u"},
		{"~P~anely ano/ne", 5, "C-o"},
		{"Porovnat ~a~dresáře", 6, "C-x d"},
		{"Co~m~pare files", 7, "C-x C-d"},
		{"E~x~terní panelizace", 8, "C-x !"},
		{"Ukázat velikost~i~ adresářů", 9, "C-Space"},
		{"--", -1, NULL},
		{"~H~istorie příkazů", 10, "M-h"},
		{"~R~ychlý přístup k adresářům", 11, "C-\\"},
		{"Seznam a~k~tivních VFS", 12, "C-x a"},
		{"Úlohy na po~z~adí", 13, "C-x j"},
		{"Seznam obrazovek", 14, "M-`"},
		{"--", -1, NULL},
		{"Upravit akc~e~ k příponám", 15, NULL},
		{"Upravit uživatelské menu", 16, NULL},
		{"Úprava souboru z~v~ýrazňovaných skupin", 17, NULL},
		{NULL, -1, NULL}
	};

	config.force8bit = false;

	setlocale(LC_ALL, "");

	initscr();
	start_color();
	clear();
	cbreak();
	noecho();
	keypad(stdscr, true);

	refresh();

	init_pair(1, COLOR_WHITE, COLOR_BLUE);

	st_menu_load_style(&config, 11, 2);


	getmaxyx(stdscr, maxy, maxx);

	mainwin = newwin(maxy, maxx, 0, 0);
	wbkgd(mainwin, COLOR_PAIR(1));

	mvwprintw(mainwin, 2, 2, "%s", "jmenuji se Pavel Stehule a bydlim ve Skalici a sem tam take nekde jinde");
	wrefresh(mainwin);


	mainpanel = new_panel(mainwin);

	menustate = st_menu_new(&config, testmenu, 1, 0, NULL);
	st_menu_post(menustate);

	doupdate();

	c = getch();
	while (c != 'q')
	{

		if (c == 10)
		{
			break;
		}

		st_menu_driver(menustate, c);
		doupdate();

		if (menustate->pressed_accelerator)
		{
			break;
		}

		c = getch();
	}

	hide_panel(menustate->panel);

	mvwprintw(mainwin, 3, 2, "Selected: %d", menustate->cursor_code);
	wrefresh(mainwin);

	update_panels();
	doupdate();

	getch();

	endwin();

	return 0;
}