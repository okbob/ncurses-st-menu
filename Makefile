all: libst_menu.so libst_menu.a simple demoapp demoapp_sl

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

INCDIRS += $(PDCURSES_INCDIR) include .
LIBDIRS += $(PDCURSES_LIBDIR) .
DEPLIBS += $(PDCURSES_DEP_LIBS)

ifeq ($(HAVE_PDCURSES),yes)
	PDCURSES_STATIC_LIB = $(PDCURSES_LIBDIR)/lib$(PDCURSES_LIB).a
	PDCURSES_DYN_LIB = $(PDCURSES_LIBDIR)/lib$(PDCURSES_LIB).so
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
	@printf "\nBuilding: $@...\n"
	$(CC) -shared -Wl,-soname,libst_menu.so -o libst_menu.so st_menu.o st_menu_styles.o $(PDCURSES_DYN_LIB) $(UNICODE_OBJ) $(ST_INCDIRS) $(CFLAGS)

libst_menu.a: st_menu_styles.o st_menu.o $(UNICODE_OBJ)
	@printf "\nBuilding: $@...\n"
	$(AR) rcs libst_menu.a st_menu_styles.o st_menu.o $(UNICODE_OBJ)

demoapp: demo/demo.c libst_menu.so libst_menu.a include/st_menu.h
	@printf "\nBuilding: $@...\n"
	$(CC) demo/demo.c -o demoapp libst_menu.a $(PDCURSES_STATIC_LIB) -Wall $(ST_LIBDIRS) $(LDLIBS) $(ST_DEPLIBS) $(ST_INCDIRS) $(CFLAGS)

demoapp_sl: demo/demo.c libst_menu.so libst_menu.a include/st_menu.h
	@printf "\nBuilding: $@...\n"
	$(CC) demo/demo.c -o demoapp_sl $(UNICODE_OBJ) $(PDCURSES_STATIC_LIB) -Wall $(ST_LIBDIRS) $(LDLIBS) $(ST_DEPLIBS) $(ST_INCDIRS) $(ST_LIBDIRS) -lst_menu $(NCURSES_PANEL_LIB) $(CFLAGS)

simple: demo/simple.c libst_menu.a include/st_menu.h
	@printf "\nBuilding: $@...\n"
	$(CC) demo/simple.c -o simple libst_menu.a $(PDCURSES_STATIC_LIB) -Wall $(LDLIBS) $(ST_DEPLIBS) $(ST_INCDIRS) $(CFLAGS)

st_menu.pc.install:
	tools/install.sh data st_menu.pc $(PKG_CONFIG_PATH)

install: libst_menu.so libst_menu.a $(PKG_CONFIG_TARGET)
	tools/install.sh data include/st_menu.h $(INCLUDEDIR)
	tools/install.sh bin libst_menu.so $(LIBDIR)
	tools/install.sh bin libst_menu.a $(LIBDIR)

clean:
	rm -f *.o *.a *.so 
	test -f demoapp && rm demoapp || true
	test -f demoapp_sl && rm demoapp_sl || true
	test -f simple && rm simple || true

cleanconfig:
	rm -f *.file config.log config.make config.status st_menu.pc *.awk
