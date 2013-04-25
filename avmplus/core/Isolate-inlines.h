/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Isolate_inlines__
#define __avmplus_Isolate_inlines__

#include "FixedHeapUtils-inlines.h"
#include "Channels-inlines.h"


namespace avmplus
{
    
	//
	// WorkerObjectBase<T>
	//
    template<class T>
    void WorkerObjectBase<T>::throwIllegalOperationError(int errorID)
    {
        AvmCore* core = AvmCore::getActiveCore();
        Toplevel* toplevel = core->codeContext()->domainEnv()->toplevel();
		toplevel->builtinClasses()->get_IllegalOperationErrorClass()->throwError(errorID);
	}

    template <class T>
    WorkerObjectBase<T>::WorkerObjectBase()
        : m_isolate(NULL)
    {}

    template <class T>
    T* WorkerObjectBase<T>::self()
    {
        return static_cast<T*>(this);
    }

    template<class T>
    void WorkerObjectBase<T>::initialize(Isolate *isolate)
    {
        if (isolate == NULL)
        {
            Isolate* parent = self()->core()->getIsolate();
            
            // the following consumes a shared resource (m_globals entry).
            isolate = parent->getAggregate()->newIsolate(parent);
            // Can isolate have been deallocated in the meantime? No,
            // because an attempt must be first made to run the isolate
            // and the current isolate is the only one that knows about
            // this new isolate so far.
            // At aggregate termination we'll probably kill all the isolates first.

            // the only way this should happen is if we run out of ids 
            // (you'd need > 2 billion active workers), memory, or aggregate is in shutdown 
			AvmAssert(isolate != NULL || parent->getAggregate()->inShutdown()); 
		}

        if (isolate) {
            setIsolate(isolate);
        }
    }

    template<class T>
    GCRef<ScriptObject> WorkerObjectBase<T>::setIsolate(Isolate* isolate)
    {
        if (m_isolate == NULL) {
            m_isolate = isolate;
        } else {
            AvmAssert(m_isolate == isolate);
        }
        AvmAssert(self()->toplevel()->getInternedObject(m_isolate) == NULL); // always check the intern table first
        self()->toplevel()->internObject(m_isolate, self());
		return self();
    }

    
    template<class T>
	Isolate::descriptor_t WorkerObjectBase<T>::descriptor() const
    {
        // the isolate can be NULL in the situation where a
        // worker is busy creating workers during a shutdown cycle,
        // as the logic for creating an isolate will return NULL 
        // in that situation.
        return m_isolate ? m_isolate->getDesc() : Isolate::INVALID_DESC;
    }
    
    template<class T>
    WorkerObjectBase<T>::~WorkerObjectBase()
    {
        m_isolate = NULL;
    }

    template<class T>
    bool WorkerObjectBase<T>::isParentOf(WorkerObjectBase* worker)
    {
        if (worker == NULL)
			self()->toplevel()->checkNull(worker, "worker");
        if (m_isolate->getDesc() == Isolate::INVALID_DESC 
            || worker->m_isolate == NULL)
			self()->toplevel()->throwArgumentError(kInvalidArgumentError);

        return worker->m_isolate->getParentDesc() == m_isolate->getDesc();
    }

    template<class T>
    bool WorkerObjectBase<T>::isPrimordial() const
    {
        // the isolate can be NULL in the situation where a
        // worker is busy creating workers during a shutdown cycle,
        // as the logic for creating an isolate will return NULL 
        // in that situation.
        if (m_isolate)
        {
            AvmAssert(m_isolate->getDesc() >= 0);
            // if this isolate has had terminate called on it before the run loop
            // has started then its m_isolate value could be NULL since we may
            // be calling this method from Worker.current (see setDescriptors)
            return m_isolate->isPrimordial();
        }
        else
        {
            return false;
        }
	}

    template <class T>
    void WorkerObjectBase<T>::start()
    {
        // because AS thunkers cannot call virtual methods on all
        // platforms one level of indirection is used.
        internalStart();
    }

    template <class T>
    Stringp WorkerObjectBase<T>::get_state()
    {
        // the isolate can be NULL in the situation where a
        // worker is busy creating workers during a shutdown cycle,
        // as the logic for creating an isolate will return NULL 
        // in that situation.
        if (m_isolate)
        {
            Aggregate* aggregate = m_isolate->getAggregate();
            Isolate::State code = aggregate->queryState(m_isolate);
            AvmAssert(code >= Isolate::NEW && code <= Isolate::EXCEPTION);
            if (code == Isolate::CANSTART || code == Isolate::STARTING)
            {
                code = Isolate::NEW;
            }
            else if (code == Isolate::FINISHING)
            {
                code = Isolate::RUNNING;
            }
            return self()->core()->workerStates[code];
        }
        else
        {
            return self()->core()->workerStates[Isolate::ABORTED];
        }
     }
    
    template <class T>
    bool WorkerObjectBase<T>::internalStop() 
    {
        // the isolate can be NULL in the situation where a
        // worker is busy creating workers during a shutdown cycle,
        // as the logic for creating an isolate will return NULL 
        // in that situation.
        if (m_isolate)
        {
            Aggregate* aggregate = m_isolate->getAggregate();
            if (aggregate->isPrimordial(m_isolate->getDesc()))  {
                aggregate->throwWorkerTerminatedException(self()->toplevel());
                return true; // not reached
            } else {
                return aggregate->requestIsolateExit(m_isolate->getDesc(), self()->toplevel());
            }
        }
        else
        {
            return false;
        }
    }
    

    template <class T>
    void WorkerObjectBase<T>::setSharedProperty(String* key, Atom value)
    {
        // the isolate can be NULL in the situation where a
        // worker is busy creating workers during a shutdown cycle,
        // as the logic for creating an isolate will return NULL 
        // in that situation.
        if (m_isolate)
        {
            StUTF8String buf(key);
            ChannelItem* item = m_isolate->makeChannelItem(self()->toplevel(), value);
            m_isolate->setSharedProperty(buf, item);
        }
    }

    template <class T>
    Atom WorkerObjectBase<T>::getSharedProperty(String* key)
    {
        // the isolate can be NULL in the situation where a
        // worker is busy creating workers during a shutdown cycle,
        // as the logic for creating an isolate will return NULL 
        // in that situation.
        if (m_isolate)
        {
		    Atom result = undefinedAtom;
		    StUTF8String buf(key);
		    ChannelItem* item;
		    const bool cOk = m_isolate->getSharedProperty(buf, &item);
		    if (cOk) 
		    {
			    result =  item->getAtom(self()->toplevel());
		    }

            return result;
        }
        else
        {
            return undefinedAtom;
        }
    }

    template <class T>
    void WorkerObjectBase<T>::internalStart()
    {
        // the isolate can be NULL in the situation where a
        // worker is busy creating workers during a shutdown cycle,
        // as the logic for creating an isolate will return NULL 
        // in that situation.
        if (m_isolate)
        {
            if (m_isolate->getAggregate()->queryState(m_isolate) != Isolate::NEW)
                throwIllegalOperationError(kWorkerAlreadyStarted);
            
            if (m_isolate->getParentDesc() != AvmCore::getActiveCore()->getIsolate()->getDesc())
                throwIllegalOperationError(kWorkerIllegalCallToStart);

            if (m_isolate->hasFailed())
                throwIllegalOperationError(kFailedWorkerCannotBeRestarted);

            Aggregate* aggregate = m_isolate->getAggregate();
            
            ByteArrayObject* byteCode = self()->getByteCode();
            // stash away in non-gc'd memory
            // If byteCode == NULL, will try to get from primordial (shell only).
            m_isolate->copyByteCode(byteCode);
            self()->clearByteCode();
            aggregate->spawnIsolateThread(m_isolate);
        }
    }

    template <class T>
    T* WorkerDomainObjectBase<T>::self()
    {
        return static_cast<T*>(this);
    }

    template <class T>
    ObjectVectorObject* WorkerDomainObjectBase<T>::listWorkers() 
    {
        return self()->core()->getIsolate()->getAggregate()->listWorkers(self()->toplevel());
    }
	

	//
	// Aggregate
	//
	REALLY_INLINE bool Aggregate::isPrimordial(Isolate::descriptor_t giid) const
    {
        return giid == m_primordialGiid;
    }
	
	
}

#endif /* __avmplus_Isolate_inlines__ */

