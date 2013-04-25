// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
  function AESEncryptCtr(plaintext:String, password:String, nBits:int):String {
    var nBytes = 20;
    var pwBytes:Array = new Array(nBytes);
    var blockSize:int = 16;  
    var blockCount:int = 20;

    var ciphertext:Array = new Array(blockCount);  
	var cipherCntr:Array = new Array(blockSize);
	var blockLength:int = 20;
	var ct:String = '';

    for (var b:int=0; b<blockCount; b++) {
      for (var i:int=0; i<blockLength; i++) {  
        var plaintextByte:int = plaintext.charCodeAt(b*blockSize+i);
        var cipherByte:int = plaintextByte ^ cipherCntr[i];
        ct += String.fromCharCode(cipherByte);
      }

      ciphertext[b] = escCtrlChars(ct);  
		print("next iter");
    }

    return "";
  }

function escCtrlChars(str:String):String {  // escape control chars which might cause problems handling ciphertext
  var s:String = "";
  for ( var i:int=0 ; i < str.length ; i++ ) {
    var c:String = str.charAt(i);
    if (c == "\0" || c == "\t" || c == "\n" || c == "\v" || c == "\f" || c == "\r" || c == "\xA0" || c == "'" || c == "\"" || c == "!" || c == "-") {
        s += "!" + c.charCodeAt(0) + "!";
	}
    else
        s += c;
  }
  return s;
}


  var plainText:String = "ROMEO: But, soft! what light through yonder window breaks?\n";

  var password:String = "O Romeo, Romeo! wherefore art thou Romeo?";

  AESEncryptCtr(plainText, password, 256);
}
