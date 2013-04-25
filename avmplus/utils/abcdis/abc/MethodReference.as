/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {
    import abc.Instruction
    import abc.Types.Trait;
    import abc.Types.ABCObject;
    import abc.Types.ScriptInfo;
    import abc.Types.InstanceInfo;
    import abc.Types.ClassInfo;
    
    
    public class MethodReference
    {
        static public const SCRIPT_INIT : uint = 0;
        static public const SCRIPT_METHOD : uint = 1;
        static public const INSTANCE_INIT : uint = 2;
        static public const INSTANCE_METHOD : uint = 3;
        static public const CLASS_INIT : uint = 4;
        static public const CLASS_METHOD : uint = 5;
        static public const FUNCTION_LITERAL : uint = 6;
    
        public function MethodReference(kind : uint, referer:Object, context : ABCObject = null)
        {
            this.kind = kind;
            this.referer = referer;
            this.context = context;
        }
        
        public var kind : uint;
        public var referer : Object;
        public var context : ABCObject;
        
        public function toString() : String
        {
            var si : ScriptInfo;
            var scriptID : int;
            var t : Trait;
            var ii : InstanceInfo;
            var ci : ClassInfo;
            var instr : Instruction;
            switch (kind)
            {
            case SCRIPT_INIT:
                si = referer as ScriptInfo;
                scriptID = si.abcfile.scriptInfos.indexOf(si);
                return "Script " + scriptID.toString() + " init";
            case SCRIPT_METHOD:
                si = ScriptInfo(referer);
                t = Trait(context);
                scriptID = si.abcfile.scriptInfos.indexOf(si);
                return "Script " + scriptID.toString() + "::" + t.multiname.toString();
            case INSTANCE_INIT:
                ii = InstanceInfo(referer);
                return ii.nameString + "::" + ii.nameString;
            case INSTANCE_METHOD:
                ii = InstanceInfo(referer);
                t = Trait(context);
                return ii.nameString + "::" + t.multiname.toString();
            case CLASS_INIT:
                ci = ClassInfo(referer);
                return ci.derivedName + "::" + ci.derivedName;
            case CLASS_METHOD:
                ci = ClassInfo(referer);
                t = Trait(context);
                return ci.derivedName + "::" + t.multiname.toString();
            case FUNCTION_LITERAL:
                instr = Instruction(referer);
                if (instr.getState())
                    return "function literal in " + instr.getState().mb.derivedName;
                else
                    return ""
            }
            return "";
        }
    }

}