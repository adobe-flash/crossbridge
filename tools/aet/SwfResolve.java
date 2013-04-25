// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package flascc;

import java.io.*;
import java.nio.charset.Charset;
import java.util.*;

import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;

import flascc.FlasccNamespaceTranslator;

import static com.adobe.flash.abc.ABCConstants.*;

public class SwfResolve {
    // definition of a global sym
    static class Def {
        final int abcID; // abc w/ prevailing definition
        final boolean isWeak; // prevailing def weak?
        final Name type; // type of definition

        public Def(int abcID, boolean isWeak, Name type) {
            this.abcID = abcID;
            this.isWeak = isWeak;
            this.type = type;
        }
    }

    // Name and data for binary data
    static class NameAndData {
        final Name name;
        final byte[] data;

        public NameAndData(Name name, byte[] data) {
            this.name = name;
            this.data = data;
        }
    }

    // lots of visitor wrapping to record info about global syms
    // records "Weak" symbols
    // records "GlobalMethod" methods
    // records data associated with "HexData" metadata tags
    static class AnalysisAbcVisitor extends NilABCVisitor {
        private Map<Name, Def> _globals;
        private List<NameAndData> _binaryData;
        private Set<MethodInfo> _globalMethodSet = new HashSet<MethodInfo>();
        private Map<MethodInfo, Integer> _methodIDs = new HashMap<MethodInfo, Integer>();
        private Set<Integer> _globalMethodIDs;
        private Map<Name, Set<Integer>> _potentialDefLocs;
        private int _abcID;

        public int lastScriptinit = 0;

        public AnalysisAbcVisitor(Map<Name, Def> globals,
                List<NameAndData> binaryData, Set<Integer> globalMethodIDs,
                Map<Name, Set<Integer>> potentialDefLocs, int abcID) {
            _globals = globals;
            _binaryData = binaryData;
            _globalMethodIDs = globalMethodIDs;
            _potentialDefLocs = potentialDefLocs;
            _abcID = abcID;
        }

        class _ScriptTraitMetadataVisitor extends NilMetadataVisitor {
            private boolean _sawWeak = false;
            private boolean _sawGlobalMethod = false;
            private ByteArrayOutputStream _binaryData = null;

            @Override
            public void visit(Metadata md) {
                String name = md.getName();

                if (name.equals("Weak") && md.getValues().length == 0)
                    _sawWeak = true;
                if (name.equals("GlobalMethod") && md.getValues().length == 0)
                    _sawGlobalMethod = true;
                else if (name.equals("HexData")) {
                    String[] keys = md.getKeys();
                    String[] values = md.getValues();

                    for (int i = 0; i < keys.length; i++)
                        if (keys[i] == null) {
                            if (_binaryData == null)
                                _binaryData = new ByteArrayOutputStream();

                            String hex = values[i];
                            int pairLen = hex.length() & ~1;

                            for (int c = 0; c < pairLen; c += 2) {
                                int val = Integer.parseInt(
                                        hex.substring(c, c + 2), 16);
                                _binaryData.write(val);
                            }
                        }
                }
            }

            public boolean sawWeak() {
                return _sawWeak;
            }

            public boolean sawGlobalMethod() {
                return _sawGlobalMethod;
            }

            public byte[] getBinaryData() {
                return (_binaryData == null) ? null : _binaryData.toByteArray();
            }
        }

        class _ScriptTraitVisitor extends NilTraitVisitor {
            Name _name;
            Name _type;
            MethodInfo _method;
            _ScriptTraitMetadataVisitor _mv;

            public _ScriptTraitVisitor(Name name, Name type, MethodInfo method) {
                _name = name;
                _type = type;
                _method = method;
            }

            @Override
            public IMetadataVisitor visitMetadata(int count) {
                return _mv = new _ScriptTraitMetadataVisitor();
            }

            @Override
            public void visitEnd() {
                // no metadata means no "weak" metadata!
                boolean weak = (_mv == null) ? false : _mv.sawWeak();
                Def def = _globals.get(_name);

                if (def == null || def.isWeak)
                {
                    // first def or existing def is weak: replace
                    _globals.put(
                            _name,
                            new Def(_abcID,
                                    weak, // degrade type to "*" if types
                                    // disagree
                                    (def == null || (def.type != null && def.type
                                            .equals(_type))) ? _type : null));
                    if (!_potentialDefLocs.containsKey(_name))
                        _potentialDefLocs.put(_name, new HashSet<Integer>());
                    _potentialDefLocs.get(_name).add(_abcID);
                } else if (!weak) // two strong defs?
                    System.err.println("Warning: Multiply defined: "
                            + _name.toString()); // multiple definitions!

                boolean globalMethod = (_mv == null) ? false : _mv
                        .sawGlobalMethod();

                // record any "global methods"
                if (globalMethod && _method != null)
                    _globalMethodSet.add(_method);

                byte[] binaryData = (_mv == null) ? null : _mv.getBinaryData();

                if (binaryData != null) {
                    if (_binaryData == null)
                        _binaryData = new ArrayList<NameAndData>();
                    _binaryData.add(new NameAndData(_name, binaryData));
                }
            }
        }

        class _ScriptTraitsVisitor extends NilTraitsVisitor {
            @Override
            public ITraitVisitor visitClassTrait(int kind, Name name,
                    int slotId, ClassInfo clazz) {
                return new _ScriptTraitVisitor(name, null, null);
            }

            @Override
            public ITraitVisitor visitMethodTrait(int kind, Name name,
                    int dispId, MethodInfo method) {
                return new _ScriptTraitVisitor(name, null, method);
            }

            @Override
            public ITraitVisitor visitSlotTrait(int kind, Name name,
                    int slotId, Name slotType, Object slotValue) {
                return new _ScriptTraitVisitor(name, slotType, null);
            }
        }

        class _ScriptVisitor extends NilScriptVisitor {
            @Override
            public ITraitsVisitor visitTraits() {
                return new _ScriptTraitsVisitor();
            }

            @Override
            public void visitInit(MethodInfo methodInfo) {
                lastScriptinit++;
            }

        }

        @Override
        public IScriptVisitor visitScript() {
            return new _ScriptVisitor();
        }

        @Override
        public IMethodVisitor visitMethod(MethodInfo minfo) {
            // record method traversal order
            _methodIDs.put(minfo, _methodIDs.size());
            return null;
        }

        @Override
        public void visitEnd() {
            // build set of "global method" method ids
            for (MethodInfo mi : _globalMethodSet)
                _globalMethodIDs.add(_methodIDs.get(mi));
            super.visitEnd();
        }
    }

    // update prevailing definitions for global symbols in the given abc (with
    // abc ID abcID) find binary data metadata
    public static void analyzeAbc(Map<Name, Def> globals,
            List<NameAndData> binaryData, Set<Integer> globalMethodIDs,
            Map<Name, Set<Integer>> potentialDefLocs,
            Map<Integer, Integer> initScripts, int abcID, byte[] abcBytes,
            int offset, int len, com.adobe.flash.abc.ABCParser.ConstantPoolTranslator constantPoolTranslator) throws IOException {
        byte[] bytes = new byte[len];
        System.arraycopy(abcBytes, offset, bytes, 0, len); // TODO can't stream?
        AnalysisAbcVisitor aav = new AnalysisAbcVisitor(globals, binaryData,
                globalMethodIDs, potentialDefLocs, abcID);
        ABCParser parser = new ABCParser(bytes);
        if(constantPoolTranslator != null)
          parser.setConstantPoolTranslator(constantPoolTranslator);
        parser.parseABC(aav);
        initScripts.put(abcID, aav.lastScriptinit);
    }

    // lots of visitor wrappers used to filter script Traits / initproperty ops
    // to remove any globals where we are not the prevailing definer
    // resolves "Weak" methods
    // fixes "GlobalMethod" methods
    static class ResolveAbcVisitor extends DelegatingABCVisitor {
        private Map<Name, Def> _globals;
        private Map<Name, Set<Integer>> _potentialDefLocs;
        private Set<Integer> _globalMethodIDs;
        private int _methodCount = 0;
        private int _abcID;

        private Set<MethodInfo> _scriptInitMethodInfos = new HashSet<MethodInfo>();
        private Namespace secretNS = null;
        private Map<Integer, Integer> _initscripts;
        private MethodInfo lastScriptInitInfo = null;
        private int scriptcount = 0;

        public ResolveAbcVisitor(IABCVisitor delegate, Map<Name, Def> globals,
                Set<Integer> globalMethodIDs,
                Map<Name, Set<Integer>> potentialDefLocs,
                Map<Integer, Integer> initscripts, int abcID, String _secretNS) {
            super(delegate);
            _globalMethodIDs = globalMethodIDs;
            _globals = globals;
            _potentialDefLocs = potentialDefLocs;
            _initscripts = initscripts;
            _abcID = abcID;
            secretNS = _secretNS != null ? new Namespace(
                    ABCConstants.CONSTANT_Namespace, _secretNS) : null;
        }

        class _MethodBodyVisitor extends DelegatingMethodBodyVisitor {
            boolean _isScriptInit;
            boolean _isGlobalMethod;
            int _lastOp = -1;
            boolean isLastScriptBody;

            public _MethodBodyVisitor(IMethodBodyVisitor delegate,
                    boolean isScriptInit, boolean isGlobalMethod,
                    MethodBodyInfo minfo) {
                super(delegate);
                _isScriptInit = isScriptInit;
                _isGlobalMethod = isGlobalMethod;
                isLastScriptBody = lastScriptInitInfo == minfo.getMethodInfo();
            }

            @Override
            public void visit() {
                super.visit();

                if (secretNS != null && isLastScriptBody && _abcID > 0) {
                    super.visitInstruction(
                            ABCConstants.OP_finddef,
                            new Object[] { new Name(secretNS, "" + (_abcID - 1)) });
                    super.visitInstruction(ABCConstants.OP_pop);
                }
            }

            @Override
            public void visitInstruction(int arg0) {
                // dupe any getlocal0/pushscope sequences if we're a
                // "global method"... this enables the VM to statically evaluate
                // references to stuff in the function's corresponding global
                // via "this" as it ensures the function's scope chain
                // has this in a non-zero scope slot (scope slot 0 is treated
                // specially -- nothing binds to it to make cross-script
                // global references behave correctly)
                if (_isGlobalMethod && arg0 == ABCConstants.OP_pushscope
                        && _lastOp == ABCConstants.OP_getlocal0) {
                    super.visitInstruction(ABCConstants.OP_dup);
                    super.visitInstruction(ABCConstants.OP_pushscope);
                }
                _lastOp = arg0;
                super.visitInstruction(arg0);
            }

            @Override
            public void visitInstruction(int arg0, int arg1) {
                _lastOp = arg0;
                // compensate for extra scope
                if (_isGlobalMethod && arg0 == ABCConstants.OP_getscopeobject)
                    arg1++;
                super.visitInstruction(arg0, arg1);
            }

            @Override
            public void visitInstruction(int op, Object arg) {
                _lastOp = op;
                if (_isScriptInit
                        && (op == ABCConstants.OP_setproperty || op == ABCConstants.OP_initproperty)
                        && isOverridden((Name) arg)) {
                    // initproperty takes two operands, so pop them both instead
                    // of actually performing the initproperty
                    super.visitInstruction(ABCConstants.OP_pop);
                    super.visitInstruction(ABCConstants.OP_pop);
                    return;
                }
                if (_isScriptInit && (op == ABCConstants.OP_findpropstrict || op == ABCConstants.OP_findproperty)) {
                    Name n = (Name) arg;
                    if (n.getQualifiers() != null
                            && n.getQualifiers().length() == 1) {
                        Namespace ns = n.getSingleQualifier();
                        if (ns != null) {
                            n = new Name(ABCConstants.CONSTANT_Qname,
                                    new Nsset(ns), n.getBaseName());
                            if (isOverridden(n)
                                    && _potentialDefLocs.containsKey(n)
                                    && _potentialDefLocs.get(n)
                                    .contains(_abcID)) {
                                // pushnull just to keep the stack balanced
                                super.visitInstruction(ABCConstants.OP_pushnull);
                                return;
                            }
                        }
                    }
                }
                super.visitInstruction(op, arg);
            }
        }

        class _MethodVisitor extends DelegatingMethodVisitor {
            public _MethodVisitor(IMethodVisitor delegate) {
                super(delegate);
            }

            @Override
            public void visitEnd() {
                _methodCount++;
                super.visitEnd();
            }

            @Override
            public IMethodBodyVisitor visitBody(MethodBodyInfo minfo) {
                IMethodBodyVisitor v = new _MethodBodyVisitor(
                        super.visitBody(minfo),
                        _scriptInitMethodInfos.contains(minfo.getMethodInfo()),
                        _globalMethodIDs.contains(_methodCount), minfo);
                return v;
            }
        }

        class _ScriptTraitsVisitor extends DelegatingTraitsVisitor {
            private Boolean slotAdded = false;

            public _ScriptTraitsVisitor(ITraitsVisitor delegate) {
                super(delegate);
            }

            @Override
            public void visitEnd() {
                if (!slotAdded && secretNS != null
                        && _initscripts.get(_abcID) == scriptcount) {
                    Name nm = new Name(secretNS, "" + _abcID);
                    ITraitVisitor tv = super.visitSlotTrait(
                            ABCConstants.TRAIT_Const, nm,
                            ITraitsVisitor.RUNTIME_SLOT, null,
                            ABCConstants.UNDEFINED_VALUE);
                    tv.visitStart();
                    tv.visitEnd();
                    slotAdded = true;
                }
                super.visitEnd();
            }

            @Override
            public ITraitVisitor visitClassTrait(int arg0, Name arg1, int arg2,
                    ClassInfo arg3) {
                return isOverridden(arg1) ? null : new DelegatingTraitVisitor(
                        super.visitClassTrait(arg0, arg1, arg2, arg3));
            }

            @Override
            public ITraitVisitor visitMethodTrait(int arg0, Name arg1,
                    int arg2, MethodInfo arg3) {
                // if overridden, don't emit at all
                return isOverridden(arg1) ? null :
                    new DelegatingTraitVisitor(super.visitMethodTrait(arg0,
                            arg1, arg2, arg3));
            }

            @Override
            public ITraitVisitor visitSlotTrait(int arg0, Name arg1, int arg2,
                    Name arg3, Object arg4) {
                return isOverridden(arg1) ? null : new DelegatingTraitVisitor(
                        super.visitSlotTrait(arg0, arg1, arg2,
                                prevailingType(arg1, arg3), arg4));
            }
        }

        class _ScriptVisitor extends DelegatingScriptVisitor {
            public _ScriptVisitor(IScriptVisitor delegate) {
                super(delegate);
            }

            @Override
            public void visit() {
                ++scriptcount;
                super.visit();
            }

            @Override
            public void visitInit(MethodInfo arg0) {
                _scriptInitMethodInfos.add(arg0);
                super.visitInit(arg0);
                if (_initscripts.get(_abcID) == scriptcount)
                    lastScriptInitInfo = arg0;
            }

            @Override
            public ITraitsVisitor visitTraits() {
                return new _ScriptTraitsVisitor(super.visitTraits());
            }
        }

        @Override
        public IScriptVisitor visitScript() {
            return new _ScriptVisitor(super.visitScript());
        }

        @Override
        public IMethodVisitor visitMethod(MethodInfo minfo) {
            return new _MethodVisitor(super.visitMethod(minfo));
        }

        public boolean isOverridden(Name name) {
            Def def = _globals.get(name);

            // not ours? change namespace to hiding private NS
            if (def != null && def.abcID != _abcID)
                return true;
            return false;
        }

        public Name prevailingType(Name name, Name defTypeName) {
            Def def = _globals.get(name);

            return (def != null) ? def.type : defTypeName;
        }
    }

    // resolve global references for a given ABC (with abc ID abcID) using
    // definitions in "globals"
    public static byte[] processAbc(Map<Name, Def> globals,
            Set<Integer> globalMethodIDs, Map<Name, Set<Integer>> overridden,
            Map<Integer, Integer> initscripts, int abcID, byte[] abcBytes,
            int offset, int len, String secretNS, com.adobe.flash.abc.ABCParser.ConstantPoolTranslator constantPoolTranslator) throws Exception {
        byte[] bytes = new byte[len];
        System.arraycopy(abcBytes, offset, bytes, 0, len); // TODO can't stream?

        ABCEmitter emitter = new ABCEmitter();
        emitter.setAllowBadJumps(true);
        DelegatingABCVisitor visitor = new ResolveAbcVisitor(emitter, globals,
                globalMethodIDs, overridden, initscripts, abcID, secretNS);
        ABCParser parser = new ABCParser(bytes);
        if(constantPoolTranslator != null)
          parser.setConstantPoolTranslator(constantPoolTranslator);
        parser.parseABC(visitor);
        return emitter.emit();
    }

    public static SWF process(SWF inSWF, short startID, boolean forceordering, String replacementNS)
    throws Exception {
        com.adobe.flash.abc.ABCParser.ConstantPoolTranslator constantPoolTranslator = (replacementNS == null) ? null :
          new FlasccNamespaceTranslator(replacementNS);
        // map script global symbols to prevailing definition
        Map<Name, Def> scriptGlobals = new HashMap<Name, Def>();
        // map class name to binary data
        List<NameAndData> binaryData = new ArrayList<NameAndData>();
        // keep offset of actual abc data in DoABC records so we don't need to
        // recompute
        Map<Integer, Integer> abcDataOffsets = new HashMap<Integer, Integer>();
        // track "global methods"
        Map<Integer, Set<Integer>> abcGlobalMethodIDs = new HashMap<Integer, Set<Integer>>();
        // track the abcs that could potentially define a weak symbol
        Map<Name, Set<Integer>> potentialDefLocs = new HashMap<Name, Set<Integer>>();

        // current abcID
        int abcID;
        // record number of last DoABC
        int lastDoABCRecNum = -1;

        // iterate over all records
        SWFHeader header = inSWF.header;
        SWFRecord[] records = inSWF.records;

        // Generate a unique NS for this SWF that will hide the script ordering
        // dependencies we inject
        String secretNS = forceordering ? "__force_ordering_ns_"
                + java.util.UUID.randomUUID().toString() : null;
                Map<Integer, Integer> scriptinits = new HashMap<Integer, Integer>();
                // iterate over all DoABCs building up scriptGlobals/binaryData
                abcID = 0;
                for (int i = 0; i < records.length; i++) {
                    SWFRecord record = records[i];

                    // DoABC -- find global syms
                    if (record.tag == 82 || record.tag == 72) {
                        int dataOffset = 0;

                        if (record.tag == 82) {
                            dataOffset = 4;
                            // skip past flags+name
                            while (record.payload[dataOffset] != 0)
                                dataOffset++;
                            dataOffset++;
                        }

                        abcDataOffsets.put(abcID, dataOffset);

                        Set<Integer> globalMethodIDs = new HashSet<Integer>();

                        abcGlobalMethodIDs.put(abcID, globalMethodIDs);

                        analyzeAbc(scriptGlobals, binaryData, globalMethodIDs,
                                potentialDefLocs, scriptinits, abcID, record.payload,
                                dataOffset, record.payload.length - dataOffset, constantPoolTranslator);
                        abcID++;
                        lastDoABCRecNum = i;
                    }
                }

                ArrayList<SWFRecord> newRecords = new ArrayList<SWFRecord>();

                // destructively transcode
                abcID = 0;
                for (int i = 0; i < records.length; i++) {
                    SWFRecord record = records[i];

                    records[i] = null;

                    // rewrite DoABCs w/ script globals resolved
                    if (record.tag == 82 || record.tag == 72) // DoABC
                    {
                        int dataOffset = abcDataOffsets.get(abcID);
                        Set<Integer> globalMethodIDs = abcGlobalMethodIDs.get(abcID);

                        byte[] abcBytes = processAbc(scriptGlobals, globalMethodIDs,
                                potentialDefLocs, scriptinits, abcID, record.payload,
                                dataOffset, record.payload.length - dataOffset,
                                secretNS, constantPoolTranslator);
                        byte[] payload = new byte[dataOffset + abcBytes.length];
                        System.arraycopy(record.payload, 0, payload, 0, dataOffset);
                        System.arraycopy(abcBytes, 0, payload, dataOffset,
                                abcBytes.length);
                        newRecords.add(new SWFRecord((short) record.tag, payload));
                        abcID++;

                        // last abc? write out binary data if any
                        if (i == lastDoABCRecNum && binaryData.size() > 0) {
                            short curID;

                            curID = startID;
                            for (NameAndData nd : binaryData) {
                                byte[] defBinPayload = new byte[nd.data.length + 6];

                                System.arraycopy(nd.data, 0, defBinPayload, 6,
                                        nd.data.length);
                                // first u16 is tag id
                                defBinPayload[0] = (byte) curID;
                                defBinPayload[1] = (byte) (curID >> 8);
                                // followed by zero u32 (so do nothing)
                                newRecords
                                .add(new SWFRecord((short) 87, defBinPayload));
                                if (--curID == 0x10)
                                    throw new Error("Ran out of tag ids!");
                            }
                            
                            // symbolclass payload
                            ByteArrayOutputStream bas = new ByteArrayOutputStream();
                            bas.write(binaryData.size());
                            bas.write(binaryData.size() >> 8);

                            curID = startID;
                            for (NameAndData nd : binaryData) {
                                // tag id
                                bas.write(curID);
                                bas.write(curID >> 8);

                                // TODO verify correct kind (which ones will work?)
                                String className = nd.name.getSingleQualifier()
                                .getName() + "." + nd.name.getBaseName();
                                bas.write(className.getBytes(Charset.forName("UTF-8")));
                                bas.write(0);

                                curID--;
                            }

                            newRecords
                            .add(new SWFRecord((short) 76, bas.toByteArray()));
                        }
                    } else {
                        newRecords.add(record);
                    }
                }
                // build output swf
                return new SWF(header, newRecords.toArray(new SWFRecord[0]));
    }
}
