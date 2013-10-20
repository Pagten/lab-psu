
GCC_DEVICE_TYPE = atmega328

CC = gcc
INCLUDES = $(TESTINCLUDES) -Isrc
CFLAGS = -std=c99
LIBS =
LDFLAGS =

SRC = src/scheduler.c

all: test

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) *.o

-include test/Makefile.inc
