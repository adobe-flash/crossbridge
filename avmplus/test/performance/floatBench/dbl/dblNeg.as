/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
const NITER:int=100000000;

function main():void
{
    var a:Number=1.2;
	var fstart:uint  = getTimer();
    for(var i:int=0;i<NITER;i++){
	    a = -a;
	}
	var ftime:uint  = getTimer() - fstart;
	trace("metric time ",ftime);
}

main();

