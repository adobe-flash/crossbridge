libdir=${@:-/usr/lib}

[ ! -d ${libdir} ] && exit 1

extrafiles=$(ls -1 2>/dev/null /usr/bin/libtool \
      /usr/share/apr/build-1/libtool \
      /usr/lib/R/bin/libtool /usr/lib/rep/i686-pc-cygwin/libtool)

echo "${extrafiles}" \
  | xargs -r grep -l '^sys_lib_search_path_spec=.*/usr/lib/gcc/i686-pc-cygwin/[34].[3-9].[0-9]' \
  | xargs -r sed -i -e "/^sys_lib_search_path_spec=/ s|\(pc-cygwin\)/[34].[3-9].[0-9]|\1/$(/usr/bin/gcc-4 -dumpversion)|g"

find ${libdir} -type f -name '*.la' \
  | xargs -r grep -l '^dependency_libs=.*/usr/lib/gcc/i686-pc-cygwin/[34].[3-9].[0-9]/lib[a-z_+]*.la' \
  | xargs -r sed -i -e "/^dependency_libs=/ s|[^ ]*pc-cygwin/[34].[3-9].[0-9]/lib\([a-z_+]*\)\.la|-l\1|g"

