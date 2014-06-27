#!/bin/sh
#
# Program:  TimeProgram.sh
#
# Synopsis: Get reliable timing results for a program
#
# Syntax:  ./TimeProgram <name> Program args...
#

NAME=$1
shift

echo "Timing $NAME"

# discard first output
time -p $* > /dev/null 2>&1

# Gather three data points
(time -p $* > /dev/null) 2>&1 | grep user | sed 's/user //'
(time -p $* > /dev/null) 2>&1 | grep user | sed 's/user //'
(time -p $* > /dev/null) 2>&1 | grep user | sed 's/user //'

