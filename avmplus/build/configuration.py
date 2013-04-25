# -*- Mode: Python; indent-tabs-mode: nil -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import sys
import build.process
import re

def writeFileIfChanged(path, contents):
    """Write some contents to a file. Avoids modifying the file timestamp if the file contents already match."""
    print "Generating " + path + "...",
    try:
        outf = open(path, "r")
        oldcontents = outf.read()
        outf.close()

        if oldcontents == contents:
            print "not changed"
            return
    except IOError:
        pass

    outf = open(path, "w")
    outf.write(contents)
    outf.close()
    print

# Figure out TARGET and CPU, a la config.guess
# Do cross-compilation in the future, which will require HOST_OS and perhaps
# other settings

def _configGuess(options):
    ostest = sys.platform
    cputest = build.process.run_for_output(['uname', '-m'])
    return _configSub(ostest, cputest, options)

def _configSub(ostest, cputest, options):
    if ostest.startswith('win') or ostest.startswith('cygwin'):
        os = 'windows'
    elif ostest.startswith('darwin') or ostest.startswith('apple-darwin'):
        os = 'darwin'
    elif ostest.startswith('linux') or ostest.startswith('pc-linux'):
        os = 'linux'
    elif ostest.startswith('sunos'):
        os = 'sunos'
    elif ostest.startswith('android'):
        os = 'android'
    else:
        raise Exception('Unrecognized OS: ' + ostest)

    if re.search(r'^i(\d86|86pc|x86)$', cputest):
        cpu = 'i686'
    elif re.search('^(x86_64|amd64)$', cputest):
        cpu = 'i686' # force 32bit builds for now
    elif re.search('^(ppc64|powerpc64)$', cputest):
        cpu = 'ppc64'
    elif re.search('^(ppc|powerpc|Power Macintosh)$', cputest):
        cpu = 'powerpc'
    elif re.search('sun', cputest):
        cpu = 'sparc'
    elif re.search('arm', cputest):
        if options.getBoolArg("arm-thumb",False):
          cpu = 'thumb2'
        else:
          cpu = 'arm'
    elif re.search('thumb2', cputest):
        cpu = 'thumb2'
    elif re.search('mips', cputest):
        cpu = 'mips'
    elif re.search('sh4', cputest):
        cpu = 'sh4'
    else:
        raise Exception('Unrecognized CPU: ' + cputest)

    return (os, cpu)

class Configuration:
    def __init__(self, topsrcdir, options=None, sourcefile=None, objdir=None,
                 optimize=True, debug=False):
        self._topsrcdir = topsrcdir
        if objdir:
            self._objdir = objdir
        else:
            self._objdir = os.getcwd()

        self._optimize = optimize
        self._debug = debug
        self._host = _configGuess(options)
        self._target = self._host

        if sourcefile:
            srcfile = self._topsrcdir + "/" + sourcefile
            if not os.path.exists(srcfile):
                raise Exception("Source file " + srcfile + " doesn't exist.")

        objfile = self._objdir + "/" + sourcefile
        if os.path.exists(objfile):
            raise Exception("It appears you're trying to build in the source directory.  "
                            "(Source file " + objfile + " exists here.)  "
                            "You must use an object directory to build Tamarin.  "
                            "Create an empty directory, cd into it, and run this configure.py script from there.")


        if options:
            o = options.getStringArg("optimize")

            if o != None:
                self._optimize = o

            d = options.getStringArg("debug")
            if d != None:
                self._debug = d

            if options.host:
                hostcpu, hostos = options.host.split('-', 1)
                self._host = _configSub(hostos, hostcpu, options)

            if options.target:
                targetcpu, targetos = options.target.split('-', 1)
                self._target = _configSub(targetos, targetcpu, options)

        self._acvars = {
            # Bug 606989: GNU make forms like VPATH treat spaces as
            # list entry delimiters.  Easiest way around this: Use
            # rel. path when refering to the topsrcdir in Makefile
            # (not fool-proof, but it is far more robust).
            'topsrcdir': os.path.relpath(self._topsrcdir),
            'HOST_OS': self._host[0],
            'TARGET_OS': self._target[0],
            'TARGET_CPU': self._target[1]
            }

        if self._host[0] == 'windows':
            self._acvars['topsrcdir'] = toMSYSPath(self._topsrcdir)

        if self._debug:
            self._acvars['ENABLE_DEBUG'] = 1


        self._compiler = 'GCC'
        self._acvars.update({
            'I_SUFFIX': 'i',
            'II_SUFFIX': 'ii',
            'OBJ_SUFFIX': 'o',
            'PCH_SUFFIX': 'gch',
            'LIB_PREFIX': 'lib',
            'LIB_SUFFIX': 'a',
            'DLL_SUFFIX': 'so',
            'PROGRAM_SUFFIX': '',
            'USE_COMPILER_DEPS': 1,
            'USE_GCC_PRECOMPILED_HEADERS': 1,
            'EXPAND_LIBNAME' : '-l$(1)',
            'EXPAND_DLLNAME' : '-l$(1)',
            'OUTOPTION' : '-o ',
            'LIBPATH': '-L'
            })

        if self._target[0] == 'windows':
            self._compiler = 'VS'
            del self._acvars['USE_COMPILER_DEPS']
            del self._acvars['USE_GCC_PRECOMPILED_HEADERS']
            
            static_crt = options.getBoolArg('static-crt')
            self._acvars.update({
                'OBJ_SUFFIX'   : 'obj',
                'LIB_PREFIX'   : '',
                'LIB_SUFFIX'   : 'lib',
                'DLL_SUFFIX'   : 'dll',
                'PROGRAM_SUFFIX': '.exe',
                'CPPFLAGS'     : (self._debug and '-MTd' or '-MT') or (self._debug and '-MDd' or '-MD'),
                'CXX'          : 'cl.exe -nologo',
                'CXXFLAGS'     : '-TP',
                'CC'           : 'cl.exe -nologo',
                'CFLAGS'       : '-TC',
                'DLL_CFLAGS'   : '',
                'AR'           : 'lib.exe -nologo',
                'LD'           : 'link.exe -nologo',
                'LDFLAGS'      : '',
                'MKSTATICLIB'  : '$(AR) -OUT:$(1)',
                'MKDLL'        : '$(LD) -DLL -OUT:$(1)',
                'MKPROGRAM'    : '$(LD) -OUT:$(1)',
                'EXPAND_LIBNAME' : '$(1).lib',
                'EXPAND_DLLNAME' : '$(1).lib',
                'OUTOPTION' : '-Fo',
                'LIBPATH'   : '-LIBPATH:'
                })
	    if self._target[1] == "thumb2":
                    self._acvars.update({'LDFLAGS' : '-NODEFAULTLIB:"oldnames.lib"'})
                    if sys.platform.startswith('cygwin'):
                        self._acvars.update({'ASM' : '$(topsrcdir)/build/cygwin-wrapper.sh armasm.exe -nologo'})
                    else:
                        self._acvars.update({'ASM' : 'armasm.exe -nologo'})

            if self._target[1] == "arm":
                self._acvars.update({'LDFLAGS' : '-NODEFAULTLIB:"oldnames.lib" -ENTRY:"mainWCRTStartup"'})
                if sys.platform.startswith('cygwin'):
                    self._acvars.update({'ASM' : '$(topsrcdir)/build/cygwin-wrapper.sh armasm.exe -nologo -arch 5T'})
                else:
                    self._acvars.update({'ASM' : 'armasm.exe -nologo -arch 5T'})

            if self._target[1] == "x86_64":
                if sys.platform.startswith('cygwin'):
                   self._acvars.update({'MASM' : '$(topsrcdir)/build/cygwin-wrapper.sh ml64.exe -nologo -c '})
                else:
                   self._acvars.update({'MASM' : 'ml64.exe -nologo -c '})

            if sys.platform.startswith('cygwin'):
                self._acvars.update({'CXX'          : '$(topsrcdir)/build/cygwin-wrapper.sh cl.exe -nologo'})
                self._acvars.update({'CC'           : '$(topsrcdir)/build/cygwin-wrapper.sh cl.exe -nologo'})

        # Hackery! Make assumptions that we want to build with GCC 3.3 on MacPPC
        # and GCC4 on MacIntel
        elif self._target[0] == 'darwin':
            self._acvars.update({
                'DLL_SUFFIX'   : 'dylib',
                'CPPFLAGS'     : '-pipe',
                'CXXFLAGS'     : '',
                'CFLAGS'       : '',
                'DLL_CFLAGS'   : '-fPIC',
                'LDFLAGS'      : '-framework CoreServices',
                'AR'           : 'libtool',
                'MKSTATICLIB'  : '$(AR) -static -o $(1)',
                'MKDLL'        : '$(CXX) -dynamiclib -single_module -install_name @executable_path/$(1) -o $(1)',
                'MKPROGRAM'    : '$(CXX) -o $(1)',
                'POSTMKPROGRAM': 'dsymutil $(1)'
                })

# -Wno-trigraphs -Wreturn-type -Wnon-virtual-dtor -Wmissing-braces -Wparentheses -Wunused-label  -Wunused-parameter -Wunused-variable -Wunused-value -Wuninitialized

            if 'CXX' in os.environ:
                self._acvars['CXX'] = os.environ['CXX']
                self._acvars['CXXFLAGS'] += os.environ.get('CXXFLAGS', '')
                self._acvars['CFLAGS'] += os.environ.get('CFLAGS', '')
                self._acvars['LDFLAGS'] += os.environ.get('LDFLAGS', '')
            elif self._target[1] == 'i686':
                self._acvars['CXX'] = 'g++'
                self._acvars['CXXFLAGS'] += ' -arch i686 '
                self._acvars['LDFLAGS'] += ' -arch i686 '
            elif self._target[1] == 'x86_64':
                self._acvars['CXX'] = 'g++'
                self._acvars['CXXFLAGS'] += ' -arch x86_64 '
                self._acvars['LDFLAGS'] += ' -arch x86_64 '
            elif self._target[1] == 'powerpc':
                self._acvars['CXX'] = 'g++'
                self._acvars['CXXFLAGS'] += ' -arch ppc '
                self._acvars['LDFLAGS'] += ' -arch ppc '
            elif self._target[1] == 'ppc64':
                self._acvars['CXX'] = 'g++'
                self._acvars['CXXFLAGS'] += ' -arch ppc64 '
                self._acvars['LDFLAGS'] += ' -arch ppc64 '
            else:
                raise Exception("Unexpected Darwin processor while setting CXX.")
                
            if 'CC' in os.environ:
                self._acvars['CC'] = os.environ['CC']
            elif self._target[1] == 'i686':
                self._acvars['CC'] = 'gcc'
                self._acvars['CFLAGS'] += ' -arch i686 '
            elif self._target[1] == 'x86_64':
                self._acvars['CC'] = 'gcc'
                self._acvars['CFLAGS'] += ' -arch x86_64 '
            elif self._target[1] == 'powerpc':
                self._acvars['CC'] = 'gcc'
                self._acvars['CFLAGS'] += ' -arch ppc '
            elif self._target[1] == 'ppc64':
                self._acvars['CC'] = 'gcc'
                self._acvars['CFLAGS'] += ' -arch ppc64 '
            else:
                raise Exception("Unexpected Darwin processor while setting CC.")

        elif self._target[0] == 'linux':
            self._acvars.update({
                'CPPFLAGS'     : os.environ.get('CPPFLAGS', ''),
                'CXX'          : os.environ.get('CXX', 'g++'),
                'CXXFLAGS'     : os.environ.get('CXXFLAGS', ''),
                'CC'           : os.environ.get('CC', 'gcc'),
                'CFLAGS'       : os.environ.get('CFLAGS', ''),
                'DLL_CFLAGS'   : '-fPIC',
                'LD'           : os.environ.get('LD', 'ar'),
                'LDFLAGS'      : os.environ.get('LDFLAGS', ''),
                'AR'           : os.environ.get('AR', 'ar'),
                'MKSTATICLIB'  : '$(AR) cr $(1)',
                'MKDLL'        : '$(CXX) -shared -o $(1)',
                'MKPROGRAM'    : '$(CXX) -o $(1)'
                })
            if self._target[1] == "mips":
                self._acvars.update({'CXXFLAGS' : ''})
                self._acvars.update({'LDFLAGS' : ''})
                self._acvars.update({'zlib_EXTRA_CFLAGS' : ''})

        elif self._target[0] == 'android':
            self._acvars.update({
                'CPPFLAGS'     : '',
                'CXXFLAGS'     : '',
                'DLL_CFLAGS'   : '',
                'LDFLAGS'      : '-lstlport_static -lc -lm -lstdc++ -lgcc ',
                'AR'           : 'arm-linux-androideabi-ar',
                'AS'           : 'arm-linux-androideabi-as',
                'MKSTATICLIB'  : '$(AR) -rcs $(1)',
                'MKDLL'        : '$(CXX) -shared -o $(1)',
                'MKPROGRAM'    : '$(CXX) $(LFLAGS_HEADLESS) $(SEARCH_DIRS) -o $(1)',
                'STRIP'        : 'arm-linux-androideabi-strip',
                'LD'           : 'arm-linux-androideabi-ld',
                'CXX'          : 'arm-linux-androideabi-g++',
                'CC'           : 'arm-linux-androideabi-gcc'
                })
            
        elif self._target[0] == 'sunos':
            if options.getBoolArg("gcc", False):
                self._acvars.update({
                'CPPFLAGS'     : os.environ.get('CPPFLAGS', '') + "-DBROKEN_OFFSETOF",
                'CXX'          : os.environ.get('CXX', 'g++'),
                'CXXFLAGS'     : os.environ.get('CXXFLAGS', ''),
                'CC'           : os.environ.get('CC', 'gcc'),
                'CFLAGS'       : os.environ.get('CFLAGS', ''),
                'DLL_CFLAGS'   : '-fPIC',
                'LD'           : 'ar',
                'LDFLAGS'      : '',
                'MKSTATICLIB'  : '$(AR) cr $(1)',
                'MKDLL'        : '$(CXX) -shared -o $(1)',
                'MKPROGRAM'    : '$(CXX) -o $(1)'
                })
            else:
                self._compiler = 'SunStudio'
                self._acvars.update({
                'I_SUFFIX': 'i',
                'II_SUFFIX': 'i',
                'CPPFLAGS'     : '',
                'CXX'          : 'CC',
                'CXXFLAGS'     : '',
                'CC'           : 'cc',
                'CFLAGS'       : '',
                'LD'           : 'ar',
                'LDFLAGS'      : '',
                'MKSTATICLIB'  : '$(AR) cr $(1)',
                'MKPROGRAM'    : '$(CXX) -o $(1)'
                })
        self._acvars['COMPILER'] = self._compiler

    def getObjDir(self):
        """Returns the build directory being configured."""
        return self._objdir

    def getHost(self):
        """Returns an (os, cpu) tuple of the host machine."""
        return self._host

    def getTarget(self):
        """Returns an (os, cpu) tuple of the target machine."""
        return self._target

    def getCompiler(self):
        """Returns the compiler in use, as a string.
Possible values are:
- 'GCC': the GNU Compiler Collection, including GCC and G++
- 'VS': Microsoft Visual Studio
- 'SunStudio': Sun Studio"""
        return self._compiler

    def getDebug(self):
        return self._debug

    def subst(self, name, value, recursive=True):
        '''Add/substitute Makefile variables:
            recursive refers to the make variable flavor:
                True  : (default) recursively expanded var, defined with "="
                False : simply expanded var, defined with ":="
            more info: http://www.gnu.org/software/make/manual/make.html#Flavors
        '''
        self._acvars[name] = {'value':value, 'recursive':recursive}

    _confvar = re.compile("@([^@]+)@")

    def generate(self, makefile):
        outpath = self._objdir + "/" + makefile

        contents = ''
        for (k,v) in self._acvars.iteritems():
            if type(v) == dict: # not all _acvars are added with self.subst
                contents += '%s%s%s\n' % (k, '=' if v['recursive'] else ':=',
                                          v['value'])
            else:
                contents += '%s=%s\n' % (k,v)

        contents += "\n\ninclude $(topsrcdir)/build/config.mk\n" \
                    "include $(topsrcdir)/manifest.mk\n" \
                    "include $(topsrcdir)/build/rules.mk\n"

        writeFileIfChanged(outpath, contents)

def toMSYSPath(path):
    if sys.platform.startswith('cygwin'):
        return path
    elif path[1] != ':':
        raise ValueError("win32 path without drive letter! %s" % path)
    else:
        return '/%s%s' % (path[0], path[2:].replace('\\', '/'))
