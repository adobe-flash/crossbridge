#!/bin/bash
# Bash postinstall script.  Assumes you are running setup.exe 2.510.2.2 or
# newer, meaning that this is executed by /bin/bash and not /bin/sh (if you
# are running an older setup.exe, this postinstall script can't do anything).
#
# Copyright (C) 2007, 2008, 2010 Eric Blake
# This file is free software; I give unlimited permission to copy and/or
# distribute it, with or without modifications, as long as this notice is
# preserved.


# Track any failure in this script.
result=0

# Install /dev/fd, /dev/std{in,out,err}.  The bash builtin test was compiled
# to assume these exist, so use /bin/test to really check.
test -d /dev || result=1
/bin/test -h /dev/stdin  || ln -sf /proc/self/fd/0 /dev/stdin  || result=1
/bin/test -h /dev/stdout || ln -sf /proc/self/fd/1 /dev/stdout || result=1
/bin/test -h /dev/stderr || ln -sf /proc/self/fd/2 /dev/stderr || result=1
/bin/test -h /dev/fd     || ln -sf /proc/self/fd   /dev/fd     || result=1

test $result = 0 || exit $result
