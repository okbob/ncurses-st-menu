all: libst_menu.so libst_menu.a demo

st_menu_styles.o: st_menu_styles.c st_menu.h
	gcc -fPIC st_menu_styles.c -o st_menu_styles.o -Wall -c -DNCURSES_WIDECHAR=1

st_menu.o: st_menu.h st_menu.c
	gcc -fPIC st_menu.c -o st_menu.o -Wall -c -DNCURSES_WIDECHAR=1

libst_menu.so: st_menu_styles.o st_menu.o
	gcc -shared -Wl,-soname,libst_menu.so -o libst_menu.so st_menu.o st_menu_styles.o

libst_menu.a: st_menu_styles.o st_menu.o
	ar rcs libst_menu.a st_menu_styles.o st_menu.o

demo: demo.c libst_menu.so
	gcc demo.c -o demo libst_menu.a -Wall -lncursesw -lpanel -lunistring -DNCURSES_WIDECHAR=1 
	gcc demo.c -o demo_sl -Wall -lncursesw -lpanel -lunistring -L. -lst_menu -DNCURSES_WIDECHAR=1 -O3

clean:
	rm st_menu_styles.o
	rm st_menu.o
	rm libst_menu.so
	rm libst_menu.a
	rm demo
	rm demo_sl
