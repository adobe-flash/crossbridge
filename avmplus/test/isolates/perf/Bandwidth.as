/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
    import flash.system.*;
    import flash.utils.*;
    use namespace async;

    public function echo(o:Object) {
        return o;
    }

    /*
     create string 2^n
    */
    public function createString(n:uint):String {
        var res:String="1";
        for (var i:uint=0;i<n;i++) {
            res+=res;
        }
        return res;
    }
    public function createArray(n:uint):Array {
        var res:Array=new Array();
        for (var i:uint=0;i<n;i++) {
            res.push(1.1);
        }
        return res;
    }
    public function createVectorUint(n:uint):Vector.<uint> {
        var res:Vector.<uint>=new Vector.<uint>();
        for (var i:uint=0;i<n;i++) {
            res.push(i);
        }
        return res;
    }
    public function createVectorNumber(n:uint):Vector.<Number> {
        var res:Vector.<Number>=new Vector.<Number>();
        for (var i:uint=0;i<n;i++) {
            res.push(1.1);
        }
        return res;
    }
    public function createObject():Object {
        var o:Object=new Object();
        o.str1=createString(5);
        o.str2=createString(6);
        o.str3=createString(4);
        o.arr1=createArray(1000);
        o.vect1=createVectorUint(1000);
        o.vect2=createVectorNumber(1000);
        o.bool=true;
        o.uint=100;
        o.int=-100;
        return o;
    }
    public function createComplexObject(n:uint):Array {
        var res:Array=new Array();
        var i:uint;
        for (i=0;i<n;i++) {
            res.push(createObject());
        }
        return res;
    }
    public function myTimer() {
        return getTimer();
    }
    public function runtest() {
        var nworkers:uint=10;
        var workerlist:Array=new Array();
        var ipromises:Array=new Array();
        var promises:Array=new Array();
        var objects:Array=new Array();
        if (Worker.current.isPrimordial()) {
            for (var i:uint=0;i<nworkers;i++) {
                workerlist[i]=WorkerDomain.current.createWorkerFromPrimordial();
                ipromises[i]=workerlist[i].start();
                objects[i]=createComplexObject(100);
            }
            trace("["+myTimer()+"] created 100 complex objects");
            start=myTimer();
            var num:uint=0;
            while (myTimer()-start<10000) {
                for (var i:uint=0;i<nworkers;i++) {
                    promises[i]=ipromises[i].echo(objects[i]);
                }
                for (var i:uint=0;i<nworkers;i++) {
                    promises[i].receive();
                }
                num++;
            }
            trace("["+myTimer()+"] metric workers "+nworkers);
            trace("["+myTimer()+"] metric transactions "+num);
            trace("["+myTimer()+"] metric time "+(myTimer()-start));
        }
    }
    trace("starting to run tests at "+new Date());
    runtest();
}