/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
import avmplus.System;
import flash.system.Promise;
import flash.system.Worker;
import flash.system.WorkerDomain;
import flash.utils.ByteArray;
import flash.net.registerClassAlias;


    public function echo(o:*):*
    {
        return o;
    }


    if (Worker.current.isPrimordial)
    {
        var w:Worker = WorkerDomain.current.createWorkerFromPrimordial();
        //link = Worker.current.newLinkTo(w);
        w.setSharedProperty("arg1", 10);
        w.setSharedProperty("arg1", 11);

        w.setSharedProperty("arg2", 1.20);
        w.setSharedProperty("arg3", true);
        var obj = new ByteArray();
        obj.writeObject({ hello: "World"});
        w.setSharedProperty("arg4", obj);
        obj = new ByteArray();
        obj.writeObject({ hello: "World"});
        w.setSharedProperty("arg4", obj);
        w.setSharedProperty("arg5", null);
        w.setSharedProperty("arg6", {bye: "Cruel World"});
        w.setSharedProperty("arg7", <hello><world/></hello>);
        w.setSharedProperty("arg8", new Date());


        var top:Promise = w.start();
        
    } else {
        for (var i = 1; i <= 8; i++) {
            print(i +":", Worker.current.getSharedProperty("arg" + i));
        }
        Worker.current.setSharedProperty("arg1", undefined);
        print("arg1:", Worker.current.getSharedProperty("arg1"));
        print("arg10:", Worker.current.getSharedProperty("arg10"));

        Worker.current.terminate();
    }

}