#!/bin/sh
#
# NAME
#     DiffOutput.sh
#
# SYNOPSIS
#     DiffOutput.sh <diff&opts> <testtype> <testname> [<goodoutput>]
#
# DESCRIPTION
#     DiffOutput.sh looks for a file named Output/<testname>.out-<testtype>
#     and uses GNU diff to compare it against the file
#     Output/<testname>.out-<goodoutput>.  This script is used to verify the
#     results of programs run in the LLVM test suite. By default,
#     <goodoutput> is "nat"; that is, test results are compared with output
#     from the program compiled with the native compiler.
#     If the files differ, DiffOutput.sh prints a portion of the
#     differences.
#
# EXIT STATUS
#     DiffOutput.sh always exits with a zero (successful) exit code.
#

# Command line parameters:
DIFF=$1
WHICHOUTPUT=$2
PROG=$3
GOODOUTPUT=${4-nat}
# Output filename:
DIFFOUTPUT=Output/${PROG}.diff-${WHICHOUTPUT}
# Input filenames:
TESTOUTPUT=Output/${PROG}.out-${WHICHOUTPUT}
GOODOUTPUT=Output/${PROG}.out-${GOODOUTPUT}

# Diff the two files.
$DIFF $GOODOUTPUT $TESTOUTPUT > $DIFFOUTPUT 2>&1 || (
  # They are different!
  echo "******************** TEST ($WHICHOUTPUT) '$PROG' FAILED! ********************"
  echo "Execution Context Diff:"
  head -n 200 $DIFFOUTPUT | cat -v
  rm $DIFFOUTPUT
  echo "******************** TEST ($WHICHOUTPUT) '$PROG' ****************************"
)
