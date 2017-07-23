EXE=battery
MODS=battery.o util.o

CC=gcc
CFLAGS=-g -O3 -Wall -Wextra --std=c99 -pedantic $$(pkg-config --cflags libnotify gtk+-3.0)
CPPFLAGS=
LDFLAGS=-g -O3 $$(pkg-config --libs libnotify gtk+-3.0)
INSTALL=install
INSTALLFLAGS=-C -D
DESTDIR=
PREFIX=$(HOME)/.local

.PHONY: default
default: $(EXE)

$(EXE): main.o $(MODS) $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXE) $^ $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(EXE) main.o $(MODS)

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) $(INSTALLFLAGS) $(EXE) $(DESTDIR)$(PREFIX)/bin/$(EXE)

