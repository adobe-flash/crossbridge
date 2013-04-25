#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil -*-
# vi: set ts=2 sw=2 expandtab:

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import os
import sys

# helper - calculate the transitive closure
# of a dictionary of sets - in the result,
# (y in d[x] and z in d[y]) implies z in d[x] 
# note: we allow reflexivity but don't ensure it.
def closure(d):
  def close(k):
    vals = set(d[k] if k in d.keys() else [])
    for v in vals.copy():
      if v != k:
        vals |= close(v)
    return vals
  return dict(zip(d.keys(), [close(k) for k in d.keys()]))

# -----------------------------------------------------
#
# Type
# NOTE: not full-powered, just modeling the current necessities.
#

# nullability enum
NULL_ALLOWED = 0      # our value set includes null, and we allow it
NULL_PROHIBITED = 1   # our value set includes null, but we prohibit it
NULL_ABSENT = 2       # our value set does not include null 

# enum used in CG
nullable_names = [
  "kTypeNullable", "kTypeNotNull", "ERROR"
]

class Type:
  def __init__(self, name, valuetype, 
               basename = None, nullable = NULL_ALLOWED, cgoverride = None):
    self.name = name
    self.valuetype = valuetype
    self.basename = basename if basename is not None else name
    self.nullable = nullable
    self.cgoverride = cgoverride
    # print 'name = %s, basename = %s, self.name = %s, self.basename = %s' % (name, basename, self.name, self.basename)
    
  def hasNull(self):
    return self.nullable == NULL_ALLOWED
    
  def ensureNotNullable(self):
    return self if not(self.hasNull()) else getType(self.name + '~')
    
  def isSubOf(self, other):
    # ? is a sub of nothing (really, we should probably throw)
    if self.basename == '?': 
      return False
    # T is a subtype of Top
    if other.basename == 'Top':
      return True
    # Bot is a subtype of T
    if self.basename == 'Bot':
      return True
    # T is a subtype of itself, if we (at least) satisfy other's constraints.
    if self.basename == other.basename:
      # reality check - nullability modifiers are illegal on null-absent types
      if (self.nullable == NULL_ABSENT or other.nullable == NULL_ABSENT) \
        and self.nullable != other.nullable:
        raise ParseError(
          "type specification error: null-absent mismatch when comparing %s and %s" 
          % (self.dump(), other.dump()))
      return self.nullable >= other.nullable
    # T is a subtype of S if T.basename in subtypes[S.basename],
    # if we (at least) satisfy S's additional constraints.
    if other.basename in subtypes and self.basename in subtypes[other.basename]:
      return self.nullable >= other.nullable
    # nope
    return False

  # are we the effect type?
  def isEffect(self):
    return self.basename == 'Effect'

  # format the expression used to denote this type in generated code
  def cgType(self):
    if self.cgoverride is not None:
      return self.cgoverride[self.nullable] if type(self.cgoverride) == dict else self.cgoverride
    if self.nullable == NULL_ABSENT:
      return 'lattice_.%s_type' % self.name.lower()
    else:
      return 'lattice_.%s_type[%s]' % (self.basename.lower(), nullable_names[self.nullable])
      
  # format the expression denoting the lattice traits object 
  # for our type. VERY fragile, should be moved or parameterized
  def cgTraits(self):
    # TODO nullability
    return 'tb.traitsConst(lattice_.%s_traits)' % self.name.lower()
      # 'lattice_.%s_traits' % (self.name.lower(), self.nullable.real))

  def dump(self):
    return self.name

# (AS) value types
#
array_type =        Type('Array', True)
atom_type =         Type('Atom', True)
boolean_type =      Type('Boolean', True, nullable = NULL_ABSENT)
class_type =        Type('Class', True)
function_type =     Type('Function', True)
int_type =          Type('Int', True, nullable = NULL_ABSENT)
number_type =       Type('Number', True, nullable = NULL_ABSENT, cgoverride = 'lattice_.double_type')
object_type =       Type('Object', True)
scriptobject_type = Type('ScriptObject', True)
string_type =       Type('String', True)
ns_type =           Type('Namespace', True)
uint_type =         Type('Uint', True, nullable = NULL_ABSENT)
vectorint_type =    Type('VectorInt', True)
vectoruint_type =   Type('VectorUInt', True)
vectordouble_type = Type('VectorDouble', True)

# value type subgroups for subtype testing
data_types = [array_type, atom_type, boolean_type, class_type,
  function_type, int_type, number_type, object_type,
  scriptobject_type, string_type, ns_type, uint_type,
  vectorint_type, vectoruint_type, vectordouble_type]
  
# VM (value) types
#
effect_type = Type('Effect', False, cgoverride = 'EFFECT')
method_type = Type('Method', False, cgoverride = 'lattice_.method_type')
name_type =   Type('Name', False, cgoverride = 'NAME')
ord_type =    Type('Ord', False, cgoverride = 'ORDINAL')
state_type =  Type('State', False, cgoverride = 'STATE')
traits_type = Type('Traits', False, cgoverride = 'TRAITS')
env_type =    Type('Env', False, cgoverride = 'ENV')

vm_types = [effect_type, method_type, name_type, 
  ord_type, state_type, traits_type, env_type]

# synthetic types, used only in HM compiler
#
top_type = Type('Top', False, cgoverride = 'TOP')
bottom_type =  Type('Bot', False, cgoverride = 'BOT')
topdata_type = Type('TopData', False, cgoverride = 'TOPDATA')

synthetic_types = [top_type, bottom_type, topdata_type]

# type wildcard, used only in CG for inference
#
infer_type = Type('?', False)

# every type, excluding type variables and wildcards
all_types = data_types + vm_types + synthetic_types

# nontrivial part of the subtype relation, used by Type.isSubOf().
# omitted: all < Top, Bot < all, T < T, done in code.
#
# note: we use names rather than type objects themselves only
# to ease the logistics of comparing types modulo qualifiers
# like nullability - each type object has a basename, its name
# stripped of qualifiers, rather than a reference to a base type
# object.
#
# TODO explain subtyping to Atom (model looseness)
# in detail, once refactoring is done
#
subtypes = closure({
  'TopData': [t.name for t in data_types],
  'Atom': [t.name for t in data_types],
  'Number': ['Int', 'Uint'],
  'ScriptObject': ['Array', 'Class', 'Function']
})

# print 'HEY subtypes = \n%s' % '\n'.join(
#   ["%s: [%s]" % (k, ','.join(v for v in subtypes[k])) for k in subtypes.keys()])

# name -> type
types = dict([(t.name, t) for t in 
  data_types + vm_types + synthetic_types + [infer_type]])

# gets a type based on name plus suffixes:
# - name~ means non-nullable 
#
def getType(s):
  if s in types:
    return types[s]
  # not found, try noticing modifiers -
  # if built successfully, register and return
  name = basename = s
  nullable = True
  # TODO other suffixes make this a loop
  if s.endswith('~'):
    basename = s[:len(s)-1]
    nullable = NULL_PROHIBITED
  # if base type is known, build/reg derived type
  if basename in types:
    baset = types[basename]
    t = types[s] = Type(name, baset.valuetype, basename, nullable, baset.cgoverride)
    return t
  # fail
  raise ParseError("unknown type '%s'" % s)

# -----------------------------------------------------
#
# Type propagation
# rather than modeling full dependent types, we model
# some simple implications with per-instruction handlers.
# These are called from Call.calcTypes().
#

# coerce uses arg 1 (a Traits) as the type of result 1
# TODO wrap result in traits type, elim valueAsType?
#
def calctypes_propagate_fromtraits1(call):
  instr = call.base
  if (len(call.args) < 2 or call.args[1].type() != traits_type 
     or len(instr.rettypes) < 2):
    raise ParseError("invalid instruction %s passed to calctypes_propagate_fromtraits0" 
	  % instr.name)
  return [instr.rettypes[0], call.args[1]]

# several instructions propagate the guarantee of non-
# nullity from arg 0 to result 0
#
def calctypes_propagate_nonnull0(call):
  instr = call.base
  if len(call.args) < 1 or len(instr.rettypes) < 1:
    raise ParseError("invalid instruction %s passed to calctypes_propagate_nonnull0" 
	  % instr.name)
  rettypes = instr.rettypes
  if not(call.args[0].type().hasNull()):
    rettypes[0] = rettypes[0].ensureNotNullable()
  return rettypes

# propagate argument type 1 to result 1, but with 
# the non-null constraint guaranteed. 
# TODO: this is basically a poor man's 
#
#    (... T<:Bar ... -> ... T~ ...)
#
# and should ultimately be replaced by it.
#
def calctypes_propagate_argtype1(call):
  instr = call.base
  if len(call.args) < 2 or len(instr.rettypes) < 2:
    raise ParseError("invalid instruction %s passed to calctypes_propagate_argtype1" 
	  % instr.name)
  rettypes = instr.rettypes
  rettypes[1] = call.args[1].type().ensureNotNullable()
  return rettypes
  
# map instrs to calctype handlers
#
calctypes_handlers = {
  "coerce": calctypes_propagate_fromtraits1,

  "string2atom": calctypes_propagate_nonnull0,
  "scriptobject2atom": calctypes_propagate_nonnull0,
  "ns2atom": calctypes_propagate_nonnull0,

  "cknullobject": calctypes_propagate_argtype1
}

# -----------------------------------------------------
#
# Shape, RepInfo
#

# vararg enum
# these do double duty as a) designators of variability,
# b) positions in the shape tuple.
#
EFFECT_IN = 0   # number of effect inputs
DATA_IN = 1     # number of data inputs
EFFECT_OUT = 2  # number of effect outputs
DATA_OUT = 3    # number of data outputs
NONE = 4        # all inputs and outputs fixed
VARKIND = 4     # shape[4] holds varkind
 
# note that effect-in and effect-out args cannot be variable.
#
vararg_names = [
  "ERROR", "kVarIn", "ERROR", "kVarOut", "kVarNone"
]
  
# bare tuple so we can use ==
def shape(neffectin, ndatain, neffectout, ndataout, varkind):
  return neffectin, ndatain, neffectout, ndataout, varkind

# last tuple item gives Varargs enum value, also tuple pos
def varargs(shape):
  return shape[VARKIND]

class RepInfo:
  def __init__(self, name, shape, gen = False):
    self.name, self.shape = name, shape
    self.gen = gen  # if true we gen builder, pred (TODO rep class)
  def enum(self):
    return self.name.upper() + "_SHAPE"
  def isvar(self):
    return self.isvarin() or self.isvarout()
  def isvarin(self):
    return self.shape[VARKIND] == EFFECT_IN or self.shape[VARKIND] == DATA_IN
  def isvarout(self):
    return self.shape[VARKIND] == EFFECT_OUT or self.shape[VARKIND] == DATA_OUT

narystmt0 = RepInfo("NaryStmt0", shape(1, 0, 1, 1, DATA_IN))
narystmt1 = RepInfo("NaryStmt1", shape(1, 1, 1, 1, DATA_IN))
narystmt2 = RepInfo("NaryStmt2", shape(1, 2, 1, 1, DATA_IN))
narystmt3 = RepInfo("NaryStmt3", shape(1, 3, 1, 1, DATA_IN))
narystmt4 = RepInfo("NaryStmt4", shape(1, 4, 1, 1, DATA_IN))
callstmt2 = RepInfo("CallStmt2", shape(1, 2, 1, 1, DATA_IN))
callstmt3 = RepInfo("CallStmt3", shape(1, 3, 1, 1, DATA_IN))
callstmt4 = RepInfo("CallStmt4", shape(1, 4, 1, 1, DATA_IN))
unaryexpr = RepInfo("UnaryExpr", shape(0, 1, 0, 1, NONE), gen = True)
unarystmt = RepInfo("UnaryStmt", shape(1, 1, 1, 1, NONE), gen = True)
binaryexpr = RepInfo("BinaryExpr", shape(0, 2, 0, 1, NONE), gen = True)
binarystmt = RepInfo("BinaryStmt", shape(1, 2, 1, 1, NONE), gen = True)
hasnext2stmt = RepInfo("Hasnext2Stmt", shape(1, 2, 1, 3, NONE))
voidstmt = RepInfo("VoidStmt", shape(1, 0, 1, 0, NONE))
constantexpr = RepInfo("ConstantExpr", shape(0, 0, 0, 1, NONE))
debuginstr = RepInfo("DebugInstr", shape(1, 1, 1, 0, NONE))

# this list is used to populate reps, a map of shapes to reps
# used to choose representations for instrs. Not all shapes
# defined above are on this list, because some are used on
# an override basis only, as specified in instr_rep_overrides
# and shape_rep_overrides, below.
#
replist = [
  narystmt0,
  narystmt1,
  callstmt2,
  callstmt3,
  callstmt4,
  unaryexpr,
  unarystmt,
  binaryexpr,
  binarystmt,
  hasnext2stmt,
  voidstmt,
  constantexpr,
  debuginstr
]

# shape -> rep, used to pick reps for instrs
# based on instr shape (subject to overrides,
# see below)
#
reps = dict([(rep.shape, rep) for rep in replist])

# NOTE: these reps are only used in IR5 shape overrides
# (see instr_rep_overrides). They are *not* selectable
# via reps() by ordinary instructions.
#
startinstr = RepInfo("StartInstr", shape(0, 0, 1, 0, DATA_OUT))
catchblockinstr = RepInfo("CatchBlockInstr", shape(0, 0, 1, 0, DATA_OUT))
stopinstr = RepInfo("StopInstr", shape(1, 0, 0, 0, DATA_IN))

# some instrs are made to use a non-default rep,
# irrespective of the instr shape.
# if an instr name is a key in this map, CG will 
# use the specified rep, rather than trying to
# find a match for the instr's shape in reps
#
instr_rep_overrides = {
  # these overrides allow a custom C++ class to 
  # store extra information.
  #
  "safepoint": RepInfo("SafepointInstr", shape(1, 0, 2, 0, DATA_IN)),
  "setlocal": RepInfo("SetlocalInstr", shape(0, 2, 0, 1, NONE)),
  # DEOPT: I think I need this because deopt_safepoint has
  # extra fields for vpc, scopep, sp, etc.
  "deopt_safepoint": RepInfo("DeoptSafepointInstr", shape(1, 0, 1, 0, DATA_IN)),
  "deopt_finish": RepInfo("DeoptFinishInstr", shape(1, 0, 1, 0, NONE)),
  "deopt_finishcall": RepInfo("DeoptFinishCallInstr", shape(1, 1, 1, 0, NONE)),

  "debugline": debuginstr,
  "debugfile": debuginstr,
  
  # we select IR5 (block delimiter) reps exclusively by
  # override. this lets us keep the IR5 reps out of the
  # standard reps dictionary, which ensures that ordinary 
  # instructions with compatible shapes never inadvertently 
  # select IR5 reps, which have special semantics.
  # 
  "goto": RepInfo("GotoInstr", shape(0, 0, 0, 0, DATA_IN)),
  "label": RepInfo("LabelInstr", shape(0, 0, 0, 0, DATA_OUT)),
  "if": RepInfo("IfInstr", shape(0, 1, 0, 0, DATA_IN)),
  "switch": RepInfo("SwitchInstr", shape(0, 1, 0, 0, DATA_IN)),
  "arm": RepInfo("ArmInstr", shape(0, 0, 0, 0, DATA_OUT)),
  "return": stopinstr,
  "throw": stopinstr,
  "start": startinstr,
  "template": startinstr,
  "catchblock": RepInfo("CatchBlockInstr", shape(0, 0, 0, 0, DATA_OUT)),

  #
  # fixed-arg instructions for which we want to use a
  # specific vararg rep that provides VM call semantics.
  #
  
  "newfunction": narystmt1,
  "newclass": narystmt2,

  "abc_findproperty": narystmt3,
  "abc_findpropertyx": narystmt4,
  "abc_findpropertyns": narystmt4,
  "abc_findpropertynsx": narystmt4,
  "abc_findpropstrict": narystmt3,
  "abc_findpropstrictx": narystmt4,
  "abc_findpropstrictns": narystmt4,
  "abc_findpropstrictnsx": narystmt4,
  "findprop2getouter": narystmt1,
  "findprop2finddef": narystmt1,

  "abc_getsuper": callstmt2,
  "abc_getsuperx": callstmt3,
  "abc_getsuperns": callstmt3,
  "abc_getsupernsx": callstmt4,
  "abc_getdescendants": callstmt2,
  "abc_getdescendantsx": callstmt3,
  "abc_getdescendantsns": callstmt3,
  "abc_getdescendantsnsx": callstmt4,
  "abc_getprop": callstmt2,
  "abc_getprop_slot": callstmt2,
  "abc_getpropx": callstmt3,
  "getpropx_u": callstmt3,
  "getpropx_i": callstmt3,
  "getpropx_d": callstmt3,
  "getpropx_au": callstmt3,
  "getpropx_ai": callstmt3,
  "getpropx_ad": callstmt3,
  "getpropx_viu": callstmt3,
  "getpropx_vii": callstmt3,
  "getpropx_vid": callstmt3,
  "getpropx_vuu": callstmt3,
  "getpropx_vui": callstmt3,
  "getpropx_vud": callstmt3,
  "getpropx_vdu": callstmt3,
  "getpropx_vdi": callstmt3,
  "getpropx_vdd": callstmt3,
  "abc_getpropns": callstmt3,
  "abc_getpropnsx": callstmt4,

  "abc_getslot": callstmt2,
  "getslot": callstmt2,

  "abc_deleteprop": callstmt2,
  "abc_deletepropx": callstmt3,
  "abc_deletepropns": callstmt3,
  "abc_deletepropnsx": callstmt4,
}

# to keep things simple, the shape-to-rep logic 
# does *not* attempt to match fixed-arg shapes 
# with compatible vararg reps, if no matching 
# fixed-arg rep is found in reps. instead, we 
# just give those mappings here, explicitly.
# 
shape_rep_overrides = {
  # route get/set ternaries and above to callstmts
  shape(1, 3, 1, 1, NONE): callstmt2,
  shape(1, 4, 1, 1, NONE): callstmt3,
  shape(1, 5, 1, 1, NONE): callstmt4
}

# look through shape table and overrides to get the rep for a given def
# if add flag is true, new shapes are added to the global replist table.
# codegen treats the replist as canon, so we don't add the shapes of
# subtemplates (like internal if arms and label defs) that don't require
# explicit factory etc. support.
# 
def getRep(d, add = False):
  if d.name in instr_rep_overrides:
    return instr_rep_overrides[d.name]
  elif d.shape in shape_rep_overrides:
    return shape_rep_overrides[d.shape]
  else:
    # NOTE: make it if it's not there, if add flag is set
    if not(d.shape in reps.keys()):
      shapeinfo = RepInfo("Shape_%s" % '_'.join(map(str, d.shape)), d.shape)
      if add:
          replist.append(shapeinfo)
      reps[d.shape] = shapeinfo
    return reps[d.shape]

# -----------------------------------------------------
#
# ParseError
#

# global current item for error reporting
curitem = None

def setCurItem(item):
  global curitem
  prev = curitem
  curitem = item
  return prev

def getCurItem():
  return curitem
  
def dumpItem(item):
  return item.dump() if hasattr(item, 'dump') else str(item)

# ParseError uses curitem in message
class ParseError(Exception):
  def __init__(self, errmsg):
    self.errmsg = errmsg
    self.item = getCurItem()
  def message(self):
    return '%s\n  in %s' % (self.errmsg, dumpItem(self.item))

# recognize real sexprs among comments, etc.
def isValid(sexpr):
  return type(sexpr) == list and len(sexpr) > 0 and type(sexpr[0]) == str

# -----------------------------------------------------
# 
# Prototype and Template
#

# split (partypes -> rettypes) sig into pair
def splitSig(sig):
  insig, outsig = [], []
  arrow = False
  for s in sig:
    if s == '->':
      if arrow:
        raise ParseError("arrow appears more than once in sig")
      arrow = True
    elif arrow:
      outsig.append(s)
    else:
      insig.append(s)
  #if len(outsig) == 0:
    #raise ParseError("signature missing return type(s)")
  return insig, outsig

# indexes where list items pass pred
def where(pred, li):
  return filter(pred, range(0, len(li)))

# return sublist built from elements at given indexes
def sublist(li, ixs):
  res = []
  for i in range(0, len(ixs)):
    res.append(li[ixs[i]])
  return res

# number of list items that pass pred
def count(pred, li):
  return len(filter(pred, li))

# occurences of effect_type in type list
def effectCount(types):
  return types.count(effect_type)

# occurences of data (non-effect) types in type list
def dataCount(types):
  return len(types) - effectCount(types)

# position of nth data type in type list
def nthData(types, n):
  data = where(lambda i: not(types[i].isEffect()), types)
  return data[n] if n < len(data) else -1

# return (resolved type list, is-variadic)
def parseSig(sig):
  varin = False
  types = []
  for s in sig:
    if varin:
      raise ParseError("signature %s is variadic before final entry" % sig)
    if s.startswith('[') and s.endswith(']'):
      varin = True
      s = s[1:len(s) - 1]
    types.append(getType(s))
  return types, varin

# InstrDef > Prototype > Template
#
class InstrDef:
  def __init__(self, name):
    self.name = name
  def hrname(self):
    return "HR_%s" % self.name

#
# Prototype has signatures but no body.
#
class Prototype(InstrDef):
  
  # ctor parses and optionally checks sigs.
  # protos check immediately, tems check post-inference
  def __init__(self, name, argsig, retsig, check = True):
    InstrDef.__init__(self, name)
    (self.partypes, self.isvarin) = parseSig(argsig)
    (self.rettypes, self.isvarout) = parseSig(retsig)
    if check:
        self.checkSigs()

  # overridden to return true in Template
  def isTemplate(self): 
    return False

  # number of params
  def numParams(self):
    return len(self.partypes)

  # number of results
  def numResults(self):
    return len(self.rettypes)

  # check signature constraints, set shape
  def checkSigs(self):
    # variadic param xor result
    if self.isvarin and self.isvarout:
      raise ParseError("args and results cannot both be variadic")
    # ? types only in templates      
    if not self.isTemplate and (
        self.rettypes.count(infer_type) > 0 or 
        self.partypes.count(infer_type) > 0):
      raise ParseError("prototype signatures cannot be inferred")
    # set shape
    if self.isvarin:
      last = self.partypes[len(self.partypes) - 1]
      self.varargs = EFFECT_IN if last.isEffect() else DATA_IN
    elif self.isvarout:
      last = self.rettypes[len(self.rettypes) - 1]
      self.varargs = EFFECT_OUT if last.isEffect() else DATA_OUT
    else:
      self.varargs = NONE
    shapeinfo = [
        effectCount(self.partypes), dataCount(self.partypes),
        effectCount(self.rettypes), dataCount(self.rettypes)]
    if self.varargs != NONE:
      shapeinfo[self.varargs] -= 1
    self.shape = shape(shapeinfo[EFFECT_IN], shapeinfo[DATA_IN],
                       shapeinfo[EFFECT_OUT], shapeinfo[DATA_OUT],
                       self.varargs)
    # self.rep = getRep(self, self.numParents() == 0)

  # linear instructions can be used as subexpressions. linear is:
  # - at most one effect in, 
  # - same number of effect in and out, 
  # - exactly one data out.
  def isLinear(self):
    fixedeffectin = not(self.isvarin and self.partypes[len(self.partypes) - 1].isEffect())
    neffectin = effectCount(self.partypes)
    fixedout = not self.isvarout
    neffectout = effectCount(self.rettypes)
    ndataout = dataCount(self.rettypes)
    return (fixedeffectin and fixedout and
            neffectin <= 1 and neffectout == neffectin and 
            ndataout == 1)

  # pure instructions do not interact with effects
  def isPure(self):
    return effectCount(self.partypes) + effectCount(self.rettypes) == 0

  # return our nesting level
  def numParents(self):
    return 0

  def dump(self):
    argsig = ', '.join([t.dump() for t in self.partypes])
    if self.numParams() != 1: argsig = '(' + argsig + ')'
    retsig = ', '.join([t.dump() for t in self.rettypes])
    if self.numResults() != 1: retsig = '(' + retsig + ')'
    return '%s: %s -> %s' % (self.name, argsig, retsig)
    
  def dumpSig(self):
    return self.dump()

#
# Template adds body (list of statements) and state mgmt 
#
class Template(Prototype):

  # ctor sets initial sig (to-infer types for explicit params,
  # plus new effect input), and sets up state.
  def __init__(self, name, parnames, body, parent = None):
    Prototype.__init__(self, name, ['Effect'] + (['?'] * len(parnames)), [], False)
    self.effectid, self.dataid, self.instrid = -1, -1, -1
    self.parent = parent
    self.ambeffects = []
    self.defAmbientEffect(self.genLocalName(effect_type))
    self.parnames = [self.ambeffects[0]] + parnames
    self.locals = {}
    self.body = body
    self.labels = {}
    self.done = False
    
  def isTemplate(self):
    return True
    
  # called when an instr produces ambient effect (as unbound result)
  def defAmbientEffect(self, varname):
    self.ambeffects.append(varname)
    return varname
    
  # called when an instr consumes ambient effect (as implicit arg)
  def useAmbientEffect(self):
    if not(self.hasAmbientEffect()):
      raise ParseError("no ambient effect var available")
    return self.ambeffects.pop()
    
  # True if ambient effect is currently available
  def hasAmbientEffect(self):
    return len(self.ambeffects) > 0
    
  # check name for collisions with local bindings.
  # as with generated names, we insist on template-wide
  # uniqueness.
  #
  def checkLocalName(self, name):
    if self.parent is not None:
      self.parent.checkLocalName(name)
    if name in self.parnames:
      raise ParseError("local name %s clashes with param name" % name)
    if name in self.locals:
      raise ParseError("local %s already defined")

  # generate new local variable name, based on a key.
  # key can a Type, an AST node representing a dependent-type-carrying
  # value, or a string.
  #
  # TODO: if we refactor type-carrying values so that they're types,
  # this code will get cleaner.
  #
  # note: we use the top-level template's suffix generator, so that
  # all variable names are unique template-wide. this may not always
  # be necessary for CG, but collisions in certain situations are
  # hard to avoid, even with block scoping in generated code.
  #
  def genLocalName(self, key):
    if self.parent is not None:
      return self.parent.genLocalName(key)
    # at top level, generate unique ids
    if type(key) == str:
      # for string keys, use instrid
      self.instrid += 1
      return '%s%i' % (key, self.instrid)
    else:
      # may be Type or AST node
      if key.isEffect():
        self.effectid += 1
        return 'effect%i' % self.effectid
      else:
        self.dataid += 1
        return 'data%i' % self.dataid

  # given a local or param name, build a reference node or throw
  # note qualified lookup: incoming name is unqualified, but may
  # refer to a local def in a nested template.
  def makeRef(self, name):
    if name in self.parnames:
      return ParamRef(name, self)
    elif name in self.locals:
      return LocalRef(name, self)
    elif self.parent is not None:
      return self.parent.makeRef(name)
    else:
      raise ParseError("no binding for name '%s'" % name)

  # add an effect pass-through to a pure template
  def addEffect(self):
    if not(self.isPure()):
      raise ParseError("addEffect() on impure template '%s'" % self.name)
    effparam = self.genLocalName(effect_type)
    self.parnames.insert(0, effparam)
    self.partypes.insert(0, effect_type)
    self.rettypes.insert(0, effect_type)
    retstmt = self.body[len(self.body) - 1]
    if (retstmt.kind != 'LocalDefs' or 
        retstmt.expr.kind != 'Call' or
        retstmt.expr.base.name != 'return'):
      raise ParseError("internal error: missing return stmt in pure template '%s'" % self.name)
    retstmt.expr.args.insert(0, self.makeRef(effparam))
    
  # helper - dump signature items
  def dumpSigItems(self):
    argsig = '(' + ', '.join([n + ':' + t.dump() for n,t in zip(self.parnames, self.partypes)]) + ')'
    retsig = ', '.join([t.dump() for t in self.rettypes])
    if self.numResults() != 1: retsig = '(' + retsig + ')'
    return argsig, retsig
      
  # return our nesting level
  def numParents(self):
    return 0 if self.parent is None else 1 + self.parent.numParents()

  def hasGoto(self):
    return any([stmt.hasGoto() for stmt in self.body])

  # true if all paths out of the template are gotos to labels
  # defined in parent templates
  def allPathsEscape(self):
    return all([e.isEscapingGoto(self) for e in self.finalExprs()])

  # return the set of final statements in this template.
  # these are the last executable statements (i.e., prior
  # to return) on every path out of the template.
  def finalExprs(self):
    last = self.body[len(self.body) - 2]
    expr = last.expr
    if expr.base.name == 'if':
      return expr.iftrue.finalExprs() + expr.iffalse.finalExprs()
    else:
      return [expr]

  def dump(self):
    indent = 2 * self.numParents()
    argsig, retsig = self.dumpSigItems()
    spaces = ' ' * indent
    labeldumps = ''.join(['\n  ' + spaces + label.dump()
      for label in self.labels.values()])
    bodydump = '\n  '.join([spaces + dumpItem(instr) for instr in self.body])
    return '%s: %s -> %s\n  %s%s' % (self.name, argsig, retsig, bodydump, labeldumps)

  def dumpSig(self):
    return '%s: %s -> %s' % ((self.name,) + self.dumpSigItems())

# create a Template or Prototype from an sexpr
def toDef(sexpr):
  setCurItem(sexpr)
  if len(sexpr) == 2:
    argsig, retsig = splitSig(sexpr[1])
    return Prototype(sexpr[0], argsig, retsig)
  else:
    return Template(sexpr[0], sexpr[1], sexpr[2:])

# helper - dump a list
def dumpList(li):
  if len(li) == 1: 
    return ''.join([e.dump() for e in li])
  return '(%s)' % ', '.join([e.dump() for e in li])

# -----------------------------------------------------
#
# AST nodes
#
 
# Call describes an instr invocation. Calls always deliver 
# a composite result, and only appear on RHS of binding 
# statements (LocalDefs), not as subexpressions.
# 
class Call:
  kind = 'Call'
  def __init__(self, base, args):
    self.base, self.args = base, args
    self.types_ = self.calcTypes()
    
  # list of result types    
  def types(self):
    return self.types_
    
  # calculate result types for this call site.
  # base case simply copies the result types of the 
  # called instr (self.base), but if there is a registered
  # type propagation function, we call that instead
  #
  def calcTypes(self):
    if self.base.name in calctypes_handlers.keys():
      return calctypes_handlers[self.base.name](self)
    else:
      return self.base.rettypes
      	  
  # true if we pass through at most one effect value,
  # and generate exactly one data value
  def isLinear(self):
    return self.base.isLinear()

  def hasGoto(self):
    return False

  # true if this
  def isEscapingGoto(self, tem):
    return False

  # helper - dump list of types to list of strings
  def dumpTypes(self):
    return dumpList(self.types())
    
  def dump(self):
    return '%s(%s)' % (self.base.name, ', '.join([a.dump() for a in self.args]))

# Goto is syntactically almost identical to a Call,
# but semantics are fairly different
#
class Goto(Call):
  def __init__(self, base, args):
    Call.__init__(self, base, args)

  def target(self):
    return self.base

  def hasGoto(self):
    return True

  # true if this goto jumps to a label outside the given template
  def isEscapingGoto(self, tem):
    return self.base.parent != tem

  def dump(self):
    return 'goto %s(%s)' % (self.base.name, ', '.join([a.dump() for a in self.args]))
  
# IfCall(condexpr, iftrue, iffalse)
#
class IfCall:
  kind = 'Call'
  def __init__(self, ifbase, condexpr, iftrue, iffalse):
    self.condexpr = condexpr
    self.base, self.args = ifbase, []
    self.iftrue, self.iffalse = iftrue, iffalse
    self.types_ = self.calcTypes()
    self.pure = not(self.types()[0].isEffect())
    
  # list of result types
  def types(self):
    return self.types_
    
  # calculate result types for this if. 
  #
  def calcTypes(self):
  
    truetypes = self.iftrue.rettypes
    falsetypes = self.iffalse.rettypes

    if len(truetypes) != len(falsetypes):
      # no obvious better place to do this: check if the mismatch
      # is due to purity on one side but not the other, and fix up
      nt, nf = len(truetypes), len(falsetypes)
      if nt == nf - 1 and self.iftrue.isPure() and not(self.iffalse.isPure()):
        self.iftrue.addEffect()
      elif nf == nt - 1 and self.iffalse.isPure() and not(self.iftrue.isPure()):
        self.iffalse.addEffect()
      else:
        raise ParseError("true and false arms return different shapes: %s, %s" %
          (dumpList(truetypes), dumpList(falsetypes)))
      
    rettypes = []
    pos = 0

    # build this call site's result type list by reconciling the type
    # lists of our subtemplates.
    #
    # NOTE: this is one of a few places (another is checkargtype) where
    # we do in-passing type inference, rather than maintaining a substitution
    # table. So far it seems like a good tradeoff, but cases are definitely 
    # missed due to limited information flow. At some point we'll probably 
    # have to bite the bullet. 
    #    
    for tt,ft in zip(truetypes, falsetypes):
      if tt.isSubOf(ft):
        rettypes.append(ft)
      elif ft.isSubOf(tt):
        rettypes.append(tt)
      elif tt == infer_type and ft == infer_type:
        rettypes.append(top_type)
        self.iftrue.rettypes[pos] = top_type
        self.iffalse.rettypes[pos] = top_type
      elif tt == infer_type:
        rettypes.append(ft)
        self.iftrue.rettypes[pos] = ft
        retexpr = self.iftrue.body[len(self.iftrue.body) - 1].expr
        if retexpr.args[pos].kind == 'ParamRef':
          retexpr.args[pos].setParamType(ft)
      elif ft == infer_type:
        rettypes.append(tt)
        self.iffalse.rettypes[pos] = tt
        retexpr = self.iffalse.body[len(self.iffalse.body) - 1].expr
        if retexpr.args[pos].kind == 'ParamRef':
          retexpr.args[pos].setParamType(tt)
      else:
        raise ParseError(
          "true and false arms return incompatible types at position %i: %s, %s" %
          (pos, tt.dump(), ft.dump()))
      pos += 1
      
    return rettypes
    
  # add an argument
  def addArg(self, arg):
    self.args.append(arg)

  # true if we pass through at most one effect value,
  # and generate exactly one data value
  def isLinear(self):
    partypes = [arg.type() for arg in self.args]
    rettypes = self.types()
    neffectin = effectCount(partypes)
    neffectout = effectCount(rettypes)
    ndataout = dataCount(rettypes)
    return neffectin <= 1 and neffectout == neffectin and ndataout == 1

  def hasGoto(self):
    return self.iftrue.hasGoto() or self.iffalse.hasGoto()

  def dump(self):
    nparents = self.iftrue.numParents()
    indent = ' ' * 2 * nparents
    return ('if %s\n%s{ %s }\n%s{ %s }' % (
      dumpList([self.condexpr] + self.args), 
      indent, self.iftrue.dump(),
      indent, self.iffalse.dump()))

# A LocalDefs node binds a set of names to a shared RHS.
# Calls and non-composite rvalues are ok.
#
class LocalDefs:
  kind = 'LocalDefs'
  def __init__(self, lhs, expr): 
    self.lhs = lhs
    self.expr = expr
    if expr.kind == 'Call':
      self.names = lhs
      self.defs = dict(zip(lhs, [Selector(expr, i) for i in range(0, len(lhs))]))
    else:
      self.names = [lhs]
      self.defs = { lhs : expr }

  def hasGoto(self):
    return self.expr.kind == 'Call' and self.expr.hasGoto()

  def dumpnames(self):
    decls = ['%s:%s' % (n, self.defs[n].type().dump()) for n in self.names]
    return '(%s)' % ', '.join(decls) if self.expr.kind == 'Call' else decls[0]

  def dump(self):
    return '%s = %s' % (self.dumpnames(), self.expr.dump())
    
# Selector extracts an item from a composite value by position.
#
class Selector:
  kind = 'Selector'
  def __init__(self, base, pos): 
    if base.kind != 'Call':
      raise ParseError("selector on non-composite base %s" % base.dump())
    self.base, self.pos = base, pos
  def type(self):
    return self.base.types()[self.pos]
  def deref(self):
    return self
  def isSubOf(self, t):
    return valueAsType(self).isSubOf(t)
  def isEffect(self):
    return valueAsType(self).isEffect()
  def dump(self):
    return '%s[%i]' % (self.base.dump(), self.pos)

# LocalRef refers by name to a local in an enclosing template
#
class LocalRef:
  kind = 'LocalRef'
  def __init__(self, name, tem): 
    self.name, self.tem = name, tem
  def type(self): 
    return self.deref().type()
  def deref(self): 
    return self.tem.locals[self.name]
  def isSubOf(self, t): 
    return self.deref().isSubOf(t)
  def isEffect(self): 
    return self.deref().isEffect()
  def dump(self):
    return self.name

# ParamRef refers by name to a param in an enclosing template
#
class ParamRef:
  kind = 'ParamRef'
  def __init__(self, name, tem): 
    self.name, self.tem = name, tem
  def pos(self): 
    return self.tem.parnames.index(self.name)
  def type(self): 
    return self.tem.partypes[self.pos()]
  def deref(self): 
    return self
  def isSubOf(self, t): 
    return valueAsType(self).isSubOf(t)
  def isEffect(self): 
    return valueAsType(self).isEffect()
  def setParamType(self, newtype):
    self.tem.partypes[self.pos()] = newtype
    return self.type()
  def dump(self): 
    return self.name

# TypeConst holds a constant type value
#
class TypeConst:
  kind = 'TypeConst'
  def __init__(self, value): 
    self.value = value
  def type(self): 
    return traits_type
  def deref(self): 
    return self.value
  def isSubOf(self, t): 
    return self.deref().isSubOf(t)
  def isEffect(self): 
    return self.deref().isEffect()
  def dump(self):
    return self.value.dump()
    
# NumConst holds a constant Int or Number value
#
class NumConst:
  kind = 'NumConst'
  def __init__(self, value): 
    self.value = value
  def type(self): 
    return int_type if type(self.value) == int else number_type
  def deref(self): 
    return self.value
  def isSubOf(self, t):
    raise ParseError("(%s).isSubOf(%s): type const or type-valued expr expected" 
          % (self.dump(), t.dump()))
  def isEffect(self):
    raise ParseError("(%s).isEffect(): type const or type-valued expr expected" 
          % self.dump())
  def dump(self):
    return '%i' % self.value

# VarRef holds the name of a C++ variable.
# This name will be dumped straight into generated code. 
# Types are looked up in VarTypes  
#
class VarRef:
  kind = 'VarRef'
  def __init__(self, value): 
    self.value = value
    if not(value in VarTypes):
      raise ParseError("no type binding for name '%s'" % value)
    self.type_ = VarTypes[value]
  def type(self): 
    return self.type_
  def deref(self): 
    return self.value
  def isSubOf(self, t):
    raise ParseError("(%s).isSubOf(%s): type const or type-valued expr expected" 
          % (self.dump(), t.dump()))
  def isEffect(self):
    raise ParseError("(%s).isEffect(): type const or type-valued expr expected" 
          % self.dump())
  def dump(self):
    return '%s' % self.value

#
# end of AST nodes
#

# C++ vars registered here are available in s-exprs
#
VarTypes = {
  "true": boolean_type,
  "false": boolean_type,
  
  "kAtomTypeMask": int_type,
  "kObjectType": int_type,
  "kStringType": int_type,
  "kNamespaceType": int_type,
  "kSpecialType": int_type,
  "kBooleanType": int_type,
  "kIntptrType": int_type,
  "kDoubleType": int_type
} 

# dependent types helper: only a traits-typed value can represent 
# a type, and it can represent any data type. 
def valueAsType(v):
  if v.type() == traits_type:
    return topdata_type
  raise ParseError("valueAsType(%s): traits-typed value required" 
        % v.dump())

# check type of arg expr against type of param.
# The type of the argument must be a subtype of the type of the 
# corresponding parameter of the called instruction.
#
# Type inference also happens here: the passing of params (of the 
# calling function) is noticed. (a) ?-typed params are constrained 
# to call-compatible types, and (b) super-typed params are tightened 
# to call-compatible subtypes.
#
# ...this isn't full unification, since the collapsing of type 
# wildcards happens only in the direction of the normal flow of
# processing. The full monty would require differentiated type 
# vars and a substitution table, but I'm avoiding that while I
# can.
#
def checkArgType(expr, goaltype, base):
  exprtype = expr.type()
  if expr.kind == 'ParamRef':
    if exprtype == infer_type or (exprtype != goaltype and goaltype.isSubOf(exprtype)):
      exprtype = expr.setParamType(goaltype)
  if not(exprtype.isSubOf(goaltype)):
    raise ParseError("argument type %s not compatible with parameter type %s\n  in arg '%s'\n  in call to = %s" 
          % (exprtype.dump(), goaltype.dump(), expr.dump(), base.dump()))

# check binding arity - may insert ambient effect local
#
def checkBindingArity(rettypes, names, tem):
  if len(names) != len(rettypes):
    if len(names) == len(rettypes) - 1 and rettypes[0].isEffect():
      names.insert(0, tem.defAmbientEffect(tem.genLocalName(effect_type)))
    else:
      raise ParseError("expected %i binding vars, got %i: %s" 
            % (len(rettypes), len(names), ','.join(names)))

# generate bindings to the multiple results of a Call.
#
def genCallBindings(tem, call, names = None):
  if names is None:
    # generate names
    names = []
    for t in call.types():
      name = tem.genLocalName(t)
      if t.isEffect():
        tem.defAmbientEffect(name)
      names.append(name)
  else:
    # check user-supplied names
    if type(names) != list:
      names = [names]
    for name in names:
      tem.checkLocalName(name)
    checkBindingArity(call.types(), names, tem)

  return LocalDefs(names, call)

# generate binding to a non-call RHS
#
def genValueBinding(tem, expr, name = None):
  if name is None:
    name = tem.genLocalName(expr.type())
  else:
    tem.checkLocalName(name)
  return LocalDefs(name, expr)

# add local bindings for expr results to template, 
# and return LocalDefs for addition to template body.
# lhs is user-supplied names - we generate if None
#
def genBindings(tem, expr, lhs = None):
  savecuritem = setCurItem('%s in %s' % (expr.dump(), tem.name))

  if expr.kind == 'Call':
    instr = genCallBindings(tem, expr, lhs)
  else:
    instr = genValueBinding(tem, expr, lhs)

  # note: template locals are added here
  for n, v in instr.defs.items():
    tem.locals[n] = v

  setCurItem(savecuritem)
  return instr

# process a subexpression in a template body.
# linear Call results are scattered to locals, 
# data result is used as subexpr value.
#
def processSubexpr(sexpr, t, defmap, stack, instrs):
  savecuritem = setCurItem('%s in %s' % (sexpr, t.name))
  expr = processExpr(sexpr, t, defmap, stack, instrs)
  if expr.kind == 'Call':
    if expr.isLinear():
      # generate local bindings and add as body statement
      locdefs = genBindings(t, expr)
      instrs.append(locdefs)
      # use data result as subexpression value
      result = LocalRef(locdefs.names[nthData(expr.types(), 0)], t)
    else:
      raise ParseError("cannot be used as a subexpression: %s" % expr.dump())
  else:
    result = expr
  setCurItem(savecuritem)
  return result

# check call arity - note: raises ParseError if there's a problem,
# return value true indicates that call needs ambient effect added.
# (can't do it here because this happens before subexpr processing,
# and adding effect happens after)
#
def checkArity(op, args):
  argc = len(args)
  # ok if we match fixed count or exceed variadic min
  if argc == op.numParams() or (op.isvarin and argc >= op.numParams() - 1):
    return False
  # allow for effect insertion
  if op.partypes[0] == effect_type:
    if argc == op.numParams() - 1 or (op.isvarin and argc == op.numParams() - 2):
      return True
  # nope
  raise ParseError("expected %i arguments to '%s', got %i" % (
    op.numParams(), op.name, len(args)))

# get type for particular position, taking variadic last types into account.
# note that we're skating by on T < [T] until/unless we build a proper 
# List(T) rep for list types, otherwise we'd have to make list/nonlist 
# copies of everything upfront.
#
def typeAtPosition(types, pos, varin):
  lastpos = len(types) - 1
  if varin and pos >= lastpos:
    return types[lastpos]
  else:
    return types[pos]

# helper: true if expr list has long-form syntax.
# long form syntax is recognized by the presence of keywords
# (strings ending with ':') at the head of list-typed exprs.
#
def isLongForm(exprs):
  return any([type(e) == list and e[0].endswith(':') for e in exprs])

# helper: process if-statement arms from list of expressions.
# both long-form (keyed, order independent) and short-form
# (unkeyed, ordered) syntax is handled.
# returns a map from if-statement selector keywords (true:, false:)
# to statement lists.
#
def processIfArms(exprs):
  arms = {}
  sels = ['true:', 'false:']

  if len(exprs) != len(sels):
    raise ParseError("exactly two arms required in if statement")

  if isLongForm(exprs):
    for expr in exprs:
      savecuritem = setCurItem('%s in %s' % (expr, getCurItem()))
      if type(expr) != list:
        raise ParseError("mixed short-form and long-form if arms")
      sel = expr[0]
      if sel in arms:
        raise ParseError("conditional selector %s appears more than once" % sel)
      if not(sel in sels):
        raise ParseError("unrecognized conditional selector %s" % sel)
      stmts = expr[1:]
      arms[sel] = stmts
      setCurItem(savecuritem)
  else:
    for i in range(0, len(exprs)):
      expr = exprs[i]
      savecuritem = setCurItem('%s in %s' % (expr, getCurItem()))
      sel = sels[i]
      stmts = [expr]
      arms[sel] = stmts
      setCurItem(savecuritem)

  return arms

# process an expr in a template body. 
# note: calls to unprocessed templates are handled eagerly
# with a recursive call to processTem()
#
# sexpr - s-expression we're processing
# tem - template in whose body the expression occurs
# defmap - available definitions (may not be processed)
# stack - call-target stack, passed through on subexprs  
# instrs - instr list being produced, passed through on subexprs
#
def processExpr(sexpr, tem, defmap, stack, instrs):
  savecuritem = setCurItem('%s in %s' % (sexpr, tem.name))
  
  if type(sexpr) == list:
    # invocation
    name, args = sexpr[0], sexpr[1:]

    if name == 'if':
      op = defmap['if'] 
      cond_expr = processSubexpr(args[0], tem, defmap, stack, instrs)
      checkArgType(cond_expr, boolean_type, op)

      if len(args) > 3:
        raise ParseError('too many subexpressions in (if ...)')

      arms = processIfArms(args[1:])

      iftrue = Template('true', [], arms['true:'], tem)
      processTem(iftrue, defmap, stack)

      iffalse = Template('false', [], arms['false:'], tem)
      processTem(iffalse, defmap, stack)
      
      ifstmt = IfCall(op, cond_expr, iftrue, iffalse)
      
      if not ifstmt.pure:
        effect_arg = processSubexpr(tem.useAmbientEffect(), tem, defmap, stack, instrs)
        ifstmt.addArg(effect_arg)
      
      result = ifstmt

    elif name == 'goto':
      labelname, args = args[0], args[1:]

      op = findLabel('@' + labelname, tem)
      if op is None:
        raise ParseError("no label for name '%s'" % labelname)

      if not op.done:
        processTem(op, defmap, stack)
        
      needs_effect = checkArity(op, args)
      argoff = 1 if needs_effect else 0
      
      for i in range(0, len(args)):
        args[i] = processSubexpr(args[i], tem, defmap, stack, instrs)
        pos = i + argoff
        goal_type = typeAtPosition(op.partypes, pos, tem.isvarin)
        checkArgType(args[i], goal_type, op)

      if needs_effect:
        args.insert(0, processSubexpr(tem.useAmbientEffect(), tem, defmap, stack, instrs))
        
      result = Goto(op, args)

    elif name == 'label':
      # already collected
      raise ParseError("internal error: label still in statement list")

    else:
      op = findDef(name, defmap)
      if op is None:
        raise ParseError("no instr for name '%s'" % name)
      
      if op.isTemplate() and not op.done:
        processTem(op, defmap, stack)
        
      needs_effect = checkArity(op, args)
      argoff = 1 if needs_effect else 0
      
      for i in range(0, len(args)):
        args[i] = processSubexpr(args[i], tem, defmap, stack, instrs)
        pos = i + argoff
        goal_type = typeAtPosition(op.partypes, pos, tem.isvarin)
        checkArgType(args[i], goal_type, op)
        
      if needs_effect:
        args.insert(0, processSubexpr(tem.useAmbientEffect(), tem, defmap, stack, instrs))
        
      result = Call(op, args)
    
  else:
    # non-list expression  
    if sexpr in types:
      # type const
      result = TypeConst(types[sexpr])
      
    elif type(sexpr) == int or type(sexpr) == float:
      # numeric const
      result = NumConst(sexpr)
      
    elif sexpr in VarTypes:
      # C++ var
      result = VarRef(sexpr)
      
    else:
      # name reference
      result = tem.makeRef(sexpr)
    
  setCurItem(savecuritem)
  return result

# helper - resolve name to definition or None
#
def findDef(name, defmap):
  return defmap[name] if name in defmap else None
  
# helper - resolve name to local label
#
def findLabel(name, tem):
  if name in tem.labels:
    return tem.labels[name]
  elif tem.parent:
    return findLabel(name, tem.parent)
  else:
    return None    
  
# build return arglist from final body statement
#
def buildRetArgs(tem, defmap):
  retargs = []
  last = tem.body[len(tem.body) - 1]
  if last.expr.kind == 'Call':
    # return call results
    for name in last.names:
      if name in tem.ambeffects: 
        tem.ambeffects.remove(name)
      retargs.append(tem.makeRef(name))
  else:
    # return final stated value 
    retargs.append(last.expr)
    tem.body.pop()
    
  # unused ambient effects: if param, remove 
  # otherwise, return in order created
  #
  # NOTE: for now, disabling effect-stripping
  # on top-level templates (but leaving it in place
  # for ifs). Before enabling for top-levels we'll
  # need to smarten the C++ to support pure templates,
  # and support single-data-value template returns in
  # CG here.
  # 
  while tem.hasAmbientEffect():
    amb = tem.useAmbientEffect()
    if amb in tem.parnames and tem.numParents() > 0:
      ix = tem.parnames.index(amb)
      tem.parnames.remove(amb)
      tem.partypes.pop(ix)
    else:
      retargs.insert(0, tem.makeRef(amb))
      
  return retargs

# finish and check template signature
#
def finishSig(tem, retargs):
  # unused args are still typed as ?
  for i in range(0, len(tem.partypes)): 
    if tem.partypes[i] == infer_type:
      tem.partypes[i] = top_type
  # derive result sig from return statement
  tem.rettypes = [v.type() for v in retargs]

  # can now check well-formedness
  tem.checkSigs()

# add return instr to template body.
#
# All template IR graphs end with an HR_return instruction,
# which gathers the template results (effect and data) as
# inputs.
#
def addReturnStmt(tem, retargs, defmap):
  retstmt = genBindings(tem, Call(defmap['return'], retargs))
  tem.body.append(retstmt)

# process template.
# process body statements, infer/check types
#
def processTem(tem, defmap, stack):
  # print '>>> %s\n' % tem.dump()
  try:
    if tem in stack:
      raise ParseError("recursion not supported")
    stack.append(tem)
    savecuritem = setCurItem(tem)

    # first collect label defs, without processing
    removed = 0
    for i in range(0, len(tem.body)):
      stmt = tem.body[i - removed]
      if type(stmt) == list and len(stmt) > 0 and stmt[0] == 'label':
        if i == 0:
          raise ParseError("first statement in template cannot be a label")
        if len(stmt) < 4:
          raise ParseError("too few items in label definition")
        name, params, body = '@' + stmt[1], stmt[2], stmt[3:]
        localdef = Template(name, params, body, tem)
        if name in tem.labels:
          raise ParseError("duplicate label '%s'" % name)
        tem.labels[name] = localdef
        # remove from template statement list
        tem.body.pop(i - removed)
        removed += 1

    # process body statements
    instrs = []
    for i in range(0, len(tem.body)):
      stmt = tem.body[i]
      bindings = None
      if type(stmt) == list and len(stmt) > 2 and stmt[1] == '=':
        if i == len(tem.body) - 1:
          raise ParseError("last statement in template cannot be a binding")
        # local bindings
        instr = processExpr(stmt[2], tem, defmap, stack, instrs)
        bindings = genBindings(tem, instr,  stmt[0])
      else:
        # unbound statement, local def, etc.
        instr = processExpr(stmt, tem, defmap, stack, instrs)
        if instr is not None:
          # note: generate local bindings
          bindings = genBindings(tem, instr)
      # check for goto
      if bindings.hasGoto() and i < len(tem.body) - 1:
        if bindings.expr.kind == 'Call' and bindings.expr.base.name == 'goto':
          raise ParseError('goto as non-final statement')
        else:
          raise ParseError('goto appears within non-final statement')
      # add  bindings processed from statement
      if bindings is not None:
        instrs.append(bindings)
        
    # swap in processed template body
    tem.body = instrs

    # add return
    retargs = buildRetArgs(tem, defmap)
    finishSig(tem, retargs)
    addReturnStmt(tem, retargs, defmap)
    tem.done = True

    setCurItem(savecuritem)
    stack.pop()
    # print '<<< %s\n' % tem.dump()
    
  except ParseError as e:
    print 'parse error: %s' % e.message()
    sys.exit(1)

# create a map from a list of defs, check for dupes
def toMap(defs):
  m = {}
  for d in defs:
    setCurItem(d)
    if d.name in m:
      raise ParseError("instr '%s' already defined" % d.name)
    m[d.name] = d
  return m

# process list of defs into a finished map.
def process(defs):
  defmap = toMap(defs)
  for d in defmap.values(): 
    if d.isTemplate() and not d.done:
      processTem(d, defmap, [])

