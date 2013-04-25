/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.system.*
if (Worker.current.isPrimordial()) {
    var w:Worker=WorkerDomain.current.createWorkerFromPrimordial();
    var p:Promise=w.start();
    var p1:Promise=p.value;
    print(p1.local::receive());
    var p2:Promise=p.Clazz.svalue;
    print(p2.local::receive());
    var p3:Promise=p.o.ivalue;
    print(p3.local::receive());
    w.stop();
} else {
    function get value() {
        return 10;
    }

    class Clazz {
	public static function get svalue() {
	    return 42;
	}

	public function get ivalue() {
	    return 7;
	}
    };

    var o = new Clazz();
}