/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CONCURRENCYGLUE_INCLUDED
#define CONCURRENCYGLUE_INCLUDED

#if defined(DEBUG)
#define DEBUG_CONDITION_MUTEX
#endif // DEBUG


namespace avmplus {

#if defined(DEBUG_CONDITION_MUTEX)
    #define DEBUG_STATE(_x_) do { AvmLog _x_; } while(0)
#else
    #define DEBUG_STATE(_x_) do { } while(0)
#endif

    //
    // Mutexes in ActionScript are composed of a OS level
    // vmpi condition and vmpi mutex. this composition is 
    // created to allow easy interrupt of a blocked isolate 
    // to support script timeout, flashbuilder and fdb debugger 
    // call stack aquisition for a blocked isolate thread, termination, 
    // and the ability to add aid for deadlock detection. this data
    // can be sent over the debugger protocol allowing for 
    // more sophisticated IDEs to provide helpful UI.
    // 
    class GC_AS3_EXACT(MutexObject, ScriptObject)
    {
    public:
        MutexObject(VTable* vtbl, ScriptObject* delegate);
        virtual ~MutexObject();
        void lock();
        bool tryLock();
        void unlock();
        void ctor();
		ChannelItem* makeChannelItem();
        
        class State;
		
    private:
        friend class ConditionObject;

        GC_NO_DATA(MutexObject)
        DECLARE_SLOTS_MutexObject;
        FixedHeapRef<State> m_state;
    };
    
    //
    // Conditions in ActionScript are also composed of a OS level
    // vmpi condition and vmpi mutex. this composition is 
    // created to allow easy interrupt of a blocked isolate 
    // to support script timeout, flashbuilder and fdb debugger 
    // call stack aquisition for a blocked isolate thread, termination, 
    // and the ability to add aid for deadlock detection. this data
    // can be sent over the debugger protocol allowing for 
    // more sophisticated IDEs to provide helpful UI.
    // 
    class GC_AS3_EXACT(ConditionObject, ScriptObject)
    {
    public:
        ConditionObject(VTable* vtbl, ScriptObject* delegate);
        virtual ~ConditionObject();

        GCRef<MutexObject> get_mutex();
        
        void ctor(GCRef<MutexObject> mutex);
        bool wait(double timeout);
        void notify();
        void notifyAll();

		ChannelItem* makeChannelItem();
        
        class State;

    private:
        friend class MutexObject;

        GC_DATA_BEGIN(ConditionObject)
        GCMember<MutexObject> m_mutex;
        GC_DATA_END(ConditionObject)
        FixedHeapRef<State> m_state;
        
        DECLARE_SLOTS_ConditionObject;
    };

    //
    // this stores the state of the ActionScript Mutex 
    // object with a reference count.  this is done to allow
    // ActionScript Mutex objects to be passed between
    // isolates allowing multiple isolates to use the same
    // OS level Mutex for coordination.
    // 
    // InterruptibleState manages the list of WaitRecords
    // for this Mutex allowing blocking operations like lock()
    // to be interrupted for termination, debugging, or script timeout
    //
    class MutexObject::State: public FixedHeapRCObject
    {
    public:
        State();
        virtual void destroy();
        bool tryLock();
        void lock(Toplevel* toplevel);
        bool unlock();

    private:
        friend class MutexObject;
        friend class ConditionObject;
        friend class ConditionObject::State;
        // manages list of threads waiting for 
        // the lock, this is a FIFO list for acquisition
        // first one waiting on the lock gets it when it
        // is unlocked.
        struct LockWaitRecord
        {
            LockWaitRecord() 
                : next(NULL)
#ifdef DEBUG_CONDITION_MUTEX
                , threadID(VMPI_currentThread())
#endif // DEBUG_CONDITION_MUTEX
            {}

            LockWaitRecord* next;
#ifdef DEBUG_CONDITION_MUTEX
            vmpi_thread_t threadID;
#endif // DEBUG_CONDITION_MUTEX
        };

        REALLY_INLINE void lockAcquired()
        {
            DEBUG_STATE(("thread %d acquired Mutex(%d)\n", VMPI_currentThread(), m_interruptibleState.gid));
            if (m_recursionCount == 0) {
                AvmAssert(m_ownerThreadID == VMPI_nullThread());
                m_ownerThreadID = VMPI_currentThread(); 
            } else {
                AvmAssert(m_ownerThreadID == VMPI_currentThread());
            }
            m_recursionCount++;
        }

        vmpi_mutex_t m_mutex;

        // recursionCount keeps track of the number of times that
        // Mutex.lock was called. Mutex.unlock needs to be called
        // that same number of times. large storage is used here
        // to try and avoid wrap around.
        int64_t m_recursionCount; 

        // keeps track of the thread that currently holds the lock
        // and is used to report programming errors back to 
        // actionscript developers.
        vmpi_thread_t volatile m_ownerThreadID;

        // when a thread attempts to acquire this mutex and fails
        // it will be placed into a FIFO wait list. the following 
        // two members help to manage that list.
        LockWaitRecord* m_lockWaitListHead;
        LockWaitRecord* m_lockWaitListTail;

        // keeps track of the success of initializing the vmpi_mutex_t
        // this value is then used to throw an Error in actionscript
        // indicating that the mutex could not be initialized.
        // additionally, if the platform does not support workers
        // this will be set to invalid during initialization, again causing
        // actionscript to throw an Error. 
        bool m_isValid;

        // any primitive like Mutex and Condition that have blocking
        // behavior need to be interruptible to allow for script timeout,
        // termination, and debugging call stack acquisition. 
        // Mutex uses an InterruptibleState when a blocking operation,
        // like waiting on Mutex.lock aquisition, needs to be performed.
        Isolate::InterruptibleState m_interruptibleState;
    };
    
    //
    // this stores the state of the ActionScript Condition 
    // object with a reference count.  this is done to allow
    // ActionScript Condition objects to be passed between
    // isolates allowing multiple isolates to use the same
    // OS level condition for coordination.
    // 
    // InterruptableState manages the list of WaitRecords
    // for this Condition allowing blocking operations like wait()
    // to be interrupted for termination, debugging, or script timeout
    //
    class ConditionObject::State: public FixedHeapRCObject
    {
    public:
        State(MutexObject::State* mutexState);
        bool wait(int32_t millis, Isolate* isolate, Toplevel* toplevel);
        virtual void destroy();

    private:
        friend class ConditionObject;

        FixedHeapRef<MutexObject::State> m_mutexState;
        Isolate::InterruptibleState m_interruptibleState;
    };
    
    class GC_AS3_EXACT(MutexClass, ClassClosure)
    {
    public:
        MutexClass(VTable* cvtable);
        bool get_isSupported();
		
		// implemented per FP/AIR/Shell to determine mutex/condition support
		static bool getMutexSupported( GCRef<avmplus::Toplevel> toplevel );
		
    private:
        GC_NO_DATA(MutexClass)
        DECLARE_SLOTS_MutexClass;
    };

    
    class GC_AS3_EXACT(ConditionClass, ClassClosure)
    {
    public:
        ConditionClass(VTable* cvtable);
        bool get_isSupported();
    private:
        GC_NO_DATA(ConditionClass)
        DECLARE_SLOTS_ConditionClass;
    };

    //
    // This class provides a place to hang the memory fence and 
    // compare and swap functions so they can be called from 
    // ActionScript.
    // 
    class ConcurrentMemory
    {
    private:
        explicit ConcurrentMemory(); // never construct

    public:
        static void mfence(ScriptObject *obj);
        static int32_t casi32(ScriptObject *obj, int32_t addr, int32_t expectedVal, int32_t newVal);
    };

}




#endif
