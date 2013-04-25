/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc.Types {

    import flash.utils.ByteArray
    import flash.utils.Endian
    
    import abc.Reader
    import abc.Instruction
    import abc.BasicBlock
    import abc.Verifier
    import abc.Types.ABCObject
    import abc.Types.ABCFile

    public final class MethodBody extends ABCObject {
        public var method_info:int
        public var max_stack:int
        public var max_regs:int
        public var scope_depth:int
        public var max_scope:int
        public var code_length:int
        public var code:ByteArray
        public var exceptions:Vector.<Exception> = new Vector.<Exception>
        public var activation_traits:Traits
        
        public var instrs:Vector.<Instruction>
        public var blocks:Vector.<BasicBlock>
        
        public var code_offset:int

        public function MethodBody(abcfile:ABCFile) {
            super(abcfile)
        }
        
        public function get methodinfo():MethodInfo {
            return abcfile.getMethodInfo(method_info)
        }
        
        public function get derivedName() : String
        {
            return methodinfo.derivedName
        }
        
        public static function findBasicBlockWithStartOffset(offset : uint, blocks : Vector.<BasicBlock>) : BasicBlock
        {
            for each ( var bb : BasicBlock in blocks )
            {
                if (bb.getInstrs()[0].getOffset() == offset)
                    return bb;
            }
            return null;
        }
        
        public function findBasicBlockWithStartOffset(offset : uint) : BasicBlock
        {
            return MethodBody.findBasicBlockWithStartOffset(offset, blocks);
        }
    }
}
