/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

    import avmplus.File
    import flash.utils.ByteArray
import com.adobe.test.Assert;

//     var SECTION = "File";
//     var VERSION = "as3";
//     var TITLE   = "test File class";


    Assert.expectEq("File does not exist",
    false,
    File.exists("nothing"));

    File.write("test.txt","test file");

    Assert.expectEq("File exists just written file",
      true,
      File.exists("test.txt"));

    Assert.expectEq("File read test file",
      "test file",
      File.read("test.txt"));

    var err="";
    try {
        File.read('unknown.txt');
    } catch (e) {
        err=e.toString();
    }
    Assert.expectEq("File read on unknown file should throw exception",
      "Error: Error #1500",
      err.substring(0,18));

    File.write("test2.ba","sample test data");
    var ba:ByteArray=File.readByteArray("test2.ba");
    Assert.expectEq("File.readByteArray length",true,ba.length=="sample test data".length);

    // Regression for https://bugzilla.mozilla.org/show_bug.cgi?id=473863
    //      File.read fail for binary files
    
    // ByteArray with partial bad utf-8 sequence
    var bytearray_bad : ByteArray = new ByteArray();
    bytearray_bad[0]=0xE4; // 19968
    bytearray_bad[1]=0xB8;
    bytearray_bad[2]=0x80;
    bytearray_bad[3]=0xE4; // bad sequence
    bytearray_bad[4]=0xE4; // 19968
    bytearray_bad[5]=0xB8;
    bytearray_bad[6]=0x80;
    
    File.write("test.bin", bytearray_bad);
    Assert.expectEq("File read binary test file",
                "\u4e00\u00E4\u4e00",
                File.read("test.bin")
               );


