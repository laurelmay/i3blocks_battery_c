EXE=battery

CC ?= gcc
override CFLAGS := -O3 -Wall -Wextra --std=c99 -pedantic $(CFLAGS) $$(pkg-config --cflags libnotify gtk+-3.0)
CPPFLAGS=
LDFLAGS=-O3 $$(pkg-config --libs libnotify gtk+-3.0)
INSTALL=install
INSTALLFLAGS=-C -D
DESTDIR=
PREFIX=$(HOME)/.local

SRCS=main.c util.c battery.c
OBJS=$(SRCS:.c=.o)

.PHONY: default
default: all

#debug: CFLAGS += -g -O0
#debug: all

.PHONY: all
all: $(EXE)

$(EXE): main.o $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXE) $^ $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(EXE) main.o $(OBJS)

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) $(INSTALLFLAGS) $(EXE) $(DESTDIR)$(PREFIX)/bin/$(EXE)

.PHONY: all default clean install
