PROJECT_NAME = tests
FW_ROOT = ..

# Source files
HAL_SOURCEFILES = gpio.c mock_timer.c mock_timers.c spi.c #timer2.c spi.c
UTIL_SOURCEFILES = ring_buffer.c crc16.c
TEST_SOURCEFILES = clock_test.c timer_test.c spi_master_test.c rotary_test.c \
	mcp4922_test.c process_test.c pwlf_test.c
SOURCEDIRS = hal util
SOURCEFILES = $(HAL_SOURCEFILES) $(UTIL_SOURCEFILES) $(TEST_SOURCEFILES)

# Target config
F_CPU = 16000000UL
CUSTOM_TARGET = 1
CC = gcc
LD = gcc
CFLAGS = -g `pkg-config --cflags check` -DF_CPU=$(F_CPU) -I$(FW_ROOT)/test
LIBS   = `pkg-config --libs check` -lpthread

OPTI = 0
NO_LTO = 1

all: $(PROJECT_NAME)

include $(FW_ROOT)/Makefile.include

