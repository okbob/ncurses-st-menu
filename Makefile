all: libst_menu.so libst_menu.a demoapp demoapp_sl simple simple2

# possible to use gcc flag -DNCURSES_WIDECHAR=1 where is possible
# then any unicode char can be a accelerator

include config.make

ifeq ($(HAVE_LIBUNISTRING),yes)
	undefine UNICODE_OBJ
else
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

demoapp: demo/demo.c libst_menu.so libst_menu.a include/st_menu.h
	$(CC) demo/demo.c -o demoapp libst_menu.a -Wall $(LDLIBS) -Iinclude $(CFLAGS)

demoapp_sl: demo/demo.c libst_menu.so libst_menu.a include/st_menu.h
	$(CC) demo/demo.c -o demoapp_sl $(UNICODE_OBJ) -Wall $(LDLIBS) -Iinclude -L. -lst_menu   $(CFLAGS)

simple: demo/simple.c libst_menu.a include/st_menu.h
	$(CC) demo/simple.c -o simple libst_menu.a -Wall $(LDLIBS) $(LIB_UNISTRING) -Iinclude

simple2: demo/simple2.c libst_menu.a include/st_menu.h
	$(CC) demo/simple2.c -o simple2 libst_menu.a -Wall $(LDLIBS) $(LIB_UNISTRING) -Iinclude

st_menu.pc.install:
	tools/install.sh data st_menu.pc $(PKG_CONFIG_PATH)

install: libst_menu.so libst_menu.a $(PKG_CONFIG_TARGET)
	tools/install.sh data include/st_menu.h $(INCLUDEDIR)
	tools/install.sh bin libst_menu.so $(LIBDIR)
	tools/install.sh bin libst_menu.a $(LIBDIR)

clean:
	rm -f st_menu_styles.o
	rm -f st_menu.o
	rm -f libst_menu.so
	rm -f libst_menu.a
	rm -f demoapp
	rm -f demoapp_sl
	rm -f simple
	rm -f simple2
	rm -f unicode.o
	rm -f st_menu.pc
	rm -f config.make
	rm -f aclocal.m4
	rm -f config.log
	rm -f config.status
