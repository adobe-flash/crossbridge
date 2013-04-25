# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import gdb
import itertools
import re

# just a preliminary hack at this problem. It appears to be busted. I'm not sure if my code is broken
# or if gdb/python is not looking into our instructions correctly. (matz)

def ownerId( defval ) :
    "return the id of the owner of defval, as a string"
    #should assert that val.type is Use or Def
    #maybe mask the type_ field and prefix the id with "c" if it's a control type of "n" otherwise.
    try:
        o = defval['owner_']
        id = o['id']
        return str(id)
    except:
        return 'id threw'


def typeName( defval ) :
    "return the name of the type of a Def, as a string"
    t  = defval['type_']
    n =  t['name']
    return str(n)
    
class DefPrinter:
    "Print the id of the defining instruction as a string"
    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return 'Def: %s owner_ id=%s type_=%s' %( str(self.val.type), ownerId(self.val), typeName(self.val) )
        except:
            return 'DefPrinter.to_string threw'

    def display_hint (self):
        return 'string'


class UsePrinter:
    "Print the id of the defining instruction as a string"
    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            return 'Use: %s owner_ id=%s' %(  str(self.val.type), ownerId(self.val))
        except:
            return 'UsePrinter.to_string threw'

    def display_hint (self):
        return 'string'

class TypePrinter:
    "Print the name of the type"
    def __init__(self, val):
        self.val = val

    def to_string(self):
        try:
            n = self.val['name']
            return 'Type: %s' %( n )
        except:
            return 'TypePrinter.to_string threw'

    def display_hint (self):
        return 'string'


class StmtInfoPrinter:
    "Print a halfmoon statement"
    def __init__(self, val):
        self.val = val

    def to_string(self):
        # Make sure &string works, too.

        ptr = self.val ['kind_']
        return ptr.string #(length = len)

    def display_hint (self):
        return 'string'


# (gdb) print/r *info
# $4 = {<halfmoon::StmtInfo<halfmoon::BinaryStmt>> = {<halfmoon::InstrInfo> = {_vptr.InstrInfo = 0x838b688, kind_ = halfmoon::HR_newfunction, num_ctrl = 1, 
#       num_args = 2, num_defs = 2, ctrl_off = 16, args_off = 32, defs_off = 64}, <No data fields>}, <No data fields>}

# (gdb) print/r *s
# $3 = {<halfmoon::Instr> = {info = 0xb7c533b8, id = -1, next_ = 0x0, prev_ = 0x0}, ctrl = {{def_ = 0xb7c53054, next_ = 0xb7c53438, prev_ = 0xb7c53438, 
#       owner_ = 0xb7c53428}}, args = {{def_ = 0xb7c53358, next_ = 0xb7c53448, prev_ = 0xb7c53448, owner_ = 0xb7c53428}, {def_ = 0xb7c53170, 
#       next_ = 0xb7c53458, prev_ = 0xb7c53458, owner_ = 0xb7c53428}}, values = {{owner_ = 0xb7c53428, type_ = 0x0, uses_ = 0x0}, {owner_ = 0xb7c53428, 
#       type_ = 0x0, uses_ = 0x0}}}
# (gdb) 

class InstrPrinter:
    "Print a halfmoon instruction of some kind"

    def __init__(self, val):
        self.val = val
        self.NAME="InstrPrinter"

    def dbg(self,str):
        print self.NAME, "self.val[", str, "]=", self.val[str]
        
    def to_string(self):
        ia = gdb.parse_and_eval('halfmoon::instr_attrs')
        name = 'bogus'
        try:
            info = self.val['info']
            kind = info['kind_']
            # this thing gets value like 0xdeadbeef "string"
            # we just want the thing in quotes (split on " and take element1)
            namexx = str(ia[kind]['name'])
            name = namexx.split('"')[1]
            return '%s: %s (id=%s)' % (str(self.val.type), name.upper(), self.val['id'])
        except:
            return 'oops, InstrPrinter.to_string threw'

    def display_hint (self):
        return 'string'


#
# this method is broken. (or perhaps gdb/python is)
# it seems to always print the same oid for every Def and use.
# I dunno why.
#
class BinaryStmtPrinter:
    "Print a BinaryStmt halfmoon instruction"

    def __init__(self, val):
        self.val = val

    def dbg(self,str):
        print "hello from dbg", str

    def oid(self, usedef) :
        i = usedef['owner_'] # the instruction defining 
        oid = i['id']        # its id
        return str(oid)
        
    def to_string(self):
        info = self.val['info']
        opcode = str(info['kind_'] )
        buf = opcode + " "

        values = self.val['values']
        buf += 'c' + ownerId(values[0]) + ', '
        buf += 'n' + ownerId(values[1]) + ' = '

        args = self.val['args']
        buf += 'n' + ownerId(args[0]['def_']) + ', '
        buf += 'n' + ownerId(args[1]['def_']) + ', '

        ctrl = self.val['ctrl']
        buf += 'c' + ownerId(ctrl[0]['def_']) 
        return buf
    
    def display_hint (self):
        return 'string'


def register_halfmoon_printers (obj):
    "Register halfmoon pretty-printers with objfile Obj."
    #print "hello from register_halfmoon_printers"
    if obj == None:
        obj = gdb

    obj.pretty_printers.append (lookup_function)

#left from std:
    
class StdPointerPrinter:
    "Print a smart pointer of some kind"

    def __init__ (self, typename, val):
        self.typename = typename
        self.val = val

    def to_string (self):
        if self.val['_M_refcount']['_M_pi'] == 0:
            return '%s (empty) %s' % (self.typename, self.val['_M_ptr'])
        return '%s (count %d) %s' % (self.typename,
                                     self.val['_M_refcount']['_M_pi']['_M_use_count'],
                                     self.val['_M_ptr'])

def lookup_function (val):
    "Look-up and return a pretty-printer that can print val."

    # Get the type.
    #, "\"", gdb.TYPE_CODE_REF, type.code
    type = val.type

    #print "lookup_function:",val.type
    
    # If it points to a reference, get the reference.
    if type.code == gdb.TYPE_CODE_REF:
        #print "lookup_function: is gdb.TYPE_CODE_REF:"
        type = type.target ()

    # Get the unqualified type, stripped of typedefs.
    type = type.unqualified ().strip_typedefs ()

    # Get the type name.    
    typename = type.tag
    #print 'typename=',typename

    if typename == None:
        #yeah but pointers don't seem to answer for tag (matz)
        if type.code == gdb.TYPE_CODE_PTR :
            #print 'is  gdb.TYPE_CODE_PTR', 'str(type)',str(type)
            #if doesn't end with * chicken out..
            if not re.compile("\\*$").search(str(type)) :
                print "type name doesn't end with *, so chicken out"
                return None
            #otherwise just pass the name of the type on. (lookout below)
            typename = str(type)
            #print 'typename=',typename
        else:
            return None #if no tag and not a pointer, chicken out.

    # Iterate over local dictionary of types to determine
    # if a printer is registered for that type.  Return an
    # instantiation of the printer if found.
    for function in pretty_printers_dict:
        if function.search (typename):
            #print "have pretty printer for", typename
            return pretty_printers_dict[function] (val)
        
    # Cannot find a pretty printer.  Return None.
    # TODO: figure out how to go up the type hierarchy to find a pretty printer for a base class if we cant find it for the actual instance.
    return None

def build_halfmoon_dictionary ():

    pretty_printers_dict[re.compile('^halfmoon::Use \*')] = lambda val: UsePrinter(val)
    pretty_printers_dict[re.compile('^halfmoon::Def \*')] = lambda val: DefPrinter(val)
    pretty_printers_dict[re.compile('^halfmoon::Type \*')] = lambda val: TypePrinter(val)
    pretty_printers_dict[re.compile('^const halfmoon::Type \*')] = lambda val: TypePrinter(val)
    pretty_printers_dict[re.compile('^halfmoon::Def$')] = lambda val: DefPrinter(val)
    pretty_printers_dict[re.compile('^halfmoon::Use$')] = lambda val: UsePrinter(val)

    pretty_printers_dict[re.compile('^halfmoon::Instr \*')] = lambda val: InstrPrinter(val)
    pretty_printers_dict[re.compile('^halfmoon::\\S*Instr \*')] = lambda val: InstrPrinter(val)

    # pretty_printers_dict[re.compile('^halfmoon::StmtInfo<.*>$')] = lambda val: StmtInfoPrinter(val)
    
    #before we write specialized versions..
    pretty_printers_dict[re.compile('^halfmoon::BinaryStmt')] = lambda val: BinaryStmtPrinter(val)
    # pretty_printers_dict[re.compile('^halfmoon::BlockInstr$')] = lambda val: InstrPrinter(val)
    # pretty_printers_dict[re.compile('^halfmoon::JumpInstr$')] = lambda val: InstrPrinter(val)
    # pretty_printers_dict[re.compile('^halfmoon::PhiInstr$')] = lambda val: InstrPrinter(val)
    # pretty_printers_dict[re.compile('^halfmoon::BinaryStmt$')] = lambda val: InstrPrinter(val)
    # pretty_printers_dict[re.compile('^halfmoon::BinaryInstr$')] = lambda val: InstrPrinter(val)

    # pretty_printers_dict[re.compile('^halfmoon::Instr$')] = lambda val: InstrPrinter(val)
    # print '^halfmoon::Instr\\s*\\*$'
    # pretty_printers_dict[re.compile('^halfmoon::\\S*Instr\\s*\\*\\s*$')] = lambda val: InstrPrinter(val)
    # pretty_printers_dict[re.compile('^const\\s*halfmoon::\\S*Instr\\s*\\*\\s*$')] = lambda val: InstrPrinter(val)
    # pretty_printers_dict[re.compile('^halfmoon::\\S*Instr\\s*\\*\\s*$')] = lambda val: InstrPrinter(val)

    #would be useful to have pretty printer for Instr* (eclipse details in expression and variable windows)
    #pretty_printers_dict[re.compile('^halfmoon::Instr\ \*$')] = lambda val: InstrPrinter(val)

#
# this gets called when package is first loaded at .gbdinit time (matz)
#
    
pretty_printers_dict = {}
#print "about to call build_halfmoon_dictionary"
build_halfmoon_dictionary ()
