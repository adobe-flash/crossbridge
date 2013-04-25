/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

gTestfile = 'proto_1.js';

/**
   File Name:          proto_1.js
   Section:
   Description:        new PrototypeObject

   This tests Object Hierarchy and Inheritance, as described in the document
   Object Hierarchy and Inheritance in JavaScript, last modified on 12/18/97
   15:19:34 on http://devedge.netscape.com/.  Current URL:
   http://devedge.netscape.com/docs/manuals/communicator/jsobj/contents.htm

   This tests the syntax ObjectName.prototype = new PrototypeObject using the
   Employee example in the document referenced above.

   Author:             christine@netscape.com
   Date:               12 november 1997
*/

// var SECTION = "proto_1";
// var VERSION = "JS1_3";
// var TITLE   = "new PrototypeObject";


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

Assert.expectEq( 
          "jim = new Employee(); jim.name",
          "",
          jim.name );


Assert.expectEq( 
          "jim = new Employee(); jim.dept",
          "general",
          jim.dept );

var sally = new Manager();

Assert.expectEq( 
          "sally = new Manager(); sally.name",
          "",
          sally.name );
Assert.expectEq( 
          "sally = new Manager(); sally.dept",
          "general",
          sally.dept );

Assert.expectEq( 
          "sally = new Manager(); sally.reports.length",
          0,
          sally.reports.length );

Assert.expectEq( 
          "sally = new Manager(); typeof sally.reports",
          "object",
          typeof sally.reports );

var fred = new SalesPerson();

Assert.expectEq( 
          "fred = new SalesPerson(); fred.name",
          "",
          fred.name );

Assert.expectEq( 
          "fred = new SalesPerson(); fred.dept",
          "sales",
          fred.dept );

Assert.expectEq( 
          "fred = new SalesPerson(); fred.quota",
          100,
          fred.quota );

Assert.expectEq( 
          "fred = new SalesPerson(); fred.projects.length",
          0,
          fred.projects.length );

var jane = new Engineer();

Assert.expectEq( 
          "jane = new Engineer(); jane.name",
          "",
          jane.name );

Assert.expectEq( 
          "jane = new Engineer(); jane.dept",
          "engineering",
          jane.dept );

Assert.expectEq( 
          "jane = new Engineer(); jane.projects.length",
          0,
          jane.projects.length );

Assert.expectEq( 
          "jane = new Engineer(); jane.machine",
          "",
          jane.machine );


