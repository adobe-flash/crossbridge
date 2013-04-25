#!/usr/bin/env python
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.



import os
import os.path
from os.path import *
import stat
import re
import sys



def usage():
    print "file-check.py <dir>"
    print "Must pass in a directory <dir>."
    exit(1)

try:
    base_dir = sys.argv[1]
    if not isdir(base_dir):
        usage()
except:
    usage()


MPL_string="You can obtain one at http://mozilla\.org/MPL/2\.0"
MPL1_string="Version: MPL 1\.1/GPL 2\.0/LGPL 2\.1"
MPL_excludes = open(os.getcwd() + '/../all/mpl_excluded.txt').read().splitlines()
mpl1_files = []
mpl_files = []

LineEnding_excludes = open(os.getcwd() + '/../all/lineending_excluded.txt').read().splitlines()
ending_file = []

file_count = 0


def walktree (top = ".", depthfirst = True):
    names = os.listdir(top)
    if not depthfirst:
        yield top, names
    for name in names:
        if name=='.hg':
            continue
        try:
            st = os.lstat(os.path.join(top, name))
        except os.error:
            continue
        if stat.S_ISDIR(st.st_mode):
            for (newtop, children) in walktree (os.path.join(top, name), depthfirst):
                yield newtop, children
    if depthfirst:
        yield top, names





for (basepath, children) in walktree(base_dir, False):
    for child in children:
        mpl1_excluded = 0
        mpl_excluded = 0
        lineending_excluded = 0
        filename = os.path.join(basepath, child)
        # Ignore directory names
        if isfile(filename):
            file_count += 1
            for line in MPL_excludes:
                if line.startswith('#') or len(line)==0: # Skip the line if blank or is comment
                    continue
                if re.search(line, filename)!=None:
                    mpl_excluded = 1
                    break
                    
            for line in LineEnding_excludes:
                if line.startswith('#') or len(line)==0: # Skip the line if blank or is comment
                    continue
                if re.search(line, filename)!=None:
                    lineending_excluded = 1
                    break
        else: # Exclude directory 
            mpl1_excluded = 1
            mpl_excluded = 1
            lineending_excluded = 1
        
        
        ############
        # MPL1 Check
        ############
        if mpl1_excluded == 0:
            file = open(filename)
            MPL1found=0    
            # Search the file for the MPL string
            for line in file:
                if re.search(MPL1_string, line)!=None:
                    MPL1found = 1
                    break

            # No MPL string was found
            if MPL1found != 0:
                mpl1_files.append(filename)


        ############
        # MPL Check
        ############
        # These files have not been mpl_excluded and need to be checked
        if mpl_excluded == 0:
            MPLfound = 0
            file = open(filename)
            
            # Search the file for the MPL string
            for line in file:
                if re.search(MPL_string, line)!=None:
                    MPLfound = 1
                    break

            # No MPL string was found
            if MPLfound != 1:
                mpl_files.append(filename)

        ####################
        # Line Ending Checks
        ####################
        # These files have not been lineending_excluded and need to be checked
        if lineending_excluded == 0:
            unix_lines = 1
            file = open(filename)
            
            # Search the file for the MPL string
            for line in file:
                if re.search("\r\n", line)!=None:
                    unix_lines = 0
                    break

            # None UNIX line endings foune
            if unix_lines != 1:
                ending_file.append(filename)



if mpl1_files or mpl_files or ending_file:
    print "buildbot_status: WARNINGS"
else:
    print "status: PASSED"

if mpl1_files:
    print ""
    print ""
    print "MPL1 Issues:"
    print "============"
    for file in mpl1_files:
        print file.replace(base_dir, '')

if mpl_files:
    print ""
    print ""
    print "MPL Issues:"
    print "==========="
    for file in mpl_files:
        print file.replace(base_dir, '')

if ending_file:
    print ""
    print ""
    print "Line Ending Issues:"
    print "==================="
    for file in ending_file:
        print file.replace(base_dir, '')

print ""
print ""
print "number of MPL Issues         : %d " % len(mpl_files)
print "number of MPL1 Issues        : %d " % len(mpl1_files)
print "number of Line Ending Issues : %d " % len(ending_file)
print "number of files              : %d " % file_count
