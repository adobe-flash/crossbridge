#!/usr/bin/sh
# To the extent possible under law, the author(s) have dedicated all 
# copyright and related and neighboring rights to this software to the 
# public domain worldwide. This software is distributed without any warranty. 
# You should have received a copy of the CC0 Public Domain Dedication along 
# with this software. 
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 

# base-files version 4.1-1

MANIFEST="/etc/preremove/base-files-manifest.lst"

[ -f ${MANIFEST} ] || (echo "Unable to find manifest file." && exit 1)

echo "*** Removing unmodified base files."
echo "*** These will be updated by the postinstall script."
echo "*** Please wait."

while read f; do
  /usr/bin/cmp -s "/${f}" "/etc/defaults/${f}" && \
    (echo "/${f} hasn't been modified, it will be updated." ; \
     /usr/bin/rm -f "/${f}")
done < ${MANIFEST}

exit 0
