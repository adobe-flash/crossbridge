pkg_config_LDADD=@GLIB_LIBS@

if USE_INSTALLED_POPT
pkg_config_LDADD += $(POPT_LIBS)
else
pkg_config_LDADD += popt/libpopt.la
popt_includes = -I$(top_srcdir)/popt
POPT_SUBDIR = popt
endif

SUBDIRS = $(GLIB_SUBDIR) $(POPT_SUBDIR) . check

m4dir = $(datadir)/aclocal
m4_DATA = pkg.m4

dist_doc_DATA = pkg-config-guide.html

man_MANS = pkg-config.1
EXTRA_DIST = $(m4_DATA) $(man_MANS) README.win32

bin_PROGRAMS = pkg-config
AM_CFLAGS=@WARN_CFLAGS@

if NATIVE_WIN32
INCLUDES= \
	-DPKG_CONFIG_PC_PATH="\"$(subst /,\/,$(pc_path))\"" \
	-DPKG_CONFIG_SYSTEM_INCLUDE_PATH="\"$(subst /,\/,$(system_include_path))\"" \
	-DPKG_CONFIG_SYSTEM_LIBRARY_PATH="\"$(subst /,\/,$(system_library_path))\"" \
	@GLIB_CFLAGS@ \
	$(popt_includes)
else
INCLUDES= \
	-DPKG_CONFIG_PC_PATH="\"$(pc_path)\"" \
	-DPKG_CONFIG_SYSTEM_INCLUDE_PATH="\"$(system_include_path)\"" \
	-DPKG_CONFIG_SYSTEM_LIBRARY_PATH="\"$(system_library_path)\"" \
	@GLIB_CFLAGS@ \
	$(popt_includes)
endif

pkg_config_SOURCES= \
	pkg.h \
	pkg.c \
	parse.h \
	parse.c \
	main.c
DISTCHECK_CONFIGURE_FLAGS = --with-installed-popt