/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{        
    MostlyNonBlockingChannel::MostlyNonBlockingChannel(MMgc::GC* gc)
	: eof(false), get_tries(0), put_tries(0), head(0), tail(0), fallback(NO_FB)
    {
        bufferData = new BufferData(gc, this, INIT_LENGTH);
        overflowBufferData = NULL;
    }

    MostlyNonBlockingChannel::~MostlyNonBlockingChannel()
    {
        // The following will deregister bufferData as GCRoot.  The
        // GCRoot list is protected by a spinlock, so it should be OK
        // to call from the main thread of another GC.
        delete bufferData;
    }

    void MostlyNonBlockingChannel::close()
    {
        SCOPE_LOCK_NAMED(lk, m_monitor) {
            eof = true;
            lk.notifyAll();
        }
    }
        
    void MostlyNonBlockingChannel::unregisterRoot()
    {
        bufferData->Destroy();
        
        SCOPE_LOCK_NAMED(lk, m_monitor) {
            if (overflowBufferData != NULL) {
                overflowBufferData->Destroy();
            }
        }
    }

    bool MostlyNonBlockingChannel::isClosed() 
    {
        VMPI_memoryBarrier();
        return eof;
    }

    bool MostlyNonBlockingChannel::isEmpty() 
    { 
        return (head == tail); 
    }

    bool MostlyNonBlockingChannel::isFull() 
    {
        return ((head + 1) % bufferData->length) == tail;
    }

    bool MostlyNonBlockingChannel::put(const Atom &in)
    {
        AvmAssert(put_tries == 0);
        do {
            do {
                if (eof) {
                    put_tries = 0;
                    return false;
                }
                if (put_tries++ == TRIES ||
                    overflowBufferData != NULL) // putting data into overflow buffer
                    break;
                
                int32_t old_h = head;
                if (((old_h + 1) % bufferData->length) == tail) {
                    // buffer full
                    if (fallback != NO_FB) {
                        // fake "full"
                        break;
                    }
                    else {
                        // wait for get() to retrieve an item
                        VMPI_threadSleep(0);
                        continue;
                    }
                }
                else {
                    // buffer is not full
                    bufferData->m_items[old_h] = in;
                    int32_t h = (old_h + 1) % bufferData->length;
                    if (VMPI_compareAndSwap32WithBarrier(old_h, h, &head)) {
                        put_tries = 0;
                        return true;
                    }
                    // if CAS failed, it means that get() intervened
                    // and set state to both full and fallback
                    AvmAssert(isFull() && fallback != NO_FB);
                    break;
                }
            } while (true);
            
            // really full or "fake" full
            SCOPE_LOCK_NAMED(lk, m_monitor) {
                
                if (overflowBufferData != NULL) {
                    AvmAssert(fallback != NO_FB);
                    AvmAssert(isEmpty()); // original buffer is "fake empty"
                    
                    // resize when head reached length-1 to leave 1
                    // empty element in case overflow buffer will be
                    // turned into regular circular buffer right after
                    // completely filling up
                    if (overflowHead == overflowBufferData->length - 1) {
                        resizeOverflowBuffer();
                    }
                    overflowBufferData->m_items[overflowHead] = in;
                    overflowHead++;
                    // if get() is in progress then let it clean up
                    // the overflow while this thread sleeps; if we
                    // tried resizing on the fly, we would risk
                    // exposing get() to inconsistent state
                    if (get_tries > 0) {
                        lk.wait();
                    }
                    // one way or the other, the item has been
                    // inserted into the channel successfully - return
                    break;
                }
                
                int32_t fb = fallback;                    
                if (fb != NO_FB) {
                    // get() went to sleep (should be full - marked by
                    // get() as such)
                    AvmAssert(isFull());
                    // fb contains value of head store by get()
                    head = fb;
                    AvmAssert(!isFull());
                    bufferData->m_items[head] = in;
                    head = (head + 1) % bufferData->length;
                    lk.notify();
                    fallback = NO_FB;
                    break;
                }
                
                int32_t t = tail;
                if (!(((head + 1) % bufferData->length) == t)) {
                    // no longer full (get() can only change head
                    // within lock-based critical section so we can
                    // use it directly) - retry
                    continue;
                }
                else {
                    // make sure that non-blocking get is not going to
                    // happen between now and when we go to sleep
                    int32_t empty_t = head;
                    // remember the actual value of tail
                    fallback = t;
                    if (VMPI_compareAndSwap32WithBarrier(t, empty_t, &tail)) {
                        // get() hasn't happened in the meantime
                        if (eof) {
                            put_tries = 0;
                            return false;
                        }
                        if (overflowBufferData == NULL) {
                            createOverflowBuffer();
                        }
                        overflowBufferData->m_items[overflowHead] = in;
                        overflowHead++;
                        // see comments in the first conditional of
                        // this critical section
                        if (get_tries > 0) {
                            lk.wait();
                        }
                        break;
                    }
                    else {
                        // get() did happen - retry
                        fallback = NO_FB;
                        continue;
                    }
                }
            }
        } while (true);
        put_tries = 0;
        return true;
    }
    
    bool MostlyNonBlockingChannel::get(Atom *outp)
    {
        AvmAssert(get_tries == 0);
        do {
            do {                    
                if (eof) {
                    get_tries = 0;
                    return false;
                }
                if (get_tries++ == TRIES)
                    break;                    
                
                int32_t old_t = tail;
                if (head == old_t) {
                    // buffer empty
                    if (fallback != NO_FB) {
                        break;
                    }
                    else {
                        // wait for put() to insert an item
                        VMPI_threadSleep(0);
                        continue;
                    }
                }
                else {
                    // buffer is not empty
                    *outp = bufferData->m_items[old_t];
                    int32_t t = (old_t + 1) % bufferData->length;
                    if (VMPI_compareAndSwap32WithBarrier(old_t, t, &tail)) {
                        get_tries = 0;
                        return true;
                    }
                    // if CAS failed, it means that put() intervened
                    // and set state to both empty and fallback
                    AvmAssert(isEmpty() && fallback != NO_FB);
                }
            } while (true);
            
            // really empty or "fake" empty
            SCOPE_LOCK_NAMED(lk, m_monitor) {
                
                int32_t fb = fallback;                    
                if (fb != NO_FB) {
                    // fb contains value of tail store by put()
                    AvmAssert(overflowBufferData != NULL);
                    *outp = overflowBufferData->m_items[0];
                    tail = 1; // just removed the first element
                    head = overflowHead;
                    bufferData->Destroy();
                    delete bufferData;
                    bufferData = overflowBufferData;
                    overflowBufferData = NULL;
                    lk.notify();
                    fallback = NO_FB;
                    break;
                }
		
                int32_t h = head;
                if (h != tail) {
                    // no longer empty (put() can only change tail
                    // within lock-based critical section so we can
                    // use it directly) - retry
                    continue;
                }
                else {
                    // make sure that non-blocking put is not going to
                    // happen between now and when we go to sleep
                    int32_t full_h = (tail == 0 ? bufferData->length - 1 : tail - 1);
                    // remember the actual value of head
                    fallback = h;
                    if (VMPI_compareAndSwap32WithBarrier(h, full_h, &head)) {
                        // put() hasn't happened in the meantime
			
                        // since the channel is indeed empty, reset
                        // all stale references to objects that still
                        // reside in it
                        bufferData->cleanup();
                        if (eof) {
                            get_tries = 0;
                            return false;
                        }
                        // cannot assert the following because the
                        // channel is "fake full" at this point
                        // (though it is in fact empty)
                        // AvmAssert(!available());
                        lk.wait();
                    } 
                    else {
                        // put() did happen - retry
                        fallback = NO_FB;
                        continue;
                    }
                }
            }
        } while (true);
        get_tries = 0;
        return true;
    }
    
    /*
     * Must not be used concurrently with get(),including inside of
     * the implementation of get().
     */
    bool MostlyNonBlockingChannel::available()
    {
        VMPI_memoryBarrier();
        bool empty = isEmpty();
        if (empty) {
            // may be a "fake" empty as a result of put()
            SCOPE_LOCK_NAMED(lk, m_monitor) {
                empty = isEmpty();
                if (empty && (fallback != NO_FB)) return head != fallback;
                else return !empty;
            }
        }
        return !empty;
    }
    
    void MostlyNonBlockingChannel::registerWithGC(MMgc::GC* gc)
    {
        bufferData->Register(gc);
    }
    
    MMgc::GC* MostlyNonBlockingChannel::gc()
    {
        return bufferData->GetGC();
    }
            
    MostlyNonBlockingChannel::BufferData::BufferData(MMgc::GC* gc,
                                                     MostlyNonBlockingChannel* ch,
                                                     int32_t size) :
        GCRoot(gc, MMgc::kExact), length(size), channel(ch)
    {
        m_items = mmfx_new_array(Atom, length);
        cleanup();
    }
            
    MostlyNonBlockingChannel::BufferData::~BufferData()
    {
        mmfx_delete_array(m_items);
    }
    
    /*virtual*/ bool MostlyNonBlockingChannel::BufferData::gcTrace(MMgc::GC* gc, size_t cursor)
    {
        (void)cursor;
        for (int i = 0; i < length; i++) {
            if (m_items[i] != unreachableAtom) { 
                // Although TraceAtom() should be able to handle this.
                gc->TraceAtom(&m_items[i]);
            }
        }
        return false;
    }

    void MostlyNonBlockingChannel::BufferData::cleanup()
    {
        VMPI_memset(m_items, unreachableAtom, length*sizeof(Atom));
    }
    

    void MostlyNonBlockingChannel::createOverflowBuffer()
    {
        int newLength = bufferData->length + INIT_LENGTH;
        AvmAssert(bufferData->GetGC() != NULL);
        overflowBufferData = new BufferData(bufferData->GetGC(), this, newLength);
        // one last element is unused when the buffer is full
        for(int i=0; i < bufferData->length-1; i++) {
            overflowBufferData->m_items[i] = bufferData->m_items[(fallback + i) % bufferData->length];
        }
        for(int i=bufferData->length-1; i<newLength; i++) {
            overflowBufferData->m_items[i] = unreachableAtom;
        }
        overflowHead = bufferData->length-1;
    }

    void MostlyNonBlockingChannel::resizeOverflowBuffer()
    {
        BufferData* oldBufferData = overflowBufferData;
        int oldLength = overflowBufferData->length;
        int newLength = oldLength + INIT_LENGTH;
        AvmAssert(oldBufferData->GetGC() != NULL);
        overflowBufferData = new BufferData(oldBufferData->GetGC(), this, newLength);
        for(int i=0; i < oldLength - 1; i++) {
            overflowBufferData->m_items[i] = oldBufferData->m_items[i];
        }
        for(int i=oldLength - 1; i<newLength; i++) {
            overflowBufferData->m_items[i] = unreachableAtom;
        }
        oldBufferData->Destroy();
        delete oldBufferData;
    }
    
    NoSyncSingleItemBuffer::NoSyncSingleItemBuffer(MMgc::GC* gc) : m_hasData(false)
    {
        bufferData = new BufferData(gc, this);
    }
	
    NoSyncSingleItemBuffer::~NoSyncSingleItemBuffer()
    {
        delete bufferData;
    }
        
    bool NoSyncSingleItemBuffer::isEmpty()
    {
        return !m_hasData;
    }

    bool NoSyncSingleItemBuffer::isFull()
    {
        return m_hasData;
    }
        
    bool NoSyncSingleItemBuffer::put(const Atom &in)
    {
        if (isFull())
            return false;
        bufferData->m_item = in;
        m_hasData = true;
        return true;
    }
    
    bool NoSyncSingleItemBuffer::get(Atom *outp)
    {
        if (isEmpty())
            return false;
        m_hasData = false;
        *outp = bufferData->m_item;
        return true;
    }
    
    void NoSyncSingleItemBuffer::registerWithGC(MMgc::GC* gc)
    {
        bufferData->Register(gc);
    }
    
    MMgc::GC* NoSyncSingleItemBuffer::gc() {
        return bufferData->GetGC();
    }
    
    void NoSyncSingleItemBuffer::unregisterRoot() {
        bufferData->Destroy();
    }
    
    NoSyncSingleItemBuffer::BufferData::BufferData(MMgc::GC* gc, NoSyncSingleItemBuffer* buf)
        : GCRoot(gc, MMgc::kExact), m_item(0), buffer(buf) 
    {
    }

    /*virtual*/ bool NoSyncSingleItemBuffer::BufferData::gcTrace(MMgc::GC* gc, size_t cursor)
    {
        (void)cursor;                
        gc->TraceAtom(&m_item);
        return false;
    }



    NoSyncMultiItemBuffer::NoSyncMultiItemBuffer()
        : m_hasData(false), head(0), tail(0), bufLength(BUF_LENGTH)
    {
        m_items = mmfx_new_array_opt(const ChannelItem*, bufLength, MMgc::kZero);
    }
    
    NoSyncMultiItemBuffer::~NoSyncMultiItemBuffer()
    {
        const ChannelItem* p = NULL;
        while(get(&p))
        {
            mmfx_delete((ChannelItem*)p);
        }
        mmfx_delete_array(m_items);
    }
                
    bool NoSyncMultiItemBuffer::isEmpty()
    {
        return (head == tail) && !m_hasData;
    }
    
    bool NoSyncMultiItemBuffer::isFull()
    {
        return (head == tail) && m_hasData;
    }
    uint32_t NoSyncMultiItemBuffer::numItemsInBuffer()
    {
        uint32_t retVal = 0;
        if (isEmpty())
        {
            retVal = 0;
        }
        else if (isFull())
        {
            retVal = bufLength;
        }
        else if (head > tail)
        {
            retVal = head - tail;
        }
        //  The circular buffer may have a tail greater than head
        else 
        {
            retVal = head + bufLength - tail;
        }
        return retVal;
    }
    
    bool NoSyncMultiItemBuffer::put(const ChannelItem* in)
    {
        if (isFull())
		{
			//  Increase by BUF_LENGTH
			uint32_t newLength = bufLength + BUF_LENGTH;
			const ChannelItem** newBuf = mmfx_new_array_opt(const ChannelItem*, newLength, MMgc::kZero);
			uint32_t it = 0;
			//  Copy the old buffer into this one
			while (get(&newBuf[it]))
		    {
				it++;
			}
			tail = 0;
			head = it;
			mmfx_delete_array(m_items);
			m_items = newBuf;
			bufLength = newLength;
		}
		m_items[head] = in;
        head = (head + 1) % bufLength;
        m_hasData = true; // just do it every time instead of checking
        return true;
    }
        
    bool NoSyncMultiItemBuffer::get(const ChannelItem** outp)
    {
        if (isEmpty())
            return false;
        *outp = m_items[tail];
        m_items[tail] = NULL;
        tail = (tail + 1) % bufLength;
        if (tail == head)
            m_hasData = false;
        return true;
    }
    	

    NonBlockingMultiItemBuffer::NonBlockingMultiItemBuffer(MMgc::GC* gc)
        : head(0), tail(0)
    {
        bufferData = new BufferData(gc, this);
    }
    
    NonBlockingMultiItemBuffer::~NonBlockingMultiItemBuffer()
    {
        delete bufferData;
    }

    bool NonBlockingMultiItemBuffer::isEmpty() 
    { 
        return (head == tail); 
    }
    
    bool NonBlockingMultiItemBuffer::isFull() 
    {
        return ((head + 1) % BUF_LENGTH) == tail;
    }
    
    bool NonBlockingMultiItemBuffer::put(const Atom &in)
    {	
        if (isFull()) return false;
        
        bufferData->m_items[head] = in;
        head = (head + 1) % BUF_LENGTH;
        VMPI_memoryBarrier();
        return true;
    }
        
    bool NonBlockingMultiItemBuffer::get(Atom *outp)
    {
        if (isEmpty()) return false;
        
        *outp = bufferData->m_items[tail];
        tail = (tail + 1) % BUF_LENGTH;
        VMPI_memoryBarrier();
        return true;
    }
    
    void NonBlockingMultiItemBuffer::registerWithGC(MMgc::GC* gc)
    {
        bufferData->Register(gc);
    }
    
    MMgc::GC* NonBlockingMultiItemBuffer::gc() {
        return bufferData->GetGC();
    }
    
    void NonBlockingMultiItemBuffer::unregisterRoot() {
        bufferData->Destroy();
    }
    
    NonBlockingMultiItemBuffer::BufferData::BufferData(MMgc::GC* gc, NonBlockingMultiItemBuffer* buf)
        : GCRoot(gc, MMgc::kExact), buffer(buf)
    {
        for (int i = 0; i < BUF_LENGTH; i++) {
            m_items[i] = unreachableAtom;
        }
    }
    
    /*virtual*/ bool NonBlockingMultiItemBuffer::BufferData::gcTrace(MMgc::GC* gc, size_t cursor)
    {
        (void)cursor;
        for (int i = 0; i < BUF_LENGTH; i++) {
            gc->TraceAtom(&m_items[i]);
        }
        return false;
    }



}
