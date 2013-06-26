#!/bin/sh
## create_cpped_version.sh for  in /Users/pouchet
##
## Made by Louis-Noel Pouchet
## Contact: <pouchet@cse.ohio-state.edu>
##
## Started on  Mon Oct 31 16:20:01 2011 Louis-Noel Pouchet
## Last update Mon Oct 31 20:42:35 2011 Louis-Noel Pouchet
##

if [ $# -lt 1 ]; then
    echo "Usage: create_cpped_version.sh <file.c> [gcc -E flags]";
    exit 1;
fi;
args="$2";
file="$1";
head -n 12 $file > .__poly_top.c;
tail -n +12 $file > .__poly_bottom.c;
filename=`echo "$file" | sed -e "s/\(.*\).c/\1/1"`;
filenameorig=`basename $file`;
benchdir=`dirname "$file"`;
gcc -E .__poly_bottom.c -I $benchdir $args 2>/dev/null > .__tmp_poly.c
sed -e "/^[ ]*;[ ]*$/d" .__tmp_poly.c | sed -e "s~.__poly_bottom.c~$filenameorig~g" > .__poly_bottom.c;
cat .__poly_top.c > $filename.preproc.c;
echo "#include <polybench.h>\n" >> $filename.preproc.c;
cat .__poly_bottom.c >> $filename.preproc.c;
rm -f .__tmp_poly.c .__poly_bottom.c .__poly_top.c;

