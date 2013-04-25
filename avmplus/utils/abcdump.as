/* -*- indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package abcdump
{
    import flash.utils.ByteArray
    import avmplus.System
    import avmplus.File

    include "abc-constants.as"

    var usage = [
    "Displays the contents of abc or swf files",
    "",
    "Usage:",
    "",
    "    abcdump [options] file ...",
    "",
    "Each file can be an ABC or SWF/SWC format file",
    "",
    "Options:",
    "",
    " -a   Extract the ABC blocks from the SWF/SWC, but do not",
    "      otherwise display their contents.  The file names are",
    "      of the form file<n>.abc where \"file\" is the name",
    "      of the input file minus the .swf/.swc extension;",
    "      and <n> is omitted if it is 0.",
    "",
    " -i   Print information about the ABC, but do not dump the byte code.",
    "",
    " -abs Print the bytecode, but no information about the ABC",
    " -api Print the public API exposed by this abc/swf",
    " -mdversions Use in conjunction with -api when the abc/swf uses old-style versioning",
    " -pools Print out the contents of the constant pools",
    " --decompress-only Write out a decompressed version of the swc and exit"
    ].join("\n");

    const TAB = "  "

    var totalSize:int
    var opSizes:Array = new Array(256)
    var opCounts:Array = new Array(256)

    function dumpPrint(s) {
        if (doExtractAbs)
            print(s);
    }

    function infoPrint(s) {
        if (doExtractInfo)
            print((doExtractAbs ? "// " : "") + s)
    }

    function toStringNull(x) {
        return (x == null) ? "<null>" : x.toString()
    }

    // keep track of old-style versioning metadata in a global stack
    var currentVersionMetadata:Array = new Array()
    var apiVersionNames:Array = ["9", "air1", "10", "air1.5", "air1.5.1",
                              "10.0.32", "air1.5.2", "10.1", "air2",
                              "fpsys", "airsys"]

    function getVersionMetadata():Array {
        if (currentVersionMetadata.length > 0)
            return currentVersionMetadata[currentVersionMetadata.length - 1]

        return []
    }

    function pushVersionMetadata(md:Array):Boolean {
        if(md == null)
            return false

        var vers:Array = []
        for each(var m in md) {
            if(m.name == "Version")
                for each(var t:Tuple in m.tuples)
                    vers.push(t.value)
            if(m.name == "API")
                for each(var t:Tuple in m.tuples)
                    vers.push(currentVersionMetadata[int(t.value) - 660])
        }

        if(vers.length > 0) {
            currentVersionMetadata.push(vers)
            return true
        }
        return false
    }

    function popVersionMetadata(cond:Boolean):void {
        if(cond)
            currentVersionMetadata.pop()
    }

    class ABCNamespace
    {
        public var kind:int
        public var uri:String
        public var apiVersions:Array = new Array()

        public function ABCNamespace(ns:String, k:int = 0x08 /* CONSTANT_Namespace */) {
            kind = k
            uri = ns == null ? null : stripVersioningChars(ns)
        }

        public function clone():ABCNamespace {
            var ns:ABCNamespace = new ABCNamespace(uri, kind)
            ns.apiVersions = apiVersions.concat()
            return ns
        }

        function stripVersioningChars(s:String) {
            var c:int = s.charCodeAt(s.length-1)
            if(c > 0xE000) {
                if(c > 0xE294)
                   apiVersions.push(apiVersionNames[c - 0xE294])
                return stripVersioningChars(s.slice(0,s.length-1))
            }
            return s
        }

        public function toString(useMD:Boolean = false):String {
            var vers:Array = useMD ? getVersionMetadata() : apiVersions
            return (uri == null ? "" : uri) + (vers.length > 0 && doDumpAPI ? ("[api: " + vers.join(',') + "]") : "") // + ("(" + constantKinds[kind] + ")")
        }

        public function isHidden():Boolean {
            return (kind == CONSTANT_PrivateNs || kind == CONSTANT_PackageInternalNs)
        }
    }

    class QualifiedName
    {
        public var ns:ABCNamespace
        public var localname:String

        public function QualifiedName(n:ABCNamespace, ln:String) {
            ns = n
            localname = ln
        }

        public function toString(useMD:Boolean = false):String {
            var nsstr = (ns == null ? "" : ns.toString(useMD))
            return (nsstr == "" ? "" : nsstr + "::") + localname
        }

        public function isHidden():Boolean {
            return ns.isHidden()
        }
    }

    class Multiname
    {
        var nsset:Array
        var name:String
        function Multiname(nsset:Array, name:String)
        {
            this.nsset = nsset
            this.name = name
        }

        public function toString(useMD:Boolean = false)
        {
            if (nsset.length == 1)
                return (new QualifiedName(nsset[0], name).toString(useMD))
            else
                return '{' + joinNsset(nsset, useMD) + '}::' + toStringNull(name)
        }

        function joinNsset(nsset:Array, useMD:Boolean):String {
            var s:String = ""
            for each (var ns:ABCNamespace in nsset) {
                s += (ns.toString(useMD) + ", ")
            }
            return s
        }

        public function isHidden():Boolean {
            for each(var ns in nsset)
                if (ns.isHidden())
                    return true
            return false
        }

        public static function createMultiname(nsset:Array, name:String, flatten:Boolean) {
            if(flatten) {
                var cur:ABCNamespace = nsset[0].clone()
                for(var i:int=1; i<nsset.length; i++) {
                    var ns:ABCNamespace = nsset[i]
                    if (cur.uri == ns.uri) {
                        for each(var v:String in ns.apiVersions)
                            if(cur.apiVersions.indexOf(v) == -1)
                                cur.apiVersions.push(v)
                    } else {
                        cur = null
                        break
                    }
                }

                if(cur)
                    return new QualifiedName(cur, name)
            }
            return new Multiname(nsset, name)
        }
    }

    class TypeName
    {
        var name;
        var types:Array;
        function TypeName(name, types)
        {
            this.name = name;
            this.types = types;
        }

        public function toString()
        {
            var s : String = name
            s += ".<"
            for( var i = 0; i < types.length; ++i )
                s += types[i] != null ? types[i].toString() : "*" + " ";
            s += ">"
            return s;
        }
    }

    class Tuple
    {
        public var key:String
        public var value:String

        function Tuple(k:String, v:String) {
            key = k
            value = v
        }

        public function toString() {
            return key + "=" + value
        }
    }

    class MetaData
    {
        public var name:String
        public var tuples:Vector.<Tuple> = new Vector.<Tuple>()

        public function toString():String
        {
            var last:String
            var s:String = last = '['+name+'('
            for each (var t:Tuple in tuples)
                s = (last = s + t.key + "=" + '"' + t.value + '"') + ','
                return last + ')]'
                }

        public function addPair(k:String, v:String) {
            tuples.push(new Tuple(k, v))
        }
    }

    class MemberInfo
    {
        var id:int
        var kind:int
        var name
        var metadata:Array
    }

    dynamic class LabelInfo
    {
        var count:int
        function labelFor (target:int):String
        {
            if (target in this)
                return this[target]
            return this[target] = "L" + (++count)
        }
    }

    class ExceptionInfo
    {
        var from:int
        var to:int
        var target:int
        var type
        var name
    }

    class ByteArrayExhaustedError extends Error
    {
        function ByteArrayExhaustedError(e) { super(e); }
    }

    class MethodInfo extends MemberInfo
    {
        var method_id:int
        var dumped:Boolean
        var flags:int
        var debugName
        var paramTypes
        var optionalValues
        var returnType
        var local_count:int
        var max_scope:int
        var max_stack:int
        var code_offset:uint
        var code_length:uint
        var code:ByteArray
        var exceptions // ExceptionInfo[]
        var activation:Traits

        public function toString():String
        {
            return format()
        }

        public function format():String
        {
            var name = this.name ? (this.name is String ? this.name : this.name.toString(useMetadataVersions)) : "function"

            return name + "(" + paramTypes + "):" + returnType + (doDumpAPI ? "" : "\t/* disp_id=" + id + " method_id=" + method_id + " */")
        }

        function dump(abc:Abc, indent:String, attr:String="")
        {
            if(doDumpAPI && name && (name is String || name.isHidden()))
                return;

            dumped = true
            dumpPrint("")

            if (metadata && !doDumpAPI) {
                for each (var md in metadata)
                    dumpPrint(indent+md)
            }

            var mdpushed:Boolean = pushVersionMetadata(metadata)

            var s:String = ""
            if (flags & NATIVE && !doDumpAPI)
                s = "native "
            s += traitKinds[kind] + " "

            dumpPrint(indent+attr+s+format())
            if (code && !doDumpAPI)
            {
                dumpPrint(indent+"{")
                var oldindent = indent
                indent += TAB
                if (flags & NEED_ACTIVATION) {
                    dumpPrint(indent+"activation {")
                    activation.dump(abc, indent+TAB, "")
                    dumpPrint(indent+"}")
                }
                dumpPrint(indent+"// local_count="+local_count+
                          " max_scope=" + max_scope +
                          " max_stack=" + max_stack +
                          " framesize=" + (local_count + max_scope + max_stack) +
                          " code_len=" + code.length +
                          " code_offset=" + code_offset)
                code.position = 0
                var labels:LabelInfo = new LabelInfo()
                while (code.bytesAvailable > 0)
                {
                    var start:int = code.position
                    var s = indent + start
                    while (s.length < 12) s += ' ';
                    var opcode = code_readUnsignedByte()

                    if (opcode == OP_label || ((code.position-1) in labels)) {
                        dumpPrint(indent)
                        dumpPrint(indent + labels.labelFor(code.position-1) + ": ")
                    }

                    s += opNames[opcode]
                    s += opNames[opcode].length < 8 ? "\t\t" : "\t"

                    switch(opcode)
                    {
                        case OP_debugfile:
                        case OP_pushstring:
                            s += '"' + abc.strings[readU32()].replace(/\n/g,"\\n").replace(/\t/g,"\\t") + '"'
                            break
                        case OP_pushnamespace:
                            s += abc.namespaces[readU32()]
                            break
                        case OP_pushint:
                            var i:int = abc.ints[readU32()]
                            s += i + "\t// 0x" + i.toString(16)
                            break
                        case OP_pushuint:
                            var u:uint = abc.uints[readU32()]
                            s += u + "\t// 0x" + u.toString(16)
                            break;
                        case OP_pushdouble:
                            s += abc.doubles[readU32()]
                            break;
                        case OP_pushfloat:
                            if( abc.floatSupport)
                                s += abc.floats[readU32()];
                            break;
                        case OP_pushfloat4:
                            if( abc.floatSupport)
                                s += abc.float4s[readU32()];
                            break;
                        case OP_getsuper:
                        case OP_setsuper:
                        case OP_getproperty:
                        case OP_initproperty:
                        case OP_setproperty:
                        case OP_getlex:
                        case OP_findpropstrict:
                        case OP_findproperty:
                        case OP_finddef:
                        case OP_deleteproperty:
                        case OP_istype:
                        case OP_coerce:
                        case OP_astype:
                        case OP_getdescendants:
                            s += abc.names[readU32()]
                            break;
                        case OP_constructprop:
                        case OP_callproperty:
                        case OP_callproplex:
                        case OP_callsuper:
                        case OP_callsupervoid:
                        case OP_callpropvoid:
                            s += abc.names[readU32()]
                            s += " (" + readU32() + ")"
                            break;
                        case OP_newfunction: {
                            var method_id = readU32()
                            s += abc.methods[method_id]
                            break;
                        }
                        case OP_callstatic:
                            s += abc.methods[readU32()]
                            s += " (" + readU32() + ")"
                            break;
                        case OP_newclass:
                            s += abc.instances[readU32()]
                            break;
                        case OP_lookupswitch:
                            var pos = code.position-1;
                            var target = pos + readS24()
                            var maxindex = readU32()
                            s += "default:" + labels.labelFor(target) // target + "("+(target-pos)+")"
                            s += " maxcase:" + maxindex
                            for (var i:int=0; i <= maxindex; i++) {
                                target = pos + readS24();
                                s += " " + labels.labelFor(target) // target + "("+(target-pos)+")"
                            }
                            break;
                        case OP_jump:
                        case OP_iftrue:     case OP_iffalse:
                        case OP_ifeq:       case OP_ifne:
                        case OP_ifge:       case OP_ifnge:
                        case OP_ifgt:       case OP_ifngt:
                        case OP_ifle:       case OP_ifnle:
                        case OP_iflt:       case OP_ifnlt:
                        case OP_ifstricteq: case OP_ifstrictne:
                            var offset = readS24()
                            var target = code.position+offset
                            //s += target + " ("+offset+")"
                            s += labels.labelFor(target)
                            if (!((code.position) in labels))
                                s += "\n"
                            break;
                        case OP_inclocal:
                        case OP_declocal:
                        case OP_inclocal_i:
                        case OP_declocal_i:
                        case OP_getlocal:
                        case OP_kill:
                        case OP_setlocal:
                        case OP_bkptline:
                        case OP_debugline:
                        case OP_getglobalslot:
                        case OP_getslot:
                        case OP_setglobalslot:
                        case OP_setslot:
                        case OP_pushshort:
                        case OP_newcatch:
                        case OP_getouterscope:
                            s += readU32()
                            break
                        case OP_debug:
                            s += code_readUnsignedByte()
                            s += " " + readU32()
                            s += " " + code_readUnsignedByte()
                            s += " " + readU32()
                            break;
                        case OP_newobject:
                            s += "{" + readU32() + "}"
                            break;
                        case OP_newarray:
                            s += "[" + readU32() + "]"
                            break;
                        case OP_call:
                        case OP_construct:
                        case OP_constructsuper:
                        case OP_applytype:
                            s += "(" + readU32() + ")"
                            break;
                        case OP_pushbyte:
                        case OP_getscopeobject:
                            s += code_readByte()
                            break;
                        case OP_hasnext2:
                            s += readU32() + " " + readU32()
                        default:
                            /*if (opNames[opcode] == ("0x"+opcode.toString(16).toUpperCase()))
                                s += " UNKNOWN OPCODE"*/
                            break;
                    }
                    var size:int = code.position - start
                    totalSize += size
                    opSizes[opcode] = int(opSizes[opcode]) + size
                    opCounts[opcode] = int(opCounts[opcode]) + 1
                    dumpPrint(s)
                }
                if (exceptions) {
                    for each (var ex in exceptions)
                        dumpPrint(indent + "// handler [" + ex.from + ", " + ex.to + "] -> " + ex.target +
                            (ex.name ? (" " + ex.name + ":" + ex.type) : (" :" + ex.type)));
                }
                dumpPrint(oldindent+"}\n")
            }

            popVersionMetadata(mdpushed)
        }

        function readU32():int
        {
            var result:int = code_readUnsignedByte();
            if (!(result & 0x00000080))
                return result;
            result = result & 0x0000007f | code_readUnsignedByte()<<7;
            if (!(result & 0x00004000))
                return result;
            result = result & 0x00003fff | code_readUnsignedByte()<<14;
            if (!(result & 0x00200000))
                return result;
            result = result & 0x001fffff | code_readUnsignedByte()<<21;
            if (!(result & 0x10000000))
                return result;
            return   result & 0x0fffffff | code_readUnsignedByte()<<28;
        }

        function readS24():int
        {
            var b:int = code_readUnsignedByte()
            b |= code_readUnsignedByte()<<8
            b |= code_readByte()<<16
            return b
        }

        function code_readUnsignedByte():int
        {
            if (code.bytesAvailable > 0)
                return code.readUnsignedByte();
            else
                throw new ByteArrayExhaustedError("code exhausted");
        }

        function code_readByte():int
        {
            if (code.bytesAvailable > 0)
                return code.readByte();
            else
                throw new ByteArrayExhaustedError("code exhausted");
        }
    }

    class SlotInfo extends MemberInfo
    {
        var type
        var value
        public function format():String
        {
            return traitKinds[kind] + " " + name.toString(useMetadataVersions) + ":" + type +
                (value !== undefined ? (" = " + (value is String ? ('"'+value+'"') : value)) : "") +
                (doDumpAPI ? "" : "\t/* slot_id " + id + " */")
        }
        function dump(abc:Abc, indent:String, attr:String="")
        {
            if(doDumpAPI && name.isHidden())
                return

            var mdpushed:Boolean = pushVersionMetadata(metadata)

            if (kind == TRAIT_Const || kind == TRAIT_Slot)
            {
                if (metadata && !doDumpAPI) {
                    for each (var md in metadata)
                        dumpPrint(indent+md)
                }
                dumpPrint(indent+attr+format())
                popVersionMetadata(mdpushed)
                return
            }

            // else, class
            var ct:Traits = value
            var it:Traits = ct.itraits
            dumpPrint('')
            if (metadata && !doDumpAPI) {
                for each (var md in metadata)
                    dumpPrint(indent+md)
            }
            var def:String;
            if (it.flags & CLASS_FLAG_interface)
                def = "interface"
            else {
                def = "class";
                if (!(it.flags & CLASS_FLAG_sealed))
                    def = "dynamic " + def;
                if (it.flags & CLASS_FLAG_final)
                    def = "final " + def;
            }

            dumpPrint(indent+attr+def+" "+name.toString(useMetadataVersions)+" extends "+it.base)
            var oldindent = indent
            indent += TAB
            if (it.interfaces.length > 0)
                dumpPrint(indent+"implements "+it.interfaces)

            if(doDumpAPI) {
                var prefix:String = indent+attr+def+" "+name.toString(useMetadataVersions)+" "
                it.init.dump(abc,prefix)
                it.dump(abc,indent,prefix)
                ct.dump(abc,indent,prefix + "static ")
                ct.init.dump(abc,indent,prefix + "static ")
            } else {
                dumpPrint(oldindent+"{")
                it.init.dump(abc,indent)
                it.dump(abc,indent)
                ct.dump(abc,indent,"static ")
                ct.init.dump(abc,indent,"static ")
                dumpPrint(oldindent+"}\n")
            }

            popVersionMetadata(mdpushed)
        }
    }

    class Traits
    {
        var name
        var init:MethodInfo
        var itraits:Traits
        var base
        var flags:int
        var protectedNs:ABCNamespace
        const interfaces:Array = []
        const names:Object = {}
        const slots:Array = []
        const methods:Array = []
        const members:Array = []

        public function toString():String
        {
            return String(name)
        }

        public function dump(abc:Abc, indent:String, attr:String="")
        {
            for each (var m in members)
                m.dump(abc, indent, attr)
        }
    }

    class Abc
    {
        private var data:ByteArray

        var major:int
        var minor:int

        var ints:Array
        var uints:Array
        var doubles:Array
        var floats:Array
        var float4s:Array
        var strings:Array
        var namespaces:Array
        var nssets:Array
        var names:Array

        var defaults:Array = new Array(constantKinds.length)

        var methods:Array
        var instances:Array
        var classes:Array
        var scripts:Array
        var metadata:Array

        var publicNs = new ABCNamespace("")
        var anyNs = new ABCNamespace("*")

        var magic:int
        var floatSupport:Boolean;

        function Abc(data:ByteArray)
        {
            data.position = 0
            this.data = data
            magic = data.readInt()

            infoPrint("magic " + magic.toString(16))

            floatSupport = true;
            switch (magic) {
            case (46<<16|14):
            case (46<<16|15):
            case (46<<16|16):
            case (47<<16|12):
            case (47<<16|13):
            case (47<<16|14):
            case (47<<16|15):
                floatSupport = false; // No float support prior to Cyrill
            case (47<<16|16):
            case (47<<16|17):
            case (47<<16|18):
            case (47<<16|19):
            case (47<<16|20):
                break;
            default:
                throw new Error("not an abc file.  magic=" + magic.toString(16))
            }

            parseCpool()

            defaults[CONSTANT_Utf8] = strings
            defaults[CONSTANT_Int] = ints
            defaults[CONSTANT_UInt] = uints
            defaults[CONSTANT_Double] = doubles
            defaults[CONSTANT_Int] = ints
            defaults[CONSTANT_False] = { 10:false }
            defaults[CONSTANT_True] = { 11:true }
            defaults[CONSTANT_Namespace] = namespaces
            defaults[CONSTANT_PrivateNs] = namespaces
            defaults[CONSTANT_PackageNs] = namespaces
            defaults[CONSTANT_PackageInternalNs] = namespaces
            defaults[CONSTANT_ProtectedNs] = namespaces
            defaults[CONSTANT_StaticProtectedNs] = namespaces
            defaults[CONSTANT_StaticProtectedNs2] = namespaces
            defaults[CONSTANT_Null] = { 12: null }
            if(floatSupport){
               defaults[CONSTANT_Float] = floats
               defaults[CONSTANT_Float4] = float4s
            }

            parseMethodInfos()
            parseMetadataInfos()
            parseInstanceInfos()
            parseClassInfos()
            parseScriptInfos()
            parseMethodBodies()

            if (doExtractAbc==true)
                File.writeByteArray(nextAbcFname(), data);
        }

        function readU32():int
        {
            var result:int = data.readUnsignedByte();
            if (!(result & 0x00000080))
                return result;
            result = result & 0x0000007f | data.readUnsignedByte()<<7;
            if (!(result & 0x00004000))
                return result;
            result = result & 0x00003fff | data.readUnsignedByte()<<14;
            if (!(result & 0x00200000))
                return result;
            result = result & 0x001fffff | data.readUnsignedByte()<<21;
            if (!(result & 0x10000000))
                return result;
            return   result & 0x0fffffff | data.readUnsignedByte()<<28;
        }

        function classFromU32()
        {
            return classes[readU32()]
        }

        function doubleFromU32()
        {
            return doubles[readU32()]
        }

        function intFromU32()
        {
            return ints[readU32()]
        }

        function uintFromU32()
        {
            return uints[readU32()]
        }

        function instanceFromU32()
        {
            return instances[readU32()]
        }

        function metadataFromU32()
        {
            return metadata[readU32()]
        }

        function methodFromU32()
        {
            return methods[readU32()]
        }

        function nameFromU32()
        {
            return names[readU32()]
        }

        function namespaceFromU32()
        {
            return namespaces[readU32()]
        }

        function nssetFromU32()
        {
            return nssets[readU32()]
        }

        function stringFromU32()
        {
            return strings[readU32()]
        }

        function dumpPool(name:String, pool:Array)
        {
            if(!doDumpPools)
                return

            for(var i:int = 0; i<pool.length; i++)
                infoPrint(name + "[" + i + "] = " + pool[i])
        }

        function padString(s:String, l:uint)
        {
            if(s.length < l)
                return padString(s + " ", l)
            return s
        }

        function parseCpool()
        {
            var i:int, j:int
            var n:int
            var kind:int

            var start:int = data.position

            // ints
            n = readU32()
            ints = [0]
            for (i=1; i < n; i++)
                ints[i] = readU32()
            dumpPool("int", ints)

            // uints
            n = readU32()
            uints = [0]
            for (i=1; i < n; i++)
                uints[i] = uint(readU32())
            dumpPool("uint", uints)

            // doubles
            n = readU32()
            doubles = [NaN]
            for (i=1; i < n; i++)
                doubles[i] = data.readDouble()
            dumpPool("double", doubles)

            infoPrint("Cpool numbers size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
            start = data.position

            // floats
            if( floatSupport) {
               n = readU32()
               floats = [float.NaN]
               for (i=1; i < n; i++)
                   floats[i] = float(data.readFloat())
               dumpPool("float", floats)

               infoPrint("Cpool floats size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
               start = data.position
               // float4
               n = readU32()
               var f4:* = float4(float.NaN,float.NaN,float.NaN,float.NaN)
               float4s = [f4]
               for (i=1; i < n; i++){
                   float4s[i] = new float4(data.readFloat(),data.readFloat(),data.readFloat(),data.readFloat());
               }
               dumpPool("float4", float4s)

               infoPrint("Cpool float4s size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
               start = data.position
            }

            // strings
            n = readU32()
            strings = [""]
            for (i=1; i < n; i++) {
                var len = readU32();
                var posPreRead = data.position;
                strings[i] = data.readUTFBytes(len);
                // Bugzilla 723448: ByteArray.readUTFBytes deliberately
                // skips an initial UTF8 BOM in the input.  Compensate
                // for this here.
                if (len >= 3 && (data[posPreRead+0] == 0xEF &&
                                 data[posPreRead+1] == 0xBB &&
                                 data[posPreRead+2] == 0xBF)) {
                    // First, double-check that readUTFBytes is still
                    // compensating for UTF8 BOM.  (Presumed rare and
                    // thus not worth caching or precomputing.)
                    var tempByteArray = new ByteArray();
                    tempByteArray.writeUTFBytes('\ufeff');
                    tempByteArray.position = 0;
                    var s = tempByteArray.readUTFBytes(tempByteArray.length);
                    if (s == "") {
                        strings[i] = '\ufeff'+strings[i];
                    }
                }
            }
            dumpPool("string", strings)

            infoPrint("Cpool strings count "+ n +" size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
            start = data.position

            // namespaces
            n = readU32()
            namespaces = [publicNs]
            var nskind = 0;
            for (i=1; i < n; i++) {
                switch (nskind = data.readByte())
                {
                case CONSTANT_Namespace:
                case CONSTANT_PackageNs:
                case CONSTANT_PackageInternalNs:
                case CONSTANT_ProtectedNs:
                case CONSTANT_StaticProtectedNs:
                case CONSTANT_StaticProtectedNs2:
                {
                    namespaces[i] = new ABCNamespace(stringFromU32(), nskind)
                    break;
                }
                case CONSTANT_PrivateNs:
                    readU32();
                    namespaces[i] = new ABCNamespace("private", nskind)
                    break;
                }
                if(doDumpPools)
                    infoPrint("namespace[" + i + "] = " + padString(constantKinds[nskind], 20) + ": " + namespaces[i].uri)
            }

            infoPrint("Cpool namespaces count "+ n +" size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
            start = data.position

            // namespace sets
            n = readU32()
            nssets = [null]
            for (i=1; i < n; i++)
            {
                var count:int = readU32()
                var nsset = nssets[i] = []
                var nsids = []
                for (j=0; j < count; j++) {
                    var nsid = readU32()
                    nsids.push(nsid)
                    nsset[j] = namespaces[nsid]
                }
                if(doDumpPools)
                    infoPrint("nsset[" + i + "] = {" + nsids.join(", ") + "}")
            }

            infoPrint("Cpool nssets count "+ n +" size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
            start = data.position

            // multinames
            n = readU32()
            names = [null]
            namespaces[0] = anyNs
            strings[0] = "*" // any name
            for (i=1; i < n; i++) {
                var nametype = data.readByte()
                switch (nametype)
                {
                case CONSTANT_Qname:
                case CONSTANT_QnameA:
                    names[i] = new QualifiedName(namespaceFromU32(), stringFromU32())
                    break;

                case CONSTANT_RTQname:
                case CONSTANT_RTQnameA:
                    names[i] = new QualifiedName(null, stringFromU32())
                    break;

                case CONSTANT_RTQnameL:
                case CONSTANT_RTQnameLA:
                    names[i] = null
                    break;

                case CONSTANT_NameL:
                case CONSTANT_NameLA:
                    names[i] = new QualifiedName(publicNs, null)
                    break;

                case CONSTANT_Multiname:
                case CONSTANT_MultinameA:
                    var name = stringFromU32()
                    names[i] = Multiname.createMultiname(nssetFromU32(), name, doDumpAPI)
                    break;

                case CONSTANT_MultinameL:
                case CONSTANT_MultinameLA:
                    names[i] = Multiname.createMultiname(nssetFromU32(), null, doDumpAPI)
                    break;

                case CONSTANT_TypeName:
                    var name = nameFromU32();
                    var count = readU32();
                    var types = [];
                    for( var t=0; t < count; ++t )
                        types.push(nameFromU32());
                    names[i] = new TypeName(name, types);
                    break;

                default:
                    throw new Error("invalid kind " + data[data.position-1])
                }
                if(doDumpPools)
                    infoPrint("name[" + i + "] = " + padString(constantKinds[nametype], 12) + ": " + names[i])
            }

            infoPrint("Cpool names count "+ n +" size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
            start = data.position

            namespaces[0] = publicNs
            strings[0] = "*"
        }

        function parseMethodInfos()
        {
            var start:int = data.position
            names[0] = new QualifiedName(publicNs, "*")
            var method_count:int = readU32()
            methods = []
            for (var i:int=0; i < method_count; i++)
            {
                var m = methods[i] = new MethodInfo()
                m.method_id = i
                var param_count:int = readU32()
                m.returnType = nameFromU32()
                m.paramTypes = []
                for (var j:int=0; j < param_count; j++)
                    m.paramTypes[j] = nameFromU32()
                m.debugName = stringFromU32()
                m.flags = data.readByte()
                if (m.flags & HAS_OPTIONAL)
                {
                    // has_optional
                    var optional_count:int = readU32();
                    m.optionalValues = []
                    for( var k:int = param_count-optional_count; k < param_count; ++k)
                    {
                        var index = readU32()    // optional value index
                        var kind:int = data.readByte() // kind byte for each default value
                        if (index == 0)
                        {
                            // kind is ignored, default value is based on type
                            m.optionalValues[k] = undefined
                        }
                        else
                        {
                            if (!defaults[kind])
                                print("ERROR kind="+kind+" method_id " + i)
                            else
                                m.optionalValues[k] = defaults[kind][index]
                        }
                    }
                }
                if (m.flags & HAS_ParamNames)
                {
                    // has_paramnames
                    for( var k:int = 0; k < param_count; ++k)
                    {
                        readU32();
                    }
                }
            }
            infoPrint("MethodInfo count " +method_count+ " size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
        }

        function parseMetadataInfos()
        {
            var start:int = data.position
            var count:int = readU32()
            metadata = []
            for (var i:int=0; i < count; i++)
            {
                // MetadataInfo
                var m = metadata[i] = new MetaData()
                m.name = stringFromU32();
                var values_count:int = readU32();
                var names:Array = []
                var keys:Array = []
                var values:Array = []

                for(var q:int = 0; q < values_count; ++q)
                    keys[q] = stringFromU32()
                for(var q:int = 0; q < values_count; ++q)
                    values[q] = stringFromU32()

                for(var q:int = 0; q < values_count; ++q)
                    m.addPair(keys[q], values[q])

                if(doDumpPools)
                    infoPrint("metadata[" + i + "] = {" + m.tuples.join(", ") + "}")
            }
            infoPrint("MetadataInfo count " +values_count+ " size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
        }

        function parseInstanceInfos()
        {
            var start:int = data.position
            var count:int = readU32()
            instances = []
            for (var i:int=0; i < count; i++)
            {
                var t = instances[i] = new Traits()
                t.name = nameFromU32()
                t.base = nameFromU32()
                t.flags = data.readByte()
                if (t.flags & 8)
                    t.protectedNs = namespaceFromU32()
                var interface_count = readU32()
                for (var j:int=0; j < interface_count; j++)
                    t.interfaces[j] = nameFromU32()
                var m = t.init = methodFromU32()
                m.name = t.name
                m.kind = TRAIT_Method
                m.id = -1
                parseTraits(t)
            }
            infoPrint("InstanceInfo count " + count + " size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
        }

        function parseTraits(t:Traits)
        {
            var namecount = readU32()
            for (var i:int=0; i < namecount; i++)
            {
                var name = nameFromU32()
                var tag = data.readByte()
                var kind = tag & 0xf
                var member
                switch(kind) {
                case TRAIT_Slot:
                case TRAIT_Const:
                case TRAIT_Class:
                    var slot = member = new SlotInfo()
                    slot.id = readU32()
                    t.slots[slot.id] = slot
                    if (kind==TRAIT_Slot || kind==TRAIT_Const)
                    {
                        slot.type = nameFromU32()
                        var index=readU32()
                        if (index)
                            slot.value = defaults[data.readByte()][index]
                    }
                    else // (kind == TRAIT_Class)
                    {
                        slot.value = classFromU32()
                    }
                    break;
                case TRAIT_Method:
                case TRAIT_Getter:
                case TRAIT_Setter:
                    var disp_id = readU32()
                    var method = member = methodFromU32()
                    t.methods[disp_id] = method
                    method.id = disp_id
                    //print("\t",traitKinds[kind],name,disp_id,method,"// disp_id", disp_id)
                    break;
                }
                if (!member)
                    print("error trait kind "+kind)
                member.kind = kind
                member.name = name
                t.names[String(name)] = t.members[i] = member

                if ( (tag >> 4) & ATTR_metadata ) {
                    member.metadata = []
                    for(var j:int=0, mdCount:int=readU32(); j < mdCount; ++j)
                        member.metadata[j] = metadata[readU32()]
                }
            }
        }

        function parseClassInfos()
        {
            var start:int = data.position
            var count:int = instances.length
            classes = []
            for (var i:int=0; i < count; i++)
            {
                var t:Traits = classes[i] = new Traits()
                t.init = methodFromU32()
                t.base = "Class"
                t.itraits = instances[i]
                t.name = t.itraits.name + "$"
                t.init.name = t.itraits.name + "$cinit"
                t.init.kind = TRAIT_Method
                parseTraits(t)
            }
            infoPrint("ClassInfo count " + count + " size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+"%")
        }

        function parseScriptInfos()
        {
            var start:int = data.position
            var count:int = readU32()
            scripts = []
            for (var i:int=0; i < count; i++)
            {
                var t = new Traits()
                scripts[i] = t
                t.name = "script" + i
                t.base = names[0] // Object
                t.init = methodFromU32()
                t.init.name = t.name + "$init"
                t.init.kind = TRAIT_Method
                parseTraits(t)
            }
            infoPrint("ScriptInfo size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
        }
        
        function parseMethodBodies()
        {
            var start:int = data.position
            var count:int = readU32()
            for (var i:int=0; i < count; i++)
            {
                var m = methodFromU32()
                m.max_stack = readU32()
                m.local_count = readU32()
                var initScopeDepth = readU32()
                var maxScopeDepth = readU32()
                m.max_scope = maxScopeDepth - initScopeDepth
                var code_length = readU32()
                m.code = new ByteArray()
                m.code.endian = "littleEndian"
                if (code_length > 0) {
                    m.code_offset = data.position;
                    data.readBytes(m.code, 0, code_length)
                    m.code_length = code_length;
                }
                var ex_count = readU32()
                if (ex_count > 0) {
                    m.exceptions = []
                    for (var j:int = 0; j < ex_count; j++)
                    {
                        var ex = new ExceptionInfo()
                        m.exceptions.push(ex)
                        ex.from = readU32()
                        ex.to = readU32()
                        ex.target = readU32()
                        ex.type = nameFromU32()
                        //print("magic " + magic.toString(16))
                        //if (magic >= (46<<16|16))
                            ex.name = nameFromU32();
                        //infoPrint("exception method_id=" + i + " [" + ex.from + ", " + ex.to + "] " + ex.type + " -> " + ex.target)
                    }
                }
                parseTraits(m.activation = new Traits)
            }
            infoPrint("MethodBodies count " + count + " size "+(data.position-start)+" "+int(100*(data.position-start)/data.length)+" %")
        }

        function dump(indent:String="")
        {
            for each (var t in scripts)
            {
                infoPrint(indent+t.name)
                t.dump(this,indent)
                t.init.dump(this,indent)
            }

            for each (var m in methods)
            {
                if (!m.dumped) {
                    try {
                        m.dump(this,indent)
                    } catch (e:Error) {
                        print(m.format())
                        throw e
                    }
                }
            }

            infoPrint(align(14,"OPCODE",' ',"left")+"\tCOUNT\t SIZE\t% OF "+totalSize)
            var done = []
            for (;;)
            {
                var max:int = -1;
                var maxsize:int = 0;
                for (var i:int=0; i < 256; i++)
                {
                    if (opSizes[i] > maxsize && !done[i])
                    {
                        max = i;
                        maxsize = opSizes[i];
                    }
                }
                if (max == -1)
                    break;
                done[max] = 1;
                infoPrint(opNames[max]+"\t"+align(6,int(opCounts[max]))+"\t"+align(6,int(opSizes[max]))+"\t"+align(2,int(100*opSizes[max]/totalSize))+"%")
            }
        }

        // right/left align 's' to 'len' characters using 'pad' as padding
        function align(len, s, pad=' ', ment='right')
        {
            return (ment == 'right')
                ? new Array(Math.max(0,len-String(s).length)).join(pad)+String(s)
                : new String(s)+Array(Math.max(0,len-String(s).length)).join(pad)
        }
    }

    class Rect
    {
        var nBits:int
        var xMin:int, xMax:int
        var yMin:int, yMax:int
        public function toString()
        {
            return "[Rect "+xMin+" "+yMin+" "+xMax+" "+yMax+"]"
        }
    }

    const stagDoABC                 :int = 72;   // embedded .abc (AVM+) bytecode
    const stagSymbolClass           :int = 76;
    const stagMetadata              :int = 77;
    const stagDoABC2                :int = 82;   // revised ABC version with a name

    var tagNames:Array = [
        "End",                  // 00
        "ShowFrame",            // 01
        "DefineShape",          // 02
        "FreeCharacter",        // 03
        "PlaceObject",          // 04
        "RemoveObject",         // 05
        "DefineBits",           // 06
        "DefineButton",         // 07
        "JPEGTables",           // 08
        "SetBackgroundColor",   // 09

        "DefineFont",           // 10
        "DefineText",           // 11
        "DoAction",             // 12
        "DefineFontInfo",       // 13

        "DefineSound",          // 14
        "StartSound",           // 15
        "StopSound",            // 16

        "DefineButtonSound",    // 17

        "SoundStreamHead",      // 18
        "SoundStreamBlock",     // 19

        "DefineBitsLossless",   // 20
        "DefineBitsJPEG2",      // 21

        "DefineShape2",         // 22
        "DefineButtonCxform",   // 23

        "Protect",              // 24

        "PathsArePostScript",   // 25

        "PlaceObject2",         // 26
        "27 (invalid)",         // 27
        "RemoveObject2",        // 28

        "SyncFrame",            // 29
        "30 (invalid)",         // 30
        "FreeAll",              // 31

        "DefineShape3",         // 32
        "DefineText2",          // 33
        "DefineButton2",        // 34
        "DefineBitsJPEG3",      // 35
        "DefineBitsLossless2",  // 36
        "DefineEditText",       // 37

        "DefineVideo",          // 38

        "DefineSprite",         // 39
        "NameCharacter",        // 40
        "ProductInfo",          // 41
        "DefineTextFormat",     // 42
        "FrameLabel",           // 43
        "DefineBehavior",       // 44
        "SoundStreamHead2",     // 45
        "DefineMorphShape",     // 46
        "FrameTag",             // 47
        "DefineFont2",          // 48
        "GenCommand",           // 49
        "DefineCommandObj",     // 50
        "CharacterSet",         // 51
        "FontRef",              // 52

        "DefineFunction",       // 53
        "PlaceFunction",        // 54

        "GenTagObject",         // 55

        "ExportAssets",         // 56
        "ImportAssets",         // 57

        "EnableDebugger",       // 58

        "DoInitAction",         // 59
        "DefineVideoStream",    // 60
        "VideoFrame",           // 61

        "DefineFontInfo2",      // 62
        "DebugID",              // 63
        "EnableDebugger2",      // 64
        "ScriptLimits",         // 65

        "SetTabIndex",          // 66

        "DefineShape4",         // 67
        "DefineMorphShape2",    // 68

        "FileAttributes",       // 69

        "PlaceObject3",         // 70
        "ImportAssets2",        // 71

        "DoABC",                // 72
        "DefineFontAlignZones", // 73
        "CSMSettings",          // 74
        "DefineFont3",          // 75
        "SymbolClass",          // 76
        "Metadata",             // 77
        "DefineScalingGrid",    // 78
        "DefineDeviceVideo",    // 79
        "80 (invalid)",         // 80
        "81 (invalid)",         // 81
        "DoABC2",               // 82
        "DefineShape4",         // 83
        "DefineMorphShape2",    // 84
        "PlaceImagePrivate",    // 85
        "DefineSceneAndFrameLabelData", // 86
        "DefineBinaryData",     // 87
        "DefineFontName",       // 88
        "StartSound",           // 89
        "DefineBitsJPEG64",     // 90
        "DefineFont4",          // 91
    ]


    class Swf
    {
        private var bitPos:int
        private var bitBuf:int

        private var data:ByteArray

        function Swf(data:ByteArray)
        {
            this.data = data
            infoPrint("size "+decodeRect())
            infoPrint("frame rate "+(data.readUnsignedByte()<<8|data.readUnsignedByte()))
            infoPrint("frame count "+data.readUnsignedShort())
            decodeTags()
        }

        static function emitSwf(fn:String,data:ByteArray,version:uint)
        {
            var ba:ByteArray = new ByteArray
            ba.endian = "littleEndian"
            ba.writeByte(0x46); ba.writeByte(0x57); ba.writeByte(0x53); // FWS
            ba.writeByte(version>>24)
            ba.writeUnsignedInt(data.length+8)
            ba.writeBytes(data,0,data.length)
            File.writeByteArray(fn+".swf",ba)
            infoPrint("wrote "+ba.length+" bytes to file "+fn+".swf")
        }

        private function decodeTags()
        {
            var type:int, h:int, length:int
            var offset:int

            while (data.position < data.length)
            {
                type = (h = data.readUnsignedShort()) >> 6;

                if (((length = h & 0x3F) == 0x3F))
                    length = data.readInt();

                var tagN = tagNames[type]
                if (type >= tagNames.length)
                    tagN = type+" (unknown)"

                infoPrint(tagN+" "+length+"b "+int(100*length/data.length)+"%")
                switch (type)
                {
                case 0: return
                case stagDoABC2:
                    var pos1:int = data.position
                    data.readInt()
                    infoPrint("\nabc name "+readString())
                    length -= (data.position-pos1)
                    // fall through
                case stagDoABC:
                    var data2 = new ByteArray
                    data2.endian = "littleEndian"
                    data.readBytes(data2,0,length)
                    new Abc(data2).dump("  ")
                    infoPrint("")
                    break
                case stagMetadata:
                    infoPrint(new XML(readString()));
                    break;
                default:
                    data.position += length
                }
            }
        }

        private function readString():String
        {
            var s:String = ""
            var c:int

            while (c=data.readUnsignedByte())
                s += String.fromCharCode(c)

            return s
        }

        private function syncBits()
        {
            bitPos = 0
        }

        private function decodeRect():Rect
        {
            syncBits();

            var rect:Rect = new Rect();

            var nBits:int = readUBits(5)
            rect.xMin = readSBits(nBits);
            rect.xMax = readSBits(nBits);
            rect.yMin = readSBits(nBits);
            rect.yMax = readSBits(nBits);

            return rect;
        }

        function readSBits(numBits:int):int
        {
            if (numBits > 32)
                throw new Error("Number of bits > 32");

            var num:int = readUBits(numBits);
            var shift:int = 32-numBits;
            // sign extension
            num = (num << shift) >> shift;
            return num;
        }

        function readUBits(numBits:int):uint
        {
            if (numBits == 0)
                return 0

            var bitsLeft:int = numBits;
            var result:int = 0;

            if (bitPos == 0) //no value in the buffer - read a byte
            {
                bitBuf = data.readUnsignedByte()
                bitPos = 8;
            }

            while (true)
            {
                var shift:int = bitsLeft - bitPos;
                if (shift > 0)
                {
                    // Consume the entire buffer
                    result |= bitBuf << shift;
                    bitsLeft -= bitPos;

                    // Get the next byte from the input stream
                    bitBuf = data.readUnsignedByte();
                    bitPos = 8;
                }
                else
                {
                    // Consume a portion of the buffer
                    result |= bitBuf >> -shift;
                    bitPos -= bitsLeft;
                    bitBuf &= 0xff >> (8 - bitPos); // mask off the consumed bits

    //                if (print) System.out.println("  read"+numBits+" " + result);
                    return result;
                }
            }

            // unreachable, but fixes a spurious compiler warning
            return result;
        }
    }

    function help()
    {
        print(usage);
        System.exit(1)
    }

    function processArg(arg:String)
    {
        if (arg == '-a') {
            doExtractAbc = true;
            doExtractInfo = false
            doExtractAbs = false
        } else if (arg == '-i') {
            // suppress abs output
            doExtractAbs = false;
        } else if (arg == '-abs') {
            // suppress info output
            doExtractInfo = false
        } else if (arg == '-api') {
            doDumpAPI = true;
            doExtractInfo = false
        } else if (arg == '-mdversions') {
            useMetadataVersions = true
        } else if (arg == '-pools') {
            doDumpPools = true
        } else if (arg == '--decompress-only') {
            doDecompressOnly = true
        } else {
            print('Unknown option '+arg)
            help()
        }
    }

    function nextAbcFname():String
    {
        var s = currentFname
        if (currentFcount>0)
            s = s.concat(currentFcount);
        currentFcount++
        return s+'.abc'
    }

    // main
    var doExtractAbc = false
    var doExtractInfo = true
    var doExtractAbs = true
    var doDumpAPI = false
    var doDumpPools = false
    var doDecompressOnly = false
    var useMetadataVersions = false
    var currentFname = ''
    var currentFcount = 0
    for each (var file in System.argv)
    {
        if (file.indexOf('-')==0)
        {
            processArg(file)
            continue
        }

        var x;
        if ((x = file.lastIndexOf(".swf")) != -1 || (x = file.lastIndexOf(".swc")) != -1)
            currentFname = file.substring(0,x);
        else
            currentFname = file;
        var data:ByteArray = File.readByteArray(file)
        data.endian = "littleEndian"
        var version:uint = data.readUnsignedInt()
        switch (version&0xffffff) {
        case 46<<16|14:
        case 46<<16|15:
        case 46<<16|16:
        case 47<<16|12:
        case 47<<16|13:
        case 47<<16|14:
        case 47<<16|15:
        case 47<<16|16:
        case 47<<16|17:
        case 47<<16|18:
        case 47<<16|19:
        case 47<<16|20:
            var abc:Abc = new Abc(data)
            abc.dump()
            break
        case 67|87<<8|83<<16: // SWC
            var udata:ByteArray = new ByteArray
            udata.endian = "littleEndian"
            data.position = 8
            data.readBytes(udata,0,data.length-data.position)
            var csize:int = udata.length
            infoPrint("header CWS (deflate compressed) version "+data[3])
            udata.uncompress()
            infoPrint("decompressed swf "+csize+" -> "+udata.length)
            if (doDecompressOnly) {
               Swf.emitSwf(file,udata,version)
               System.exit(0)
            }
            udata.position = 0
            /*var swf:Swf =*/ new Swf(udata)
            break
        case 70|87<<8|83<<16: // SWF
            if (doDecompressOnly)
               System.exit(0)
            infoPrint("header FWS version "+data[3])
            data.position = 8 // skip header and length
            /*var swf:Swf =*/ new Swf(data)
            break
        case 90|87<<8|83<<16: // SWZ
            if (doDecompressOnly)
               System.exit(0)
            infoPrint("header ZWS (lzma compressed) version "+data[3])
            udata=new ByteArray
            udata.endian = "littleEndian"
            var ptr;
            // for swfs encoded in lzma: uncompressed length = bytes 4-7, lzma properties = bytes 8-12
            // also the swf-lzma length includes the ZWS+version (4 bytes) and the length(4 bytes) so we subtract 8
            // for 7z lzma files: lzma properties: bytes 0-4, uncompressed length: bytes 5-12
            // to correctly decompress the swf-lzma we have to change the headers

            // udata is a copy of the bytearray
            // put lzma properties in udata[0-4]
            for (ptr=0;ptr<5;ptr++) {
                udata[ptr]=data[12+ptr]
            }
            // calculate uncompressed length, subtract 8 (4 for ZWS+version, 4 for this length field)
            var scriptlen:uint=data[4]+(data[5]<<8)+(data[6]<<16)+(data[7]<<24)-8;

            // write uncompressed length in udata[5-8] then write 0s into udata[9-12] since 7z lzma expects length of 8 bytes
            udata[5]=scriptlen&0xFF;
            udata[6]=(scriptlen>>8) & 0xFF;
            udata[7]=(scriptlen>>16) & 0xFF;
            udata[8]=(scriptlen>>24) & 0xFF;

            for (ptr=0;ptr<4;ptr++) {
                udata[9+ptr]=0
            }

            // copy the compressed data from original data: 17 to copy: 13
            data.position = 17
            data.readBytes(udata,13,data.length-data.position)

            udata.position=0
            csize = udata.length

            // uncompress should work now, Error #2038 or Error #1000 will occur if the lzma format is incorrect
            udata.uncompress("lzma")
            infoPrint("decompressed swf "+csize+" -> "+udata.length)
            /*var swf:Swf =*/ new Swf(udata)
            break
        default:
            print('unknown format 0x'+version.toString(16))
            break
        }
    }

    if (System.argv.length < 1)
        help();
}

