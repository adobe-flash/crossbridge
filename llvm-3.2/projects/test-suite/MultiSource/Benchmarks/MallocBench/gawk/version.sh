#! /bin/sh

# version.sh --- create version.c

if [ "x$1" = "x" ]
then
	echo you must specify a release number on the command line
	exit 1
fi

RELEASE="$1"

cat << EOF
char *version_string = "@(#)Gnu Awk (gawk) ${RELEASE}";

/* 1.02		fixed /= += *= etc to return the new Left Hand Side instead
		of the Right Hand Side */

/* 1.03		Fixed split() to treat strings of space and tab as FS if
		the split char is ' '.

		Added -v option to print version number
 		
		Fixed bug that caused rounding when printing large numbers  */

/* 2.00beta	Incorporated the functionality of the "new" awk as described
		the book (reference not handy).  Extensively tested, but no 
		doubt still buggy.  Badly needs tuning and cleanup, in
		particular in memory management which is currently almost
		non-existent. */

/* 2.01		JF:  Modified to compile under GCC, and fixed a few
		bugs while I was at it.  I hope I didn't add any more.
		I modified parse.y to reduce the number of reduce/reduce
		conflicts.  There are still a few left. */

/* 2.02		Fixed JF's bugs; improved memory management, still needs
		lots of work. */

/* 2.10		Major grammar rework and lots of bug fixes from David.
		Major changes for performance enhancements from David.
		A number of minor bug fixes and new features from Arnold.
		Changes for MSDOS from Conrad Kwok and Scott Garfinkle.
		The gawk.texinfo and info files included! */

/* 2.11		Bug fix release to 2.10.  Lots of changes for portability,
		speed, and configurability.  */
EOF
exit 0
