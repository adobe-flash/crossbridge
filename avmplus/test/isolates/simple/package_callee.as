/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package bar {

public var dummy:Number = 42;

}

package baz {

public var v1:Number = 7;

public var redef:Number = 42;

public function foo():String {
  return "BLAH";
}

}

package baz.bar {

public var v2:Number = 77;

public function foo():String {
  return "BLEH";
}

}

package bam.bar {

public var o = new Object();

}

package {

public function unnamedFoo():String {
  return "UNNAMED";
}

}

function foo():String {
  return "BLOH";
}

var v3:Number = 777;

var bam:Number = 7777;

var bar:Number = 77777;

class baz {
  public static var redef:Number = 0;
}

