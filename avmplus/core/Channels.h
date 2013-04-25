/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Channels__
#define __avmplus_Channels__

namespace avmplus
{
#define NOT_REACHED AvmAssert(false); return NULL;


    class ChannelItem 
    {
    public:
        virtual ~ChannelItem() {};
        virtual Atom getAtom(Toplevel* toplevel) const = 0;
    };

    /*
     * A "BlockingChannel" implements a blocking (each operation is
     * synchronized using locks) read-write stream over a "Buffer"
     * that does not implement any synchronization of its own.
     *
     * A Buffer must implement:
     *   bool isEmpty()          -- whether the buffer is empty
     *   bool isFull()           -- whether the buffer is full
     *   bool put(item)          -- puts the item, returns whether it succeeded
     *   bool get(itemPtr)       -- gets an item, returns whether it succeeded
     */
    template<class BUFFER>
    class BlockingChannel : public FixedHeapRCObject
    {        
    public:
        
        BlockingChannel();       
        ~BlockingChannel();
        
        void close();
        bool isClosed();
        bool isEmpty();
        bool isFull();
        bool available();
        uint32_t numItemsInBuffer();
        bool put(const ChannelItem* in);
        bool get(const ChannelItem** outp);

    private:
        //  Lock these up
        BlockingChannel(const BlockingChannel &copyFrom);
        BlockingChannel & operator=(const BlockingChannel &assignFrom);
        
        bool eof;
        BUFFER* buffer;                
        vmbase::WaitNotifyMonitor m_monitor;
    };
    
    /*
     * A "NonBlockingChannel" implements a non blocking read-write
     * stream over a "Buffer" that by itself supports non-blocking
     * atomic updates.
     */
    template<class BUFFER>
    class NonBlockingChannel : public FixedHeapRCObject
    {
    public:
        
        NonBlockingChannel(MMgc::GC* gc);
	~NonBlockingChannel();

        void close();        
        void unregisterRoot();
        bool isClosed();
        bool isEmpty();
        bool isFull();
        bool available();
        bool put(const Atom &in);
        bool get(Atom *outp);
        void registerWithGC(MMgc::GC* gc);
        MMgc::GC* gc();

    private:
        volatile bool eof;
        BUFFER* buffer;
    };


    /*
     * A "MostlyNonBlockingChannel" implements a read-write stream
     * that utilizes blocking synchronization operations only on the
     * boundary conditions (full/empty) and uses non-blocking
     * synchronization otherwise. It is based on the non-blocking
     * circular buffer implementation by Lamport.
     *
     * The main challenge, having one channel user suspended (via
     * wait) because the channel is full/empty, is to avoid the other
     * channel user to finish updating the channel state (to
     * non-full/non-empty) without realizing that notification is
     * required.
     *
     * The algorithm prevents that from happening by, right before
     * invoking wait, atomically changing the state of the channel so
     * that the potential notifier is prevented from completing its
     * current operation. If the potential notifier is a sender than
     * the state is set to (fake) full, otherwise it is set to (fake)
     * empty.
     *
     * The actual state of the channel is stored in the variable
     * called fallback, which is used both to detect that notification
     * (i.e. "fallback" on standard locks) is required and revert
     * channel to the actual (legal) state.
     *
     * In case of a constant stream of small data, once the channel
     * "falls back" on using locks it has a tendency to keep using the
     * locks. The notifier does not seem to have enough time to
     * perform another operation before the other channel user issues
     * the second (after the one for which it has been notified)
     * request and "falls back" on using locks again. The algorithm
     * tries to prevent that by enforcing th the next two (though it's
     * configurable) requests after being notified to proceed in a
     * fully non-blocking fashion (never "falling back" on locks).
     */
    class MostlyNonBlockingChannel : public FixedHeapRCObject
    {        

    public:
        
        MostlyNonBlockingChannel(MMgc::GC* gc);
        ~MostlyNonBlockingChannel();

        void close();
        void unregisterRoot();
        bool isClosed();
        bool isEmpty();
        bool isFull();
        bool put(const Atom &in);
        bool get(Atom *outp);
        /*
         * Must not be used concurrently with get(),including inside
         * of the implementation of get().
         */
        bool available();
        void registerWithGC(MMgc::GC* gc);
        MMgc::GC* gc();

    private:
        volatile bool eof;
        vmbase::WaitNotifyMonitor m_monitor;
        static const int INIT_LENGTH = 16;
        // how many non-blocking attempts
        static const int TRIES = 3;
        // how many fully non-blocking attempts after wait()
        // (to try and reset non-blocking flow)
        static const int NO_FB = -1;
        volatile int get_tries;
        volatile int put_tries;

        class BufferData : public MMgc::GCRoot
        {
            friend class MostlyNonBlockingChannel;
        public: 
            Atom* m_items;
            int32_t length;
            
            BufferData(MMgc::GC* gc, MostlyNonBlockingChannel* ch, int32_t size);
            ~BufferData();
                
            virtual bool gcTrace(MMgc::GC* gc, size_t cursor);
            void cleanup();

        private:
            MostlyNonBlockingChannel* channel;
        };

        BufferData* bufferData;
        volatile int32_t head;
        volatile int32_t tail;
        volatile int32_t fallback;

        BufferData* overflowBufferData;
        volatile int32_t overflowHead; // tail is always 0

        void createOverflowBuffer();
        void resizeOverflowBuffer();

    };

/*
 * Buffers must be GC-enabled so that their content can be traced by
 * the GC - they have to inherit from the GCTraceableObject class.
 */

/*
 * Trivial single-item buffer.
 */
    class NoSyncSingleItemBuffer
    {
    public:
        NoSyncSingleItemBuffer(MMgc::GC* gc);
        ~NoSyncSingleItemBuffer();
        
        bool isEmpty();
        bool isFull();
        bool put(const Atom &in);
        bool get(Atom *outp);
        void registerWithGC(MMgc::GC* gc);
        MMgc::GC* gc();
        void unregisterRoot();

    protected:
        bool m_hasData;
    private:

        class BufferData : public MMgc::GCRoot
        {
        public: 
            Atom m_item;

            BufferData(MMgc::GC* gc, NoSyncSingleItemBuffer* buf);

	    virtual bool gcTrace(MMgc::GC* gc, size_t cursor);

        private:
            NoSyncSingleItemBuffer* buffer;
        };
            
        BufferData* bufferData;
    };

/*
 * Trivial multi-item buffer.
 */
    class NoSyncMultiItemBuffer
    {
    public:

        NoSyncMultiItemBuffer();
        ~NoSyncMultiItemBuffer();

        bool isEmpty();
        bool isFull();
        bool put(const ChannelItem* in);
        bool get(const ChannelItem** outp);
        uint32_t numItemsInBuffer();

    protected:
        bool m_hasData;
    private:
        static const int BUF_LENGTH = 16;

		const ChannelItem** m_items;
		
        int head;
        int tail;
		uint32_t bufLength;

    };


/*
 * Non-blocking multi-item buffer (Lamport's implmentation).
 */
    class NonBlockingMultiItemBuffer
    {
    public:
        NonBlockingMultiItemBuffer(MMgc::GC* gc);
        ~NonBlockingMultiItemBuffer();
        bool isEmpty();
        bool isFull();
        bool put(const Atom &in);
        bool get(Atom *outp);
        void registerWithGC(MMgc::GC* gc);
        MMgc::GC* gc();
        void unregisterRoot();

    private:
        static const int BUF_LENGTH = 16;

        class BufferData : public MMgc::GCRoot
        {
            friend class NonBlockingMultiItemBuffer;
        public: 
            Atom m_items[BUF_LENGTH];
            
            BufferData(MMgc::GC* gc, NonBlockingMultiItemBuffer* buf);
                
            virtual bool gcTrace(MMgc::GC* gc, size_t cursor);

        private:
            NonBlockingMultiItemBuffer* buffer;
        };

        BufferData* bufferData;
        volatile int32_t head;
        volatile int32_t tail;
    };

    /*
     * Externally visible superclass of the PromiseChannel. Its choice
     * determines how the channel is implemented:
     * blocking/non-blocking, single-item/multi-item etc.
     */
    typedef MostlyNonBlockingChannel PromiseChannelImpl;
    //    typedef NonBlockingChannel<NonBlockingMultiItemBuffer> PromiseChannelImpl;
    //    typedef BlockingChannel<NoSyncMultiItemBuffer> PromiseChannelImpl;
    //    typedef BlockingChannel<NoSyncSingleItemBuffer> PromiseChannelImpl;

    /*
     * Every message channel must implement:
     *   void registerWithGC(gc) -- register channel content to be traced by the GC
     *   void gc()               -- returns GC the channel content is registered with
     */
    class PromiseChannel : public PromiseChannelImpl
    {
        // The buffer can contain a pointer that the GC of the sender should know about.

        friend class Aggregate;
        
        PromiseChannel(int64_t guid, int32_t sender, int32_t receiver, MMgc::GC* gc);

    public:
        virtual void destroy();
        virtual ~PromiseChannel();
        bool isEndpoint(const Isolate* isolate) const;
        bool close(int32_t endpoint_giid);

        const int64_t channelGuid;
        int32_t sender;
        int32_t receiver;
    private:
        int32_t m_commInProgress;        
    };
    

}
#endif
