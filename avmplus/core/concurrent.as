/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.concurrent 
{

import flash.errors.IllegalOperationError;
include "api-versions.as"

/**
 * The Mutex (short for "mutual exclusion") class provides a way to make sure 
 * that only one set of code operates on a particular block of memory or other 
 * shared resource at a time. The primary use for a Mutex is to manage code in 
 * different workers accessing a shareable byte array (a ByteArray object whose 
 * <code>shareable</code> property is <code>true</code>). However, a Mutex can be 
 * used to manage workers' access to any shareable resource, such as an AIR native 
 * extension or a filesystem file. No matter what the resource, the purpose of 
 * the mutex is to ensure that only one set of code accesses the resource at a time.
 *
 * <p>A mutex manages access using the concept of resource ownership. At any 
 * time a single mutex is "owned" by at most one worker. When ownership of a 
 * mutex switches from one worker to another the transision is atomic. This 
 * guarantees that it will never be possible for more than one worker to take 
 * ownership of the mutex. As long as code in a worker only operates on a shared 
 * resource when that worker owns the mutex, you can be certain that there will 
 * never be a conflict from multiple workers.</p>
 * 
 * <p>Use the <code>tryLock()</code> method to take ownership of the mutex if 
 * it is available. Use the <code>lock()</code> method to pause the current 
 * worker's execution until the mutex is available, then take ownership of the 
 * mutex. Once the current worker has ownership of the mutex, it can safely 
 * operate on the shared resource. When those operations are complete, call the 
 * <code>unlock()</code> method to release the mutex. At that point the current 
 * worker should no longer access the shared resource.</p>
 *
 * <p>The Mutex class is one of the special object types that are shared 
 * between workers rather than copied between them. When you pass a mutex from 
 * one worker to another worker either by calling the Worker object's 
 * <code>setSharedProperty()</code> method or by using a MessageChannel object, 
 * both workers have a reference to the same Mutex object in the runtime's memory.</p>
 *
 * @see flash.system.Worker Worker class
 * @see flash.utils.ByteArray#shareable ByteArray.shareable property
 * @see flash.concurrent.Condition Condition class
 *
 * @langversion 3.0
 * @playerversion Flash 11.5	
 * @playerversion AIR 3.5
 */
[API(CONFIG::SWF_18)]
[native(cls="MutexClass",instance="MutexObject",gc="exact")]
final public class Mutex
{
    /**
     * Creates a new Mutex instance.
     *
     * @throws Error if the mutex could not be initialized.
     * 
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public function Mutex()
    {
        ctor();
    }

    /**
     * Pauses execution of the current worker until this mutex is available and 
     * then takes ownership of the mutex. If another worker owns the 
     * mutex when <code>lock()</code> is called, the calling worker's execution thread pauses at the 
     * <code>lock()</code> call and the worker is added to the queue of ownership 
     * requests. Once the calling worker acquires the mutex, the worker's 
     * execution continues with the line of code following the 
     * <code>lock()</code> call.
     *
     * <p>Once the current worker has ownership of the mutex, it can safely 
     * operate on the shared resource. When those operations are complete, call 
     * the <code>unlock()</code> method to release the mutex. At that point the 
     * current worker should no longer access the shared resource.</p>
     *
     * <p>Internally, a mutex keeps a count of the number of lock requests it 
     * has received. The mutex must receive the same number of unlock requests 
     * before it is completely released. If code in the worker that owns the 
     * mutex locks it again (by calling the <code>lock()</code> method) the 
     * internal lock count increases by one. You must call the 
     * <code>unlock()</code> method as many times as the number of lock requests 
     * to release ownership of the mutex.</p>
     * 
     * <p>When multiple workers are waiting for a mutex, the mutex gives priority
     * to assigning ownership to the longest-waiting worker. However, scheduling 
     * of worker threads is managed by the host operating system so there is no 
     * guarantee of a particular code execution order across workers.</p>
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public native function lock() :void

    /**
     * Acquires ownership of the mutex if it is available. If another worker 
     * already owns the mutex or another worker has called the <code>lock()</code> 
     * method and is waiting to acquire the mutex, the mutex is not available. 
     * In that case, calling this method returns <code>false</code> and code 
     * execution continues immediately.
     *
     * <p>Once the current worker has ownership of the mutex, it can safely 
     * operate on the shared resource. When those operations are complete, call 
     * the <code>unlock()</code> method to release the mutex. At that point the 
     * current worker should no longer access the shared resource.</p>
     *
     * <p>When multiple workers are waiting for a mutex, the mutex gives priority
     * to assigning ownership to the longest-waiting worker. However, scheduling 
     * of worker threads is managed by the host operating system so there is no 
     * guarantee of a particular code execution order across workers.</p>
     *
     * @return <code>true</code> if the mutex was available (and is now owned 
     * by the current worker), or <code>false</code> if the current worker did 
     * not acquire ownership of the mutex.
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public native function tryLock() :Boolean;

    /**
     * Releases ownership of this mutex, allowing any worker to acquire it and 
     * perform work on the associated resource.
     *
     * <p>Internally, a mutex keeps a count of the number of lock requests it 
     * has received. Code in a worker must call the <code>unlock()</code> 
     * method as many times as the number of lock requests in order to release 
     * ownership of the mutex.</p>
     * 
     * @throws flash.errors.IllegalOperationError when the current worker 
     *         doesn't own the mutex.
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    native public function unlock():void;
    
    /**
     * Indicates whether the use of the Mutex class is supported for the current platform.
     *
     * <p>This property is <code>true</code> if the current platform supports mutexes; otherwise
     * <code>false</code>.</p>
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public native static function get isSupported(): Boolean;

    private native function ctor() :void;
}
    
/**
 * A Condition object is a tool for sharing a resource between workers with the 
 * additional capability of pausing execution until a particular condition is 
 * satisfied. A Condition object is used in conjunction with a Mutex object, and 
 * adds additional functionality to the mutex's behavior. By working in 
 * combination with a mutex, the runtime ensures that each transition of 
 * ownership between workers is atomic.
 * 
 * <p>The following is one possible workflow for using a Condition object:</p>
 * <ol>
 *   <li>Before using a Condition object, the first worker must take ownership 
 *       of the condition's associated mutex by calling the Mutex object's 
 *       <code>lock()</code> or <code>tryLock()</code> methods.</li>
 *   <li>The worker's code operates on the shared resource until some condition 
 *       becomes false, preventing the worker from doing more work with the 
 *       shared resource. For example, if the shared resource is a set of data 
 *       to process, when there is no more data to process the worker can't 
 *       do any more work.</li>
 *   <li>At that point, call the Condition object's <code>wait()</code> method 
 *       to pause the worker's execution and release ownership of the mutex.</li>
 *   <li>At some point, a second worker takes ownership of the mutex. Because 
 *       the mutex is available, it is safe for the second worker's code to 
 *       operate on the shared resource. The second worker does whatever is 
 *       necessary to satisfy the condition so that the first worker can do its 
 *       work again. For example, if the first worker has no data to process, 
 *       the second worker could pass more data to process into the shared resource.</li>
 *   <li>At that point, the condition related to the first worker's work is now 
 *       true, so the first worker needs to be notified that the condition is 
 *       fulfilled. To notify the first worker, the second worker's code calls 
 *       the Condition object's <code>notify()</code> method or its 
 *       <code>notifyAll()</code> method.</li>
 *   <li>In addition to calling <code>notify()</code>, the second worker needs 
 *       to release ownership of the mutex. It does this either by calling the 
 *       Mutex object's <code>unlock()</code> method or the Condition object's 
 *       <code>wait()</code> method. Since the first worker called the 
 *       <code>wait()</code> method, ownership of the mutex returns to the 
 *       first worker. Code execution in the first worker then continues again 
 *       with the next line of code following the <code>wait()</code> call.</li>
 * </ol>
 * 
 * <p>The Condition class is one of the special object types that are shared 
 * between workers rather than copied between them. When you pass a condition 
 * from one worker to another worker either by calling the Worker object's 
 * <code>setSharedProperty()</code> method or by using a MessageChannel object, 
 * both workers have a reference to the same Condition object in the runtime's 
 * memory.</p>
 *
 * @see flash.concurrent.Mutex Mutex class
 * @see flash.system.Worker Worker class
 * 
 * @langversion 3.0
 * @playerversion Flash 11.5	
 * @playerversion AIR 3.5
 */
[API(CONFIG::SWF_18)]
[native(cls="ConditionClass",instance="ConditionObject",gc="exact")]
final public class Condition 
{
    /**
     * Creates a new Condition instance.
     *
     * @param mutex the mutex that the condition uses to control transitions 
    *               between workers
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public function Condition(mutex:Mutex)
    {
        ctor(mutex);
    }

    /**
     * The mutex associated with this condition.
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
     public native function get mutex():Mutex;


    /**
     * Specifies that the condition that this Condition object represents isn't 
     * satisfied, and the current worker needs to wait until it is satisfied 
     * before executing more code. Calling this method pauses the current 
     * worker's execution thread and releases ownership of the condition's mutex. 
     * These steps are performed as a single atomic operation. The worker remains 
     * paused until another worker calls this Condition object's 
     * <code>notify()</code> or <code>notifyAll()</code> methods.
     *
     * @param timeout the maximum amount of time, in milliseconds, that the 
     *                worker should pause execution before continuing. If this 
     *                value is -1 (the default) there is no no timeout and 
     *                execution pauses indefinitely.
     *
     * @return <code>true</code> if the method returned because the timeout 
     *         time elapsed. Otherwise the method returns <code>false</code>.
     *
     * @throws flash.errors.IllegalOperationError if the current worker doesn't 
     *         own this condition's mutex
     *
     * @throws ArgumentError if the <code>timeout</code> argument is less than 
     *         0 and not -1
     * 
     * @throws flash.errors.ScriptTimeoutError if the method is called from 
     *         code in the primordial worker in Flash Player and worker pauses 
     *         longer than the script timeout limit (15 seconds by default)
     *
     * @throws flash.errors.Error if the method is called and, while the calling 
     *         worker's execution is paused, the waiting worker is terminated.
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public native function wait(timeout:Number = -1) :Boolean;

    /**
     * Specifes that the condition that this Condition object represents has 
     been satisfied and that ownership of the mutex will be returned to the 
     next worker (if any) that's waiting on this condition.
     *
     * <p>Calling this method doesn't automatically release ownership of the 
     * mutex. After calling <code>notify()</code>, you should explicitly 
     * release ownership of the mutex in one of two ways: call the 
     * <code>Mutex.unlock()</code> method if the current worker doesn't need the 
     * mutex again, or call <code>wait()</code> if the worker should get 
     * ownership of the mutex again after other workers have completed their work.</p>
     *
     * <p>One the mutex's lock is released, the next worker in the queue of 
     * workers that have called the <code>wait()</code> method acquires the 
     * mutex and resumes code execution.</p>
     *
     * @throws flash.errors.IllegalOperationError if the current worker doesn't 
     *         own this condition's mutex
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public native function notify() :void;

    /**
     * Specifies that the condition that this Condition object represents has 
     * been satisfied and that ownership of the mutex will be returned to all 
     * workers that are waiting on this condition.
     * 
     * <p>Calling this method doesn't automatically release ownership of the 
     * mutex. After calling <code>notify()</code>, you should explicitly release 
     * ownership of the mutex in one of two ways: call the 
     * <code>Mutex.unlock()</code> method if the current worker doesn't need the 
     * mutex again, or call <code>wait()</code> if the worker should get 
     * ownership of the mutex again after other workers have completed their work.</p>
     *
     * <p>Once the mutex's lock is released, the waiting workers receive 
     * ownership one at a time in the order they called the <code>wait()</code> 
     * method. Each worker that has called the <code>wait()</code> method 
     * acquires the mutex in turn and resumes code execution. When that worker 
     * calls the <code>Mutex.unlock()</code> method or the <code>wait()</code> 
     * method, mutex ownership then switches to the next waiting worker. Each 
     * time mutex ownership switches between workers, the transition is performed 
     * as a single atomic operation.</p>
     *
     * @throws flash.errors.IllegalOperationError if the current worker doesn't 
     *         own this condition's mutex
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public native function notifyAll() :void;

    /**
     * Indicates whether the Condition class is supported for the current platform.
     *
     * <p><strong>Note</strong>: if the Mutex class is not supported, creation 
     * of a Condition instance is not possible and this property is <code>false</code>.</p>
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
    public native static function get isSupported(): Boolean;
    
    private native function ctor(mutex:Mutex) :void;
}

}

package avm2.intrinsics.memory
{
    /**
     * @private
     * A complete memory barrier for domainMemory (for both load and store instructions).
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
	[API(CONFIG::SWF_18)]
	[native("ConcurrentMemory::mfence")]
	public native function mfence():void;
    /**
     * @private
     * A compare and swap for domainMemory.
     * Behaves like ByteArray.atomicCompareAndSwapIntAt but operates on the current domainMemory.
     *
     * @langversion 3.0
     * @playerversion Flash 11.5	
     * @playerversion AIR 3.5
     */
	[API(CONFIG::SWF_18)]
	[native("ConcurrentMemory::casi32")]
	public native function casi32(addr:int, expectedVal:int, newVal:int):int;
}

