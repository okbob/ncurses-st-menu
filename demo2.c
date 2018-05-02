#include <string.h>
#include <curses.h>
#include <panel.h>
#include <locale.h>
#include "unicode.h"
#include <stdlib.h>
#include <ctype.h>

typedef struct _ST_MENU
{
	char	*text;
	int		code;
	char	*help;
	struct _ST_MENU *submenu;
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
	bool	extra_inner_space;		/* when true, then there 2 spaces between text and border */
	int		shadow_width;			/* when shadow_width is higher than zero, shadow is visible */
	int		menu_background_cpn;	/* draw area color pair number */
	int		menu_background_attr;	/* draw area attributte */
	int		menu_shadow_cpn;	/* draw area color pair number */
	int		menu_shadow_attr;	/* draw area attributte */
	int		accelerator_cpn;		/* color pair of accelerators */
	int		accelerator_attr;		/* accelerator attributes */
	int		cursor_cpn;				/* cursor color pair */
	int		cursor_attr;			/* cursor attributte */
	int		cursor_accel_cpn;		/* color pair of accelerator on cursor row */
	int		cursor_accel_attr;		/* attributte of of accelerator on cursor row */
	int		disabled_cpn;			/* color of disabled menu fields */
	int		disabled_attr;			/* attributes of disabled menu fields */
	int		helper_space;			/* spaces between text and helper */
	int		text_space;				/* spaces between text fields (menubar), when it is -1, then dynamic spaces (FAND) */
	int		init_text_space;		/* initial space for menu bar */
	int		menu_bar_menu_offset;	/* offset between menu bar and menu */
	int		inner_space;			/* space between draw area and border, FAND uses 2 spaces */
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

typedef struct _ST_MENU_STATE
{
	ST_MENU	   *menu;
	WINDOW	   *draw_area;
	WINDOW	   *window;
	PANEL	   *panel;
	WINDOW	   *shadow_window;
	PANEL	   *shadow_panel;
	int			cursor_row;
	ST_MENU_ACCELERATOR		*accelerators;
	int			naccelerators;
	ST_MENU_CONFIG *config;
	int			help_x_pos;
	int			nitems;									/* number of menu items */
	int		   *bar_fields_x_pos;						/* array of x positions of menubar fields */
	char	   *title;
	bool		pressed_accelerator;
	int			cursor_code;
	bool		is_menubar;
	bool		is_visible;
	bool		is_disabled;
	bool		has_focus;
	struct _ST_MENU_STATE	*active_submenu;
	struct _ST_MENU_STATE	**submenu_states;
} ST_MENU_STATE;

ST_MENU_STATE *st_menu_new(ST_MENU_CONFIG *config, ST_MENU *menu, int begin_y, int begin_x, char *title);
void st_menu_post(ST_MENU_STATE *menustate);

WINDOW *mainwin;

MEVENT		mevent;
bool		mevent_is_active;				/* ungetmouse doesn't work */


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

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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

			config->helper_space = 5;
			config->text_space = 5;
			config->init_text_space = 2;
			config->menu_bar_menu_offset = 0;
			config->shadow_width = 0;

			break;

		case ST_MENU_STYLE_MC:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = A_BOLD;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_CYAN);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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
			config->extra_inner_space = false;

			config->helper_space = 5;
			config->text_space = 5;
			config->init_text_space = 2;
			config->menu_bar_menu_offset = 0;
			config->shadow_width = 0;

			break;

		case ST_MENU_STYLE_VISION:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = A_REVERSE;
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
			config->extra_inner_space = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 2;
			config->menu_bar_menu_offset = 1;
			config->shadow_width = 2;

			break;

		case ST_MENU_STYLE_DOS:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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
			config->extra_inner_space = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 1;
			config->shadow_width = 2;

			break;

		case ST_MENU_STYLE_FAND_1:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_CYAN);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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
			config->extra_inner_space = true;

			config->helper_space = 4;
			config->text_space = -1;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 2;
			config->shadow_width = 2;

			break;

		case ST_MENU_STYLE_FAND_2:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_CYAN);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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
			config->extra_inner_space = true;

			config->helper_space = 4;
			config->text_space = -1;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 2;
			config->shadow_width = 2;

			break;

		case ST_MENU_STYLE_FOXPRO:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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
			config->extra_inner_space = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 1;
			config->shadow_width = 2;

			break;

		case ST_MENU_STYLE_PERFECT:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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
			config->extra_inner_space = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 1;
			config->shadow_width = 2;

			break;

		case ST_MENU_STYLE_NOCOLOR:
			use_default_colors();
			config->menu_background_cpn = 0;
			config->menu_background_attr = 0;

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = A_REVERSE;

			config->accelerator_cpn = 0;
			config->accelerator_attr = A_UNDERLINE;

			config->cursor_cpn = 0;
			config->cursor_attr = A_REVERSE;

			config->cursor_accel_cpn = 0;
			config->cursor_accel_attr = A_UNDERLINE | A_REVERSE;

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;
			config->extra_inner_space = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 1;
			config->shadow_width = 0;

			break;

		case ST_MENU_STYLE_ONECOLOR:
			use_default_colors();
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = A_REVERSE;

			config->accelerator_cpn = start_from_cpn;
			config->accelerator_attr = A_UNDERLINE;

			config->cursor_cpn = start_from_cpn;
			config->cursor_attr = A_REVERSE;

			config->cursor_accel_cpn = start_from_cpn;
			config->cursor_accel_attr = A_UNDERLINE | A_REVERSE;

			config->left_alligned_helpers = false;
			config->wide_vborders = false;
			config->wide_hborders = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 1;
			config->extra_inner_space = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 1;
			config->shadow_width = 0;

			break;

		case ST_MENU_STYLE_TURBO:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_WHITE);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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
			config->extra_inner_space = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 1;
			config->shadow_width = 2;

			break;

		case ST_MENU_STYLE_PDMENU:
			config->menu_background_cpn = start_from_cpn;
			config->menu_background_attr = 0;
			init_pair(start_from_cpn++, COLOR_BLACK, COLOR_CYAN);

			config->menu_shadow_cpn = start_from_cpn;
			config->menu_shadow_attr = 0;
			init_pair(start_from_cpn++, COLOR_WHITE, COLOR_BLACK);

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
			config->extra_inner_space = false;

			config->helper_space = 4;
			config->text_space = 2;
			config->init_text_space = 1;
			config->menu_bar_menu_offset = 1;
			config->shadow_width = 2;

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

	if (accelerator != NULL)
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
				if (accelerator != NULL && *accelerator == NULL)
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
				*columns = max_int(*columns, 1 + text_width + 1 + (config->extra_inner_space ? 2 : 0) + (help_width > 0 ? help_width + 4 : 0));
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

	if (menustate->shadow_window != NULL)
	{
		int		smaxy, smaxx;
		int		i;

		getmaxyx(menustate->shadow_window, smaxy, smaxx);

		overwrite(mainwin, menustate->shadow_window);

		for (i = 0; i <= smaxy; i++)
			mvwchgat(menustate->shadow_window, i, 0, smaxx,
							config->menu_shadow_attr,
							config->menu_shadow_cpn,
							NULL);

		wnoutrefresh(menustate->shadow_window);
	}

	werase(menustate->window);

	getmaxyx(draw_area, maxy, maxx);

	if (draw_box)
		box(draw_area, 0, 0);

	text_min_x = (draw_box ? 1 : 0) + (config->extra_inner_space ? 1 : 0);
	text_max_x = maxx - (draw_box ? 1 : 0) - (config->extra_inner_space ? 1 : 0);

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

			for(i = 0; i < maxx - 1 - (draw_box ? 1 : 0); i++)
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

	wnoutrefresh(menustate->window);
}

static void
MenubarDraw(ST_MENU_STATE *menustate)
{
	ST_MENU	   *menu = menustate->menu;
	ST_MENU_CONFIG	*config = menustate->config;
	ST_MENU *aux_menu;
	int		i;

	werase(menustate->window);

	aux_menu = menu;
	i = 0;
	while (aux_menu->text != NULL)
	{
		char	*text = aux_menu->text;
		bool	highlight = false;
		bool	is_cursor_row = menustate->cursor_row == i + 1;
		int		current_pos;

		current_pos = menustate->bar_fields_x_pos[i];

		if (is_cursor_row)
		{
			wmove(menustate->window, 0, current_pos - 1);
			wattron(menustate->window, COLOR_PAIR(config->cursor_cpn) | config->cursor_attr);
			waddstr(menustate->window, " ");
		}
		else
			wmove(menustate->window, 0, current_pos);

		while (*text)
		{
			if (*text == '~')
			{
				if (text[1] == '~')
				{
					waddstr(menustate->window, "~");
					text += 2;
					continue;
				}

				if (!highlight)
				{
					wattron(menustate->window,
						COLOR_PAIR(is_cursor_row ? config->cursor_accel_cpn : config->accelerator_cpn) |
								   (is_cursor_row ? config->cursor_accel_attr : config->accelerator_attr));
				}
				else
				{
					wattroff(menustate->window,
						COLOR_PAIR(is_cursor_row ? config->cursor_accel_cpn : config->accelerator_cpn) |
								   (is_cursor_row ? config->cursor_accel_attr : config->accelerator_attr));
					if (is_cursor_row)
						wattron(menustate->window, COLOR_PAIR(config->cursor_cpn) | config->cursor_attr);
				}

				highlight = !highlight;
				text += 1;
			}
			else
			{
				int chlen = menustate->config->force8bit ? 1 : utf8charlen(*text);

				waddnstr(menustate->window, text, chlen);
				text += chlen;
			}
		}

		if (is_cursor_row)
		{
			waddstr(menustate->window, " ");
			wattroff(menustate->window, COLOR_PAIR(config->cursor_cpn) | config->cursor_attr);
		}

		aux_menu += 1;
		i += 1;
	}

	wnoutrefresh(menustate->window);
}

ST_MENU_STATE *
st_menu_new(ST_MENU_CONFIG *config, ST_MENU *menu, int begin_y, int begin_x, char *title)
{
	ST_MENU_STATE *menustate;
	int		rows, cols;
	ST_MENU *aux_menu;
	int		menu_fields;
	int		i;

	menustate = malloc(sizeof(ST_MENU_STATE));

	menustate->menu = menu;
	menustate->config = config;
	menustate->title = title;
	menustate->naccelerators = 0;
	menustate->is_menubar = false;
	menustate->is_visible = false;

	aux_menu = menu;
	while (aux_menu->text != NULL)
	{
		menu_fields += 1;
		aux_menu += 1;
	}

	/* preallocate good enough memory */
	menustate->accelerators = malloc(sizeof(ST_MENU_ACCELERATOR) * menu_fields);
	menustate->submenu_states = malloc(sizeof(ST_MENU_STATE) * menu_fields);

	aux_menu = menu;
	i = 0;
	while (aux_menu->text != NULL)
	{
		if (aux_menu->submenu != NULL)
		{
			menustate->submenu_states[i] = 
					st_menu_new(config, aux_menu->submenu,
										begin_y + i + 1 
										+ (config->draw_box ? 1 : 0)
										+ (config->wide_vborders ? 1 : 0),
										begin_x
										+ (config->wide_hborders ? 1 : 0)
										+ (config->draw_box ? 1 : 0) + 1,
										NULL);
		}
		else
			menustate->submenu_states[i] = NULL;

		aux_menu += 1;
		i += 1;
	}

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

	if (config->shadow_width > 0)
	{
		menustate->shadow_window = newwin(rows, cols, begin_y + 1, begin_x + config->shadow_width);
		menustate->shadow_panel = new_panel(menustate->shadow_window);
		hide_panel(menustate->shadow_panel);
		wbkgd(menustate->shadow_window, COLOR_PAIR(config->menu_shadow_cpn) | config->menu_shadow_attr);

		wnoutrefresh(menustate->shadow_window);
	}
	else
	{
		menustate->shadow_window = NULL;
		menustate->shadow_panel = NULL;
	}

	menustate->window = newwin(rows, cols, begin_y, begin_x);

	wbkgd(menustate->window, COLOR_PAIR(config->menu_background_cpn) | config->menu_background_attr);
	wnoutrefresh(menustate->window);

	/* draw area can be same like window or smaller */
	if (config->wide_vborders || config->wide_hborders)
	{
		menustate->draw_area = derwin(menustate->window,
			rows - (config->wide_hborders ? 2 : 0),
			cols - (config->wide_vborders ? 2 : 0),
			config->wide_hborders ? 1 : 0,
			config->wide_vborders ? 1 : 0);

		wbkgd(menustate->draw_area, COLOR_PAIR(config->menu_background_cpn) | config->menu_background_attr);

		wnoutrefresh(menustate->draw_area);
	}
	else
		menustate->draw_area = menustate->window;

	menustate->panel = new_panel(menustate->window);

	hide_panel(menustate->panel);
	if (menustate->shadow_panel != NULL)
		hide_panel(menustate->shadow_panel);

	return menustate;
}

void
st_menu_post(ST_MENU_STATE *menustate)
{
	menustate->is_visible = true;
	mevent_is_active = false;

	if (menustate->shadow_panel != NULL)
	{
		show_panel(menustate->shadow_panel);
		top_panel(menustate->shadow_panel);
	}

	show_panel(menustate->panel);
	top_panel(menustate->panel);

	update_panels();

	curs_set(0);
	noecho();

	if (menustate->is_menubar)
		MenubarDraw(menustate);
	else
		PullDownMenuDraw(menustate);
}

void
st_menu_unpost(ST_MENU_STATE *menustate)
{
	menustate->is_visible = false;

	hide_panel(menustate->panel);
	if (menustate->shadow_panel != NULL)
		hide_panel(menustate->shadow_panel);
	update_panels();
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
	int		mouse_row = -1;
	int		cursor_row = menustate->cursor_row;
	bool	found_row = false;
	int		search_code = -1;
	bool	is_menubar = menustate->is_menubar;
	ST_MENU_CONFIG	*config = menustate->config;

	menustate->pressed_accelerator = false;

	if (c == KEY_MOUSE)
	{
		if (!mevent_is_active)
		{
			mevent_is_active = (getmouse(&mevent) == OK);
		}

		if (mevent_is_active)
		{
			if (mevent.bstate & BUTTON5_PRESSED)
			{
				c = KEY_DOWN;
			}
			else if (mevent.bstate & BUTTON4_PRESSED)
			{
				c = KEY_UP;
			}
			else if (mevent.bstate & BUTTON1_PRESSED)
			{
				if (is_menubar)
				{
					if (mevent.y == 0)
					{
						int		i = 0;
						int		offset;

						offset = (config->text_space != -1) ? (config->text_space / 2) : 1;

						menu = menustate->menu;
						while (menu->text != NULL)
						{
							int		minx, maxx;

							if (i == 0)
								minx = 0;
							else
								minx = menustate->bar_fields_x_pos[i] - offset;

							maxx = menustate->bar_fields_x_pos[i + 1] - offset;

							if (mevent.x >= minx && mevent.x < maxx)
							{
								search_code = menu->code;
								break;
							}

							menu += 1;
							i = i + 1;
						}
						mevent_is_active = false;
					}
					else
					{
						if (menustate->active_submenu != NULL)
						{
							if (!wenclose(menustate->active_submenu->draw_area, mevent.y, mevent.x))
							{
								st_menu_unpost(menustate->active_submenu);
								menustate->active_submenu = NULL;
								mevent_is_active = false;
							}
						}
					}
				}
				else
				{
					int		row, col;

					row = mevent.y;
					col = mevent.x;

					if (wmouse_trafo(menustate->draw_area, &row, &col, false))
						mouse_row = row ;
					mevent_is_active = false;
				}
			}
		}
	}

	if (c != KEY_HOME && c != KEY_END && c != KEY_UP && c != KEY_DOWN && c != KEY_MOUSE)
	{
		/* ToDo to_fold(int) */
		int		accelerator = menustate->config->force8bit ? tolower(c) : c;
		int		i;

		if (menustate->active_submenu == NULL)
		{
			for (i = 0; i < menustate->naccelerators; i++)
			{
				if (menustate->accelerators[i].c == accelerator)
				{
					search_code = menustate->accelerators[i].code;
					break;
				}
			}
		}
	}

	menu = menustate->menu;
	while (menu->text != 0)
	{
		if (*menu->text != '\0' && strncmp(menu->text, "--", 2) != 0)
		{
			if (first_row == -1)
			{
				first_row = row;
				first_code = menu->code;

				if (c == KEY_HOME && !is_menubar)
				{
					menustate->cursor_row = row;
					menustate->cursor_code = menu->code;
					found_row = true;
					break;
				}
			}

			if (row == mouse_row && mouse_row != -1)
			{
				menustate->cursor_row = row;
				menustate->cursor_code = menu->code;
				found_row = true;
				break;
			}

			if (row > cursor_row && c == KEY_RIGHT && is_menubar)
			{
				menustate->cursor_row = row;
				menustate->cursor_code = menu->code;
				found_row = true;
				break;
			}

			if (row == cursor_row && c == KEY_LEFT && is_menubar)
			{
				if (last_row != -1)
				{
					menustate->cursor_row = last_row;
					menustate->cursor_code = last_code;
					found_row = true;
					break;
				}
			}

			if (row == cursor_row && c == KEY_UP && !is_menubar)
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

			if (row > cursor_row && c == KEY_DOWN  && !is_menubar)
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

	if (c == KEY_END && !is_menubar)
	{
		menustate->cursor_row = last_row;
		menustate->cursor_code = last_code;
	}
	else if (!found_row && c == KEY_DOWN && !is_menubar)
	{
		menustate->cursor_row = first_row;
		menustate->cursor_code = first_code;
	}
	else if (!found_row && c == KEY_RIGHT && is_menubar)
	{
		menustate->cursor_row = first_row;
		menustate->cursor_code = first_code;
	}
	else if (!found_row && c == KEY_LEFT && is_menubar)
	{
		menustate->cursor_row = last_row;
		menustate->cursor_code = last_code;
	}

	if (is_menubar)
	{
		bool	visible_submenu = false;
	
		if (menustate->active_submenu != NULL && cursor_row != menustate->cursor_row)
		{
			st_menu_unpost(menustate->active_submenu);
			menustate->active_submenu = NULL;
			visible_submenu = true;
		}
		if (menustate->active_submenu != NULL)
		{
			if (c != KEY_MOUSE || (c == KEY_MOUSE && mevent_is_active))
			st_menu_driver(menustate->active_submenu, c);
		}
		else if (menustate->pressed_accelerator || c == KEY_DOWN || c == 10 || visible_submenu)
		{
			menustate->active_submenu = menustate->submenu_states[menustate->cursor_row - 1];
			st_menu_post(menustate->active_submenu);
		}
	}

	if (menustate->is_menubar)
		MenubarDraw(menustate);
	else
		PullDownMenuDraw(menustate);
}

ST_MENU_STATE *
st_menu_new_menubar(ST_MENU_CONFIG *config, ST_MENU *menu)
{
	ST_MENU_STATE *menustate;
	int		maxy, maxx;
	ST_MENU *aux_menu;
	int		menu_fields;
	int		aux_width = 0;
	char   *aux_accel;
	int		text_space;
	int		current_pos;
	int		i = 0;
	int		naccel = 0;

	getmaxyx(stdscr, maxy, maxx);

	menustate = malloc(sizeof(ST_MENU_STATE));

	menustate->window = newwin(1, maxx, 0, 0);
	menustate->panel = new_panel(menustate->window);

	menustate->shadow_window = NULL;
	menustate->shadow_panel = NULL;

	menustate->config = config;
	menustate->menu = menu;
	menustate->cursor_row = 1;
	menustate->active_submenu = NULL;

	menustate->is_menubar = true;

	wnoutrefresh(menustate->window);


	wbkgd(menustate->window, COLOR_PAIR(config->menu_background_cpn) | config->menu_background_attr);

	aux_menu = menu;
	menu_fields = 0;
	while (aux_menu->text != NULL)
	{
		menu_fields += 1;

		if (config->text_space == -1)
			aux_width += MenuTextDisplayWidth(config, aux_menu->text, NULL);

		aux_menu += 1;
	}

	/*
	 * last bar position is hypotetical - we should not to calculate length of last field
	 * every time.
	 */
	menustate->bar_fields_x_pos = malloc(sizeof(int) * (menu_fields + 1));
	menustate->submenu_states = malloc(sizeof(ST_MENU_STATE) * menu_fields);
	menustate->accelerators = malloc(sizeof(ST_MENU_ACCELERATOR) * menu_fields);

	menustate->nitems = menu_fields;

	if (config->text_space == -1)
	{
		text_space = (maxx + 1 - aux_width) / (menu_fields + 1);
		if (text_space < 4)
			text_space = 4;
		else if (text_space > 15)
			text_space = 15;
		current_pos = text_space;
	}
	else
	{
		text_space = config->text_space;
		current_pos = config->init_text_space;
	}

	aux_menu = menu;
	i = 0;

	while (aux_menu->text != NULL)
	{
		char	*accelerator;

		menustate->bar_fields_x_pos[i] = current_pos;
		current_pos += MenuTextDisplayWidth(config, aux_menu->text, &accelerator);
		current_pos += text_space;
		if (aux_menu->submenu != NULL)
		{
			menustate->submenu_states[i] = 
					st_menu_new(config, aux_menu->submenu,
										1, menustate->bar_fields_x_pos[i] + 
										config->menu_bar_menu_offset
										- (config->draw_box ? 1 : 0)
										- (config->wide_vborders ? 1 : 0)
										- (config->extra_inner_space ? 1 : 0) - 1, NULL);
		}
		else
			menustate->submenu_states[i] = NULL;

		if (accelerator != NULL)
		{
				menustate->accelerators[naccel].c = 
					config->force8bit ? tolower(*accelerator) : utf8_tofold(accelerator);
				menustate->accelerators[naccel++].code = aux_menu->code;
		}

		menustate->naccelerators = naccel;

		aux_menu += 1;
		i += 1;
	}

	/*
	 * store hypotetical x bar position
	 */
	menustate->bar_fields_x_pos[i] = current_pos;

	return menustate;
}


int
main()
{
	int		maxx, maxy;
	PANEL *mainpanel;
	ST_MENU_CONFIG config;
	ST_MENU_STATE *menustate;
	int		c;
	mmask_t		prev_mousemask = 0;


	ST_MENU _left[] = {
		{"Seznam souborů", 1, NULL},
		{"~R~ychlé zobrazení", 2, "C-x q"},
		{"~I~nfo", 3, "C-x i"},
		{"~S~trom", 4, NULL},
		{"--", -1, NULL},
		{"Režim ~v~ýpisu...", 5, NULL},
		{"~P~ořadí...", 6, NULL},
		{"~F~iltr...", 7, NULL},
		{"~K~ódování", 8, "M-e"},
		{"--", -1, NULL},
		{"F~T~P spojení...", 9, NULL},
		{"S~h~ellové spojení...", 10, NULL},
		{"S~F~TP link...", 11, NULL},
		{"SM~B~ spojení...", 12, NULL},
		{"Paneli~z~e", 13, NULL},
		{"--", -1, NULL},
		{"~O~bnovit", 14, "C-r"},
		{NULL, -1, NULL}
	};

	ST_MENU _file[] = {
		{"~P~rohlížet", 15, "F3"},
		{"Pro~h~lížet soubor...",16, NULL},
		{"~F~iltrovaný pohled", 17, "M-!"},
		{"~U~pravit", 18, "F4"},
		{"~K~opírovat", 19, "F5"},
		{"~Z~měna práv", 20, "C-x c"},
		{"O~d~kaz", 21, "C-x l"},
		{"Symbolický ~o~dkaz", 22, "C-x s"},
		{"Relativní symlink", 23, "C-x v"},
		{"Upravit s~y~mbolický odkaz", 24, "C-x C-s"},
		{"Změna ~v~lastníka", 25, "C-x o"},
		{"~R~ozšířena změna práv/vlastníka", 26, NULL},
		{"Přej~m~enovat/přesunout", 27, "F6"},
		{"~N~ový adresář", 28, "F7"},
		{"~S~mazat", 29, "F8"},
		{"Ry~c~hlá změna adresáře", 30, "M-c"},
		{"--", -1, NULL},
		{"Vy~b~rat skupinu", 31, "+"},
		{"Zr~u~šit výběr skupiny", 32, "-"},
		{"~I~nvert selection", 33, "*"},
		{"--", -1, NULL},
		{"Ukonč~i~t", 34, "F10"},
		{NULL, -1, NULL}
	};

	ST_MENU _command[] = {
		{"~U~živatelské menu", 35, "F2"},
		{"Strom a~d~resářů", 36, NULL},
		{"~N~ajít soubor", 37, "M-?"},
		{"Proh~o~dit panely", 38, "C-u"},
		{"~P~anely ano/ne", 39, "C-o"},
		{"Porovnat ~a~dresáře", 40, "C-x d"},
		{"Co~m~pare files", 41, "C-x C-d"},
		{"E~x~terní panelizace", 42, "C-x !"},
		{"Ukázat velikost~i~ adresářů", 43, "C-Space"},
		{"--", -1, NULL},
		{"~H~istorie příkazů", 44, "M-h"},
		{"~R~ychlý přístup k adresářům", 45, "C-\\"},
		{"Seznam a~k~tivních VFS", 46, "C-x a"},
		{"Úlohy na po~z~adí", 47, "C-x j"},
		{"Seznam obrazovek", 48, "M-`"},
		{"--", -1, NULL},
		{"Upravit akc~e~ k příponám", 49, NULL},
		{"Upravit uživatelské menu", 50, NULL},
		{"Úprava souboru z~v~ýrazňovaných skupin", 51, NULL},
		{NULL, -1, NULL}
	};

	ST_MENU menubar[] = {
		{"~L~evý", 60, NULL, _left},
		{"~S~oubor", 61, NULL, _file},
		{"~P~říkaz", 62, NULL, _command},
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

	st_menu_load_style(&config, 1, 2);

#if NCURSES_MOUSE_VERSION > 1

	mousemask(BUTTON1_PRESSED | BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);

#else

	mousemask(BUTTON1_PRESSED, NULL);

#endif


	getmaxyx(stdscr, maxy, maxx);

	mainwin = newwin(maxy, maxx, 0, 0);
	wbkgd(mainwin, COLOR_PAIR(1));

	mvwprintw(mainwin, 2, 2, "%s", "jmenuji se Pavel Stehule a bydlim ve Skalici a sem tam take nekde jinde");
	wrefresh(mainwin);

	mainpanel = new_panel(mainwin);

	menustate = st_menu_new_menubar(&config, menubar);
	st_menu_post(menustate);

	doupdate();

mouseinterval(0);


	c = getch();
	mevent_is_active = false;
	while (c != 'q')
	{

		/* when submenu is not active, then enter activate submenu,
		 * else end
		 */
		if (c == 10 && menustate->active_submenu != NULL)
		{
			break;
		}

		st_menu_driver(menustate, c);
		doupdate();

		c = getch();
		mevent_is_active = false;
	}

	endwin();

	return 0;
}