/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "6.12";
// var VERSION = "AS3";
// var TITLE   = "The comma operator agumented by float values";


AddStrictTestCase("0f, 1f", 1f, (0f, 1f));
AddStrictTestCase("1f, 0f", 0f, (1f, 0f));


