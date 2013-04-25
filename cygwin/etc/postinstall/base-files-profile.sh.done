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

while read f; do
  fSrc="/etc/defaults/${f}"
  fDest="/${f}"
  if [ ! -e ${fDest} -a ! -L ${fDest} ]; then
    echo "Using the default version of ${fDest} (${fSrc})"
    /usr/bin/mkdir -p $(dirname ${fDest})
    /usr/bin/touch ${fDest}
    /usr/bin/cp ${fSrc} ${fDest}
  else
    echo "${fDest} is already in existance, not overwriting."
  fi
done < ${MANIFEST}

exit 0
