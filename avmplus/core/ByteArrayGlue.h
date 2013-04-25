/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef BYTEARRAYGLUE_INCLUDED
#define BYTEARRAYGLUE_INCLUDED

namespace avmplus
{
    class ByteArray : public DataInput,
                      public DataOutput
    {
        
    public:

        friend class ByteArrayTask;
        friend class ByteArrayObject;
        friend class ByteArraySetLengthTask;
        friend class ByteArraySwapBufferTask;
        friend class ByteArrayCompareAndSwapLengthTask;
        class Buffer : public FixedHeapRCObject
        {
        public:
            virtual void destroy();
            virtual ~Buffer();
            uint8_t* array;
            uint32_t capacity;
            uint32_t length;
        };


        ByteArray(Toplevel* toplevel);
        ByteArray(Toplevel* toplevel, const ByteArray& other);
        ByteArray(Toplevel* toplevel, ByteArray::Buffer* source, bool shared);
        
        ~ByteArray();

        void Clear();
        
        REALLY_INLINE uint8_t operator[](uint32_t index) const { AvmAssert(index < m_buffer->length); return (index < m_buffer->length) ? m_buffer->array[index] : 0; }
        uint8_t& operator[](uint32_t index);

        REALLY_INLINE uint32_t GetLength() const { return m_buffer->length; }
        REALLY_INLINE bool IsShared() const { return m_isShareable && (m_buffer->RefCount() > 1); }

        // Ensure that the capacity of the ByteArray is at least 'newLength',
        // and set length = max(GetLength(), newLength),
        // and set position = min(GetPosition(), newLength)
        //
        // Note that SetLength(0) empties the (logical) contents of the ByteArray,
        // but doesn't reduce the underlying capacity allocated.
        // Use Clear() to eliminate existing memory allocations.
        void FASTCALL SetLength(uint32_t newLength);

    private:
        // Ensure that the capacity of the ByteArray is at least 'newLength',
        // and set length = max(GetLength(), newLength),
        // and set position = min(GetPosition(), newLength)
        //
        // This is alternative entry point that *only* the length setter
        // uses; it serves as a hint from client that newLength is
        // expected maximum length for immediate future.
        void FASTCALL SetLengthFromAS3(uint32_t newLength);
        void SetLengthCommon(uint32_t newLength, bool calledFromSetter);
        void UnprotectedSetLengthCommon(uint32_t newLength, bool calledFromSetter);
        int32_t UnprotectedAtomicCompareAndSwapLength(int32_t expectedLength, int32_t newLength);

    public:
        // Set the length to x+y, with overflow check.  If x+y overflows a uint32_t then
        // throw a MemoryError (same error that the one-argument variety will throw if
        // trying to create a buffer larger than the buffer limit, which is less than 2^32-1).
        void FASTCALL SetLength(uint32_t x, uint32_t y);
        
        // You can use this call to get a READ_ONLY pointer into the ByteArray.
        // The pointer starts at offset zero (regardless of the value of GetPosition())
        // and the data is guaranteed to be valid for GetLength() bytes.
        //
        // *** USE THIS METHOD WITH EXTREME CAUTION, AND ONLY WHEN ABSOLUTELY NECESSARY ***
        //
        // In particular, you must not use this method and const_cast the result, as
        // you may overwrite ByteArrays that are copy-on-write; please use GetWritableBuffer
        // instead if the data must be written to.
        //
        // You normally should use Read() rather than this method,
        // which is provided solely for compatibility with existing code paths;
        // it's highly recommend you not use this method for new code.
        //
        REALLY_INLINE const uint8_t* GetReadableBuffer() const { return m_buffer->array; }

        // You can use this call to get a WRITABLE pointer into the ByteArray.
        // The pointer starts at offset zero (regardless of the value of GetPosition())
        // and the data is guaranteed to be valid for GetLength() bytes.
        //
        // *** USE THIS METHOD WITH EXTREME CAUTION, AND ONLY WHEN ABSOLUTELY NECESSARY ***
        //
        // You normally should use Write() rather than this method,
        // which is provided solely for compatibility with existing code paths;
        // it's highly recommend you not use this method for new code.
        //
        uint8_t* FASTCALL GetWritableBuffer();
        
        // Make this ByteArray a copy-on-write ByteArray. All existing content will be discarded
        // and the new data referenced instead. "owner", if non-null, is a GCObject that
        // owns the data (a reference will be kept to it to ensure the data isn't collected);
        // if owner is null, it's assumed that the data will never become invalid.
        void SetCopyOnWriteData(MMgc::GCObject* owner, const uint8_t* data, uint32_t length);

        REALLY_INLINE uint32_t GetPosition() { return m_position; }
        REALLY_INLINE void SetPosition(uint32_t pos) { m_position = pos; }
        
        // Ensure that the ByteArray has a capacity of (at least) capacity.
        // This does not affect GetLength() or GetPosition().
        // If an attempted expansion fails, throws AS3 exception.
        void EnsureCapacity(uint32_t capacity);
        void EnsureCapacityNoInline(uint32_t capacity); // (same, but NO_INLINE)

        // Ensure that the ByteArray has a capacity of (at least) capacity
        // and return true, or set exn_recv to the thrown exception and
        // return false.
        bool EnsureCapacityOrFail(uint32_t capacity,
                                  enum CatchAction, Exception **exn_recv);

        // overrides from DataInput
        /*virtual*/ uint32_t Available() { return (m_position <= m_buffer->length) ? (m_buffer->length - m_position) : 0; }
        /*virtual*/ void Read(void* buffer, uint32_t count);

        // overrides from DataOutput
        /*virtual*/ void Write(const void* buffer, uint32_t count);

        bool isShareable () const;
        bool setShareable (bool value);
        
             
        bool addSubscriber(DomainEnv* subscriber);
        bool removeSubscriber(DomainEnv* subscriber);

        // compression / decompression
        enum CompressionAlgorithm
        {
            k_zlib,
            k_deflate,
            k_lzma            
        };

        void Compress(CompressionAlgorithm algorithm);
        void Uncompress(CompressionAlgorithm algorithm);

        // For use by ByteArrayObject for optimized read functions: check that there are
        // nbytes available to read at the current position, and throw an EOF exception
        // if not.  Update the position to point beyond that area.  Return a pointer to
        // the start of the area.
        //
        // Precondition: 0 < nbytes < 4096
        uint8_t* requestBytesForShortRead(uint32_t nbytes);
        
        // For use by ByteArrayObject for optimized write functions: check that there are
        // nbytes available to write at the current position, and extend the array if not.
        // Throw MemoryError if the array cannot be extended.  Update the position to point
        // beyond the write area.  Return a pointer to the start of the area.
        //
        // Precondition: 0 < nbytes < 4096
        uint8_t* requestBytesForShortWrite(uint32_t nbytes);

        int32_t CAS(uint32_t index, int32_t expected, int32_t next);
        bool share();
        
#ifdef DEBUGGER
    public:
        // Called by the profiler to determine the number of bytes attributed
        // to this object, not including sizeof(*this).
        virtual uint64_t bytesUsed() const;
#endif

        /*protected: */
        virtual Toplevel* toplevel() const { return m_toplevel; }

        static void UpdateSubscribers();

    private:

        void CompressViaZlibVariant(CompressionAlgorithm algorithm);
        void UncompressViaZlibVariant(CompressionAlgorithm algorithm);

        void CompressViaLzma();
        void UncompressViaLzma();

    public: // Tasks need it
        class Grower
        {
            friend class ByteArray;
        public:
            Grower(ByteArray* owner, uint32_t minimumCapacity)
                : m_owner(owner)
                , m_oldArray(owner->m_buffer->array)
                , m_oldLength(owner->m_buffer->length)
                , m_oldCapacity(owner->m_buffer->capacity)
                , m_minimumCapacity(minimumCapacity)
            {
            }
            void FASTCALL ReallocBackingStore(uint32_t newCapacity);
            void FASTCALL EnsureWritableCapacity();
            REALLY_INLINE bool RequestWillReallocBackingStore() const;
            REALLY_INLINE bool RequestExceedsMemoryAvailable() const;
            void SetLengthCommon(uint32_t newLength, bool calledFromLengthSetter);

            void run(); // from SafepointTask
            virtual ~Grower();
        private:
            ByteArray*  m_owner;
            uint8_t*    m_oldArray;
            uint32_t    m_oldLength;
            uint32_t    m_oldCapacity;
            uint32_t    m_minimumCapacity;
        };
    public:
        
        Buffer* getUnderlyingBuffer() { return m_buffer; }
        

    private:
        enum { kGrowthIncr = 4096 };

        typedef WeakRefList<DomainEnv> WeakSubscriberList;

    private:
        void _Clear();
        
    public:// FIXME made public for Aggregate::reloadGlobalMemories()
        void NotifySubscribers();
    private: // end FIXME

        void TellGcNewBufferMemory(const uint8_t* buf, uint32_t numberOfBytes);
        void TellGcDeleteBufferMemory(const uint8_t* buf, uint32_t numberOfBytes);
        
        REALLY_INLINE bool IsCopyOnWrite() const { return m_copyOnWriteOwner != NULL; }
        void SetCopyOnWriteOwner(MMgc::GCObject* owner);

        ByteArray(const ByteArray& lhs);        // unimplemented
        ByteArray& operator=(const ByteArray&); // unimplemented

    public:
        void gcTrace(MMgc::GC* gc)
        {
            // This tracer assumes the following (true as of 2010-12-08):
            //  - m_array is allocated with FixedMalloc
            gc->TraceLocation(&m_toplevel);
            gc->TraceLocation(&m_copyOnWriteOwner);
            m_subscribers.gcTrace(gc);
            DataInput::gcTrace(gc);
            DataOutput::gcTrace(gc);
        }
        
#ifdef DEBUG
        // Normally manually traced classes would subclass
        // GCInlineObject to pick this up but ByteArray is an anomaly.
        MMgc::GCTracerCheckResult gcTraceOffsetIsTraced(uint32_t) const
        {
            return MMgc::kPassDueToManualTracer;
        }
#endif

    private:
        Toplevel* const         m_toplevel;
        MMgc::GC* const         m_gc;
        WeakSubscriberList      m_subscribers;
        //
        // If this is NULL, we are not a copy-on-write ByteArray.
        //
        // If this is non-NULL, we are copy-on-write, with this pointer being used
        // to ensure that our data isn't collected out from under us. (Note that this
        // currently requires the object to be GCObject or GCFinalizedObject, not RCObject;
        // that's fine for all extant use cases.) (Note that the pointer is written with explicit WB calls.)
        //
        // Note that (under the covers) we use GC::emptyWeakRef as a sentinel to mean
        // "I am copy on write but there is no GCObject controlling my lifespan"; this
        // can be the case if (e.g.) the data is compile-time-constant data we are just
        // wrapping. Using GC::emptyWeakRef here is, admittedly, an ugly hack, but doing so
        // avoids using the lower-three-bits-as-flags approach we're trying to eradicate elsewhere,
        // and avoids adding a "bool" field which would expand this struct by an average of 7 bytes
        // due to MMgc alignment rules.
        // FIXME due to factoring out of the Buffer object alignment may be affected.
        //
        MMgc::GCObject*         m_copyOnWriteOwner;
        uint32_t                m_position;
        FixedHeapRef<Buffer>    m_buffer;
        bool                    m_isShareable;
    public: // FIXME permissions
        bool                    m_isLinkWrapper;
    };

    //
    // ByteArrayClass
    //

    class GC_AS3_EXACT(ByteArrayClass, ClassClosure)
    {
    protected:
        ByteArrayClass(VTable *vtable);

    public:
        GCRef<ByteArrayObject> constructByteArray();

        uint32_t get_defaultObjectEncoding() const { return get__defaultObjectEncoding(); }
        void set_defaultObjectEncoding(uint32_t version) { set__defaultObjectEncoding(version); }
        
    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(ByteArrayClass)

        DECLARE_SLOTS_ByteArrayClass;
    // ------------------------ DATA SECTION END
    };

    class GC_AS3_EXACT(ByteArrayObject, ScriptObject)
    {
        friend class ByteArrayClass;
        friend class MessageChannelObject; // FIXME Still needed?
    protected:
        ByteArrayObject(VTable* ivtable, ScriptObject* delegate);
        ByteArrayObject(VTable* ivtable, ScriptObject* delegate, const ByteArray& source);
    public:
        ByteArrayObject(VTable* ivtable, ScriptObject* delegate, ByteArray::Buffer* source);

    public:

        virtual bool hasAtomProperty(Atom name) const;
        virtual void setAtomProperty(Atom name, Atom value);
        virtual Atom getAtomProperty(Atom name) const;
        virtual bool hasUintProperty(uint32_t i) const;
        virtual Atom getUintProperty(uint32_t i) const;
        virtual void setUintProperty(uint32_t i, Atom value);
        Atom getMultinameProperty(const Multiname* name) const;
        void setMultinameProperty(const Multiname* name, Atom value);
        bool hasMultinameProperty(const Multiname* name) const;
        Atom readObject();
        void writeObject(Atom value);

        void readBytes(ByteArrayObject *bytes, uint32_t offset, uint32_t length);
        void writeBytes(ByteArrayObject *bytes, uint32_t offset, uint32_t length);

        String* _toString();
        
        void _compress(String* algorithm);
        void _uncompress(String* algorithm);

        void writeBoolean(bool value);
        void writeByte(int value);
        void writeShort(int value);
        void writeInt(int value);
        void writeUnsignedInt(uint32_t value);
        void writeFloat(double value);
#ifdef VMCFG_FLOAT
        void writeFloat4(const float4_t& value);
#endif
        void writeDouble(double value);
        void writeMultiByte(String* value, String* charSet);
        void writeUTF(String* value);
        void writeUTFBytes(String* value);
    
        bool readBoolean();
        int readByte();
        int readUnsignedByte();
        int readShort();
        int readUnsignedShort();
        int readInt();
        uint32_t readUnsignedInt();
        double readFloat();
#ifdef VMCFG_FLOAT
        void readFloat4(float4_t*);
#endif
        double readDouble();
        String* readMultiByte(uint32_t length, String* charSet);
        String* readUTF();
        String* readUTFBytes(uint32_t length);

        uint32_t get_bytesAvailable() { return m_byteArray.Available(); }
        
        uint32_t get_position() { return m_byteArray.GetPosition(); }
        void set_position(uint32_t offset) { m_byteArray.SetPosition(offset); }

        uint32_t get_length() { return m_byteArray.GetLength(); }
        void set_length(uint32_t value) { m_byteArray.SetLengthFromAS3(value); }

        ByteArray& GetByteArray() { return m_byteArray; }

        uint32_t get_objectEncoding();
        void set_objectEncoding(uint32_t version);

        Stringp get_endian();
        void set_endian(Stringp value);

        void clear();
        int32_t atomicCompareAndSwapIntAt(int32_t byteIndex , int32_t expectedValue, int32_t newValue );
        int32_t atomicCompareAndSwapLength(int32_t expectedLength, int32_t newLength);
        
        bool get_shareable();
        void set_shareable(bool val);

	public:
		ChannelItem* makeChannelItem();

#ifdef DEBUGGER
    public:
        virtual uint64_t bytesUsed() const;
#endif

    private:
        // REALLY_INLINE helpers for fast reading and writing, without code duplication.
        uint32_t read32();
        uint32_t read16();
        float readFloatHelper(uint8_t* b);                  // ...Helper because AOT hates overloaded names
        void writeFloatHelper(float value, uint8_t* b);     // ditto
        void write32(uint32_t value);

        ByteArray::CompressionAlgorithm algorithmToEnum(String* algorithm);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_DATA_BEGIN(ByteArrayObject)

        MMgc::Cleaner   c;
        ByteArray       GC_STRUCTURE(m_byteArray);

        GC_DATA_END(ByteArrayObject)

        DECLARE_SLOTS_ByteArrayObject;
    // ------------------------ DATA SECTION END
    };

}

#endif /* BYTEARRAYGLUE_INCLUDED */
