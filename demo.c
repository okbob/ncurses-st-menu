#include <curses.h>


int main()
{
	int		maxx, maxy;
	WINDOW *mainwin;

	int i, j, k;

	initscr();
	start_color();
	clear();
	cbreak();
	noecho();

	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_BLACK);

refresh();

	getmaxyx(stdscr, maxy, maxx);
	mainwin = newwin(maxy, maxx, 0, 0);

	wbkgd(mainwin, COLOR_PAIR(1));
	wrefresh(mainwin);

	k = 0;

//endwin();
//printf("%d %d\n", maxy, maxx);
//exit(0);


	for (i = 0; i <= maxy; i++)
		for (j = 0; j <= maxx; j++)
		{
			bool bold = false;

			if (0 && k++ % 2 == 0)
			{
				bold = true;

				wattron(mainwin, A_BOLD);
			}

			mvwaddch(mainwin, i, j, ACS_CKBOARD);
			//mvwaddch(mainwin, i, j, ACS_CKBOARD);
			//mvwprintw(mainwin, i, j, "%c", ACS_VLINE);

			if (bold)
				wattroff(mainwin, A_BOLD);

		}

	wrefresh(mainwin);

	refresh();

	getch();

	for (i = 0; i < 10; i++)
	{
		chtype c = mvwinch(mainwin, 10, 10 + i);
		//mvwchgat(mainwin, 10, 10 + i, 1, A_BOLD/* c & A_ATTRIBUTES */, 2, 0);
		wattron(mainwin, A_BOLD | COLOR_PAIR(2));
		mvwaddch(mainwin, 10, 10 + i, c & (A_CHARTEXT | A_ALTCHARSET));
	}
	

	//mvwchgat(mainwin, 10, 5, 10, 0, COLOR_PAIR(2), NULL);
	//mvchgat(10, 5, 10, 0, COLOR_PAIR(2), NULL);
	wrefresh(mainwin);
	refresh();

	getch();


	endwin();

	return 0;
}