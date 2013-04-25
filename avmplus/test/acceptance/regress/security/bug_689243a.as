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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

// MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=689243

import flash.utils.ByteArray;

var SECTION = "ByteArray";
var VERSION = "as3";
startTest();
var TITLE   = "bug 689243";

writeHeaderToLog( SECTION + " "+ TITLE );

var a:ByteArray = new ByteArray;
var b:ByteArray = new ByteArray;
a.position = 0xFFFFFFF0;
b.length = 1000;

var exn = "No exception";
var res = null;
try {
   a.writeBytes(b);
}
catch (e) {
   if (e != null)
       res = e.toString().match(/\w*Error: Error #\d+/);
}
if (res != null)
   exn = res[0];

// "MemoryError" prints just as "Error"
AddTestCase("Correct exception thrown",
           "Error: Error #1000",
           exn);

AddTestCase("ByteArray length did not change",
           0,
           a.length);

AddTestCase("ByteArray position did not change",
           0xFFFFFFF0,
           a.position);

test();

// MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=689243
