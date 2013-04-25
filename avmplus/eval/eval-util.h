/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This file is included into eval.h
namespace avmplus {
namespace RTC {

// Utility functions

/**
 * Simple string.  Always allocated inside an Allocator heap.  Always NUL-terminated
 * for debugging convenience.
 */

class Str {
public:
    uint32_t    length;     // number of chars excluding NUL
    uint32_t    hash;       // hash code
    uint32_t    ident;      // ~0 or the index in the string pool
    Str*        next;       // next in hashtable bucket
    wchar       s[1];       // actually longer
    
    // Returns < 0 if this < other, 0 if this == other, > 0 if this > other.
    // Don't use this for comparing for equality, just use == instead: a Str
    // is always interned.
    int compareTo(Str* other);
};

uint32_t hashString(const wchar* chars, uint32_t nchars);
uint32_t lenU30(uint32_t val);
uint32_t utf8length(Str* str);
uint8_t* emitU16(uint8_t* out, uint16_t val);
uint8_t* emitU32(uint8_t* out, uint32_t val);
uint8_t* emitU30(uint8_t* out, uint32_t val);
uint8_t* emitS32(uint8_t* out, int32_t val);
uint8_t* emitS24(uint8_t* out, int32_t val);
uint8_t* emitDouble(uint8_t* out, double d);
uint8_t* emitFloat(uint8_t* out, float f);
uint8_t* emitFloat4(uint8_t* out, const float4_t& f4);
uint8_t* emitUtf8(uint8_t* out, Str* s);

int32_t readS24(uint8_t* in);

void formatUtf8(char* buf, size_t bufsiz, const wchar* s);

#ifdef DEBUG
// Get up to limit-1 characters from s into buf, chop high bits, NUL-terminate, return buf
char* getn(char* buf, const Str* s, size_t limit);
#endif

inline uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }
inline uint32_t max(uint32_t a, uint32_t b) { return a > b ? a : b; }

inline int32_t min(int32_t a, int32_t b) { return a < b ? a : b; }
inline int32_t max(int32_t a, int32_t b) { return a > b ? a : b; }

/**
 * Convenience wrapper for accumulating string values for
 * identifiers, strings, regular expressions, xml text.
 */

class SBChunk {
public:
    enum {
        chunksize = 100
    };
    
    wchar data[chunksize];
    SBChunk* next;
};

class StringBuilder {
public:
    StringBuilder(Compiler* compiler);
    ~StringBuilder();
    
    void clear();
    void append(Str* other);
    void append(const char* other);
    void append(StringBuilder* other);
    void append(const wchar* ptr, const wchar* lim);
    void append(int c);
    uint32_t length();
    Str* str();
    char *chardata();           // NUL-terminated array, freshly allocated on every call.  Data are chopped to 7 bits.
    
private:
    void append(SBChunk* other);
    wchar* copyInto(wchar* buf, SBChunk* c);
    char* copyInto(char* buf, SBChunk* c);
    void pushChunk();
    void popChunk();
    
    Allocator* const allocator;
    SBChunk* chunk;             // current chunk
    uint32_t nextchar;          // next free char in chunk
    uint32_t len;               // total length
};


/**
 * Bump-a-pointer allocator.
 *
 * Used for all allocation by the run-time compiler except the allocation for the
 * final ABC bytevector.
 *
 * FIXME: The allocator is a little prone to internal fragmentation on the
 * block level.  This is because fairly large blocks at the end of a segment
 * may be lost if a large request comes in and causes a new segment to be allocated.
 * We can fix that with a simple free list of large blocks, if it turns out
 * to be a problem in practice, or we can make the growth increment relatively
 * larger.
 */
class Allocator {
public:
    Allocator(Compiler* compiler);
    ~Allocator();
    
    void* alloc(size_t nbytes);
    
    Compiler* const compiler;
    SBChunk* free_sbchunks; // shared among all StringBuilders working off this allocator
    
private:
    void* allocSlow(size_t nbytes);
    void refill(size_t nbytes);
    
    class Chunk {
    public:
        Chunk* prev;
#ifndef AVMPLUS_64BIT
        uintptr_t padding;
#endif
        char data[1];
    };
    
    Chunk* current_chunk;
    char* current_top;
    char* current_limit;
};


/**
 * Efficient accumulator for sequence of bytes.  check() checks for available
 * space and returns a valid output pointer for that many bytes; the number
 * of bytes requested may be larger than 'increment' but this should be the
 * exception, not the rule.  'increment' is the allocation quantum from the
 * underlying allocator; it should be a smallish fraction of the underlying
 * allocator's quantum.
 *
 * size() returns the number of bytes currently in the buffer.  serialize()
 * copies those into a sequential array starting at b.
 */
class ByteBuffer {
public:
    ByteBuffer(Allocator* allocator, uint32_t increment=100);

    uint32_t size() const;
    void serialize(uint8_t* b) const;

    void emitU8(uint8_t val);
    void emitS8(int8_t val);
    void emitU16(uint16_t val);
    void emitU32(uint32_t val);
    void emitU30(uint32_t val);
    void emitS32(int32_t val);
    void emitS24(int32_t val);
    void emitDouble(double d);
    void emitFloat(float f);
    void emitFloat4(const float4_t& f);
    void emitUtf8(uint32_t nbytes, Str* s);
    
private:
    class Chunk {
    public:
        uint8_t* end;
        Chunk* next;
        uint8_t start[1];       // actually longer
    };
    
    Allocator * const allocator;
    const uint32_t increment;
    uint8_t* out;
    uint8_t* limit;
    Chunk* first;
    Chunk* last;
    uint32_t size_rest;     // collected size of the chunks not including last
    
    void makeRoom(uint32_t nbytes);
    void makeRoomSlow(uint32_t nbytes);
};

// This is useful if an allocator is in scope with the name "allocator"
#define ALLOC(type, args)           \
    ::new (allocator->alloc(sizeof(type))) type args

template<class T> class Seq {
public:
    Seq(T hd, Seq<T>* tl=NULL) : hd(hd), tl(tl) {}
    T       hd;
    Seq<T>* tl;
};

template<class T>
uint32_t length(Seq<T*>* elts) {
    uint32_t len = 0;
    while (elts != NULL) {
        elts = elts->tl;
        len++;
    }
    return len;
}

template<class T> class SeqBuilder {
public:
    SeqBuilder(Allocator* allocator) : allocator(allocator), items(NULL), last(NULL) {}
    
    void addAtEnd(T item);          // enqueue
    T dequeue();                    // pop and return the first element; queue must not be empty
    bool isEmpty();                 // true iff queue is empty
    Seq<T>* get() const;            // return the elements in the queue, do not clear the queue
    void clear();                   // reset the queue

private:
    Allocator* allocator;
    Seq<T>* items;
    Seq<T>* last;
};

}}
