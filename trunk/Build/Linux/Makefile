CXXFLAGS = -g -O2 -Wall -I$(MGALIB_ROOT)/Include -I$(MGALIB_ROOT)/Include/utf8/source -I$(MGALIB_ROOT)
# LDFLAGS += -Wl,--gc-sections
AR = ar
ARFLAGS = -crs 
CP = cp
MKDIR = mkdir
EGREP = egrep

## Architecture settings

UNAME := $(shell uname)
ISLINUX := $(shell uname -o | $(EGREP) -c -i "linux")
ISX86 := $(shell uname -m | $(EGREP) -c "i.86|x86|i86|amd64")

ifeq ($(ISX86),0) 
$(error Intel X86 arcitectures are supported only.)
endif

ifeq ($(ISLINUX),0) 
$(error Linux operating systems (32/64 bit) are supported only.)
endif

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
  INTEL_LINUX_64 = 1
else
  INTEL_LINUX_32 = 1
endif

## Project settings
MGALIB_ROOT := ../..
BINDIR := $(MGALIB_ROOT)/Bin

CORE_SRC_DIR := $(MGALIB_ROOT)/Core
CORE_LIB := $(BINDIR)/libcore.a
CORE_SOURCES := $(wildcard $(CORE_SRC_DIR)/*.cpp)
CORE_HEADERS := $(wildcard $(CORE_SRC_DIR)/*.h)
CORE_OBJS := $(CORE_SOURCES:.cpp=.o)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

$(CORE_LIB) : $(CORE_OBJS)
	$(AR) $(ARFLAGS) $@ $(CORE_OBJS)

all : $(CORE_LIB)

.PHONY: all clean

