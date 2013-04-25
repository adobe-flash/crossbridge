/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
const NITER:int=5000000; // 20 times less 

function main():void
{
    var a:float=1.2f;
	var b:float=2.4f;
	var c:float = -3.2f;
	var fstart:uint  = getTimer();
    for(var i:int=0;i<NITER;i++){
	    a = b%c;
		c = a%b;
		b = c%a;
	}
	var ftime:uint  = getTimer() - fstart;
	trace("metric time ",ftime);
}

main();

