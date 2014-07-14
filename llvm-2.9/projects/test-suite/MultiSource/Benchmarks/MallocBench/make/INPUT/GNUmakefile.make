# Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
# This file is part of GNU Make.
# 
# GNU Make is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# GNU Make is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Make; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

#
#	Makefile for GNU Make
#

CFLAGS = $(defines) -g
LDFLAGS = -g

# Define these for your system as follows:
# 	-DUSG			System V
#	-DUSGr3			SVR3 (also define USG)
#	-DHPUX			HP-UX (also define USG and USGr3 appropriately)
#	-DHAVE_SYS_WAIT		USG, but have <sys/wait.h> and wait3
#	-DHAVE_SIGLIST		USG, but have sys_siglist
#	-DHAVE_DUP2		USG and not USGr3, but have dup2
#	-DNO_MINUS_C_MINUS_O	cc can't handle "cc -c foo.c -o foo.o"
#	-DPOSIX			A 1003.1 system (or trying to be)
#	-DNO_ARCHIVES		To disable `ar' archive support.
#	-DNO_FLOAT		To avoid using floating-point numbers.
#	-DENUM_BITFIELDS	If the compiler isn't GCC but groks enum foo:2.
#				Some compilers apparently accept this
#				without complaint but produce losing code,
#				so beware.
# NeXT 1.0a uses an old version of GCC, which required -D__inline=inline.
defines =

# Define these for your system as follows:
#	-DUMAX		Encore UMAX
#	-DUMAX_43	Encore UMAX 4.3 (also define UMAX)
#	-DNO_LDAV	Disable load-average checking.
# To read /dev/kmem (most Unix systems), define these if different from the
# given defaults:
#	-DKERNEL_FILE_NAME=\"/vmunix\"
#	-DLDAV_SYMBOL=\"_avenrun\"
#	-DLDAV_CVT="(double) load"
# Define:
#	-DNLIST_NAME_UNION	If `struct nlist' has a n_un member.
#	-DNLIST_NAME_ARRAY	If `n_name' is an array.
LOAD_AVG =

# If you don't want archive support, comment these out.
ARCHIVES = arscan.o ar.o
ARCHIVES_SRC = arscan.c ar.c

# If your system needs extra libraries loaded in, define them here.
# System V probably need -lPW for alloca.  HP-UX 7.0's alloca in
# libPW.a is broken on HP9000s300 and HP9000s400 machines.  Use
# alloca.c (below) instead on those machines.  SGI Irix needs -lmld
# for nlist.
LOADLIBES =

# If your system doesn't have alloca, or the one provided is bad,
# get it from the Emacs distribution and define these.
#ALLOCA = alloca.o
#ALLOCASRC = alloca.c

# If there are remote execution facilities defined,
# enable them with switches here (see remote-*.c).
REMOTE =

# Any extra object files your system needs.
extras =

# Comment this out if POSIX.2 glob is installed on your system
# (it's in the GNU C Library, so if you're using that, this is
# not needed at all.)
globdep = glob/libglob.a

# Library containing POSIX.2 `glob' function.
# Comment this line out if it's in the C library (which is the case if you
# are using the GNU C Library), or change it to the appropriate file name
# or -l switch.
globlib = $(globdep)

# Name under which to install GNU make.
instname = make
# Directory to install `make' in.
bindir = $(prefix)/usr/local/bin
# Directory to install the man page in.
mandir = $(prefix)/usr/local/man/manl
# Number to put on the man page filename.
manext = l

# Install make setgid to this group so it can read /dev/kmem.
group = kmem

objs = commands.o job.o dir.o file.o load.o misc.o main.o read.o \
       remake.o remote.o rule.o implicit.o default.o variable.o expand.o \
       function.o vpath.o version.o $(ARCHIVES) $(ALLOCA) $(extras)
srcs = commands.c job.c dir.c file.c load.c misc.c main.c read.c \
       remake.c remote.c rule.c implicit.c default.c variable.c expand.c \
       function.c vpath.c version.c $(ALLOCASRC) $(ARCHIVES_SRC) \
       commands.h dep.h file.h job.h make.h rule.h variable.h

BASETARGET=make
OBJS = $(objs)
EXTRALIBS = glob/libglob.a
#
# This is the prototype makefile for building the memory-intensive
# applications and linking them with different malloc implementations.
#


SHELL=csh

C++=/srl/Gcc2/bin/g++
CC = gcc

OFILES = $(OBJS)

# possible values of CTYPE: simple (default)

ifeq ($(CTYPE),)
CTYPE=simple
CEXT=
endif

# possible values of ALLOC: decos (default), knuth, bsd, gnu,
#			mmalloc, mmalloc4, mmalloc16, mmalloc8, mmalloc32
#			gpp, cm, cminline, qf, cache, scache
#			bwgc, bwgc2.0, bwgc2.1
##			ff,rv
ifeq ($(ALLOC),)
ALLOC = decos
endif
# possible values of USEROPT: TRUE, FALSE
ifeq ($(USEROPT),)
USEROPT = FALSE
endif

# possible values of MEAS: NONE (default)
ifeq ($(MEAS),)
MEAS = NONE
endif

# need to add an object file if using the customalloc code
ifeq ($(ALLOC),cm)
OBJS := $(OBJS) customalloc.o
endif

ifeq ($(ALLOC),cm16)
OBJS := $(OBJS) customalloc16.o
endif

ifeq ($(ALLOC),cm32)
OBJS := $(OBJS) customalloc32.o
endif

ifeq ($(ALLOC),cm8)
OBJS := $(OBJS) customalloc8.o
endif

# need to add an object file if using the customalloc code
ifeq ($(ALLOC),cminline)
CFLAGS := -I. $(CFLAGS) -D__INCLUDE_CUSTOMALLOC_H__=\"customalloc.h\"
OBJS := $(OBJS) customalloc.o
else 
#CFLAGS := $(CFLAGS) -D__INCLUDE_CUSTOMALLOC_H__=\<ctype.h\>
endif

#BASE = /homes/zorn/work/m
#ALLOCBASE = /homes/zorn/work/m/dmm/lib
BASE =      /cs/research/gc/dec
ALLOCBASE = /cs/research/gc/dec/dmm/lib
MISCLIB = $(ALLOCBASE)/misc.a

CONSUMER_DIR = $(BASE)/apps/consumers

vpath	%.c .:$(CONSUMER_DIR)

#BASEFLAGS = -g -O -Winline
BASEFLAGS = -g
ALLOCEXT = -decos
#REDEFINES = -Dcalloc=bZc -Dmalloc=bZa -Drealloc=bZr -Dfree=bZf

CMINLINEOBJS = $(OBJS:.o=-cmi.o)

%-cmi.o : %.c customalloc.h
	$(CC) -DNOMEMOPT $(CFLAGS) -o $@ -c $<

NOOPTOBJS = $(OBJS:.o=-noopt.o)

%-noopt.o : %.c
	$(CC) -DNOMEMOPT $(CFLAGS) -o $@ -c $<

%-noopt.o : %.cc
	$(C++) -DNOMEMOPT $(CFLAGS) -o $@ -c $<

BWGCOBJS = $(OBJS:.o=-bwgc.o)

%-bwgc.o : %.c
	$(CC) -DNOMEMOPT -DIGNOREFREE -DBWGC $(CFLAGS) -o $@ -c $<

# configuration for nomemopt compilation (now obsolete, instead use NOPT)

ifeq ($(USEROPT),FALSE)
OFILES = $(NOOPTOBJS)
USEROPTEXT =
endif

ifeq ($(USEROPT),TRUE)
OFILES = $(NOOPTOBJS)
USEROPTEXT = -uopt
endif

# configuration for GC assisted compilation

ifeq ($(USEROPT),BWGC)
OFILES = $(BWGCOBJS)
ARCHEXT = -bwgc.a
ALLOCEXT = -bwgc
ALLOCLIB = 
endif

ifeq ($(MEAS),NONE)
MEASEXT = 
endif

# possible allocators

ifeq ($(ALLOC),mmalloc)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),mmalloc4)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),mmalloc16)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),mmalloc32)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),mmalloc8)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),knuth)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),krv)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),bsd)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),gnu)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),bwgc2.0)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),bwgc)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),bwgc2.1)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),gpp)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),ff)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),rv)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),qf)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),qf32)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),qf32c)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),qfc)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),cache)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),scache)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = $(ALLOCBASE)/$(ALLOC)$(MEASEXT).a
endif

ifeq ($(ALLOC),cm)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = 
endif

ifeq ($(ALLOC),cm16)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = 
endif

ifeq ($(ALLOC),cm32)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = 
endif

ifeq ($(ALLOC),cm8)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = 
endif

ifeq ($(ALLOC),cminline)
OFILES = $(CMINLINEOBJS)
ALLOCCFLAGS = $(REDEFINES)
ALLOCEXT = -$(ALLOC)
ALLOCLIB = 
endif

TARGET := $(BASETARGET)$(ALLOCEXT)$(USEROPTEXT)$(MEASEXT).exe

realall: $(TARGET)

OTHERFLAGS = $(BASEFLAGS) $(MEASFLAGS) $(ALLOCFLAGS)

CFLAGS := $(CFLAGS) $(OTHERFLAGS) $(APPCFLAGS)

	
$(TARGET): $(OFILES) $(ALLOCLIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(OFILES) $(ALLOCLIB) $(MISCLIB) $(EXTRALIBS) -lm 

oclean:
	rm -f $(OBJS) $(NOOPTOBJS) $(BWGCOBJS)

xclean:
	rm -f *.exe

customalloc16.c: CUSTOMDATA-16
	customalloc -f CUSTOMDATA-16 -g -o customalloc16 >& CUSTOMALLOC-16-OUTPUT

customalloc8.c: CUSTOMDATA-8
	customalloc -f CUSTOMDATA-8 -g -o customalloc8 >& CUSTOMALLOC-8-OUTPUT

customalloc32.c: CUSTOMDATA-32
	customalloc -f CUSTOMDATA-32 -g -o customalloc32 >& CUSTOMALLOC-32-OUTPUT

QPT = $(HOME)/.dec-mips/bin/qpt

$(TARGET).qp: $(TARGET)
	$(QPT) -t $^

ifeq ($(CTYPE),simple)
CONSUMER_OBJS =  $(CONSUMER_DIR)/adtr.o
endif

CREATED_HDRS = qpt_forward_decls.h
CREATED_SRCS = $(wildcard $(TARGET)_sma*.c) 
CREATED_OBJS = $(subst .c,.o, $(CREATED_SRCS))
CREATED_OTHERS = $(TARGET).Trace

$(TARGET).$(CEXT)cns: $(TARGET) $(CONSUMER_OBJS) $(CREATED_OBJS)
	$(CC) -o $@ $(CONSUMER_OBJS) $(CREATED_OBJS)


.SUFFIXES:
.SUFFIXES: .o .c .h .ps .dvi .texinfo

.PHONY: all doc
all: make
doc: make.info make.dvi


# Take your pick.
#makeinfo = emacs -batch make.texinfo -f texinfo-format-buffer -f save-buffer
makeinfo = makeinfo make.texinfo

make.info: make.texinfo
	$(makeinfo)


make.dvi: make.texinfo
	-tex make.texinfo
	texindex make.cp make.fn make.ky make.pg make.tp make.vr
	-tex make.texinfo

make.ps: make.dvi
	dvi2ps make.dvi > make.ps

make: $(objs) $(globdep)
	$(CC) $(LDFLAGS) $(objs) $(globlib) $(LOADLIBES) -o make.new
	mv -f make.new make

load.o: load.c
	$(CC) $(CFLAGS) $(LOAD_AVG) -c load.c
remote.o: remote.c
	$(CC) $(CFLAGS) $(REMOTE) -c remote.c

# For some losing Unix makes.
MAKE = make

glob/libglob.a: force
	cd glob; $(MAKE) CC='$(CC)' CFLAGS='$(CFLAGS) -I..' libglob.a
force:

TAGS: $(srcs)
	etags -tw $(srcs)
tags: $(srcs)
	ctags -tw $(srcs)

.PHONY: install
install: $(bindir)/$(instname) $(mandir)/$(instname).$(manext)

$(bindir)/$(instname): make
	cp make $@.new
# These are necessary for load-average checking to work on most Unix machines.
	chgrp $(group) $@.new
	chmod g+s $@.new
	mv $@.new $@

$(mandir)/$(instname).$(manext): make.man
	cp make.man $@

.PHONY: clean realclean
clean: glob-clean
	-rm -f make *.o core
realclean: clean glob-realclean
	-rm -f TAGS tags make.info* make-* make.dvi *~
	-rm -f make.?? make.??s make.log make.toc make.*aux

.PHONY: glob-clean glob-realclean
glob-clean glob-realclean:
	cd glob; $(MAKE) $@

# Automatically generated dependencies will be put at the end of the file.

# Automatically generated dependencies.
commands.o : commands.c make.h dep.h commands.h file.h variable.h job.h 
job.o : job.c make.h commands.h job.h file.h variable.h 
dir.o : dir.c make.h 
file.o : file.c make.h commands.h dep.h file.h variable.h 
load.o : load.c make.h commands.h job.h 
misc.o : misc.c make.h dep.h 
main.o : main.c make.h commands.h dep.h file.h variable.h job.h 
read.o : read.c make.h commands.h dep.h file.h variable.h 
remake.o : remake.c make.h commands.h job.h dep.h file.h 
remote.o : remote.c remote-stub.c make.h commands.h 
rule.o : rule.c make.h commands.h dep.h file.h variable.h rule.h 
implicit.o : implicit.c make.h rule.h dep.h file.h 
default.o : default.c make.h rule.h dep.h file.h commands.h variable.h 
variable.o : variable.c make.h commands.h variable.h dep.h file.h 
expand.o : expand.c make.h commands.h file.h variable.h 
function.o : function.c make.h variable.h dep.h commands.h job.h 
vpath.o : vpath.c make.h file.h variable.h 
version.o : version.c 
arscan.o : arscan.c 
ar.o : ar.c make.h file.h 
