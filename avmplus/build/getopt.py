# -*- Mode: Python; indent-tabs-mode: nil -*-
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys
import re

_target = re.compile("^--target=(.*)$")
_host = re.compile("^--host=(.*)$")
_ignore = re.compile(r"^--(srcdir|cache-file)=")
_arg = re.compile(r"^--(enable|disable|with|without)-([\w-]+)(?:=(.*)|$)$")
_yes = re.compile("^(t|true|yes|y|1)$", re.I)
_no = re.compile("^(f|false|no|n|0)$", re.I)
_help = re.compile("^(-h|--help)$")
_sdk = re.compile("^--mac-sdk=(.*)$")
_xcode = re.compile("^--mac-xcode=(.*)$")
_arm_arch = re.compile("^--arm-arch=(.*)$")

class Options:
    def __init__(self, argv = sys.argv):
        self._args = {}
        self.target = None
        self.host = None
        self.ignore_unknown_flags = False
        self.help = False
        self._allargs = {}
        self.mac_sdk = None
        self.mac_xcode = None
        self.arm_arch = "armv7-a"

        # These arguments don't go in _allargs, and they aren't standard
        # --enable/--disable switches. The getHelp method needs a list of these
        # special arguments.
        # The argument names must correspond exactly to the property initializations
        # above or "--help" will not work.
        self._specialargs = (               \
                "target",                   \
                "host",                     \
                "ignore_unknown_flags",     \
                "mac_sdk",                  \
                "mac_xcode",                \
                "arm_arch"                  \
                )

        unknown_args = []
        for arg in argv[1:]:
            m = _target.search(arg)
            if m:
                self.target = m.group(1)
                continue

            m = _host.search(arg)
            if m:
                self.host = m.group(1)
                continue

            m = _sdk.search(arg)
            if m:
                self.mac_sdk = m.group(1)
                continue

            m = _xcode.search(arg)
            if m:
                self.mac_xcode = m.group(1)
                continue

            m = _arm_arch.search(arg)
            if m:
                self.arm_arch = m.group(1)
                continue

            if _ignore.search(arg) is not None:
                continue

            if arg == '--ignore-unknown-flags':
                self.ignore_unknown_flags = True
                continue

            if _help.search(arg) is not None:
                self.help = True
                continue

            m = _arg.search(arg)
            if not m:
                unknown_args.append(arg)
                continue

            (t, n, v) = m.groups()

            if type(v) == str:
                if _yes.search(v):
                    v = True
                if _no.search(v):
                    v = False

            if t == "enable" or t == "with":
                if v:
                    self._args[n] = v
                else:
                    self._args[n] = True

            elif t == "disable" or t == "without":
                if v:
                    raise Exception("--disable-" + n + " does not take a value.")

                self._args[n] = False

        if unknown_args and not self.ignore_unknown_flags:
            raise Exception("Unrecognized command line parameter(s): "
                            + ', '.join(unknown_args))


    def getBoolArg(self, name, default=None, remove=True):
        self._allargs[name] = default
        if not name in self._args:
            return default

        val = self._args[name]
        if remove:
            del self._args[name]

        if type(val) == bool:
            return val

        raise Exception("Unrecognized value for option '" + name + "'.")

    def getStringArg(self, name, default=None):
        self._allargs[name] = default
        if not name in self._args:
            return default

        val = self._args[name]
        del self._args[name]
        return val

    def peekBoolArg(self, name, default=None):
        return Options.getBoolArg(self,name,default,False)

    def peekStringArg(self, name, default=None):
        return Options.getStringArg(self,name,default,False)

    def getHelp(self):
        ret = ''
        for opt,default in sorted(self._allargs.iteritems()):
            if default == True:
                ret += "%-35s [=enabled]\n" % ("--enable-%s" % opt)
            else:
                ret += "%-35s [=not enabled]\n" % ("--enable-%s" % opt)
                
        # Print special-case options that aren't standard --enable/--disable
        # switches.
        for opt in self._specialargs:
            arg = "--%s=..." % (opt)
            ret += "%-35s [=%s]\n" % (arg, getattr(self,opt))
                
        return ret



    def finish(self):
        if not self.ignore_unknown_flags:
            if len(self._args):
                raise Exception("Unrecognized command line parameters: " +
                                ", ".join(self._args.keys()))
