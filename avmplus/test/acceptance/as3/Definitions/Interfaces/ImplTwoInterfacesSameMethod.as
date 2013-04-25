/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import ImplTwoInterfacesSameMethod.*;
import com.adobe.test.Assert;


var OBJ = new C();

Assert.expectEq( "Interface name does not disambiguate method definitions",
             "{A,B} function sameName",
             OBJ.A::sameName() );
Assert.expectEq( "Interface name does not disambiguate method definitions",
             "{A,B} function sameName",
             OBJ.B::sameName() );




