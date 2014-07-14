/*
    Copyright 2007-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html )
*/

/******************************************************************************/

Goals:

To help compiler vendors identify places where they may be able to improve
the performance of the code they generate.

To help developers understand the performance impact of using different
data types, operations, and C++ langugage features with their
target compilers and OSes.

/******************************************************************************/

Secondary goals:

To take performance problems found in real world code and turn them
    into benchmarks for compiler vendors and other developers to learn from.

Keep the benchmark portable to as many compilers and OSes as possible
    This means keeping things simple and external dependencies minimal

Not to use specialized optimization flags per test
    No pragmas or other compiler directives are allowed in the source.
    All source files should use the same compilation flags.
    Use the common optimization flags (-O, -O1, -O2, -O3, or -Os).
    If another option improves optimization, then why isn't it on for -O3?
    If an optimization flag doesn't always improve performance, that is
        most likely a bug in the optimization code that needs to be fixed.
	In the real world, developers can't test all permutations of all
		optimization flags.  They expect the standard flags to work.

/******************************************************************************/

**** A note to compiler vendors:
     Please match the idioms, not the instances.
     The benchmark code will be changing over time.
     And we do read your assembly output.

/******************************************************************************/

Building:

Unix users should be able to use "make all" to build and "make report"
to generate the report. If you wish to use a different compiler, you can
set that from the make command line, or edit the makefile.

Windows users will need to make sure that the VC environment variables
are set for their shell (command prompt), then use "nmake -f makefile.nt all"
and "nmake -f makefile.nt report" from within that shell.

