PROJECT_NAME = tests

# Source files
HAL_SOURCEFILES = timer2.c spi.c
UTIL_SOURCEFILES = ring_buffer.c
TEST_SOURCEFILES = rotary_test.c scheduler_test.c spi_master_test.c 
SOURCEDIRS = hal util
SOURCEFILES = $(HAL_SOURCEFILES) $(UTIL_SOURCEFILES) $(TEST_SOURCEFILES)

# Target config
CUSTOM_TARGET = 1
CC = gcc
LD = gcc
CFLAGS = `pkg-config --cflags check`
LIBS   = `pkg-config --libs check` -lpthread


all: $(PROJECT_NAME)


FW_ROOT = ..
include $(FW_ROOT)/Makefile.include

