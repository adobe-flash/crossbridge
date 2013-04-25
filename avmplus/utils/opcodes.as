/* -*- indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Compile command:
// java -jar utils/asc.jar -d -m -i -AS3 -strict -import generated/builtin.abc -import generated/shell_toplevel.abc utils/opcodes.as 
 
 
package {
import avmplus.System;
import avmplus.File;

final class OpCode
{
    public function OpCode(isUsed : Boolean, opCode : uint, operandCount : int, canThrow : Boolean, stackMovement : int, name : String, internalOnly : Boolean)
    {
        m_isUsed = isUsed;
        m_opCode = opCode;
        m_operandCount = operandCount;
        m_canThrow = canThrow;
        m_stackMovement = stackMovement;
        m_name = name;
        m_internalOnly = internalOnly;
    }
    
    public function get used() : Boolean { return m_isUsed; }
    public function get opCode() : uint { return m_opCode; }
    public function get operandCount() : int { return m_operandCount; }
    public function get canThrow() : Boolean { return m_canThrow; }
    public function get stackMovement() : int { return m_stackMovement; }
    public function get name() : String { return m_name; }
    public function get internalOnly() : Boolean { return m_internalOnly; }

    private var m_isUsed : Boolean;
    private var m_opCode : uint;
    private var m_operandCount : int;
    private var m_canThrow : Boolean;
    private var m_stackMovement : int;
    private var m_name : String;
    private var m_internalOnly : Boolean;
    
    private static const s_hexRegEx : RegExp = /0x[0-9a-fA-F]{2}/;
}

final class OpCodes
{
    public function OpCodes(opCodes : Array)
    {
        throw new Error("Don't consruct me!!!");
    }
    
    public static function parse(opCodesText : String) : Array
    {
        var abcOpCodeRegEx : RegExp = /^\w*ABC_(?P<u>(UNUSED_)?)OP(?:_F)?\(\s*(?P<oc>-?[0-9]+)\s*,\s*(?P<ct>[01])\s*,\s*(?P<sm>-?[0-9]+)\s*,\s*(?P<internal>[01])\s*,\s*(?P<n>[\S\)]+)\s*\)/gm
        var opCodeValue : uint = 0;
        var match : Object = abcOpCodeRegEx.exec(opCodesText);
        var opCodes : Array = new Array(256);
        while ((match != null) && (opCodeValue < 256)){
            var opCode = new OpCode(match.u.length == 0, opCodeValue, int(match.oc), match.ct == "1", int(match.sm), match.n, match.internal == "1");
            opCodes[opCodeValue] = opCode;
            ++opCodeValue;
            match = abcOpCodeRegEx.exec(opCodesText)
        }
        
        if (opCodeValue == 256) {
            return opCodes;
        }
        return null;
    }
}

class OpCodeOutput
{
    public function OpCodeOutput(fileName : String)
    {
        this.m_fileName = fileName;
    }
    
    public function output(opCodes : Array) : void { throw new Error("Call to abstract method!!!"); }
    
    protected function write(s : String) : void
    {
        File.write(m_fileName, s);
    }
    
    private static const copyright : String = <x><![CDATA[
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
]]></x>.text()[0];
    
    protected function wrapInCommentForCLikeLanguages(header : String, body : String, footer : String) : String
    {
        var lines : Array = body.split("\n");
        while (lines[0] == "")
            lines.shift();
        lines = lines.map(function (line : *) : * { return (line.length > 0 ? " * " : " *") + line; });
        
        return "/*" 
            + ((header != null) ? (" ***** " + header + " *****") : "") + "\n"
            + lines.join("\n")
            + "\n" + ((footer != null) ? (" * ***** " + footer + " *****") : "" ) + " */\n\n";
    }
    
    protected function get copyrightCommentForCLikeLanguages() : String
    {
        if (m_copyrightCommentForCLikeLanguages != null)
            return m_copyrightCommentForCLikeLanguages;
        m_copyrightCommentForCLikeLanguages = wrapInCommentForCLikeLanguages("", copyright, "");
        return m_copyrightCommentForCLikeLanguages;
    }
    
    protected function get generatedFileCommentForCLikeLanguages() : String
    {
        if (m_generatedFileCommentForCLikeLanguages != null)
            return m_generatedFileCommentForCLikeLanguages;
        var slashLine : String = "//////////////////////////////////////////////////////////////////////////////////\n";
        var str : String = slashLine;
        str += "// GENERATED FILE: DO NOT EDIT!!! See utils/opcodes.as in the tamarin repository.\n"
        str += slashLine;
        str += "\n";
        m_generatedFileCommentForCLikeLanguages = str;
        return str;
    }
    
    private static var m_copyrightCommentForCLikeLanguages : String = null;
    private static var m_generatedFileCommentForCLikeLanguages : String = null;
    private var m_fileName : String;
}

class JavaOutput extends OpCodeOutput
{
    public function JavaOutput(fileName : String)
    {
        super(fileName);
    }
    
    public override function output(opCodes : Array) : void
    {
        var str : String = this.copyrightCommentForCLikeLanguages;
        str += this.generatedFileCommentForCLikeLanguages;
        str += "package adobe.abc;\n\n"
        str += "public interface ABCOpCodes {\n"
        for each (var opCode : OpCode in opCodes) {
            if (opCode.used) {
                var opCodeHex : String = opCode.opCode.toString(16);
                opCodeHex = opCodeHex.length == 1 ? "0" + opCodeHex : opCodeHex;
                str += "    public static final int OP_" + opCode.name + " = 0x" + opCodeHex + ";\n";
            }
        }
        
        str += "    public static final int operandCountTable[] = new int [] {\n";
        for each (var opCode : OpCode in opCodes) {
            str += "        " + opCode.operandCount.toString(10) + ",\n" ;
        } 
        str += "    };\n"
        
        str += "    public static final String opNames[] = new String [] {\n";
        for each (var opCode : OpCode in opCodes) {
            str += "        \"" + opCode.name + "\",\n" ;
        } 
        str += "    };\n"
        
        
        str += "}\n";
        write(str);
    }
    
    public static const ext = "java";
    
}

class AS3Output extends OpCodeOutput
{
    public function AS3Output(fileName : String)
    {
        super(fileName);
    }
    
    private static function padOpName(opName : String, minLength : uint) : String
    {
        while (opName.length < minLength) {
            opName = opName + " ";
        }
        return opName;
    }
    
    public override function output(opCodes : Array) : void
    {
        var str : String = this.copyrightCommentForCLikeLanguages;
        str += this.generatedFileCommentForCLikeLanguages;
        str += "// This file is to be included by its clients.\n\n"
        for each (var opCode : OpCode in opCodes) {
            if ((opCode.used) && (!opCode.internalOnly)) {
                var opCodeHex : String = opCode.opCode.toString(16);
                opCodeHex = opCodeHex.length == 1 ? "0" + opCodeHex : opCodeHex;
                str += "const OP_" + opCode.name + " : int = 0x" + opCodeHex + ";\n";
            }
        }
        
        var opNames : Array = new Array(256);
        var maxOpNameLen : uint = 0;
        for each (var opCode : OpCode in opCodes) {
            var opName : String = opCode.used ? opCode.name : "OP_" + opCode.name;
            if (opCode.internalOnly) {
                opName = "OP_0x" + opCode.opCode.toString(16).toUpperCase();
            }
            opNames[opCode.opCode] = opName;
            maxOpNameLen = Math.max(maxOpNameLen, opName.length);
        }
        
        str += "const opNames : Array = [\n";
        for each (var opName : String in opNames) {
            str += "    \"" + padOpName(opName, maxOpNameLen) + "\",\n" ;
        } 
        str += "];\n"
        
        
        write(str);
    }
    
    public static const ext = "as";
}

class TextTable
{
    public function TextTable(nCols : uint, minPadding : uint)
    {
        m_nCols = nCols;
        m_minPadding = minPadding;
        m_rows = new Array();
    }
    
    public function addRow(r : Array)
    {
        if (r.length != m_nCols)
            throw new Error("Invalid row");
        m_rows.push(r);
    }
    
    private static function padString(s : String, minLength : uint) : String
    {
        while (s.length < minLength)
            s += " ";
        return s;
    }
    
    private function paddedRowStrings() : Array
    {
        var colWidths : Array = new Array(m_nCols);
        
        for ( var i : uint = 0; i < m_nCols; ++i) {
            colWidths[i] = 0;
        }
        
        for each ( var r : Array in m_rows ) {
            for ( var i : uint = 0; i < r.length; ++i )
                colWidths[i] = Math.max(colWidths[i], r[i].length + m_minPadding); 
        }
        
        var result : Array = new Array();
        for each ( var r : Array in m_rows ) {
            var rowStr : String = "";
            for ( var i : uint = 0; i < r.length; ++i ) {
                rowStr += padString(r[i], colWidths[i]); 
            }
            result.push(rowStr);
        }
        return result;
    }
    
    public function toString() : String
    {
        return paddedRowStrings().join("\n");
    }
    
    private var m_nCols : uint;
    private var m_minPadding : uint;
    private var m_rows : Array;
}

class TblOutput extends OpCodeOutput
{
    public function TblOutput(fileName : String)
    {
        super(fileName);
    }
    
    private static function padOpName(opName : String, minLength : uint) : String
    {
        while (opName.length < minLength) {
            opName = opName + " ";
        }
        return opName;
    }
    
    public override function output(opCodes : Array) : void
    {
        var str : String = this.copyrightCommentForCLikeLanguages;
        var comment : String = <x><![CDATA[
Includers must define an ABC_OP and ABC_UNUSED_OP macros of the following form:

  #define ABC_OP(opCount, throw, stack, internal, name) ...          // defines regular op code
  #define ABC_UNUSED_OP(opCount, throw, stack, internal, name) ...  // defines unused op code

Selected arguments can then be used within the macro expansions.
- opCount        Number of operands.  Uses -1 for "invalid", we can avoid that if necessary
- throw          1 if the operation can throw, 0 otherwise
- stack          Stack movement not taking into account run-time names or function arguments
- internal       1 if the operation is internal to the VM, 0 otherwise
- name           Unquoted name of operation.

There MUST be exactly 256 entries in this table.
The location of an opcode definition in this table determines its hex value, which is
what must correspond to what is read out of the ABC file.
Entries should never be added or removed from this table.  New abc instructions may be added
by updating entries for unused op codes.  Unused op codes have a "name" that is a hex value.
]]></x>.text()[0];
        
        str += wrapInCommentForCLikeLanguages(null, comment, null);
        str += "//--------------------------------------------------------------------------------------------------------\n";
        
        var table : TextTable = new TextTable(7, 2);
        table.addRow(["//", "opCount", "throw", "stack", "internal", "name", "hex"]);
        for each (var opCode : OpCode in opCodes) {
            var abcOP : String = opCode.used ? "ABC_OP(" : "ABC_UNUSED_OP(";
            var operandCount : String = opCode.operandCount.toString(10) + ",";
            var throwStr : String = (opCode.canThrow ? "1" : "0") + ",";
            var stack : String = opCode.stackMovement.toString(10) + ",";
            var internalOnly : String = (opCode.internalOnly ? "1" : "0") + ",";
            var name : String = opCode.name + ")";
            var hex : String = "// 0x" + ((opCode.opCode < 16) ? "0" : "") + opCode.opCode.toString(16).toUpperCase();
            table.addRow([abcOP, operandCount, throwStr, stack, internalOnly, name, hex]);
        }
        
        str += table.toString() + "\n";
        write(str);
    }
    
    public static const ext = "tbl";
}

var outputsArr : Array = [ JavaOutput, AS3Output, TblOutput ];
var outputs : Object = {}
for each (var output : * in outputsArr)
    outputs[output.ext] = output;

function usage() : void 
{
    var supportedOutputFileStrings : Array = outputsArr.map(function (output : *) { return "outputfile." + output.ext; });
    var supportedOutputsStr : String = supportedOutputFileStrings.join(" | ");
    print("usage: opcodes opcodes.tbl (" + supportedOutputsStr +") [[" + supportedOutputsStr +"] ...]");
}

function main() : void {
    if (System.argv.length < 1) {
        print("You need to specify the opcodes.tbl file");
    }

    if (System.argv.length < 2) {
        print("You need to specify at least one output file");
        usage();
        System.exit(1);
    }

    var opCodesFileName : String = System.argv[0]
    if (!File.exists(opCodesFileName)) {
        print("No such file:" + opCodesFileName);
        System.exit(1);
    }

    var opCodesText : String = File.read(opCodesFileName);

    var opCodes : Array = OpCodes.parse(opCodesText);
    if (opCodes == null) {
        print("Unable to parse opcodes table!");
        System.exit(1);
    }

    for (var i : Number = 1; i < System.argv.length; ++i) {
        var outputFileName : String = System.argv[i];
        var dotIndex : int = outputFileName.lastIndexOf(".");
        if (dotIndex != -1) {
            var ext : String = outputFileName.slice(dotIndex + 1).toLowerCase();
            if (ext in outputs) {
                var output : * = outputs[ext];
                var o : OpCodeOutput = new output(outputFileName);
                o.output(opCodes);
            }
            else {
                print("Output type for \"" + outputFileName + "\", " + ext + " is not supported\n");
                System.exit(1);
            }
        }

    }
}

main();

}
