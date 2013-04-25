/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "AS3";  // Version of JavaScript or ECMA
// var TITLE   = "Interface Definition";       // Provide ECMA section title or a description
var BUGNUMBER = "";


//-----------------------------------------------------------------------------

import QualUnqualAccess.*;

import com.adobe.test.Assert;
var eg = new AccessTest();
Assert.expectEq("single implements, default, interface, class", "a.T::f(),a.T::f(),a.T::f()", eg.doAccessA());
Assert.expectEq("single implements (extends and add), default, interface, base, class", "b.T::f(),b.T::f(),b.T::f(),b.T::f()", eg.doAccessBF());
Assert.expectEq("single implements (extends and add), default, interface, class", "b.U::g(),b.U::g(),b.U::g()", eg.doAccessBG());
Assert.expectEq("double implements, default, interface, class", "c.T::f(),c.T::f(),c.T::f()", eg.doAccessCF());

Assert.expectEq("double implements, default, interface, class", "c.V::h(),c.V::h(),c.V::h()", eg.doAccessCH());

Assert.expectEq("double implements (extends and add), default, interface, base, class", "d.T::f(),d.T::f(),d.T::f(),d.T::f()", eg.doAccessDF());

Assert.expectEq("double implements (extends and add), default, interface, class", "d.U::g(),d.U::g(),d.U::g()", eg.doAccessDG());

Assert.expectEq("double implements, default, interface, class", "d.V::h(),d.V::h(),d.V::h()", eg.doAccessDH());

Assert.expectEq("extends (implements (extends and add)) and add, default, interface, base, class",
                "b.T::f(),b.T::f(),b.T::f(),b.T::f()", eg.doAccessEF());

Assert.expectEq("extends (implements (extends and add)) and add, default, interface, class",
                "b.U::g(),b.U::g(),b.U::g()", eg.doAccessEG());


Assert.expectEq("extends and add, default, interface, class",
                "e.E::h()", eg.doAccessEH());

Assert.expectEq("extends (implements (extends and add)) and add, default, interface, base, class",
                "b.T::f(),b.T::f(),b.T::f(),b.T::f()", eg.doAccessFF());

Assert.expectEq("extends (implements (extends and add)) and add, default, interface, class",
                "b.U::g(),b.U::g(),b.U::g()", eg.doAccessFG());
/*
Assert.expectEq("extends and add, default, interface, class",
                "f.V::h(),f.V::h(),f.V::h()", eg.doAccessFH());
*/
              // displays results.
