# $RCSfile$
#
# $Log$
# Revision 1.1  2004/02/17 22:22:39  criswell
# Adding GNU Makefile Malloc Benchmark input files.
#
# Revision 4.0.1.1  91/04/11  17:30:39  lwall
# patch1: C flags are now settable on a per-file basis
# 
# Revision 4.0  91/03/20  00:58:54  lwall
# 4.0 baseline.
# 
# 

YACC = /bin/yacc
bin = /usr/local/bin
scriptdir = /usr/local/bin
privlib = /usr/local/lib/perl
mansrc = /usr/man/manl
manext = l
LDFLAGS = 
CLDFLAGS = 
SMALL = 
LARGE =  
#mallocsrc = malloc.c
#mallocobj = malloc.o
SLN = ln -s

libs = -lm 

public = perl taintperl 


#CFLAGS = `sh cflags.SH $@`
CFLAGS = $(OTHERCFLAGS)

private = 

scripts = h2ph

MAKE = make

manpages = perl.man h2ph.man

util =

sh = Makefile.SH makedepend.SH h2ph.SH

h1 = EXTERN.h INTERN.h arg.h array.h cmd.h config.h form.h handy.h
h2 = hash.h perl.h regcomp.h regexp.h spat.h stab.h str.h util.h

h = $(h1) $(h2)

c1 = array.c cmd.c cons.c consarg.c doarg.c doio.c dolist.c dump.c
c2 = eval.c form.c hash.c $(mallocsrc) perl.c regcomp.c regexec.c
c3 = stab.c str.c toke.c util.c usersub.c

c = $(c1) $(c2) $(c3)

obj1 = array.o cmd.o cons.o consarg.o doarg.o doio.o dolist.o dump.o
obj2 = eval.o form.o hash.o $(mallocobj) perl.o regcomp.o regexec.o
obj3 = stab.o str.o toke.o util.o

obj = $(obj1) $(obj2) $(obj3)

tobj1 = tarray.o tcmd.o tcons.o tconsarg.o tdoarg.o tdoio.o tdolist.o tdump.o
tobj2 = teval.o tform.o thash.o $(mallocobj) tregcomp.o tregexec.o
tobj3 = tstab.o tstr.o ttoke.o tutil.o

tobj = $(tobj1) $(tobj2) $(tobj3)

lintflags = -hbvxac

addedbyconf = Makefile.old bsd eunice filexp loc pdp11 usg v7

# grrr
SHELL = /bin/sh

.c.o:
	$(CC) -c $(CFLAGS) $*.c

#all: $(public) $(private) $(util) uperl.o $(scripts)
#	cd x2p; $(MAKE) all
#	touch all

# This is the standard version that contains no "taint" checks and is
# used for all scripts that aren't set-id or running under something set-id.
# The $& notation is tells Sequent machines that it can do a parallel make,
# and is harmless otherwise.

BASETARGET = perl
OBJS = perly.o $(obj) usersub.o
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

    

uperl.o: $& perly.o $(obj)
	-ld $(LARGE) $(LDFLAGS) -r $(obj) perly.o $(libs) -o uperl.o

saber: perly.c
	# load $(c) perly.c

# This version, if specified in Configure, does ONLY those scripts which need
# set-id emulation.  Suidperl must be setuid root.  It contains the "taint"
# checks as well as the special code to validate that the script in question
# has been invoked correctly.

suidperl: $& tperly.o sperl.o $(tobj) usersub.o
	$(CC) $(LARGE) $(CLDFLAGS) sperl.o $(tobj) tperly.o usersub.o $(libs) \
	    -o suidperl

# This version interprets scripts that are already set-id either via a wrapper
# or through the kernel allowing set-id scripts (bad idea).  Taintperl must
# NOT be setuid to root or anything else.  The only difference between it
# and normal perl is the presence of the "taint" checks.

taintperl: $& tperly.o tperl.o $(tobj) usersub.o
	$(CC) $(LARGE) $(CLDFLAGS) tperl.o $(tobj) tperly.o usersub.o $(libs) \
	    -o taintperl

# Replicating all this junk is yucky, but I don't see a portable way to fix it.

tperly.o: perly.c perly.h $(h)
	/bin/rm -f tperly.c
	$(SLN) perly.c tperly.c
	$(CC) -c -DTAINT $(CFLAGS) tperly.c
	/bin/rm -f tperly.c

tperl.o: perl.c perly.h patchlevel.h perl.h $(h)
	/bin/rm -f tperl.c
	$(SLN) perl.c tperl.c
	$(CC) -c -DTAINT $(CFLAGS) tperl.c
	/bin/rm -f tperl.c

sperl.o: perl.c perly.h patchlevel.h $(h)
	/bin/rm -f sperl.c
	$(SLN) perl.c sperl.c
	$(CC) -c -DTAINT -DIAMSUID $(CFLAGS) sperl.c
	/bin/rm -f sperl.c

tarray.o: array.c $(h)
	/bin/rm -f tarray.c
	$(SLN) array.c tarray.c
	$(CC) -c -DTAINT $(CFLAGS) tarray.c
	/bin/rm -f tarray.c

tcmd.o: cmd.c $(h)
	/bin/rm -f tcmd.c
	$(SLN) cmd.c tcmd.c
	$(CC) -c -DTAINT $(CFLAGS) tcmd.c
	/bin/rm -f tcmd.c

tcons.o: cons.c $(h) perly.h
	/bin/rm -f tcons.c
	$(SLN) cons.c tcons.c
	$(CC) -c -DTAINT $(CFLAGS) tcons.c
	/bin/rm -f tcons.c

tconsarg.o: consarg.c $(h)
	/bin/rm -f tconsarg.c
	$(SLN) consarg.c tconsarg.c
	$(CC) -c -DTAINT $(CFLAGS) tconsarg.c
	/bin/rm -f tconsarg.c

tdoarg.o: doarg.c $(h)
	/bin/rm -f tdoarg.c
	$(SLN) doarg.c tdoarg.c
	$(CC) -c -DTAINT $(CFLAGS) tdoarg.c
	/bin/rm -f tdoarg.c

tdoio.o: doio.c $(h)
	/bin/rm -f tdoio.c
	$(SLN) doio.c tdoio.c
	$(CC) -c -DTAINT $(CFLAGS) tdoio.c
	/bin/rm -f tdoio.c

tdolist.o: dolist.c $(h)
	/bin/rm -f tdolist.c
	$(SLN) dolist.c tdolist.c
	$(CC) -c -DTAINT $(CFLAGS) tdolist.c
	/bin/rm -f tdolist.c

tdump.o: dump.c $(h)
	/bin/rm -f tdump.c
	$(SLN) dump.c tdump.c
	$(CC) -c -DTAINT $(CFLAGS) tdump.c
	/bin/rm -f tdump.c

teval.o: eval.c $(h)
	/bin/rm -f teval.c
	$(SLN) eval.c teval.c
	$(CC) -c -DTAINT $(CFLAGS) teval.c
	/bin/rm -f teval.c

tform.o: form.c $(h)
	/bin/rm -f tform.c
	$(SLN) form.c tform.c
	$(CC) -c -DTAINT $(CFLAGS) tform.c
	/bin/rm -f tform.c

thash.o: hash.c $(h)
	/bin/rm -f thash.c
	$(SLN) hash.c thash.c
	$(CC) -c -DTAINT $(CFLAGS) thash.c
	/bin/rm -f thash.c

tregcomp.o: regcomp.c $(h)
	/bin/rm -f tregcomp.c
	$(SLN) regcomp.c tregcomp.c
	$(CC) -c -DTAINT $(CFLAGS) tregcomp.c
	/bin/rm -f tregcomp.c

tregexec.o: regexec.c $(h)
	/bin/rm -f tregexec.c
	$(SLN) regexec.c tregexec.c
	$(CC) -c -DTAINT $(CFLAGS) tregexec.c
	/bin/rm -f tregexec.c

tstab.o: stab.c $(h)
	/bin/rm -f tstab.c
	$(SLN) stab.c tstab.c
	$(CC) -c -DTAINT $(CFLAGS) tstab.c
	/bin/rm -f tstab.c

tstr.o: str.c $(h) perly.h
	/bin/rm -f tstr.c
	$(SLN) str.c tstr.c
	$(CC) -c -DTAINT $(CFLAGS) tstr.c
	/bin/rm -f tstr.c

ttoke.o: toke.c $(h) perly.h
	/bin/rm -f ttoke.c
	$(SLN) toke.c ttoke.c
	$(CC) -c -DTAINT $(CFLAGS) ttoke.c
	/bin/rm -f ttoke.c

tutil.o: util.c $(h)
	/bin/rm -f tutil.c
	$(SLN) util.c tutil.c
	$(CC) -c -DTAINT $(CFLAGS) tutil.c
	/bin/rm -f tutil.c

perly.h: perly.c
	@ echo Dummy dependency for dumb parallel make
	touch perly.h

perly.c: perly.y
	@ echo 'Expect either' 29 shift/reduce and 59 reduce/reduce conflicts...
	@ echo '           or' 27 shift/reduce and 61 reduce/reduce conflicts...
	$(YACC) -d perly.y
	sh perly.fixer y.tab.c perly.c
	mv y.tab.h perly.h
	echo 'extern YYSTYPE yylval;' >>perly.h

perly.o: perly.c perly.h $(h)
	$(CC) -c $(CFLAGS) perly.c

install: all
	./perl installperl
	cd x2p; $(MAKE) install

clean:
	rm -f *.o all perl taintperl suidperl
	cd x2p; $(MAKE) clean

realclean: clean
	cd x2p; $(MAKE) realclean
	rm -f *.orig */*.orig *~ */*~ core $(addedbyconf) h2ph h2ph.man
	rm -f perly.c perly.h t/perl Makefile config.h makedepend makedir
	rm -f x2p/Makefile

# The following lint has practically everything turned on.  Unfortunately,
# you have to wade through a lot of mumbo jumbo that can't be suppressed.
# If the source file has a /*NOSTRICT*/ somewhere, ignore the lint message
# for that spot.

lint: perly.c $(c)
	lint $(lintflags) $(defs) perly.c $(c) > perl.fuzz

depend: makedepend
	- test -f perly.h || cp /dev/null perly.h
	./makedepend
	- test -s perly.h || /bin/rm -f perly.h
	cd x2p; $(MAKE) depend

test: perl
	- cd t && chmod +x TEST */*.t
	- cd t && (rm -f perl; $(SLN) ../perl .) && ./perl TEST </dev/tty

clist:
	echo $(c) | tr ' ' '\012' >.clist

hlist:
	echo $(h) | tr ' ' '\012' >.hlist

shlist:
	echo $(sh) | tr ' ' '\012' >.shlist

# AUTOMATICALLY GENERATED MAKE DEPENDENCIES--PUT NOTHING BELOW THIS LINE
# If this runs make out of memory, delete /usr/include lines.
array.o: /usr/include/ctype.h
array.o: /usr/include/dirent.h
array.o: /usr/include/errno.h
array.o: /usr/include/machine/param.h
array.o: /usr/include/machine/setjmp.h
array.o: /usr/include/ndbm.h
array.o: /usr/include/netinet/in.h
array.o: /usr/include/setjmp.h
array.o: /usr/include/stdio.h
array.o: /usr/include/stdlib.h
array.o: /usr/include/string.h
array.o: /usr/include/sys/dirent.h
array.o: /usr/include/sys/errno.h
array.o: /usr/include/sys/filio.h
array.o: /usr/include/sys/ioccom.h
array.o: /usr/include/sys/ioctl.h
array.o: /usr/include/sys/param.h
array.o: /usr/include/sys/signal.h
array.o: /usr/include/sys/sockio.h
array.o: /usr/include/sys/stat.h
array.o: /usr/include/sys/stdtypes.h
array.o: /usr/include/sys/sysmacros.h
array.o: /usr/include/sys/time.h
array.o: /usr/include/sys/times.h
array.o: /usr/include/sys/ttold.h
array.o: /usr/include/sys/ttychars.h
array.o: /usr/include/sys/ttycom.h
array.o: /usr/include/sys/ttydev.h
array.o: /usr/include/sys/types.h
array.o: /usr/include/time.h
array.o: /usr/include/vm/faultcode.h
array.o: EXTERN.h
array.o: arg.h
array.o: array.c
array.o: array.h
array.o: cmd.h
array.o: config.h
array.o: form.h
array.o: handy.h
array.o: hash.h
array.o: perl.h
array.o: regexp.h
array.o: spat.h
array.o: stab.h
array.o: str.h
array.o: util.h
cmd.o: /usr/include/ctype.h
cmd.o: /usr/include/dirent.h
cmd.o: /usr/include/errno.h
cmd.o: /usr/include/machine/param.h
cmd.o: /usr/include/machine/setjmp.h
cmd.o: /usr/include/ndbm.h
cmd.o: /usr/include/netinet/in.h
cmd.o: /usr/include/setjmp.h
cmd.o: /usr/include/stdio.h
cmd.o: /usr/include/stdlib.h
cmd.o: /usr/include/string.h
cmd.o: /usr/include/sys/dirent.h
cmd.o: /usr/include/sys/errno.h
cmd.o: /usr/include/sys/filio.h
cmd.o: /usr/include/sys/ioccom.h
cmd.o: /usr/include/sys/ioctl.h
cmd.o: /usr/include/sys/param.h
cmd.o: /usr/include/sys/signal.h
cmd.o: /usr/include/sys/sockio.h
cmd.o: /usr/include/sys/stat.h
cmd.o: /usr/include/sys/stdtypes.h
cmd.o: /usr/include/sys/sysmacros.h
cmd.o: /usr/include/sys/time.h
cmd.o: /usr/include/sys/times.h
cmd.o: /usr/include/sys/ttold.h
cmd.o: /usr/include/sys/ttychars.h
cmd.o: /usr/include/sys/ttycom.h
cmd.o: /usr/include/sys/ttydev.h
cmd.o: /usr/include/sys/types.h
cmd.o: /usr/include/time.h
cmd.o: /usr/include/vm/faultcode.h
cmd.o: /usr/local/lib/gcc-include/varargs.h
cmd.o: EXTERN.h
cmd.o: arg.h
cmd.o: array.h
cmd.o: cmd.c
cmd.o: cmd.h
cmd.o: config.h
cmd.o: form.h
cmd.o: handy.h
cmd.o: hash.h
cmd.o: perl.h
cmd.o: regexp.h
cmd.o: spat.h
cmd.o: stab.h
cmd.o: str.h
cmd.o: util.h
cons.o: /usr/include/ctype.h
cons.o: /usr/include/dirent.h
cons.o: /usr/include/errno.h
cons.o: /usr/include/machine/param.h
cons.o: /usr/include/machine/setjmp.h
cons.o: /usr/include/ndbm.h
cons.o: /usr/include/netinet/in.h
cons.o: /usr/include/setjmp.h
cons.o: /usr/include/stdio.h
cons.o: /usr/include/stdlib.h
cons.o: /usr/include/string.h
cons.o: /usr/include/sys/dirent.h
cons.o: /usr/include/sys/errno.h
cons.o: /usr/include/sys/filio.h
cons.o: /usr/include/sys/ioccom.h
cons.o: /usr/include/sys/ioctl.h
cons.o: /usr/include/sys/param.h
cons.o: /usr/include/sys/signal.h
cons.o: /usr/include/sys/sockio.h
cons.o: /usr/include/sys/stat.h
cons.o: /usr/include/sys/stdtypes.h
cons.o: /usr/include/sys/sysmacros.h
cons.o: /usr/include/sys/time.h
cons.o: /usr/include/sys/times.h
cons.o: /usr/include/sys/ttold.h
cons.o: /usr/include/sys/ttychars.h
cons.o: /usr/include/sys/ttycom.h
cons.o: /usr/include/sys/ttydev.h
cons.o: /usr/include/sys/types.h
cons.o: /usr/include/time.h
cons.o: /usr/include/vm/faultcode.h
cons.o: EXTERN.h
cons.o: arg.h
cons.o: array.h
cons.o: cmd.h
cons.o: config.h
cons.o: cons.c
cons.o: form.h
cons.o: handy.h
cons.o: hash.h
cons.o: perl.h
cons.o: perly.h
cons.o: regexp.h
cons.o: spat.h
cons.o: stab.h
cons.o: str.h
cons.o: util.h
consarg.o: /usr/include/ctype.h
consarg.o: /usr/include/dirent.h
consarg.o: /usr/include/errno.h
consarg.o: /usr/include/machine/param.h
consarg.o: /usr/include/machine/setjmp.h
consarg.o: /usr/include/ndbm.h
consarg.o: /usr/include/netinet/in.h
consarg.o: /usr/include/setjmp.h
consarg.o: /usr/include/stdio.h
consarg.o: /usr/include/stdlib.h
consarg.o: /usr/include/string.h
consarg.o: /usr/include/sys/dirent.h
consarg.o: /usr/include/sys/errno.h
consarg.o: /usr/include/sys/filio.h
consarg.o: /usr/include/sys/ioccom.h
consarg.o: /usr/include/sys/ioctl.h
consarg.o: /usr/include/sys/param.h
consarg.o: /usr/include/sys/signal.h
consarg.o: /usr/include/sys/sockio.h
consarg.o: /usr/include/sys/stat.h
consarg.o: /usr/include/sys/stdtypes.h
consarg.o: /usr/include/sys/sysmacros.h
consarg.o: /usr/include/sys/time.h
consarg.o: /usr/include/sys/times.h
consarg.o: /usr/include/sys/ttold.h
consarg.o: /usr/include/sys/ttychars.h
consarg.o: /usr/include/sys/ttycom.h
consarg.o: /usr/include/sys/ttydev.h
consarg.o: /usr/include/sys/types.h
consarg.o: /usr/include/time.h
consarg.o: /usr/include/vm/faultcode.h
consarg.o: EXTERN.h
consarg.o: arg.h
consarg.o: array.h
consarg.o: cmd.h
consarg.o: config.h
consarg.o: consarg.c
consarg.o: form.h
consarg.o: handy.h
consarg.o: hash.h
consarg.o: perl.h
consarg.o: regexp.h
consarg.o: spat.h
consarg.o: stab.h
consarg.o: str.h
consarg.o: util.h
doarg.o: /usr/include/ctype.h
doarg.o: /usr/include/dirent.h
doarg.o: /usr/include/errno.h
doarg.o: /usr/include/machine/param.h
doarg.o: /usr/include/machine/setjmp.h
doarg.o: /usr/include/ndbm.h
doarg.o: /usr/include/netinet/in.h
doarg.o: /usr/include/setjmp.h
doarg.o: /usr/include/stdio.h
doarg.o: /usr/include/stdlib.h
doarg.o: /usr/include/string.h
doarg.o: /usr/include/sys/dirent.h
doarg.o: /usr/include/sys/errno.h
doarg.o: /usr/include/sys/filio.h
doarg.o: /usr/include/sys/ioccom.h
doarg.o: /usr/include/sys/ioctl.h
doarg.o: /usr/include/sys/param.h
doarg.o: /usr/include/sys/signal.h
doarg.o: /usr/include/sys/sockio.h
doarg.o: /usr/include/sys/stat.h
doarg.o: /usr/include/sys/stdtypes.h
doarg.o: /usr/include/sys/sysmacros.h
doarg.o: /usr/include/sys/time.h
doarg.o: /usr/include/sys/times.h
doarg.o: /usr/include/sys/ttold.h
doarg.o: /usr/include/sys/ttychars.h
doarg.o: /usr/include/sys/ttycom.h
doarg.o: /usr/include/sys/ttydev.h
doarg.o: /usr/include/sys/types.h
doarg.o: /usr/include/time.h
doarg.o: /usr/include/vm/faultcode.h
doarg.o: EXTERN.h
doarg.o: arg.h
doarg.o: array.h
doarg.o: cmd.h
doarg.o: config.h
doarg.o: doarg.c
doarg.o: form.h
doarg.o: handy.h
doarg.o: hash.h
doarg.o: perl.h
doarg.o: regexp.h
doarg.o: spat.h
doarg.o: stab.h
doarg.o: str.h
doarg.o: util.h
doio.o: /usr/include/ctype.h
doio.o: /usr/include/dirent.h
doio.o: /usr/include/errno.h
doio.o: /usr/include/grp.h
doio.o: /usr/include/machine/mmu.h
doio.o: /usr/include/machine/param.h
doio.o: /usr/include/machine/setjmp.h
doio.o: /usr/include/ndbm.h
doio.o: /usr/include/netdb.h
doio.o: /usr/include/netinet/in.h
doio.o: /usr/include/pwd.h
doio.o: /usr/include/setjmp.h
doio.o: /usr/include/stdio.h
doio.o: /usr/include/stdlib.h
doio.o: /usr/include/string.h
doio.o: /usr/include/sys/dirent.h
doio.o: /usr/include/sys/errno.h
doio.o: /usr/include/sys/fcntlcom.h
doio.o: /usr/include/sys/file.h
doio.o: /usr/include/sys/filio.h
doio.o: /usr/include/sys/ioccom.h
doio.o: /usr/include/sys/ioctl.h
doio.o: /usr/include/sys/ipc.h
doio.o: /usr/include/sys/msg.h
doio.o: /usr/include/sys/param.h
doio.o: /usr/include/sys/sem.h
doio.o: /usr/include/sys/shm.h
doio.o: /usr/include/sys/signal.h
doio.o: /usr/include/sys/socket.h
doio.o: /usr/include/sys/sockio.h
doio.o: /usr/include/sys/stat.h
doio.o: /usr/include/sys/stdtypes.h
doio.o: /usr/include/sys/sysmacros.h
doio.o: /usr/include/sys/time.h
doio.o: /usr/include/sys/times.h
doio.o: /usr/include/sys/ttold.h
doio.o: /usr/include/sys/ttychars.h
doio.o: /usr/include/sys/ttycom.h
doio.o: /usr/include/sys/ttydev.h
doio.o: /usr/include/sys/types.h
doio.o: /usr/include/time.h
doio.o: /usr/include/utime.h
doio.o: /usr/include/vm/faultcode.h
doio.o: EXTERN.h
doio.o: arg.h
doio.o: array.h
doio.o: cmd.h
doio.o: config.h
doio.o: doio.c
doio.o: form.h
doio.o: handy.h
doio.o: hash.h
doio.o: perl.h
doio.o: regexp.h
doio.o: spat.h
doio.o: stab.h
doio.o: str.h
doio.o: util.h
dolist.o: /usr/include/ctype.h
dolist.o: /usr/include/dirent.h
dolist.o: /usr/include/errno.h
dolist.o: /usr/include/machine/param.h
dolist.o: /usr/include/machine/setjmp.h
dolist.o: /usr/include/ndbm.h
dolist.o: /usr/include/netinet/in.h
dolist.o: /usr/include/setjmp.h
dolist.o: /usr/include/stdio.h
dolist.o: /usr/include/stdlib.h
dolist.o: /usr/include/string.h
dolist.o: /usr/include/sys/dirent.h
dolist.o: /usr/include/sys/errno.h
dolist.o: /usr/include/sys/filio.h
dolist.o: /usr/include/sys/ioccom.h
dolist.o: /usr/include/sys/ioctl.h
dolist.o: /usr/include/sys/param.h
dolist.o: /usr/include/sys/signal.h
dolist.o: /usr/include/sys/sockio.h
dolist.o: /usr/include/sys/stat.h
dolist.o: /usr/include/sys/stdtypes.h
dolist.o: /usr/include/sys/sysmacros.h
dolist.o: /usr/include/sys/time.h
dolist.o: /usr/include/sys/times.h
dolist.o: /usr/include/sys/ttold.h
dolist.o: /usr/include/sys/ttychars.h
dolist.o: /usr/include/sys/ttycom.h
dolist.o: /usr/include/sys/ttydev.h
dolist.o: /usr/include/sys/types.h
dolist.o: /usr/include/time.h
dolist.o: /usr/include/vm/faultcode.h
dolist.o: EXTERN.h
dolist.o: arg.h
dolist.o: array.h
dolist.o: cmd.h
dolist.o: config.h
dolist.o: dolist.c
dolist.o: form.h
dolist.o: handy.h
dolist.o: hash.h
dolist.o: perl.h
dolist.o: regexp.h
dolist.o: spat.h
dolist.o: stab.h
dolist.o: str.h
dolist.o: util.h
dump.o: /usr/include/ctype.h
dump.o: /usr/include/dirent.h
dump.o: /usr/include/errno.h
dump.o: /usr/include/machine/param.h
dump.o: /usr/include/machine/setjmp.h
dump.o: /usr/include/ndbm.h
dump.o: /usr/include/netinet/in.h
dump.o: /usr/include/setjmp.h
dump.o: /usr/include/stdio.h
dump.o: /usr/include/stdlib.h
dump.o: /usr/include/string.h
dump.o: /usr/include/sys/dirent.h
dump.o: /usr/include/sys/errno.h
dump.o: /usr/include/sys/filio.h
dump.o: /usr/include/sys/ioccom.h
dump.o: /usr/include/sys/ioctl.h
dump.o: /usr/include/sys/param.h
dump.o: /usr/include/sys/signal.h
dump.o: /usr/include/sys/sockio.h
dump.o: /usr/include/sys/stat.h
dump.o: /usr/include/sys/stdtypes.h
dump.o: /usr/include/sys/sysmacros.h
dump.o: /usr/include/sys/time.h
dump.o: /usr/include/sys/times.h
dump.o: /usr/include/sys/ttold.h
dump.o: /usr/include/sys/ttychars.h
dump.o: /usr/include/sys/ttycom.h
dump.o: /usr/include/sys/ttydev.h
dump.o: /usr/include/sys/types.h
dump.o: /usr/include/time.h
dump.o: /usr/include/vm/faultcode.h
dump.o: EXTERN.h
dump.o: arg.h
dump.o: array.h
dump.o: cmd.h
dump.o: config.h
dump.o: dump.c
dump.o: form.h
dump.o: handy.h
dump.o: hash.h
dump.o: perl.h
dump.o: regexp.h
dump.o: spat.h
dump.o: stab.h
dump.o: str.h
dump.o: util.h
eval.o: /usr/include/ctype.h
eval.o: /usr/include/dirent.h
eval.o: /usr/include/errno.h
eval.o: /usr/include/machine/param.h
eval.o: /usr/include/machine/setjmp.h
eval.o: /usr/include/ndbm.h
eval.o: /usr/include/netinet/in.h
eval.o: /usr/include/setjmp.h
eval.o: /usr/include/stdio.h
eval.o: /usr/include/stdlib.h
eval.o: /usr/include/string.h
eval.o: /usr/include/sys/dirent.h
eval.o: /usr/include/sys/errno.h
eval.o: /usr/include/sys/fcntlcom.h
eval.o: /usr/include/sys/file.h
eval.o: /usr/include/sys/filio.h
eval.o: /usr/include/sys/ioccom.h
eval.o: /usr/include/sys/ioctl.h
eval.o: /usr/include/sys/param.h
eval.o: /usr/include/sys/signal.h
eval.o: /usr/include/sys/sockio.h
eval.o: /usr/include/sys/stat.h
eval.o: /usr/include/sys/stdtypes.h
eval.o: /usr/include/sys/sysmacros.h
eval.o: /usr/include/sys/time.h
eval.o: /usr/include/sys/times.h
eval.o: /usr/include/sys/ttold.h
eval.o: /usr/include/sys/ttychars.h
eval.o: /usr/include/sys/ttycom.h
eval.o: /usr/include/sys/ttydev.h
eval.o: /usr/include/sys/types.h
eval.o: /usr/include/time.h
eval.o: /usr/include/vfork.h
eval.o: /usr/include/vm/faultcode.h
eval.o: EXTERN.h
eval.o: arg.h
eval.o: array.h
eval.o: cmd.h
eval.o: config.h
eval.o: eval.c
eval.o: form.h
eval.o: handy.h
eval.o: hash.h
eval.o: perl.h
eval.o: regexp.h
eval.o: spat.h
eval.o: stab.h
eval.o: str.h
eval.o: util.h
form.o: /usr/include/ctype.h
form.o: /usr/include/dirent.h
form.o: /usr/include/errno.h
form.o: /usr/include/machine/param.h
form.o: /usr/include/machine/setjmp.h
form.o: /usr/include/ndbm.h
form.o: /usr/include/netinet/in.h
form.o: /usr/include/setjmp.h
form.o: /usr/include/stdio.h
form.o: /usr/include/stdlib.h
form.o: /usr/include/string.h
form.o: /usr/include/sys/dirent.h
form.o: /usr/include/sys/errno.h
form.o: /usr/include/sys/filio.h
form.o: /usr/include/sys/ioccom.h
form.o: /usr/include/sys/ioctl.h
form.o: /usr/include/sys/param.h
form.o: /usr/include/sys/signal.h
form.o: /usr/include/sys/sockio.h
form.o: /usr/include/sys/stat.h
form.o: /usr/include/sys/stdtypes.h
form.o: /usr/include/sys/sysmacros.h
form.o: /usr/include/sys/time.h
form.o: /usr/include/sys/times.h
form.o: /usr/include/sys/ttold.h
form.o: /usr/include/sys/ttychars.h
form.o: /usr/include/sys/ttycom.h
form.o: /usr/include/sys/ttydev.h
form.o: /usr/include/sys/types.h
form.o: /usr/include/time.h
form.o: /usr/include/vm/faultcode.h
form.o: EXTERN.h
form.o: arg.h
form.o: array.h
form.o: cmd.h
form.o: config.h
form.o: form.c
form.o: form.h
form.o: handy.h
form.o: hash.h
form.o: perl.h
form.o: regexp.h
form.o: spat.h
form.o: stab.h
form.o: str.h
form.o: util.h
hash.o: /usr/include/ctype.h
hash.o: /usr/include/dirent.h
hash.o: /usr/include/errno.h
hash.o: /usr/include/machine/param.h
hash.o: /usr/include/machine/setjmp.h
hash.o: /usr/include/ndbm.h
hash.o: /usr/include/netinet/in.h
hash.o: /usr/include/setjmp.h
hash.o: /usr/include/stdio.h
hash.o: /usr/include/stdlib.h
hash.o: /usr/include/string.h
hash.o: /usr/include/sys/dirent.h
hash.o: /usr/include/sys/errno.h
hash.o: /usr/include/sys/fcntlcom.h
hash.o: /usr/include/sys/file.h
hash.o: /usr/include/sys/filio.h
hash.o: /usr/include/sys/ioccom.h
hash.o: /usr/include/sys/ioctl.h
hash.o: /usr/include/sys/param.h
hash.o: /usr/include/sys/signal.h
hash.o: /usr/include/sys/sockio.h
hash.o: /usr/include/sys/stat.h
hash.o: /usr/include/sys/stdtypes.h
hash.o: /usr/include/sys/sysmacros.h
hash.o: /usr/include/sys/time.h
hash.o: /usr/include/sys/times.h
hash.o: /usr/include/sys/ttold.h
hash.o: /usr/include/sys/ttychars.h
hash.o: /usr/include/sys/ttycom.h
hash.o: /usr/include/sys/ttydev.h
hash.o: /usr/include/sys/types.h
hash.o: /usr/include/time.h
hash.o: /usr/include/vm/faultcode.h
hash.o: EXTERN.h
hash.o: arg.h
hash.o: array.h
hash.o: cmd.h
hash.o: config.h
hash.o: form.h
hash.o: handy.h
hash.o: hash.c
hash.o: hash.h
hash.o: perl.h
hash.o: regexp.h
hash.o: spat.h
hash.o: stab.h
hash.o: str.h
hash.o: util.h
malloc.o: /usr/include/ctype.h
malloc.o: /usr/include/dirent.h
malloc.o: /usr/include/errno.h
malloc.o: /usr/include/machine/param.h
malloc.o: /usr/include/machine/setjmp.h
malloc.o: /usr/include/ndbm.h
malloc.o: /usr/include/netinet/in.h
malloc.o: /usr/include/setjmp.h
malloc.o: /usr/include/stdio.h
malloc.o: /usr/include/stdlib.h
malloc.o: /usr/include/string.h
malloc.o: /usr/include/sys/dirent.h
malloc.o: /usr/include/sys/errno.h
malloc.o: /usr/include/sys/filio.h
malloc.o: /usr/include/sys/ioccom.h
malloc.o: /usr/include/sys/ioctl.h
malloc.o: /usr/include/sys/param.h
malloc.o: /usr/include/sys/signal.h
malloc.o: /usr/include/sys/sockio.h
malloc.o: /usr/include/sys/stat.h
malloc.o: /usr/include/sys/stdtypes.h
malloc.o: /usr/include/sys/sysmacros.h
malloc.o: /usr/include/sys/time.h
malloc.o: /usr/include/sys/times.h
malloc.o: /usr/include/sys/ttold.h
malloc.o: /usr/include/sys/ttychars.h
malloc.o: /usr/include/sys/ttycom.h
malloc.o: /usr/include/sys/ttydev.h
malloc.o: /usr/include/sys/types.h
malloc.o: /usr/include/time.h
malloc.o: /usr/include/vm/faultcode.h
malloc.o: EXTERN.h
malloc.o: arg.h
malloc.o: array.h
malloc.o: cmd.h
malloc.o: config.h
malloc.o: form.h
malloc.o: handy.h
malloc.o: hash.h
malloc.o: malloc.c
malloc.o: perl.h
malloc.o: regexp.h
malloc.o: spat.h
malloc.o: stab.h
malloc.o: str.h
malloc.o: util.h
perl.o: /usr/include/ctype.h
perl.o: /usr/include/dirent.h
perl.o: /usr/include/errno.h
perl.o: /usr/include/machine/param.h
perl.o: /usr/include/machine/setjmp.h
perl.o: /usr/include/ndbm.h
perl.o: /usr/include/netinet/in.h
perl.o: /usr/include/setjmp.h
perl.o: /usr/include/stdio.h
perl.o: /usr/include/stdlib.h
perl.o: /usr/include/string.h
perl.o: /usr/include/sys/dirent.h
perl.o: /usr/include/sys/errno.h
perl.o: /usr/include/sys/filio.h
perl.o: /usr/include/sys/ioccom.h
perl.o: /usr/include/sys/ioctl.h
perl.o: /usr/include/sys/param.h
perl.o: /usr/include/sys/signal.h
perl.o: /usr/include/sys/sockio.h
perl.o: /usr/include/sys/stat.h
perl.o: /usr/include/sys/stdtypes.h
perl.o: /usr/include/sys/sysmacros.h
perl.o: /usr/include/sys/time.h
perl.o: /usr/include/sys/times.h
perl.o: /usr/include/sys/ttold.h
perl.o: /usr/include/sys/ttychars.h
perl.o: /usr/include/sys/ttycom.h
perl.o: /usr/include/sys/ttydev.h
perl.o: /usr/include/sys/types.h
perl.o: /usr/include/time.h
perl.o: /usr/include/vm/faultcode.h
perl.o: EXTERN.h
perl.o: arg.h
perl.o: array.h
perl.o: cmd.h
perl.o: config.h
perl.o: form.h
perl.o: handy.h
perl.o: hash.h
perl.o: patchlevel.h
perl.o: perl.c
perl.o: perl.h
perl.o: perly.h
perl.o: regexp.h
perl.o: spat.h
perl.o: stab.h
perl.o: str.h
perl.o: util.h
regcomp.o: /usr/include/ctype.h
regcomp.o: /usr/include/dirent.h
regcomp.o: /usr/include/errno.h
regcomp.o: /usr/include/machine/param.h
regcomp.o: /usr/include/machine/setjmp.h
regcomp.o: /usr/include/ndbm.h
regcomp.o: /usr/include/netinet/in.h
regcomp.o: /usr/include/setjmp.h
regcomp.o: /usr/include/stdio.h
regcomp.o: /usr/include/stdlib.h
regcomp.o: /usr/include/string.h
regcomp.o: /usr/include/sys/dirent.h
regcomp.o: /usr/include/sys/errno.h
regcomp.o: /usr/include/sys/filio.h
regcomp.o: /usr/include/sys/ioccom.h
regcomp.o: /usr/include/sys/ioctl.h
regcomp.o: /usr/include/sys/param.h
regcomp.o: /usr/include/sys/signal.h
regcomp.o: /usr/include/sys/sockio.h
regcomp.o: /usr/include/sys/stat.h
regcomp.o: /usr/include/sys/stdtypes.h
regcomp.o: /usr/include/sys/sysmacros.h
regcomp.o: /usr/include/sys/time.h
regcomp.o: /usr/include/sys/times.h
regcomp.o: /usr/include/sys/ttold.h
regcomp.o: /usr/include/sys/ttychars.h
regcomp.o: /usr/include/sys/ttycom.h
regcomp.o: /usr/include/sys/ttydev.h
regcomp.o: /usr/include/sys/types.h
regcomp.o: /usr/include/time.h
regcomp.o: /usr/include/vm/faultcode.h
regcomp.o: EXTERN.h
regcomp.o: INTERN.h
regcomp.o: arg.h
regcomp.o: array.h
regcomp.o: cmd.h
regcomp.o: config.h
regcomp.o: form.h
regcomp.o: handy.h
regcomp.o: hash.h
regcomp.o: perl.h
regcomp.o: regcomp.c
regcomp.o: regcomp.h
regcomp.o: regexp.h
regcomp.o: spat.h
regcomp.o: stab.h
regcomp.o: str.h
regcomp.o: util.h
regexec.o: /usr/include/ctype.h
regexec.o: /usr/include/dirent.h
regexec.o: /usr/include/errno.h
regexec.o: /usr/include/machine/param.h
regexec.o: /usr/include/machine/setjmp.h
regexec.o: /usr/include/ndbm.h
regexec.o: /usr/include/netinet/in.h
regexec.o: /usr/include/setjmp.h
regexec.o: /usr/include/stdio.h
regexec.o: /usr/include/stdlib.h
regexec.o: /usr/include/string.h
regexec.o: /usr/include/sys/dirent.h
regexec.o: /usr/include/sys/errno.h
regexec.o: /usr/include/sys/filio.h
regexec.o: /usr/include/sys/ioccom.h
regexec.o: /usr/include/sys/ioctl.h
regexec.o: /usr/include/sys/param.h
regexec.o: /usr/include/sys/signal.h
regexec.o: /usr/include/sys/sockio.h
regexec.o: /usr/include/sys/stat.h
regexec.o: /usr/include/sys/stdtypes.h
regexec.o: /usr/include/sys/sysmacros.h
regexec.o: /usr/include/sys/time.h
regexec.o: /usr/include/sys/times.h
regexec.o: /usr/include/sys/ttold.h
regexec.o: /usr/include/sys/ttychars.h
regexec.o: /usr/include/sys/ttycom.h
regexec.o: /usr/include/sys/ttydev.h
regexec.o: /usr/include/sys/types.h
regexec.o: /usr/include/time.h
regexec.o: /usr/include/vm/faultcode.h
regexec.o: EXTERN.h
regexec.o: arg.h
regexec.o: array.h
regexec.o: cmd.h
regexec.o: config.h
regexec.o: form.h
regexec.o: handy.h
regexec.o: hash.h
regexec.o: perl.h
regexec.o: regcomp.h
regexec.o: regexec.c
regexec.o: regexp.h
regexec.o: spat.h
regexec.o: stab.h
regexec.o: str.h
regexec.o: util.h
stab.o: /usr/include/ctype.h
stab.o: /usr/include/dirent.h
stab.o: /usr/include/errno.h
stab.o: /usr/include/machine/param.h
stab.o: /usr/include/machine/setjmp.h
stab.o: /usr/include/ndbm.h
stab.o: /usr/include/netinet/in.h
stab.o: /usr/include/setjmp.h
stab.o: /usr/include/stdio.h
stab.o: /usr/include/stdlib.h
stab.o: /usr/include/string.h
stab.o: /usr/include/sys/dirent.h
stab.o: /usr/include/sys/errno.h
stab.o: /usr/include/sys/filio.h
stab.o: /usr/include/sys/ioccom.h
stab.o: /usr/include/sys/ioctl.h
stab.o: /usr/include/sys/param.h
stab.o: /usr/include/sys/signal.h
stab.o: /usr/include/sys/sockio.h
stab.o: /usr/include/sys/stat.h
stab.o: /usr/include/sys/stdtypes.h
stab.o: /usr/include/sys/sysmacros.h
stab.o: /usr/include/sys/time.h
stab.o: /usr/include/sys/times.h
stab.o: /usr/include/sys/ttold.h
stab.o: /usr/include/sys/ttychars.h
stab.o: /usr/include/sys/ttycom.h
stab.o: /usr/include/sys/ttydev.h
stab.o: /usr/include/sys/types.h
stab.o: /usr/include/time.h
stab.o: /usr/include/vm/faultcode.h
stab.o: EXTERN.h
stab.o: arg.h
stab.o: array.h
stab.o: cmd.h
stab.o: config.h
stab.o: form.h
stab.o: handy.h
stab.o: hash.h
stab.o: perl.h
stab.o: regexp.h
stab.o: spat.h
stab.o: stab.c
stab.o: stab.h
stab.o: str.h
stab.o: util.h
str.o: /usr/include/ctype.h
str.o: /usr/include/dirent.h
str.o: /usr/include/errno.h
str.o: /usr/include/machine/param.h
str.o: /usr/include/machine/setjmp.h
str.o: /usr/include/ndbm.h
str.o: /usr/include/netinet/in.h
str.o: /usr/include/setjmp.h
str.o: /usr/include/stdio.h
str.o: /usr/include/stdlib.h
str.o: /usr/include/string.h
str.o: /usr/include/sys/dirent.h
str.o: /usr/include/sys/errno.h
str.o: /usr/include/sys/filio.h
str.o: /usr/include/sys/ioccom.h
str.o: /usr/include/sys/ioctl.h
str.o: /usr/include/sys/param.h
str.o: /usr/include/sys/signal.h
str.o: /usr/include/sys/sockio.h
str.o: /usr/include/sys/stat.h
str.o: /usr/include/sys/stdtypes.h
str.o: /usr/include/sys/sysmacros.h
str.o: /usr/include/sys/time.h
str.o: /usr/include/sys/times.h
str.o: /usr/include/sys/ttold.h
str.o: /usr/include/sys/ttychars.h
str.o: /usr/include/sys/ttycom.h
str.o: /usr/include/sys/ttydev.h
str.o: /usr/include/sys/types.h
str.o: /usr/include/time.h
str.o: /usr/include/vm/faultcode.h
str.o: EXTERN.h
str.o: arg.h
str.o: array.h
str.o: cmd.h
str.o: config.h
str.o: form.h
str.o: handy.h
str.o: hash.h
str.o: perl.h
str.o: perly.h
str.o: regexp.h
str.o: spat.h
str.o: stab.h
str.o: str.c
str.o: str.h
str.o: util.h
toke.o: /usr/include/ctype.h
toke.o: /usr/include/dirent.h
toke.o: /usr/include/errno.h
toke.o: /usr/include/machine/param.h
toke.o: /usr/include/machine/setjmp.h
toke.o: /usr/include/ndbm.h
toke.o: /usr/include/netinet/in.h
toke.o: /usr/include/setjmp.h
toke.o: /usr/include/stdio.h
toke.o: /usr/include/stdlib.h
toke.o: /usr/include/string.h
toke.o: /usr/include/sys/dirent.h
toke.o: /usr/include/sys/errno.h
toke.o: /usr/include/sys/fcntlcom.h
toke.o: /usr/include/sys/file.h
toke.o: /usr/include/sys/filio.h
toke.o: /usr/include/sys/ioccom.h
toke.o: /usr/include/sys/ioctl.h
toke.o: /usr/include/sys/param.h
toke.o: /usr/include/sys/signal.h
toke.o: /usr/include/sys/sockio.h
toke.o: /usr/include/sys/stat.h
toke.o: /usr/include/sys/stdtypes.h
toke.o: /usr/include/sys/sysmacros.h
toke.o: /usr/include/sys/time.h
toke.o: /usr/include/sys/times.h
toke.o: /usr/include/sys/ttold.h
toke.o: /usr/include/sys/ttychars.h
toke.o: /usr/include/sys/ttycom.h
toke.o: /usr/include/sys/ttydev.h
toke.o: /usr/include/sys/types.h
toke.o: /usr/include/time.h
toke.o: /usr/include/vm/faultcode.h
toke.o: EXTERN.h
toke.o: arg.h
toke.o: array.h
toke.o: cmd.h
toke.o: config.h
toke.o: form.h
toke.o: handy.h
toke.o: hash.h
toke.o: perl.h
toke.o: perly.h
toke.o: regexp.h
toke.o: spat.h
toke.o: stab.h
toke.o: str.h
toke.o: toke.c
toke.o: util.h
util.o: /usr/include/ctype.h
util.o: /usr/include/dirent.h
util.o: /usr/include/errno.h
util.o: /usr/include/machine/param.h
util.o: /usr/include/machine/setjmp.h
util.o: /usr/include/ndbm.h
util.o: /usr/include/netinet/in.h
util.o: /usr/include/setjmp.h
util.o: /usr/include/stdio.h
util.o: /usr/include/stdlib.h
util.o: /usr/include/string.h
util.o: /usr/include/sys/dirent.h
util.o: /usr/include/sys/errno.h
util.o: /usr/include/sys/fcntlcom.h
util.o: /usr/include/sys/file.h
util.o: /usr/include/sys/filio.h
util.o: /usr/include/sys/ioccom.h
util.o: /usr/include/sys/ioctl.h
util.o: /usr/include/sys/param.h
util.o: /usr/include/sys/signal.h
util.o: /usr/include/sys/sockio.h
util.o: /usr/include/sys/stat.h
util.o: /usr/include/sys/stdtypes.h
util.o: /usr/include/sys/sysmacros.h
util.o: /usr/include/sys/time.h
util.o: /usr/include/sys/times.h
util.o: /usr/include/sys/ttold.h
util.o: /usr/include/sys/ttychars.h
util.o: /usr/include/sys/ttycom.h
util.o: /usr/include/sys/ttydev.h
util.o: /usr/include/sys/types.h
util.o: /usr/include/time.h
util.o: /usr/include/vfork.h
util.o: /usr/include/vm/faultcode.h
util.o: /usr/local/lib/gcc-include/varargs.h
util.o: EXTERN.h
util.o: arg.h
util.o: array.h
util.o: cmd.h
util.o: config.h
util.o: form.h
util.o: handy.h
util.o: hash.h
util.o: perl.h
util.o: regexp.h
util.o: spat.h
util.o: stab.h
util.o: str.h
util.o: util.c
util.o: util.h
usersub.o: /usr/include/ctype.h
usersub.o: /usr/include/dirent.h
usersub.o: /usr/include/errno.h
usersub.o: /usr/include/machine/param.h
usersub.o: /usr/include/machine/setjmp.h
usersub.o: /usr/include/ndbm.h
usersub.o: /usr/include/netinet/in.h
usersub.o: /usr/include/setjmp.h
usersub.o: /usr/include/stdio.h
usersub.o: /usr/include/stdlib.h
usersub.o: /usr/include/string.h
usersub.o: /usr/include/sys/dirent.h
usersub.o: /usr/include/sys/errno.h
usersub.o: /usr/include/sys/filio.h
usersub.o: /usr/include/sys/ioccom.h
usersub.o: /usr/include/sys/ioctl.h
usersub.o: /usr/include/sys/param.h
usersub.o: /usr/include/sys/signal.h
usersub.o: /usr/include/sys/sockio.h
usersub.o: /usr/include/sys/stat.h
usersub.o: /usr/include/sys/stdtypes.h
usersub.o: /usr/include/sys/sysmacros.h
usersub.o: /usr/include/sys/time.h
usersub.o: /usr/include/sys/times.h
usersub.o: /usr/include/sys/ttold.h
usersub.o: /usr/include/sys/ttychars.h
usersub.o: /usr/include/sys/ttycom.h
usersub.o: /usr/include/sys/ttydev.h
usersub.o: /usr/include/sys/types.h
usersub.o: /usr/include/time.h
usersub.o: /usr/include/vm/faultcode.h
usersub.o: EXTERN.h
usersub.o: arg.h
usersub.o: array.h
usersub.o: cmd.h
usersub.o: config.h
usersub.o: form.h
usersub.o: handy.h
usersub.o: hash.h
usersub.o: perl.h
usersub.o: regexp.h
usersub.o: spat.h
usersub.o: stab.h
usersub.o: str.h
usersub.o: usersub.c
usersub.o: util.h
Makefile: Makefile.SH config.sh ; /bin/sh Makefile.SH
makedepend: makedepend.SH config.sh ; /bin/sh makedepend.SH
h2ph: h2ph.SH config.sh ; /bin/sh h2ph.SH
# WARNING: Put nothing here or make depend will gobble it up!
