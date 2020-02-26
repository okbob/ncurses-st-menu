all: libst_menu.so libst_menu.a demoapp demoapp_sl simple simple2 post_build

# possible to use gcc flag -DNCURSES_WIDECHAR=1 where is possible
# then any unicode char can be a accelerator

include config.make

ifeq ($(HAVE_LIBUNISTRING),yes)
	undefine UNICODE_OBJ
else
	UNICODE_OBJ = unicode.o
endif

ifeq ($(BUILD_OS),windows)
	PROG_EXT=.exe
	DLL_EXT=.dll
else
	DLL_EXT=.so
endif

ifdef PKG_CONFIG
PKG_CONFIG_PATH = `$(PKG_CONFIG) --variable pc_path pkg-config|cut -d: -f1`
PKG_CONFIG_TARGET = st_menu.pc.install
endif

INCDIRS += $(PDCURSES_INCDIR) include .
LIBDIRS += $(PDCURSES_LIBDIR) .
DEPLIBS += $(PDCURSES_DEP_LIBS)

ifeq ($(HAVE_PDCURSES),yes)
	PDCURSES_STATIC_LIB = $(PDCURSES_LIBDIR)/$(PDCURSES_LIB).a
	ifeq "$(BUILD_OS)" "windows"
	PDCURSES_DYN_LIB = $(PDCURSES_LIBDIR)/$(PDCURSES_LIB).dll
	else
	PDCURSES_DYN_LIB = $(PDCURSES_LIBDIR)/$(PDCURSES_LIB).so
	endif
else
	# This is required only for the shared lib implementation 
	# because by default the panel lib is defined too early 
	# in the list of dependant libs/obejcts.  Might be another
	# solution but this seems to work.
	NCURSES_PANEL_LIB = -lpanel
endif

ST_INCDIRS := $(foreach incdir,$(INCDIRS),-I$(incdir))
ST_LIBDIRS := $(foreach librarydir,$(LIBDIRS),-L$(librarydir))
ST_DEPLIBS := $(foreach library,$(DEPLIBS),-l$(library))

st_menu_styles.o: src/st_menu_styles.c include/st_menu.h
	@printf "\nBuilding: $<...\n"
	$(CC) -fPIC src/st_menu_styles.c -o st_menu_styles.o -Wall -c $(ST_INCDIRS) $(CFLAGS)

unicode.o: src/unicode.h src/unicode.c
	@printf "\nBuilding: $@...\n"
	$(CC) -fPIC src/unicode.c -o unicode.o -Wall -c $(ST_INCDIRS) $(CFLAGS)

st_menu.o: include/st_menu.h src/st_menu.c
	@printf "\nBuilding: $@...\n"
	$(CC) -fPIC src/st_menu.c -o st_menu.o -c -O3 -g $(CFLAGS) $(ST_INCDIRS)

libst_menu.so: st_menu_styles.o st_menu.o $(UNICODE_OBJ)
	@printf "\nBuilding: libst_menu$(DLL_EXT)...\n"
	$(CC) -shared -Wl,-soname,libst_menu$(DLL_EXT) -o libst_menu$(DLL_EXT) st_menu.o st_menu_styles.o $(PDCURSES_DYN_LIB) $(UNICODE_OBJ) $(ST_INCDIRS) $(CFLAGS)

libst_menu.a: st_menu_styles.o st_menu.o $(UNICODE_OBJ)
	@printf "\nBuilding: $@...\n"
	$(AR) rcs libst_menu.a st_menu_styles.o st_menu.o $(UNICODE_OBJ)

demoapp: demo/demo.c libst_menu.so libst_menu.a include/st_menu.h
	@printf "\nBuilding: $@...\n"
	$(CC) demo/demo.c -o demoapp libst_menu.a $(PDCURSES_STATIC_LIB) -Wall $(ST_LIBDIRS) $(LDLIBS) $(ST_DEPLIBS) $(ST_INCDIRS) $(CFLAGS)

demoapp_sl: demo/demo.c libst_menu.so libst_menu.a include/st_menu.h
	@printf "\nBuilding: $@...\n"
	$(CC) demo/demo.c -o demoapp_sl $(UNICODE_OBJ) -lst_menu $(PDCURSES_STATIC_LIB) -Wall $(ST_LIBDIRS) $(LDLIBS) $(ST_DEPLIBS) $(ST_INCDIRS) $(ST_LIBDIRS) $(NCURSES_PANEL_LIB) $(CFLAGS)

simple: demo/simple.c libst_menu.a include/st_menu.h
	@printf "\nBuilding: $@...\n"
	$(CC) demo/simple.c -o simple libst_menu.a $(PDCURSES_STATIC_LIB) -Wall $(ST_LIBDIRS) $(LDLIBS) $(ST_DEPLIBS) $(ST_INCDIRS) $(CFLAGS)

simple2: demo/simple2.c libst_menu.a include/st_menu.h
	@printf "\nBuilding: $@...\n"
	$(CC) demo/simple2.c -o simple2 libst_menu.a $(PDCURSES_STATIC_LIB) -Wall $(ST_LIBDIRS) $(LDLIBS) $(ST_DEPLIBS) $(ST_INCDIRS) $(CFLAGS)

post_build:
ifeq "$(BUILD_OS)" "windows"
	test -f $(PDCURSES_LIBDIR)/$(PDCURSES_LIB).dll && cp $(PDCURSES_LIBDIR)/$(PDCURSES_LIB).dll . || true
endif

st_menu.pc.install:
	tools/install.sh data st_menu.pc $(PKG_CONFIG_PATH)

install: libst_menu.so libst_menu.a $(PKG_CONFIG_TARGET)
	tools/install.sh data include/st_menu.h $(INCLUDEDIR)
	tools/install.sh bin libst_menu.so $(LIBDIR)
	tools/install.sh bin libst_menu.a $(LIBDIR)

clean:
	rm -f *.o *.a *.so 
	test -f demoapp$(PROG_EXT) && rm demoapp$(PROG_EXT) || true
	test -f demoapp_sl$(PROG_EXT) && rm demoapp_sl$(PROG_EXT) || true
	test -f simple$(PROG_EXT) && rm simple$(PROG_EXT) || true
	test -f simple2$(PROG_EXT) && rm simple2$(PROG_EXT) || true
ifeq "$(BUILD_OS)" "windows"
	test -f $(PDCURSES_LIB).dll && rm $(PDCURSES_LIB).dll || true
endif

cleanall: clean
	rm -f *.file config.log config.make config.status st_menu.pc *.awk configure

.PHONY: clean cleanall
