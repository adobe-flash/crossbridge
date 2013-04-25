#!/bin/sh
# Preremove script for coreutils.

manifest=/etc/preremove/coreutils-manifest.lst

[ -f $manifest ] || { echo "Unable to find manifest file"; exit 1; }

echo "$0: *** Removing unmodified base files."
echo "*** These will be updated by the postinstall script."
echo "*** Please wait."

while read f; do
  /bin/cmp -s "/${f}" "/etc/defaults/${f}" && \
    { echo "$0: /$f hasn't been modified, it will be updated" ; \
     /bin/rm -f "/${f}"; }
done < ${manifest}
