/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {

    import flash.utils.ByteArray
    import Utils;
    import abc.FrameState
    import abc.Types.MethodBody
    
    public final class Instruction {
        private var offset:uint
        private var size:uint
        private var opcode:int
        private var imms:Vector.<int> = new Vector.<int>
        private var block : BasicBlock
        
        private var state:FrameState = null
        
        public function Instruction() {
        }
        
        public function getOffset():uint {
            return offset
        }
        
        public function getOpcode():uint {
            return opcode
        }
        
        public function getOpcodeName():String {
            return Constants.opNames[opcode]
        }
        
        public function getImmString():String {
            return (imms.length == 0 ? "" : " [" + imms.join(", ") + "]")
        }
        
        public function getSize():uint {
            return size
        }
        
        public function getImm(i:uint):int {
            return imms[i]
        }
        
        public function getImms() : Vector.<int>
        {
            return imms;
        }
        
        public function setState(s:FrameState):void {
            state = s.clone()
        }
        
        public function getState():FrameState {
            return state
        }
        
        public function getVerifyError():String {
            return (state == null ? null : state.verifyError)
        }
        
        public function setBasicBlock(b : BasicBlock) : void
        {
            block = b;
        }
        
        public function getBasicBlock() : BasicBlock
        {
            return block
        }
        
        public function get isJump() : Boolean
        {
            switch(opcode) {
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
                case Constants.OP_jump:
                    return true;
                default:
                    return false;
            }
        }
        
        public function get isBranch():Boolean {
            return isJump || (opcode == Constants.OP_lookupswitch);
        }
        
        public function get jumpTarget() : BasicBlock
        {
            if (!isJump)
                throw new Error("Can only compute jump target for jump instructions!");
            if (!block)
                throw new Error("Can only compute jump target after the control graph has been computed!")
            return MethodBody.findBasicBlockWithStartOffset(getOffset() + getSize() + getImm(0), block.getSuccs())
        }
        
        public function get isTerminator():Boolean {
            switch(opcode) {
                case Constants.OP_throw:
                case Constants.OP_returnvalue:
                case Constants.OP_returnvoid:
                    return true;
                default:
                    return false;
            }
        }
        
        public function get hasName() : Boolean
        {
            return Constants.hasName.indexOf(opcode) != -1;
        }
        
        public static function read(data:ByteArray):Instruction {
            var i:Instruction = new Instruction()
            
            i.offset = data.position
            i.opcode = data.readUnsignedByte()
            
            if(Constants.singleU32Imm.indexOf(i.opcode) >= 0) {
                i.imms.push(Reader.readU32(data))
            } else if(Constants.doubleU32Imm.indexOf(i.opcode) >= 0) {
                i.imms.push(Reader.readU32(data))
                i.imms.push(Reader.readU32(data))
            } else if(Constants.singleS24Imm.indexOf(i.opcode) >= 0) {
                i.imms.push(Reader.readS24(data))
            } else if(Constants.singleByteImm.indexOf(i.opcode) >= 0) {
                i.imms.push(data.readUnsignedByte())
            } else if(i.opcode == Constants.OP_debug) {
                i.imms.push(data.readUnsignedByte())
                i.imms.push(Reader.readU32(data))
                i.imms.push(data.readUnsignedByte())
                i.imms.push(Reader.readU32(data))
            } else if(i.opcode == Constants.OP_lookupswitch) {
                i.imms.push(Reader.readS24(data))
                var maxindex:int = Reader.readU32(data)
                i.imms.push(maxindex)
                while(maxindex-- >= 0)
                    i.imms.push(Reader.readS24(data))
            }
            
            i.size = data.position - i.offset
            
            return i
        }
        
        public static function readAll(data:ByteArray):Vector.<Instruction> {
            var instrs:Vector.<Instruction> = new Vector.<Instruction>()
            
            while(data.position < data.length)
                instrs.push(Instruction.read(data))
            
            return instrs
        }
        
        public function toString():String {
            return Utils.padString(offset + "", 8) + getOpcodeName() + getImmString()
        }
    }
}