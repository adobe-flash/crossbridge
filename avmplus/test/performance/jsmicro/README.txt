// See end of file for copyright notice and license terms.

These are javascript / ecmascript microbenchmarks, focusing narrowly
on the performance of language features.

Each test is available as an .as file (which includes the .js file) or
as an .html file (which also includes the .js file).  The former can
be compiled with ASC and run in the VM shell or -- we hope -- in the
Flash Player; the latter can be run in any web browser.

The .as and .html files also include common test drivers.

The .as and .html files are *generated* by generate-drivers.as, see
comments in the file for how to run it.  You only have to write the
.js file.

Each test should be in a separate file; only one result should be
reported per test.

Each test should be very specific; no attempt should be made to make
it "realistic" or to disable compiler optimizations by making results
appear to be used, other than by assigning the results to variables.
These tests are not real programs, they represent only themselves.

Under ABSOLUTELY NO CIRCUMSTANCES may any of the .js files use
non-ECMAScript idioms.  Stick to ECMAScript 3 until ES5 is implemented
in the major browsers.

As the AVM+ is incompatible with the ECMAScript spec in corner cases
(start at https://bugzilla.mozilla.org/show_bug.cgi?id=472863 and
drill down) the .js files should avoid tickling those
incompatibilities.

To see brief descriptions of the benchmarks, evaluate this:

  grep DESC *.js | sed -e 's/:[^"]*\"/ --- /g' -e 's/"[^"]*$//g'

or maybe even this:

  grep DESC *.js | \
    sed -e 's/:[^"]*\"/---/g' -e 's/"[^"]*$//g' | \
    awk -F--- \
      'BEGIN { max=0; k=0 } 
             { if (length($1) > max) max=length($1); f[k]=$1; d[k]=$2; k++ }
       END   { for ( i=0 ; i < k ; i++ ) 
                   print(sprintf("%-" (max+4) "s%s", f[i], d[i]))
             }'


// What we need:

Operators of particular interest:

  - computation over arrays of numbers - well behaved but no static type information in the source
  - global-environment lookup, if we're able to isolate it
  - String comparison
  - "in"
  - "instanceof"

Important library functions first (and now we should test in AS3 mode
too), but eventually all of them.

  - new <builtin type>, for every type
  - Object.hasOwnProperty
  - Array.length access
  - Function.apply, call
  - Number.PI access, maybe others
  - Math.<all of them>
  - Date.toString, valueOf

// Marginal?

Array: "generic" array functions applied to other kinds of objects
E4X, compare with Firefox?
JS1.6, etc, compare with Firefox?


/*
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */