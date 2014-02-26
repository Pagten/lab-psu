PROJECT_NAME = tests

# Source files
HAL_SOURCEFILES = mock_timer.c mock_timers.c spi.c #timer2.c spi.c
UTIL_SOURCEFILES = ring_buffer.c
TEST_SOURCEFILES = clock_test.c timer_test.c spi_master_test.c rotary_test.c \
	mcp4922_test.c process_test.c
SOURCEDIRS = hal util
SOURCEFILES = $(HAL_SOURCEFILES) $(UTIL_SOURCEFILES) $(TEST_SOURCEFILES)

# Target config
CUSTOM_TARGET = 1
CC = gcc
LD = gcc
CFLAGS = -g `pkg-config --cflags check`
LIBS   = `pkg-config --libs check` -lpthread

NO_LTO = 1

all: $(PROJECT_NAME)


FW_ROOT = ..
include $(FW_ROOT)/Makefile.include

