/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package flash.trace
{
    [native(cls="TraceClass", methods="auto", construct="none")]
    public class Trace
    {
        /* levels of desired logging */
        public static const OFF:int = 0;
        public static const METHODS:int = 1;                        // method entry only
        public static const METHODS_WITH_ARGS:int = 2;              // method entry and arguments
        public static const METHODS_AND_LINES:int = 3;              // method entry and line numbers
        public static const METHODS_AND_LINES_WITH_ARGS:int = 4;    // method entry, arguments and line numbers

        /* used to select either file or listener callback logging. */
        public static const FILE = 1;
        public static const LISTENER = 2;

        public static native function setLevel(l:int, target:int=LISTENER);
        public static native function getLevel(target:int=LISTENER):int;

        /**
         * Register a listener to receive trace callbacks upon method and/or
         * line number changes.  Passing null to this function effectively
         * 'unregisters' the listener and only a single listener can
         *  be specified.
         *
         *  The callback funcition must have the following signature:
         *
         *     function foo(file_name:String, linenum:int, method_name:String, method_args:String):void
         *
         *  Depending upon the setting of the trace level, 2 or more arguments may be non-empty.
         *  In all cases file_name and method_name are obtained and passed to the callback.  If
         *  the trace level include line numbers information then this value is also obtained
         *  and passed into the callback.  Setting the trace level to include method arguments implies
         *  that the 'method_args' parameter will be filled with string representation of the arguments
         *  passed into the call.
         */
        public static native function setListener(f:Function);
        public static native function getListener():Function;
    };
};
