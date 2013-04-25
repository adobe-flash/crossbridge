/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package
{
    import avmplus.System;
    Main.main(System.argv)
}

import abc.Reader
import abc.Types.ABCFile
import abc.Types.ConstantMultiname
import abc.Types.ConstantNamespace
import abc.Types.ScriptInfo
import abc.Types.Trait
import abc.Types.Pool
import abc.Instruction
import abc.Types.MethodBody
import SWF.SWFReader
import SWF.SWF;
import SWF.Tag;
import abc.IReader
import abc.ABCReader
import abc.AbcAPIWriter
import abc.Verifier
import IPrinter
import TablePrinter
import avmplus.File
import abc.AbcDumpWriter
import flash.utils.ByteArray
import SWC.SWCReader;

class UsageError extends Error
{
    public function UsageError(err : String)
    {
        super(err);
    }
}

final class TracePrinter implements IPrinter
{
    public function println(s: String) : void
    {
        trace(s)
    }
}

final class CommentPrinter implements IPrinter
{
    public function CommentPrinter(delegate : IPrinter)
    {
        this.delegate = delegate;
    }
    
    private var delegate : IPrinter;
    
    public function println(s: String) : void
    {
        if (s.length > 0)
            delegate.println("// " + s)
        else
            delegate.println("")
    }
}

final class InstructionCountAndSize
{
    public function InstructionCountAndSize(opCode : uint)
    {
        this.opCode = opCode;
    }
    
    public function toString() : String
    {
        return opCode.toString() + ":" + count.toString() + ":" + accumulatedSize.toString();
    }
    
    public var opCode : uint;
    public var count : uint;
    public var accumulatedSize : uint;
}

final class FilteredAbcDumpWriter extends AbcDumpWriter
{
    public function FilteredAbcDumpWriter(abc:ABCFile, scriptFilter : Function = null)
    {
        super(abc)
        this.scriptFilter = scriptFilter != null ? scriptFilter : defaultScriptFilter;
    }
    
    private var scriptFilter : Function;
    
    protected override function traverseScript(id : uint, scriptInfo : ScriptInfo) : void
    {
        if (scriptFilter(scriptInfo))
            super.traverseScript(id, scriptInfo)
    }
    
    private function defaultScriptFilter(si : ScriptInfo) : Boolean
    {
        return true;
    }
}

final class ScriptFilter
{
    public function ScriptFilter(filterStr : String)
    {
        if (filterStr != null)
            regEx = new RegExp(filterStr);
    }
    
    private var regEx : RegExp;
    
    public function filterABC(abc : ABCFile) : Boolean
    {
        if (regEx == null)
            return true;
        return applyFilterRegExpToABC(regEx, abc)
    }
    
    public function filterScript(si : ScriptInfo) : Boolean
    {
        if (regEx == null)
            return true;
        return applyFilterRegExpToScript(regEx, si);
    }
    
    private static function applyFilterRegExpToName(filterRegEx : RegExp, name : ConstantMultiname) : Boolean
    {
        var abcConstants : Class = abc.Constants;
        var validKind : Boolean = (name.kind == abcConstants.CONSTANT_Qname) || ((name.kind == abcConstants.CONSTANT_Multiname) && (name.nsSet.namespaces.length == 1));
        if (!validKind)
            return false;
        
        var ns : ConstantNamespace = name.kind == abcConstants.CONSTANT_Qname ? name.ns : (name.nsSet.constantNamespaces[0])
        if ((ns.kind != abcConstants.CONSTANT_PackageNs) && (ns.kind != abcConstants.CONSTANT_PackageInternalNs))
            return false;
        
        var packageStr : String = ns.uriString;
        packageStr = packageStr.length == 0 ? "" : (packageStr + ".");
        var qName : String = packageStr + name.nameString
        return filterRegEx.exec(qName) != null;
    }
    
    private static function applyFilterRegExpToScript(filterRegEx : RegExp, si : ScriptInfo) : Boolean
    {
        for each (var t : Trait in si.traits.ts)
        {
            if (applyFilterRegExpToName(filterRegEx, t.multiname))
                return true;
        }
        return false;
    }
    
    private static function applyFilterRegExpToABC(filterRegEx : RegExp, a : ABCFile) : Boolean
    {
        for (var i : uint ; i < a.scriptInfos.getNumItems(); ++i)
        {
            var si : ScriptInfo = ScriptInfo(a.scriptInfos.get(i));
            if (applyFilterRegExpToScript(filterRegEx, si))
                return true;
        }
        return false;
    }
}

final class Main
{
    private static const usage : String = <x><![CDATA[
Displays the contents of abc or swf files

Usage:

    abcdump [-i] [-pools] file ...
    abcdump --decompress-only file ...
    abcdump -a file ...
    abcdump -api file ...
    abcdump -abs file ...

Each file can be an ABC or SWF/SWC format file

Options:

 -a   Extract the ABC blocks from the SWF/SWC, but do not
      otherwise display their contents.  The file names are
      of the form file<n>.abc where "file" is the name
      of the input file minus the .swf/.swc extension;
      and <n> is omitted if it is 0.

 -i   Print information about the ABC, but do not dump the byte code.

 -abs Print the bytecode, but no information about the ABC
 -api Print the public API exposed by this abc/swf
 -pools Print out the contents of the constant pools
 --decompress-only Write out a decompressed version of the swc and exit
]]></x>.text()[0];

    private static function help(err : String) : void
    {
        throw new UsageError(err);
    }

    // --decompress-only dominates all other options and args
    // -a is next dominator.
    // -api -i is a NOP
    // -abs -i is a NOP
    // -api  dominates -abs and -pools
    // -pools is modifier
    
    // command opts:
    // --decompress-only -> decompress SWF
    // -a -> extract abc
    // -api -> api dump
    // -i -> print only SWF info
    //   -pools -> print pools as well
    // -abs -> print asm only
    // [default] print info, asm
    //   -pools -> print pools as well
    

    private static var commands : Object = { "-a"                   : { cmd : Main.extractABC, opts : [], command : "-a" },
                                             "-abs"                 : { cmd : Main.dumpABS, opts : ["-filter="], command : "-abs" },
                                             "-api"                 : { cmd : Main.dumpAPI, opts : [], command : "-api" },
                                             "-i"                   : { cmd : Main.dumpInfo, opts : ["-pools"], command : "-i"},
                                             "--decompress-only"    : { cmd : Main.decompress, opts : [], command : "--decompress-only" },
                                             ""                     : { cmd : Main.defaultDump, opts : ["-pools", "-filter="] }
                                              }

    public static function main(argv : Array) : void
    {
        try
        {
            var commandDict : Object = null;
            var optsDict : Object = new Object();
            var args : Array = new Array();
        
            for each (var arg : String in argv)
            {
                if (arg.charAt(0) == "-")
                {
                    if (args.length != 0)
                        help("Options must occur before arguments: " + arg)
                    
                    var newCommandDict : * = commands[arg]
                    if (newCommandDict)
                    {
                        if (commandDict)
                            help("Incompatible options: " + commandDict.command + " and " + arg)
                        commandDict = newCommandDict
                    }
                    else
                    {
                        var optionName : String;
                        var optionValue : String = null;
                        var eqIndexInArg : int = arg.indexOf("=")
                        
                        if (eqIndexInArg == -1)
                            optionName = arg
                        else
                        {
                            optionName = arg.substring(0, eqIndexInArg + 1)
                            optionValue = arg.substring(eqIndexInArg + 1)
                        }
                        if (optionName in optsDict)
                            help("Duplicate option: " + arg)
                        if (!optionValue)
                        {
                            optsDict[arg] = true
                        }
                        else
                        {
                            optsDict[optionName] = optionValue
                        }
                    }
                }
                else
                {
                    args.push(arg)
                }
            }
            if (!commandDict)
                commandDict = commands[""]
            var opt : String;
            for (opt in optsDict)
            {
                if (commandDict.opts.indexOf(opt) == -1)
                    help("Invalid option: " + opt)
            }
            
            var methodArgs : Array = new Array()
            for each (opt in commandDict.opts)
            {
                if (opt in optsDict)
                    methodArgs.push(optsDict[opt])
                else
                    methodArgs.push(null)
            }
            methodArgs.push(args)
            commandDict.cmd.apply(null, methodArgs)
            
        }
        catch (usageError : UsageError)
        {
            trace(usageError.message)
            trace(usage)
        }
    }
    
    private static function endsWith(s : String, suffix : String) : Boolean
    {
        return s.lastIndexOf(suffix) == (s.length - suffix.length);
    }
    
    private static function stripExt(s : String) : String
    {
        var indexOfDot : int = s.lastIndexOf(".")
        if (indexOfDot != -1)
            return s.substring(0, indexOfDot)
        return s;
    }
    
    private static function extractABC(files : Array) : void
    {
        for each (var file : String in files)
        {
            if (endsWith(file, ".swf"))
            {
                var stem : String = stripExt(file)
                var abcs:Array = Reader.readABCBytes(file)
                if (abcs.length > 0)
                    File.writeByteArray(stem + ".abc", abcs[0])
                
                for (var i : uint = 1; i < abcs.length; ++i)
                    File.writeByteArray(stem + i + ".abc", abcs[i])
            }
        }
    }
    
    private static function dumpABS(filterStr : String, files : Array) : void
    {
        var printer : IPrinter = new TracePrinter();
        var filter : ScriptFilter = new ScriptFilter(filterStr)
        for each (var file : String in files)
        {
            var abcs : Array = Reader.readFile(file)
            for each (var abc : ABCFile in abcs)
            {
                if (filter.filterABC(abc))
                {
                    Verifier.verify(abc)
                    var dumpWriter : FilteredAbcDumpWriter = new FilteredAbcDumpWriter(abc, filter.filterScript)
                    dumpWriter.write(printer)
                }
            }
        }
    }
    
    private static function dumpAPI(files : Array) : void
    {
        var printer : IPrinter = new TracePrinter();
        for each (var file : String in files)
        {
            var abcs : Array = Reader.readFile(file)
            for each (var abc : ABCFile in abcs)
                AbcAPIWriter.print(abc, printer)
        }
        
    }
    
    private static function percentString(n : Number, total : Number) : String
    {
        return int(Math.round((n * 10000)/total)) / 100 + "%"
    }
    
    private static function dumpPoolInfo(poolTablePrinter : TablePrinter, abc : ABCFile, poolInfo : Object, abcLen : uint, dumpContents : Boolean) : void
    {
        var poolPropertyName : String = poolInfo.prop;
        var poolName : String = poolInfo.name
        var p : Pool = Pool(abc[poolPropertyName])
        if (dumpContents && poolInfo.dumpValues && (p.getNumItems() > 1))
        {
            var fields : Array = poolInfo.fields;
            var contentPrinter : IPrinter = poolTablePrinter.addSpannedRow();
            var contentTablePrinter : TablePrinter;
            var contentRow : Array;
            var id : uint;
            var c : uint;
            var nCols : uint;
            var entry : Object;
            contentPrinter.println("");
            
            if (fields)
            {
                nCols = fields.length + 1;
                contentTablePrinter = new TablePrinter(nCols, 2)
                
                contentRow = new Array(nCols);
                contentRow[0] = "id"
                for (c=1; c < nCols; ++c)
                {
                    contentRow[c] = fields[c-1]
                }
                contentTablePrinter.addRow(contentRow)
                
                for(id=1; id < p.getNumItems(); ++id)
                {
                    contentRow = new Array(nCols);
                    contentRow[0] = id;
                    entry = p.get(id)
                    for (c=1; c < nCols; ++c)
                        contentRow[c] = entry[fields[c-1]]
                    contentTablePrinter.addRow(contentRow)
                }
            }
            else
            {
                nCols = 2
                contentTablePrinter = new TablePrinter(nCols, 2)
                contentRow = ["id", "value"]
                contentTablePrinter.addRow(contentRow)
                for(id=1; id < p.getNumItems(); ++id)
                {
                    contentRow = [id, p.get(id)]
                    contentTablePrinter.addRow(contentRow)
                }
            }
            contentTablePrinter.print(contentPrinter)
        
        }
        poolTablePrinter.addRow([poolName, p.getNumItems().toString(), p.getSizeInBytes().toString(), percentString(p.getSizeInBytes(), abcLen)])
    }
    
    private static const poolInfos : Array = [   { prop : "intPool", name : "integers", fields : null, dumpValues : true },
                                                { prop : "uintPool", name : "unsigned integers", fields : null, dumpValues : true },
                                                { prop : "numberPool", name : "numbers", fields : null, dumpValues : true },
                                                { prop : "stringPool", name : "strings", fields : null, dumpValues : true },
                                                { prop : "nsPool", name : "namespaces", fields : ["kind", "name_index", "kindString", "uriString", "apiVersionsString"], dumpValues : true },
                                                { prop : "nssetPool", name : "namespace sets", fields : ["namespaceIds", "namespaceStrings"], dumpValues : true },
                                                { prop : "namePool", name : "names", fields : ["kind", "name_index", "ns_index", "nsset_index", "kindString", "typeIds", "asString"], dumpValues : true },
                                                { prop : "methodInfos", name : "methods", fields : null, dumpValues : false },
                                                { prop : "metadataInfos", name : "metadatas", fields : null, dumpValues : false },
                                                { prop : "instanceInfos", name : "instances", fields : null, dumpValues : false },
                                                { prop : "classInfos", name : "classes", fields : null, dumpValues : false },
                                                { prop : "scriptInfos", name : "scripts", fields : null, dumpValues : false },
                                                { prop : "methodBodies", name : "method bodies", fields : null, dumpValues : false } ];
    
    private static function dumpABCInfo(printer : IPrinter, a : ABCFile, abcLen : uint, dumpPools : Boolean, dumpBody : Function = null)
    {
        var poolTablePrinter : TablePrinter = new TablePrinter(4, 2)
        poolTablePrinter.addRow(["pool name", "item count", "size", "% of " + abcLen])
        for each (var poolInfo : Object in poolInfos)
        {
            dumpPoolInfo(poolTablePrinter, a, poolInfo, abcLen, dumpPools)
        }
        poolTablePrinter.print(printer);
        
        var instructionCounts : Vector.<InstructionCountAndSize> = new Vector.<InstructionCountAndSize>(256, true);
        var numMethodBodies : uint = a.methodBodies.getNumItems();
        var totalSize : uint = 0;
        var countAndSize : InstructionCountAndSize;
        for each (var mb : MethodBody in a.methodBodies.getItems())
        {
            for each (var inst : Instruction in mb.instrs)
            {
                var opcode : uint = inst.getOpcode();
                countAndSize = instructionCounts[opcode]
                if (!countAndSize)
                {
                    countAndSize = new InstructionCountAndSize(opcode)
                    instructionCounts[opcode] = countAndSize
                }
                countAndSize.count += 1
                countAndSize.accumulatedSize += inst.getSize();
                totalSize += inst.getSize()
            }
        }
        
        instructionCounts = instructionCounts.sort(function (a : InstructionCountAndSize, b : InstructionCountAndSize) : int {
            var aSize : int = a != null ? a.accumulatedSize : 0;
            var bSize : int = b != null ? b.accumulatedSize : 0;
            return bSize - aSize;
        });
        
        printer.println("");
        if (dumpBody)
        {
            dumpBody(a);
            printer.println("");
        }
        
        var abcConstants : Class = abc.Constants;
        var opcodeTablePrinter : TablePrinter = new TablePrinter(5, 2)
        
        opcodeTablePrinter.addRow(["opcode", "count", "size", "% of " + totalSize, "% of " + abcLen]);
        for (var i : uint = 0; i < 256; ++i)
        {
            countAndSize = instructionCounts[i]
            if (countAndSize)
                opcodeTablePrinter.addRow([abcConstants.opNames[countAndSize.opCode], countAndSize.count.toString(), countAndSize.accumulatedSize.toString(), percentString(countAndSize.accumulatedSize, totalSize), percentString(countAndSize.accumulatedSize, abcLen)]);
        }
        
        opcodeTablePrinter.print(printer);
        
    }
    
    private static function dumpSWFInfo(printer : IPrinter, dumpABC : Function, dumpPools : Boolean, swfReader : SWFReader) : void
    {
        var swfConstants : Class = SWF.Constants;
        var uncompressedBytes : ByteArray = swfReader.getDecompressedBytes()
        var originalData : ByteArray = swfReader.getOriginalBytes()
        var uncompressedBytesLength : Number = uncompressedBytes.length;
        if (uncompressedBytes !== originalData)
        {
            printer.println("decompressed swf " + originalData.length + " -> " + uncompressedBytesLength)
            
        }
        var swf : SWF = swfReader.readSWF()
        printer.println("size " + swf.rect.toString())
        printer.println("frame rate " + swf.frameRate)
        printer.println("frame count " + swf.frameCount)
        for each (var tag : Tag in swf.tags)
        {
            printer.println(swfConstants.tagNames[tag.type] + " " + tag.length.toString() + "b " + int((tag.length * 10000)/uncompressedBytesLength) / 100 + "%")
            switch (tag.type)
            {
            case swfConstants.TAG_DoABC2:
            case swfConstants.TAG_DoABC:
                printer.println("");
                printer.println("");
                SWFReader.decodeABCTag(tag, uncompressedBytes, function (abcBytes : ByteArray, abcName : String) : void {
                    var a : ABCFile = new ABCReader(abcBytes).readABC()[0]
                    a.abcName = abcName
                    dumpABC(printer, a, abcBytes.length, dumpPools)
                })
                printer.println("");
                printer.println("");
            }
        }
    }
    
    private static function dumpInfoHelper(printer : IPrinter, dumpABC : Function, dumpPools : Boolean, files : Array) : void
    {
        for each (var file : String in files)
        {
            var r : IReader = Reader.createReader(file)
            if (r is SWFReader)
            {
                var swfReader : SWFReader = SWFReader(r);
                dumpSWFInfo(printer, dumpABC, dumpPools, swfReader)
            }
            else if (r is SWCReader)
            {
                var swcReader : SWCReader = SWCReader(r)
                var libs : Array = swcReader.readLibraries();
                for each (var lib : Object in libs)
                {
                    printer.println("swf " + lib.path)
                    dumpSWFInfo(printer, dumpABC, dumpPools, lib.swf)
                }
            }
            else if (r is ABCReader)
            {
                var abcReader : ABCReader = ABCReader(r);
                var abcBytes : ByteArray = abcReader.readABCBytes()[0]
                var abc : ABCFile = abcReader.readABC()[0];
                dumpABC(printer, abc, abcBytes.length, dumpPools)
            }
        }
    }
    
    private static function dumpInfo(dumpPools : Boolean, files : Array) : void
    {
        var printer : IPrinter = new TracePrinter();
        dumpInfoHelper(printer, dumpABCInfo, dumpPools, files)
    }
    
    private static function decompress(files : Array) : void
    {
        for each (var file : String in files)
        {
            if (endsWith(file, ".swf"))
            {
                var stem : String = stripExt(file)
                var r : IReader = Reader.createReader(file)
                if (r is SWFReader)
                {
                    var swfReader : SWFReader = SWFReader(r);
                    var uncompressedBytes : ByteArray = swfReader.getDecompressedBytes()
                    var originalData : ByteArray = swfReader.getOriginalBytes()
                    if (uncompressedBytes !== originalData)
                    {
                        var outputByteArray : ByteArray = new ByteArray();
                        outputByteArray.writeByte(0x46); outputByteArray.writeByte(0x57); outputByteArray.writeByte(0x53)
                        outputByteArray.writeBytes(uncompressedBytes)
                        var outputFilename : String = file + ".swf";
                        File.writeByteArray(file + ".swf", outputByteArray)
                        trace("// decompressed swf " + originalData.length + " -> " + outputByteArray.length)
                        trace("// wrote " + outputByteArray.length + " bytes to file " + outputFilename) 
                    }
                }
            }
        }
    }
    
    private static function defaultDump(dumpPools : Boolean, filterStr:String, files : Array) : void
    {
        var tracePrinter : IPrinter = new TracePrinter();
        var commentPrinter : IPrinter = new CommentPrinter(tracePrinter);
        var filter : ScriptFilter = new ScriptFilter(filterStr)
        
        dumpInfoHelper(commentPrinter, function (printer : IPrinter, abc : ABCFile, abcLen : uint, dumpPools : Boolean) : void {
            if (!filter.filterABC(abc))
                return;
            dumpABCInfo(printer, abc, abcLen, dumpPools, function (abcFile : ABCFile) : void {
                //Verifier.verify(abcFile)
                var dumpWriter : FilteredAbcDumpWriter = new FilteredAbcDumpWriter(abcFile, filter.filterScript)
                dumpWriter.write(tracePrinter)
            })
        } , dumpPools, files)
    }
}

