#!/bin/sh

set -eu

if [ $# -lt 2 ]; then
  echo "usage: $0 <timeout> <program> <args...>"
  echo
  echo "  This script simply runs another program under ulimit and always"
  echo "  returns true. It is used for invoking tools from the LLVM test suite"
  echo "  Makefiles, where we need them to always return true so that we don't"
  echo "  abort the test run."
  exit 1
fi

# Save a copy of the original arguments in a string before we
# clobber them with the shift command.
TIMEOUT=$1
shift
PROGRAM=$1
shift

SYSTEM=`uname -s`

ULIMITCMD=""
case $SYSTEM in
  CYGWIN*) 
    ;;
  Darwin*)
    # Disable core file emission, the script doesn't find it anyway because it
    # is put into /cores.
    ULIMITCMD="$ULIMITCMD ulimit -c 0;"
    ULIMITCMD="$ULIMITCMD ulimit -t $TIMEOUT;"
    # To prevent infinite loops which fill up the disk, specify a limit on size
    # of files being output by the tests. 10 MB should be enough for anybody. ;)
    ULIMITCMD="$ULIMITCMD ulimit -f 10485760;"
    ;;
  *)
    ULIMITCMD="$ULIMITCMD ulimit -t $TIMEOUT;"
    ULIMITCMD="$ULIMITCMD ulimit -c unlimited;"
    # To prevent infinite loops which fill up the disk, specify a limit on size
    # of files being output by the tests. 10 MB should be enough for anybody. ;)
    ULIMITCMD="$ULIMITCMD ulimit -f 10485760;"

    # virtual memory: 600 MB should be enough for anybody. ;)
    ULIMITCMD="$ULIMITCMD ulimit -v 600000;"
esac

if ( ! sh -c "$ULIMITCMD sh -c '$PROGRAM $*'" ); then
    echo "warning: command failed: '$PROGRAM $*'"
fi

# Always return "successful" so that tests will continue to be run.
exit 0
