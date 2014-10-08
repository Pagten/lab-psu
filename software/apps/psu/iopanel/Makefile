PROJECT_NAME = psu-iopanel
all: $(PROJECT_NAME)

#OPTI=0
#NO_LTO=1

DEBUG=1

LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm

FW_ROOT = ../../..
include $(FW_ROOT)/Makefile.include

