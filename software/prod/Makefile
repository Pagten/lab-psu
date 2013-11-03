
GCC_DEVICE_TYPE = atmega328

CC = gcc
INCLUDES = $(TESTINCLUDES) -Isrc
CFLAGS = -g -std=c99
LIBS =
LDFLAGS =

BUILD_DIR = build
SRC = src/scheduler.c src/spi_master.c

all: test

$(BUILD_DIR)/%.o: %.c
	mkdir -p `dirname $@`
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)/*

-include test/Makefile.inc
