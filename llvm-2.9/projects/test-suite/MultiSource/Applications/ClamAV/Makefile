LEVEL = ../../../
#RUN_OPTIONS     = 

Source = $(addprefix libclamav_, \
	 aspack.c autoit.c binhex.c blob.c cab.c chmunpack.c cvd.c dconf.c dsig.c\
	 elf.c entconv.c filetypes.c fsg.c hashtab.c htmlnorm.c\
	 is_tar.c jscript.c line.c lockdb.c matcher-ac.c matcher-bm.c\
	 matcher.c mbox.c md5.c message.c mew.c msexpand.c mspack.c ole2_extract.c\
	 others.c packlibs.c pdf.c pe.c petite.c phishcheck.c phish_domaincheck_db.c phish_whitelist.c\
	 pst.c readdb.c rebuildpe.c regex_list.c rtf.c scanners.c sis.c snprintf.c special.c spin.c str.c suecrypt.c\
	 table.c text.c tnef.c unarj.c unsp.c untar.c unzip.c upack.c upx.c uuencode.c vba_extract.c wwunpack.c yc.c)\
	 $(addprefix clamscan_, \
 	 clamscan.c manager.c others.c treewalk.c)\
	 $(addprefix shared_, \
	 cdiff.c cfgparser.c getopt.c misc.c network.c options.c output.c sha256.c)\
	 $(addprefix libclamav_regex_, \
	 regerror.c, regfree.c, \
	 regcomp.c,  regexec.c, strlcpy.c)\
	 $(addprefix libclamav_nsis_, \
		 nulsft.c, bzlib.c, infblock.c, LZMADecode.c )
#        SVN version has these:	
#	 inflate64.c lzma_iface.c
#	 $(addprefix libclamav_lzma_, \
#	 LzmaStateDecode.c) \

PROG = clamscan
CPPFLAGS += -DHAVE_CONFIG_H -I. -DDONT_LOCK_DBDIRS
LDFLAGS = -lz
RUN_OPTIONS = --debug --exclude-dir .svn --verbose -d$(PROJ_SRC_DIR)/dbdir -r $(PROJ_SRC_DIR)/inputs/*

include $(LEVEL)/Makefile.config


# for ClamAV these are in clamav-config.h, but since we can't run configure, we have to define them here


# -- OS macros --
#  It is especially important to get this correctly defined for windows.

ifeq ($(TARGET_OS),Darwin)
   CPPFLAGS += -DC_DARWIN
endif
ifeq ($(TARGET_OS),AIX)
   CPPFLAGS += -DC_AIX
endif
ifeq ($(TARGET_OS), IRIX)
   CPPFLAGS += -DC_IRIX
endif
ifeq ($(TARGET_OS), Cygwin)
   CPPFLAGS += -DC_CYGWIN
endif
ifeq ($(TARGET_OS), FreeBSD)
   CPPFLAGS += -DC_BSD
endif
ifeq ($(TARGET_OS), OpenBSD)
   CPPFLAGS += -DC_BSD
endif
ifeq ($(TARGET_OS), NetBSD)
   CPPFLAGS += -DC_BSD
endif
ifeq ($(TARGET_OS), HP-UX)
   CPPFLAGS += -DC_HPUX
endif
ifeq ($(TARGET_OS), Interix)
  CPPFLAGS += -DC_INTERIX -DC_WINDOWS
endif
ifeq ($(TARGET_OS), Win32)
  CPPFLAGS += -DC_WINDOWS
endif
ifeq ($(TARGET_OS), mingw)
  CPPFLAGS += -DC_WINDOWS
endif
ifeq ($(TARGET_OS), Linux)
  CPPFLAGS += -DC_LINUX
endif

# -- except ARM all arches have WORDS_BIGENDIAN same as ENDIAN.
#  for ARM we'd need a runtime autoconf check to detect.
#  Only affects autoit.c, not defining this macro will partially disable it.

ifneq ($(ARCH),ARM)
ifeq ($(ENDIAN),little)
   CPPFLAGS += -DFPU_WORDS_BIGENDIAN=0
endif
ifeq ($(ENDIAN),big)
   CPPFLAGS += -DFPU_WORDS_BIGENDIAN=1
endif
endif


# -- endianness detection --
#  It is important to get this correctly, especially for Sparc.
#  If arch is little endian the code possibly does unaligned accesses, that would
#  cause a SIGBUS on Sparc.

ifeq ($(ENDIAN),little)
   CPPFLAGS += -DWORDS_BIGENDIAN=0
endif
ifeq ($(ENDIAN),big)
   CPPFLAGS += -DWORDS_BIGENDIAN=1
endif

include ../../Makefile.multisrc
#compare debug output
DIFFPROG := $(PROGDIR)/DiffOutput.sh "diff "

ifeq ($(TARGET_OS),Darwin)
EXTRA_LLIFLAGS += -load=libz.dylib
else
ifeq ($(TARGET_OS),Linux)
EXTRA_LLIFLAGS += -load=libz.so.1
else
EXTRA_LLIFLAGS += -load=libz
endif
endif
