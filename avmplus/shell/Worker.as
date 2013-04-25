/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.system
{

    include "../core/api-versions.as"
    import flash.utils.Dictionary;
    import flash.utils.ByteArray;

    /**
     * This class represents an enumeration of the possible worker states. 
     */
    [API(CONFIG::SWF_17)]
    public final class WorkerState
    {
        /**
        * These value represents a worker state returned value for Worker.state
        * and used by WorkerEvent.
        *
        * An object that represents the new worker has been created, 
        * but the worker is dormant; no code is executing on its behalf.
        */
        public static const NEW:String = "new";
                
        /**
        * The worker has begun executing application code
        * and it has not been directed to terminate in any way yet.
        */
        public static const RUNNING:String = "running";
                
        /**
        * The worker has been stopped programatically by some other worker 
        * that invoked the Worker.terminate() method on it.
        */
        public static const TERMINATED:String = "terminated";
        
        /**
        * The worker could not start due to lack of resources 
        * or due to some other error condition. 
        */
        public static const FAILED:String = "failed";
        
        /**
        * An active worker has been terminated involuntarily 
        * by the runtime system due to some error situation 
        * without ending in EXCEPTION and without stopping normally.
        * For instance, a debugger may be able to abort workers.
        */
        public static const ABORTED:String = "aborted";
        
        /**
        * The worker has terminated by not catching an exception.
        */
        public static const EXCEPTION:String = "exception";


        public static function code(s:String): int
        {
            if (s == NEW) return 1;
            else if (s == RUNNING) return 2;
            else if (s == TERMINATED) return 3;
            else if (s == FAILED) return 4;
            else if (s == ABORTED) return 5;
            else if (s == EXCEPTION) return 6;
            else throw Error("not an enum value: " + s);
        }

    }

    /**
    * A user program-initiated, software-isolated, virtual player instance.
    *
    * There can be multiple ActionScript programs running in the same OS process.
    * In earlier versions, this only happened when a user opened several browser tabs
    * and a separate Flash script/application was to appear in each of these.
    *
    * Now ActionScript code can start new player instances programmatically, using this class. We call these "workers".
    *
    * The primordial ActionScript program that is running when the player starts up is also regarded as a worker.
    * It is called the "main" Worker.
    * The others are called "background" workers.
    *
    * The APIs available to background workers may be somewhat limited.
    * Furthermore physical screen access may not be available to background workers.
    * Off-screen rendering applies then instead.
    *
    * TODO: explain what happens in AVM shell? Do we document AVM shell in public APIs at all?
    */
    [native(cls="::avmshell::ShellWorkerClass", instance="::avmshell::ShellWorkerObject", methods="auto", construct="native")]
    public final class Worker
    {

        /**
        * @private
        */
        public function Worker() { }



        public native function get state():String;

        /**
        * Run the code blobs passed into the constructor in order and enter the event loop.
        * @return proxy to the remote worker.
        */
        public native function start() :void;

        public native function isParentOf(other: Worker): Boolean;
        public native function get isPrimordial(): Boolean;

        public native function setSharedProperty(key:String, value:*):void;
        public native function getSharedProperty(key:String):*;


        /**
        * Terminate this worker.
        *
        * If this is the main worker, then all other workers are terminated, too.
        * Currently blocks for termination, which should be prompt.
        *
        * @return true if the worker was running and interrupted, false if it had already terminated.
        */
        public native function terminate() :Boolean;

        /**
        * Obtain a reference to the worker in which this code is currently running.
        */
        public static function get current() :Worker
        {
            return m_current;
        }

        /**
        * Temporary - debugging only
        */
        public static native function pr(s :String) :void;

        private static var m_current :Worker;

        private var m_byteCode :ByteArray;
        

    }


    // Mockup worker event for shell use.
    public final class WorkerEvent
    {

        public static const WORKER_STATE:String = "workerState";

        public function WorkerEvent(previousState:String, currentState:String)
        {
            m_previousState = previousState;
            m_currentState = currentState;
            
        }
        public function get currentState():String
        {
            return m_currentState;
        }

        public function get previousState():String
        {
            return m_previousState;
        }

        public function get target():Object
        {
            return m_target;
        }

        private var m_previousState:String;
        private var m_currentState:String;
        var m_target:Object;

    }

}
