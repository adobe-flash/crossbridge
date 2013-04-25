/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Attrs {

// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "Clean AS2";  // Version of JavaScript or ECMA
// var TITLE   = "Extend Default Class";       // Provide ECMA section title or a description
//var BUGNUMBER = "";



/*===========================================================================*/

var ATTRS = new Attrs();
import com.adobe.test.Assert;
// Variables
Assert.expectEq("var, empty         :", "var, empty         ", ATTRS.emptyVar);
Assert.expectEq("var, public        :", "var, public        ", ATTRS.pubVar);
var privateVarErr="no error"; try { ATTRS.privVar } catch (e) { privateVarErr=e.toString(); }
Assert.expectEq("var, private       :", "ReferenceError: Error #1069", privateVarErr.substr(0,27));
//Assert.expectEq("var, private       :", "var, private       ", ATTRS.privVar);
Assert.expectEq("var, static        :", "var, static        ", ATTRS.getStatVar());
Assert.expectEq("var, public static :", "var, public static ", ATTRS.getPubStatVar());
Assert.expectEq("var, private static:", "var, private static", ATTRS.getPrivStatVar());
Assert.expectEq("var, static public :", "var, static public ", ATTRS.getStatPubVar());
Assert.expectEq("var, static private:", "var, static private", ATTRS.getStatPrivVar());

// Functions
Assert.expectEq("func, empty         :", "func, empty         ", ATTRS.emptyFunc());
Assert.expectEq("func, public        :", "func, public        ", ATTRS.pubFunc());
var privFunc="no error"; try { ATTRS.privFunc(); } catch (e) { privFunc=e.toString(); }
Assert.expectEq("func, private       :", "ReferenceError: Error #1069", privateVarErr.substr(0,27));
//Assert.expectEq("func, private       :", "func, private       ", ATTRS.privFunc());
Assert.expectEq("func, static        :", "func, static        ", ATTRS.getStatFunc());
Assert.expectEq("func, public static :", "func, public static ", ATTRS.getPubStatFunc());
Assert.expectEq("func, private static:", "func, private static", ATTRS.getPrivStatFunc());
Assert.expectEq("func, static public :", "func, static public ", ATTRS.getStatPubFunc());
Assert.expectEq("func, static private:", "func, static private", ATTRS.getStatPrivFunc());

// Classes
//var c = new ClassEmpty();
//Assert.expectEq("class, empty         :", "class, empty         ", c.fn());

//var cpub = new ClassPub();
//Assert.expectEq("class, public        :", "class, public        ", cpub.fn());

/* The class with 'private' access specifiers are commented */
/*
var cpriv = new ClassPriv();
Assert.expectEq("class, private       :", "class, private       ", cpriv.fn());
*/

//var cstat = new ClassStat();
//Assert.expectEq("class, static        :", "class, static        ", cstat.fn());

//var cpubstat = new ClassPubStat();
//Assert.expectEq("class, public static :", "class, public static ", cpubsat.fn());

/*
var cprivstat = new ClassPrivStat();
Assert.expectEq("class, private static:", "class, private static", cpriv.fn());
*/

//var cstatpub = new ClassStatPub();
//Assert.expectEq("class, static public :", "class, static public ", cstatpub.fn());

/*
var cstatpriv = new ClassStatPriv();
Assert.expectEq("class, static private:", "class, static private", cstatpriv.fn());
*/

// Interfaces
//var i = new IfEmpty_();
//Assert.expectEq("interface, empty         :", "interface, empty         ", i.fn());

//var ipub = new IfPub_();
//Assert.expectEq("interface, public        :", "interface, public        ", ipub.fn());

/*
var ipriv = new IfPriv_();
Assert.expectEq("interface, private       :", "interface, private       ", ipriv.fn());
*/

//var istat = new IfStat_();
//Assert.expectEq("interface, static        :", "interface, static        ", istat.fn());

//var ipubstat = new IfPubStat_();
//Assert.expectEq("interface, public static :", "interface, public static ", ipubstat.fn());

/*
var iprivstat = new IfPrivStat_();
Assert.expectEq("interface, private static:", "interface, private static", iprivstat.fn());
*/

//import Directives.Attributes.IfStatPub_;
//var istatpub = new IfStatPub_();
//Assert.expectEq("interface, static public :", "interface, static public ", istatpub.fn());

/*
var istatpriv = new IfStatPriv_();
Assert.expectEq("interface, static private:", "interface, static private", istatpriv.fn());
*/

/*===========================================================================*/

              // displays results.
}
