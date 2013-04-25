/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_NewString__
#define __avmplus_NewString__

namespace avmplus
{
    class ByteArray;

    /// The utf8_t data type expressively means UTF-8 data.
    typedef uint8_t utf8_t;

    /**
    A String can have many faces, dependent on the way the string data is stored.
    The most common is the kDynamic type, meaning that the string data is owned by
    the string instance. The kStatic type has the string instance point to a static buffer.
    This buffer must exist as long as the string exists; character constants are
    ideal candidates for this type. The kDependent type is a string that points
    into another string; this type is created in a substring or concatenation operation.
    Strings cannot be deleted directly, because they may be referenced be dependent strings.
    <p>
    String concatenation attempts first to use additional memory that the memory
    allocator has left behind when a kDynamic string was allocated. If the right-hand
    string fits into that buffer, the data is appended, and a dependent string is returned,
    pointing to the new, larger buffer (the original string keeps its initial length
    and thus does not know about the additional data). If the data does not fit, a new
    kDynamic string is allocated with some extra bytes at the end, assuming that there may
    be more characters to append. The minimum of extra characters is 32, then it is twice
    the new length, up to a platform-dependent maximum(usually 64K). This value can be
    tweaked for platforms with memory constraints in favor of more copying operations.
    <p>
    Strings exist in 8, 16, and 32-bit flavors. The 8-bit flavor only holds the first
    256 Unicode characters. All widths ignore Unicode surrogate pairs, treating them
    as ordinary characters. Use the createUTFxx() methods to deal with surrogate pairs and
    UTF-8 encoding. If the kAuto type is used during creation, a quick scan is made to
    see if a string would fit into a narrower width than the buffer suggests, i.e. if
    a 16-bit buffer only contains 8-bit characters.
    <p>
    Strings cannot be deleted, since the create() methods may choose to return standard
    string constants, or interned strings, or other strings that other code depends on.
    */
    class String : public AvmPlusScriptableObject
    {
        friend class AvmCore;   // for internSubstring

    public:

        /// String type constants. Note that isDependent() and isStatic() rely on using these values as bitflags.
        enum Type
        {
            kDynamic            = 0,    // buffer is on the heap
            kStatic             = 1,    // buffer is static
            kDependent          = 2     // string points into master string
        };
        /// String width constants.
        enum Width
        {
            kAuto   = -1,   // only used in APIs
            k8      = 0,    // chosen such that i<<k8 == i*sizeof(uint8_t)
            k16     = 1     // chosen such that i<<k16 == i*sizeof(uint16_t)
        };
        /**
        Use this constant to define the default width for this system. If you use anything
        else but kAuto, this would create strings of that width. This is not recommended.
        */
        static  const Width kDefaultWidth = kAuto;

        /**
        Create a string using Latin-1 data. Characters are just widened and copied.
        To create an UTF-8 string, use createUTF8().
        @param  core                the AvmCore instance to use
        @param  buffer              the character buffer; if NULL, assume an empty string.
        @param  len                 the size in characters. If < 0, assume NULL termination and calculate.
        @param  desiredWidth        the desired width; use kAuto to get a string as narrow as possible
        @param  staticBuf           if true, the buffer is static, and may be used by the string
        @return                     the String instance
        */
        static  Stringp             createLatin1(AvmCore* core, const char* buffer, int32_t len = -1, Width desiredWidth = kDefaultWidth, bool staticBuf = false);

        /**
        Create a string using UTF-8 data. To preserve backwards compatibility, an additional
        "strict" flag can be set to false. This allows a bug in the UTF-8 conversion routine
        to prevail, where invalid UTF-8 sequences are copied as single characters.
        @param  avm                 the AvmCore instance to use
        @param  buffer              the UTF-8 buffer; if NULL, assume an empty string.
        @param  len                 the size in bytes. If < 0, assume NULL termination and calculate.
        @param  desiredWidth        the desired width; use kAuto to get a string as narrow as possible
        @param  staticBuf           if true, the buffer is static, and may be used by the string
        @param  strict              if false, return NULL on invalid characters (see above)
        @return                     the String instance, or NULL on bad characters
        */
        static  Stringp             createUTF8(AvmCore* core, const utf8_t* buffer, int32_t len = -1,
                                                String::Width desiredWidth = String::kDefaultWidth,
                                                bool staticBuf = false, bool strict = true);
        /**
        Create a string using UTF-16 data. If the desired width is too small to fit the source data,
        or if the data contains an invalid surrogate pair if "strict" is true, return NULL. If
        "strict" is false, invalid surrogate pairs are treated as two separate characters.
        @param  avm                 the AvmCore instance to use
        @param  buffer              the UTF-16 buffer; if NULL, assume an empty string.
        @param  len                 the size in characters. If < 0, assume NULL termination and calculate.
        @param  desiredWidth        the desired width; use kAuto to get a string as narrow as possible
        @param  staticBuf           if true, the buffer is static, and may be used by the string
        @param  strict              if false, return NULL on invalid surrogate pairs
        @return                     the String instance, or NULL on errors
        */
        static  Stringp             createUTF16(AvmCore* core, const wchar* buffer, int32_t len = -1,
                                                 String::Width desiredWidth = String::kDefaultWidth,
                                                 bool staticBuf = false, bool strict = true);

        virtual                     ~String();

        virtual bool                gcTrace(MMgc::GC* gc, size_t cursor);

        /**
        Create a string with a given width out of this string. If the width is equal to the current
        width, return this instance. If the desired width is too narrow to fit, or kAuto is passed
        in, return NULL.
        @param  w                   the width of the new string(kAuto is not supported)
        @return                     the String instance, or NULL on kAuto, or string too wide
        */
                Stringp FASTCALL    getFixedWidthString(Width w) const;
        /**
        Returns the Atom equivalent of this String.  This is
        done by or'ing the proper type bits into the pointer.
        */
        REALLY_INLINE   Atom        atom() const { return Atom(AtomConstants::kStringType | uintptr_t(this)); }
        /**
        virtual version of atom():
        */
        virtual Atom                toAtom() const { return atom(); }
        /**
        If this string is a static or dependent string, make it dynamic so the static
        data can be released. The string is only
        made dynamic if the static string pointer falls within the given data buffer.
        This prevents unnecessary dynamization of static strings if the string data
        belongs to a different data buffer.
        */
                void                makeDynamic(const uint8_t* dataStart, uint32_t dataSize);
        /**
        Check the master of this string if this is a dependent string. If there is
        any indication that using this string would hold a lock on a big master
        string, create a dynamic string and release the master string. The AvmCore
        uses this method to ensure that interned strings do not keep a lock on a huge
        master string if the master string is dynamic, trying to limit the waste of
        memory.
        */
                void                fixDependentString();
        /**
        Produce a has code of this string.
        */
                int32_t             hashCode() const;
        /**
        Use the same algorithm to produce a hash code for Latin1 data.
        */
        static  int32_t FASTCALL    hashCodeLatin1(const char* buf, int32_t len);
        /**
        Use the same algorithm to produce a hash code for UTF-16 data.
        */
        static  int32_t FASTCALL    hashCodeUTF16(const wchar* buf, int32_t len);

        /**
        Use the same algorithm to produce hash code for string corresp to value.
        */
        static  int32_t FASTCALL    hashCodeUInt(uint32_t value);

        /// Return the length in characters.
        REALLY_INLINE   int32_t     length() const { return m_length; }
        // overload used by AS3 glue code.
                int                 get_length() const { return m_length; }
        /// Is this string empty?
        REALLY_INLINE   bool        isEmpty() const { return m_length == 0; }
        /// Return the width constant.
        REALLY_INLINE   Width       getWidth() const { return Width(m_bitsAndFlags & TSTR_WIDTH_MASK); }
        /// Return the string type.
        REALLY_INLINE   int32_t     getType() const { return ((m_bitsAndFlags & TSTR_TYPE_MASK) >> TSTR_TYPE_SHIFT); }
        /// Return true iff getType() == kDependent.
        REALLY_INLINE   bool        isDependent() const { return (m_bitsAndFlags & (kDependent << TSTR_TYPE_SHIFT)) != 0; }
        /// Return true iff getType() == kStatic.
        REALLY_INLINE   bool        isStatic() const { return (m_bitsAndFlags & (kStatic << TSTR_TYPE_SHIFT)) != 0; }
        /// Is this an interned string?
        REALLY_INLINE   bool        isInterned() const { return (m_bitsAndFlags & TSTR_INTERNED_FLAG) != 0; }
        /// Mark this string as interned.
        REALLY_INLINE   void        setInterned() { m_bitsAndFlags |= TSTR_INTERNED_FLAG; }
        /**
        Return the character at the given position. No index checks!
        @param  index               the index
        @return                     the character at the index
        */
                wchar FASTCALL      charAt(int32_t index) const;

        /**
        Compare the String with toCompare. If the length is > 0, compare
        the other string up to the given length.
        @param  other               the string to compare with
        @param  start               the starting position (in other)
        @param  length              the length to compare (if > 0) (in other)
        @return = 0 if the strings are identical,
                < 0 if this string is less than toCompare,
                > 0 if this string is greater than toCompare
         */
                int32_t FASTCALL    Compare(String& other, int32_t other_start = 0, int32_t other_length = 0) const;

        /**
        Compare this string with another string.
        */
                bool FASTCALL       equals(Stringp that) const;

        /**
        Compare one string with another string where each one could be null.
        This returns an int32_t to work directly with the JIT
        */
        static int32_t FASTCALL       equalsWithNullChecks(Stringp s1, Stringp s2);

        /**
        Compare this string with a Latin1 string.
        */
                bool    FASTCALL    equalsLatin1(const char* p, int32_t len = -1) const;
        /**
        Compare this string with a UTF-16 string.
        */
                bool    FASTCALL    equalsUTF16(const wchar* p, int32_t len) const;

        /*@{*/
        /**
         * Returns the length of str, in # of characters.  If the length does not
         * fit in an int32_t, these functions do not return.
         */
        static int32_t FASTCALL Length(const wchar* str);
        static int32_t FASTCALL Length(const char* str);
        /*@}*/

        /**
        Implements String.indexOf().
        */
                int32_t FASTCALL    indexOf(Stringp s, int32_t offset = 0) const;

        /**
        Convenience method for old code (boolean result)
        */
        REALLY_INLINE   bool        contains(Stringp s) const { return indexOf(s) >= 0; }
        /**
        Convenience method: indexOf() for a Latin-1 string within a given range.
        @param  p                   the character string to compare; NULL returns -1
        @param  len                 the number of characters to compare; if < 0, call Length()
        @param  start               the starting position
        @param  end                 the ending position
        @return                     the index of the found position, or -1 if no match
        */
                int32_t FASTCALL    indexOfLatin1(const char* p, int32_t len = -1, int32_t start = 0, int32_t end = 0x7FFFFFFF) const;

        /**
        Convenience method: indexOf() for a string of length 1.
        @param  c                   the character code to compare
        @param  start               the starting position
        @param  end                 the ending position
        @return                     the index of the found position, or -1 if no match
        */
                int32_t FASTCALL    indexOfCharCode(wchar c, int32_t start = 0, int32_t end = 0x7FFFFFFF) const;
        /**
        Convenience method for old code (boolean result)
        */
        REALLY_INLINE   bool        containsLatin1(const char* p) const { return indexOfLatin1(p) >= 0; }

        /**
        Convenience method: Does a Latin-1 string match at the current position?
        @param  p                   the character string to compare; NULL returns false
        @param  len                 the number of characters to compare; if < 0, call Length()
        @param  pos                 the position to match
        @return                     true if the string matches
        */
                bool     FASTCALL   matchesLatin1(const char* p, int32_t len, int32_t pos);

        /**
        Convenience method: Does a Latin-1 string match at the current position, ignoring case?
        @param  p                   the character string to compare; NULL returns false
        @param  len                 the number of characters to compare; if < 0, call Length()
        @param  pos                 the position to match
        @return                     true if the string matches
        */
                bool     FASTCALL   matchesLatin1_caseless(const char* p, int32_t len, int32_t pos);
        /**
        Implements String.lastIndexOf().
        */
                int32_t FASTCALL    lastIndexOf(Stringp s, int32_t offset = 0x7fffffff) const;
        /**
        Concatenate two strings, and return the result. If the right string fits into the buffer
        end of the left string, append the data and return a new dependent string pointing
        to that buffer. If it does not fit, create a kDynamic string containing the entire
        buffer, with extra padding at the end to support in-place concatenation.
        @param  left                the left string; may be NULL
        @param  right               the right string; may be NULL, although not meaningful
        @return                     the concatenated string
        */
        static  Stringp FASTCALL    concatStrings(Stringp leftStr, Stringp rightStr);
        /**
        Append a String instance.
        @param  src                 the string to append
        @return                     the concatenated string
        */
                Stringp FASTCALL    append(Stringp str);

        /*
        Append a 8-bit-wide string. For Unicode, strings should be Latin1, not UTF8.
        */
        REALLY_INLINE   Stringp     appendLatin1(const char* p) { return _append(NULL, Pointers((const uint8_t*)p), Length(p), k8); }
        REALLY_INLINE   Stringp     appendLatin1(const char* p, int32_t len) { return _append(NULL, Pointers((const uint8_t*)p), len, k8); }
        /*
        Append a 16-bit-wide string. For Unicode, strings should be UTF16, but this is not enforced
        by this method: indeed, several callers expect to be able to create "illegal" UTF16 sequences
        via this call, for backwards compatibility. Thus, this is a dangerous call and should be used with
        caution (and is also the reason it is not named "appendUTF16").
        */
        REALLY_INLINE   Stringp     append16(const wchar* p) { return _append(NULL, Pointers(p), Length(p), k16); }
        REALLY_INLINE   Stringp     append16(const wchar* p, int32_t len) { return _append(NULL, Pointers(p), len, k16); }
        /**
        Implement String.substr(). The resulting String object points into the original string,
        and holds a reference to the original string.
        */
                Stringp FASTCALL    substr(int32_t start, int32_t len = 0x7fffffff);
        /**
        Implement String.substring(). The resulting String object points into the original string,
        and holds a reference to the original string.
        */
                Stringp FASTCALL    substring(int32_t start, int32_t end = 0x7fffffff);
        /**
         * Make a copy in the target core.
         */
            Stringp clone(AvmCore* targetCore);

        /**
        Implement String.slice(). The resulting String object points into the original string,
        and holds a reference to the original string.
        */
                Stringp FASTCALL    slice(int32_t start, int32_t end);
        /**
        This routine is a very specific parser to generate a positive integer from a string.
        The following are supported:
        "0" - one single digit for zero - NOT "00" or any other form of zero
        [1-9]+[0-9]* up to 2^32-2(4294967294)
        2^32-1(4294967295) is not supported(see ECMA quote below).
        The ECMA that we're supporting with this routine is...
        cn:  the ES3 test for a valid array index is
         "A property name P(in the form of a string value) is an array index if and
         only if ToString(ToUint32(P)) is equal to P and ToUint32(P) is not equal to 2^32-1."
        Don't support 000000 as 0.
        We don't support 0x1234 as 1234 in hex since string(1234) doesn't equal '0x1234')
        No leading zeros are supported
        */
                bool    FASTCALL    parseIndex(uint32_t& result) const;
        /**
        Returns a new string object which is a copy of this string object, with all
        characters in the string converted to uppercase.
        Unicode character classes for uppercase and lowercase are used. The conversion
        behavior is compliant with the String.toUpperCase method. The method returns
        this instance if no changes were detected.
        @return                     the resulting string or NULL
        */
                Stringp FASTCALL    toUpperCase();
        /**
        Returns a new string object which is a copy of this string object, with all
        characters in the string converted to lowercase.
        Unicode character classes for uppercase and lowercase are used. The conversion
        behavior is compliant with the String.toUpperCase method. The method returns
        this instance if no changes were detected.
        @return                     the resulting string or NULL
        */
                Stringp FASTCALL    toLowerCase();
        /**
        Change the case of a string according to the case mapper supplied.
        If no changes were detected, return this instance, otherwise, return
        a new instance.
        @param  unimapper           the mapping function to call
        @return                     the changed string
        */
                Stringp FASTCALL    caseChange(uint32_t(*unimapper)(uint32_t));
        /**
        Returns a kIntptrType Atom if the string holds an integer that fits into
        such an atom. For use in our ScriptObject HashTable implementation.  If we
        have a valid integer equivalent, it will never be zero since kIntptrType tag != 0.
        */
                Atom    FASTCALL    getIntAtom() const;
        /**
        This conversion handles hex, octal, base 10 integer, float, and "Infinity"/"-Infinity".
        */
                double              toNumber();
        /**
        Check if this character is a valid space character.
        */
        static  bool                isSpace(wchar ch);
        /**
        Is this string all whitespace?
        */
                bool                isWhitespace() const;

        /// Native functions, used by StringClass.cpp
                int                 _indexOf(Stringp s, int i=0);
                int                 AS3_indexOf(Stringp s, double i=0);

                int                 _lastIndexOf(Stringp s, int i=0x7fffffff);
                int                 AS3_lastIndexOf(Stringp s, double i=0x7fffffff);

                Stringp             _charAtI(int32_t i);
                Stringp             _charAtU(uint32_t i);
                Stringp             AS3_charAt(double i=0);

                double              _charCodeAtDI(int32_t i); // returns NaN for out-of-bounds
                int32_t             _charCodeAtII(int32_t i); // returns 0 for out-of-bounds
                double              AS3_charCodeAt(double i); // returns NaN for out-of-bounds
                int32_t             _charCodeAtID(double i); // returns 0 for out-of-bounds
                double              _charCodeAtDU(uint32_t i); // returns NaN for out-of-bounds
                int32_t             _charCodeAtIU(uint32_t i); // returns 0 for out-of-bounds

                int32_t             AS3_localeCompare(Atom other);

                Stringp             _substring(int i_start, int i_count);
                Stringp             AS3_substring(double d_start, double d_count);

                Stringp             _slice(int dStart, int dEnd);
                Stringp             AS3_slice(double dStart, double dEnd);

                Stringp             _substr(int dStart, int dEnd);
                Stringp             AS3_substr(double dStart, double dEnd);

                Stringp             AS3_toUpperCase();
                Stringp             AS3_toLowerCase();

        // Useful utilities used by the core code.
        static  wchar               wCharToUpper(wchar ch) { return (wchar) unicharToUpper(ch); }
        static  wchar               wCharToLower(wchar ch) { return (wchar) unicharToLower(ch); }
        static  uint32_t            unicharToUpper(uint32_t ch);
        static  uint32_t            unicharToLower(uint32_t ch);
#ifdef DEBUGGER
        virtual uint64_t            bytesUsed() const;
                Stringp             getMasterString() const; // used by profiler
#endif

        PrintWriter& print(PrintWriter& prw) const;

    private:
        friend class StringIndexer;
        friend class StUTF8String;
        friend class StUTF16String;
        friend class CodegenLIR;

    private:
        /**
            This is a union of three different pointers, or an offset value -- you
            must know what type of String this is (static, dynamic, dependent) to know
            how to interpret the field.

            Note that the offset value is always in bytes, regardless of string width!

            *** WARNING ***
            This struct is only used inside of String itself, and should not ever be allocated on the stack.
            If you want to obtain a pointer to the start of the string's character buffer, the simplest
            and safest way to do this is to use the Pointers struct (see below).
        */
        struct Buffer
        {
            union
            {
                void*           pv;
                uint8_t*        p8;
                wchar*          p16;
                uintptr_t       offset_bytes;
            };
            REALLY_INLINE explicit Buffer(const void* _pv) { pv = const_cast<void*>(_pv); }
            REALLY_INLINE explicit Buffer(uintptr_t _offset_bytes) { offset_bytes = _offset_bytes; }
        };

        /**
            Extra storage, for the Master pointer (for dependent strings)
            or index value (lazily calculated for other strings)
        */
        struct Extra
        {
            union
            {
                Stringp         master; // used for dependent strings
        mutable uint32_t        index;  // if not dependent, this is the index value for getIntAtom/parseIndex
            };
            REALLY_INLINE explicit Extra(Stringp _master) { master = _master; }
        };

        // ------------------------ DATA SECTION BEGIN
                Buffer          m_buffer;   // buffer pointer (dynamic, static, or offset into master)
                Extra           m_extra;
                int32_t         m_length;                   // length in characters
        mutable uint32_t        m_bitsAndFlags;             // various bits and flags, see below (must be unsigned)
        // ------------------------ DATA SECTION END

                enum {
                    TSTR_WIDTH_MASK         = 0x00000001,   // string width (right-aligned for fast access)
                    TSTR_TYPE_MASK          = 0x00000006,   // type index, 2 bits
                    TSTR_TYPE_SHIFT         = 1,
                    // If TSTR_7BIT_FLAG is set, the string has width of k8, with no characters having the high bit set.
                    // Thus the string is both 7-bit ascii and "utf8-compatible" as-is; knowing this can produce
                    // huge speedups in code that uses utf8 conversion heavily (in conjunction with "ascii" strings, of course).
                    // Note that this bit is set lazily (and currently, only by StUTF8String), thus, if this bit is clear,
                    // the string might still be 7-bit-ascii... we just haven't checked yet.
                    TSTR_7BIT_FLAG          = 0x00000008,
                    TSTR_7BIT_SHIFT         = 3,
                    TSTR_INTERNED_FLAG      = 0x00000010,   // this string is interned
                    TSTR_NOINT_FLAG         = 0x00000020,   // set in getIntAtom() if the string is not an 28-bit integer
                    TSTR_NOUINT_FLAG        = 0x00000040,   // set in parseIndex() if the string is not an unsigned integer
                    TSTR_UINT28_FLAG        = 0x00000080,   // set if m_index contains value for getIntAtom()
                    TSTR_UINT32_FLAG        = 0x00000100,   // set if m_index contains value for parseIndex()
                    TSTR_CHARSLEFT_MASK     = 0xFFFFFE00,   // characters left in buffer field (for inplace concat)
                    TSTR_CHARSLEFT_SHIFT    = 9
                };

        /**
            This is a TEMPORARY struct, always stack-allocated, that is used to extract the current starting
            pointer for a string. It may look superficially similar to the "Buffer" struct, but is different,
            in that the pointer is always correct (unlike Buffer, which might actually be an offset into a master).

            WARNING: this struct is not valid across a GC collection; if you do anything that could trigger
            a collection (most commonly, any sort of allocation), you must assume the contents of this are invalid.
        */
        struct Pointers
        {
            union
            {
                void*           pv;
                uint8_t*        p8;
                wchar*          p16;
            };
            REALLY_INLINE explicit Pointers(const String* const self)
            {
                AvmAssert(self != NULL);
                if (!self->isDependent())
                    p8 = self->m_buffer.p8;
                else
                    p8 = self->m_extra.master->m_buffer.p8 + self->m_buffer.offset_bytes;
            }
            REALLY_INLINE explicit Pointers(const uint8_t* _p8) { p8 = const_cast<uint8_t*>(_p8); }
            REALLY_INLINE explicit Pointers(const uint16_t* _p16) { p16 = const_cast<uint16_t*>(_p16); }
        };

        REALLY_INLINE   void        setType(char index)         { m_bitsAndFlags = (m_bitsAndFlags & ~TSTR_TYPE_MASK) |(index << TSTR_TYPE_SHIFT); }
        REALLY_INLINE   int32_t     getCharsLeft() const        { return (m_bitsAndFlags & TSTR_CHARSLEFT_MASK) >> TSTR_CHARSLEFT_SHIFT; }
        REALLY_INLINE   void        setCharsLeft(int32_t n)     { m_bitsAndFlags = (m_bitsAndFlags & ~TSTR_CHARSLEFT_MASK) |(n << TSTR_CHARSLEFT_SHIFT); }

        // Create a string with no buffer.
        static  Stringp             createDependent(MMgc::GC* gc, Stringp master, int32_t start, int32_t len);
        // Create a string with a dynamic buffer.
        static  Stringp             createDynamic(MMgc::GC* gc, const void* data, int32_t len, Width w, bool is7bit, int32_t extra=0);
        // Create a string with a static buffer.
        static  Stringp             createStatic(MMgc::GC* gc, const void* data, int32_t len, Width w, bool is7bit);

        // Convert the string data to a dynamic buffer.
                void                convertToDynamic();

        /**
        Low-level append worker.
        */
                Stringp             _append(Stringp rightStrPtr, const Pointers& rightStr, int32_t numChars, Width width);

        #ifdef _DEBUG
            void verify7bit() const;
            //#define VERIFY_7BIT(s) do { if (s) (s)->verify7bit(); } while (0)
            // extremely slow, so disabled by default, even in debug mode.
            #define VERIFY_7BIT(s) do {  } while (0)
        #else
            #define VERIFY_7BIT(s) do { } while (0)
        #endif

        /**
        Make operator new private - people should use the create functions
        */
        REALLY_INLINE   void*       operator new(size_t size, MMgc::GC *gc, MMgc::GCExactFlag)
        {
            return AvmPlusScriptableObject::operator new(size, gc, MMgc::kExact);
        }
        REALLY_INLINE   void        operator delete(void*) {}   // Strings cannot be deleted

        // ctor for a static string.
        REALLY_INLINE               String(const void* buffer, Width w, int32_t length, bool is7bit);
        // ctor for a dynamic string.
        REALLY_INLINE               String(MMgc::GC* gc, void* buffer, Width w, int32_t length, int32_t charsLeft, bool is7bit);
        // ctor for a dependent string.
        REALLY_INLINE               String(MMgc::GC* gc, Stringp master, int32_t start, int32_t length);
    };

    // Compare helpers
    REALLY_INLINE bool operator==(String& s1, String& s2)
    {
        return s1.equals(&s2);
    }
    REALLY_INLINE bool operator!=(String& s1, String& s2)
    {
        return !s1.equals(&s2);
    }
    REALLY_INLINE bool operator<(String& s1, String& s2)
    {
        return s2.Compare(s1) < 0;
    }
    REALLY_INLINE bool operator>(String& s1, String& s2)
    {
        return s2.Compare(s1) > 0;
    }
    REALLY_INLINE bool operator<=(String& s1, String& s2)
    {
        return s2.Compare(s1) <= 0;
    }
    REALLY_INLINE bool operator>=(String& s1, String& s2)
    {
        return s2.Compare(s1) >= 0;
    }

    REALLY_INLINE /*static*/ bool String::isSpace(wchar ch)
    {
        const uint32_t IS_SPACE_MASK =
            (1U << (32-1)) |    // space
            (1U << (9-1)) |     // tab
            (1U << (10-1)) |    // LF
            (1U << (13-1));     // CR

        ch -= 1;
        return (ch < 32) &      // bitwise and, *not* logical and -- avoids a branch
                ((IS_SPACE_MASK & (1U<<ch)) != 0);
    }

    /**
    The StringIndexer class provides quick access to single characters by index.
    Use an instance of this class on the stack if multiple index access is required.
    Previously, this class allowed for faster access, as the interior pointer
    didn't need to be recalculated; later changes made this unsafe, so this class
    exists mostly for legacy purposes at this point.
    */

    class StringIndexer
    {
    public:
        /// The constructor takes the string to index.
        REALLY_INLINE   explicit    StringIndexer(Stringp s) : m_str(s) { AvmAssert(s != NULL); }
        /// Return the embedded string.
        REALLY_INLINE   String*     operator->() const { return m_str; }
        REALLY_INLINE   String*     str() const { return m_str; }
        /// Quick index operator.
        REALLY_INLINE   wchar       operator[](int index) const { return m_str->charAt(index); }

    private:
                Stringp const m_str;

        // do not create on the heap
                void*       operator new(size_t); // unimplemented
                void        operator delete(void*); // unimplemented
    };

    /**
    The StUTF8String class is simply a data buffer containing 0-terminated UTF-8 data.
    The instance can only be created on the stack to preserve the data buffer during GC.
    Note that the length() function returns the length not including the 0-terminator.
    Also note that the string might contain interior NULL characters (if the original
    String did) and thus String::Length, strlen, etc might return misleading values.
    */

    class StUTF8String
    {
    public:
                explicit            StUTF8String(Stringp str);
                                    ~StUTF8String();
        REALLY_INLINE   const char* c_str() const { return m_buffer; }
        REALLY_INLINE   int32_t     length() const { return m_length; }
    private:
        // do not create on the heap
        void*               operator new(size_t); // unimplemented
        void                operator delete(void*); // unimplemented
    private:
                const char*         m_buffer;
                int32_t             m_length;
    };

    /**
    The StUTF16String class is simply a data buffer containing 0-terminated UTF-16 data.
    The instance can only be created on the stack to preserve the data buffer during GC.
    Note that the length() function returns the length not including the 0-terminator.
    Also note that the string might contain interior NULL characters (if the original
    String did) and thus String::Length, strlen, etc might return misleading values.
    If the string is a 32-bit string, characters > 0xFFFF are converted to surrogate pairs.
    */

    class StUTF16String
    {
    public:
                explicit        StUTF16String(Stringp str);
                                ~StUTF16String();
        REALLY_INLINE   const wchar*    c_str() const { return m_buffer; }
        REALLY_INLINE   int32_t         length() const { return m_length; }
    private:
                const wchar*    m_buffer;
                int32_t         m_length;
        // do not create on the heap
        void*                   operator new(size_t); // unimplemented
        void                    operator delete(void*); // unimplemented
    };

    class StIndexableUTF8String : public StUTF8String
    {
    private:
        int32_t     m_lastPos;
        int32_t     m_lastUtf8Pos;
        bool        m_indexable;

    public:

        explicit StIndexableUTF8String(Stringp s);

        /**
        Convert a string index to an UTF-8 index.
        Return the original index if out of range.
        */
        int32_t FASTCALL toUtf8Index(int32_t pos);

        /**
        Convert an UTF-8 index to a string index.
        Return the original index if < 0.
        */
        int32_t FASTCALL toIndex(int32_t uf8Pos);
    };

}

#endif  // __avmplus_NewString__
