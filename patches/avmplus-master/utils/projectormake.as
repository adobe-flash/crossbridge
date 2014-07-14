/* -*- mode: java -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is [Open Source Virtual Machine.].
 *
 * The Initial Developer of the Original Code is
 * Adobe System Incorporated.
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Adobe AS3 Team
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/* Utility to make an avmshell projector from an ABC/SWF file.
 *
 * Usage:
 *
 *   projectormake [-o filename] avmshell abc/swf [-- vmargs]
 *
 *   -o filename    defines the name of the output file
 *
 * By default the name of the output file is the name of the abc/swf
 * file with ".abc" or ".swf" removed.
 */

package projectormake
{
    import avmplus.*;
    import flash.utils.*;

    var input_names = []
    var output_name = null
    var vmargs = null

    var i=0, j;
    var argc=System.argv.length;
    var argv=System.argv;
    var s;
    while (i < argc) {
        s = argv[i];
        if (s == "-o") {
            if (output_name != null || i+1 == argc)
            usage();
            output_name = argv[i+1];
            i += 2;
        }
        else if (s == "--") {
            vmargs = argv.slice(i+1)
            break
        }
        else if (s.charAt(0) == "-") {
            usage();
        }
        else {
            input_names.push(s);
            i++;
        }
    }

    if (input_names.length != 2)
        usage();

    if (output_name == null) {
        output_name = input_names[1];
        output_name = output_name.replace(/\.abc$/, "");
        output_name = output_name.replace(/\.swf$/, "");
    }

    var result = new ByteArray()
    result.endian = "littleEndian"
    result.writeBytes(File.readByteArray(input_names[0]))

    var content = File.readByteArray(input_names[1])
    result.writeBytes(content)
    
    if(vmargs != null) {
        var vmargslength = result.position
        for each(var vmarg in vmargs) {
            result.writeUTFBytes(vmarg)
            result.writeByte(0x0)
        }
        vmargslength = result.position - vmargslength
    }

    result.writeByte(0x56);
    result.writeByte(0x34);
    result.writeByte(0x12);
    result.writeByte(0xFA);
    result.writeUnsignedInt(content.length);
    result.writeUnsignedInt(vmargs == null ? int.MAX_VALUE : vmargslength);
    File.writeByteArray(output_name, result);
    
    if(vmargs == null) {
        print("content length: "+content.length)
    } else {
        print("content length: "+content.length+", args length: "+vmargslength+", argc: " + vmargs.length)
    }

    System.exit(0);

    function usage() {
        print("Usage: avmshell projectormake.abc -- [-o filename]  avmshell abc/swf [-- vmargs]");
        System.exit(1);
    }
}