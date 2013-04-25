/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {

    import flash.utils.ByteArray
    
    import Utils;
    import abc.FrameState
    import abc.Types.Exception
    
    public final class BasicBlock {
        private var preds:Vector.<BasicBlock> = new Vector.<BasicBlock>()
        private var succs:Vector.<BasicBlock> = new Vector.<BasicBlock>()
        private var instrs:Vector.<Instruction> = new Vector.<Instruction>()
        
        public var state:FrameState = null
        public var entryState:FrameState = null

        public function BasicBlock() {
        }
        
        public function propagateFrameState(fs:FrameState):Boolean {
            if(entryState == null) {
                entryState = fs.clone()
            } else if(! entryState.checkEquals(fs)) {
                if(state == null) {
                    state = new FrameState(fs.mb)
                }
                state.verifyError = "framestate mismatch"
                return false
            }
            return true
        }
        
        public function getPreds():Vector.<BasicBlock> {
            return preds
        }
        
        public function getSuccs():Vector.<BasicBlock> {
            return succs
        }
        
        public function getInstrs():Vector.<Instruction> {
            return instrs
        }
        
        public function getLastInstr():Instruction {
            return instrs[instrs.length-1]
        }
        
        private function addPred(bb:BasicBlock):void {
            if(preds.indexOf(bb) < 0) {
                preds.push(bb)
                bb.addSucc(this)
            }
        }
        
        private function addSucc(bb:BasicBlock):void {
            if(succs.indexOf(bb) < 0) {
                succs.push(bb)
                bb.addPred(this)
            }
        }
        
        public function getPredIds(blockList:Vector.<BasicBlock>):Array {
            var xs:Array = new Array()
            for each(var bb:BasicBlock in preds)
                xs.push(blockList.indexOf(bb))
            return xs
        }
        
        public function getSuccIds(blockList:Vector.<BasicBlock>):Array {
            var xs:Array = new Array()
            for each(var bb:BasicBlock in succs)
                xs.push(blockList.indexOf(bb))
            return xs
        }
        
        private static function addJumpTarget(targets:Array, jt:uint, js:uint):void {
            if(!(targets[jt]))
                targets[jt] = new Array();
            targets[jt].push(js)
        }
        
        public static function computeCFG(instrs:Vector.<Instruction>, exceptions:Vector.<Exception>):Vector.<BasicBlock> {
            var jumpTargetMap : Array = new Array()
            for(var i:int=0; i<instrs.length; i++) {
                var prev:Instruction = i-1 >= 0 ? instrs[i-1] : null
                var cur:Instruction = instrs[i]
                var next:Instruction = i+1 < instrs.length ? instrs[i+1] : null
                switch(cur.getOpcode()) {
                    case Constants.OP_ifnlt:
                    case Constants.OP_ifnle:
                    case Constants.OP_ifngt:
                    case Constants.OP_ifnge:
                    case Constants.OP_iftrue:
                    case Constants.OP_iffalse:
                    case Constants.OP_ifeq:
                    case Constants.OP_ifne:
                    case Constants.OP_iflt:
                    case Constants.OP_ifle:
                    case Constants.OP_ifgt:
                    case Constants.OP_ifge:
                    case Constants.OP_ifstricteq:
                    case Constants.OP_ifstrictne:
                        addJumpTarget(jumpTargetMap, next.getOffset(), cur.getOffset())
                        addJumpTarget(jumpTargetMap, next.getOffset() + cur.getImm(0), cur.getOffset())
                        break
                    case Constants.OP_jump:
                        if(next) {
                            // make sure we start a new bb after an unconditional jump
                            if(jumpTargetMap[next.getOffset()] == null)
                                jumpTargetMap[next.getOffset()] = []
                                
                            addJumpTarget(jumpTargetMap, next.getOffset() + cur.getImm(0), cur.getOffset())
                        } else {
                            addJumpTarget(jumpTargetMap, cur.getOffset() + cur.getImm(0), cur.getOffset())
                        }
                        break
                    case Constants.OP_throw:
                        // make sure we start a new bb after a throw
                        if(next != null && jumpTargetMap[next.getOffset()] == null)
                            jumpTargetMap[next.getOffset()] = []
                        break
                    case Constants.OP_lookupswitch:
                        addJumpTarget(jumpTargetMap, cur.getOffset() + cur.getImm(0), cur.getOffset())

                        var n:int = cur.getImm(1)
                        for(var j:int=0; j<=n; j++)
                            addJumpTarget(jumpTargetMap, cur.getOffset() + cur.getImm(2 + j), cur.getOffset())
                        break
                }
            }
            
            // sort the jump targets and remove any duplicates. Inserting
            // pseudo targets -1 and +inf make the splitting logic easier.
            
            var jumpTargets : Array = [-1, Number.POSITIVE_INFINITY]
            // for in does not seem to work reliably on sparse arrays,
            // so we have to use a regular for loop here.
            for(var jt:uint = 0; jt < jumpTargetMap.length; ++jt) {
                if (jumpTargetMap[jt])
                    jumpTargets.push(jt)
            }
            
            for each (var exception : Exception in exceptions)
            {
                jumpTargets.push(exception.start)
                jumpTargets.push(exception.end)
                jumpTargets.push(exception.target)
            }
            jumpTargets = Utils.sortAndRemoveDuplicates(jumpTargets)
            
            // maintain a mapping of instruction offset to basic block
            var blockMap : Array = new Array()
            var blockList:Vector.<BasicBlock> = new Vector.<BasicBlock>()
            var currentBlock:BasicBlock = null

            // each instruction ends up in one (and only one) basic block
            // during this process we must handle "fall-through" edges
            // as they aren't represented by the explicit jump targets
            for each(var instr:Instruction in instrs) {
                if(instr.getOffset() < jumpTargets[0]) {
                    instr.setBasicBlock(currentBlock)
                    currentBlock.instrs.push(instr)
                } else {
                    var newBlock : BasicBlock = new BasicBlock()
                    newBlock.instrs.push(instr)
                    instr.setBasicBlock(newBlock)
                    jumpTargets.shift()
                    
                    if(currentBlock != null) {
                        blockList.push(currentBlock)
                        if((!currentBlock.getLastInstr().isBranch) && (!currentBlock.getLastInstr().isTerminator))
                            newBlock.addPred(currentBlock)
                    }
                    currentBlock = newBlock
                }
                
                blockMap[instr.getOffset()] = currentBlock
            }
            
            if(currentBlock != null)
                blockList.push(currentBlock)
            
            // We stored the jump source for each jump target in the first pass.
            // We use that information here to connect the blocks
            for each(var bb:BasicBlock in blockList) {
                var preds:Array = jumpTargetMap[bb.getInstrs()[0].getOffset()]
                for each(var p : int in preds) {
                    bb.addPred(blockMap[p])
                }
            }
             
            return blockList
        }
    }
}
