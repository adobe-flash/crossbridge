#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


import os, sys, re
from stat import *

class StatCache:
    def __init__(self):
        self._dict = {}

    def getStat(self, key):
        if not key in self._dict:
            try:
                self._dict[key] = os.stat(key)
            except OSError:
                self._dict[key] = None

        return self._dict[key]

    def getMTime(self, key):
        s = self.getStat(key);
        if s == None:
            return 0;

        return s.st_mtime

_statcache = StatCache()

def rebuildNeeded(file, dependencies, verbose):
    """Calculate whether a file needs to be rebuilt by comparing its timestamp
    against a list of dependencies.

    returns True or False"""

    f = _statcache.getMTime(file)
    if f == 0:
        if verbose:
            print >>sys.stderr, "Target %s doesn't exist" % file
        return True

    for dep in dependencies:
        d = _statcache.getMTime(dep)
        if d == 0:
            if verbose:
                print >>sys.stderr, "Target %s missing dependency %s" % (file, dep)
            return True
        if d > f:
            if verbose:
                print >>sys.stderr, "Target %s older than dependency %s" % (file, dep)
            return True

    return False

_fileFromLine = re.compile('^([^:]+): FORCE')

def rebuildsNeeded(files, outfile, verbose, quiet):
    """Write a makefile snippet indicating whether object files need to be
    rebuilt.

    @param files: a dictionary of { 'objfile', 'depfile' }"""

    oldrebuilds = {}
    do_write = False

    if os.path.exists(outfile):
        istream = open(outfile, "r")
        for line in istream:
            m = _fileFromLine.match(line)
            if m:
                oldrebuilds[m.group(1)] = None
        istream.close()
    else:
        if not quiet:
            print >>sys.stderr, "Writing %s: doesn't exist" % outfile
        do_write = True

    newrebuilds = []

    for (objfile, depfile) in files.items():
        rebuild = True

        try:
            d = open(depfile, "r")
            rebuild = \
                rebuildNeeded(objfile, \
                              [line.rstrip("\n\r") for line in d.readlines()],
                              verbose)
            d.close()

        except IOError:
            if verbose:
                print >>sys.stderr, "IOError attempting to check rebuildNeeded %s %s" % (objfile, depfile)
            pass

        if rebuild:
            if verbose:
                print >>sys.stderr, "Scheduling %s for rebuilding" % objfile
            newrebuilds.append(objfile)

            if objfile in oldrebuilds:
                del oldrebuilds[objfile]
            else:
                do_write = True

    if do_write or len(oldrebuilds):
        if not quiet:
            print "Building %s" % outfile
        ostream = open(outfile, "w")
        for objfile in newrebuilds:
            ostream.write(objfile + ": FORCE\n")
        ostream.close()

if __name__ == "__main__":
    import sys
    import re

    _argExpr = re.compile("\\.ii?$")

    files = {}
    verbose = 'CALCDEPENDS_VERBOSE' in os.environ
    quiet = False

    sys.argv.pop(0)
    if sys.argv[0] in ('-v', '--verbose'):
        verbose = True
        sys.argv.pop(0)

    elif sys.argv[0] in ('-q', '--quiet'):
        quiet = True
        sys.argv.pop(0)

    outfile = sys.argv.pop(0)

    for objfile in sys.argv:
        depfile = _argExpr.sub(".deps", objfile)
        files[objfile] = depfile

    rebuildsNeeded(files, outfile, verbose, quiet)
