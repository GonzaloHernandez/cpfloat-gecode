# Unix makefile for sound math functions

CC = gcc
#CFLAGS = -Wall -fno-builtin

# added -O0 to make sure optimization is off, probably not necessary, but
# let's make sure  --dkw  2005-10-24
CFLAGS = -g -O0

AR = ar
RANLIB = ranlib
#INCS = IA.h smath.h "../clip.h"
AS = as

OBJS = narrow.o sound_ops.o IA.o\
       narrow_mult.o \
       narrow_trig.o trig2pi.o IAsincos.o IAasincos.o \
       narrow_bool.o \
       exp.o IAexp.o power.o

all: libsmath.a

libsmath.a: $(OBJS) $(INCS)
	$(AR) rv libsmath.a $(OBJS)
	$(RANLIB) libsmath.a
