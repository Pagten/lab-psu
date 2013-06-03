#################   Project options  #################
# Project name
#  This will be the filename of the resulting elf
#  file.
PROJECTNAME=dac-test


#################     GCC options    #################
# Microcontroller name for gcc
#  See "AVR Options" in the avr-gcc main manual for
#  the list of supported MCU types.
GCC_MCU=atmega328

# Project source directory
SRC_DIR= ./

# Project source files within the source directory
SRC_FILES= rotary.c dac-test.c
# Libraries to link with
LIBS=

# Optimization level, 
#  Available options: s (size), 1, 2, 3 or 0 (off)
OPT=s


#################  AVR Dude options  #################
# Microcontroller name for avrdude
#  See the avrdude manual for the list of supported
#  MCU types.
AVRDUDE_MCU=m328

# Name of the programmer used to program the MCU
#  See the avrdude manual for the lst of supported
#  programmers.
AVRDUDE_PROGRAMMER=usbasp

# Serial or parallel port to which the programmer is
# attached. If the programmer is connected through
# USB, this value is not used.
AVRDUDE_PORT=/dev/ttyS0


################ End of configuration ################
#####     No need to edit beyond this point      #####

## Toolchain executables
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
AVRDUDE=avrdude
REMOVE=rm -f

## Output filename
TRG=$(PROJECTNAME).elf
HEXROMTRG=$(PROJECTNAME).hex
HEXTRG=$(HEXROMTRG) $(PROJECTNAME).ee.hex

## Compiler flags
CFLAGS= -std=c99 -I$(SRC_DIR) -mmcu=$(GCC_MCU) -O$(OPT)  \
	-fpack-struct -fshort-enums             \
	-funsigned-bitfields -funsigned-char    \
	-Wall -Wstrict-prototypes

## Linker flags
LDFLAGS=-Wl,-Map,$(TRG).map \
        -mmcu=$(GCC_MCU) $(LIBS)

## Project source files
SRC=$(addprefix $(SRC_DIR),$(SRC_FILES))
CFILES=$(filter %.c, $(SRC))

## Object files to create
OBJDEPS=$(CFILES:.c=.o)


## Make targets:
.SUFFIXES : .c .o .elf .hex .ee.hex .h

.PHONY: all stats hex upload clean

all: $(TRG)

stats: $(TRG)
	$(SIZE) -C --mcu=$(GCC_MCU) $(TRG) 

hex: $(HEXTRG)

upload: hex
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMER)       \
	 -p $(AVRDUDE_MCU) -P $(AVRDUDE_PORT) -e \
	 -U flash:w:$(HEXROMTRG)

$(TRG): $(OBJDEPS) 
	$(CC) -o $(TRG) $(OBJDEPS) $(LDFLAGS)


## How to generate object files from c sources
.c.o: 
	$(CC) $(CFLAGS) -c $< -o $@

## How to generate hex files (ROM and EEPROM) from an
## elf
HEXFORMAT=ihex

.elf.hex:
	$(OBJCOPY) -j .text                    \
		-j .data                       \
		-O $(HEXFORMAT) $< $@

.elf.ee.hex:
	$(OBJCOPY) -j .eeprom                  \
		--change-section-lma .eeprom=0 \
		-O $(HEXFORMAT) $< $@


## Cleanup
clean:
	$(REMOVE) $(TRG) $(TRG).map
	$(REMOVE) $(OBJDEPS)
	$(REMOVE) $(HEXTRG)

