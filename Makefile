all: libst_menu.so libst_menu.a demo simple

# possible to use gcc flag -DNCURSES_WIDECHAR=1 where is possible
# then any unicode char can be a accelerator

include config.make

ifdef HAVE_LIBUNISTRING
undefine UNICODE_OBJ
#else
UNICODE_OBJ = unicode.o
endif

ifdef PKG_CONFIG
PKG_CONFIG_PATH = `$(PKG_CONFIG) --variable pc_path pkg-config|cut -d: -f1`
PKG_CONFIG_TARGET = st_menu.pc.install
endif

st_menu_styles.o: src/st_menu_styles.c include/st_menu.h
	$(CC) -fPIC src/st_menu_styles.c -o st_menu_styles.o -Wall -c -Iinclude $(CFLAGS)

unicode.o: src/unicode.h src/unicode.c
	$(CC) -fPIC src/unicode.c -o unicode.o -Wall -c -Iinclude $(CFLAGS)

st_menu.o: include/st_menu.h src/st_menu.c
	$(CC) -fPIC src/st_menu.c -o st_menu.o -c -O3 -g $(CFLAGS) -Iinclude -I.

libst_menu.so: st_menu_styles.o st_menu.o $(UNICODE_OBJ)
	$(CC) -shared -Wl,-soname,libst_menu.so -o libst_menu.so st_menu.o st_menu_styles.o $(UNICODE_OBJ) -Iinclude $(CFLAGS)

libst_menu.a: st_menu_styles.o st_menu.o $(UNICODE_OBJ)
	$(AR) rcs libst_menu.a st_menu_styles.o st_menu.o $(UNICODE_OBJ)

demo: demo/demo.c libst_menu.so libst_menu.a include/st_menu.h
	$(CC) demo/demo.c -o demoapp libst_menu.a -Wall $(LDLIBS) -Iinclude $(CFLAGS)
	$(CC) demo/demo.c -o demoapp_sl $(UNICODE_OBJ) -Wall $(LDLIBS) -Iinclude -L. -lst_menu   $(CFLAGS)

simple: demo/simple.c libst_menu.a include/st_menu.h
	$(CC) demo/simple.c -o simple libst_menu.a -Wall $(LDLIBS) $(LIB_UNISTRING) -Iinclude

st_menu.pc.install:
	tools/install.sh data st_menu.pc $(PKG_CONFIG_PATH)

install: libst_menu.so libst_menu.a $(PKG_CONFIG_TARGET)
	tools/install.sh data include/st_menu.h $(INCLUDEDIR)
	tools/install.sh bin libst_menu.so $(LIBDIR)
	tools/install.sh bin libst_menu.a $(LIBDIR)

clean:
	rm st_menu_styles.o
	rm st_menu.o
	rm libst_menu.so
	rm libst_menu.a
	rm demoapp
	rm demoapp_sl
	rm simple
	test -f unicode.o && rm unicode.o || true
