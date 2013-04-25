/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {
    
    import flash.utils.ByteArray
    import flash.utils.Endian
    
    import Utils
    import abc.Reader
    //import abc.IWriter
    import abc.Types.Pool
    import abc.Types.ABCFile
    import abc.Types.ConstantNamespace
    import abc.Types.ConstantMultiname
    import abc.Types.ConstantNamespaceSet
    import abc.Types.Exception
    import abc.Types.Trait
    import abc.Types.Traits
    import abc.Types.ValueKind
    import abc.Types.MethodInfo
    import abc.Types.MethodBody
    import abc.Types.MetadataInfo
    import abc.Types.ClassInfo
    import abc.Types.ScriptInfo
    import abc.Types.InstanceInfo
    
    public final class DOTWriter /*implements IWriter*/ {
        private var abcfile:ABCFile
        private var file:String
        
        public function DOTWriter(abcfile:ABCFile, file:String) {
            this.abcfile = abcfile
            this.file = file
        }

        public static function writeFile(abcfile:ABCFile, file:String) : void {
            var xw:DOTWriter = new DOTWriter(abcfile, file)
            xw.write()
        }
        
        public function write():void {
            var dot2pdf:String = ""
            var mbs : Array = abcfile.getMethodBodies()
            for(var i:int; i<mbs.length; i++) {
                dot2pdf += "dot -Tpdf "+file+"-mb-"+i+".dot -o "+file+"-mb-"+i+".pdf\n"
                Utils.writeStringToFile(writeMethodBody(mbs[i]), file+"-mb-" + i + ".dot")
            }
            Utils.writeStringToFile(dot2pdf, file+"-mb-genpdfs")
        }
        
        public function writeMethodBody(mb:MethodBody):String {
            var s:String = "digraph methodbody_" + mb.method_info + " {\n"
            var j : int;
            for(var i:int =0; i<mb.blocks.length; i++) {
                var next:BasicBlock = (i+1 < mb.blocks.length) ? mb.blocks[i+1] : null
                
                for each(j in mb.blocks[i].getSuccIds(mb.blocks)) {
                    s += "    bb"+i+" -> bb" + j + (next == null ? "" : (mb.blocks[j] == next ? " [style=dashed]" : "")) + "\n"
                }

                s += "    bb"+i+" [shape=none, label=<<table bgcolor='white' border='1' cellborder='0' cellspacing='0' cellpadding='5'>"
                s += "<tr><td bgcolor='black' align='left'><font color='white'>bb"+i+"</font></td><td bgcolor='black' align='left'><font color='white'>Stack</font></td></tr>\n"
                for(j=0; j<mb.blocks[i].getInstrs().length; j++) {
                    var inst:Instruction = mb.blocks[i].getInstrs()[j]
                    s += "<tr><td align='left'><font face='monaco'>"+inst+"</font></td><td>"+(inst.getState() ==null ? "" : inst.getState().stackDepth)+"</td></tr>\n"
                }
                s += "</table>>];\n"
            }
            return s + "}"
        }
    }
}