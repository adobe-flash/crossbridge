LEVEL = ../../../..
RUN_OPTIONS     = -l ./ -q -m 64M
STDOUT_FILENAME = $(RUN_TYPE).out
STDIN_FILENAME  = $(RUN_TYPE).in

# We need to include unistd.h because something with Fedora Core 1 is broken,
# and we need the sysconf() macros.
CPPFLAGS = -include unistd.h -DSYS_HAS_CALLOC_PROTO -DSYS_HAS_IOCTL_PROTO -DSYS_HAS_TIME_PROTO -DSYS_HAS_READ_PROTO
include ../../Makefile.spec2000

ifeq ($(TARGET_OS),Darwin)
CPPFLAGS += -DSYS_IS_BSD
else
CPPFLAGS += -DSYS_IS_USG
endif

# 254.gap relies on signed arithmetic overflow being well-defined.
CFLAGS += -fwrapv
