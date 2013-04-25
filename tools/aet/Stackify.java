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
import java.util.*;

import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;
import com.adobe.flash.abc.optimize.*;
import com.adobe.flash.abc.graph.*;
import com.adobe.flash.abc.models.*;
import com.adobe.flash.abc.instructionlist.*;

import static com.adobe.flash.abc.ABCConstants.*;

public final class Stackify extends DelegatingABCVisitor {
    protected int removedSets = 0, failedremovals = 0;
    protected int totalRegs = 0;
    protected int killedRegs = 0, liveoutRegs = 0;
    protected int swapCount = 0;
    protected int mid = 0;

    public Stackify(IABCVisitor delegate) {
      super(delegate);
    }

    private static void debug(String s) {
        // System.err.println(s);
    }

    private static void info(String s) {
        System.out.println(s);
    }

    class InstructionID {
        public int blockID, instructionoffset;

        InstructionID(int bid, int iid) {
            blockID = bid;
            instructionoffset = iid;
        }

        @Override
        public boolean equals(Object o) {
            if (o instanceof InstructionID) {
                InstructionID iid = (InstructionID) o;
                return blockID == iid.blockID
                        && instructionoffset == iid.instructionoffset;
            }

            return false;
        }

        @Override
        public int hashCode() {
            return blockID << 16 | instructionoffset;
        }
    }

    static class UnderFlowChecker extends FrameModelEncoder {
        private UnderFlowCheckerVisitor ucv;

        public Boolean underflow() {
            return ucv.underflow;
        }

        public int stackdepth() {
            return ucv.stkdepth;
        }

        static class UnderFlowCheckerVisitor implements
                FrameModelVisitor<Boolean> {
            public int stkdepth = 0;
            public Boolean underflow = false;

            public void visit(FrameModelEncoder encoder) {
            }

            public void visitEnd() {
            }

            private void modStack(int incr) {
                stkdepth += incr;
                if (stkdepth < 0)
                    underflow = true;
            }

            public Boolean noFrameEffect(Instruction i) {
                return underflow;
            }

            public Boolean consumeValue(Instruction i, int count) {
                modStack(-count);
                return underflow;
            }

            public Boolean produceValue(Instruction i) {
                modStack(1);
                return underflow;
            }

            public Boolean consumeAndProduceValue(Instruction i,
                    int consumeCount) {
                modStack(-consumeCount);
                modStack(1);
                return underflow;
            }

            public Boolean branch(Instruction i, IBasicBlock target) {
                return underflow;
            }

            public Boolean multiwayBranch(Instruction i,
                    Collection<IBasicBlock> targets) {
                return underflow;
            }

            public Boolean getlocal(Instruction i, int idx) {
                modStack(1);
                return underflow;
            }

            public Boolean setlocal(Instruction i, int idx) {
                modStack(-1);
                return underflow;
            }

            public void modifyLocal(Instruction i, int idx) {
            }

            public Boolean moveValueToScopeStack(Instruction i) {
                modStack(-1);
                return underflow;
            }

            public Boolean popscope(Instruction i) {
                return underflow;
            }

            public Boolean getScopeobject(Instruction i, int idx) {
                modStack(1);
                return underflow;
            }

            public Boolean hasnext2(Instruction i) {
                modStack(1);
                return underflow;
            }

            public Boolean dup(Instruction i) {
                modStack(-1);
                modStack(2);
                return underflow;
            }

            public Boolean swap(Instruction i) {
                modStack(-2);
                modStack(2);
                return underflow;
            }

            public boolean visitBlock(IBasicBlock b) {
                return underflow;
            }

            public void visitEndBlock(IBasicBlock b) {
            }

            public void visitEdge(IBasicBlock from, IBasicBlock to) {
            }
        }

        private UnderFlowChecker(MethodBodyInfo mbi, UnderFlowCheckerVisitor fmv) {
            super(mbi, fmv, null);
            ucv = fmv;
        }

        public static UnderFlowChecker create(MethodBodyInfo mbi) {
            return new UnderFlowChecker(mbi, new UnderFlowCheckerVisitor());
        }
    }

    public class InstructionReWriter extends DeadCodeFilter {
        public InstructionReWriter(MethodBodyInfo mbi,
                IMethodBodyVisitor delegate, IDiagnosticsVisitor diagnostics) {
            super(mbi, delegate, diagnostics);
            this.mbi = mbi;
            this.diagnostics = diagnostics;
        }

        protected final MethodBodyInfo mbi;

        protected final IDiagnosticsVisitor diagnostics;

        @Override
        public void visitEnd() {
            IFlowgraph cfg = this.mbi.getCfg();
            List<IBasicBlock> blocks = cfg.getBlocksInEntryOrder();
            HashMap<IBasicBlock, Integer> cfgOrder = new HashMap<IBasicBlock, Integer>();

            {
              int i = 0;
              for (IBasicBlock b : cfg.blocksInControlFlowOrder())
                cfgOrder.put(b, i++);
              for (IBasicBlock b : blocks)
                if(!cfgOrder.containsKey(b))
                  cfgOrder.put(b, i++);
            }

            for (IBasicBlock b : blocks) {
                Iterator<Instruction> it = b.getInstructions().iterator();
                List<Instruction> newinstrs = new ArrayList<Instruction>();

                int ioff = 0;
                while (it.hasNext()) {
                    InstructionID iid = new InstructionID(cfgOrder.get(b), ioff);
                    newinstrs.add(transformInstruction(it.next(), iid));
                    ioff++;
                }
                b.getInstructions().clear();
                b.getInstructions().addAll(newinstrs);
            }

            super.visitEnd();
        }

        protected Instruction transformInstruction(Instruction i,
                InstructionID iid) {
            return i;
        }
    }

    class _MethodBodyVisitor extends InstructionReWriter {
        MethodBodyInfo mbinfo;

        public _MethodBodyVisitor(IMethodBodyVisitor delegate,
                MethodBodyInfo _mbinfo) {
            super(_mbinfo, delegate, new NilDiagnosticsVisitor());
            mbinfo = _mbinfo;
        }

        private HashSet<InstructionID> replaceDup = new HashSet<InstructionID>();
        private HashSet<InstructionID> replaceNop = new HashSet<InstructionID>();
        private HashSet<InstructionID> replacePop = new HashSet<InstructionID>();
        private HashSet<InstructionID> replaceSwap = new HashSet<InstructionID>();

        @Override
        protected Instruction transformInstruction(Instruction insn,
                InstructionID pos) {
            if (replaceDup.contains(pos))
                insn = InstructionFactory.getInstruction(OP_dup);
            else if (replaceNop.contains(pos))
                insn = InstructionFactory.getInstruction(OP_nop);
            else if (replaceSwap.contains(pos))
                insn = InstructionFactory.getInstruction(OP_swap);
            else if (replacePop.contains(pos))
                insn = InstructionFactory.getInstruction(OP_pop);

            return super.transformInstruction(insn, pos);
        }

        @Override
        public void visitEnd() {
            debug("Method: (max_scope: " + mbinfo.max_scope + ", max_local: "
                    + mbinfo.max_local + ", max_slot: " + mbinfo.max_slot
                    + ", code_len: " + mbinfo.code_len + ")");
            totalRegs += mbinfo.max_local;

            if (mbinfo.getExceptions().size() != 0) {
                debug("Skipping method with exception info: stackify");
                super.visitEnd();
                return;
            }

            IFlowgraph cfg = mbinfo.getCfg();

            // If a register is set before the first get in all blocks that
            // reference that local then all uses of that local can be
            // replaced with stack usage.
            HashSet<IBasicBlock> visitedBlocks = new HashSet<IBasicBlock>();
            HashSet<Integer> locals = new HashSet<Integer>();
            HashMap<Integer, Boolean> registerKills = new HashMap<Integer, Boolean>();
            for (IBasicBlock b : cfg.blocksInControlFlowOrder()) {
                if (visitedBlocks.contains(b))
                    continue;
                else
                    visitedBlocks.add(b);

                HashMap<Integer, Boolean> localregisterKills = new HashMap<Integer, Boolean>();
                for (Instruction i : b.getInstructions()) {
                    boolean isSet = true;
                    int local = 0;
                    switch (i.getOpcode()) {
                    case OP_setlocal0:
                        local = 0;
                        break;
                    case OP_setlocal1:
                        local = 1;
                        break;
                    case OP_setlocal2:
                        local = 2;
                        break;
                    case OP_setlocal3:
                        local = 3;
                        break;
                    case OP_setlocal:
                    case OP_kill:
                        local = i.getImmediate();
                        break;

                    case OP_getlocal0:
                        local = 0;
                        isSet = false;
                        break;
                    case OP_getlocal1:
                        local = 1;
                        isSet = false;
                        break;
                    case OP_getlocal2:
                        local = 2;
                        isSet = false;
                        break;
                    case OP_getlocal3:
                        local = 3;
                        isSet = false;
                        break;
                    case OP_hasnext2:
                        // Bail when we encounter a hasnext2
                        localregisterKills
                                .put((Integer) i.getOperand(0), false);
                        localregisterKills
                                .put((Integer) i.getOperand(1), false);
                        registerKills.put((Integer) i.getOperand(0), false);
                        registerKills.put((Integer) i.getOperand(1), false);
                        continue;
                    case OP_getlocal:
                    case OP_inclocal:
                    case OP_declocal:
                    case OP_inclocal_i:
                    case OP_declocal_i:
                        local = i.getImmediate();
                        isSet = false;
                        break;
                    default:
                        continue;
                    }

                    locals.add(local);

                    if (localregisterKills.containsKey(local))
                        continue;

                    localregisterKills.put(local, isSet);

                    if (registerKills.containsKey(local)) {
                        isSet = isSet && registerKills.get(local);
                    }

                    registerKills.put(local, isSet);
                }
            }

            debug("Kill info: (blocks: " + visitedBlocks.size() + ")");

            for (Integer i : registerKills.keySet()) {
                debug("  local " + i + " "
                        + (registerKills.get(i) ? "KILL" : "liveout"));

                if (registerKills.get(i))
                    killedRegs++;
                else
                    liveoutRegs++;
            }

            // Find set/get pairs and replace them with stack usage

            visitedBlocks = new HashSet<IBasicBlock>();
            Boolean firstBlock = true;
            int bi = -1;
            for (IBasicBlock b : cfg.blocksInControlFlowOrder()) {
                bi++;
                if (firstBlock) {
                    firstBlock = false;
                    visitedBlocks.add(b);

                    for (int i = 0; i < b.getInstructions().size(); i++) {
                        Instruction instr = b.getInstructions().get(i);
                        switch (instr.getOpcode()) {
                        case OP_pushbyte:
                        case OP_pushdouble:
                        case OP_pushfalse:
                        case OP_pushfloat:
                        case OP_pushfloat4:
                        case OP_pushint:
                        case OP_pushnamespace:
                        case OP_pushnan:
                        case OP_pushnull:
                        case OP_pushshort:
                        case OP_pushstring:
                        case OP_pushtrue:
                        case OP_pushuint:
                        case OP_pushundefined: {
                            int nextopcode = i + 1 < b.getInstructions().size() ? b
                                    .getInstructions().get(i + 1).getOpcode()
                                    : -1;
                            int local = -1;
                            switch (nextopcode) {
                            case OP_setlocal0:
                                local = 0;
                                break;
                            case OP_setlocal1:
                                local = 1;
                                break;
                            case OP_setlocal2:
                                local = 2;
                                break;
                            case OP_setlocal3:
                                local = 3;
                                break;
                            case OP_setlocal:
                                local = b.getInstructions().get(i + 1)
                                        .getImmediate();
                                break;
                            default:
                                break;
                            }

                            if (local == -1) {
                                // The next opcode isn't a setlocal so this
                                // isn't a removal candidate
                                continue;
                            }

                            if (!registerKills.get(local)) {
                                // This local isn't killed in other blocks so we
                                // must preserve it
                                continue;
                            }

                            int l2 = -1;
                            for (int j = i + 2; j < b.getInstructions().size(); j++) {
                                Instruction i2 = b.getInstructions().get(j);

                                switch (i2.getOpcode()) {
                                case OP_getlocal0:
                                    l2 = 0;
                                    break;
                                case OP_getlocal1:
                                    l2 = 1;
                                    break;
                                case OP_getlocal2:
                                    l2 = 2;
                                    break;
                                case OP_getlocal3:
                                    l2 = 3;
                                    break;
                                case OP_hasnext2:
                                    // Bail when we encounter a hasnext2
                                    l2 = local;
                                    break;
                                case OP_getlocal:
                                case OP_declocal:
                                case OP_inclocal:
                                case OP_declocal_i:
                                case OP_inclocal_i:

                                    l2 = i2.getImmediate();
                                    break;
                                }
                                if (local == l2) {
                                    // This local is used so we must leave the
                                    // set alone
                                    break;
                                }
                            }

                            if (local == l2) {
                                // This local is used so we must leave the set
                                // alone
                                continue;
                            }

                            // The set is redundant, remove both the push and
                            // the set
                            replaceNop.add(new InstructionID(bi, i));
                            replaceNop
                                    .add(new InstructionID(bi, i + 1));
                            i++; // no need to inspect the set

                            removedSets++;
                        }
                        default:
                            continue;
                        }
                    }
                    continue;
                }

                if (visitedBlocks.contains(b))
                    continue;
                else
                    visitedBlocks.add(b);

                int originalBlockPos = bi;

                for (int i = 0; i < b.getInstructions().size(); i++) {
                    Instruction instr = b.getInstructions().get(i);
                    boolean isSet = false;
                    int local = 0;
                    switch (instr.getOpcode()) {
                    case OP_setlocal0:
                        local = 0;
                        isSet = true;
                        break;
                    case OP_setlocal1:
                        local = 1;
                        isSet = true;
                        break;
                    case OP_setlocal2:
                        local = 2;
                        isSet = true;
                        break;
                    case OP_setlocal3:
                        local = 3;
                        isSet = true;
                        break;
                    case OP_setlocal:
                    case OP_kill:
                        local = instr.getImmediate();
                        isSet = true;
                        break;
                    default:
                        break;
                    }
                    if (isSet && registerKills.get(local) == true) {
                        debug("  inspecting local " + local + " in block "
                                + originalBlockPos);

                        // Found a killed register set. If all the following
                        // gets before the next set within this block are at
                        // the same stack depth and the stack depth never
                        // drops below the current stack depth then this set
                        // can be replaced with as many dups as their are
                        // gets
                        ArrayList<InstructionID> gets = new ArrayList<InstructionID>();
                        UnderFlowChecker uc = UnderFlowChecker.create(mbinfo);
                        Boolean ok = true, swap = false;
                        int getsBeforeUnderflow = -1;
                        for (int j = i + 1; j < b.getInstructions().size(); j++) {
                            Instruction i2 = b.getInstructions().get(j);
                            // Remember to take into account any modifications
                            // we've queued up otherwise stack depths will
                            // be computed incorrectly.
                            if (replaceNop.contains(new InstructionID(
                                    originalBlockPos, j)))
                                i2 = InstructionFactory.getInstruction(OP_nop);
                            else if (replaceDup.contains(new InstructionID(
                                    originalBlockPos, j)))
                                i2 = InstructionFactory.getInstruction(OP_dup);
                            else if (replaceSwap.contains(new InstructionID(
                                    originalBlockPos, j)))
                                i2 = InstructionFactory.getInstruction(OP_swap);
                            else if (replacePop.contains(new InstructionID(
                                    originalBlockPos, j)))
                                i2 = InstructionFactory.getInstruction(OP_pop);

                            debug("   -- op: "
                                    + Instruction.decodeOp(i2.getOpcode()));

                            uc.visitInstruction(i2);

                            if (uc.underflow()) {
                                debug("  underflow inspecting local " + local
                                        + " in block " + originalBlockPos);

                                if (getsBeforeUnderflow == -1) {
                                    getsBeforeUnderflow = gets.size();
                                    if (gets.size() == 0) {
                                        ok = false;
                                        break;
                                    }
                                } else if (gets.size() > getsBeforeUnderflow) {
                                    ok = false;
                                    break;
                                }
                            }

                            boolean isI2Set = false, isI2Get = false;
                            int l2 = -1;
                            switch (i2.getOpcode()) {
                            case OP_setlocal0:
                                l2 = 0;
                                isI2Set = true;
                                break;
                            case OP_setlocal1:
                                l2 = 1;
                                isI2Set = true;
                                break;
                            case OP_setlocal2:
                                l2 = 2;
                                isI2Set = true;
                                break;
                            case OP_setlocal3:
                                l2 = 3;
                                isI2Set = true;
                                break;
                            case OP_setlocal:
                            case OP_kill:
                                l2 = i2.getImmediate();
                                isI2Set = true;
                                break;

                            case OP_getlocal0:
                                l2 = 0;
                                isI2Get = true;
                                break;
                            case OP_getlocal1:
                                l2 = 1;
                                isI2Get = true;
                                break;
                            case OP_getlocal2:
                                l2 = 2;
                                isI2Get = true;
                                break;
                            case OP_getlocal3:
                                l2 = 3;
                                isI2Get = true;
                                break;
                            case OP_getlocal:
                                l2 = i2.getImmediate();
                                isI2Get = true;
                                break;

                            case OP_hasnext2:
                                debug("  bailing on hasnext2 for local" + local
                                        + " in block " + originalBlockPos);
                                ok = false;
                                break;

                            case OP_inclocal:
                            case OP_declocal:
                            case OP_inclocal_i:
                            case OP_declocal_i:
                                debug("  bailing on inc/dec! for local" + local
                                        + " in block " + originalBlockPos);
                                ok = false;
                                break;
                            default:
                                break;
                            }

                            if (isI2Get)
                                debug("   --     local(" + l2 + ")");

                            if (isI2Get && l2 == local) {
                                if (uc.stackdepth() == 1) {
                                    gets.add(new InstructionID(
                                            originalBlockPos, j));
                                } else if (uc.stackdepth() == 2) {
                                    debug("   -- swap!!");

                                    gets.add(new InstructionID(
                                            originalBlockPos, j));
                                    swap = true;
                                } else {
                                    ok = false;
                                    break;
                                }
                            }

                            if (isI2Set && l2 == local) {
                                break;
                            }
                        }

                        if (gets.size() == 0 && ok) {
                            debug("  replacing get/set: " + gets.size() + " "
                                    + uc.underflow());
                            removedSets++;
                            if (instr.getOpcode() == OP_kill)
                                replaceNop.add(new InstructionID(
                                        originalBlockPos, i));
                            else
                                replacePop.add(new InstructionID(
                                        originalBlockPos, i));
                        } else if (gets.size() > 0 && ok) {

                            // bail on kills followed by gets
                            if (instr.getOpcode() == OP_kill)
                                continue;

                            if (uc.underflow()
                                    && gets.size() > getsBeforeUnderflow) {
                                failedremovals++;
                                continue;
                            }

                            if (swap) {
                                swapCount++;
                                if (gets.size() == 1) {
                                    removedSets++;
                                    debug("  replacing get/set: " + gets.size()
                                            + " with swap");
                                    replaceNop.add(new InstructionID(
                                            originalBlockPos, i));
                                    replaceSwap.add(gets.remove(0));
                                } else {
                                    failedremovals++;
                                }
                                continue;
                            }

                            debug("  replacing get/set: " + gets.size() + " "
                                    + uc.underflow());
                            removedSets++;

                            replaceNop.add(new InstructionID(originalBlockPos,
                                    i));

                            // Last get must be replaced by a NOP
                            replaceNop.add(gets.remove(gets.size() - 1));

                            // All other gets must be replaced by DUPs
                            for (InstructionID getinstr : gets) {
                                replaceDup.add(getinstr);
                            }
                        } else {
                            failedremovals++;
                        }
                    }
                }
            }
            super.visitEnd();
        }
    }

    class _MethodVisitor extends DelegatingMethodVisitor {
        public _MethodVisitor(IMethodVisitor delegate) {
            super(delegate);
        }

        @Override
        public IMethodBodyVisitor visitBody(MethodBodyInfo minfo) {
            return new _MethodBodyVisitor(super.visitBody(minfo), minfo);
        }
    }

    @Override
    public IMethodVisitor visitMethod(MethodInfo minfo) {
        return new _MethodVisitor(super.visitMethod(minfo));
    }

    public static SWF process(SWF inSWF) throws Exception {
        SWFHeader header = inSWF.header;
        SWFRecord[] records = inSWF.records;
        Set<String> symClassEntries = new HashSet<String>();

        ArrayList<SWFRecord> newRecords = new ArrayList<SWFRecord>();

        // transcode
        for (int i = 0; i < records.length; i++) {
            SWFRecord record = records[i];

            if (record.tag == 82) // abc
            {
                int dataOffset = 4;

                while (record.payload[dataOffset] != 0)
                    // skip past flags+name
                    dataOffset++;
                dataOffset++;
                byte[] inABC = new byte[record.payload.length - dataOffset];
                System.arraycopy(record.payload, dataOffset, inABC, 0,
                        record.payload.length - dataOffset);

                ABCEmitter emitter = new ABCEmitter();
                emitter.setAllowBadJumps(true);
                Stackify stackify = new Stackify(emitter);
                new ABCParser(inABC).parseABC(stackify);
                byte[] outABC = emitter.emit();

                ABCLinker.ABCLinkerSettings linkerSettings = new ABCLinker.ABCLinkerSettings();
                linkerSettings.setOptimize(true);
                linkerSettings.setStripDebugOpcodes(true);
                ArrayList<byte[]> abcs = new ArrayList<byte[]>();
                abcs.add(outABC);
                outABC = ABCLinker.linkABC(abcs,
                        ABCConstants.VERSION_ABC_MAJOR_FP10,
                        ABCConstants.VERSION_ABC_MINOR_FP10, linkerSettings);

                byte[] payload = new byte[outABC.length + dataOffset];
                System.arraycopy(record.payload, 0, payload, 0, dataOffset);
                System.arraycopy(outABC, 0, payload, dataOffset, outABC.length);
                newRecords.add(new SWFRecord((short) 82, payload));

                info("swapCount " + stackify.swapCount);
                info("Killed " + stackify.killedRegs);
                info("Liveout " + stackify.liveoutRegs);
                info("Total " + stackify.totalRegs);
                info("Removed " + stackify.removedSets
                        + " redundant set/get pairs ("
                        + stackify.failedremovals + " removals failed)");
            } else {
                newRecords.add(record);
            }
        }

        // build output swf
        return new SWF(header, newRecords.toArray(new SWFRecord[0]));
    }

    public static void main(String[] args) throws IOException, Exception {
        InputStream is = System.in;
        OutputStream os = System.out;

        for (int i = 0; i < args.length; i++) {
            if (args[i].equals("-o"))
                os = new FileOutputStream(new File(args[++i]));
            else
                is = new FileInputStream(new File(args[i]));
        }

        // read input SWF
        SWF swf = SWF.read(is);
        is.close();

        // destructive to input swf!
        swf = Stackify.process(swf);
        swf.write(os, true).close();
    }
}
