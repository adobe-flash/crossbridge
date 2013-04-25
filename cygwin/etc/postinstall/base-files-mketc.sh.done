#!/usr/bin/sh
# To the extent possible under law, the author(s) have dedicated all 
# copyright and related and neighboring rights to this software to the 
# public domain worldwide. This software is distributed without any warranty. 
# You should have received a copy of the CC0 Public Domain Dedication along 
# with this software. 
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 

# base-files version 4.1-1

# Create symbolic links from some /etc files to the Windows equivalents
# Set perms to /tmp directory

FILES="hosts protocols services networks"
OSNAME="$(/usr/bin/uname -s)"
WINSYS32HOME="$(/usr/bin/cygpath -S -w)"
WINETC="${WINSYS32HOME}\\drivers\\etc"

if [ ! -d "${WINETC}" ]; then
  echo "Directory ${WINETC} does not exist; exiting" >&2
  echo "If directory name is garbage you need to update your cygwin package" >&2
  exit 1
fi

for mketc in ${FILES}
do
  if [ ! -e "/etc/${mketc}" -a ! -L "/etc/${mketc}" ]
  then
    # Windows only uses the first 8 characters
    WFILE="${WINETC}\\$(expr substr "${mketc}" 1 8)"
    /usr/bin/ln -s -v "${WFILE}" "/etc/${mketc}"
  fi
done

/usr/bin/chmod 1777 /tmp 2>/dev/null

exit 0
