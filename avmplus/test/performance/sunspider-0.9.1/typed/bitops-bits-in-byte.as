// Copyright (c) 2004 by Arthur Langereis (arthur_ext at domain xfinitegames, tld com)


// 1 op = 2 assigns, 16 compare/branches, 8 ANDs, (0-8) ADDs, 8 SHLs
// O(n)

package {

function bitsinbyte(b:uint):uint {
var m:uint = 1, c:uint = 0;
while(m<0x100) {
if(b & m) c++;
m <<= 1;
}
return c;
}

function TimeFunc(func:Function):void {
var x:uint, y:uint, t:uint;
for(x=0; x<350; x++)
for(y=0; y<256; y++) func(y);
}

// main entry point for running testcase
function runTest():void{
TimeFunc(bitsinbyte);
} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

}
