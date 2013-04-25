/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'proto_4.js';

/**
   File Name:          proto_4.js
   Section:
   Description:        new PrototypeObject

   This tests Object Hierarchy and Inheritance, as described in the document
   Object Hierarchy and Inheritance in JavaScript, last modified on 12/18/97
   15:19:34 on http://devedge.netscape.com/.  Current URL:
   http://devedge.netscape.com/docs/manuals/communicator/jsobj/contents.htm

   This tests the syntax ObjectName.prototype = new PrototypeObject using the
   Employee example in the document referenced above.

   If you add a property to an object in the prototype chain, instances of
   objects that derive from that prototype should inherit that property, even
   if they were instatiated after the property was added to the prototype object.


   Author:             christine@netscape.com
   Date:               12 november 1997
*/

// var SECTION = "proto_3";
// var VERSION = "JS1_3";
// var TITLE   = "Adding properties to the prototype";


function Employee () {
  this.name = "";
  this.dept = "general";
}
function Manager () {
  this.reports = [];
}
Manager.prototype = new Employee();

function WorkerBee () {
  this.projects = new Array();
}

WorkerBee.prototype = new Employee();

function SalesPerson () {
  this.dept = "sales";
  this.quota = 100;
}
SalesPerson.prototype = new WorkerBee();

function Engineer () {
  this.dept = "engineering";
  this.machine = "";
}
Engineer.prototype = new WorkerBee();

var jim = new Employee();
var terry = new Engineer();
var sean = new SalesPerson();
var wally = new Manager();

Employee.prototype.specialty = "none";

var pat = new Employee();
var leslie = new Engineer();
var bubbles = new SalesPerson();
var furry = new Manager();

Engineer.prototype.specialty = "code";

var chris = new Engineer();


Assert.expectEq( 
          "jim = new Employee(); jim.specialty",
          "none",
          jim.specialty );

Assert.expectEq( 
          "terry = new Engineer(); terry.specialty",
          "code",
          terry.specialty );

Assert.expectEq( 
          "sean = new SalesPerson(); sean.specialty",
          "none",
          sean.specialty );

Assert.expectEq( 
          "wally = new Manager(); wally.specialty",
          "none",
          wally.specialty );

Assert.expectEq( 
          "furry = new Manager(); furry.specialty",
          "none",
          furry.specialty );

Assert.expectEq( 
          "pat = new Employee(); pat.specialty",
          "none",
          pat.specialty );

Assert.expectEq( 
          "leslie = new Engineer(); leslie.specialty",
          "code",
          leslie.specialty );

Assert.expectEq( 
          "bubbles = new SalesPerson(); bubbles.specialty",
          "none",
          bubbles.specialty );


Assert.expectEq( 
          "chris = new Employee(); chris.specialty",
          "code",
          chris.specialty );
