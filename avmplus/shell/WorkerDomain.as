/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.system {

    import flash.utils.ByteArray;
    import avmplus.File;

    [native(cls="::avmshell::ShellWorkerDomainClass", instance="::avmshell::ShellWorkerDomainObject", methods="auto")]
    public final class WorkerDomain
    {

        public function WorkerDomain() { }

        public static function get current() :WorkerDomain
        {
            return m_current;
        }

        /**
        * Create a new Worker.
        *
        * Code to be executed in the Worker is specified by the byteArray argument.
        * The method does not start execution of the worker, rather, execution is initiated by the Worker.start() method.
        * The worker terminates when the code exits by returning or by throwing an exception,
        * or when it calls stop().
        *
        * @param byteArray  A ByteArray representing ABC code
        *
        * @exceptions  Throws an Error if the worker could not be created
        *              (usually when exceeding implementation limits)
        *
        * @note Workers are heavy-weight.  A new worker has a
        *       new GC heap, a fresh set of primitives, a new jit buffer, and new
        *       jitted code.  Very little is shared in the implementation.
        */
        public function createWorkerFromByteArray(byteArray :ByteArray) :Worker
        {
            return createWorkerFromByteArrayInternal(byteArray);
        }

        public function createWorkerFromPrimordial() :Worker
        {
            return createWorkerFromByteArrayInternal(null);
        }

        private native function createWorkerFromByteArrayInternal(byteArray :ByteArray) :Worker;

        public native function listWorkers():Vector.<Worker>;

        private static var m_current :WorkerDomain;

    }

}
