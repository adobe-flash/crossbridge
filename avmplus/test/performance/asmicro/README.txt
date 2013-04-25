// See end of file for copyright notice and license terms.

These are actionscript microbenchmarks, focusing narrowly
on the performance of language features.

Each test is available as an .as file, which can be compiled with ASC.
The tests are meant to cover language features tested in ../jsmicro/
(or a superset thereof), but in an environment where:

  1. AS3 features such as type annotations can be included to improve
     measurement precision, and

  2. AS3 language/library features (and/or warts) can be tested and
     compared.  (This is the place to use non-ECMA-script idioms.)

Each test should be in a separate file; only one result should be
reported per test.

Each test in jsmicro/*.js should have an analogous .as file here;
this is the base test set.

There may be other files here that have no analogue in jsmicro; also,
since there may be different ways to type or otherwise port Javascript
to AS3, there can be >1 file for any one test in jsmicro.  Generally
though, most files in jsmicro should map to a single file with the
obvious type annotations.  (It is prudent to give additional files
with no correspondent in jsmicro fresh filename prefixes not appearing
in jsmicro.)

Type annotations are strictly optional, there's no compulsion to use
them.

NOTE that the default compilation mode when run with runtests.py is
-AS3 and that if public methods are meant to be accessed rather than
the AS3:: counterparts then compiler switch overrides must be
provided.

To see brief descriptions of the benchmarks, evaluate this:

  grep DESC *.as | sed -e 's/:[^"]*\"/ --- /g' -e 's/"[^"]*$//g'

or maybe even this:

  grep DESC *.as | \
    sed -e 's/:[^"]*\"/---/g' -e 's/"[^"]*$//g' | \
    awk -F--- \
      'BEGIN { max=0; k=0 } 
             { if (length($1) > max) max=length($1); f[k]=$1; d[k]=$2; k++ }
       END   { for ( i=0 ; i < k ; i++ ) 
                   print(sprintf("%-" (max+4) "s%s", f[i], d[i]))
             }'

To print all benchmarks missing a DESC definition (or confirm that
they all have one), evaluate this:

  for f in *-[0-9].as ; do if ! grep -q DESC $f; then echo $f; fi; done

/*
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */