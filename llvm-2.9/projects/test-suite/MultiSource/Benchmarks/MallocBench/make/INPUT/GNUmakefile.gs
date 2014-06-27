#    Copyright (C) 1989, 1990 Aladdin Enterprises.  All rights reserved.
#    Distributed by Free Software Foundation, Inc.
#
# This file is part of Ghostscript.
#
# Ghostscript is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
# to anyone for the consequences of using it or for whether it serves any
# particular purpose or works at all, unless he says so in writing.  Refer
# to the Ghostscript General Public License for full details.
#
# Everyone is granted permission to copy, modify and redistribute
# Ghostscript, but only under the conditions described in the Ghostscript
# General Public License.  A copy of this license is supposed to have been
# given to you along with Ghostscript so you can know your rights and
# responsibilities.  It should be in a file named COPYING.  Among other
# things, the copyright notice and this notice must be preserved on all
# copies.

# makefile for Ghostscript, Unix/gcc/X11 configuration.

# ------------------------------- Options ------------------------------- #

####### The following are the only parts of the file you should need to edit.

# ------ Generic options ------ #

# Define the default directory/ies for the runtime
# initialization and font files.  Separate multiple directories with a :.
# `pwd` means use the directory in which the 'make' is being done.

GS_LIB_DEFAULT=`pwd`:`pwd`/fonts

# Choose generic configuration options.

# -DDEBUG
#	includes debugging features (-Z switch) in the code.
#	  Code runs substantially slower even if no debugging switches
#	  are set.
# -DNOPRIVATE
#	makes private (static) procedures and variables public,
#	  so they are visible to the debugger and profiler.
#	  No execution time or space penalty.

GENOPT=

# ------ Platform-specific options ------ #

all: realall

# Define the name of the C compiler.

#CC=gcc

# Define the other compilation flags.
# Add -DBSD4_2 for 4.2bsd systems.
# Add -DUSG (GNU convention) or -DSYSV for System V.
# We don't include -ansi, because this gets in the way of the platform-
#   specific stuff that <math.h> typically needs.

CFLAGS= $(OTHERCFLAGS) -I/usr/local/X.V11R5/include

# Define platform flags for ld.
# Most Unix systems accept -X, but some don't.
# Sun OS4.n needs -Bstatic.

LDPLAT=-X

# Define any extra libraries to link into the executable.
# The default is for X Windows.

#EXTRALIBS= -lX11
EXTRALIBS= -L/usr/local/X.V11R5/lib -lX11

# Define the installation commands and target directories for
# executables and files.  Only relevant to `make install'.  If you don't
# have GNU install (part of the fileutils distribution), use cp.

proginstall = install -m 775
fileinstall = install -m 664
bindir = /usr/local/gnu/bin
libdir = /usr/local/gnu/lib/ghostscript

# --------------------------- Choice of devices --------------------------- #

# Choose the device(s) to include.  See gdevs.mak for details.

DEVICES=x11
#DEVICES=deskjet
DEVICE_OBJS=$(x11_)
#DEVICE_OBJS=$(deskjet_)

# ---------------------------- End of options --------------------------- #

# Define the name of the makefile -- used in dependencies.

MAKEFILE=unix-gcc.mak

# Define the extensions for the object and executable files.

OBJ=o
XE=

# Define the ANSI-to-K&R dependency.  (gcc accepts ANSI syntax.)

AK=

# Define the directory separator and shell quote string.

DS=/
Q=\"

# Define the compilation rules.

CCFLAGS=$(GENOPT) $(CFLAGS)

#.c.o:
#	$(CC) $(CCFLAGS<) -c $*.c

CCA=$(CC) $(CCFLAGS) -c
CCNA=$(CCA)
CCINT=$(CCA)

# --------------------------- Generic makefile ---------------------------- #

# The remainder of the makefile (ghost.mak, gdevs.mak, and unixtail.mak)
# is generic.  tar_gs concatenates all these together.
#    Copyright (C) 1989, 1990, 1991 Aladdin Enterprises.  All rights reserved.
#    Distributed by Free Software Foundation, Inc.
#
# This file is part of Ghostscript.
#
# Ghostscript is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
# to anyone for the consequences of using it or for whether it serves any
# particular purpose or works at all, unless he says so in writing.  Refer
# to the Ghostscript General Public License for full details.
#
# Everyone is granted permission to copy, modify and redistribute
# Ghostscript, but only under the conditions described in the Ghostscript
# General Public License.  A copy of this license is supposed to have been
# given to you along with Ghostscript so you can know your rights and
# responsibilities.  It should be in a file named COPYING.  Among other
# things, the copyright notice and this notice must be preserved on all
# copies.

# Generic makefile for Ghostscript.
# The platform-specific makefiles 'include' this file.
# They define the following symbols:
#	GS_LIB_DEFAULT - the default directory/ies for searching for the
#		initialization and font files at run time.
#	Q - the string that causes the shell to pass a " to a program
#		(" on MS-DOS, \" on Unix).
#	XE - the extension for executable files (e.g., null or .exe).
#	OBJ - the extension for relocatable object files (e.g., o or obj).
#	CCA - the ANSI C invocation for normal compilation.
#	CCNA - the non-ANSI C invocation for files that on some platforms
#		include in-line assembly code or other non-standard
#		constructs.  Currently this is needed on every file that
#		includes sstorei.h, and a few files that use Turbo C
#		"short pointer" constructs.
#	CCINT - the C invocation for compiling the main interpreter module,
#		normally the same as CCNA: this is needed because the
#		Borland compiler generates *worse* code for this module
#		(but only this module) when optimization (-O) is turned on.
#	AK - if source files must be converted from ANSI to K&R syntax,
#		this is ansi2knr$(XE); if not, it is null.
#	DS - the directory separator (/ or \).
# The platform-specific makefiles must also include rules for creating
#   ansi2knr$(XE) and genarch$(XE) from the corresponding .c files,
#   and for making arch.h by executing genarch$(XE).  (This
#   shouldn't really be necessary, but Turbo C and Unix C treat the -o
#   switch slightly differently (Turbo C requires no following space,
#   Unix C requires a following space), and I haven't found a way to capture
#   the difference in a macro; also, Unix requires ./ because . may not be
#   in the search path, whereas MS-DOS always looks in the current
#   directory first.)

default: gs$(XE)

test: gt$(XE)

clean:
	rm -f *.$(OBJ) *.a core gmon.out
	rm -f *.dev gdevs.h gdevs.tl
	rm -f t _temp_* _temp_*.* libc*.tl *.map *.sym
	rm -f ansi2knr$(XE) genarch$(XE) arch.h gs$(XE)

# Note: Unix uses malloc.h and memory.h;
# Turbo C uses alloc.h, stdlib.h, and mem.h.
# 4.2bsd uses strings.h; other systems use string.h.
# gcc on VMS doesn't have a math.h.
# We handle this by using local include files called
# malloc_.h, math_.h, memory_.h, and string_.h
# that perform appropriate indirection.

# Auxiliary programs

arch.h: genarch$(XE)
	.$(DS)genarch

# -------------------------------- Library -------------------------------- #

GX=$(AK) std.h gx.h
GXERR=$(GX) gserrors.h

###### High-level facilities

gschar.$(OBJ): gschar.c $(GXERR) \
  gxfixed.h gxarith.h gxmatrix.h gzdevice.h gxdevmem.h gxfont.h gxchar.h gstype1.h gzpath.h gzcolor.h gzstate.h

gscolor.$(OBJ): gscolor.c $(GXERR) \
  gxfixed.h gxmatrix.h gxdevice.h gzstate.h gzcolor.h gzht.h

gscoord.$(OBJ): gscoord.c $(GXERR) \
  gxfixed.h gxmatrix.h gzdevice.h gzstate.h gscoord.h

gsdevice.$(OBJ): gsdevice.c $(GXERR) \
  gxfixed.h gxmatrix.h gxbitmap.h gxdevmem.h gzstate.h gzdevice.h

gsfile.$(OBJ): gsfile.c $(GXERR) \
  gsmatrix.h gxdevice.h gxdevmem.h

gsfont.$(OBJ): gsfont.c $(GXERR) \
  gxdevice.h gxfixed.h gxmatrix.h gxfont.h gxfdir.h gzstate.h

gsimage.$(OBJ): gsimage.c $(GXERR) \
  arch.h gxfixed.h gxarith.h gxmatrix.h gspaint.h gzcolor.h gzdevice.h gzpath.h gzstate.h gximage.h

gsim2out.$(OBJ): gsim2out.c $(GXERR) \
  gsstate.h gsmatrix.h gscoord.h gxfixed.h gxtype1.h

gsline.$(OBJ): gsline.c $(GXERR) \
  gxfixed.h gxmatrix.h gzstate.h gzline.h

gsmatrix.$(OBJ): gsmatrix.c $(GXERR) \
  gxfixed.h gxarith.h gxmatrix.h

gsmisc.$(OBJ): gsmisc.c $(GX)

gspaint.$(OBJ): gspaint.c $(GX) \
  gxfixed.h gxmatrix.h gspaint.h gzpath.h gzstate.h gzdevice.h gximage.h

gspath.$(OBJ): gspath.c $(GXERR) \
  gxfixed.h gxmatrix.h gxpath.h gzstate.h

gspath2.$(OBJ): gspath2.c $(GXERR) \
  gspath.h gxfixed.h gxmatrix.h gzstate.h gzpath.h gzdevice.h

gsstate.$(OBJ): gsstate.c $(GXERR) \
  gxfixed.h gxmatrix.h gzstate.h gzcolor.h gzdevice.h gzht.h gzline.h gzpath.h

gstype1.$(OBJ): gstype1.c $(GXERR) \
  gxarith.h gxfixed.h gxmatrix.h gxchar.h gxdevmem.h gxtype1.h gxfont1.h gzstate.h gzdevice.h gzpath.h

###### Low-level facilities

gxcache.$(OBJ): gxcache.c $(GX) \
  gserrors.h gxfixed.h gxmatrix.h gspaint.h gzdevice.h gzcolor.h gxdevmem.h gxfont.h gxfdir.h gxchar.h gzstate.h gzpath.h

gxcolor.$(OBJ): gxcolor.c $(GX) \
  gxfixed.h gxmatrix.h gxdevice.h gzcolor.h gzht.h

gxdither.$(OBJ): gxdither.c $(GX) \
  gxfixed.h gxmatrix.h gzstate.h gzdevice.h gzcolor.h gzht.h

gxdraw.$(OBJ): gxdraw.c $(GX) \
  gxfixed.h gxmatrix.h gxbitmap.h gzcolor.h gzdevice.h gzstate.h

gxfill.$(OBJ): gxfill.c $(GXERR) \
  gxfixed.h gxmatrix.h gxdevice.h gzcolor.h gzpath.h gzstate.h

gxht.$(OBJ): gxht.c $(GXERR) \
  gxfixed.h gxmatrix.h gxbitmap.h gzstate.h gzcolor.h gzdevice.h gzht.h

gxpath.$(OBJ): gxpath.c $(GXERR) \
  gxfixed.h gzpath.h

gxpath2.$(OBJ): gxpath2.c $(GXERR) \
  gxfixed.h gxarith.h gzpath.h

gxstroke.$(OBJ): gxstroke.c $(GXERR) \
  gxfixed.h gxarith.h gxmatrix.h gzstate.h gzcolor.h gzdevice.h gzline.h gzpath.h

###### The "memory" device

gdevmem.$(OBJ): gdevmem.c $(AK) \
  gs.h arch.h gxbitmap.h gsmatrix.h gxdevice.h gxdevmem.h
	$(CCNA) gdevmem.c

###### Files dependent on the set of installed devices.
###### Generating gdevs.h also generates gdevs.tl.

gdevs.h: gdevs.mak $(MAKEFILE) makefile
	.$(DS)gsconfig $(DEVICES)

gdevs.$(OBJ): gdevs.c $(AK) gdevs.h

###### On Unix, we pre-link all of the library except the back end.
###### On MS-DOS, we have to do the whole thing at once.

LIB=gschar.$(OBJ) gscolor.$(OBJ) gscoord.$(OBJ) gsdevice.$(OBJ) \
 gsfile.$(OBJ) gsfont.$(OBJ) gsimage.$(OBJ) gsim2out.$(OBJ) \
 gsline.$(OBJ) gsmatrix.$(OBJ) gsmisc.$(OBJ) \
 gspaint.$(OBJ) gspath.$(OBJ) gspath2.$(OBJ) gsstate.$(OBJ) gstype1.$(OBJ) \
 gxcache.$(OBJ) gxcolor.$(OBJ) gxdither.$(OBJ) gxdraw.$(OBJ) gxfill.$(OBJ) \
 gxht.$(OBJ) gxpath.$(OBJ) gxpath2.$(OBJ) gxstroke.$(OBJ) \
 gdevmem.$(OBJ) gdevs.$(OBJ)

# ------------------------------ Interpreter ------------------------------ #

###### Utilities

GH=$(AK) ghost.h

ialloc.$(OBJ): ialloc.c $(AK) std.h alloc.h
	$(CCNA) ialloc.c

idebug.$(OBJ): idebug.c $(GH) name.h

idict.$(OBJ): idict.c $(GH) alloc.h errors.h name.h store.h dict.h

iinit.$(OBJ): iinit.c $(GH) dict.h oper.h store.h

iname.$(OBJ): iname.c $(GH) alloc.h errors.h name.h store.h

iscan.$(OBJ): iscan.c $(GH) arch.h alloc.h dict.h errors.h name.h store.h stream.h scanchar.h

iutil.$(OBJ): iutil.c $(GH) errors.h alloc.h oper.h store.h gsmatrix.h gxdevice.h gzcolor.h

stream.$(OBJ): stream.c $(AK) std.h stream.h scanchar.h gxfixed.h gstype1.h

###### Non-graphics operators

OP=$(GH) errors.h oper.h

zarith.$(OBJ): zarith.c $(OP) store.h

zarray.$(OBJ): zarray.c $(OP) alloc.h store.h sstorei.h
	$(CCNA) zarray.c

zcontrol.$(OBJ): zcontrol.c $(OP) estack.h store.h sstorei.h
	$(CCNA) zcontrol.c

zdict.$(OBJ): zdict.c $(OP) dict.h store.h

zfile.$(OBJ): zfile.c $(OP) alloc.h stream.h store.h gsmatrix.h gxdevice.h gxdevmem.h

zgeneric.$(OBJ): zgeneric.c $(OP) dict.h estack.h store.h

zmath.$(OBJ): zmath.c $(OP) store.h

zmisc.$(OBJ): zmisc.c $(OP) alloc.h dict.h store.h gstype1.h gxfixed.h

zpacked.$(OBJ): zpacked.c $(OP) store.h

zrelbit.$(OBJ): zrelbit.c $(OP) store.h sstorei.h dict.h
	$(CCNA) zrelbit.c

zstack.$(OBJ): zstack.c $(OP) store.h sstorei.h
	$(CCNA) zstack.c

zstring.$(OBJ): zstring.c $(OP) alloc.h store.h stream.h

ztype.$(OBJ): ztype.c $(OP) dict.h name.h stream.h store.h

zvmem.$(OBJ): zvmem.c $(OP) alloc.h state.h store.h gsmatrix.h gsstate.h

###### Graphics operators

zchar.$(OBJ): zchar.c $(OP) gxmatrix.h gschar.h gstype1.h gxdevice.h gxfixed.h gxfont.h gxfont1.h gzpath.h gzstate.h alloc.h dict.h font.h estack.h state.h store.h

zcolor.$(OBJ): zcolor.c $(OP) alloc.h gsmatrix.h gsstate.h state.h store.h

zdevice.$(OBJ): zdevice.c $(OP) alloc.h state.h gsmatrix.h gsstate.h gxdevice.h store.h

zfont.$(OBJ): zfont.c $(OP) gsmatrix.h gxdevice.h gxfont.h gxfont1.h alloc.h font.h dict.h name.h state.h store.h

zgstate.$(OBJ): zgstate.c $(OP) alloc.h gsmatrix.h gsstate.h state.h store.h

zht.$(OBJ): zht.c $(OP) alloc.h estack.h gsmatrix.h gsstate.h state.h store.h

zmatrix.$(OBJ): zmatrix.c $(OP) gsmatrix.h state.h gscoord.h store.h

zpaint.$(OBJ): zpaint.c $(OP) alloc.h estack.h gsmatrix.h gspaint.h state.h store.h

zpath.$(OBJ): zpath.c $(OP) gsmatrix.h gspath.h state.h store.h

zpath2.$(OBJ): zpath2.c $(OP) alloc.h estack.h gspath.h state.h store.h

###### Linking

INT=ialloc.$(OBJ) idebug.$(OBJ) idict.$(OBJ) iinit.$(OBJ) iname.$(OBJ) \
 interp.$(OBJ) iscan.$(OBJ) iutil.$(OBJ) stream.$(OBJ) \
 zarith.$(OBJ) zarray.$(OBJ) zcontrol.$(OBJ) zdict.$(OBJ) zfile.$(OBJ) \
 zgeneric.$(OBJ) zmath.$(OBJ) zmisc.$(OBJ) zpacked.$(OBJ) zrelbit.$(OBJ) \
 zstack.$(OBJ) zstring.$(OBJ) ztype.$(OBJ) zvmem.$(OBJ) \
 zchar.$(OBJ) zcolor.$(OBJ) zfont.$(OBJ) zdevice.$(OBJ) zgstate.$(OBJ) \
 zht.$(OBJ) zmatrix.$(OBJ) zpaint.$(OBJ) zpath.$(OBJ) zpath2.$(OBJ)

# ----------------------------- Main program ------------------------------ #

# Utilities shared between platforms

gsmain.$(OBJ): gsmain.c $(GX) \
  gsmatrix.h gxdevice.h

# Library test program driver

gt.$(OBJ): gt.c $(GX) \
  gsmatrix.h gsstate.h gscoord.h gspaint.h gspath.h gxdevice.h

# Interpreter main program

interp.$(OBJ): interp.c $(GH) \
  errors.h name.h dict.h oper.h store.h sstorei.h stream.h
	$(CCINT) interp.c

gs.$(OBJ): gs.c $(GH) alloc.h store.h stream.h $(MAKEFILE)
	$(CCA) -DGS_LIB_DEFAULT=$(Q)$(GS_LIB_DEFAULT)$(Q) gs.c
#    Copyright (C) 1989, 1990, 1991 Aladdin Enterprises.  All rights reserved.
#    Distributed by Free Software Foundation, Inc.
#
# This file is part of Ghostscript.
#
# Ghostscript is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
# to anyone for the consequences of using it or for whether it serves any
# particular purpose or works at all, unless he says so in writing.  Refer
# to the Ghostscript General Public License for full details.
#
# Everyone is granted permission to copy, modify and redistribute
# Ghostscript, but only under the conditions described in the Ghostscript
# General Public License.  A copy of this license is supposed to have been
# given to you along with Ghostscript so you can know your rights and
# responsibilities.  It should be in a file named COPYING.  Among other
# things, the copyright notice and this notice must be preserved on all
# copies.

# makefile for Ghostscript device drivers.

# -------------------------------- Catalog ------------------------------- #

# It is possible to build Ghostscript with an arbitrary collection of
# device drivers, although many drivers are supported only on a subset
# of the target platforms.  The currently available drivers are:

# Displays:
#	bgi	Borland Graphics Interface   [MS-DOS only]
#	ega	EGA display   [MS-DOS only]
#	ega_bios  EGA display using BIOS calls (very slow)   [MS-DOS only]
#	mdb10	EIZO MDB-10 display (1024 x 768)   [MS-DOS only]
#	sonyfb	Sony Microsystems monochrome display   [Sony only]
#	sunview  SunView window system   [SunOS only]
#	vga	VGA display   [MS-DOS only]
#	x11	X Windows version 11, release >=3   [Unix only]
# (NOTE: no more than one MS-DOS display driver may be included in
#   any build.)
# Printers:
#	bj10e	Canon BubbleJet BJ10e  [MS-DOS only]
#	deskjet  H-P DeskJet   [MS-DOS & Unix]
#	epson	Epson dot matrix printers   [MS-DOS only]
#	laserjet  H-P LaserJet   [MS-DOS & Unix]
#	nwp533  Sony Microsystems NWP533 laser printer   [Sony only]
# ###	paintjet  H-P PaintJet color printer [Unix, tested on SunOS only]
# ###	  (NOT INCLUDED IN THIS RELEASE)

# If you add drivers, it would be nice if you kept each list
# in alphabetical order.

# Each platform-specific makefile should contain a pair of lines of the form
#	DEVICES=<dev1> ... <devn>
#	DEVICE_OBJS=$(<dev1>_) ... $(<devn>_)
# where dev1 ... devn are the devices to be included in the build.
# dev1 will be used as the default device.  Don't forget the _s!
# On MS-DOS platforms, the first of these lines must appear *before*,
# and the second *after*, the lines
#	(!)include gdevs.mak
#	(!)include ghost.mak
# in the makefile.  On Unix systems, the two device definition lines
# may appear anywhere in the makefile.
#
# The executable must be linked with all the files named in DEVICE_OBJS.
# On MS-DOS platforms, this is done by constructing a file called gdevs.tl,
# to get around the limit on the length of a DOS command line.

# ---------------------------- End of catalog ---------------------------- #

# If you want to add a new device driver, the examples below should be
# enough of a guide to the correct form for the makefile rules.

# All device drivers depend on the following
# (note that we include some indirect dependencies explicitly):

GDEV=$(AK) gs.h gx.h gsmatrix.h gxbitmap.h gxdevice.h

###### ------------------- MS-DOS display devices ------------------- ######

GDEVPCFB=gdevpcfb.h $(GDEV)

gdevegaa.$(OBJ): gdevegaa.asm

### -------------------------- The EGA device -------------------------- ###

ETEST=ega.$(OBJ) trace.$(OBJ) $(ega_)
ega.exe: $(ETEST) libc$(MM).tl
	tlink /m /l $(LIBDIR)\c0$(MM) @ega.tl @libc$(MM).tl

ega.$(OBJ): ega.c $(GDEV)

ega_=gdevega.$(OBJ) gdevegaa.$(OBJ)
ega.dev: $(ega_)
	.$(DS)gssetdev ega.dev $(ega_)

gdevega.$(OBJ): gdevpcfb.c gdevega.h $(GDEVPCFB)
	cp gdevega.h gdevxxfb.h
	$(CCNA) gdevpcfb.c
	rm gdevxxfb.h
	cp gdevpcfb.obj gdevega.obj

ega_bios_=gdevegab.$(OBJ) gdevegaa.$(OBJ)
ega_bios.dev: $(ega_bios_)
	.$(DS)gssetdev ega_bios.dev $(ega_bios_)

gdevegab.$(OBJ): gdevegab.c $(GDEVPCFB)
	$(CCNA) $(D_EGA) gdevegab.c

### -------------------------- The VGA device -------------------------- ###

vga_=gdevvga.$(OBJ) gdevegaa.$(OBJ)
vga.dev: $(vga_)
	.$(DS)gssetdev vga.dev $(vga_)

gdevvga.$(OBJ): gdevpcfb.c gdevvga.h $(GDEVPCFB)
	cp gdevvga.h gdevxxfb.h
	$(CCNA) gdevpcfb.c
	rm gdevxxfb.h
	cp gdevpcfb.obj gdevvga.obj

### ---------------------- The EIZO MDB-10 device ---------------------- ###

mdb10_=gdevmd10.$(OBJ) gdevegaa.$(OBJ)
mdb10.dev: $(mdb10_)
	.$(DS)gssetdev mdb10.dev $(mdb10_)

gdevmd10.$(OBJ): gdevpcfb.c gdevmd10.h $(GDEVPCFB)
	cp gdevmd10.h gdevxxfb.h
	$(CCNA) gdevpcfb.c
	rm gdevxxfb.h
	cp gdevpcfb.obj gdevmd10.obj

###### --------- The BGI (Borland Graphics Interface) device -------- ######

bgi_=gdevbgi.$(OBJ) egavga.$(OBJ)
bgi.dev: $(bgi_)
	.$(DS)gssetdev bgi.dev $(bgi_)

gdevbgi.$(OBJ): gdevbgi.c $(GDEV)
	$(CCNA) gdevbgi.c

egavga.$(OBJ): $(COMPDIR)\egavga.bgi
	bgiobj egavga

###### --------------- Memory-buffered printer devices --------------- ######

gvirtmem.$(OBJ): gvirtmem.c gvirtmem.h

# Virtual memory test program -- requires VMDEBUG

gvm.exe: gvirtmem.$(OBJ) trace.$(OBJ)
	tlink /m /l $(LIBDIR)\c0$(MM) gvirtmem trace,gvm,gvm,$(LIBDIR)\c$(MM)

PDEVH=$(GDEV) gxdevmem.h gvirtmem.h gdevprn.h

# We use CCNA only because the MS-DOS version of the driver
# refers to stdprn, which is non-ANSI.
gdevprn.$(OBJ): gdevprn.c $(PDEVH)
	$(CCNA) gdevprn.c

### ----------------- The Canon BubbleJet BJ10e device ----------------- ###

bj10e_=gdevbj10.$(OBJ) gvirtmem.$(OBJ) gdevprn.$(OBJ)
bj10e.dev: $(bj10e_)
	.$(DS)gssetdev bj10e.dev $(bj10e_)

gdevbj10.$(OBJ): gdevbj10.c $(PDEVH)

### ------------------ The H-P DeskJet printer device ------------------ ###

# Note that this shares code with the LaserJet device (below).

deskjet_=gdevdjet.$(OBJ) gvirtmem.$(OBJ) gdevprn.$(OBJ)
deskjet.dev: $(deskjet_)
	.$(DS)gssetdev deskjet.dev $(deskjet_)

gdevdjet.$(OBJ): gdevdjet.c $(PDEVH)
	$(CCA) -DLASER=0 gdevdjet.c

### ----------------- The generic Epson printer device ----------------- ###

epson_=gdevepsn.$(OBJ) gvirtmem.$(OBJ) gdevprn.$(OBJ)
epson.dev: $(epson_)
	.$(DS)gssetdev epson.dev $(epson_)

gdevepsn.$(OBJ): gdevepsn.c $(PDEVH)

### ------------------ The H-P LaserJet printer device ----------------- ###

# Note that this shares code with the DeskJet device (above).

laserjet_=gdevljet.$(OBJ) gvirtmem.$(OBJ) gdevprn.$(OBJ)
laserjet.dev: $(laserjet_)
	.$(DS)gssetdev laserjet.dev $(laserjet_)

gdevljet.$(OBJ): gdevdjet.c $(PDEVH)
	cp gdevdjet.c gdevljet.c
	$(CCA) -DLASER=1 gdevljet.c
	rm gdevljet.c

### ------------ The H-P PaintJet color printer device ----------------- ###
### Note: this driver was contributed by users:                          ###
###       please contact marc@vlsi.polymtl.ca if you have questions.     ###

### This driver was not ready to be included in release 2.0.
### Do not attempt to use it: the files are not included in the fileset.

PJETH=$(GDEV) gdevprn.h gdevpjet.h

paintjet_=gdevpjet.$(OBJ)
paintjet.dev: $(paintjet_)
	.$(DS)gssetdev paintjet.dev $(paintjet_)

gdevpjet.$(OBJ): gdevpjet.c $(PJETH)

###### ------------------ Sony frame buffer device ----------------- ######

sonyfb_=gdevsnfb.$(OBJ) gvirtmem.$(OBJ) gdevprn.$(OBJ)
sonyfb.dev: $(sonyfb_)
	.$(DS)gssetdev sonyfb.dev $(sonyfb_)

gdevsnfb.$(OBJ): gdevsnfb.c $(PDEVH)
	$(CCA) gdevsnfb.c

###### ----------------- Sony NWP533 printer device ----------------- ######

nwp533_=gdevn533.$(OBJ) gvirtmem.$(OBJ) gdevprn.$(OBJ)
nwp533.dev: $(nwp533_)
	.$(DS)gssetdev nwp533.dev $(nwp533_)

gdevn533.$(OBJ): gdevn533.c $(PDEVH)
	$(CCA) gdevn533.c

###### --------------------- The SunView device --------------------- ######

sunview_=gdevsun.$(OBJ)
sunview.dev: $(sunview_)
	.$(DS)gssetdev sunview.dev $(sunview_)

gdevsun.$(OBJ): gdevsun.c $(GDEV)

###### ----------------------- The X11 device ----------------------- ######

x11_=gdevx.$(OBJ)
x11.dev: $(x11_)
	.$(DS)gssetdev x11.dev $(x11_)

### Note: if the X11 client header libraries are not on /usr/include/X11,
### you may have to change the compilation line below to add a -I switch.
### For example, if the header files are in /usr/local/X/include/X11,
### you must add the switch -I/usr/local/X/include.
gdevx.$(OBJ): gdevx.c $(GDEV) gdevx.h
	$(CCA) -I/usr/local/X.V11R5/include gdevx.c


#    Copyright (C) 1990, 1991 Aladdin Enterprises.  All rights reserved.
#    Distributed by Free Software Foundation, Inc.
#
# This file is part of Ghostscript.
#
# Ghostscript is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
# to anyone for the consequences of using it or for whether it serves any
# particular purpose or works at all, unless he says so in writing.  Refer
# to the Ghostscript General Public License for full details.
#
# Everyone is granted permission to copy, modify and redistribute
# Ghostscript, but only under the conditions described in the Ghostscript
# General Public License.  A copy of this license is supposed to have been
# given to you along with Ghostscript so you can know your rights and
# responsibilities.  It should be in a file named COPYING.  Among other
# things, the copyright notice and this notice must be preserved on all
# copies.

# Partial makefile for Ghostscript, common to all Unix configurations.

# This is the last part of the makefile for Unix configurations.
# Since Unix make doesn't have an 'include' facility, we concatenate
# the various parts of the makefile together by brute force (in tar_gs).

# -------------------------------- Library -------------------------------- #

## The Unix platform

PLATUNIX=gp_unix.$(OBJ)

gp_unix.$(OBJ): gp_unix.c

# -------------------------- Auxiliary programs --------------------------- #

ansi2knr$(XE):
	$(CC) -o ansi2knr$(XE) $(CFLAGS) ansi2knr.c

genarch$(XE): genarch.c
	$(CC) -o genarch$(XE) $(CFLAGS) genarch.c

# ----------------------------- Main program ------------------------------ #

# (Dummy) tracing package

utrace.$(OBJ): utrace.c cframe_.h

# Main program

ALLUNIX=gsmain.$(OBJ) utrace.$(OBJ) $(LIB) $(PLATUNIX) $(DEVICE_OBJS)

# Library test programs

GTUNIX=gt.$(OBJ) $(ALLUNIX)
gt: $(GTUNIX)
	$(CC) $(CFLAGS) $(LDPLAT) -o gt $(GTUNIX) $(EXTRALIBS) -lm

# Interpreter main program

GSUNIX=gs.$(OBJ) $(INT) $(ALLUNIX)

BASETARGET = gs
APPCFLAGS = -DGS_LIB_DEFAULT=$(Q)$(GS_LIB_DEFAULT)$(Q)
OBJS = $(GSUNIX)
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


# Installation

sysps = ghost.ps gfonts.ps statusd.ps
artps = chess.ps cheq.ps golfer.ps escher.ps
utilps = decrypt.ps bdftops.ps pstoppm.ps fcutils.ps prfont.ps

install:
	$(proginstall) gs gsnd bdftops $(bindir)
	$(fileinstall) $(sysps) $(artps) $(utilps) $(libdir)
