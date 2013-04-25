#!/usr/bin/env python

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import copy
import re

verbose = False
def verbose_print(*args):
    if verbose:
        print args
    
def read_buildfile(filename):
    f = open(filename, 'r')
    try:
        builds = []
        options = []
        option_dependencies = []
        combined = []
        list = None
        for l in f.readlines():
            l = l.strip()
            verbose_print('line: ', l)
            if len(l) == 0 or l[0] == '#':
                verbose_print('continuing')
                continue
            elif l == '[builds]':
                list = builds
            elif l == '[options]':
                list = options
            elif l == '[option_dependencies]':
                list = option_dependencies
            elif l == '[combined]':
                list = combined
            else:
                if l == '-':
                    l = ''
                #l=re.sub('{shell_release}',avmr,l)
                #verbose_print(l)
                #l=re.sub('{shell_release_debugger}',avmrd,l)
                #verbose_print(l)
                list.append(l)
                verbose_print(list)
        return builds, options, option_dependencies, combined
    finally:
        f.close()
# make_vmlist: combine all the option choices with all the build choices
#    and return a list of len(builds) * len(options) command lines.
def make_vmlist(buildfile):
    builds, options, option_dependencies, combined = read_buildfile(buildfile)
    #    missing = [p for p in builds if not exists(p)]
    #    if len(missing) > 0:
    #        for p in missing:
    #            sys.stderr.write("no such file: %s\n" % p)
    #        exit(1)
    vms = []
    for build in builds:
        for o in options:
            vms.append('%s %s' % (build, o))
    vms = vms + combined
    verbose_print('created %d configurations' % len(vms))
    for v in vms:
        verbose_print(v)
    
    return vms

def remove_conflicting_optrecs(controller, optrecs, optdeps):
    my_list = copy.deepcopy(optrecs)
    if len(my_list) == 0 or len(optdeps) == 0:
        return my_list
    for optrec in my_list:
        for optdep in optdeps:
            if optdep[0] == controller and optrec[0] == optdep[1][0]:
                verbose_print('optdep: ', optdep, ' controller: ', controller)
                verbose_print('Before: optrec[2]: ', optrec[2], ' optdep[1][1]: ', optdep[1][1])
                optrec[2] = (optdep[1][1],)
                verbose_print('After: optrec[2]: ', optrec[2])
    return my_list

def genruns(runs, optrecs, optdeps, pre_strings, post_strings):
    if len(optrecs) == 0:
        verbose_print('End recursion')
        verbose_print('Adding run: ', pre_strings, post_strings)
        runs.append([list(pre_strings), list(post_strings)])
        return
    current_optrec = optrecs[0]
    verbose_print('current_optrec: ', current_optrec)
    if current_optrec[1] == 'Command':
        post_strings.append(current_optrec[0])
        optrec_subset = remove_conflicting_optrecs((optrecs[0][0], 1), optrecs[1:], optdeps)
        verbose_print('Recurse')
        genruns(runs, optrec_subset, optdeps, pre_strings, post_strings)
        post_strings.pop()
    elif current_optrec[1] == 'Boolean' or current_optrec[1] == 'Integer':
        for val in current_optrec[2]:
            pre_strings.append(current_optrec[0] + '=' + str(val))
            verbose_print('Remaining optrecs before removal: ', optrecs[1:])
            optrec_subset = remove_conflicting_optrecs((optrecs[0][0], val), optrecs[1:], optdeps)
            verbose_print('Remaining optrecs after removal: ', optrec_subset)
            verbose_print('Recurse')
            genruns(runs, optrec_subset, optdeps, pre_strings, post_strings)
            pre_strings.pop()

def genperm(buildfile):
    avmr='../objdir-release/shell/avmshell.exe'
    avmrd='../objdir-releasedebugger/shell/avmshell.exe'

    make_vmlist('avmdiff2.cfg')

    builds, options, option_dependencies, combined = read_buildfile('avmdiff2.cfg')

    verbose_print('Options: ')
    optrecs = []
    for opt in options:
        verbose_print(opt)
        optrecs.append(eval(opt))

    optdict = dict([(optrec[0], optrec) for optrec in optrecs]) 
    verbose_print(optdict)

    for key in optdict:
        verbose_print(key)
        value = optdict[key]
        verbose_print(value)
        
    # read dependencies
    optdeps = []
    for optdep in option_dependencies:
        optdeps.append(eval(optdep))

    verbose_print(optdeps)

    # Build list of configurations
    runs = []
    pre_strings = []
    post_strings = []
    genruns(runs, optrecs, optdeps, pre_strings, post_strings)
    verbose_print(runs)
    verbose_print(len(runs))
    
    return runs
