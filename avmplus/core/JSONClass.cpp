/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include "BuiltinNatives.h"
#include "TypeDescriber.h"

namespace avmplus
{
    // The design in this file is deliberately modeled after
    // the AS3 JSON implementation by Lars Hansen attached
    // to Bugzilla 584704:
    // https://bugzilla.mozilla.org/show_bug.cgi?id=584704
    // Subsequent devlopment of the C++ version was discussed
    // in Bugzilla 640711:
    // https://bugzilla.mozilla.org/show_bug.cgi?id=640711

    // Architecture overview: The only class visible outside this file
    // is JSONClass, and it provides a parsing method, parseCore, which
    // reads in strings and produces objects, and a stringification
    // method, stringifySpecializedToString, which reads in objects and
    // produces strings.
    //
    //
    // PARSING
    //
    // JSONClass::parseCore is a simple, hopefully fast, parser.  It
    // delegates its work to the JSONParser class, which defines a
    // set of mutually recursive methods.  The parser is simple
    // for the following reasons:
    //
    // 1. The extra effort of running a optionally provided
    //    reviver callback has been left as a post-pass in the AS3 glue
    //    code.  (For why the reviver must be left as a post-pass, see
    //    discussion in Lars's original code on Bugzilla 584704.)
    // 2. The parser makes no attempt to construct instances of AS3
    //    objects; it closely follows the ECMA-262 5th edition specification
    //    for how parsing behaves.  If the client wants AS3 class instances,
    //    for now they must construct them via the reviver callback.
    // 3. Since the reviver callback is a post-pass, control will not
    //    flow into AS3 code from the parser.
    //
    // The main issues that can arise during parsing are thrown exceptions,
    // from ether invalid parse input, or from internal control stack
    // exhaustion from the recursive parsing routines.  Since the code
    // can throw exceptions, the parser is careful not to allocate
    // unmanaged heap memory.
    //
    // The parser maintains a cursor into the input string, representing
    // the start of the next token to be processed.  When the token
    // is more than a single character, the entire string associated with
    // the token is stored in a String object in the parser.
    //
    // Right now the parser only supports parsing String objects.  We will
    // probably generalize it beyond that, initially to ByteArrays and then
    // to classes providing a proper streaming interface to Unicode data.
    // (The IDataInput API does not currently suffice for this.)
    //
    //
    // STRINGIFICATION
    //
    // JSONClass::stringifySpecializedToString is more complex than the
    // parser.  It delegates its work to the JSONSerializer class.
    // Stringification is more complex because:
    //
    // 1. It must interleave invocations of the replacer function with
    //    its traveral of the object graph; this is dictated by ECMA-262
    //    because the client code can observe the traversal's behavior
    //    via toJSON methods and via the replacer function.
    // 2. The stringifier not only traverses the dynamic own properties of
    //    objects (as dictated by ECMA-262), but also the fixed properties
    //    of class instances (which is not part of ECMA-262).
    // 3. It must detect cycles in the input object graph and signal
    //    an exception in response to them.
    //
    // In addition, the JSONSerializer class has gone through a couple
    // iterations of development, and some of the prototypes would maintain
    // references to unmanaged memory on the C stack as it traversed the
    // input object.  This meant that it could not simply throw an
    // AS3 exception the same way that the Parser does; instead it needed
    // to unwind the stack and free the memory properly.  This led to
    // the use of a pattern of passing back a ReturnCondition from
    // the various Stringification methods.  In hindsight; it may have
    // been better practice to maintain all references to unmanaged memory
    // within the JSONSerializer instance, which would have allowed storage
    // to be freed at the point where the exception is first caught and
    // then immediately rethrow it.
    //
    // The structure of the code is meant to parallel the description
    // of the stringification algorithm described in section 15 of
    // ECMA-262 5th ed.  Some deviations in its structure were
    // introduced to avoid creation of intermediate strings during
    // the process.  (And of course, other code changes were introduced
    // to support AS3-only features such as class instances that are
    // not part of ECMA-262.)  It may be useful for the reader to first
    // review the algorithm described there before diving into the code
    // below.

    class JSONParser
    {
        friend class JSONClass;
        JSONParser(Toplevel* toplevel, String* text);

        // Main entry point.  Parses a single value, and checks that
        // there are no subsequent tokens.  Returns the parsed value;
        // throws on syntax error.
        Atom parseNative();

        // Value parser.
        //
        // Returns the parsed value, which is one of null, Boolean,
        // String, Number/int/uint, Array, Object.
        Atom parseValue();

        // Returns Array for successful parse; throws on failed parse.
        ArrayObject* parseArray();

        // Returns Object for successful parse; throws on failed parse.
        ScriptObject* parseObject();

        // Returns Number for successful parse; throws on failed parse.
        Atom parseFloat(String* v);

        // Scanner.
        // Consumes the next token, stores it in 'token' and 'value'.
        // Throws kJSONInvalidParseInput Syntax Error if and only if
        // next token could not be parsed.
        void advance();
        void adv_number();
        void adv_digits();
        void adv_string();

        wchar hexDigitValue(wchar c);
        bool isHexDigit(wchar c);

        void throwParseInputSyntaxError() {
            m_toplevel->syntaxErrorClass()->throwError(kJSONInvalidParseInput);
        }

    private:
        // Notes:
        // - JSONParser class is solely stack-allocated.
        // - Invoking its methods may cause AS3 exceptions to be thrown
        // - Therefore, its stack-allocated members will not always
        //   have their destructors called.  This is okay, since the
        //   members all point only to GC-allocated memory; in
        //   particular, m_textUTF8.m_buffer is GC-allocated.

        Toplevel*  const m_toplevel;
        String*  const m_text;          // The input string
        StUTF8String m_textUTF8;        // input string as utf8 byte array
        uint32_t m_i;                   // Current index in 'm_textUTF8'
        uint32_t const m_len;           // Length of 'm_textUTF8'
        bool m_indexValidForText;       // Implies m_i indexes m_text correctly
        char m_token;                   // The current token

        String* m_value;                // Any value associated with
                                        // the current token (for
                                        // numbers and strings)
    };

    // The purpose of ReturnCondition is to signal what end-result
    // we have ended up in from the stringify call.
    enum ReturnCondition {
        kSomeOutput,      // normal case; emitted > 0 characters
        kNoOutput,        // emitted *zero* characters
        kCycleTraversed,  // cyclic traversal, prepare to throw exception
        kThrewException   // callback threw an exception; percolating up
    };

#define RET_EXN_CHECK(__x__)                                        \
    do { if ((__x__) >= kCycleTraversed) return (__x__); } while (0)

    class JSONSerializer
    {
        friend class JSONClass;

        JSONSerializer(Toplevel* toplevel,
                       Namespace *as3ns,
                       ArrayObject* proplist,
                       FunctionObject* replacer,
                       String* gap);

        String* stringify(Atom value);
    private:
        // Str(key, holder, pendingPrefix, pendingPropnameColon)
        // If holder[key] post JSON conversion is not undefined,
        // then:
        // - emits pendingPrefix,
        // - emits String(key)+":" iff pendingPropnameColon,
        // - emits JSON-formatted JSON-converted holder[key].
        //
        // The invariant is that pendingPropnameColon will be true
        // when called from JO* methods (i.e. object traversals), and
        // false when called from JA* methods (i.e. array traversals).
        //
        // Returns kSomeOutput if holder[key] post JSON
        // conversion was not undefined (and thus we emitted
        // at least one character to the output).
        //
        // Returns kNoOutput if holder[key] post JSON
        // conversion was undefined.
        //
        // Returns kThrewException if the attempt to stringify threw
        // an exception; bubble this result back up (releasing
        // resources along the way) without emitting further output.

        ReturnCondition Str(String* key, ScriptObject* holder,
                            String* pendingPrefix,
                            bool pendingPropnameColon);
        ReturnCondition Str(uint32_t key, ScriptObject* holder,
                            String* pendingPrefix,
                            bool pendingPropnameColon);
        ReturnCondition Str(Multiname const* key, ScriptObject* holder,
                            String* pendingPrefix,
                            bool pendingPropnameColon);

        // StrFoundValue is called only if holder[key] == val and is
        // not undefined.
        ReturnCondition StrFoundValue(Atom val,
                                      Atom key, // key is String or uint32_t
                                      ScriptObject* holder,
                                      String* pendingPrefix,
                                      bool pendingPropnameColon);

        // Attempts call to method via toplevel.
        // If method throws, returns true.
        // If method returns normally, then sets *recv to result and returns false.
        bool TryToplevelCall(Atom* method,
                             int32_t argc,
                             Atom* args,
                             Atom* recv)
        {
            TRY(core(), kCatchAction_Rethrow)
            {
                *recv = avmplus::op_call(m_toplevel, *method, argc, args);
                return false;
            }
            CATCH(Exception* exception)
            {
                m_exception = exception;
                return true;
            }
            END_CATCH
            END_TRY
        }

        bool TryAS3Call(FunctionObject* f,
                        Atom* thisAtom,
                        Atom* args,
                        int32_t argc,
                        Atom* recv)
        {
            TRY(core(), kCatchAction_Rethrow)
            {
                *recv = f->AS3_call(*thisAtom, args, argc);
                return false;
            }
            CATCH(Exception* exception)
            {
                m_exception = exception;
                return true;
            }
            END_CATCH
            END_TRY
        }

        void Quote(String* value);
        void QuoteAtomKey(Atom key);

        ReturnCondition JO(ScriptObject* value);
        ReturnCondition JODescriptions(ScriptObject* names,
                                       ScriptObject* value,
                                       String* pendingPre,
                                       String* connective);
        ReturnCondition JOProp(Atom p,
                               ScriptObject* value,
                               String* pendingPrefix);
        ReturnCondition JOPropMultiname(Atom p,
                                        ScriptObject* value,
                                        String* pendingPrefix);

        ReturnCondition JAarray(ArrayObject* value);
        ReturnCondition JAvector(VectorBaseObject* value);
        ReturnCondition JAfinish(ScriptObject* value, uint32_t len);

    private:
        AvmCore* core() const;
        String* emptyString();
        Atom propName(char const* literal);
        ScriptObject* newobject();
        bool willDefinitelyEmitOn(Atom value);
        bool hasTransientMetadata(ScriptObject* description);
        bool stackOverflowCheck();

        // void emit(char const* literal);

        void emit(char ch);
        void emit(char const* buf, uint32_t len);
        void emit(utf8_t const* buf, uint32_t len);
        void emit(String* chars);

        void committedToEmitFor(Atom key, String* pending,
                                bool pendingPropnameColon);

        bool isVectorInstance(ScriptObject* obj);

        // if value is parent of object in current traversal then
        // returns true.  Otherwise marks value as parent of
        // objects in current traversal and returns false.
        bool valueActive(ScriptObject* value);
        // unmarks value (ie not parent of any object in current traversal)
        void valueNotActive(ScriptObject* value);

        // The purpose of AutoDestructingAtomArray is to build up a
        // worklist of property names to traverse during stringify.
        // (A hypothetical direct traversal that recursively processes
        //  the properties easily hits stack overflow problems.)
        //
        // Note that it allocates the worklist via the given FixedMalloc
        // allocator and automatically frees the worklist during destruction;
        // thus the stringify kernel must be careful to catch any AS3
        // exception from a callback and unroll our stack properly
        // before rethrowing the exception.
        //
        // Note also that it is an anti-pattern to be storing Atoms in
        // the worklist (a FixedMalloc-allocated array) in this
        // fashion, because the worklist will not be traced by the
        // garbage collector.  The argument justifying it in this case
        // is that all of these atoms are property names that are kept
        // alive by the object we are stringifying, but that is not
        // a great argument.  Also this code would completely break
        // in the presence of a copying garbage collector.
        struct AutoDestructingAtomArray {
            AutoDestructingAtomArray(MMgc::FixedMalloc* fm, int32_t atomCount)
                : m_atoms(NULL)
                , m_atomCount(atomCount)
                , m_fixedmalloc(fm)
            {
                if (atomCount > 0)
                    m_atoms = (Atom*)fm->Alloc(atomCount*sizeof(Atom));
            }

            ~AutoDestructingAtomArray() {
                if (m_atomCount > 0)
                    m_fixedmalloc->Free(m_atoms);
            }

            Atom*              m_atoms;
            int32_t            m_atomCount;
            MMgc::FixedMalloc* m_fixedmalloc;
        };

        // The purpose of Rope is to allow the stringify code to emit
        // its output incrementally.  Since we do not know a priori
        // how much output may be associated with a particular input,
        // we cannot preallocate the output buffer.  So we avoid the
        // O(n lg n) time blowup (where n is output size) associated
        // with automatically resizing (and recopying) the target
        // buffer every time the emission hits the string's capacity
        // threshold.  Instead we emit to a sequence of fixed length
        // buffers, and then after emission is complete, we assemble
        // the final output string in one pass, thus maintaining a
        // O(n) time bound.  (Unlike some other rope structures, this
        // uses a list rather than a tree; this works because the
        // use-case is quite specialized here.)
        struct Rope
        {
            struct Chunk
            {
                typedef utf8_t bufelem_t;

                // Arbitrarily picking BUF_SIZE such that
                // sizeof(Chunk) is approximately 4096 bytes.
                static const int32_t BUF_SIZE =
                    (4096 - 2*sizeof(intptr_t))/sizeof(bufelem_t);
                // (Alternative BUF_SIZE with small chunks for testing)
                // static const int32_t BUF_SIZE = 16;

                Chunk* m_next;
                int32_t m_cursor;
                bufelem_t m_payload[BUF_SIZE];

                REALLY_INLINE bool exhausted() {
                    AvmAssert(m_cursor <= BUF_SIZE);
                    return m_cursor == BUF_SIZE;
                }

                // amount of space left in m_payload array
                // measured in bufelem_t's.
                REALLY_INLINE int32_t avail() { return BUF_SIZE - m_cursor; }

                REALLY_INLINE int32_t min(int32_t x, int32_t y) { return (x<y)?x:y; }

                // returns amount actually written, measured in bufelem_t's
                REALLY_INLINE int32_t emit(bufelem_t const* src, int32_t len) {
                    int32_t write_amt = min(len, avail());
                    AvmAssert(write_amt > 0);
                    memcpy(&m_payload[m_cursor],
                           src,
                           write_amt*sizeof(bufelem_t));
                    m_cursor += write_amt;
                    return write_amt;
                }
            };

            Chunk* newChunk(Chunk* prev) {
                Chunk* ret = (Chunk*)m_fixedmalloc->Alloc(sizeof(Chunk));
                prev->m_next = ret;
                ret->m_next = NULL;
                ret->m_cursor = 0;
                return ret;
            }

             REALLY_INLINE void emit(char ch) {
                 utf8_t b = ch;
                 this->emit(&b, 1);
             }
             REALLY_INLINE void emit(utf8_t const* buf, int32_t len) {
                 while (len > 0) {
                     int32_t wrote = m_ropeEnd->emit(buf, len);
                     len -= wrote;
                     AvmAssert(len >= 0);
                     buf += wrote;
                     m_len += wrote;
                     if (m_ropeEnd->exhausted()) {
                         Chunk* newchunk = newChunk(m_ropeEnd);
                         m_ropeEnd = newchunk;
                     }
                 }
             }

             String* toStringViaConcat(AvmCore* core) {
                 char* concatenated = concat();
                 int32_t len = length();
                 String* str = core->newStringUTF8(concatenated, len);
                 m_fixedmalloc->Free(concatenated);
                 return str;
             }

#if 0
             // Bugzilla 650918: toStringViaAppend() avoids building
             // intermediate concatenated array; but auto-growing
             // strings suffer O(n lg n) time cost, versus O(n) for
             // toStringViaConcat() above.  If String constructor had
             // parameter for initial capacity, then below could win.
             //
             // (It would need to be vetted for correctness, of course;
             //  in particular, multi-byte codepoints at the edges of
             //  chunks would need to be addressed.)
             String* toStringViaAppend(AvmCore* core) {
                 Chunk* c = m_ropeStart;
                 String* str = core->newStringUTF8(c->m_payload,
                                                   c->m_cursor);
                 c = c->m_next;
                 while (c != NULL) {
                     str = str->appendLatin1(c->m_payload, c->m_cursor);
                     c = c->m_next;
                 }
                 return str;
             }
#endif
             int32_t length() { return m_len; }
             int32_t checkLength() {
                 int32_t ret = 0;
                 Chunk* r = m_ropeStart;
                 while (r != NULL) {
                     ret += r->m_cursor;
                     r = r->m_next;
                 }
                 return ret;
             }

             // it is responsbility of caller to free the returned buffer!
             char* concat() {
                 AvmAssert(checkLength() == m_len);
                 char* dst = (char*)m_fixedmalloc->Alloc(m_len);
                 char* end = dst;
                 Chunk* r = m_ropeStart;
                 while (r != NULL) {
                     memcpy(end, r->m_payload, r->m_cursor);
                     end += r->m_cursor;
                     r = r->m_next;
                 }
                 return dst;
             }

             void freeChunks() {
                 Chunk* r = m_ropeStart;
                 Chunk* next;
                 while (r != NULL) {
                     next = r->m_next;
                     m_fixedmalloc->Free(r);
                     r = next;
                 }
                 m_ropeStart = NULL;
                 m_ropeEnd   = NULL;
             }

             Rope(MMgc::FixedMalloc* fm)
                 : m_fixedmalloc(fm)
                 , m_len(0)
             {
                 Chunk* firstChunk = (Chunk*)fm->Alloc(sizeof(Chunk));
                 firstChunk->m_next = NULL;
                 firstChunk->m_cursor = 0;
                 m_ropeStart = firstChunk;
                 m_ropeEnd   = m_ropeStart;
             }

             Chunk*             m_ropeStart;
             Chunk*             m_ropeEnd;
             MMgc::FixedMalloc* m_fixedmalloc;
             int32_t            m_len;
         };


    private:
        // (Note: JSONSerializer class is solely stack-allocated.)

        Toplevel* const  m_toplevel;
        Namespace* const m_as3ns;
        ArrayObject*     m_proplist; // null implies enumerate all properties
        FunctionObject*  m_replacerFunction; // null implies no replacer
        String* const    m_gap;
        String*          m_indent;

        // (As far as Felix can tell, there is no stack-allocated hash
        // table class.  That is probably not a huge loss, as
        // hash-tables in general must be expandable, and we are
        // trying to get rid of calloc-abuse.)
        HeapHashtable*   m_activeValues;

        // some preallocated strings
        String*          m_str_toJSON;
        String*          m_str_comma;
        String*          m_str_newline;
        String*          m_str_commanewline;

        Exception* m_exception;         // If a callback throws then
                                        // we carry the resulting
                                        // exception here for
                                        // percolation.


        MMgc::FixedMalloc*     m_fixedmalloc;

        // These should *only* be written-to by the emit methods
        // and only read-from by the stringify method.
        Rope m_rope;
    };

    JSONClass::JSONClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {

        // Bug 651971: Lars says ES5 spec requires
        // JSON's [[Prototype]] is Object.prototype.
        // If that's not happening already,
        // probably need to make it so here.

    }

    Atom JSONClass::parseCore(String* text)
    {
        JSONParser parser(vtable->toplevel(), text);
        return parser.parseNative();
    }

    String* JSONClass::stringifySpecializedToString(Atom value,
                                                    ArrayObject* proplist,
                                                    FunctionObject* replacer,
                                                    String* gap)
    {
        Toplevel *toplevel = vtable->toplevel();
        Namespace *as3ns = get_as3ns();
        JSONSerializer serializer(toplevel, as3ns, proplist, replacer, gap);
        return serializer.stringify(value);
    }

    JSONParser::JSONParser(Toplevel* toplevel, String* text)
        : m_toplevel(toplevel)
        , m_text(text)
        , m_textUTF8(text)
        , m_i(0)
        , m_len(uint32_t(m_textUTF8.length()))
        , m_indexValidForText(true)
        , m_token('\0')
        , m_value(NULL)
    { }

        // If the reviver is null then the parser returns null, Boolean, Number/int/uint, String, Array, or Object.

    Atom JSONParser::parseNative()
    {
        advance();
        Atom atom = parseValue();

        // even if an initial parse succeeds, must confirm that
        // subsequent call to advance found no remaining tokens.
        if (m_token != '\0')
            throwParseInputSyntaxError();

        return atom;
    }

    // Value parser.
    //
    // Returns the parsed value, which is one of null, Boolean,
    // String, Number/int/uint, Array, or Object if parse is
    // successful.
    //
    // If the parse fails, throw kJSONInvalidParseInput Syntax Error
    //
    // Copied below from original AS3 by lhansen:
    // ISSUE: AS3 has a richer notion of number than ECMAScript, and for numbers in the int
    // range we could return an int value rather than a Number value.  There could be a
    // performance impact in some cases.  At present we return a Number value (actually
    // the result of parseFloat).

    Atom JSONParser::parseValue()
    {
        m_toplevel->core()->stackCheck(m_toplevel);

        String* v;
        switch (m_token) {
        case 'n':
            advance();
            return nullObjectAtom;
        case 't':
            advance();
            return trueAtom;
        case 'f':
            advance();
            return falseAtom;
        case '"':
            v = m_value;
            advance();
            return v->toAtom();
        case '0':
            v = m_value;
            advance();
            return parseFloat(v);
        case '{': {
            ScriptObject* obj = parseObject();
            AvmAssert(obj != NULL);
            return obj->toAtom();
        }
        case '[': {
            ArrayObject* arr = parseArray();
            AvmAssert(arr != NULL);
            return arr->toAtom();
        }
        default:
            throwParseInputSyntaxError();
            return undefinedAtom; /*unreachable*/
        }
    }

    Atom JSONParser::parseFloat(String* s)
    {
        AvmAssert(s != NULL);
        double result;

        // Returning kNaN becuase that matches behavior of parseFloat.
        if (!MathUtils::convertStringToDouble(s, &result, false))
            result = MathUtils::kNaN;

        return m_toplevel->core()->doubleToAtom(result);
    }

    ArrayObject* JSONParser::parseArray()
    {
        advance();
        ArrayObject* a = m_toplevel->arrayClass()->newArray();
        if (m_token != ']') {
            for (;;) {
                uint32_t a_len = a->getLength();
                Atom val = parseValue();
                a->setUintProperty(a_len, val);
                if (m_token != ',')
                    break;
                advance();
            }
        }
        if (m_token != ']')
            throwParseInputSyntaxError();
        advance();
        return a;
    }

    ScriptObject* JSONParser::parseObject()
    {
        advance();

        Atom oa = m_toplevel->objectClass->constructObject();
        ScriptObject* o = AvmCore::atomToScriptObject(oa);
        if (m_token != '}') {
            for (;;) {
                // Could short-cut the string alloc when result is
                // parseIndex-able (e.g. specialized digits parser).
                Atom name = parseValue();

                if (! AvmCore::isString(name))
                    throwParseInputSyntaxError();
                if (m_token != ':')
                    throwParseInputSyntaxError();

                advance();

                Atom val = parseValue();

                // Avoid interning if property name is parseIndex-able.
                uint32_t recv;
                if (m_toplevel->core()->atomToString(name)->parseIndex(recv)) {
                    o->setUintProperty(recv, val);
                } else {
                    name = m_toplevel->core()->intern(name)->atom();
                    o->setAtomProperty(name, val);
                }
                if (m_token != ',')
                    break;
                advance();
            }
        }
        if (m_token != '}')
            throwParseInputSyntaxError();

        advance();
        return o;
    }


        // Scanner.
        // Consumes the next token, stores it in 'token' and 'value'.

    void JSONParser::advance()
    {
        // resetting token to '\0':
        // 1. avoids infinite regress on e.g. JSON.parse("["), and
        // 2. allows parse() to check that entire string has been processed.
        m_token = '\0';

        while (m_i < m_len) {
            char c = m_textUTF8.c_str()[m_i];
            switch (c) {
            case '\t':
            case '\n':
            case '\r':
            case ' ':
                m_i++;
                continue;
            case ',':
            case ':':
            case '{':
            case '}':
            case '[':
            case ']':
                m_i++;
                m_token = c;
                return;
            case '"':
                adv_string();
                return;
            case '-':
            case '.':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                adv_number();
                return;
            case 'n':
                if (m_i+3 < m_len &&
                    m_textUTF8.c_str()[m_i+1] == 'u' &&
                    m_textUTF8.c_str()[m_i+2] == 'l' &&
                    m_textUTF8.c_str()[m_i+3] == 'l')
                {
                    m_i += 4;
                    m_token = 'n';
                    return;
                } else {
                    throwParseInputSyntaxError();
                }
            case 't':
                if (m_i+3 < m_len &&
                    m_textUTF8.c_str()[m_i+1] == 'r' &&
                    m_textUTF8.c_str()[m_i+2] == 'u' &&
                    m_textUTF8.c_str()[m_i+3] == 'e')
                {
                    m_i += 4;
                    m_token = 't';
                    return;
                } else {
                    throwParseInputSyntaxError();
                }
            case 'f':
                if (m_i+4 < m_len &&
                    m_textUTF8.c_str()[m_i+1] == 'a' &&
                    m_textUTF8.c_str()[m_i+2] == 'l' &&
                    m_textUTF8.c_str()[m_i+3] == 's' &&
                    m_textUTF8.c_str()[m_i+4] == 'e')
                {
                    m_i += 5;
                    m_token = 'f';
                    return;
                } else {
                    throwParseInputSyntaxError();
                }
            default:
                throwParseInputSyntaxError();
            }
        }
    }

    void JSONParser::adv_number() {
        uint32_t start = m_i;
        uint32_t c;

        if (m_textUTF8.c_str()[m_i] == '-')
            m_i++;

        adv_digits();

        if (m_i < m_len && m_textUTF8.c_str()[m_i] == '.') {
            m_i++;
            adv_digits();
        }

        if (m_i < m_len && ((c = m_textUTF8.c_str()[m_i]) == 'e' || c == 'E')) {
            m_i++;
            if (m_i < m_len && ((c = m_textUTF8.c_str()[m_i]) == '-' || c == '+'))
                m_i++;
            adv_digits();
        }

        if (m_indexValidForText)
            m_value = m_text->substring(start, m_i);
        else
            m_value = m_toplevel->core()->newStringUTF8(&m_textUTF8.c_str()[start], m_i - start);

        m_token = '0';
    }

    void JSONParser::adv_digits()
    {
        uint32_t start = m_i;
        uint32_t c;

        while (m_i < m_len && ((c = m_textUTF8.c_str()[m_i]) >= 48 && c <= 57))
            m_i++;

        if (m_i <= start)
            throwParseInputSyntaxError();
    }

    void JSONParser::adv_string()
    {
        uint32_t start;
        String* buffer = m_toplevel->core()->newStringLatin1("");

        // Consume the initial double-quote
        m_i++;

        start = m_i;
        while (m_i < m_len) {
            uint32_t c = m_textUTF8.c_str()[m_i];
            if (c < 32)
                throwParseInputSyntaxError();
            if (c >= 128)
                m_indexValidForText = false;
            if (c == 34)    // double-quote
                break;
            if (c == 92)  { // backslash
                if (m_indexValidForText)
                    buffer = buffer->append(m_text->substring(start, m_i));
                else
                    buffer = buffer->append
                        (m_toplevel->core()->newStringUTF8
                         (&m_textUTF8.c_str()[start], m_i - start));
                m_i++;
                if (m_i == m_len)
                    throwParseInputSyntaxError();
                switch (c = m_textUTF8.c_str()[m_i]) {
                case  '"': buffer = buffer->appendLatin1("\""); break;
                case  '/': buffer = buffer->appendLatin1("/"); break;
                case '\\': buffer = buffer->appendLatin1("\\"); break;
                case  'b': buffer = buffer->appendLatin1("\b"); break;
                case  'f': buffer = buffer->appendLatin1("\f"); break;
                case  'n': buffer = buffer->appendLatin1("\n"); break;
                case  'r': buffer = buffer->appendLatin1("\r"); break;
                case  't': buffer = buffer->appendLatin1("\t"); break;
                case  'u':
                    m_i++;
                    wchar d1, d2, d3, d4;
                    if (m_i + 3 < m_len &&
                        isHexDigit(d1 = m_textUTF8.c_str()[m_i]) &&
                        isHexDigit(d2 = m_textUTF8.c_str()[m_i+1]) &&
                        isHexDigit(d3 = m_textUTF8.c_str()[m_i+2]) &&
                        isHexDigit(d4 = m_textUTF8.c_str()[m_i+3]))
                    {
                        wchar charcode =
                            wchar((hexDigitValue(d1) << 12)
                                  + (hexDigitValue(d2) << 8)
                                  + (hexDigitValue(d3) << 4)
                                  + (hexDigitValue(d4)));

                        // FSK: copied below from AS3_fromCharCode
                        // note: this code is allowed to construct a string
                        // containing illegal UTF16 sequences!
                        //
                        // FSK: I copied the note, but really should
                        // check if the example from AS3_fromCharCode also
                        // applies here, and if so, what does that imply
                        // about the JSON parser?
                        buffer = buffer->append16(&charcode, 1);

                        m_i += 3;
                        break;
                    }
                    else
                    {
                        throwParseInputSyntaxError();
                    }
                default:
                    throwParseInputSyntaxError();
                }
                m_i++;
                start = m_i;
            }
            else
                m_i++;
        }
        if (m_indexValidForText)
            buffer = buffer->append(m_text->substring(start, m_i));
        else
            buffer = buffer->append
                (m_toplevel->core()->newStringUTF8
                 (&m_textUTF8.c_str()[start], m_i - start));

        if (m_i == m_len || m_textUTF8.c_str()[m_i] != 34)  // double-quote
            throwParseInputSyntaxError();
        m_i++;

        m_value = buffer;
        m_token = '\"';
    }

    bool JSONParser::isHexDigit(wchar c)
    {
        return (c >= '0' && c <= '9')
            || (c >= 'A' && c <= 'F')
            || (c >= 'a' && c <= 'f');
    }

    // adapted from MathUtils.cpp:parseIntDigit, but restricted to
    // hex; still, the general functionality could probably be
    // usefully lifted out of MathUtils.
    //
    // requires: isHexDigit(c)
    wchar JSONParser::hexDigitValue(wchar ch)
    {
        if (ch >= '0' && ch <= '9') {
            return (ch - '0');
        } else if (ch >= 'a' && ch <= 'f') {
            return (ch - 'a' + 10);
        } else if (ch >= 'A' && ch <= 'F') {
            return (ch - 'A' + 10);
        } else {
            AvmAssert(false); return ch;
        }
    }

    JSONSerializer::JSONSerializer(Toplevel* toplevel,
                                   Namespace *as3ns,
                                   ArrayObject* proplist,
                                   FunctionObject* replacer,
                                   String* gap)
        : m_toplevel(toplevel)
        , m_as3ns(as3ns)
        , m_proplist(proplist)
        , m_replacerFunction(replacer)
        , m_gap(gap)
        , m_exception(NULL)
        , m_fixedmalloc(MMgc::FixedMalloc::GetFixedMalloc())
        , m_rope(m_fixedmalloc)
    {
        m_indent = emptyString();
        m_activeValues = HeapHashtable::create(core()->GetGC());

        m_str_toJSON   = core()->internStringLatin1("toJSON");
        m_str_comma           = core()->newStringLatin1(",");
        m_str_newline         = core()->newStringLatin1("\n");
        m_str_commanewline    = core()->newStringLatin1(",\n");
    }

    REALLY_INLINE AvmCore* JSONSerializer::core() const
    {
        return m_toplevel->core();
    }

    REALLY_INLINE String* JSONSerializer::emptyString()
    {
        return core()->kEmptyString;
    }

    REALLY_INLINE Atom JSONSerializer::propName(char const* literal)
    {
        return core()->internStringLatin1(literal)->atom();
    }

    REALLY_INLINE ScriptObject* JSONSerializer::newobject()
    {
        // Would it be worthwhile to pass hashtable htCapacity param below?
        Atom oa = m_toplevel->objectClass->constructObject();
        return AvmCore::atomToScriptObject(oa);
    }

    REALLY_INLINE void JSONSerializer::emit(char c)
    {
        m_rope.emit(c);
    }

    REALLY_INLINE void JSONSerializer::emit(char const* buf, uint32_t len)
    {
        m_rope.emit((utf8_t const*)buf, len);
    }

    REALLY_INLINE void JSONSerializer::emit(utf8_t const* buf, uint32_t len)
    {
        m_rope.emit(buf, len);
    }

    REALLY_INLINE void JSONSerializer::emit(String* chars)
    {
        StUTF8String charsUTF8(chars);
        int32_t len = charsUTF8.length();
        m_rope.emit((const utf8_t*)charsUTF8.c_str(), len);
    }

    REALLY_INLINE void JSONSerializer::QuoteAtomKey(Atom key)
    {
        // keys can only be uint32_t or String.
        switch (atomKind(key)) {
        case kStringType:
            Quote(core()->atomToString(key));
            return;
        default:
            AvmAssert(atomKind(key) == kIntptrType);
            AvmAssert(atomCanBeUint32(key));
            Quote(core()->uintToString(uint32_t(atomGetIntptr(key))));
            return;
        }
    }

    REALLY_INLINE void JSONSerializer::committedToEmitFor(Atom key,
                                                          String* pending,
                                                          bool pendingPropnameColon)
    {
        emit(pending);
        if (pendingPropnameColon) {
            QuoteAtomKey(key);
            emit(':');
            if (!m_gap->isEmpty()) {
                // Lars has note in his reference implementation about
                // missing emph around "space" in ECMA-262 spec.
                // Felix has checked with ECMA committee, it was a typo.
                emit(' ');
            }
        }
    }

    bool JSONSerializer::valueActive(ScriptObject* value)
    {
        AvmAssert(value != NULL);
        Atom atom = value->atom();

        if (m_activeValues->contains(atom)) {
            return true;
        } else {
            m_activeValues->add(atom, trueAtom);
            return false;
        }
    }

    void JSONSerializer::valueNotActive(ScriptObject* value)
    {
        AvmAssert(value != NULL);
        Atom atom = value->atom();
        AvmAssert(m_activeValues->contains(atom));
        m_activeValues->remove(atom);
    }

    bool JSONSerializer::isVectorInstance(ScriptObject* o)
    {
        Traits* traits = o->vtable->traits;

        // Bug 652129:
        // A request for comment from others:
        //
        // Consider the AS3 code
        //   var vb : Vector.<Boolean> = new Vector.<Boolean>(5, true);
        // and let T_vb be its traits, ie. T_vb = vb_obj->vtable->traits
        //
        // For some reason,
        // in AS3:
        //   (vb is Vector)                     ==> false
        //   (vb is Vector.<*>)                 ==> true
        //   (vb is Vector.<Object>)            ==> false
        // in C++:
        //   T_vb->subtypeof(vector_itraits)    ==> false
        //   T_vb->subtypeof(vectorobj_itraits) ==> true
        //
        // Why is this?  Is it because Vector.<*> in AS3 corresponds
        // to vectorobj_itraits in C++?  Where is this documented?
        //
        // In any case, it seems from actual runs that to cover all of
        // the cases I need to do a subtypeof check for all the
        // variants of Vector.<>; (this is absolutely fine with me;
        // there is no reason for the variants of a parametric type to
        // share a common supertype.)
        //
        // But I would like to better understand what vector_itraits
        // is meant to denote.  From PoolObject.cpp, I infer that
        // vector_itraits might represent the type constructor Vector
        // itself, which is not a type (ie 'not of kind *' in PL
        // speak) but rather a type constructor (ie 'of kind * -> *')
        // If that is true, then I should not include vector_itraits
        // in the conditions below.  But it also might not matter
        // if one cannot actually construct an object whose traits
        // would ever say true for subtypeof(vector_itraits).
        return
            (traits->subtypeof(core()->traits.vectorobj_itraits)    ||
             traits->subtypeof(core()->traits.vectordouble_itraits) ||
             traits->subtypeof(core()->traits.vectorint_itraits)    ||
             traits->subtypeof(core()->traits.vectoruint_itraits)   ||
#ifdef VMCFG_FLOAT
             traits->subtypeof(core()->traits.vectorfloat_itraits)    ||
             traits->subtypeof(core()->traits.vectorfloat4_itraits)   ||
#endif
#if 0
             traits->subtypeof(core()->traits.vector_itraits)       ||
#endif
             false);
    }

    String* JSONSerializer::stringify(Atom value)
    {
        ScriptObject* wrapper = newobject();
        wrapper->setAtomProperty(emptyString()->atom(), value);
        ReturnCondition ret;
        ret = Str(emptyString(), wrapper, emptyString(), false);

        String* str;
        Exception* exn;

        switch (ret) {
        case kSomeOutput:
            str = m_rope.toStringViaConcat(core());
            m_rope.freeChunks();
            return str;
        case kThrewException:
            exn = m_exception;
            m_rope.freeChunks();
            core()->throwException(exn);
        case kCycleTraversed:
            m_rope.freeChunks();
            m_toplevel->typeErrorClass()->throwError(1129);
        case kNoOutput:
            m_rope.freeChunks();
        }
        return NULL;
    }

    ReturnCondition JSONSerializer::Str(uint32_t key,
                                        ScriptObject* holder,
                                        String* pendingPrefix,
                                        bool pendingPropnameColon)
    {
        Atom value;
        value = holder->getUintProperty(key);
        return StrFoundValue(value, core()->uintToAtom(key), holder,
                             pendingPrefix, pendingPropnameColon);
    }

    ReturnCondition JSONSerializer::Str(String* key,
                                        ScriptObject* holder,
                                        String* pendingPrefix,
                                        bool pendingPropnameColon)
    {
        Atom value;
        value = holder->getAtomProperty(key->atom());
        return StrFoundValue(value, key->atom(), holder,
                             pendingPrefix, pendingPropnameColon);
    }

    ReturnCondition JSONSerializer::Str(Multiname const* name,
                                        ScriptObject* holder,
                                        String* pendingPrefix,
                                        bool pendingPropnameColon)
    {
        // Atom value = holder->getMultinameProperty(name);
        Atom holderAtom = holder->atom();
        VTable* vtable = m_toplevel->toVTable(holderAtom);
        Atom value = m_toplevel->getproperty(holderAtom, name, vtable);

        return StrFoundValue(value, name->getName()->atom(), holder,
                             pendingPrefix, pendingPropnameColon);
    }

    bool JSONSerializer::stackOverflowCheck()
    {
        TRY(core(), kCatchAction_Rethrow)
        {
            core()->stackCheck(m_toplevel);
            return false;
        }
        CATCH(Exception* exception)
        {
            m_exception = exception;
            return true;
        }
        END_CATCH
        END_TRY
    }

    ReturnCondition JSONSerializer::StrFoundValue(Atom value,
                                                  Atom key,
                                                  ScriptObject* holder,
                                                  String* pendingPrefix,
                                                  bool pendingPropnameColon)
    {
        if (stackOverflowCheck())
            return kThrewException;

        if (!AvmCore::isNullOrUndefined(value)) {
            Atom probe = nullObjectAtom;
            VTable* vtable = m_toplevel->toVTable(value);

            // hasproperty tells us if toJSON is present even if it is a
            // method of an AS3 class; such a toJSON method does need to
            // be public to be visible to hasproperty.
            //
            // We cannot apply this simple pattern to everywhere else in
            // this file, though, because the name passed to hasproperty
            // must be a multiname, and the numeric indices that the JA
            // methods pass as names should be handled by the
            // getUintProperty method instead of the Toplevel hasproperty.

            // Bug 652200: can't use getprop to lookup the (potentially
            // overridden) toJSON property on XML objects directly,
            // because getprop has special behavior for public names on
            // XML objects.  We resolve this by adding another level of
            // indirection: JSON will lookup toJSON in the AS3 namespace
            // first, and the public namespace second.

            Multiname name;
            name.setNamespace(core()->findPublicNamespace());
            name.setName(m_str_toJSON);

            Multiname as3name;
            as3name.setNamespace(m_as3ns);
            as3name.setName(m_str_toJSON);

            if (m_toplevel->hasproperty(value, &as3name, vtable))
            {
                probe = m_toplevel->getproperty(value, &as3name, vtable);
            }
            else if (m_toplevel->hasproperty(value, &name, vtable))
            {
                probe = m_toplevel->getproperty(value, &name, vtable);
            }

            // toJSON dispatch only when probe is Callable value.
            if (core()->isFunction(probe)) {
                // toJSON takes only strings for key, regardless of what
                // form the properties take internally.
                if (atomIsIntptr(key)) {
                    AvmAssert(atomCanBeUint32(key));
                    uint32_t uintKey = uint32_t(atomGetIntptr(key));
                    key = core()->uintToString(uintKey)->atom();
                }

                Atom args[2] = { value, key };
                if (TryToplevelCall(&probe, 1, args, &value))
                    return kThrewException;
            }
        }

        if (m_replacerFunction != NULL) {
            const int argc = 2;

            // replacer takes only strings for key, regardless of what
            // form the properties take internally.
            if (atomIsIntptr(key)) {
                AvmAssert(atomCanBeUint32(key));
                uint32_t uintKey = uint32_t(atomGetIntptr(key));
                key = core()->uintToString(uintKey)->atom();
            }

            Atom holderAtom = holder->atom();
            Atom args[argc] = { key, value };
            if (TryAS3Call(m_replacerFunction, &holderAtom, args, argc, &value))
                return kThrewException;
        }

        // Note: potential AS3 callbacks are complete for the
        // remainder of this method when invoked from
        // JSON.stringify().  (Of course this is not true for
        // JSON.stringifyTo(), where every emit() is a callback into
        // an IDataOutput method)

        // Now emit the JSON string for the value.

        if (core()->isNull(value)) {
            committedToEmitFor(key, pendingPrefix, pendingPropnameColon);
            emit("null", 4);
            return kSomeOutput;
        }

        ScriptObject* obj;
        ArrayObject* array;
        double d;
        switch (atomKind(value)) {
        case kBooleanType:
            committedToEmitFor(key, pendingPrefix, pendingPropnameColon);
            if (value == trueAtom)
                emit("true", 4);
            else
                emit("false", 5);
            return kSomeOutput;
        case kStringType:
            committedToEmitFor(key, pendingPrefix, pendingPropnameColon);
            Quote(core()->atomToString(value));
            return kSomeOutput;
        case kIntptrType:
        case kDoubleType:
            committedToEmitFor(key, pendingPrefix, pendingPropnameColon);
            d = core()->number(value);
            if (m_toplevel->isFinite(NULL, d))
                emit(core()->doubleToString(d));
            else
                emit("null", 4);
            return kSomeOutput;
        case kObjectType:
            obj = AvmCore::atomToScriptObject(value);
            if (!core()->isFunction(value)) {
                committedToEmitFor(key, pendingPrefix, pendingPropnameColon);

                array = obj->toArrayObject();
                if (array != NULL)
                    return JAarray(array);
                if (isVectorInstance(obj)) {
                    // Bug 652129:
                    // Is there another option besides the cast below?
                    VectorBaseObject* vobj = (VectorBaseObject*)obj;
                    return JAvector(vobj);
                }

                return JO(obj);

            } else {
                return kNoOutput;
            }
#ifdef VMCFG_FLOAT
        case kSpecialBibopType:
            if (value != undefinedAtom)
            {
                if (AvmCore::isFloat(value))
                {
                    committedToEmitFor(key, pendingPrefix, pendingPropnameColon);

                    d = AvmCore::atomToFloat(value);
                    if (m_toplevel->isFinite(NULL, d))
                        emit(core()->doubleToString(d));
                    else
                        emit("null", 4);
                    return kSomeOutput;
                }
                else if (AvmCore::isFloat4(value))
                {
                    committedToEmitFor(key, pendingPrefix, pendingPropnameColon);

                    // OPTIMIZEME: It would be better if this didn't cons a new object with a new
                    // dynamic property table every time it needed to format a float4.

                    float4_t f = AvmCore::atomToFloat4(value);
                    const float* floats = reinterpret_cast<const float*>(&f);
                    ScriptObject* obj = m_toplevel->builtinClasses()->get_ObjectClass()->construct();
                    const String *names[] = { core()->kx, core()->ky, core()->kz, core()->kw };
                    for ( int i=0 ; i < 4 ; i++ )
                    {
                        Atom val = m_toplevel->isFinite(NULL, floats[i]) ? core()->floatToAtom(floats[i]) : core()->knull->atom();
                        obj->setAtomProperty(names[i]->atom(), val);
                    }
                    return JO(obj);
                }
                else
                    return kNoOutput;
            }
#endif
        default:
            return kNoOutput;
        }
    }

    void JSONSerializer::Quote(String* value)
    {
        AvmAssert(value != NULL);
        uint32_t i = 0;

        StUTF8String valueUTF8(value);
        const utf8_t* value_src = (const utf8_t*) valueUTF8.c_str();

        uint32_t len = valueUTF8.length();
        uint32_t start;

        char int_conv_buf[8];
        char* ptr;
        int32_t buflen;

        emit('"');

        start = i;
        while (i < len) {
            utf8_t c = value_src[i];
            if (c >= 32 && c != 34 && c != 92) {
                i++;
                continue;
            }

            emit(&value_src[start], i-start);
            i++;
            start = i;

            emit('\\');
            switch (c) {
            case '\"': emit('\"'); break;
            case '\\': emit('\\'); break;
            case '\b': emit('b'); break;
            case '\t': emit('t'); break;
            case '\n': emit('n'); break;
            case '\f': emit('f'); break;
            case '\r': emit('r'); break;
            default: // control character => \u####
                buflen = sizeof(int_conv_buf);
                ptr =
                    MathUtils::
                    convertIntegerToStringBuffer(c+0x10000,
                                                 (char*)int_conv_buf,
                                                 buflen,
                                                 16,
                                                 MathUtils::kTreatAsUnsigned);

                emit('u');
                emit(ptr+1, 4);
                break;
            }
        }

        emit(&value_src[start], i-start);
        emit('\"');
    }

    bool JSONSerializer::hasTransientMetadata(ScriptObject* description)
    {
        Atom metadata =
            description->getAtomProperty(propName("metadata"));
        if (AvmCore::isNullOrUndefined(metadata))
            return false;
        ScriptObject* obj = AvmCore::atomToScriptObject(metadata);
        ArrayObject* array = obj->toArrayObject();
        if (array != NULL) {
            uint32_t len = array->getLength();
            for (uint32_t i=0; i < len; i++) {
                Atom entryAtom = array->getUintProperty(i);
                ScriptObject* entry =
                    AvmCore::atomToScriptObject(entryAtom);
                Atom entryNameAtom =
                    entry->getAtomProperty(propName("name"));
                if (AvmCore::isString(entryNameAtom)) {
                    String* entryNameString =
                        core()->atomToString(entryNameAtom);
                    if (entryNameString->equalsLatin1("Transient"))
                        return true;
                }
            }
        }
        return false;
    }

    ReturnCondition JSONSerializer::JODescriptions(ScriptObject* descs,
                                                   ScriptObject* value,
                                                   String* pendingPre,
                                                   String* connective)
    {
        int32_t index;

        bool emitted = false;

        if (descs != NULL) {

            for (index = descs->nextNameIndex(0);
                 index != 0;
                 index = descs->nextNameIndex(index))
            {
                Atom nextValue = descs->nextValue(index);
                ScriptObject* description =
                    AvmCore::atomToScriptObject(nextValue);
                Atom access =
                    description->getAtomProperty(propName("access"));
                if (core()->atomToString(access)->charAt(0) == 'r') {
                    if (hasTransientMetadata(description))
                        continue;
                    Atom name =
                        description->getAtomProperty(propName("name"));

                    ReturnCondition ret;
                    ret = JOPropMultiname(name, value, pendingPre);
                    RET_EXN_CHECK(ret);
                    if (ret == kSomeOutput) {
                        pendingPre = connective;
                        emitted = true;
                    }
                }
            }
        }
        if (emitted)
            return kSomeOutput;
        else
            return kNoOutput;
    }

    ReturnCondition JSONSerializer::JO(ScriptObject* value)
    {
        if (valueActive(value))
            return kCycleTraversed;

        String* stepback = m_indent;
        m_indent = m_indent->append(m_gap);

        String* pendingPre;
        String* connective;
        String* pendingPost;

        if (m_gap->isEmpty()) {
            pendingPre  = emptyString();   // ""
            connective  = m_str_comma;     // ","
            pendingPost = emptyString();   // ""
        } else {
            pendingPre  = m_str_newline->append(m_indent);
            connective  = m_str_commanewline->append(m_indent);
            pendingPost = m_str_newline->append(stepback);
        }

        emit('{');

        bool emittedSomething = false;

    if (m_proplist != NULL) {
        uint32_t len = m_proplist->getLength();
        for (uint32_t i = 0; i < len; i++) {
            Atom userprop = m_proplist->getUintProperty(i);
            ReturnCondition ret;

            // Cannot always use JOProp, because that would not cover
            // cases where user explicitly listed strings that
            // correspond to AS3 field names.
            //
            // Cannot always use JOPropMultiname, because that would
            // not cover cases where user provided a number instead of
            // a name.
            //
            // So dispatch on type of user provided property name.

            if (AvmCore::isString(userprop)) {
                ret = JOPropMultiname(userprop, value, pendingPre);
                RET_EXN_CHECK(ret);
            } else {
                ret = JOProp(userprop, value, pendingPre);
                RET_EXN_CHECK(ret);
            }

            if (ret == kSomeOutput) {
                pendingPre = connective;
                emittedSomething = true;
            }
        }
    } else {
        // We were not provided with a property list via the
        // replacer parameter; therefore we are obligated by
        // ECMA-262 to build up an internal copy of the own
        // propery names for value.

        // Bug 652117: nextNameIndex and nextName need better
        // documentation, both in headers and in avm2overview.

        int32_t ownDynPropCount = 0;
        int32_t index;

        index = value->nextNameIndex(0);
        while (index != 0) {
            ownDynPropCount++;
            index = value->nextNameIndex(index);
        }

        AutoDestructingAtomArray propNames(m_fixedmalloc, ownDynPropCount);

        index = value->nextNameIndex(0);
        int32_t propNamesIdx = 0;
        while (index != 0) {
            Atom name = value->nextName(index);
            propNames.m_atoms[propNamesIdx] = name;
            propNamesIdx++;
            index = value->nextNameIndex(index);
        }
        AvmAssert(propNamesIdx == ownDynPropCount);

        // In any case, if our object has Traits, then it is an
        // instance of an AS3 class, and we want to enumerate its
        // public non-method properties first.

        // Bug 652116: support for this traversal could be provided by
        // TypeDescriber, rather than allocating the intermediate
        // ScriptObject dictated by TypeDescriber's public interface.

        TypeDescriber typeDescriber(m_toplevel);
        uint32_t flags =
            (TypeDescriber::HIDE_OBJECT |
             TypeDescriber::INCLUDE_VARIABLES |
             TypeDescriber::INCLUDE_ACCESSORS |
             TypeDescriber::INCLUDE_TRAITS |
             TypeDescriber::INCLUDE_BASES |
             TypeDescriber::INCLUDE_METADATA );
        ScriptObject* valueDescription =
            typeDescriber.describeType(value->atom(), flags);

        if (valueDescription != NULL) {
            ScriptObject* traitsDescription =
                AvmCore::atomToScriptObject(
                    valueDescription->getAtomProperty(propName("traits")));

            ReturnCondition ret;

            // loops over descriptions (analogous to DescribeType.as)

            ScriptObject* varsDescriptions =
                AvmCore::atomToScriptObject(
                    traitsDescription->getAtomProperty(propName("variables")));
            ret = JODescriptions(varsDescriptions, value,
                                 pendingPre, connective);
            RET_EXN_CHECK(ret);
            if (ret == kSomeOutput) {
                pendingPre = connective;
                emittedSomething = true;
            }

            ScriptObject* gettersDescriptions =
                AvmCore::atomToScriptObject(
                    traitsDescription->getAtomProperty(propName("accessors")));
            ret = JODescriptions(gettersDescriptions, value,
                                 pendingPre, connective);
            RET_EXN_CHECK(ret);
            if (ret == kSomeOutput) {
                pendingPre = connective;
                emittedSomething = true;
            }
        }

        for (propNamesIdx = 0; propNamesIdx < ownDynPropCount; propNamesIdx++) {

            Atom name = propNames.m_atoms[propNamesIdx];

            // Is the getAtomPropertyIsEnumerable test necessary
            // (and if it is, is there potential for changing how
            // object nextNameIndex works)?  And what really is
            // the contract of getAtomPropertyIsEnumerable;
            // e.g. if it requires a string, does that imply that
            // non-string indices are always enumerable?
            if (!AvmCore::isString(name) ||
                value->getAtomPropertyIsEnumerable(name)) {
                ReturnCondition ret;
                ret = JOProp(name, value, pendingPre);
                RET_EXN_CHECK(ret);
                if (ret == kSomeOutput) {
                    pendingPre = connective;
                    emittedSomething = true;
                }
            }
        }
    }

        if (emittedSomething)
            emit(pendingPost);

        emit('}');

        valueNotActive(value);

        m_indent = stepback;

        return kSomeOutput;
    }

    ReturnCondition JSONSerializer::JOProp(Atom p,
                                           ScriptObject* value,
                                           String* pendingPrefix)
    {
        String* pName;
        ReturnCondition ret;
        if (AvmCore::isString(p)) {
            pName = core()->atomToString(p);
            ret = Str(pName, value, pendingPrefix, true);
            RET_EXN_CHECK(ret);
        } else if (atomIsIntptr(p) && atomCanBeUint32(p)) {
            uint32_t Puint = uint32_t(atomGetIntptr(p));
            pName = core()->uintToString(Puint);
            ret = Str(pName, value, pendingPrefix, true);
            RET_EXN_CHECK(ret);
        } else {
            // This case would arise if we inadvertantly attempt to loop
            // over the properties of a Dictionary object.  Only sane
            // options would be to throw an exception, convert to a
            // string, or discard by treated as an undefined result.
            // I am choosing the latter and returning kNoOutput.
            // (Current plan is to put an sophisticated handling of a
            //  Dictionary into its toJSON method, so users should
            //  not encounter this anyway.)
            // To test this case, Felix is currently guessing that the
            // best option is to remove Dictionary's toJSON method.
            ret = kNoOutput;
        }

        return ret;
    }

    ReturnCondition JSONSerializer::JOPropMultiname(Atom p,
                                                    ScriptObject* value,
                                                    String* pendingPrefix)
    {
        AvmAssert(AvmCore::isString(p));
        Multiname name;
        name.setNamespace(core()->findPublicNamespace());
        name.setName(core()->intern(p));
        ReturnCondition ret;
        ret = Str(&name, value, pendingPrefix, true);
        RET_EXN_CHECK(ret);
        return ret;
    }

    ReturnCondition JSONSerializer::JAarray(ArrayObject* value)
    {
        uint32_t len = value->getLength();
        return JAfinish(value, len);
    }

    ReturnCondition JSONSerializer::JAvector(VectorBaseObject* value)
    {
        uint32_t len = value->getLength();
        return JAfinish(value, len);
    }

    ReturnCondition JSONSerializer::JAfinish(ScriptObject* value, uint32_t len)
    {
        if (valueActive(value))
            return kCycleTraversed;

        String* stepback = m_indent;
        m_indent = m_indent->append(m_gap);

        String* pendingPre;
        String* connective;
        String* pendingPost;

        if (m_gap->isEmpty()) {
            pendingPre = emptyString();       // ""
            connective = m_str_comma;         // ","
            pendingPost = emptyString();      // ""
        } else {
            pendingPre  = m_str_newline->append(m_indent);
            connective  = m_str_commanewline->append(m_indent);
            pendingPost = m_str_newline->append(stepback);
        }

        emit('[');

        bool emitted = false;
        for (uint32_t index = 0; index < len; index++) {
            ReturnCondition justEmitted = Str(index, value, pendingPre, false);
            switch (justEmitted) {
            case kSomeOutput:
                pendingPre = connective;
                emitted = true;
                break;
            case kNoOutput:
                // Did not emit in the Str invocation above, but will
                // emit now; ensure pending string emitted first.
                emit(pendingPre);
                emit("null", 4);
                pendingPre = connective;
                emitted = true;
                break;
            case kCycleTraversed:
            case kThrewException:
                // percolate error cases up.
                return justEmitted;
            }
        }
        if (emitted)
            emit(pendingPost);

        emit(']');

        valueNotActive(value);

        m_indent = stepback;

        return kSomeOutput;
    }
}
