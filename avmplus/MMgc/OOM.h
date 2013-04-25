/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __OOM_H__
#define __OOM_H__

#define MMGC_ENTER_VOID                         \
    MMgc::GCHeap::EnterLock();                  \
    if(MMgc::GCHeap::ShouldNotEnter())          \
    {                                           \
        MMgc::GCHeap::EnterRelease();           \
        return;                                 \
    }                                           \
    MMgc::EnterFrame _ef;                       \
    MMgc::GCHeap::EnterRelease();               \
    _ef.status = VMPI_setjmpNoUnwind(_ef.jmpbuf);            \
    if(_ef.status != 0)                         \
        return;


#define MMGC_ENTER_VOID_NO_GUARD                \
    MMgc::GCHeap::EnterLock();                  \
    MMgc::EnterFrame _ef;                       \
    MMgc::GCHeap::EnterRelease();               \
    _ef.status = VMPI_setjmpNoUnwind(_ef.jmpbuf);            \
    if(_ef.status != 0)                         \
        return;


#define MMGC_ENTER_RETURN_NO_GUARD(_val)        \
    MMgc::GCHeap::EnterLock();                  \
    MMgc::EnterFrame _ef;                       \
    MMgc::GCHeap::EnterRelease();               \
    _ef.status = VMPI_setjmpNoUnwind(_ef.jmpbuf);            \
    if(_ef.status != 0)                         \
        return _val;


#define MMGC_ENTER_RETURN(_val)                 \
    MMgc::GCHeap::EnterLock();                  \
    if(MMgc::GCHeap::ShouldNotEnter())          \
    {                                           \
        MMgc::GCHeap::EnterRelease();           \
        return _val;                            \
    }                                           \
    MMgc::EnterFrame _ef;                       \
    MMgc::GCHeap::EnterRelease();               \
    _ef.status = VMPI_setjmpNoUnwind(_ef.jmpbuf);            \
    if(_ef.status != 0)                         \
        return _val;

#define MMGC_ENTER_SUSPEND MMgc::SuspendEnterFrame _efSuspend

namespace MMgc
{
    class AbortUnwindObject
    {

        public:
            AbortUnwindObject() : next(NULL), previous(NULL)  {}
            virtual ~AbortUnwindObject()
#ifdef DEBUG
                    ;
#else
                    {}
#endif
            virtual void Unwind(){/*noop in base*/}

        private:
            AbortUnwindObject *next;
            AbortUnwindObject *previous;

        friend class EnterFrame;
    };

    class EnterFrame
    {
        friend class GCHeap;
        friend class FixedMalloc;
    public:
        EnterFrame();
        ~EnterFrame();
        jmp_buf jmpbuf;
        int status;
        void Destroy() { m_heap = NULL; }
        GC* GetActiveGC() { return m_gc; }
        GC* SetActiveGC(GC *gc) { GC* prev = m_gc; m_gc = gc; return prev; }

        void UnwindAllObjects();

        //  It is the caller's responsibility to make sure the AbortUnwindObject has not already been added to this list
        //  This method is not thread safe.
        void AddAbortUnwindObject(AbortUnwindObject *obj);

        //  It is the caller's responsibility to make sure the AbortUnwindObject has previously been added to the list
        //  This method is not thread safe.
        void RemoveAbortUnwindObject(AbortUnwindObject *obj);

#ifdef DEBUG
        static bool IsAbortUnwindObjectInList(AbortUnwindObject *obj);
#endif

        bool Suspended() { return m_suspended; }
        void Resume() { m_suspended = false; }
        void Suspend() { m_suspended = true; }
        EnterFrame *Previous() { return m_previous; }

    private:
        GCHeap *m_heap;
        GC *m_gc;
        AbortUnwindObject *m_abortUnwindList;
        EnterFrame *m_previous;
        bool m_suspended;
    };

    class SuspendEnterFrame
    {
    public:
        SuspendEnterFrame();
        ~SuspendEnterFrame();
    private:
        EnterFrame *m_ef;
    };

    typedef enum _MemoryStatus {
        //  Events (never manifest as permanent status on GCHeap
        kFreeMemoryIfPossible,
        //  Status Values
        kMemNormal,
        kMemSoftLimit,
        kMemAbort
    } MemoryStatus;

    /**
     * Mutator oom callback mechanism, subclass and call GCHeap::AddOOMCallback
     */
    class OOMCallback
    {
    public:
        virtual ~OOMCallback() {}
        virtual void memoryStatusChange(MemoryStatus oldStatus, MemoryStatus newStatus) = 0;
    };
}

#endif /* __OOM_H__ */

