## Makefile.am -- Process this file with automake to produce Makefile.in
##
##   Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
##   Free Software Foundation, Inc.
##   Written by Gary V. Vaughan, 2003
##
##   This file is part of GNU Libtool.
##
## GNU Libtool is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; either version 2 of
## the License, or (at your option) any later version.
##
## GNU Libtool is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with GNU Libtool; see the file COPYING.  If not, a copy
## can be downloaded from  http://www.gnu.org/licenses/gpl.html,
## or obtained by writing to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#####

ACLOCAL_AMFLAGS  	= -I libltdl/m4

AM_CPPFLAGS		=
AM_LDFLAGS		=

DIST_SUBDIRS		= .
EXTRA_DIST		=

BUILT_SOURCES		= libtool libtoolize

CLEANFILES		=
MOSTLYCLEANFILES	=
DISTCLEANFILES		=
MAINTAINERCLEANFILES	=

include_HEADERS		=
noinst_LTLIBRARIES	=
lib_LTLIBRARIES		=
EXTRA_LTLIBRARIES	=

auxdir			= libltdl/config
m4dir			= libltdl/m4

# Using `cd' in backquotes may print the directory name, use this instead:
lt__cd		= CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd

MKSTAMP = $(SHELL) $(srcdir)/$(auxdir)/mkstamp

timestamp = set dummy `$(MKSTAMP) $(srcdir)`; shift; \
	case $(VERSION) in \
	  *[acegikmoqsuwy]) TIMESTAMP=" $$1 $$2" ;; \
	  *) TIMESTAMP="" ;; \
	esac

rebuild = rebuild=:; $(timestamp); correctver=$$1


# ---------- #
# Bootstrap. #
# ---------- #

sh_files	= $(auxdir)/general.m4sh $(auxdir)/getopt.m4sh
EXTRA_DIST     += bootstrap $(srcdir)/libtoolize.in $(auxdir)/ltmain.m4sh \
		  $(auxdir)/mkstamp $(sh_files) \
		  ChangeLog.1996 ChangeLog.1997 ChangeLog.1998 \
		  ChangeLog.1999 ChangeLog.2000 ChangeLog.2001 \
		  ChangeLog.2002 ChangeLog.2003 ChangeLog.2004 \
		  ChangeLog.2005 ChangeLog.2006 ChangeLog.2007 \
		  ChangeLog.2008 ChangeLog.2009 ChangeLog.2010
CLEANFILES     += libtool libtoolize libtoolize.tmp \
		  $(auxdir)/ltmain.tmp $(m4dir)/ltversion.tmp

## These are the replacements that need to be made at bootstrap time,
## because they must be static in distributed files, and not accidentally
## changed by configure running on the build machine.
bootstrap_edit  = sed \
		  -e 's,@MACRO_VERSION\@,$(VERSION),g' \
		  -e "s,@MACRO_REVISION\@,$$correctver,g" \
		  -e "s,@MACRO_SERIAL\@,$$serial,g" \
		  -e 's,@PACKAGE\@,$(PACKAGE),g' \
		  -e 's,@PACKAGE_BUGREPORT\@,$(PACKAGE_BUGREPORT),g' \
		  -e 's,@PACKAGE_URL\@,$(PACKAGE_URL),g' \
		  -e 's,@PACKAGE_NAME\@,$(PACKAGE_NAME),g' \
		  -e "s,@package_revision\@,$$correctver,g" \
		  -e 's,@PACKAGE_STRING\@,$(PACKAGE_NAME) $(VERSION),g' \
		  -e 's,@PACKAGE_TARNAME\@,$(PACKAGE),g' \
		  -e 's,@PACKAGE_VERSION\@,$(VERSION),g' \
		  -e "s,@TIMESTAMP\@,$$TIMESTAMP,g" \
		  -e 's,@VERSION\@,$(VERSION),g'

## We build ltversion.m4 here, instead of from config.status,
## because config.status is rerun each time one of configure's
## dependencies change and ltversion.m4 happens to be a configure
## dependency.  configure and ltversion.m4 would be rebuilt in
## a loop otherwise.
## We used to do this with a 'stamp-vcl' file, but non-gmake builds
## would rerun configure on every invocation, so now we manually
## check the version numbers from the build rule when necessary.
## Use `$(srcdir)/m4' for the benefit of non-GNU makes: this is
## how ltversion.m4 appears in our dependencies.
EXTRA_DIST += $(m4dir)/ltversion.in $(srcdir)/$(m4dir)/ltversion.m4
$(srcdir)/$(m4dir)/ltversion.m4: $(m4dir)/ltversion.in configure.ac ChangeLog
	@target='$(srcdir)/$(m4dir)/ltversion.m4'; $(rebuild); \
	if test -f "$$target"; then \
	  set dummy `sed -n '/^# serial /p' "$$target"`; shift; \
	  actualver=1.$$3; \
	  test "$$actualver" = "$$correctver" && rebuild=false; \
	fi; \
	for prereq in $?; do \
	  case $$prereq in *ChangeLog | *configure.ac);; *) rebuild=:;; esac; \
	done; \
	if $$rebuild; then \
	  cd $(srcdir); \
	  rm -f $(m4dir)/ltversion.tmp; \
	  serial=`echo "$$correctver" | sed 's,^1[.],,g'`; \
	  echo $(bootstrap_edit) \
	    $(srcdir)/$(m4dir)/ltversion.in \> $(srcdir)/$(m4dir)/ltversion.m4; \
	  $(bootstrap_edit) \
		  $(m4dir)/ltversion.in > $(m4dir)/ltversion.tmp; \
	  chmod a-w $(m4dir)/ltversion.tmp; \
	  mv -f $(m4dir)/ltversion.tmp $(m4dir)/ltversion.m4; \
	fi

## And for similar reasons, ltmain.sh can't be built from config.status.
## We used to do this with a 'stamp-vcl' file, but non-gmake builds
## would rerun configure on every invocation, so now we manually
## check the version numbers from the build rule when necessary.
## !WARNING! If you edit this rule to change the contents of ltmain.sh,
##           you must `touch $(srcdir)/$(auxdir)/ltmain.in' from the
##           shell if you need ltmain.sh to be regenerated.  Ideally, we
##           should make this rule depend on Makefile but that will break
##           distcheck (at least) by rebuilding ltmain.sh in the source
##           tree whenever config.status regenerates the Makefile.
EXTRA_DIST += $(srcdir)/$(auxdir)/ltmain.sh
$(srcdir)/$(auxdir)/ltmain.sh: $(sh_files) $(auxdir)/ltmain.m4sh configure.ac ChangeLog
	@target='$(srcdir)/$(auxdir)/ltmain.sh'; $(rebuild); \
	if test -f "$$target"; then \
	  eval `sed -n '/^package_revision=/p' "$$target"`; \
	  actualver=$$package_revision; \
	  test "$$actualver" = "$$correctver" && rebuild=false; \
	fi; \
	for prereq in $?; do \
	  case $$prereq in *ChangeLog);; *) rebuild=:;; esac; \
	done; \
	if $$rebuild; then \
	  cd $(srcdir); \
	  rm -f $(auxdir)/ltmain.in $(auxdir)/ltmain.tmp \
	    $(auxdir)/ltmain.sh; \
	  echo $(M4SH) -B $(auxdir) $(auxdir)/ltmain.m4sh \
	    \> $(auxdir)/ltmain.in; \
	  $(M4SH) -B $(auxdir) $(auxdir)/ltmain.m4sh \
	    > $(auxdir)/ltmain.in; \
	  echo $(bootstrap_edit) \
	    $(srcdir)/$(auxdir)/ltmain.in "> $$target"; \
	  $(bootstrap_edit) -e '/^: \$${.*="@.*@"}$$/d' \
	    $(auxdir)/ltmain.in > $(auxdir)/ltmain.tmp; \
	  rm -f $(auxdir)/ltmain.in; \
	  chmod a-w $(auxdir)/ltmain.tmp; \
	  mv -f $(auxdir)/ltmain.tmp $(auxdir)/ltmain.sh; \
	fi

# Use `$(srcdir)' for the benefit of non-GNU makes: this is
# how libtoolize.in appears in our dependencies.
EXTRA_DIST += libtoolize.m4sh
$(srcdir)/libtoolize.in: $(sh_files) libtoolize.m4sh Makefile.am
	cd $(srcdir); \
	rm -f libtoolize.in libtoolize.tmp; \
	$(M4SH) -B $(auxdir) libtoolize.m4sh > libtoolize.tmp; \
	$(bootstrap_edit) libtoolize.tmp > libtoolize.in; \
	rm -f libtoolize.tmp

$(srcdir)/libltdl/Makefile.am: $(srcdir)/libltdl/Makefile.inc
	cd $(srcdir); \
	in=libltdl/Makefile.inc; out=libltdl/Makefile.am; \
	rm -f $$out; \
	( $(SED) -n '1,/^.. DO NOT REMOVE THIS LINE -- /p' $$in; \
	{ echo 'ACLOCAL_AMFLAGS = -I m4'; \
	  echo 'AUTOMAKE_OPTIONS = foreign'; \
	  echo 'AM_CPPFLAGS ='; \
	  echo 'AM_LDFLAGS ='; \
	  echo 'BUILT_SOURCES ='; \
	  echo 'include_HEADERS ='; \
	  echo 'noinst_LTLIBRARIES ='; \
	  echo 'lib_LTLIBRARIES ='; \
	  echo 'EXTRA_LTLIBRARIES ='; \
	  echo 'EXTRA_DIST ='; \
	  echo 'CLEANFILES ='; \
	  echo 'MOSTLYCLEANFILES ='; \
	}; \
	$(SED) -n '/^.. DO NOT REMOVE THIS LINE -- /,$$p' $$in | \
	$(SED) -e 's,libltdl_,,; s,libltdl/,,; s,: libltdl/,: ,' \
	       -e 's,\$$(libltdl_,$$(,' \
	) | \
	$(SED) -e '/^.. DO NOT REMOVE THIS LINE -- /d' \
	       -e '1s,^\(.. Makefile.\)inc.*,\1am -- Process this file with automake to produce Makefile.in,' > $$out;
	chmod a-w $(srcdir)/libltdl/Makefile.am


## Unfortunately, all this bogeyness means that we have to manually
## keep the generated files in libltdl up to date.
LTDL_BOOTSTRAP_DEPS = $(srcdir)/libltdl/aclocal.m4 \
	$(srcdir)/libltdl/stamp-mk \
	$(srcdir)/libltdl/configure \
	$(srcdir)/libltdl/config-h.in

all-local: $(LTDL_BOOTSTRAP_DEPS)


## ---------------- ##
## Libtool scripts. ##
## ---------------- ##

configure_edit = sed \
	-e 's,@aclocal_DATA\@,$(aclocalfiles),g' \
	-e 's,@aclocaldir\@,$(aclocaldir),g' \
	-e 's,@datadir\@,$(datadir),g' \
	-e 's,@EGREP\@,$(EGREP),g' \
	-e 's,@FGREP\@,$(FGREP),g' \
	-e 's,@GREP\@,$(GREP),g' \
	-e 's,@host_triplet\@,$(host_triplet),g' \
	-e 's,@LN_S\@,$(LN_S),g' \
	-e "s,@pkgconfig_files\@,$(auxfiles),g" \
	-e 's,@pkgdatadir\@,$(pkgdatadir),g' \
	-e "s,@pkgltdl_files\@,$(ltdldatafiles),g" \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@SED\@,$(SED),g'

# The libtool distributor and the standalone libtool script.
bin_SCRIPTS = libtoolize libtool

libtoolize: $(srcdir)/libtoolize.in $(top_builddir)/config.status
	rm -f libtoolize.tmp libtoolize
	$(configure_edit) \
		$(srcdir)/libtoolize.in > libtoolize.tmp
	chmod a+x libtoolize.tmp
	chmod a-w libtoolize.tmp
	mv -f libtoolize.tmp libtoolize

# We used to do this with a 'stamp-vcl' file, but non-gmake builds
# would rerun configure on every invocation, so now we manually
# check the version numbers from the build rule when necessary.
libtool: $(top_builddir)/config.status $(srcdir)/$(auxdir)/ltmain.sh ChangeLog
	@target=libtool; $(rebuild); \
	if test -f "$$target"; then \
	  set dummy `./$$target --version | sed 1q`; actualver="$$5"; \
	  test "$$actualver" = "$$correctver" && rebuild=false; \
	fi; \
	for prereq in $?; do \
	  case $$prereq in *ChangeLog);; *) rebuild=:;; esac; \
	done; \
	if $$rebuild; then \
	  echo $(SHELL) ./config.status $$target; \
	  cd $(top_builddir) && $(SHELL) ./config.status $$target; \
	fi

.PHONY: configure-subdirs
configure-subdirs distdir: $(DIST_MAKEFILE_LIST)
@DIST_MAKEFILE_LIST@:
	dir=`echo $@ | sed 's,^[^/]*$$,.,;s,/[^/]*$$,,'`; \
	test -d $$dir || mkdir $$dir || exit 1; \
	abs_srcdir=`$(lt__cd) $(srcdir) && pwd`; \
	(cd $$dir && $$abs_srcdir/$$dir/configure --with-dist) || exit 1


## -------- ##
## Libltdl. ##
## -------- ##

include libltdl/Makefile.inc

EXTRA_DIST += $(srcdir)/libltdl/stamp-mk $(m4dir)/lt~obsolete.m4

$(srcdir)/libltdl/Makefile.in: $(srcdir)/libltdl/Makefile.am \
	    $(srcdir)/libltdl/aclocal.m4 
	cd $(srcdir)/libltdl && $(AUTOMAKE) Makefile

$(srcdir)/libltdl/stamp-mk: $(srcdir)/libltdl/Makefile.in
	cd $(srcdir)/libltdl && \
	sed -e 's,config/mdate-sh,,' -e 's,config/texinfo.tex,,' \
	    -e 's,config/mkinstalldirs,,' \
	    < Makefile.in > Makefile.inT && \
	mv -f Makefile.inT Makefile.in
	echo stamp > $@

sub_aclocal_m4_deps = \
	$(srcdir)/libltdl/configure.ac \
	$(m4dir)/libtool.m4 \
	$(m4dir)/ltoptions.m4 \
	$(m4dir)/ltdl.m4 \
	$(srcdir)/$(m4dir)/ltversion.m4 \
	$(m4dir)/ltsugar.m4 \
	$(m4dir)/argz.m4 \
	$(m4dir)/lt~obsolete.m4

sub_configure_deps = $(sub_aclocal_m4_deps) $(srcdir)/libltdl/aclocal.m4

$(srcdir)/libltdl/aclocal.m4: $(sub_aclocal_m4_deps)
	cd $(srcdir)/libltdl && $(ACLOCAL) -I m4

$(srcdir)/libltdl/configure: $(sub_configure_deps)
	cd $(srcdir)/libltdl && $(AUTOCONF)

$(srcdir)/libltdl/config-h.in: $(sub_configure_deps)
	cd $(srcdir)/libltdl && $(AUTOHEADER)
	touch $@


## -------------- ##
## Documentation. ##
## -------------- ##

info_TEXINFOS		= doc/libtool.texi
doc_libtool_TEXINFOS	= doc/PLATFORMS doc/fdl.texi doc/notes.texi
EXTRA_DIST	       += $(srcdir)/doc/notes.txt
## Workaround for a texinfo bug described here:
## http://thread.gmane.org/gmane.comp.tex.texinfo.bugs/3930
DISTCLEANFILES	       += libtool.dvi

all-local: $(srcdir)/doc/notes.txt

$(srcdir)/doc/notes.txt: $(srcdir)/doc/notes.texi
	cd $(srcdir)/doc && \
	  $(MAKEINFO) --no-headers $(MAKEINFOFLAGS) -o notes.txt notes.texi

dist_man1_MANS		= $(srcdir)/doc/libtool.1 $(srcdir)/doc/libtoolize.1
MAINTAINERCLEANFILES	+= $(dist_man1_MANS)
update_mans = \
  PATH=".$(PATH_SEPARATOR)$$PATH"; export PATH; \
  $(HELP2MAN) --output=$@
$(srcdir)/doc/libtool.1: $(srcdir)/$(auxdir)/ltmain.sh
	$(update_mans) --help-option=--help-all libtool
$(srcdir)/doc/libtoolize.1: $(srcdir)/libtoolize.in
	$(update_mans) libtoolize


## ------------- ##
## Installation. ##
## ------------- ##

# These are required by libtoolize and must be executable when installed.
# The timestamps on these files must be preserved carefully so we install,
# uninstall and set executable with custom rules here.
auxexefiles	= config/compile config/config.guess config/config.sub \
		  config/depcomp config/install-sh config/missing
auxfiles	= $(auxexefiles) config/ltmain.sh

# Everything that gets picked up by aclocal is automatically distributed,
# this is the list of macro files we install on the user's system.
aclocalfiles	= m4/argz.m4 m4/libtool.m4 m4/ltdl.m4 m4/ltoptions.m4 \
		  m4/ltsugar.m4 m4/ltversion.m4 m4/lt~obsolete.m4

## These are installed as a subdirectory of pkgdatadir so that
## libtoolize --ltdl can find them later:
ltdldatafiles	= libltdl/COPYING.LIB \
		  libltdl/README \
		  libltdl/Makefile.inc \
		  libltdl/Makefile.am \
		  libltdl/configure.ac \
		  libltdl/aclocal.m4 \
		  libltdl/Makefile.in \
		  libltdl/config-h.in \
		  libltdl/configure \
		  libltdl/argz_.h \
		  libltdl/argz.c \
		  libltdl/loaders/dld_link.c \
		  libltdl/loaders/dlopen.c \
		  libltdl/loaders/dyld.c \
		  libltdl/loaders/load_add_on.c \
		  libltdl/loaders/loadlibrary.c \
		  libltdl/loaders/shl_load.c \
		  libltdl/lt__dirent.c \
		  libltdl/lt__strl.c \
		  $(libltdl_libltdl_la_SOURCES)

install-data-local: libltdl/Makefile.in
	@$(NORMAL_INSTALL)
## Don't install over the top of an old pkgdatadir
	-rm -rf $(DESTDIR)$(pkgdatadir)/*
## To avoid spurious reconfiguration when the user installs these files
## with libtoolize, we have to install them in the correct order:
## First, put a copy of the libtool m4 macros in the aclocal dir
	$(mkinstalldirs) $(DESTDIR)$(aclocaldir)
	@list='$(aclocalfiles)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's|^.*/||'`; \
	  echo " $(INSTALL_DATA) '$(srcdir)/$(m4dir)/$$f' '$(DESTDIR)$(aclocaldir)/$$f'"; \
	  $(INSTALL_DATA) "$(srcdir)/$(m4dir)/$$f" "$(DESTDIR)$(aclocaldir)/$$f"; \
	done
## install the helper scripts
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/config
	@list='$(auxexefiles)' && for p in $$list; do \
	  echo " $(INSTALL_SCRIPT) '$(srcdir)/libltdl/$$p' '$(DESTDIR)$(pkgdatadir)/$$p'"; \
	  $(INSTALL_SCRIPT) "$(srcdir)/libltdl/$$p" "$(DESTDIR)$(pkgdatadir)/$$p"; \
	done
	$(INSTALL_DATA) "$(srcdir)/libltdl/config/ltmain.sh" "$(DESTDIR)$(pkgdatadir)/config/ltmain.sh" 
## install the libltdl files
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/libltdl
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/libltdl/libltdl
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/libltdl/loaders
	@list='$(ltdldatafiles)' && for p in $$list; do \
	  echo " $(INSTALL_DATA) '$(srcdir)/$$p' '$(DESTDIR)$(pkgdatadir)/$$p'"; \
	  $(INSTALL_DATA) "$(srcdir)/$$p" "$(DESTDIR)$(pkgdatadir)/$$p"; \
	done
	-chmod a+x $(DESTDIR)$(pkgdatadir)/libltdl/configure


## ------------- ##
## Distribution. ##
## ------------- ##

edit_readme_alpha = $(auxdir)/edit-readme-alpha

EXTRA_DIST += $(edit_readme_alpha)

uninstall-hook:
	@$(NORMAL_UNINSTALL)
	@list='$(ltdldatafiles) $(auxfiles)'; for f in $$list; do \
	  echo " rm -f '$(DESTDIR)$(pkgdatadir)/$$f'"; \
	  rm -f "$(DESTDIR)$(pkgdatadir)/$$f"; \
	done
	@for p in $(aclocalfiles); do \
	  f=`echo "$$p" | sed 's|^.*/||'`; \
	  echo " rm -f '$(DESTDIR)$(aclocaldir)/$$f'"; \
	  rm -f "$(DESTDIR)$(aclocaldir)/$$f"; \
	done

dist-hook:
## Edit the README file for alpha releases.
	case $(VERSION) in \
	  *[a-z]) $(SHELL) $(srcdir)/$(edit_readme_alpha) $(distdir)/README ;; \
	esac
## Ensure aclocal has not wrongly picked up old macro definitions.
	for macro in LT_INIT AC_PROG_LIBTOOL AM_PROG_LIBTOOL; do \
	  if grep $$macro $(srcdir)/aclocal.m4 $(srcdir)/libltdl/aclocal.m4; then \
	    echo "Bogus $$macro macro contents in an aclocal.m4 file." >&2; \
	    exit 1; \
	  else :; fi; \
	done


## ----------- ##
## Test suite. ##
## ----------- ##

# The testsuite files are evaluated in the order given here.
TESTSUITE	= tests/testsuite
TESTSUITE_AT	= tests/testsuite.at \
		  tests/getopt-m4sh.at \
		  tests/libtoolize.at \
		  tests/help.at \
		  tests/duplicate_members.at \
		  tests/duplicate_conv.at \
		  tests/duplicate_deps.at \
		  tests/flags.at \
		  tests/inherited_flags.at \
		  tests/convenience.at \
		  tests/link-order.at \
		  tests/link-order2.at \
		  tests/fail.at \
		  tests/shlibpath.at \
		  tests/runpath-in-lalib.at \
		  tests/static.at \
		  tests/export.at \
		  tests/search-path.at \
		  tests/indirect_deps.at \
		  tests/archive-in-archive.at \
		  tests/exeext.at \
		  tests/execute-mode.at \
		  tests/bindir.at \
		  tests/cwrapper.at \
		  tests/deplib-in-subdir.at \
		  tests/infer-tag.at \
		  tests/localization.at \
		  tests/nocase.at \
		  tests/install.at \
		  tests/versioning.at \
		  tests/destdir.at \
		  tests/old-m4-iface.at \
		  tests/am-subdir.at \
		  tests/lt_dlexit.at \
		  tests/lt_dladvise.at \
		  tests/lt_dlopen.at \
		  tests/lt_dlopen_a.at \
		  tests/lt_dlopenext.at \
		  tests/ltdl-libdir.at \
		  tests/ltdl-api.at \
		  tests/dlloader-api.at \
		  tests/loadlibrary.at \
		  tests/lalib-syntax.at \
		  tests/resident.at \
		  tests/slist.at \
		  tests/need_lib_prefix.at \
		  tests/standalone.at \
		  tests/subproject.at \
		  tests/nonrecursive.at \
		  tests/recursive.at \
		  tests/template.at \
		  tests/ctor.at \
		  tests/exceptions.at \
		  tests/early-libtool.at \
		  tests/with-pic.at \
		  tests/no-executables.at \
		  tests/deplibs-ident.at \
		  tests/configure-iface.at \
		  tests/stresstest.at \
		  tests/cmdline_wrap.at \
		  tests/pic_flag.at \
		  tests/darwin.at \
		  tests/dumpbin-symbols.at \
		  tests/deplibs-mingw.at \
		  tests/sysroot.at

EXTRA_DIST     += $(srcdir)/$(TESTSUITE) $(TESTSUITE_AT) $(srcdir)/tests/package.m4

# Be sure to reexport important environment variables:
TESTS_ENVIRONMENT = MAKE="$(MAKE)" CC="$(CC)" CFLAGS="$(CFLAGS)" \
	CPP="$(CPP)" CPPFLAGS="$(CPPFLAGS)" LD="$(LD)" LDFLAGS="$(LDFLAGS)" \
	LIBS="$(LIBS)" LN_S="$(LN_S)" NM="$(NM)" RANLIB="$(RANLIB)" \
	AR="$(AR)" \
	M4SH="$(M4SH)" SED="$(SED)" STRIP="$(STRIP)" lt_INSTALL="$(INSTALL)" \
	MANIFEST_TOOL="$(MANIFEST_TOOL)" \
	OBJEXT="$(OBJEXT)" EXEEXT="$(EXEEXT)" \
	SHELL="$(SHELL)" CONFIG_SHELL="$(SHELL)" \
	CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" CXXCPP="$(CXXCPP)" \
	F77="$(F77)" FFLAGS="$(FFLAGS)" \
	FC="$(FC)" FCFLAGS="$(FCFLAGS)" \
	GCJ="$(GCJ)" GCJFLAGS="$(GCJFLAGS)" \
	lt_cv_to_host_file_cmd="$(to_host_file_cmd)" \
	lt_cv_to_tool_file_cmd="$(to_tool_file_cmd)"

BUILDCHECK_ENVIRONMENT = _lt_pkgdatadir="$(abs_top_srcdir)" \
	LIBTOOLIZE="$(abs_top_builddir)/libtoolize" \
	LIBTOOL="$(abs_top_builddir)/libtool" \
	tst_aclocaldir="$(abs_top_srcdir)/libltdl/m4"

INSTALLCHECK_ENVIRONMENT = \
	LIBTOOLIZE="$(bindir)/`echo libtoolize | sed '$(program_transform_name)'`" \
	LIBTOOL="$(bindir)/`echo libtool | sed '$(program_transform_name)'`" \
	LTDLINCL="-I$(includedir)" \
	LIBLTDL="$(libdir)/libltdl.la" \
	tst_aclocaldir="$(aclocaldir)"

# Use `$(srcdir)' for the benefit of non-GNU makes: this is
# how `testsuite' appears in our dependencies.
$(srcdir)/$(TESTSUITE): $(srcdir)/tests/package.m4 $(TESTSUITE_AT) Makefile.am
	cd $(srcdir)/tests && \
	$(AUTOTEST) `echo $(TESTSUITE_AT) | sed 's,tests/,,g'` -o testsuite.tmp && \
	mv -f testsuite.tmp testsuite

$(srcdir)/tests/package.m4: $(srcdir)/configure.ac Makefile.am
	{ \
	  echo '# Signature of the current package.'; \
	  echo 'm4_define([AT_PACKAGE_NAME],      [@PACKAGE_NAME@])'; \
	  echo 'm4_define([AT_PACKAGE_TARNAME],   [@PACKAGE_TARNAME@])'; \
	  echo 'm4_define([AT_PACKAGE_VERSION],   [@PACKAGE_VERSION@])'; \
	  echo 'm4_define([AT_PACKAGE_STRING],    [@PACKAGE_STRING@])'; \
	  echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	  echo 'm4_define([AT_PACKAGE_URL],       [@PACKAGE_URL@])'; \
	} | $(bootstrap_edit) > $(srcdir)/tests/package.m4

tests/atconfig: $(top_builddir)/config.status
	$(SHELL) ./config.status tests/atconfig

DISTCLEANFILES += tests/atconfig

CD_TESTDIR	= abs_srcdir=`$(lt__cd) $(srcdir) && pwd`; cd tests

testsuite_deps = tests/atconfig $(srcdir)/$(TESTSUITE)
testsuite_deps_uninstalled = $(testsuite_deps) libltdl/libltdlc.la \
			     $(bin_SCRIPTS) $(LTDL_BOOTSTRAP_DEPS)

# Hook the test suite into the check rule
check-local: $(testsuite_deps_uninstalled)
	$(CD_TESTDIR); \
	CONFIG_SHELL="$(SHELL)" $(SHELL) $$abs_srcdir/$(TESTSUITE) \
	  $(TESTS_ENVIRONMENT) $(BUILDCHECK_ENVIRONMENT) $(TESTSUITEFLAGS)

# Run the test suite on the *installed* tree.
installcheck-local: $(testsuite_deps)
	$(CD_TESTDIR); \
	CONFIG_SHELL="$(SHELL)" $(SHELL) $$abs_srcdir/$(TESTSUITE) \
	  $(TESTS_ENVIRONMENT) $(INSTALLCHECK_ENVIRONMENT) $(TESTSUITEFLAGS) \
	  AUTOTEST_PATH="$(exec_prefix)/bin"

check-noninteractive-old:
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS TESTS='$(NONINTERACTIVE_TESTS)'
check-interactive-old:
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS TESTS='$(INTERACTIVE_TESTS)'

# Run only noninteractive parts of the new testsuite.
check-noninteractive-new: $(testsuite_deps_uninstalled)
	$(CD_TESTDIR); \
	CONFIG_SHELL="$(SHELL)" $(SHELL) $$abs_srcdir/$(TESTSUITE) \
	  $(TESTS_ENVIRONMENT) $(BUILDCHECK_ENVIRONMENT) \
	  -k !interactive INNER_TESTSUITEFLAGS=",!interactive" \
	  $(TESTSUITEFLAGS)

# Run only interactive parts of the new testsuite.
check-interactive-new: $(testsuite_deps_uninstalled)
	$(CD_TESTDIR); \
	CONFIG_SHELL="$(SHELL)" $(SHELL) $$abs_srcdir/$(TESTSUITE) \
	  $(TESTS_ENVIRONMENT) $(BUILDCHECK_ENVIRONMENT) \
	  -k interactive -k recursive INNER_TESTSUITEFLAGS=",interactive" \
	  $(TESTSUITEFLAGS)

check-interactive: check-interactive-old check-interactive-new
check-noninteractive: check-noninteractive-old check-noninteractive-new

# We need to remove any file droppings left behind by testsuite
clean-local: clean-local-legacy
	-$(CD_TESTDIR); \
	test -f $$abs_srcdir/$(TESTSUITE) && \
	    $(SHELL) $$abs_srcdir/$(TESTSUITE) --clean


## ------------- ##
## Legacy tests. ##
## ------------- ##

# !WARNING! Don't add any new tests here, we are migrating to an
#           Autotest driven framework, please add new test cases
#           using the new framework above.  When the migration is
#           complete this section should be removed.

CXX_TESTS = \
	tests/tagdemo-static.test \
	tests/tagdemo-static-make.test \
	tests/tagdemo-static-exec.test \
	tests/tagdemo-conf.test \
	tests/tagdemo-make.test \
	tests/tagdemo-exec.test \
	tests/tagdemo-shared.test \
	tests/tagdemo-shared-make.test \
	tests/tagdemo-shared-exec.test \
	tests/tagdemo-undef.test \
	tests/tagdemo-undef-make.test \
	tests/tagdemo-undef-exec.test

tests/tagdemo-undef-exec.log:	tests/tagdemo-undef-make.log
tests/tagdemo-undef-make.log:	tests/tagdemo-undef.log
tests/tagdemo-undef.log:	tests/tagdemo-shared-exec.log
tests/tagdemo-shared-exec.log:	tests/tagdemo-shared-make.log
tests/tagdemo-shared-make.log:	tests/tagdemo-shared.log
tests/tagdemo-shared.log:	tests/tagdemo-exec.log
tests/tagdemo-exec.log:		tests/tagdemo-make.log
tests/tagdemo-make.log:		tests/tagdemo-conf.log
tests/tagdemo-conf.log:		tests/tagdemo-static-exec.log
tests/tagdemo-static-exec.log:	tests/tagdemo-static-make.log
tests/tagdemo-static-make.log:	tests/tagdemo-static.log

F77_TESTS = \
	tests/f77demo-static.test \
	tests/f77demo-static-make.test \
	tests/f77demo-static-exec.test \
	tests/f77demo-conf.test \
	tests/f77demo-make.test \
	tests/f77demo-exec.test \
	tests/f77demo-shared.test \
	tests/f77demo-shared-make.test \
	tests/f77demo-shared-exec.test

tests/f77demo-shared-exec.log:	tests/f77demo-shared-make.log
tests/f77demo-shared-make.log:	tests/f77demo-shared.log
tests/f77demo-shared.log:	tests/f77demo-exec.log
tests/f77demo-exec.log:		tests/f77demo-make.log
tests/f77demo-make.log:		tests/f77demo-conf.log
tests/f77demo-conf.log:		tests/f77demo-static-exec.log
tests/f77demo-static-exec.log:	tests/f77demo-static-make.log
tests/f77demo-static-make.log:	tests/f77demo-static.log

FC_TESTS = \
	tests/fcdemo-static.test \
	tests/fcdemo-static-make.test \
	tests/fcdemo-static-exec.test \
	tests/fcdemo-conf.test \
	tests/fcdemo-make.test \
	tests/fcdemo-exec.test \
	tests/fcdemo-shared.test \
	tests/fcdemo-shared-make.test \
	tests/fcdemo-shared-exec.test

tests/fcdemo-shared-exec.log:	tests/fcdemo-shared-make.log
tests/fcdemo-shared-make.log:	tests/fcdemo-shared.log
tests/fcdemo-shared.log:	tests/fcdemo-exec.log
tests/fcdemo-exec.log:		tests/fcdemo-make.log
tests/fcdemo-make.log:		tests/fcdemo-conf.log
tests/fcdemo-conf.log:		tests/fcdemo-static-exec.log
tests/fcdemo-static-exec.log:	tests/fcdemo-static-make.log
tests/fcdemo-static-make.log:	tests/fcdemo-static.log

COMMON_TESTS = \
	tests/link.test \
	tests/link-2.test \
	tests/nomode.test \
	tests/objectlist.test \
	tests/quote.test \
	tests/sh.test \
	tests/suffix.test \
	tests/tagtrace.test \
	tests/cdemo-static.test \
	tests/cdemo-static-make.test \
	tests/cdemo-static-exec.test \
	tests/demo-static.test \
	tests/demo-static-make.test \
	tests/demo-static-exec.test \
	tests/demo-static-inst.test \
	tests/demo-static-unst.test \
	tests/depdemo-static.test \
	tests/depdemo-static-make.test \
	tests/depdemo-static-exec.test \
	tests/depdemo-static-inst.test \
	tests/depdemo-static-unst.test \
	tests/mdemo-static.test \
	tests/mdemo-static-make.test \
	tests/mdemo-static-exec.test \
	tests/mdemo-static-inst.test \
	tests/mdemo-static-unst.test \
	tests/cdemo-conf.test \
	tests/cdemo-make.test \
	tests/cdemo-exec.test \
	tests/demo-conf.test \
	tests/demo-make.test \
	tests/demo-exec.test \
	tests/demo-inst.test \
	tests/demo-unst.test \
	tests/demo-deplibs.test \
	tests/depdemo-conf.test \
	tests/depdemo-make.test \
	tests/depdemo-exec.test \
	tests/depdemo-inst.test \
	tests/depdemo-unst.test \
	tests/mdemo-conf.test \
	tests/mdemo-make.test \
	tests/mdemo-exec.test \
	tests/mdemo-inst.test \
	tests/mdemo-unst.test \
	tests/mdemo-dryrun.test \
	tests/mdemo2-conf.test \
	tests/mdemo2-make.test \
	tests/mdemo2-exec.test \
	tests/pdemo-conf.test \
	tests/pdemo-make.test \
	tests/pdemo-exec.test \
	tests/pdemo-inst.test \
	tests/demo-nofast.test \
	tests/demo-nofast-make.test \
	tests/demo-nofast-exec.test \
	tests/demo-nofast-inst.test \
	tests/demo-nofast-unst.test \
	tests/depdemo-nofast.test \
	tests/depdemo-nofast-make.test \
	tests/depdemo-nofast-exec.test \
	tests/depdemo-nofast-inst.test \
	tests/depdemo-nofast-unst.test \
	tests/demo-pic.test \
	tests/demo-pic-make.test \
	tests/demo-pic-exec.test \
	tests/demo-nopic.test \
	tests/demo-nopic-make.test \
	tests/demo-nopic-exec.test \
	tests/cdemo-shared.test \
	tests/cdemo-shared-make.test \
	tests/cdemo-shared-exec.test \
	tests/mdemo-shared.test \
	tests/mdemo-shared-make.test \
	tests/mdemo-shared-exec.test \
	tests/mdemo-shared-inst.test \
	tests/mdemo-shared-unst.test \
	tests/cdemo-undef.test \
	tests/cdemo-undef-make.test \
	tests/cdemo-undef-exec.test

# Actually, only demo-relink and depdemo-relink require interaction,
# but they depend on the other tests being run beforehand.
INTERACTIVE_TESTS = \
	tests/demo-shared.test \
	tests/demo-shared-make.test \
	tests/demo-shared-exec.test \
	tests/demo-shared-inst.test \
	tests/demo-hardcode.test \
	tests/demo-relink.test \
	tests/demo-noinst-link.test \
	tests/demo-shared-unst.test \
	tests/depdemo-shared.test \
	tests/depdemo-shared-make.test \
	tests/depdemo-shared-exec.test \
	tests/depdemo-shared-inst.test \
	tests/depdemo-relink.test \
	tests/depdemo-shared-unst.test

tests/cdemo-undef-exec.log:	tests/cdemo-undef-make.log
tests/cdemo-undef-make.log:	tests/cdemo-undef.log
tests/cdemo-undef.log: @ORDER@	tests/cdemo-shared-exec.log
tests/cdemo-shared-exec.log:	tests/cdemo-shared-make.log
tests/cdemo-shared-make.log:	tests/cdemo-shared.log
tests/cdemo-shared.log: @ORDER@	tests/cdemo-exec.log
tests/cdemo-exec.log:		tests/cdemo-make.log
tests/cdemo-make.log:		tests/cdemo-conf.log
tests/cdemo-conf.log: @ORDER@	tests/cdemo-static-exec.log
tests/cdemo-static-exec.log:	tests/cdemo-static-make.log
tests/cdemo-static-make.log:	tests/cdemo-static.log

tests/demo-shared-unst.log:	tests/demo-noinst-link.log
tests/demo-noinst-link.log:	tests/demo-relink.log
tests/demo-relink.log:		tests/demo-hardcode.log
tests/demo-hardcode.log:	tests/demo-shared-inst.log
tests/demo-shared-inst.log:	tests/demo-shared-exec.log
tests/demo-shared-exec.log:	tests/demo-shared-make.log
tests/demo-shared-make.log:	tests/demo-shared.log
tests/demo-shared.log: @ORDER@	tests/demo-nopic-exec.log
tests/demo-nopic-exec.log:	tests/demo-nopic-make.log
tests/demo-nopic-make.log:	tests/demo-nopic.log
tests/demo-nopic.log: @ORDER@	tests/demo-pic-exec.log
tests/demo-pic-exec.log:	tests/demo-pic-make.log
tests/demo-pic-make.log:	tests/demo-pic.log
tests/demo-pic.log: @ORDER@	tests/demo-nofast-unst.log
tests/demo-nofast-unst.log:	tests/demo-nofast-inst.log
tests/demo-nofast-inst.log:	tests/demo-nofast-exec.log
tests/demo-nofast-exec.log:	tests/demo-nofast-make.log
tests/demo-nofast-make.log:	tests/demo-nofast.log
tests/demo-nofast.log: @ORDER@	tests/demo-deplibs.log
tests/demo-deplibs.log:		tests/demo-unst.log
tests/demo-unst.log:		tests/demo-inst.log
tests/demo-inst.log:		tests/demo-exec.log
tests/demo-exec.log:		tests/demo-make.log
tests/demo-make.log:		tests/demo-conf.log
tests/demo-conf.log: @ORDER@	tests/demo-static-unst.log
tests/demo-static-unst.log:	tests/demo-static-inst.log
tests/demo-static-inst.log:	tests/demo-static-exec.log
tests/demo-static-exec.log:	tests/demo-static-make.log
tests/demo-static-make.log:	tests/demo-static.log

tests/depdemo-shared-unst.log:	tests/depdemo-relink.log
tests/depdemo-relink.log:	tests/depdemo-shared-inst.log
tests/depdemo-shared-inst.log:	tests/depdemo-shared-exec.log
tests/depdemo-shared-exec.log:	tests/depdemo-shared-make.log
tests/depdemo-shared-make.log:	tests/depdemo-shared.log
tests/depdemo-shared.log: @ORDER@ tests/depdemo-nofast-unst.log
tests/depdemo-nofast-unst.log:	tests/depdemo-nofast-inst.log
tests/depdemo-nofast-inst.log:	tests/depdemo-nofast-exec.log
tests/depdemo-nofast-exec.log:	tests/depdemo-nofast-make.log
tests/depdemo-nofast-make.log:	tests/depdemo-nofast.log
tests/depdemo-nofast.log: @ORDER@ tests/depdemo-unst.log
tests/depdemo-unst.log:		tests/depdemo-inst.log
tests/depdemo-inst.log:		tests/depdemo-exec.log
tests/depdemo-exec.log:		tests/depdemo-make.log
tests/depdemo-make.log:		tests/depdemo-conf.log
tests/depdemo-conf.log: @ORDER@	tests/depdemo-static-unst.log
tests/depdemo-static-unst.log:	tests/depdemo-static-inst.log
tests/depdemo-static-inst.log:	tests/depdemo-static-exec.log
tests/depdemo-static-exec.log:	tests/depdemo-static-make.log
tests/depdemo-static-make.log:	tests/depdemo-static.log

tests/mdemo-shared-unst.log:	tests/mdemo-shared-inst.log
tests/mdemo-shared-inst.log:	tests/mdemo-shared-exec.log
tests/mdemo-shared-exec.log:	tests/mdemo-shared-make.log
tests/mdemo-shared-make.log:	tests/mdemo-shared.log
tests/mdemo-shared.log: @ORDER@	tests/mdemo-dryrun.log \
				tests/mdemo2-exec.log

tests/mdemo-dryrun.log:		tests/mdemo-unst.log
tests/mdemo-unst.log:		tests/mdemo-inst.log
tests/mdemo-inst.log:		tests/mdemo-exec.log
tests/mdemo-exec.log:		tests/mdemo-make.log
tests/mdemo-make.log:		tests/mdemo-conf.log
tests/mdemo-conf.log: @ORDER@	tests/mdemo-static-unst.log
tests/mdemo-static-unst.log:	tests/mdemo-static-inst.log
tests/mdemo-static-inst.log:	tests/mdemo-static-exec.log
tests/mdemo-static-exec.log:	tests/mdemo-static-make.log
tests/mdemo-static-make.log:	tests/mdemo-static.log

tests/mdemo2-exec.log:		tests/mdemo2-make.log
tests/mdemo2-make.log:		tests/mdemo2-conf.log \
				tests/mdemo-dryrun.log

tests/pdemo-inst.log:		tests/pdemo-exec.log
tests/pdemo-exec.log:		tests/pdemo-make.log
tests/pdemo-make.log:		tests/pdemo-conf.log


NONINTERACTIVE_TESTS = $(COMMON_TESTS)
if HAVE_CXX
NONINTERACTIVE_TESTS += $(CXX_TESTS)
endif
if HAVE_F77
# f77demo-static-exec.test might be interactive on MSYS.
INTERACTIVE_TESTS += $(F77_TESTS)
endif
if HAVE_FC
NONINTERACTIVE_TESTS += $(FC_TESTS)
endif
TESTS = $(NONINTERACTIVE_TESTS) $(INTERACTIVE_TESTS)

EXTRA_DIST     += $(srcdir)/tests/defs.in tests/defs.m4sh \
		  $(COMMON_TESTS) $(CXX_TESTS) $(F77_TESTS) $(FC_TESTS) \
		  $(INTERACTIVE_TESTS)
DIST_SUBDIRS   += $(CONF_SUBDIRS)

# The defs script shouldn't be recreated whenever the Makefile is
# regenerated since the source tree can be read-only.
check-recursive: tests/defs
tests/defs: $(srcdir)/tests/defs.in
	rm -f tests/defs.tmp tests/defs; \
	$(configure_edit) $(srcdir)/tests/defs.in > tests/defs.tmp; \
	mv -f tests/defs.tmp tests/defs

# Use `$(srcdir)/tests' for the benefit of non-GNU makes: this is
# how defs.in appears in our dependencies.
$(srcdir)/tests/defs.in: $(auxdir)/general.m4sh tests/defs.m4sh Makefile.am
	cd $(srcdir); \
	rm -f tests/defs.in; \
	$(M4SH) -B $(auxdir) tests/defs.m4sh > tests/defs.in

# We need to remove any files that the above tests created.
clean-local-legacy:
	-for dir in $(CONF_SUBDIRS); do \
	  if test -f $$dir/Makefile; then \
	    (cd $$dir && $(MAKE) $(AM_MAKEFLAGS) distclean); \
	  else :; fi; \
	done
	rm -rf _inst _inst-*

# For distclean, we may have to fake Makefiles in the test directories
# so that descending in DIST_SUBDIRS works.
# Hide the additional dependency from automake so it still outputs the rule.
distclean_recursive = distclean-recursive
$(distclean_recursive): fake-distclean-legacy
.PHONY: fake-distclean-legacy
fake-distclean-legacy:
	-for dir in $(CONF_SUBDIRS); do \
	  if test ! -f $$dir/Makefile; then \
	    $(mkinstalldirs) $$dir; \
	    echo 'distclean: ; rm -f Makefile' > $$dir/Makefile; \
	  else :; fi; \
	done

$(TESTS): tests/defs
DISTCLEANFILES += tests/defs
