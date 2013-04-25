/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Bugzilla 707902

// This is helper code: this must be compiled with -abcfuture (ABC 47.16) into an abc, which will then
// be imported during the compilation of import-with-float4-main.as.

package fnord
{
	public function f(x:float4 = float4(1,2,3,4)) { return x }
}
