/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package
{
    //pseudo-final - no user class can extend Class
    [native(cls="ClassClass", gc="exact", instance="ClassClosure", methods="auto", construct="instance")]
    public dynamic class Class
    {
        // {DontEnum,DontDelete,ReadOnly}
        public native final function get prototype()

        // Class.length = 1 per ES3
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1;
    }
}
