TARGET=battery

CC=gcc
CFLAGS=-g -O3 -Wall -Wextra --std=c99 -pedantic $$(pkg-config --cflags libnotify gtk+-3.0)
CPPFLAGS=
LDFLAGS=-g -O3 $$(pkg-config --libs libnotify gtk+-3.0)
INSTALL=install
INSTALLFLAGS=-C -D
DESTDIR=
PREFIX=$(HOME)/.local

SRCS=main.c util.c battery.c
OBJS=$(SRCS:.c=.o)

.PHONY: default
default: $(TARGET)

$(TARGET): main.o $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $^ $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) main.o $(OBJS)

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) $(INSTALLFLAGS) $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

