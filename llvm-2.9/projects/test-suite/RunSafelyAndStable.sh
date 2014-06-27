#!/bin/sh
#
# Program:  RunSafelyAndStable.sh
#
# Synopsis: This script runs another program three times by repeatedly 
#           invoking the RunSafely.sh script. It collects the timings of
#           the program and reports the smallest one. The <outfile>.time
#           file will reflect the time and result code of the fastest
#           run.
#
# Syntax: 
#   ./RunSafelyAndStable.sh <ulimit> <exitok> <infile> <outfile> \
#      <program> <args...>
#
#   See the RunSafely.sh script for more details.
#
DIR=${0%%`basename $0`}
ULIMIT=$1
EXITOK=$2
INFILE=$3
OUTFILE=$4
PROGRAM=$5
shift 5

# Run it the first time
${DIR}/RunSafely.sh $ULIMIT $EXITOK $INFILE $OUTFILE $PROGRAM $*
exitval=$?
mv $OUTFILE.time $OUTFILE.time1
TIME1=`grep program $OUTFILE.time1 | sed 's/^program//'`
echo "Program $PROGRAM run #1 time: $TIME1"

# Run it the second time
${DIR}/RunSafely.sh $ULIMIT $EXITOK $INFILE $OUTFILE $PROGRAM $*
mv $OUTFILE.time $OUTFILE.time2
TIME2=`grep program $OUTFILE.time2 | sed 's/^program//'`
echo "Program $PROGRAM run #2 time: $TIME2"

# Run it the third time
${DIR}/RunSafely.sh $ULIMIT $EXITOK $INFILE $OUTFILE $PROGRAM $*
mv $OUTFILE.time $OUTFILE.time3
TIME3=`grep program $OUTFILE.time3 | sed 's/^program//'`
echo "Program $PROGRAM run #3 time: $TIME3"

# Figure out which run had the smallest run time:
SHORTEST=`echo -e "$TIME1 time1\n$TIME2 time2\n$TIME3 time3" | sort | 
                head -1 | sed "s|.*time||"`

echo "Program $PROGRAM run #$SHORTEST was fastest"
cp $OUTFILE.time$SHORTEST $OUTFILE.time

exit $exitval
