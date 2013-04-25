/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;

//var dummy_number = NaN;
//var isAS3:Boolean = dummy_number.toString == dummy_number.AS3::toString;

function getNumberProp(name):String
{
   string = '';
   for ( prop in Number )
   {
       string += ( prop == name ) ? prop : '';
   }
   return string;
}

