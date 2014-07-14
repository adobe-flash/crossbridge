This directory contains programs that are to be compiled and run, but consist
of multiple source files.  In this scheme, every program gets a subdirectory,
and subdirectories are automatically activated just by existing in this directory.

If you don't want a testcase to be run in your tree, just rename the subdirectory
to XXX-disabled, which will prevent it from running.


Olden Code Sizes:

trinity% foreach f ( Olden-[a-z]* )
cd $f
echo "-------------------- $f --------------------"
wc *.[ch]
cd -
end
