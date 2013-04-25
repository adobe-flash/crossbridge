/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Channels_inlines__
#define __avmplus_Channels_inlines__

namespace avmplus
{
        
    template<class BUFFER>
    BlockingChannel<BUFFER>::BlockingChannel() : eof(false)
    {
        buffer = new BUFFER();
    }
    
    template<class BUFFER>
    BlockingChannel<BUFFER>::~BlockingChannel()
    {
        delete buffer;
    }

    template<class BUFFER>
    void BlockingChannel<BUFFER>::close()
    {
        SCOPE_LOCK_NAMED(lk, m_monitor) {
            eof = true;
            lk.notifyAll();
        }
    }
    
    
    template<class BUFFER>
    bool BlockingChannel<BUFFER>::isClosed() 
    {
        SCOPE_LOCK(m_monitor) {
            return eof;
        }
        NOT_REACHED;
    }

    template<class BUFFER>
    bool BlockingChannel<BUFFER>::isEmpty() 
    {
        bool res;
        SCOPE_LOCK(m_monitor) {
            res =  buffer->isEmpty();
        }
        return res;
    }

    template<class BUFFER>
    bool BlockingChannel<BUFFER>::isFull() 
    { 
        bool res;
        SCOPE_LOCK(m_monitor) {
            res = buffer->isFull();
        }
        return res;
    }
    
    template<class BUFFER>
    uint32_t BlockingChannel<BUFFER>::numItemsInBuffer() 
    { 
        uint32_t res = 0;
        SCOPE_LOCK(m_monitor) {
            res = buffer->numItemsInBuffer();
        }
        return res;
    }

    template<class BUFFER>
    bool BlockingChannel<BUFFER>::available()
    {
        bool res = false;
        SCOPE_LOCK(m_monitor) {
            res = !buffer->isEmpty();
        }
        return res;
    }

    template<class BUFFER>
    bool BlockingChannel<BUFFER>::put(const ChannelItem* in)
    {
        SCOPE_LOCK_NAMED(lk, m_monitor) {
            if (eof) return false;
            while (!buffer->put(in)) {
                if (eof) {
                    return false;
                }
                lk.wait();
            }
            lk.notify();
            return true;
        }
        NOT_REACHED;
    }
        
    template<class BUFFER>
    bool BlockingChannel<BUFFER>::get(const ChannelItem** outp)
    {
        SCOPE_LOCK_NAMED(lk, m_monitor) {
            while (buffer->isEmpty() && !eof)
                lk.wait();
            if (!buffer->get(outp))
                return false;
            lk.notify();
            return true;
        }
        NOT_REACHED;
    }
        

    template<class BUFFER>
    NonBlockingChannel<BUFFER>::NonBlockingChannel(MMgc::GC* gc) : eof(false)
    {
        buffer = new BUFFER(gc);
    }
       
    template<class BUFFER>
    NonBlockingChannel<BUFFER>::~NonBlockingChannel()
    {
        delete buffer;
    }

    template<class BUFFER>
    void NonBlockingChannel<BUFFER>::close()
    {
        eof = true;
        VMPI_memoryBarrier();
    }
        
    template<class BUFFER>
    void NonBlockingChannel<BUFFER>::unregisterRoot() {
        buffer->unregisterRoot();
    }

    template<class BUFFER>
    bool NonBlockingChannel<BUFFER>::isClosed() 
    {
        VMPI_memoryBarrier();
        return eof;
    }

    template<class BUFFER>
    bool NonBlockingChannel<BUFFER>::isEmpty() 
    { 
        VMPI_memoryBarrier();
        return buffer->isEmpty();
    }

    template<class BUFFER>
    bool NonBlockingChannel<BUFFER>::isFull() 
    { 
        VMPI_memoryBarrier();
        return buffer->isFull();
    }

    template<class BUFFER>
    bool NonBlockingChannel<BUFFER>::available()
    {
        VMPI_memoryBarrier();
        return !buffer->isEmpty();
    }

    template<class BUFFER>
    bool NonBlockingChannel<BUFFER>::put(const Atom &in)
    {
        while (true) {
            if (buffer->put(in)) return true;
            if (eof) return false;
            VMPI_threadSleep(0);
        }
        NOT_REACHED;            
    }
        
    template<class BUFFER>
    bool NonBlockingChannel<BUFFER>::get(Atom *outp)
    {
        while (true) {
            if (buffer->get(outp)) return true;
            if (eof) return false;
            VMPI_threadSleep(0);
        }
        NOT_REACHED;            
    }
    
    template<class BUFFER>
    void NonBlockingChannel<BUFFER>::registerWithGC(MMgc::GC* gc)
    {
        buffer->registerWithGC(gc);
    }
    
    template<class BUFFER>
    MMgc::GC* NonBlockingChannel<BUFFER>::gc()
    {
        return buffer->gc();
    }

}
#endif
