/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package
{
import avmplus.System
import avmplus.File
import avmplus.describeType;
import avmplus.FLASH10_FLAGS;

import flash.utils.ByteArray

import abc.Reader
import abc.DOTWriter
import abc.XMLWriter
import abc.AbcFormatWriter
import abc.AbcDumpWriter
import abc.AbcAsmWriter
import abc.Verifier
import abc.Types.ABCFile
import abc.AbcAPIWriter

public class ABCDump
{

    private function dumpABC(abcfile : ABCFile, abcFormatPrinter : IPrinter, abcDumpPrinter : IPrinter, dumpXML : Boolean, dumpDOT : Boolean, dumpABCASM : Boolean, dumpAPI : Boolean, dir : String, name : String) : void
    {
        Verifier.verify(abcfile)
        AbcFormatWriter.print(abcfile,  abcFormatPrinter)
        AbcDumpWriter.print(abcfile, abcDumpPrinter)
        if (dumpXML)
            XMLWriter.writeFile(abcfile,        dir + name + ".xml")
        if (dumpDOT)
            DOTWriter.writeFile(abcfile,       dir + name)
        if (dumpABCASM)
            AbcAsmWriter.writeFile(abcfile, dir + name + ".abs")
        if (dumpAPI)
            AbcAPIWriter.writeFile(abcfile, dir + name + ".api.txt")
    }

    public function ABCDump()
    {
        
    }
    
    public static function abcDump() : void
    {
        var dumper : ABCDump = new ABCDump();
        var cmdLine : CmdLine = CmdLine.parse(System.argv);

        var abcs:Array = Reader.readFile(cmdLine.inputFileName)

        var baseName : String = cmdLine.inputFileName.substring(cmdLine.inputFileName.lastIndexOf(pathSep));
        var abcFormatBytes : ByteArray = new ByteArray();
        var abcFormatPrinter : IPrinter = new ByteArrayPrinter(abcFormatBytes);
        var abcDumpBytes : ByteArray = new ByteArray();
        var abcDumpPrinter : IPrinter = new ByteArrayPrinter(abcDumpBytes)
        
        if (abcs.length == 1)
        {
            dumper.dumpABC(abcs[0], abcFormatPrinter, abcDumpPrinter, cmdLine.dumpXML, cmdLine.dumpDOT, cmdLine.dumpABCASM, cmdLine.dumpAPI, cmdLine.outputDirectory, baseName);
        }
        else
        {
            var i : uint = 0;
            for each ( var abcfile : ABCFile in abcs )
            {
                abcFormatPrinter.println("// abc " + " " + i + (abcfile.abcName != null ? " " + abcfile.abcName : ""));
                dumper.dumpABC(abcfile, abcFormatPrinter, abcDumpPrinter, cmdLine.dumpXML, cmdLine.dumpDOT, cmdLine.dumpABCASM, cmdLine.dumpAPI, cmdLine.outputDirectory, baseName + "-" + i);
                abcFormatPrinter.println("");
                abcFormatPrinter.println("");
                abcDumpPrinter.println("");
                abcDumpPrinter.println("");
                i++;
            }
        }
        File.writeByteArray(cmdLine.outputDirectory + baseName + ".dump", abcFormatBytes);
        File.writeByteArray(cmdLine.outputDirectory + baseName + ".txt", abcDumpBytes);
    }
}

}

const pathSep : String = "/";

class CmdLine
{
    public var inputFileName : String;
    public var outputDirectory : String;
    public var dumpXML : Boolean;
    public var dumpDOT : Boolean;
    public var dumpABCASM : Boolean;
    public var dumpAPI : Boolean;
    
    private static var usageStr : String = <x><![CDATA[
[-xml] [-dot] [-abcasm] [-api] inputFileName outputDirectory
]]></x>.text()[0]
    
    private static function usage(msg : String) : void
    {
        throw new Error(msg + "\n" + usageStr);
    }
    
    public function toString() : String
    {
        return [inputFileName, outputDirectory, dumpXML.toString(), dumpDOT.toString()].join(", ");
    }
    
    public static function parse(argv : Array) : CmdLine
    {
        var result : CmdLine = new CmdLine();
        var parsingOpts : Boolean = true;
        
        for each (var arg : String in argv) {
            if (parsingOpts) {
                if (arg.charAt(0) == "-") {
                    switch (arg)
                    {
                    case "-xml":
                        result.dumpXML = true;
                        break;
                    case "-dot":
                        result.dumpDOT = true;
                        break;
                    case "-abcasm":
                        result.dumpABCASM = true;
                        break;
                    case "-api":
                        result.dumpAPI = true;
                        break;
                    default:
                        usage("Invalid option:" + arg);
                    }
                    continue;
                }
            }
            parsingOpts = false;
            
            if (result.inputFileName == null) {
                result.inputFileName = arg;
            }
            else if (result.outputDirectory == null) {
                if (arg.charAt(arg.length - 1) != pathSep)
                    arg += pathSep;
                result.outputDirectory = arg;
            }
            else {
                usage("Too many arguments");
            }
        }
        
        if ((result.inputFileName == null) || (result.outputDirectory == null))
            usage("Too few arguments");
        return result;
    }
}

