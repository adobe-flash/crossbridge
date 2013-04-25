/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#include "FixedHeapUtils.cpp"
#include "Channels.cpp"
#include "VMThread.h"

#ifdef DEBUG
//#define DEBUG_LIST
#endif

#ifdef DEBUG_LIST
#define DEBUG_CONDITION_MUTEX
#define DUMP_LIST(x) { avmplus::Isolate::InterruptibleState::WaitRecord* t = x; while(t) { AvmLog("thread %d signaled %d\n", t->threadID, t->signaled); t = t->next; }}
#else
#define DUMP_LIST(x)  do { } while(0)
#endif

namespace avmplus
{

    Isolate* Aggregate::Globals::getIsolateForID(Isolate::descriptor_t giid)
    {
		Isolate* result = NULL;
		
        AvmAssert(m_lock.isLockedByCurrentThread());
        SCOPE_LOCK(m_isolateMap.m_lock) {
            FixedHeapRef<Isolate> isolateRef(NULL);
            if (m_isolateMap.LookupItem(giid, &isolateRef)) 
			{
                AvmAssert(isolateRef->RefCount() > 0);
                result = isolateRef;
            } 
        }
		
        return result;
    }

    Aggregate::Globals::IsolateMap::IsolateMap(int initialSize)
	: FixedHeapHashTable<Isolate::descriptor_t, FixedHeapRef<Isolate> >(initialSize)
    {}

    Aggregate::Globals::IsolateMap::~IsolateMap()
    {
        Clear();
        // The destructor defined by the base class will call Clear(), but at that time
        // the object vtable will be that of the base class, so the overriden DestroyItem() can't be seen and won't 
        // be called! We'll call Clear() here first, the subsequent Clear() in the base class will have nothing to do. 
    }


    Aggregate::Aggregate()
        : m_primordialGiid(Isolate::INVALID_DESC) // eventually there will be many of those
        , m_activeIsolateCount(0)
        , m_inShutdown(false)
        , m_activeIsolateThreadMap(15)
    {}

    Aggregate::~Aggregate()
    {
    }

    void Aggregate::destroy()
    {
        mmfx_delete(this);
    }
    
    void Aggregate::signalOutOfMemoryShutdown()
    {
        requestAggregateExit();
		vmbase::SafepointRecord::cleanupAfterOOM();
    }

    bool Aggregate::requestIsolateExit(Isolate::descriptor_t desc, Toplevel* currentToplevel)
    {
        bool result = false;
        SCOPE_LOCK_NAMED(locker, m_globals.m_lock) {
            Isolate* isolate = m_globals.getIsolateForID(desc);
            // if the isolate isn't found in the global table terminate 
            // has been called previously, skip the rest.
            if (isolate == NULL) { 
                return false; 
            }

            // if the isolate is in CANSTART, STARTING or RUNNING state it should be interrupted
            if (isolate->m_state > Isolate::NEW && isolate->m_state <= Isolate::RUNNING) {
                isolate->interrupt();
            }
            result = true;

            // if the isolate has not been started i.e. it is still
            // in the NEW or CANSTART state then leave it in the table,
            // it will then be properly disposed of during aggregate exit.
            if (isolate->m_state > Isolate::CANSTART) {
                SCOPE_LOCK(m_globals.m_isolateMap.m_lock) { 
				    m_globals.orphanFor(desc);
                    m_globals.m_isolateMap.RemoveItem(desc);
                }
            }

            // if the isolate is in a NEW, CANSTART, FAILED, ABORTED, TERMINATED, 
            // or EXCEPTION state then we can just exit here
            if (isolate->m_state <= Isolate::CANSTART || isolate->m_state > Isolate::FINISHING) {
                return false;
            }

			AvmCore* core = isolate->m_core;
            if (core == currentToplevel->core()) {
                // calling terminate on this thread:
                // no need to interrupt or signal as
                // the thread is currently active (not blocked)
                goto throw_terminated_error;
            } else if (core != NULL) {
                // interrupt actionscript 
                core->raiseInterrupt(AvmCore::ExternalInterrupt);
                // Hope for the best? the thread being interrupted won't necessarily fence
                vmbase::MemoryBarrier::readWrite();
            }
            isolate->releaseActiveResources();
        }
        return result;

throw_terminated_error:
        throwWorkerTerminatedException(currentToplevel);
        AvmAssert(false);
        return false;
    }


    void Aggregate::requestAggregateExit()
    {
            // this iterator interrupts every isolate's core
            class IsolateCoreInterrupt: public Globals::IsolateMap::Iterator
            {
                Aggregate* m_aggregate;
            public:
                IsolateCoreInterrupt(Aggregate* aggregate): m_aggregate(aggregate) {}
				
                virtual void each(Isolate::descriptor_t , FixedHeapRef<Isolate> isolate) 
                {
                    if (m_aggregate == isolate->m_aggregate && !isolate->isInterrupted())
					{
                        if (isolate->m_state <= Isolate::CANSTART) {
							m_aggregate->stateTransition(isolate, Isolate::TERMINATED);
						}
						else {

							if (isolate->m_state < Isolate::FAILED) {
								AvmCore* core = isolate->m_core;
								// Ensure that the write to AvmCore::interrupted is visible to the other thread.

								// in some shutdown cases the core is already NULL so we have to check for
								// this situation.
								if (core != NULL) {
									core->raiseInterrupt(AvmCore::ExternalInterrupt);
								}
								isolate->interrupt();
							}
						}
                    }
                }
            };
            
            // this iterator releses resources for every isolate's core
            class ReleaseResources: public Globals::IsolateMap::Iterator
            {
                Aggregate* m_aggregate;
            public:
                ReleaseResources(Aggregate* aggregate): m_aggregate(aggregate) {}
				
                virtual void each(Isolate::descriptor_t, FixedHeapRef<Isolate> isolate) 
                {
                    if (m_aggregate == isolate->m_aggregate)
						isolate->releaseActiveResources();
                }
            };
		
		
		SCOPE_LOCK(m_globals.m_lock) {
            m_inShutdown = true;
           // we need to make two passes here 
            //  (1) needs to make sure that all cores are interrupted so that the very next
            //      thing they do is unwind, we don't want to continue executing if they could
            //      possibly be in an ideteriminate state (get unblocked from a Mutex or Condition
            //      when the condition hasn't been met.
            //  (2) needs to release resources (unlock any held Mutex or Condition, wake any blocked
            //      workers, clear shared properties, etc)
            IsolateCoreInterrupt iInterrupt(this);
            ReleaseResources iRelease(this);
            SCOPE_LOCK(m_globals.m_isolateMap.m_lock) {
                m_globals.m_isolateMap.ForEach(iInterrupt);
                m_globals.m_isolateMap.ForEach(iRelease);
            }
        }
    }

    /* virtual */
    void Aggregate::throwWorkerTerminatedException(Toplevel* currentToplevel)
    {
        AvmCore* core = currentToplevel->core();
        AvmAssert(core->getIsolate()->getAggregate() == this);
        Stringp errorMessage = core->getErrorMessage(kWorkerTerminated);
        GCRef<ErrorObject> error = currentToplevel->errorClass()->constructObject(errorMessage->atom(), core->intToAtom(kWorkerTerminated));
        Exception *exception = new (core->GetGC()) Exception(core, error->atom());
        exception->flags |= Exception::EXIT_EXCEPTION;
        exception->flags |= Exception::SUPPRESS_ERROR_REPORT;
        core->throwException(exception);
    }
    
    void Aggregate::processWorkerInterrupt(Toplevel* currentToplevel)
    {
        AvmCore* core = currentToplevel->core();
        if (!core->interruptCheckReason(AvmCore::ScriptTimeout)) {
            throwWorkerTerminatedException(currentToplevel);
        }
        else {
            core->clearInterrupt(); // clear existing interrupt before throwing
            core->interrupt(currentToplevel, AvmCore::ScriptTimeout);
        }
    }

    Isolate::Isolate(Isolate::descriptor_t desc, Isolate::descriptor_t parentDesc, Aggregate* aggregate)
        : m_desc(desc)
        , m_parentDesc(parentDesc)
        , m_core(NULL)
        , m_interruptibleState(NULL)
        , m_aggregate(aggregate)
        , m_state(Isolate::NEW)
        , m_failed(false)
        , m_interrupted(false)
    {
		AvmAssert(m_desc != INVALID_DESC && m_desc != POISON_DESC);
        VMPI_recursiveMutexInit(&m_interruptibleStateMutex);
    }

    void Aggregate::stateTransition(Isolate* isolate, Isolate::State to)
    {
        SCOPE_LOCK_NAMED(locker, m_globals.m_lock) {
            enum Isolate::State from = isolate->m_state;
            isolate->m_state = to;
#ifdef _DEBUG
            vmbase::VMThread* t = NULL;
#endif // _DEBUG
            bool isolateInactive = false;
            if (to == Isolate::CANSTART) {
                AvmAssert(from == Isolate::NEW);
            } else if (to == Isolate::STARTING) {
                AvmAssert(from == Isolate::CANSTART);
            } else if (to == Isolate::RUNNING) {
                AvmAssert(from < to); // FIXME this can be violated (?)
                AvmAssert(isolate->getDesc() == m_primordialGiid || m_activeIsolateThreadMap.LookupItem(isolate->getDesc(), &t) == true); 
                AvmAssert(isolate->m_core != NULL);
            } else if (to == Isolate::EXCEPTION) {
                AvmAssert(from > Isolate::NEW);
                setIsolateAsInterrupted(isolate);
                isolate->stopRunLoop();
                isolateInactive = true;
            } else if (to == Isolate::FAILED) {
                AvmAssert(from == Isolate::NEW);
                isolate->m_failed = true;
                isolateInactive = true;
            } else if (to == Isolate::FINISHING) {
                AvmAssert(from == Isolate::CANSTART || from == Isolate::RUNNING || from == Isolate::STARTING || from == Isolate::FINISHING);
                AvmAssert(isolate->getDesc() == m_primordialGiid || m_activeIsolateThreadMap.LookupItem(isolate->getDesc(), &t) == true); 
				if (from != Isolate::FINISHING)
				{
                    setIsolateAsInterrupted(isolate);
					isolate->stopRunLoop();
				}
            } else if (to == Isolate::TERMINATED) {
                AvmAssert(from == Isolate::RUNNING || from == Isolate::STARTING || from == Isolate::FINISHING || from == Isolate::NEW);
                setIsolateAsInterrupted(isolate);
                isolateInactive = true;
			}
            
            if (isolateInactive) {
                m_activeIsolateCount--;
                locker.notifyAll();
            }
            
            isolate->stateChanged(to);
        }
    }

    void Aggregate::setIsolateAsInterrupted(Isolate* isolate) 
    {
        Isolate::lockInSafepoint(&isolate->m_interruptibleStateMutex);
        {
            Isolate::InterruptibleState* state = isolate->m_interruptibleState;
            if (state) {
                SCOPE_LOCK(state->getMonitor()) {
                    isolate->m_interrupted = true;
                }
            }
            else {
                isolate->m_interrupted = true;
            }
        }
        VMPI_recursiveMutexUnlock(&isolate->m_interruptibleStateMutex);
    }

    
    Isolate::SharedPropertyMap::SharedPropertyMap()
    {
    }
    
    Isolate::SharedPropertyMap::~SharedPropertyMap()
    {
        Clear();
        // The destructor defined by the base class will call Clear(), but at that time
        // the object vtable will be that of the base class, so the overriden DestroyItem() can't be seen and won't 
        // be called! We'll call Clear() here first, the subsequent Clear() in the base class will have nothing to do. 
    }
    
    uintptr_t Isolate::SharedPropertyMap::HashKey(Isolate::SharedPropertyNamep key) const
    {
        return String::hashCodeLatin1(key->values, key->length);
    }
    
    bool Isolate::SharedPropertyMap::KeysEqual(Isolate::SharedPropertyNamep k1, const Isolate::SharedPropertyNamep k2) const
    {
        if (k1->length != k2->length)
            return false;
        int32_t length = k1->length;
        for (int32_t j = 0; j < length; j++)
        {
            if (k1->values[j] != k2->values[j])
                return false;
        }
        return true;
    }

    void Isolate::releaseActiveResources()
    {
		SCOPE_LOCK(m_sharedPropertyLock) {
			// make sure that we don't hold a reference to something that could prevent 
			// resources from being collected and released, like a locked Mutex, waiting Condition, 
			// or a reference to ourself 
			m_sharedProperties.Clear();
		}
        
        // if we have any active Mutexes or Conditions release them.
        // this is safe because we have already interrupted our core and the very next execution
        // point should be to throw an external interrupt error
        signalInterruptibleState();
    }

    void Isolate::SharedPropertyMap::DestroyItem(Isolate::SharedPropertyNamep key, ChannelItem* item)
    {
        mmfx_delete(item);
        key->deallocate(); 
        mmfx_delete(key);
    }
    
    void Isolate::setSharedProperty(const StUTF8String& key, ChannelItem* item)
    {
        SCOPE_LOCK(m_sharedPropertyLock) {
            Isolate::SharedPropertyNamep keyInternal = mmfx_new(FixedHeapArray<char>());
			const int32_t cLen = key.length();
            char* values = mmfx_new_array(char, cLen);
            VMPI_memcpy(values, key.c_str(), cLen * sizeof(char));
            keyInternal->values = values;
            keyInternal->length = cLen;
            ChannelItem* previous;
            if (m_sharedProperties.LookupItem(keyInternal, &previous)) {
                m_sharedProperties.RemoveItem(keyInternal); // this will dealloc old key
            }
            if (item) {
                m_sharedProperties.InsertItem(keyInternal, item);
            } else {
                keyInternal->deallocate();
                mmfx_delete(keyInternal); // m_properties doesn't own the key
                // We already removed.
            }
        }
        // else the key is owned by the hashmap
    }

    bool Isolate::getSharedProperty(const StUTF8String& key, ChannelItem** item)
    {
		bool result = false;
        SCOPE_LOCK(m_sharedPropertyLock) {
            Isolate::SharedPropertyNamep keyInternal = mmfx_new(FixedHeapArray<char>());
            keyInternal->values = (char*)key.c_str(); // it's OK, we won't touch it
            keyInternal->length = key.length();
            result = m_sharedProperties.LookupItem(keyInternal, item);
            keyInternal->values = NULL;
            mmfx_delete(keyInternal);
        }
        return result;
    }

    ChannelItem* Isolate::makeChannelItem(Toplevel* toplevel, Atom atom)
    {
        class DoubleChannelItem: public ChannelItem
        {
        public:
            DoubleChannelItem(double value) : m_value(value) {}

            Atom getAtom(Toplevel* toplevel) const
            {
                return toplevel->core()->doubleToAtom(m_value);
            }

        private:
            double m_value;
        };

        class IntPtrChannelItem: public ChannelItem
        {
        public:
            IntPtrChannelItem(intptr_t value):m_value(value) {}

            Atom getAtom(Toplevel* toplevel) const
            {
                (void)toplevel;
                return atomFromIntptrValue(m_value);
            }

        private:
            intptr_t m_value;
        };

        class BooleanChannelItem: public ChannelItem
        {
        public:
            BooleanChannelItem(bool value):m_value(value) {}

            Atom getAtom(Toplevel* toplevel) const
            {
                (void)toplevel;
                return m_value ? trueAtom : falseAtom;
            }

        private:
            bool m_value;
        };

        class ScriptObjectChannelItem: public ChannelItem
        {
        public:
            ScriptObjectChannelItem(Toplevel* toplevel, Atom value)
            {
                GCRef<ByteArrayObject> ba = toplevel->byteArrayClass()->constructByteArray();
                ba->writeObject(value);
		        ByteArray::Buffer* buffer = ba->GetByteArray().getUnderlyingBuffer();
			    ByteArray::Buffer* copy = mmfx_new(ByteArray::Buffer);
			    copy->capacity = buffer->capacity;
			    copy->length = buffer->length;
			    if (buffer->array) {
				    copy->array = mmfx_new_array_opt(uint8_t, buffer->capacity, MMgc::kCanFailAndZero);
                    if (copy->array) {
				        VMPI_memcpy(copy->array, buffer->array, buffer->length);
                    }
			    } else {
				    copy->array = NULL;
			    }
                m_value = copy;
            }

            Atom getAtom(Toplevel* toplevel) const
            {
                ByteArrayClass* cls = toplevel->byteArrayClass();
		        GCRef<ByteArrayObject> byteArray = new (toplevel->gc(), MMgc::kExact) ByteArrayObject(cls->ivtable(), cls->prototypePtr(), m_value);
                return byteArray->readObject();
            }

        private:
            FixedHeapRef<ByteArray::Buffer> m_value;
        };

        ChannelItem* item = NULL;
        Atom kind = atomKind(atom);
            
        if (kind == kDoubleType) {
            item = mmfx_new(DoubleChannelItem(toplevel->core()->atomToDouble(atom)));
        } else if (kind == kIntptrType) {
            item = mmfx_new(IntPtrChannelItem(atomGetIntptr(atom)));
        } else if (kind == kBooleanType) {
            item = mmfx_new(BooleanChannelItem(atom == trueAtom ? true : false));
        } else if (kind == kSpecialBibopType) {
            // no value
        } else if (AvmCore::istype(atom, toplevel->byteArrayClass()->ivtable()->traits)) {
            ByteArrayObject* ba = static_cast<ByteArrayObject*>(toplevel->core()->atomToScriptObject(atom));
			item = ba->makeChannelItem();
        } else if (AvmCore::istype(atom, toplevel->builtinClasses()->get_MutexClass()->ivtable()->traits)) {
            MutexObject* mutexObj = static_cast<MutexObject*>(toplevel->core()->atomToScriptObject(atom));
			item = mutexObj->makeChannelItem();
        } else if (AvmCore::istype(atom, toplevel->builtinClasses()->get_ConditionClass()->ivtable()->traits)) {
            ConditionObject* conditionObj = static_cast<ConditionObject*>(toplevel->core()->atomToScriptObject(atom));
			item = conditionObj->makeChannelItem();
        } else {
        	item = mmfx_new(ScriptObjectChannelItem(toplevel, atom));
        }
        return item;
    }
    
    // this method is used to determine if a currently active
    // wait record should be reactivated. reactivating a waiting
    // record should only happen if the signal occured because 
    // the debugger needs to get a call stack.
    bool Isolate::retryInterruptibleState()
    {
        return false;
    }

    void Isolate::signalInterruptibleState()
    {
        Isolate::lockInSafepoint(&m_interruptibleStateMutex);
        if (m_interruptibleState) {
            // We need to notify all threads
            // as we do not know where in the
            // list of threads our blocked thread
            // may be, we only know that it is
            // blocked on this state.
            // 
            // This does mean that in some instances
            // a Mutex.lock() that is blocking several
            // threads will wake all of them and 
            // they will get another chance to contend
            // for that thread.
            //
            // In the case of Condition.wait() threads
            // will be awoken and will merely start 
            // waiting again.
            m_interruptibleState->signal();
        }
        VMPI_recursiveMutexUnlock(&m_interruptibleStateMutex);
    }

    void Isolate::initialize(AvmCore* core) 
    {
        AvmAssert(AvmCore::getActiveCore() == core);
        m_core = core; 
        core->setIsolate(this);
    }
    
    void Isolate::copyByteCode(ByteArrayObject* byteCode)
    {
         if (byteCode) {
             ByteArray& bytes = byteCode->GetByteArray();
             m_code.allocate(1);
             m_code.values[0].allocate(bytes.GetLength());
             m_code.values[0].initialize(bytes.GetReadableBuffer());
		 }
    }
    
    bool Isolate::hasFailed() const
    {
        return m_failed;
    }

    bool Isolate::interrupt()
    {
        bool wasInterrupted = m_interrupted;
        if (!wasInterrupted) {
            m_aggregate->stateTransition(this, Isolate::FINISHING);
        }
        return wasInterrupted;
    }

    bool Isolate::isInterrupted() const
    {
        return m_interrupted;
    }

    bool Isolate::isMemoryManagementShutDown() const
    {
        AvmAssert(AvmCore::getActiveCore() == m_core);
        // In the player it can happen when m_state == FINISHING
        return m_state > FINISHING;
    }
        
    bool Isolate::isPrimordial() const
    {
        return m_aggregate->isPrimordial(m_desc);
    }
     
    void Isolate::run() 
    {
        // Make sure the isolate survives for the duration of the doRun() call by keeping a ref to it.
        FixedHeapRef<Isolate> handle(this);
		DecrementRef();		// Let go of the hand-over-hand-safety-ref set by spawnIsolateThread() now that handle is valid.

        // don't run if interrupted
        if (m_interrupted) {
            m_aggregate->stateTransition(this, Isolate::TERMINATED);
        }
        else {
            m_aggregate->stateTransition(this, Isolate::STARTING);
		    doRun();
        }
    }

    Isolate::~Isolate()
    {
		AvmAssert(m_interruptibleState == NULL);
        AvmAssert(RefCount() == 0);
        if (m_code.length > 0) {
            for (int i = 0; i < m_code.length; i++)
                m_code.values[i].deallocate();
            m_code.deallocate();
        }
		
        m_aggregate->cleanupIsolate(this);
        VMPI_recursiveMutexDestroy(&m_interruptibleStateMutex);
	}

    // Caution, can be triggered by GC sweep or incremental collection.
    void Isolate::destroy()
    {
        mmfx_delete(this);
    }

    /* static */
    void Isolate::lockInSafepoint(vmpi_mutex_t* mutex)
    {
        if (VMPI_recursiveMutexTryLock(mutex) == false) {
            vmbase::SafepointRecord* const current = vmbase::SafepointRecord::current();
            if (current && !current->isSafe()) {
                VMPI_callWithRegistersSaved(lockInSafepointGate, (void*)mutex);
            } else {
                assert(!vmbase::SafepointRecord::hasCurrent() || vmbase::SafepointRecord::current()->manager()->inSafepointTask());
                VMPI_recursiveMutexLock(mutex);
            }
        }
    }

    /*static*/
    void Isolate::lockInSafepointGate(void* stackPointer, void* mutex)
    {
        vmbase::SafepointGate gate(stackPointer);
        VMPI_recursiveMutexLock((vmpi_mutex_t*)mutex);
    }


#ifdef DEBUG_INTERRUPTIBLE_STATE
    int Isolate::InterruptibleState::globalId = 0;
#endif // DEBUG_INTERRUPTIBLE_STATE


    Isolate::InterruptibleState::Enter::ActiveInterruptibleStateHelper::ActiveInterruptibleStateHelper(Isolate* isolate, InterruptibleState* state)
        : m_isolate(isolate)
    {
        AvmAssert(m_isolate != NULL);
        AvmAssert(state != NULL);
        m_mutex = &m_isolate->m_interruptibleStateMutex;
        Isolate::lockInSafepoint(m_mutex);
        m_isolate->m_interruptibleState = state;
    #ifdef DEBUG_INTERRUPTIBLE_STATE
        gid = state->gid;
    #endif // DEBUG_INTERRUPTIBLE_STATE
    }

    Isolate::InterruptibleState::Enter::ActiveInterruptibleStateHelper::~ActiveInterruptibleStateHelper()
    {
        Isolate::lockInSafepoint(m_mutex);
        m_isolate->m_interruptibleState = NULL;
        unlock();
    }

    REALLY_INLINE void Isolate::InterruptibleState::Enter::ActiveInterruptibleStateHelper::unlock() const
    {
        VMPI_recursiveMutexUnlock(m_mutex);
    }

    REALLY_INLINE Isolate* Isolate::InterruptibleState::Enter::ActiveInterruptibleStateHelper::getIsolate() const
    {
        return m_isolate;
    }
    //
    // InterruptibleState Enter
    //
    Isolate::InterruptibleState::Enter::Enter(WaitRecord& record, InterruptibleState* state, Isolate* isolate)
        : interrupted(false)
        , result(false)
        , m_stateSetter(isolate, state) // asserts state != NULL
        , m_monitor(state->m_condition) // acquires m_condition lock
        , m_state(state)
        , m_waitRecord(&record)
    {
        AvmAssert(vmbase::SafepointRecord::hasCurrent());
        AvmAssert(m_state->getMonitor().isLockedByCurrentThread());
        // ensure that m_condition lock is held before allowing 
        // a call to Isolate::signal() to continue or
        // reading the current state of the Isolate's reference
        // to this InterruptibleState
        m_stateSetter.unlock();

        if (m_state->m_head == NULL) {
            m_state->m_head = m_waitRecord;
        }
        else {
            m_state->m_tail->next = m_waitRecord;
        }
        m_state->m_tail = m_waitRecord;

    }

    Isolate::InterruptibleState::Enter::~Enter()
    {
        AvmAssert(m_state->getMonitor().isLockedByCurrentThread());
        // walk through the list and remove current wait record from it
        WaitRecord* cur = m_state->m_head;
        WaitRecord* prev = NULL;
        do {
            if (cur == m_waitRecord) {
                if (prev == NULL) {
                    m_state->m_head = cur->next;
                } 
                else {
                    prev->next = cur->next;
                }

                if (cur->next == NULL) {
                    m_state->m_tail = prev;
                }
                break;
            } 
            prev = cur;
            cur = cur->next;
        } while (cur);
        DUMP_LIST((m_state->m_head));
    }

    void Isolate::InterruptibleState::Enter::notify()
    {
        DEBUG_STATE(("thread %d is calling notify on (%d)\n", VMPI_currentThread(), m_stateSetter.gid));
        m_monitor.notify();
    }

    void Isolate::InterruptibleState::Enter::wait(int32_t millis)
    {
        AvmAssert(m_state->getMonitor().isLockedByCurrentThread());
        // we might have to wait again if we get signaled and we are
        // not the target isolate, i.e. our associated isolate isn't
        // being terminated or signaled.

        Isolate* isolate = m_stateSetter.getIsolate();
        // In order to signal any waiting workers the mutex for this
        // state must be acquired, and cannot be acquired while any
        // thread is executing in this block. The following code
        // is provided to insure proper execution in the following
        // situation:
        //  * a signal is issued.
        //  * before any existing signaled threads wake 
        //  * wait is called for this thread
        //  * this thread owns the condition
        // In this situation only a check that this state's associated isolate
        // is not being terminated is required. If it is being terminated/interrupted
        // then no wait should be attempted, if not then it is safe to wait.
        if (isolate->isInterrupted() || isolate->getAvmCore()->interruptCheckReason(AvmCore::ScriptTimeout))
        {
            interrupted = true;
            // no need to set m_waitRecord->signaled = false since 
            // the record will be removed from the list after returning
            // and the waiter count won't be updated, this also means 
            // that signaled watiers count will not be affected.
            return;
        }

        VMPI_atomicIncAndGet32(&m_state->m_waiterCount);
        bool continueWait = false;
        uint64_t endTime = millis != -1 ? VMPI_getTime() + millis : 0;    
        DEBUG_STATE(("thread %d is sleeping (iso-id:%d)\n", VMPI_currentThread(), isolate->getDesc()));

        do {
            if (millis == -1) {
                m_monitor.wait();
            }
            else {
                // if signaled during a timed wait return
                // to waiting for the remaining time
                uint64_t now = VMPI_getTime();
                if (now < endTime) {
                    result = m_monitor.wait(int32_t(endTime - now));
                }
                else if (!continueWait)
				{
					// In order to ensure that the lock is released even though the wait time
					// has expired, a wait() for the minimum time must be done here. 
                    result = m_monitor.wait(0);
                }
				else
				{
					// continueWait can only be true iff a wait has occurred.
					result = true;
                    continueWait = false;
				}
            }

            // the value of interrupt tells us if the isolate was terminated OR 
            // if a script timeout has fired; script timeouts only happen for the primordial isolate. 
	        interrupted = isolate->isInterrupted() || isolate->getAvmCore()->interruptCheckReason(AvmCore::ScriptTimeout);

            // when signaled a thread will return to a blocked
            // waiting state if it's associated isolate has not
            // been terminated.
            // threads are only signaled because of a termination request, 
            // caused by either a call to terminate or shutdown, otherwise
            // they are notified by user code. 
            // when signaled each waiting thread must check its associated 
            // isolate to see if it will need to return to a waiting state. 
            // to distingush between a user code notify/notifyAll and a signal
            // each state tracks the number of waiting and signaled threads.  
            // a signal "cycle" is completed when all threads waiting at 
            // the moment of the signal call have woken and taken action (either exit 
            // or return to a waiting state).
            // the cycle is completed when the signaled waiters count reaches zero.
            //
            // there is the possibility that a timed wait expired and tried to
            // acquire the condition but could not because a call to signal()
            // has already acquired it.  in this situation the record would
            // remain signaled and the signaled waiters value needs to be 
            // decremented but no continuation of waiting should occur.
            if (m_waitRecord->signaled) {
                AvmAssert(m_state->m_signaledWaiters > 0);
                continueWait = !interrupted && !result;
                m_waitRecord->signaled = false;
                m_state->m_signaledWaiters--;
            }
        } 
        while (continueWait);

        VMPI_atomicDecAndGet32(&m_state->m_waiterCount);
        AvmAssert(m_state->getMonitor().isLockedByCurrentThread());

        DEBUG_STATE(("thread %d is awake (iso-id:%d)\n", VMPI_currentThread(), isolate->getDesc()));
        DUMP_LIST((m_state->m_head));
    }

    //
    // InterruptibleState 
    //
    Isolate::InterruptibleState::InterruptibleState()
        : m_waiterCount(0)
        , m_signaledWaiters(0)
        , m_head(NULL)
        , m_tail(NULL)
    {
#ifdef DEBUG_INTERRUPTIBLE_STATE
        gid = ++globalId;
#endif // DEBUG_INTERRUPTIBLE_STATE
    }

    bool Isolate::InterruptibleState::hasWaiters()
    {
        bool result = vmbase::AtomicOps::compareAndSwap32WithBarrierPrev(0, 0, reinterpret_cast<volatile int32_t*>(&m_waiterCount)) != 0;
        return result;
    }

    //
    // this method is used to wake all threads blocked on
    // this state, each thread will check to see if its
    // associated isolate has been terminated or if it
    // should return to a waiting state.
    //
    // a signaled "cycle" refers to the condition in which
    // all threads have woken up, exited or returned to a 
    // waiting state. a count of waiting threads is kept to 
    // determine when a signal "cycle" has completed.
    //
    // a signal "cycle" determines whether the spurious waking
    // event is caused by terminate/shutdown versus user
    // code calling notify/notifyAll
    //
    void Isolate::InterruptibleState::signal()
    {
        DEBUG_STATE(("thread %d is calling signal on (%d)\n", VMPI_currentThread(), gid));

        SCOPE_LOCK_NAMED(cond, m_condition) {
            // a notifyAll must occur for each call to signal
            // otherwise shutdown can be stopped waiting for
            // blocked workers.  if a signal cycle is already
            // in progress wait until it is completed and then
            // start a new one.
            while (m_signaledWaiters > 0) {
                cond.wait(0);
            }
            
            if (m_waiterCount > 0) {
                m_signaledWaiters = m_waiterCount;
                // mark all waiting threads as signaled so only those
                // threads will check state and possibly wait again
                // other threads will simply ignore the wake request
                // and return immediately to a waiting state.
                WaitRecord* cur = m_head;
                while (cur) {
                    cur->signaled = true;
                    cur = cur->next;
                }
                cond.notifyAll();
            }
        }
    }

    void Isolate::InterruptibleState::notify()
    {
        SCOPE_LOCK_NAMED(cond, m_condition) {
            // wait until the current signal cycle completes
            // if there is one.
            while (m_signaledWaiters > 0) {
                cond.wait(0);
            }
            DEBUG_STATE(("thread %d is calling notify on (%d)\n", VMPI_currentThread(), gid));
            cond.notify();
        }
    }

    void Isolate::InterruptibleState::notifyAll()
    {
        SCOPE_LOCK_NAMED(cond, m_condition) {
            // wait until the current signal cycle completes
            // if there is one.
            while (m_signaledWaiters > 0) {
                cond.wait(0);
            }

            DEBUG_STATE(("thread %d is calling notifyAll on (%d)\n", VMPI_currentThread(), gid));
            cond.notifyAll();
        }
    }

    //
    // Aggregate
    //

    //
    // ActiveIsolateThreadMap
    //

	Aggregate::ActiveIsolateThreadMap::ActiveIsolateThreadMap(int initialSize)
	    : FixedHeapHashTable<Isolate::descriptor_t, vmbase::VMThread*>(initialSize)
	{
	}
	
    //
    // any threads that are still active will be joined and cleaned up
    //
	void Aggregate::ActiveIsolateThreadMap::cleanup()
	{
        class IsolateIterator: public ActiveIsolateThreadMap::Iterator
        {
        public:
            virtual void each(Isolate::descriptor_t, vmbase::VMThread* thread)
            {
                thread->join();         
                mmfx_delete(thread);
            }
        };

        IsolateIterator iter;
        ForEach(iter);
        Clear();
	}
	
    bool Aggregate::spawnIsolateThread(Isolate* isolate)
    {
		bool result = false;
		
        SCOPE_LOCK(m_globals.m_lock) {
#ifdef _DEBUG
            vmbase::VMThread* t;
#endif // _DEBUG

            AvmAssert(m_activeIsolateThreadMap.LookupItem(isolate->getDesc(), &t) == false);
            vmbase::VMThread* thread = mmfx_new(vmbase::VMThread(isolate));
			// increment the refcount on the isolate as a hand-over-hand-safety measure to ensure that 
			// when the thread spins up the isolate will still be around for the thread to 
			// take a ref to the isolate.
 			isolate->IncrementRef();		// run() is responsible for dec ref

            if (thread->start()) {
                stateTransition(isolate, Isolate::CANSTART);
				m_activeIsolateThreadMap.InsertItem(isolate->getDesc(), thread);
				result = true;
            } else {
				isolate->DecrementRef();	// thread will not start, so undo the increments
                // We will never try to spawn this isolate again.
				stateTransition(isolate, Isolate::FAILED);
				mmfx_delete(thread);
                result = false;
            }
        }
        return result;
    }

    void Aggregate::cleanupIsolate(Isolate* isolate)
    {
        vmbase::VMThread* thread = NULL;
        if (m_activeIsolateThreadMap.LookupItem(isolate->getDesc(), &thread)) {
            if (thread != vmbase::VMThread::currentThread()) {
				thread->join(); // shouldn't block
                mmfx_delete(thread);
                // thread has been deleted, so remove it from the hashtable
                // otherwise it will get leaked.
                // @TODO: to keep the table small add a periodic cleanup routine
			    m_activeIsolateThreadMap.RemoveItem(isolate->getDesc());
            }
        }
    }

    Isolate* Aggregate::getIsolate(Isolate::descriptor_t desc)
    {
		Isolate* result = NULL;
        SCOPE_LOCK(m_globals.m_lock) {
            result = m_globals.getIsolateForID(desc);
        }
        return result;
    }
    
    Isolate::State Aggregate::queryState(Isolate* isolate) 
    {
        AvmAssert(isolate != NULL);
		
		Isolate::State result = Isolate::NONE;
		SCOPE_LOCK(m_globals.m_lock) {
            result = isolate->m_state;
        }
		
		return result;
    }

    Isolate* Aggregate::newIsolate(Isolate* parent)
    {
		Isolate* newIsolate = NULL;
		
        SCOPE_LOCK(m_globals.m_lock) {
            if (!m_inShutdown) {

				const Isolate::descriptor_t giid = m_globals.getNewID(); 
				AvmAssert(giid != Isolate::POISON_DESC);	// this can only happen if there are Isolate::POISON_DESC currently running workers or a bug.

				newIsolate = Isolate::newIsolate(giid, 
					 parent ? parent->getDesc() : Isolate::INVALID_DESC, this);

				SCOPE_LOCK(m_globals.m_isolateMap.m_lock) {
					m_globals.m_isolateMap.InsertItem(giid, FixedHeapRef<Isolate>(newIsolate));
				}
#ifdef _DEBUG
                vmbase::VMThread* t;
#endif // _DEBUG
				AvmAssert(m_activeIsolateThreadMap.LookupItem(giid, &t) == false);
				AvmAssert(parent == NULL || AvmCore::getActiveCore()->getIsolate() == parent);
				if (parent == NULL) {
					// Primordial isolate, already active.
					m_primordialGiid = giid;
				}
				m_activeIsolateCount++;
			}
        }
		return newIsolate;
    }

    void Aggregate::initialize(AvmCore* targetCore, Isolate* current)
    {
        AvmAssert(AvmCore::getActiveCore() == targetCore);
        SCOPE_LOCK_NAMED(locker, m_globals.m_lock) {
            current->initialize(targetCore);
            // if terminate is called before we actually start we will not 
            // have an entry in the global table
            AvmAssert(m_globals.getIsolateForID(current->getDesc()) == NULL || m_globals.getIsolateForID(current->getDesc())->m_state > Isolate::NEW);
        }
        AvmAssert(targetCore->getIsolate() == current);
    }

    EnterSafepointManager::EnterSafepointManager(AvmCore* core)
	: m_safepointMgr(0)
    {
        Isolate* isolate = core->getIsolate();
        if (isolate) {
			// hang onto the aggregate during the safepont to ensure it doesn't go away.
		    m_aggregate = isolate->getAggregate();
			
			m_safepointMgr = m_aggregate->safepointManager();
			m_spRecord.setLocationAndDesc( (int32_t*)&core->interrupted, core->getIsolateDesc() ); 

            AvmAssert(m_safepointMgr != NULL);
			m_safepointMgr->enter(&m_spRecord);
		}
    }
    
    EnterSafepointManager::~EnterSafepointManager()
    {
        leaveSafepoint();
    }

    void EnterSafepointManager::leaveSafepoint() 
    {
		if (m_safepointMgr)
		{
			m_safepointMgr->leave(&m_spRecord);
			m_spRecord.setLocationAndDesc( NULL, Isolate::INVALID_DESC );
		}
    }

    void Aggregate::beforeCoreDeletion(Isolate* current) {
        // if we terminate before the core is running we may not have one
        AvmAssert(AvmCore::getActiveCore() == NULL || AvmCore::getActiveCore()->getIsolate() == current);
        
        SCOPE_LOCK(m_globals.m_lock) {
#ifdef DEBUG
            {
                Isolate* checked = m_globals.getIsolateForID(current->getDesc());
                AvmAssert(checked == NULL || checked == current);
            }
#endif
            if (current->m_core) {
                current->m_core->setIsolate(NULL);
            }
        }
    }

    void Aggregate::afterGCDeletion(Isolate* current) {
        SCOPE_LOCK(m_globals.m_lock) {
#ifdef _DEBUG
            vmbase::VMThread* t;
#endif // _DEBUG
            AvmAssert(current->getDesc() == m_primordialGiid || m_activeIsolateThreadMap.LookupItem(current->getDesc(), &t) == true); 
            if (current->m_state != Isolate::EXCEPTION && current->m_state != Isolate::FAILED) {
                stateTransition(current, Isolate::TERMINATED); 
            }

			current->releaseActiveResources();
            current->m_core = NULL;
        }
    }

    void Aggregate::waitUntilNoIsolates()
    {
        SCOPE_LOCK_NAMED(locker, m_globals.m_lock) {
            while (m_activeIsolateCount > 0) {
                locker.wait();
            }
            
			// Empty out the map of all the Isolate references so they can go away.
			m_globals.m_isolateMap.Clear();
		}
		
		m_activeIsolateThreadMap.cleanup();
    }

    GCRef<ObjectVectorObject> Aggregate::listWorkers(Toplevel* toplevel)
    {
        GCRef<ClassClosure> workerClass = toplevel->workerClass();
        GCRef<ObjectVectorObject> workerVector = toplevel->vectorClass()->newVector(workerClass);
        
        class IsolateLister: public Globals::IsolateMap::Iterator
        {
		private:
            GCRef<ObjectVectorObject> m_workerVector;
            Toplevel* m_toplevel;
            int m_index;
			
        public:
            IsolateLister(GCRef<ObjectVectorObject> workerVector, Toplevel* toplevel)
                : m_workerVector(workerVector)
                , m_toplevel(toplevel)
                , m_index(0)
            {}
            virtual void each(Isolate::descriptor_t, FixedHeapRef<Isolate> isolate) 
            {
                // Only list workers that are in the RUNNING state
				// We don't care about having a lock on the isolate here because
				// the state is fluid around this call -- it can be chagned as soon as
				// we would be done asking and before the list is returned. Client is
				// responsible for double checking the result
                if (isolate->m_state == Isolate::RUNNING)
                {
                    GCRef<ScriptObject> interned = m_toplevel->getInternedObject(isolate);
                    if (interned == NULL) {
                        interned = isolate->newWorkerObject(m_toplevel);
                    }
                    m_workerVector->setUintProperty(m_index++, interned->atom());
                }
            }
        };
        
        IsolateLister lister(workerVector, toplevel);
        SCOPE_LOCK(m_globals.m_isolateMap.m_lock) {
            m_globals.m_isolateMap.ForEach(lister);
        }
        
        return workerVector;
    }


    void Aggregate::runSafepointTaskHoldingIsolateMapLock(vmbase::SafepointTask* task)
    {
        SCOPE_LOCK(m_globals.m_isolateMap.m_lock) {
            safepointManager()->requestSafepointTask(*task);
        }
    }
    

    void Aggregate::reloadGlobalMemories()
    {
        // This is a heavyhanded approach and reloads all the global memories
        // known, because the ByteArray subscriber mechanism can't yet handle
        // cross-GC-heap references.
        class Reloader: public Globals::IsolateMap::Iterator
        {
            virtual void each(Isolate::descriptor_t, FixedHeapRef<Isolate> isolate) 
            {
                AvmCore* core = isolate->getAvmCore();
				if (isolate->m_state == Isolate::RUNNING)
				{
					for (uint32_t i = 0, n = core->m_domainEnvs.length(); i < n; ++i)
					    {
						    DomainEnv* domainEnv = core->m_domainEnvs.get(i);
						    if (domainEnv) {
							    ByteArrayObject* mem = domainEnv->get_globalMemory();
							    if (mem)
							    {
								    mem->GetByteArray().NotifySubscribers();
							    }
						    }
					    }
                    }
				}
        };
     
        AvmAssert(m_globals.m_isolateMap.m_lock.isLockedByCurrentThread());
        Reloader reloader;
        m_globals.m_isolateMap.ForEach(reloader);
    }

}


