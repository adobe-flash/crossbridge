#!/bin/sh
#
# Program:  Sandbox.sh
#
# Synopsis: This script is used to provide a sandbox for SPEC programs to
#           execute in.  It provides an isolated environment, the input files
#           necessary, and then finally runs the program.
#
# Syntax:  ./Sandbox.sh <uid> <output_file> <ref input>   <program> <arguments>
#

TEST_UID=$1
OUTPUT_FILE=$2
INPUT_DIR=$3
shift;shift;shift

mkdir Output/$TEST_UID 2> /dev/null
cd Output/$TEST_UID
rm -rf *

# Copy all of the test input files into the current directory.
cp -r $INPUT_DIR/* .

# If there is an "all" input directory, take it as well.  This
# effectively copies over $INPUT_DIR/../../all/input, but works if the
# actual $INPUT_DIR directory doesn't exist, as is the case with 444.namd.
UP1=`dirname $INPUT_DIR`
UP2=`dirname $UP1`
cp -r $UP2/all/input/* . > /dev/null 2>&1 

# If any of the input files is a .bz2 file, uncompress it now.
bunzip2 *.bz2 > /dev/null 2>&1

# Run the program now.
echo Running: "$@"
exec "$@"
