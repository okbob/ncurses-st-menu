all: libst_menu.so libst_menu.a demo simple

# possible to use gcc flag -DNCURSES_WIDECHAR=1 where is possible
# then any unicode char can be a accelerator

st_menu_styles.o: st_menu_styles.c st_menu.h
	gcc -fPIC st_menu_styles.c -o st_menu_styles.o -Wall -c -O3 -g

st_menu.o: st_menu.h st_menu.c
	gcc -fPIC st_menu.c -o st_menu.o -Wall -c -O3 -g

libst_menu.so: st_menu_styles.o st_menu.o
	gcc -shared -Wl,-soname,libst_menu.so -o libst_menu.so st_menu.o st_menu_styles.o -g

libst_menu.a: st_menu_styles.o st_menu.o
	ar rcs libst_menu.a st_menu_styles.o st_menu.o

demo: demo.c libst_menu.so libst_menu.a
	gcc demo.c -o demo libst_menu.a -Wall -lncursesw -lpanel -lunistring -g
	gcc demo.c -o demo_sl -Wall -lncursesw -lpanel -lunistring -L. -lst_menu -g

simple: simple.c libst_menu.a
	gcc simple.c -o simple libst_menu.a -Wall -lncursesw -lpanel -lunistring


clean:
	rm st_menu_styles.o
	rm st_menu.o
	rm libst_menu.so
	rm libst_menu.a
	rm demo
	rm demo_sl
	rm simple