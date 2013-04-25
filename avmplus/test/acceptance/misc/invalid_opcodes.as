/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import avmplus.Domain;
import flash.utils.ByteArray;
import avmplus.System;
import com.adobe.test.Assert;

// var SECTION = "Misc Tests";
// var VERSION = "";
// var TITLE = "Verify that invalid opcodes are rejected by the verifier";


var invalidOpcodes:Array = [
    0,
    0x0A,
    0x0B,
    0x22,   // OP_pushfloat
    0x33,   // OP_lix8
    0x34,   // OP_lix16
    0x3F,
    0x4B,   // OP_callsuperid
    0x4D,   // OP_callinterface
    0x54,
    0x5B,   // OP_findpropglobalstrict
    0x5C,   // OP_findpropglobal
    0x69,
    0x6B,
    0x79,   // OP_convert_f
    0x7A,   // OP_convert_num
   0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE,
    0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
];

/*
 *  The bytecode array is from the following abcasm code:
 *
 *  function m() {
 *      // Try to use the internal opcode OP_lix8 = 0x33
 *      0x33
 *  }
 *
 *  The invalid opcode is at bytecode[0x1E]
 *
*/

var bytecode:Array = [0x10, 0, 0x2E, 0,
                      1, 1, 1, 0x02,
                      1, 0x6D, 1, 1,
                      1, 1, 0, 0,
                      1, 0, 0, 0,
                      1, 0, 0, 1,
                      0, 0, 1, 0,
                      0, 1,
                      0x33, // This is the opcode that gets changed on each loop
                      0,0
                      ];

var ba:ByteArray = new ByteArray();

var valid_new_opcodes:Array = new Array();

if(System.swfVersion>=15){
   // Float support enabled; add the new opcodes as "valid".
   valid_new_opcodes.push(0x22);   // OP_pushfloat
   valid_new_opcodes.push(0x79);   // OP_convert_f
   valid_new_opcodes.push(0x7A);   // OP_convert_num
}

for (var i = 0; i < bytecode.length; i++) {
    ba.writeByte(bytecode[i]);
}


var d:Domain = new Domain(Domain.currentDomain);

var expected:String = 'VerifyError: Error #1011';
var actual:String;

for (var i = 0; i < invalidOpcodes.length; i++) {
   actual = 'no error';
    ba[0x1E] = invalidOpcodes[i];   // set the invalid opcode
   if(valid_new_opcodes.indexOf(ba[0x1E])>=0)
      continue; // opcode is now valid...

    try {
        d.loadBytes(ba);
    } catch (v:VerifyError) {
        actual = v.toString().substr(0,expected.length);
        if (v.toString().length > expected.length) {
            // debugger build, also test the error message
            var opcodeMsg = 'contained illegal opcode '+invalidOpcodes[i].toString();
            Assert.expectEq('opcode: '+invalidOpcodes[i]+' check illegal opcode message == actual illegal opcode', true, v.toString().search(opcodeMsg) > 0);
            print(v.toString())
        }
    } finally {
        Assert.expectEq('Testing invalid opcode: '+invalidOpcodes[i], expected, actual);
    }
}


