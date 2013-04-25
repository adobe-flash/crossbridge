/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
const NITER:int=100000000;

function main():void
{
    var a:* = 1.2f;
	var b:* = 7.2f;
	var c:* = -7.2f;
	var fstart:uint  = getTimer();
    for(var i:int=0;i < NITER; ++i){
	    a = b + c;
		c = a + b + c;
		b = c + a;
	}
	var ftime:uint  = getTimer() - fstart;
	if(a === 0f && b === 0f && c === 0f)
	    trace("metric time ",ftime);
	else
	    trace("FAIL: results invalid, metric irrelevant")
}

main();

