#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil -*-
# vi: set ts=4 sw=4 expandtab:
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import sys
import os
import pprint
import bisect
import pickle
import subprocess
import optparse

scriptname = os.path.basename(os.path.normpath(os.path.abspath(sys.argv[0])))
standardHeader = """// This file was auto-generated, do not modify by hand.
// """ + scriptname + " generates this file.\n"

# ------------------------------------------------------------------------------
# Process creation / execution
# ------------------------------------------------------------------------------

def runProcess(p, msg, ignoreErrors = False):
    (stdoutdata, stderrdata) = p.communicate()
    if not ignoreErrors:
        if not p.returncode == 0:
            if stderrdata:
                print stderrdata
            print msg
            sys.exit(1)
    return (stdoutdata, stderrdata)

def createProcess(exe, args, verbose = False):
    cmdargs = [exe] + args
    
    if verbose:
        print "running: " + " ".join(cmdargs)
    
    return subprocess.Popen(cmdargs, executable=exe, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

# ------------------------------------------------------------------------------
# demangle
# ------------------------------------------------------------------------------

def getType(t):
    t = t.strip()
    
    if t.startswith("avmplus::"):
        return t.replace("avmplus::", "")
    elif t.find("S*") != -1:
        return "".join(t.split("S*"))
    elif t.find("*") != -1:
        p = t.find("*")
        return getType(t[0:p]) + t[p:len(t)]
    elif t == "unsigned int" or t == "uint32_t":
        return "uint32_t"
    elif t == "int":
        return "int32_t"
    else:
        return t
    
def demangle(n):
    (stdout, stderr) = runProcess(createProcess('c++filt', [u'-n', n], False), "Unable to demangle...")
    stdout = stdout.replace("((anonymous namespace)::LLVMSelectGetSetDelHasProperty)", "FAIL")
    (functype, args) = stdout.split("(");
    args = args.replace(")", "").split(",")
    functype = functype.replace("<", " ").replace(">", " ").replace(",", " ").replace("unsigned int", "uint32_t").split(" ")

    # we only care if its templated
    if len(functype) > 2:
        return "template %s %s(%s);" % (getType(functype[0]), getType(functype[1]), ", ".join(map(getType, args)).strip())
    else:
        # print "# ignoring non-templated function: %s" % n
        return None


# ------------------------------------------------------------------------------
# Stub order optimisation
# ------------------------------------------------------------------------------

stuborder = {}
pickleFile = "AOTStubs.pickle"

def updateStubOrder(fn):
    global stuborder
    count = 0
    for info in open(fn).read().splitlines():
        count += 1
        bits = info.split("|")
        n = demangle(bits[0].strip())
        try:
            stuborder[n] += int(bits[1])
        except KeyError:
            stuborder[n] = int(bits[1])
    print "# Found %d stubs in %s" % (count, fn)
            
def updateStubOrdering(files):
    global stuborder
    global pickleFile
    if os.path.exists(pickleFile):
        f = open(pickleFile, 'rb')
        stuborder = pickle.load(f)
        f.close()
    else:
        print "No stub ordering file found: '%s'" % os.path.abspath(pickleFile)
    
    if len(files) > 0:
        for fn in files:
            updateStubOrder(fn)
        f = open(pickleFile, 'wb')
        pickle.dump(stuborder, f)
        f.close()

def dumpStubOrderInfo(files):
    global stuborder
    updateStubOrdering(files)
    
    for (s,c) in stuborder.iteritems():
        print "%s | %d" % (s, c)

def getStubSortOrder(stub):
    global stuborder

    substubs = []
    substubs.append( stub )

    # CUIDADO! Be sure to get the number of spaces correct in the replacements
    if stub.find(" DOUBLE_ALLOCA_DECL") != -1:
        substubs.append( stub.replace(" DOUBLE_ALLOCA_DECL", "") )
        substubs.append( stub.replace(" DOUBLE_ALLOCA_DECL", ", double *") )

    for substub in substubs:
        try:
            return stuborder[substub]
        except KeyError:
            pass

    return 0

# ------------------------------------------------------------------------------
# Header Generation
# ------------------------------------------------------------------------------

stubs = []
currentfile = None
stubcount = 0
stubmax = 4000
numstubheaders = 30

def subgroups(xs, n):
    result = []
    s = len(xs)/n
    for i in range(n-1):
        result.append(xs[:s])
        xs = xs[s:]
    if len(xs) > 0:
        result.append(xs)
    return result

def genCPPFiles(stubs, filenum):

    for xs in subgroups(stubs, numstubheaders):
    
        hfile = "AOTStubs-%05d.cpp" % filenum
        hfile = open(hfile, "w")
    
        print >>hfile, standardHeader
        print >>hfile, "#include \"AOTStubs.h\""
        for x in xs:
            print >>hfile, (x[1])
        hfile.close()

        filenum += 1

# ------------------------------------------------------------------------------
# Stub Generation
# ------------------------------------------------------------------------------

argdesctypes = ["uint32_t", "char*"]
vectortypes = ["DoubleVectorObject*", "IntVectorObject*", "UIntVectorObject*", "ObjectVectorObject*"]
objecttypes = ["ScriptObject*", "ArrayObject*", "LLVMAtom"]
receivertypes = objecttypes + ["String*", "double"]
mosttypes =	 ["double", "int32_t", "uint32_t", "String*", "LLVMBool", "Namespace*", "QNameObject*"] + objecttypes
alltypes = ["void"] + mosttypes
multinameIndexTypes = ["LLVMMultinameIndex", "Multiname*"]
multinameIndexTypesWithInt = multinameIndexTypes + ["LLVMMultinameIndexMaybeInt", "LLVMMultinamePtrMaybeInt"]

def genPerms(xs):
    if len(xs) == 0:
        return [[]]
    else:
        p = genPerms(xs[1:])
        return [[x] + y for x in xs[0] for y in p]

def genStubs(name, types, filterFunc=None):
    perms = genPerms(types)
    if (filterFunc is not None):
        perms = filterFunc(perms)
    if len(types) == 1:
        for p in perms:
            genCall((p[0], name, ""))
    else:
        for p in perms:
            genCall((p[0], name, ", ".join(p[1:])))
            
def genCall(params):
    global stubs
    decl = "template %s %s(%s);" % params
    bisect.insort(stubs, (- getStubSortOrder(decl), decl))

def genPropRelatedWithIntOptDouble(name, retTypes, argTypes = [mosttypes]):
    nameTypes = mosttypes + ["LLVMUnusedParam"]
    legalUintNameTypes = set(("double", "int32_t", "uint32_t", "LLVMAtom"))
    legalUintObjectTypes = set(objecttypes) 
    # perm: 0:retType, 1: MethodEnv*, 2:multinameIndex, 3:n, 4:ns, 5:obj
    filterIntPermutations = lambda perms: filter(lambda perm: (perm[2] in multinameIndexTypes) or ((perm[3] in legalUintNameTypes) and (perm[4] == "LLVMUnusedParam") and (perm[5] in legalUintObjectTypes)), perms)
    genStubs(name, [retTypes, ["MethodEnv* DOUBLE_ALLOCA_DECL"], multinameIndexTypesWithInt, nameTypes, nameTypes] + argTypes, filterIntPermutations)

def genPropRelatedWithVectorOpts(name, retTypes, argTypes):
    nameTypes = mosttypes + ["LLVMUnusedParam"]
    legalUintNameTypes = set(("double", "int32_t", "uint32_t", "LLVMAtom"))
    legalUintObjectTypes = set(objecttypes + vectortypes)
    # perm: 0:retType, 1: MethodEnv*, 2:multinameIndex, 3:n, 4:ns, 5:obj
    filterIntPermutations = lambda perms: filter(lambda perm: (perm[2] in multinameIndexTypes) or ((perm[3] in legalUintNameTypes) and (perm[4] == "LLVMUnusedParam") and (perm[5] in legalUintObjectTypes)), perms)
    genStubs(name, [retTypes, ["MethodEnv* DOUBLE_ALLOCA_DECL"], multinameIndexTypesWithInt, nameTypes, nameTypes] + argTypes, filterIntPermutations)

def genPropRelatedWithInt(name, retTypes, argTypes = [mosttypes]):
    nameTypes = mosttypes + ["LLVMUnusedParam"]
    legalUintNameTypes = set(("double", "int32_t", "uint32_t", "LLVMAtom"))
    legalUintObjectTypes = set(objecttypes) 
    # perm: 0:retType, 1: MethodEnv*, 2:multinameIndex, 3:n, 4:ns, 5:obj
    filterIntPermutations = lambda perms: filter(lambda perm: (perm[2] in multinameIndexTypes) or ((perm[3] in legalUintNameTypes) and (perm[4] == "LLVMUnusedParam") and (perm[5] in legalUintObjectTypes)), perms)
    genStubs(name, [retTypes, ["MethodEnv*"], multinameIndexTypesWithInt, nameTypes, nameTypes] + argTypes, filterIntPermutations)

def genPropRelated(name, retTypes, argTypes = [mosttypes]):
    nameTypes = mosttypes + ["LLVMUnusedParam"]
    genStubs(name, [retTypes, ["MethodEnv*"], multinameIndexTypes, nameTypes, nameTypes] + argTypes)
    
# ------------------------------------------------------------------------------
# Main Entrypoint
# ------------------------------------------------------------------------------

if __name__ == "__main__":
    import os.path

    optParser = optparse.OptionParser(usage='usage: %prog [ options ] file1.abc ... fileN.abc')
    optParser.set_defaults()
    optParser.allow_interspersed_args = True
    optParser.add_option( '-d', '--dump', dest="dump", default = False)
    optParser.add_option( '-n', '--numstubheaders', dest="numstubheaders", default = 30)
    optParser.add_option( '-p', '--picklefile', dest="pickleFile", default = None)
    
    (opts, args) = optParser.parse_args()
    
    if opts.dump:
        dumpStubOrderInfo(args)
        sys.exit(0)        
    if opts.pickleFile:
        pickleFile = opts.pickleFile
    updateStubOrdering(args)
    
    genStubs("abcOP_si8", [["void"], ["MethodEnv*"], ["uint32_t", "int32_t", "LLVMAtom"], ["uint32_t", "int32_t", "double", "LLVMAtom"]])
    genStubs("abcOP_si16", [["void"], ["MethodEnv*"], ["uint32_t", "int32_t", "LLVMAtom"], ["uint32_t", "int32_t", "double", "LLVMAtom"]])
    genStubs("abcOP_si32", [["void"], ["MethodEnv*"], ["uint32_t", "int32_t", "LLVMAtom"], ["uint32_t", "int32_t", "double", "LLVMAtom"]])
    genStubs("abcOP_sf32", [["void"], ["MethodEnv*"], ["double", "int32_t", "LLVMAtom"], ["uint32_t", "int32_t", "double", "LLVMAtom"]])
    genStubs("abcOP_sf64", [["void"], ["MethodEnv*"], ["double", "int32_t", "LLVMAtom"], ["uint32_t", "int32_t", "double", "LLVMAtom"]])
    
    genStubs("abcOP_li8", [["uint32_t", "int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_li16", [["uint32_t", "int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_li32", [["uint32_t", "int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_lf32", [["double"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_lf64", [["double"], ["MethodEnv*"], mosttypes])
    
    genStubs("abcOP_sxi1", [["uint32_t", "int32_t", "LLVMAtom"], ["MethodEnv*"], ["uint32_t", "int32_t", "LLVMAtom"]])
    genStubs("abcOP_sxi8", [["uint32_t", "int32_t", "LLVMAtom"], ["MethodEnv*"], ["uint32_t", "int32_t", "LLVMAtom"]])
    genStubs("abcOP_sxi16", [["uint32_t", "int32_t", "LLVMAtom"], ["MethodEnv*"], ["uint32_t", "int32_t", "LLVMAtom"]])

    genPropRelatedWithInt("abcOP_deleteproperty", ["LLVMBool"])
    genPropRelatedWithVectorOpts("abcOP_getproperty", mosttypes, argTypes = [mosttypes + vectortypes])
    genPropRelatedWithVectorOpts("abcOP_getproperty_nonc", mosttypes, argTypes = [mosttypes + vectortypes])
    genPropRelatedWithVectorOpts("abcOP_setproperty", ["void"], argTypes = [mosttypes + vectortypes, mosttypes])
    genPropRelatedWithVectorOpts("abcOP_setproperty_nonc", ["void"], argTypes = [mosttypes + vectortypes, mosttypes])
    genPropRelated("abcOP_initproperty", ["void"], argTypes = [mosttypes, mosttypes])
    genPropRelated("abcOP_callproperty", alltypes, argTypes = [mosttypes, argdesctypes, ["..."]])
	

    genPropRelated("abcOP_constructprop", mosttypes, argTypes = [argdesctypes, ["..."]])
    genPropRelated("abcOP_getdescendants", mosttypes, argTypes = [mosttypes])
    genPropRelated("abcOP_getsuper", mosttypes, argTypes = [mosttypes])
    genPropRelated("abcOP_setsuper", ["void"], argTypes = [mosttypes, mosttypes])
    
    genPropRelated("abcOP_findproperty", mosttypes, argTypes = [["LLVMAtom*"], ["int32_t"], ["int32_t"]])
    genPropRelated("abcOP_findpropstrict", mosttypes, argTypes = [["LLVMAtom*"], ["int32_t"], ["int32_t"]])
    genStubs("abcOP_finddef", [objecttypes, ["MethodEnv*"], multinameIndexTypes])

    genStubs("abcOP_methodEnvFromDispId", [["MethodEnv*"], ["MethodEnv*"], mosttypes, ["int32_t"]])
    genStubs("abcOP_methodEnvFromBaseDispId", [["MethodEnv*"], ["MethodEnv*"], mosttypes, ["int32_t"]])
    genStubs("abcOP_handlerFromMethodEnv", [["int32_t*"], ["MethodEnv*"], mosttypes])

    genStubs("abcOP_call", [mosttypes, ["MethodEnv*"], objecttypes, argdesctypes, ["..."]])
    genStubs("abcOP_callmethod", [alltypes, ["MethodEnv*"], mosttypes, ["int32_t"], argdesctypes, ["..."]])
    genStubs("abcOP_callstatic", [alltypes, ["MethodEnv*"], ["AbcEnv*"], ["int32_t"], argdesctypes, ["..."]])
    genPropRelated("abcOP_callsuper", alltypes, argTypes = [argdesctypes, ["..."]])

    genStubs("abcOP_throwCallOfNonFunctionError", [alltypes, ["MethodEnv*"]])

    genStubs("abcOP_construct", [mosttypes, ["MethodEnv*"], mosttypes, argdesctypes, ["..."]])
    
    genStubs("abcOP_getglobalscope", [mosttypes, ["MethodEnv*"]])

    genStubs("abcOP_findInterfaceBinding", [["int32_t"], ["int32_t"], ["const uint32_t*", "const uint16_t*"]])
    
    genStubs("abcOP_not", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_increment", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_decrement", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_increment_i", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_decrement_i", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])

    genStubs("abcOP_add", [mosttypes, ["MethodEnv* DOUBLE_ALLOCA_DECL"], mosttypes, mosttypes])
    genStubs("abcOP_add_i", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_subtract", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_subtract_i", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_multiply", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_multiply_i", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_divide", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])

    genStubs("abcOP_modulo", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_bitand", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_bitor", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_bitxor", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_bitnot", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    
    genStubs("abcOP_lshift", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_rshift", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_urshift", [["uint32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_negate", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_negate_i", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    
    genStubs("abcOP_true", [["LLVMBool", "LLVMAtom", "int32_t"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_equals", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_strictequals", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_lessthan", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_greaterthan", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_greaterequals", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_lessequals", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_istype", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], ["Traits**"], mosttypes])
    genStubs("abcOP_astype", [mosttypes, ["MethodEnv*"], multinameIndexTypes, mosttypes])
    genStubs("abcOP_istypelate", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_astypelate", [mosttypes, ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_applytype", [mosttypes, ["MethodEnv*"], mosttypes, argdesctypes, ["..."]])
    
    genStubs("abcOP_typeof", [["String*", "LLVMAtom"], ["MethodEnv*"], mosttypes])

    genStubs("abcOP_instanceof", [mosttypes, ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_in", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_coerce", [mosttypes, ["MethodEnv*"], ["Traits**"], mosttypes])
    genStubs("abcOP_coerce_s", [["String*", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_coerce_a", [["LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_coerce_o", [["LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_nullcheck", [["void"], ["MethodEnv*"], mosttypes])
    
    genStubs("abcOP_convert_o", [["ScriptObject*", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_convert_b", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_convert_i", [["int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_convert_u", [["uint32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_convert_s", [["String*", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_convert_d", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_convert_ns", [["Namespace*", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    
    genStubs("abcOP_loadstring", [["String*", "LLVMAtom"], ["MethodEnv*"], ["uint32_t"]])
    genStubs("abcOP_loadnamespace", [["ScriptObject*", "Namespace*", "LLVMAtom"], ["MethodEnv*"], ["uint32_t"]])
    
    genStubs("abcOP_loadundefined", [mosttypes, ["MethodEnv*"]])
    genStubs("abcOP_loadnull", [mosttypes, ["MethodEnv*"]])
    genStubs("abcOP_loadnan", [mosttypes, ["MethodEnv*"]])
    genStubs("abcOP_loadtrue", [mosttypes, ["MethodEnv*"]])
    genStubs("abcOP_loadfalse", [mosttypes, ["MethodEnv*"]])
    
    genStubs("abcOP_setcallee", [["void"], ["MethodEnv*"], mosttypes, mosttypes])

    genStubs("abcOP_getslot", [mosttypes, ["MethodEnv*"], ["char*"], ["ScriptObject*"]])
    genStubs("abcOP_setslot", [["void"], ["MethodEnv*"], ["char*"], ["ScriptObject*"], mosttypes])

    genStubs("abcOP_getslot_nonc", [mosttypes, ["MethodEnv*"], ["char*"], ["ScriptObject*"]])
    genStubs("abcOP_setslot_nonc", [["void"], ["MethodEnv*"], ["char*"], ["ScriptObject*"], mosttypes])

    genStubs("abcOP_pushscope", [["void"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_pushwith", [["void"], ["MethodEnv*"], mosttypes])

    genStubs("abcOP_newactivation", [objecttypes, ["MethodEnv*"]])
    genStubs("abcOP_newobject", [objecttypes, ["MethodEnv*"], argdesctypes, ["..."]])
    genStubs("abcOP_newarray", [objecttypes, ["MethodEnv*"], argdesctypes, ["..."]])
    genStubs("abcOP_newcatch", [objecttypes, ["MethodEnv*"], ["int32_t"], ["char*"]])
    genStubs("abcOP_newfunction", [objecttypes, ["MethodEnv*"], ["Traits**"],  ["Traits***"], ["uint32_t"], ["uint32_t"], ["LLVMAtom*"], ["uint32_t"]])
    genStubs("abcOP_newclass", [objecttypes, ["MethodEnv*"], ["Traits**"], ["Traits**"], ["Traits***"], ["uint32_t"], ["uint32_t"], ["LLVMAtom*"], ["ScriptObject*"], ["uint32_t"]])
    
    genStubs("abcOP_throw", [["void"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_xarg", [mosttypes, ["MethodEnv*"], ["ExceptionFrame*"]])
    
    genStubs("abcOP_hasnext", [["LLVMBool", "int32_t", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_hasnext2", [["LLVMBool", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes, ["int32_t*"], ["LLVMAtom*"]])
    genStubs("abcOP_nextname", [mosttypes, ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_nextvalue", [mosttypes, ["MethodEnv*"], mosttypes, mosttypes])
    
    genStubs("abcOP_dxnslate", [["void"], ["MethodEnv*"], ["AOTMethodFrame*"], mosttypes])
    
    genStubs("abcOP_checkfilter", [["void"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_esc_xattr", [["String*", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_esc_xelem", [["String*", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    
    genStubs("abcOP_upcast", [mosttypes, ["MethodEnv*"], mosttypes])
    genStubs("abcOP_getouterscope", [["ScriptObject*"], ["MethodEnv*"], ["uint32_t"]])

    # math functions
    genStubs("abcOP_Math_abs", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_acos", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_asin", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_atan", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_atan2", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_Math_ceil", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_cos", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_exp", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_floor", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_log", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_pow", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes, mosttypes])
    genStubs("abcOP_Math_round", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_sin", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_sqrt", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])
    genStubs("abcOP_Math_tan", [["double", "LLVMAtom"], ["MethodEnv*"], mosttypes])

    # string functions
    genStubs("abcOP_String_charAt", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_charAt", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"]])
    genStubs("abcOP_String_charCodeAt", [["int32_t", "double", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_charCodeAt", [["int32_t", "double", "LLVMAtom"], ["MethodEnv*"], ["String*"]])
    genStubs("abcOP_String_concat", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], ["LLVMAtom"], ["int32_t"], ["ArrayObject*"]])
    genStubs("abcOP_String_fromCharCode", [["String*", "LLVMAtom"], ["MethodEnv*"], ["LLVMAtom"], ["int32_t"], ["ArrayObject*"]])
    genStubs("abcOP_String_indexOf", [["int32_t", "double", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes, mosttypes])
    genStubs("abcOP_String_indexOf", [["int32_t", "double", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_lastIndexOf", [["int32_t", "double", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes, mosttypes])
    genStubs("abcOP_String_lastIndexOf", [["int32_t", "double", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_localeCompare", [["int32_t", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_match", [["ArrayObject*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_replace", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes, mosttypes])
    genStubs("abcOP_String_search", [["int32_t", "double", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_slice", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes, mosttypes])
    genStubs("abcOP_String_slice", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_split", [["ArrayObject*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes, mosttypes])
    genStubs("abcOP_String_split", [["ArrayObject*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes ])
    genStubs("abcOP_String_substr", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes, mosttypes])
    genStubs("abcOP_String_substr", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_substring", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes, mosttypes])
    genStubs("abcOP_String_substring", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"], mosttypes])
    genStubs("abcOP_String_toLowerCase", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"]])
    genStubs("abcOP_String_toUpperCase", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"]])
    genStubs("abcOP_String_toLocaleLowerCase", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"]])
    genStubs("abcOP_String_toLocaleUpperCase", [["String*", "LLVMAtom"], ["MethodEnv*"], ["String*"]])

    # support for numstubheaders
    numstubheaders = int(opts.numstubheaders)
    stubmax = len(stubs) / int(opts.numstubheaders);
    if (len(stubs) % int(opts.numstubheaders)) == 0:
        stubmax = stubmax - 1;
    
    genCPPFiles(stubs, 0)
