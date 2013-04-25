#!/usr/bin/env python
# -*- Mode: Python; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
# vi: set ts=4 sw=4 expandtab:

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# This is a wrapper script around the exactgc script. Usage:
# Env:
# The AVM env var must point to an avmshell
# The ASC env var must point to asc.jar
# Invocation:
# The script exports one function "GenerateTracers" with these arguments:
# prefix = module specific prefix string used in generated file names, ie "avmplus", "avmglue"
# inputfiles = string of list of files, can contain wildcards
# outputdir = where output files go

import os
import shutil
import sys
import filecmp
import glob
import tempfile
import platform
import subprocess
import string

def platform_filename(filename):
    filename = os.path.abspath(filename);

    if sys.platform.startswith("cygwin"):
        from subprocess import Popen
        from subprocess import PIPE

        retval = Popen(["/usr/bin/cygpath", "-m", filename], stdout=PIPE).communicate()[0]
    else:
        retval = filename;
    return string.rstrip(retval);


utilsdir = platform_filename(os.path.dirname(__file__))

def gen(prefix,inputfiles,outputdir,srcdir=os.getcwd(),ns=''):
    avm = os.environ.get('AVM')
    if avm == None:
        print "ERROR: AVM environment variable must point to avm executable"
        exit(1)

    asfile = utilsdir + "/exactgc.as"
    abcfile = utilsdir + "/exactgc.abc"

    # note this script tries not to rely on CWD but compiling exactgc.as does.
    if not os.path.exists(abcfile) or (os.path.getmtime(abcfile) < os.path.getmtime(asfile) and os.access(abcfile, os.W_OK)):
        classpath = os.environ.get('ASC')
        if classpath == None:
            print "ERROR: ASC environment variable must point to asc.jar"
            exit(1)
        print("Compiling exactgc script...")

        java_home = os.environ.get('JAVA_HOME')
        if java_home == None:
            print "warning: no JAVA_HOME set; inferring executable is 'java' and on system path."
            java_bin = 'java'
        else:
            java_bin = os.path.join(java_home, "bin", "java")

        # "java_bin" because path may have spaces, parentheses, etc (Windows).
        os.system("\"%s\" -jar %s -AS3 -import %s/../generated/builtin.abc -import %s/../generated/shell_toplevel.abc -debug %s" % (java_bin, classpath, utilsdir, utilsdir, asfile))

    # in case outputdir is relative make it absolute before chdir'ing
    outputdir = os.path.abspath(outputdir)
        
    print("Generating "+prefix+" exact gc generated code into " + outputdir)
    savedir = os.getcwd()

    os.chdir(srcdir)

    # expand wildcards in input file list
    filelist = '\n'.join([y for x in map(glob.glob, inputfiles.split()) for y in x])
    
    # don't bother trying to pass via os.system, dump into a tmp file
    # for windows we need a new python API to keep the file around ater closing, which
    # we must do in order for the exact gc script to open it.
    oldpy = sys.version_info < (2,6)
    if oldpy and platform.system() == 'Windows':
		print "Error: exactgc script requirets newer python on windows."
		exit(1)
		
    if oldpy:
		tmpfile = tempfile.NamedTemporaryFile()
	else:
	    tmpfile = tempfile.NamedTemporaryFile(delete = False)
	    
	tmpfile.write(filelist)
	
	# close deletes in old world
	if not oldpy:
		tmpfile.close()
    else:
        tmpfile.flush()

    # leave off -ns arg if default namespace
    if ns != '':
        ns = '-ns ' + ns

    exactgccmd = '%s %s -- -b %s-tracers.hh -n %s-tracers.hh -i %s-tracers.h %s %s' % (avm, abcfile, prefix, prefix, prefix, ns, '@'+platform_filename(tmpfile.name))
    ret = os.system(exactgccmd)
    
    if oldpy:
        tmpfile.close()
    else:
        os.unlink(tmpfile.name)
        
    success = True
    if ret != 0:
        print "Invoking avmshell on exactgc script failed with command:", exactgccmd
        success = False

    tmpfile.close()

    if not os.path.exists(prefix+'-tracers.hh'):
        print "Error: failed to generate tracers"
        success = False
    elif not os.path.exists(outputdir):
        os.makedirs(outputdir)

    # copy changed headers stuff to output dir
    for src in [prefix+'-tracers.hh', prefix+'-tracers.h']:
        target = outputdir + "/" + src
        # delete target file in case of error
        if not success:
            if os.path.exists(target):            
                os.remove(target)
        else:
            if not os.path.exists(target) or not filecmp.cmp(target,src):
                shutil.move(src,target)
            else:
                os.remove(src)

    os.chdir(savedir)

    if not success:
        exit(1)
    
            
def gen_builtins(outdir):
    coredir = utilsdir + "/../core/"
    gen(prefix = 'avmplus', inputfiles = '*.h *.as', outputdir = outdir, srcdir = coredir, ns = 'avmplus')

def gen_shell(outdir):
    shelldir = utilsdir + "/../shell/"
    gen(prefix = 'avmshell', inputfiles = 'shell_toplevel.as DebugCLI.h ShellCore.h SystemClass.h', outputdir = outdir, srcdir = shelldir, ns = 'avmshell')
    gen(prefix = 'extensions', inputfiles = 'DomainClass.h Domain.as ../extensions/*.h ../extensions/*.as', outputdir = outdir, srcdir = shelldir, ns = 'avmplus')
