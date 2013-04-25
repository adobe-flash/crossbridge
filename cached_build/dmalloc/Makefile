###############################################################################
# Makefile.  Generated from Makefile.in by configure.
# Makefile for the DMalloc library.
# $Id: Makefile.all.in,v 1.133 2007/05/14 17:13:45 gray Exp $
###############################################################################

# default root installation directory
prefix = FLASCC_SRC_DIR/sdk/usr
exec_prefix = ${prefix}

srcdir = FLASCC_SRC_DIR/dmalloc-5.5.2
VPATH = FLASCC_SRC_DIR/dmalloc-5.5.2

bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
shlibdir = ${exec_prefix}/lib
includedir = ${prefix}/include
docdir = $(prefix)/share/doc/$(MODULE)

# default c-compiler
CC = gcc
CXX = g++

DEFS = -DHAVE_STDARG_H=1 \
	-DHAVE_STDLIB_H=1 \
	-DHAVE_STRING_H=1 \
	-DHAVE_UNISTD_H=1 \
	-DHAVE_SYS_MMAN_H=1 \
	-DHAVE_SYS_TYPES_H=1 \
	-DHAVE_W32API_WINBASE_H=0 \
	-DHAVE_W32API_WINDEF_H=0 \
	-DHAVE_SYS_CYGWIN_H=0 \
	-DHAVE_SIGNAL_H=1 \
	$(DEFINES)
INCS = -I. -I$(srcdir) $(INCLUDES)
LIBS = -L. 
A_OUT = aout

MODULE	= dmalloc
LIBRARY	= lib$(MODULE).a

# thread version of the library
LIB_TH	= lib$(MODULE)th.a
LIB_TH_SL = lib$(MODULE)th.so
#BUILD_ALL_1 = threads
#INSTALL_LIB_1 = installth
#BUILD_THREADS_1 = $(LIB_TH_SL)
#INSTALL_THREADS_1 = installthsl
##BUILD_ALL_2 = $(LIB_TH_SL)
##INSTALL_LIB_2 = installthsl

# C++ version of the library
LIB_CXX	= lib$(MODULE)xx.a
LIB_CXX_SL = lib$(MODULE)xx.so
#BUILD_ALL_3 = $(LIB_CXX)
#INSTALL_LIB_3 = installcxx
#BUILD_CXX_3 = $(LIB_CXX_SL)
#INSTALL_CXX_3 = installcxxsl
##BUILD_ALL_4 = $(LIB_CXX_SL)
##INSTALL_LIB_4 = installcxxsl

# threads + C++
LIB_TH_CXX = lib$(MODULE)thcxx.a
LIB_TH_CXX_SL = lib$(MODULE)thcxx.so
##BUILD_ALL_5 = $(LIB_TH_CXX)
##INSTALL_LIB_5 = installthcxx
#BUILD_CXX_5 = $(LIB_TH_CXX)
#INSTALL_CXX_5 = installthcxx
#BUILD_THREADS_5 = $(LIB_TH_CXX)
#INSTALL_THREADS_5 = installthcxx
###BUILD_ALL_6 = $(LIB_TH_CXX_SL)
###INSTALL_LIB_6 = installthcxxsl
##BUILD_CXX_6 = $(LIB_TH_CXX_SL)
##INSTALL_CXX_6 = installthcxxsl
##BUILD_THREADS_6 = $(LIB_TH_CXX_SL)
##INSTALL_THREADS_6 = installthcxxsl

# shared versions of the libraries
LIB_SL = lib$(MODULE).so
#BUILD_ALL_7 = $(LIB_SL)
#INSTALL_LIB_7 = installsl
#BUILD_TH_CXX_7 = $(LIB_TH_CXX_SL)
#INSTALL_TH_CXX_7 = installthcxxsl
#BUILD_SL_7 = $(LIB_TH_SL)
#INSTALL_SL_7 = installthsl
#BUILD_SL_8 = $(LIB_CXX_SL)
#INSTALL_SL_8 = installcxxsl
##BUILD_SL_9 = $(LIB_TH_CXX_SL)
##INSTALL_SL_9 = installthcxxsl

# now construct our build and install targets
BUILD_ALL = dmalloc.h $(LIBRARY) $(UTIL) \
	$(BUILD_ALL_1) $(BUILD_ALL_2) $(BUILD_ALL_3) $(BUILD_ALL_4) \
	$(BUILD_ALL_5) $(BUILD_ALL_6) $(BUILD_ALL_7) $(BUILD_ALL_8)
INSTALL_LIB = $(LIBRARY) \
	$(INSTALL_LIB_1) $(INSTALL_LIB_2) $(INSTALL_LIB_3) $(INSTALL_LIB_4) \
	$(INSTALL_LIB_5) $(INSTALL_LIB_6) $(INSTALL_LIB_7) $(INSTALL_LIB_8)
BUILD_THREADS = $(LIB_TH) \
	$(BUILD_THREADS_1) $(BUILD_THREADS_2) $(BUILD_THREADS_3) \
	$(BUILD_THREADS_4) $(BUILD_THREADS_5) $(BUILD_THREADS_6)
INSTALL_THREADS = $(LIB_TH) \
	$(INSTALL_THREADS_1) $(INSTALL_THREADS_2) $(INSTALL_THREADS_3) \
	$(INSTALL_THREADS_4) $(INSTALL_THREADS_5) $(INSTALL_THREADS_6)
BUILD_CXX = $(LIB_CXX) \
	$(BUILD_CXX_1) $(BUILD_CXX_2) $(BUILD_CXX_3) $(BUILD_CXX_4) \
	$(BUILD_CXX_5) $(BUILD_CXX_6) $(BUILD_CXX_7) $(BUILD_CXX_8)
INSTALL_CXX = $(LIB_CXX) \
	$(INSTALL_CXX_1) $(INSTALL_CXX_2) $(INSTALL_CXX_3) $(INSTALL_CXX_4) \
	$(INSTALL_CXX_5) $(INSTALL_CXX_6) $(INSTALL_CXX_7) $(INSTALL_CXX_8)
BUILD_TH_CXX = $(LIB_TH_CXX) \
	$(BUILD_TH_CXX_1) $(BUILD_TH_CXX_2) $(BUILD_TH_CXX_3) \
	$(BUILD_TH_CXX_4) $(BUILD_TH_CXX_5) $(BUILD_TH_CXX_6) \
	$(BUILD_TH_CXX_7) $(BUILD_TH_CXX_8) $(BUILD_TH_CXX_9)
INSTALL_TH_CXX = $(LIB_TH_CXX) \
	$(INSTALL_TH_CXX_1) $(INSTALL_TH_CXX_2) $(INSTALL_TH_CXX_3) \
	$(INSTALL_TH_CXX_4) $(INSTALL_TH_CXX_5) $(INSTALL_TH_CXX_6) \
	$(INSTALL_TH_CXX_7) $(INSTALL_TH_CXX_8) $(INSTALL_TH_CXX_9)
BUILD_SL = $(LIB_SL) \
	$(BUILD_SL_1) $(BUILD_SL_2) $(BUILD_SL_3) $(BUILD_SL_4) \
	$(BUILD_SL_5) $(BUILD_SL_6) $(BUILD_SL_7) $(BUILD_SL_8) \
	$(BUILD_SL_9)
INSTALL_SL = $(LIB_SL) \
	$(INSTALL_SL_1) $(INSTALL_SL_2) $(INSTALL_SL_3) $(INSTALL_SL_4) \
	$(INSTALL_SL_5) $(INSTALL_SL_6) $(INSTALL_SL_7) $(INSTALL_SL_8) \
	$(INSTALL_SL_9)

UTIL = $(MODULE)
HTMLFILE = $(MODULE).html
TEXIFILE = $(MODULE).texi
PDFFILE = $(MODULE).pdf
DOCFILES = $(HTMLFILE) $(TEXIFILE) $(PDFFILE)

CCFLAGS = -g -O2
LDFLAGS = 

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

###############################################################################
# End of system configuration section.
###############################################################################

SHELL = /bin/sh

HFLS = dmalloc.h
OBJS = arg_check.o compat.o dmalloc_rand.o dmalloc_tab.o env.o heap.o
NORMAL_OBJS = chunk.o error.o malloc.o
THREAD_OBJS = chunk_th.o error_th.o malloc_th.o
CXX_OBJS = dmallocc.o

CFLAGS = $(CCFLAGS)
TEST = $(MODULE)_t
TEST_FC = $(MODULE)_fc_t

all : $(BUILD_ALL)
	@echo "To make the thread version of the library type 'make threads'"
	@echo "To make the C++ version of the library type 'make cxx'"
	@echo "To make the shared version of the library type 'make shlib'"

clean :
	rm -f $(A_OUT) core *.o *.t
	rm -f $(LIBRARY) $(LIB_TH) $(LIB_CXX) $(LIB_TH_CXX) $(TEST) $(TEST_FC)
	rm -f $(LIB_TH_SL) $(LIB_CXX_SL) $(LIB_TH_CXX_SL) $(LIB_SL)
	rm -f $(UTIL) dmalloc.h

realclean : clean

distclean : clean
	rm -f confdefs.h config.status config.log config.cache conf.h
	rm -f configure.lineno
	rm -rf autom4te.cache
	rm -f Makefile settings.h dmalloc.h.2
#	rm -f configure

installdirs :
	$(srcdir)/mkinstalldirs $(includedir) $(libdir) $(bindir)

installincs : $(HFLS)
	$(srcdir)/mkinstalldirs $(includedir)
	$(INSTALL_DATA) $(HFLS) $(includedir)

installthsl : $(LIB_TH_SL)
	$(srcdir)/mkinstalldirs $(libdir)
	$(INSTALL_PROGRAM) $(LIB_TH_SL) $(libdir)

installth : $(INSTALL_THREADS)
	$(srcdir)/mkinstalldirs $(libdir)
	$(INSTALL_PROGRAM) $(LIB_TH) $(libdir)
	@echo "Enter 'make installthcxx' to install the threaded C++ library"
	@echo "Enter 'make installthsl' to install the threaded shared-library"

installthcxxsl : $(LIB_TH_CXX_SL)
	$(srcdir)/mkinstalldirs $(shlibdir)
	$(INSTALL_PROGRAM) $(LIB_TH_CXX_SL) $(shlibdir)

installthcxx : $(INSTALL_TH_CXX)
	$(srcdir)/mkinstalldirs $(libdir)
	$(INSTALL_PROGRAM) $(LIB_TH_CXX) $(libdir)
	@echo "Enter 'make installthcxxsl' to install the threaded C++ shared-library"

installcxxsl : $(LIB_CXX_SL)
	$(srcdir)/mkinstalldirs $(libdir)
	$(INSTALL_PROGRAM) $(LIB_CXX_SL) $(libdir)

installcxx : $(INSTALL_CXX)
	$(srcdir)/mkinstalldirs $(libdir)
	$(INSTALL_PROGRAM) $(LIB_CXX) $(libdir)
	@echo "Enter 'make installthcxx' to install the threaded C++ library"
	@echo "Enter 'make installcxxsl' to install the C++ shared-library"

installsl : $(LIB_SL)
	$(srcdir)/mkinstalldirs $(shlibdir)
	$(INSTALL_PROGRAM) $(LIB_SL) $(shlibdir)
	@echo "Enter 'make installcxxsl' to install the C++ shared-library"
	@echo "Enter 'make installthsl' to install thread shared-library"

installlib : $(INSTALL_LIB)
	$(srcdir)/mkinstalldirs $(libdir)
	$(INSTALL_PROGRAM) $(LIBRARY) $(libdir)
	avm2-unknown-freebsd8-ranlib $(libdir)/$(LIBRARY)
	@echo "Enter 'make installsl' to install $(LIB_SL) in $(shlibdir)"
	@echo "Enter 'make installcxx' to install the C++ library"
	@echo "Enter 'make installth' to install thread library"

installdocs : $(srcdir)/docs/$(HTMLFILE) $(srcdir)/docs/$(TEXIFILE) \
		$(srcdir)/docs/$(PDFFILE)
	$(srcdir)/mkinstalldirs $(docdir)
	$(INSTALL_DATA) $(srcdir)/docs/$(HTMLFILE) $(docdir)
	$(INSTALL_DATA) $(srcdir)/docs/$(TEXIFILE) $(docdir)
	$(INSTALL_DATA) $(srcdir)/docs/$(PDFFILE) $(docdir)

install : installincs installlib $(UTIL)
	$(srcdir)/mkinstalldirs $(bindir)
	$(INSTALL_PROGRAM) $(UTIL) $(bindir)
	@echo "Enter 'make installdocs' to install $(DOCFILES) in $(docdir)"

dmalloc.h.2 : $(srcdir)/configure
	$(SHELL) $(srcdir)/configure
	-touch $@

settings.h : $(srcdir)/settings.dist $(srcdir)/configure
	$(SHELL) $(srcdir)/configure
	-touch $@

dmalloc.h : $(srcdir)/dmalloc.h.1 dmalloc.h.2 $(srcdir)/dmalloc.h.3
	rm -f $@ $@.t
	cat $(srcdir)/dmalloc.h.1 dmalloc.h.2 $(srcdir)/dmalloc.h.3 > $@.t
	mv $@.t $@

shlib : $(BUILD_SL)
	@echo "To make the thread version of the library type 'make threads'"
	@echo "To make the C++ version of the library type 'make cxx'"

# NOTE: you may have to edit the configure.ac script to get this to
# work on your operating system.  Please send feedback to the author
# via: http://256.com/gray/email.html
$(LIB_SL) : $(LIBRARY)
	rm -f $@ $@.t
	ld -G -o $@.t $(LIBRARY) $(OBJS) $(NORMAL_OBJS)
	mv $@.t $@

$(LIBRARY) : $(OBJS) $(NORMAL_OBJS)
	ar cr $@ $?
	avm2-unknown-freebsd8-ranlib $@

$(LIB_TH) : $(OBJS) $(THREAD_OBJS)
	ar cr $@ $?
	avm2-unknown-freebsd8-ranlib $@

$(LIB_TH_SL) : $(LIB_TH)
	rm -f $@ $@.t
	ld -G -o $@.t $(LIB_TH) $(OBJS) $(THREAD_OBJS)
	mv $@.t $@

$(LIB_CXX) : $(OBJS) $(NORMAL_OBJS) $(CXX_OBJS)
	ar cr $@ $?
	avm2-unknown-freebsd8-ranlib $@

$(LIB_CXX_SL) : $(LIB_CXX)
	rm -f $@ $@.t
	ld -G -o $@.t $(LIB_CXX) $(OBJS) $(NORMAL_OBJS) $(CXX_OBJS)
	mv $@.t $@

$(LIB_TH_CXX) : $(OBJS) $(THREAD_OBJS) $(CXX_OBJS)
	ar cr $@ $?
	avm2-unknown-freebsd8-ranlib $@

$(LIB_TH_CXX_SL) : $(LIB_TH_CXX)
	rm -f $@ $@.t
	ld -G -o $@.t $(LIB_TH_CXX) $(OBJS) $(THREAD_OBJS) $(CXX_OBJS)
	mv $@.t $@

threadssl : $(LIB_TH_SL)

threadscxxsl : $(LIB_TH_CXX_SL)

threadscxx : $(BUILD_TH_CXX)
	@echo "Enter 'make threadscxxsl' to build the threaded C++ shared-library"

threads : $(BUILD_THREADS)
	@echo "Enter 'make threadscxx' to build the threaded C++ library"
	@echo "Enter 'make threadssl' to build the threaded shared library"

cxxsl :  $(LIB_CXX_SL)

cxx : $(BUILD_CXX)
	@echo "Enter 'make cxxsl' to build the cxx shared library"

utils : $(UTIL)

$(UTIL) : $(UTIL).o dmalloc_argv.o compat.o env.o
	rm -f $@
	$(CC) $(LDFLAGS) -o $(A_OUT) $(UTIL).o dmalloc_argv.o compat.o env.o \
		$(LIBS)
	mv $(A_OUT) $@

# special _th versions of objects with the LOCK_THREADS variable defined to 1
chunk_th.o : $(srcdir)/chunk.c
	rm -f $@
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS) $(INCS) -DLOCK_THREADS=1 \
		-c $(srcdir)/chunk.c -o ./$@

error_th.o : $(srcdir)/error.c
	rm -f $@
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS) $(INCS) -DLOCK_THREADS=1 \
		-c $(srcdir)/error.c -o ./$@

malloc_th.o : $(srcdir)/malloc.c
	rm -f $@
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS) $(INCS) -DLOCK_THREADS=1 \
		-c $(srcdir)/malloc.c -o ./$@

tests : $(TEST) $(TEST_FC)

$(TEST) : $(TEST).o dmalloc_argv.o $(LIBRARY)
	rm -f $@
	$(CC) $(LDFLAGS) -o $(A_OUT) $(TEST).o dmalloc_argv.o $(LIBRARY)
	mv $(A_OUT) $@

$(TEST_FC) : $(TEST_FC).o dmalloc_argv.o $(LIBRARY)
	rm -f $@
	$(CC) $(LDFLAGS) -o $(A_OUT) $(TEST_FC).o dmalloc_argv.o $(LIBRARY)
	mv $(A_OUT) $@

check light : $(TEST) $(TEST_FC)
	./$(TEST_FC) -s
	./$(TEST) -s -t 10000
	./$(TEST) -s -t 10000
	./$(TEST) -s -t 10000
	./$(TEST) -s -t 10000
	./$(TEST) -s -t 10000
	@echo tests have passed

heavy : $(TEST) light
	./$(TEST) -s -t 100000
	./$(TEST) -s -t 100000
	./$(TEST) -s -t 100000
	@echo heavy tests have passed

.c.o :
	rm -f $@
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEFS) $(INCS) -c $< -o ./$@

#
# .cc.o auto-target doesn't work on some systems. 
# also, $? did not work on OSF for the -c, grrr
#
dmallocc.o : $(srcdir)/dmallocc.cc
	rm -f $@
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(DEFS) $(INCS) -c $(srcdir)/dmallocc.cc \
		-o ./$@

#
# auto configure settings - uncomment if you are doing configure
# development on the library
#

#Makefile : $(srcdir)/Makefile.in config.status
#	$(SHELL) config.status

#conf.h : $(srcdir)/conf.h.in config.status
#	$(SHELL) config.status

#config.status : $(srcdir)/configure
#	$(SHELL) config.status --recheck

# $(srcdir)/configure : $(srcdir)/configure.ac
#	cd $(srcdir); autoconf

###############################################################################
#
# These dependencies are automatically generated.  Do not edit by hand.
#

arg_check.o: arg_check.c conf.h settings.h dmalloc.h chunk.h debug_tok.h \
  dmalloc_loc.h error.h arg_check.h
chunk.o: chunk.c conf.h settings.h dmalloc.h chunk.h chunk_loc.h \
  dmalloc_loc.h compat.h debug_tok.h dmalloc_rand.h dmalloc_tab.h error.h \
  error_val.h heap.h
compat.o: compat.c conf.h settings.h dmalloc.h compat.h dmalloc_loc.h
dmalloc.o: dmalloc.c conf.h settings.h dmalloc_argv.h dmalloc.h compat.h \
  debug_tok.h dmalloc_loc.h env.h error_val.h version.h
dmalloc_argv.o: dmalloc_argv.c conf.h settings.h dmalloc_argv.h \
  dmalloc_argv_loc.h compat.h
dmalloc_fc_t.o: dmalloc_fc_t.c conf.h settings.h dmalloc.h dmalloc_argv.h \
  dmalloc_rand.h debug_tok.h dmalloc_loc.h error_val.h
dmalloc_rand.o: dmalloc_rand.c dmalloc_rand.h
dmalloc_t.o: dmalloc_t.c conf.h settings.h compat.h dmalloc.h \
  dmalloc_argv.h dmalloc_rand.h arg_check.h debug_tok.h dmalloc_loc.h \
  error_val.h heap.h
dmalloc_tab.o: dmalloc_tab.c conf.h settings.h chunk.h compat.h dmalloc.h \
  dmalloc_loc.h error.h error_val.h dmalloc_tab.h dmalloc_tab_loc.h
env.o: env.c conf.h settings.h dmalloc.h compat.h dmalloc_loc.h \
  debug_tok.h env.h error.h
error.o: error.c conf.h settings.h dmalloc.h chunk.h compat.h debug_tok.h \
  dmalloc_loc.h env.h error.h error_val.h version.h
heap.o: heap.c conf.h settings.h dmalloc.h chunk.h compat.h debug_tok.h \
  dmalloc_loc.h error.h error_val.h heap.h
malloc.o: malloc.c conf.h settings.h dmalloc.h chunk.h compat.h \
  debug_tok.h dmalloc_loc.h env.h error.h error_val.h heap.h \
  malloc_funcs.h return.h
protect.o: protect.c conf.h settings.h dmalloc.h dmalloc_loc.h error.h \
  heap.h protect.h
chunk_th.o: chunk.c conf.h settings.h dmalloc.h chunk.h chunk_loc.h \
  dmalloc_loc.h compat.h debug_tok.h dmalloc_rand.h dmalloc_tab.h error.h \
  error_val.h heap.h
error_th.o: error.c conf.h settings.h dmalloc.h chunk.h compat.h debug_tok.h \
  dmalloc_loc.h env.h error.h error_val.h version.h
malloc_th.o: malloc.c conf.h settings.h dmalloc.h chunk.h compat.h \
  debug_tok.h dmalloc_loc.h env.h error.h error_val.h heap.h \
  malloc_funcs.h return.h
