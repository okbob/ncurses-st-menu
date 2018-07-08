all: libst_menu.so libst_menu.a demo simple

# possible to use gcc flag -DNCURSES_WIDECHAR=1 where is possible
# then any unicode char can be a accelerator

UNISTRING = 1

ifdef UNISTRING
LIB_UNISTRING = -lunistring
LIB_UNISTRING_DEF = -D LIBUNISTRING
else
UNICODE_OBJ = unicode.o
endif


st_menu_styles.o: st_menu_styles.c st_menu.h
	gcc -fPIC st_menu_styles.c -o st_menu_styles.o -Wall -c -O3 -g

unicode.o: unicode.h unicode.c
	gcc -fPIC unicode.c -o unicode.o -Wall -c -O3 -g

st_menu.o: st_menu.h st_menu.c
	gcc -fPIC st_menu.c -o st_menu.o -Wall -c -O3 -g $(LIB_UNISTRING_DEF)

libst_menu.so: st_menu_styles.o st_menu.o $(UNICODE_OBJ)
	gcc -shared -Wl,-soname,libst_menu.so -o libst_menu.so st_menu.o st_menu_styles.o -g

libst_menu.a: st_menu_styles.o st_menu.o $(UNICODE_OBJ)
	ar rcs libst_menu.a st_menu_styles.o st_menu.o $(UNICODE_OBJ)

demo: demo.c libst_menu.so libst_menu.a
	gcc demo.c -o demo libst_menu.a -Wall -lncursesw -lpanel $(LIB_UNISTRING) -g
	gcc demo.c -o demo_sl $(UNICODE_OBJ) -Wall -lncursesw -lpanel $(LIB_UNISTRING) -L. -lst_menu -g

simple: simple.c libst_menu.a
	gcc simple.c -o simple libst_menu.a -Wall -lncursesw -lpanel $(LIB_UNISTRING)

clean:
	rm st_menu_styles.o
	rm st_menu.o
	rm libst_menu.so
	rm libst_menu.a
	rm demo
	-rm demo_sl
	rm simple
	-rm $(UNICODE_OBJ)