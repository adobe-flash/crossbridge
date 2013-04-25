/*
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
    var datatotal:uint=400;

    public function main() {
        var worker:Worker;
        var bytearray:ByteArray;
        mutex=new Mutex();
        var workers:Array=new Array();
        var bytearray:ByteArray=new ByteArray();
        bytearray.shareable=true;
        bytearray.length=10000;
        var activeworkers:uint=0;
        for (var i:uint=0;i<nworkers;i++) {
            workers[i]=WorkerDomain.current.createWorkerFromPrimordial();
            workers[i].setSharedProperty("bytearray",bytearray);
            workers[i].setSharedProperty("workernum",""+(i));
            workers[i].setSharedProperty("mutex",mutex);
            workers[i].start();
	        activeworkers+=1;
            safetrace("[main] background worker "+i+" started");
        }
        for (var i:uint=0;i<nworkers;i++) {
            WaitForValue(bytearray,i,1);
        }
        var senddata:uint=0;
        var gotdata:uint=0;
        var workerctr:uint=0;
        var testfailures='';
        while (true) {
            var request:uint=GetByteArray(bytearray,workerctr%nworkers);
	    if (request==12) {
                mutex.lock();
                bytearray.position=100+nworkers*4+200*(workerctr%nworkers);
		        var res:String=bytearray.readUTF();
                mutex.unlock();
                //safetrace("[main] got result: ["+res+"] from ["+(workerctr%nworkers)+"]");
                if (res.indexOf("[request")==-1 || res.indexOf("[request",10)==-1) {
                    testfailures+="from "+(workerctr%nworkers)+" ["+res+"]\n";
                }
                UpdateByteArray(bytearray,(workerctr%nworkers),1);
                gotdata+=1;
                request=GetByteArray(bytearray,workerctr%nworkers);
            }
            if (gotdata==datatotal) 
                break;
            if (request==1 && senddata<datatotal) {
                mutex.lock();
                bytearray.position=nworkers*4+200*(workerctr%nworkers);
                bytearray.writeUTF("[request "+senddata+"] ");
                mutex.unlock();
                //safetrace("[main] sent request ["+senddata+"] ["+(workerctr%nworkers)+"]");
                UpdateByteArray(bytearray,(workerctr%nworkers),2);
                senddata+=1;
            }
            workerctr+=1;
        }
        var workersalive:uint=nworkers;
        workerctr=0;
        while (workersalive>0) {
            if (GetByteArray(bytearray,workerctr%nworkers)==1) {
                UpdateByteArray(bytearray,workerctr%nworkers,253);
            }
            if (GetByteArray(bytearray,workerctr%nworkers)==254) {
                UpdateByteArray(bytearray,workerctr%nworkers,255);
                workers[workerctr%nworkers].terminate();
                safetrace("terminated worker "+(workerctr%nworkers));
                workersalive=workersalive-1;
            }
            workerctr+=1;
        }
        trace("done "+getTimer());
        Assert.expectEq("Workers messages were correct, no failures","",testfailures);
        Assert.expectEq("Workers state is terminated","terminated",workers[0].state);
        Assert.expectEq("mutex is not locked after test",true,mutex.tryLock());
        for (var i:uint=0;i<nworkers;i++) {
            bytearray.position=nworkers*4+i*200;
            var s1:String=bytearray.readUTF();
            trace("s1="+s1);
            bytearray.position=nworkers*4+i*200+100;
            var s2:String=bytearray.readUTF();
            Assert.expectEq("For worker "+i+" test string was doubled "+s1.length+" * 2 = "+s2.length,s1.length*2==s2.length,true)
        }
        mutex.unlock();
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
                mutex.lock();
                bytearray.position=nworkers*4+workernum*200;
                data=bytearray.readUTF();
                mutex.unlock();
                data=data+data;
                System.sleep(10);
                mutex.lock();
                bytearray.position=100+nworkers*4+workernum*200;
                bytearray.writeUTF(data);
                mutex.unlock();
                UpdateByteArray(bytearray,workernum,12);
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
//         var TITLE   = "Test Workers passes data through shared byte array";
        main();
    } else {
        background();
    }
}
