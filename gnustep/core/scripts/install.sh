#! /bin/sh

PREFIX=$1
MAKE=${2-make}
DOMAIN=${3-SYSTEM}

. $PREFIX/System/Library/Makefiles/GNUstep.sh

$MAKE GNUSTEP_INSTALLATION_DOMAIN=${DOMAIN} messages=yes install

exit 0
