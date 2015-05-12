# EPL: SP2015
# LifeForm Makefile with FLTK support
# Author: EPL TA

# Mac OS X users please follow the instructions to setup FLTK.
# 1. Install FLTK through MacPorts using the command:
#    sudo port install fltk
#
# 2. Change FLTK_DIR, FLTK_LIB, and LIBS of this Makefile:
#    FLTK_DIR=/opt/local
#    FLTK_LIB=$(FLTK_DIR)/lib/libfltk.a
#    LIBS = $(FLTK_LIB) -lm -ldl -lpthread -framework Cocoa
#

# For Manual Installation (e.g., Windows)
FLTK_DIR=../../../examples/fltk
# For MacPorts Installation (Mac OS X)
# FLTK_DIR=/opt/local

FLTK_INC=-I$(FLTK_DIR)

#choose based on system
FLTK_LIB=$(FLTK_DIR)/lib/fltk64.a #class virtual machine uses this
#FLTK_LIB=$(FLTK_DIR)/lib/libfltk.a # Mac OS X + MacPorts uses this

IFLAGS =
DFLAGS = -DDEBUG=0 -DNO_WINDOW=0 -DSPECIES_SUMMARY=1 -DALGAE_SPORES=1 -DRANDOM=0 -DSLOWDOWN=0 -DUSE_GC=0
CXX = g++ --std=c++11 $(FLTK_INC)
CC  = $(GCC)
GCC = g++ --std=c++11 $(FLTK_INC)
LD  = $(CXX)

LIBS = $(FLTK_LIB) -lX11 -lm -ldl -lpthread
#LIBS = $(FLTK_LIB) -lm -ldl -lpthread -framework Cocoa # Mac OS X uses this

WFLAGS = -Wall
SYMFLAGS = -g

PROFILE = #-pg
OPTFLAGS =#-O
CFLAGS = $(OPTFLAGS) $(PROFILE) $(WFLAGS) $(IFLAGS) $(SYMFLAGS)
CXXFLAGS = $(CFLAGS)
CPPFLAGS = $(IFLAGS) $(DFLAGS)
LDFLAGS = $(PROFILE) -g

PROGRAM = animals
#CXXSRCS = LifeForm.cpp animals.cpp Window.cpp Event.cpp Algae.cpp \
          Craig.cpp LifeForm-Craig.cpp Params.cpp Praveen.cpp Jerry.cpp

CXXSRCS = $(shell ls *.cpp)
CSRCS =

SRCS = $(CXXSRCS) $(CSRCS)

OBJS = $(CXXSRCS:.cpp=.o) $(CSRCS:.c=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) yh7483.o bx522.o yl23394.o Yz7962.o Jeremy64.o $(LIBS)

test: $(PROGRAM)
	./$(PROGRAM)

clean:
	-rm -f $(OBJS) $(PROGRAM) .*.d

ifneq ($(strip $(CSRCS)),)
.%.d: %.c
	$(SHELL) -ec '$(GCC) -MM $(CPPFLAGS) $< > $@'

include $(CSRCS:%.c=.%.d)
endif

ifneq ($(strip $(CXXSRCS)),)
.%.d: %.cpp
	$(SHELL) -ec '$(GCC) -MM $(CPPFLAGS) $< > $@'

include $(CXXSRCS:%.cpp=.%.d)
endif

