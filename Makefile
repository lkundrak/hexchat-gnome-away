gio_CFLAGS = $(shell pkg-config --cflags gio-2.0)
gio_CFLAGS += -DGLIB_VERSION_MIN_REQUIRED=GLIB_VERSION_2_28
gio_CFLAGS += -DGLIB_VERSION_MAX_ALLOWED=GLIB_VERSION_2_28
gio_LIBS = $(shell pkg-config --libs gio-2.0)
hexchat_CFLAGS = $(shell pkg-config --cflags hexchat-plugin)
hexchat_LIBS = $(shell pkg-config --libs hexchat-plugin)
module_CFLAGS = -fPIC
module_LDFLAGS = -shared

HEXCHATLIBDIR = $(shell pkg-config --variable=hexchatlibdir hexchat-plugin)
LOCALHEXCHATLIBDIR = $(HOME)/.config/hexchat/addons

CFLAGS = -g -Wall
override CFLAGS += $(gio_CFLAGS)
override LDFLAGS += $(gio_LIBS)
override CFLAGS += $(hexchat_CFLAGS)
override LDFLAGS += $(hexchat_LIBS)
override CFLAGS += $(module_CFLAGS)
override LDFLAGS += $(module_LIBS)

all: hexchat-gnome-away.so

%.so: %.o
	$(CC) -shared -o $@ $<

install: hexchat-gnome-away.so
	mkdir -p $(DESTDIR)$(HEXCHATLIBDIR)/
	install hexchat-gnome-away.so $(DESTDIR)$(HEXCHATLIBDIR)/

uninstall:
	rm $(DESTDIR)$(HEXCHATLIBDIR)/hexchat-gnome-away.so

install-local: hexchat-gnome-away.so
	mkdir -p $(DESTDIR)$(LOCALHEXCHATLIBDIR)/
	install hexchat-gnome-away.so $(DESTDIR)$(LOCALHEXCHATLIBDIR)/

uninstall-local:
	rm $(DESTDIR)$(LOCALHEXCHATLIBDIR)/hexchat-gnome-away.so
clean:
	rm -f *.o *.so
