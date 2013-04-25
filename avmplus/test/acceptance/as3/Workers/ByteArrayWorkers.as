/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
package {

    import avmplus.System
    import flash.system.Worker
    import flash.system.WorkerDomain
    import flash.utils.ByteArray
    import flash.concurrent.Mutex
import com.adobe.test.Assert;

    var mutex:Mutex;
    var nworkers:uint=5;
 
    public function main() {
        var worker:Worker;
        var bytearray:ByteArray;
        mutex=new Mutex();
        var workers:Array=new Array();
        var bytearray:ByteArray=new ByteArray();
        bytearray.shareable=true;
        bytearray.length=2000;
        var activeworkers:uint=0;
        for (var i:uint=0;i<nworkers;i++) {
            worker=WorkerDomain.current.createWorkerFromPrimordial();
            worker.setSharedProperty("bytearray",bytearray);
            worker.setSharedProperty("mutex",mutex);
            worker.setSharedProperty("workernum",""+i);
            worker.start();
            workers[i]=worker;
        }
        // wait for all workers to set state = 1
        for (var i:uint=0;i<nworkers;i++) {
            WaitForValue(bytearray,i,1);
        }
        for (var i:uint=0;i<nworkers;i++) {
            UpdateByteArray(bytearray,i,2);
        }

        var counter:uint=0;
        while (true) {
            for (var i:uint=0;i<nworkers;i++) {
                var state:uint=GetByteArray(bytearray,i);
                if (state==12) {
                    safetrace("worker "+i+" = 12");
                    UpdateByteArray(bytearray,i,99);
                    counter++;
                }
            }           
            if (counter==nworkers) break;
        }
        trace("test 1 finished");
        checkResult(bytearray,"multiple workers overwrite bytearray using direct access, all values should be received");
        for (var i:uint=0;i<nworkers;i++) {
            UpdateByteArray(bytearray,i,3);
        }
        counter=0;
        while (true) {
            for (var i:uint=0;i<nworkers;i++) {
                var state:uint=GetByteArray(bytearray,i);
                if (state==13) {
                    UpdateByteArray(bytearray,i,99);
                    safetrace("worker "+i+" = 13");
                    counter+=1;
                }
            }           
            if (counter==nworkers) break;
        }
        trace("test 2 finished");
        checkResult(bytearray,"multiple workers overwrite bytearray using atomicCompareAndSwapIntAt, all values should be received");
        for (var i:uint=0;i<nworkers;i++) {
            UpdateByteArray(bytearray,i,4);
        }        
        counter=0;
        while (true) {
            for (var i:uint=0;i<nworkers;i++) {
                var state:uint=GetByteArray(bytearray,i);
                if (state==14) {
                    UpdateByteArray(bytearray,i,99);
                    safetrace("worker "+i+" = 13");
                    counter+=1;
                }
            }           
            if (counter==nworkers) break;
        }
        trace("test 3 finished");
        checkResult(bytearray,"multiple workers overwrite bytearray using mutex and direct access, all values should be received");
        for (var i:uint=0;i<nworkers;i++) {
            UpdateByteArray(bytearray,i,5);
        }        
        counter=0;
        while (true) {
            for (var i:uint=0;i<nworkers;i++) {
                var state:uint=GetByteArray(bytearray,i);
                if (state==15) {
                    UpdateByteArray(bytearray,i,99);
                    safetrace("worker "+i+" = 13");
                    counter+=1;
                }
            }           
            if (counter==nworkers) break;
        }
        trace("test 4 finished");
        checkResult(bytearray,"multiple workers overwrite bytearray using atomicCompareAndSwapIntAt and random sleeps, all values should be received");
        for (var i:uint=0;i<nworkers;i++) {
            UpdateByteArray(bytearray,i,253);
        }        
        counter=0;
        while (true) {
            for (var i:uint=0;i<nworkers;i++) {
                var state:uint=GetByteArray(bytearray,i);
                if (state==254) {
                    UpdateByteArray(bytearray,i,255);
                    workers[i].terminate();
                    counter+=1;
                }
            }           
            if (counter==nworkers) break;
        }
        trace("done");

    }
    public function checkResult(bytearray:ByteArray,msg:String) {
        var results:Array=new Array();
        for (var i:uint=0;i<nworkers;i++)
            results[i]=0;
        for (var i:uint=0;i<100;i++) {
            results[bytearray[nworkers*4+i*4]]+=1;
        }
        var count:uint=0;
        for (var i:uint=0;i<nworkers;i++) {
            trace(""+i+" = "+results[i]);
            count+=results[i];
        }
        Assert.expectEq(msg,100,count);
    }
    public function background() {
        var worker:Worker=Worker.current;
        var bytearray:ByteArray;
        bytearray=worker.getSharedProperty("bytearray");
        workernum=int(worker.getSharedProperty("workernum"));
        mutex=worker.getSharedProperty("mutex");
        safetrace("[bg"+workernum+"] setting status 1");
        UpdateByteArray(bytearray,workernum,1);
        var request:uint;
        var data:String;
        while (true) {
            request=GetByteArray(bytearray,workernum);
            if (request==2) {
                for (var n:uint=0;n<1000;n++) {
                    for (var i:uint=0;i<100;i++) {
                        bytearray[nworkers*4+i*4]=workernum;
                    }
                }
                UpdateByteArray(bytearray,workernum,12);
            } else if (request==3) {
                for (var n:uint=0;n<1000;n++) {
                    for (var i:uint=0;i<100;i++) {
                        UpdateByteArray(bytearray,nworkers*4+i,workernum);
                    }
                }
                UpdateByteArray(bytearray,workernum,13);

            } else if (request==4) {
                for (var n:uint=0;n<100;n++) {
                    for (var i:uint=0;i<100;i++) {
                        mutex.lock();
                        bytearray[nworkers*4+i*4]=workernum;
                        mutex.unlock();
                    }
                }
                UpdateByteArray(bytearray,workernum,14);
            } else if (request==5) {
                for (var n:uint=0;n<1;n++) {
                    for (var i:uint=0;i<100;i++) {
                        System.sleep(Math.random()*50);
                        bytearray[nworkers*4+i*4]=workernum;
                    }
                }
                UpdateByteArray(bytearray,workernum,15);
            } else if (request==253) {
                safetrace("[worker"+workernum+"] finishing");
                UpdateByteArray(bytearray,workernum,254); 
                break;
            }
        }
    }
    public function safetrace(s:String) {
        mutex.lock();
        trace(s);
        mutex.unlock();
    }
    public function GetByteArray(bytearray:ByteArray,index:uint):uint {
        return bytearray[4*index+0]; 
    }
    public function UpdateByteArray(bytearray:ByteArray,index:uint,value:uint):void {
        var attempts:uint=0;
        var result:uint=0;
        var expected:uint;
        expected=GetByteArray(bytearray,index);
        while (attempts<10000) {
            result=bytearray.atomicCompareAndSwapIntAt(index*4,expected,value);
            if (result==expected) break;
            expected=result;
            attempts+=1;
        }
        assert(result==expected);
    }
    public function assert(b:Boolean,msg:String=""):void {
        if (b==false) {
            throw new Error("assertion failed "+msg);
        }
    }
    public function WaitForValue(b:ByteArray,index:uint,value:uint,timeout:uint=1000) {
        var startTime:uint=getTimer();
        while (true) {
            if (b[index*4]==value) {
                break;
            }
            if (getTimer()-startTime>timeout) {
                safetrace("error: WaitForValue "+index+" "+value+" timed out");
                return -1;
            }
        }
        return b[index*4];
    }
    if (Worker.current.isPrimordial) {
//         var SECTION = "workers";
//         var VERSION = "AS3";
//         var TITLE   = "Test ByteArray with multiple workers";
        main();
    } else {
        background();
    }
}
