/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

#include "pcre.h"

// todo figure out what to do about all the new/delete in here
// todo general clean-up

namespace avmplus
{
    using namespace MMgc;


    class PcreState {
    public:
        PcreState(Toplevel* t) { AvmCore::setPCREContext(t); }
        ~PcreState() { AvmCore::setPCREContext(NULL); }
    };

#define PCRE_STATE(x) PcreState p(x)

#define OVECTOR_SIZE 99 // 32 matches = (32+1)*3

    CompiledRegExp::~CompiledRegExp()
    {
        // NOTE: we do not set the PCRE_STATE here, because we don't have a toplevel
        // that we can use to pass to AvmCore::setPCREContext.  It's OK: pcre_free
        // just frees a char[], it does not go deep like the compilation step does.

        (pcre_free)((void*)(pcre*)regex);
        regex = NULL;
    }

    RegExpObject::RegExpObject(VTable* ivtable, ScriptObject *objectPrototype)
        : ScriptObject(ivtable, objectPrototype)
        , m_source(core()->kEmptyString)
        , m_lastIndex(0)
        , m_optionFlags(PCRE_UTF8)
        , m_global(false)
        , m_hasNamedGroups(false)
    {
        completeInitialization(core()->kEmptyString);
    }

    // This variant is used for "new RegExp( re )" where "re" is a RegExp - we copy it.

    RegExpObject::RegExpObject(RegExpObject *toCopy)
        : ScriptObject(toCopy->vtable, toCopy->getDelegate())
        , m_source(toCopy->m_source)
        , m_pcreInst(toCopy->m_pcreInst)
        , m_lastIndex(0)
        , m_optionFlags(toCopy->m_optionFlags)
        , m_global(toCopy->m_global)
        , m_hasNamedGroups(toCopy->m_hasNamedGroups)
    {
    }

    // This variant is used for "new RegExp(s)" and "new RegExp(s,f)" where s is not
    // a RegExp object.  In the one-argument case "s" may have a trailing flags string.

    RegExpObject::RegExpObject(VTable* ivtable, ScriptObject* delegate, Stringp pattern, Stringp options)
        : ScriptObject(ivtable, delegate)
        , m_source(pattern)
        , m_lastIndex(0)
        , m_optionFlags(PCRE_UTF8)
        , m_global(false)
        , m_hasNamedGroups(false)
    {
        // Skip the precompilation if the pattern and options hit the cache.

        if (!core()->m_regexCache.testCachedRegex(pattern, options))
        {
            // Precompilation: check for named groups, and split out the options if no options were
            // passed in, as for new RegExp( regex.toString() ).

            int32_t numSlashSeen = 0;
            int32_t pos = 0;
            int32_t length = pattern->length();
            int32_t optionpos = 0;

            // Scan the pattern, look for options.

            while (pos < length)
            {
                wchar c = pattern->charAt(pos);
                if (c == 0)
                    break;
                if (c == '(' &&
                    pos+3 < length &&
                    pattern->charAt(pos+1) == '?' &&
                    pattern->charAt(pos+2) == 'P' &&
                    pattern->charAt(pos+3) == '<')
                {
                    m_hasNamedGroups = true;
                }
                else if (options == NULL && c == '/' && (pos == 0 || pattern->charAt(pos-1) != '\\') && numSlashSeen++ > 0)
                {
                    options = pattern;
                    optionpos = pos+1;
                }
                pos++;
            }

            // Scan the options.

            if (options != NULL)
            {
                pos = optionpos;
                length = options->length();
                while (pos < length)
                {
                    switch(options->charAt(pos))
                    {
                    case 'g':
                        m_global = true;
                        break;
                    case 'i':
                        m_optionFlags |= PCRE_CASELESS;
                        break;
                    case 'm':
                        m_optionFlags |= PCRE_MULTILINE;
                        break;
                    case 's':
                        m_optionFlags |= PCRE_DOTALL;
                        break;
                    case 'x':
                        m_optionFlags |= PCRE_EXTENDED;
                        break;
                    }
                    pos++;
                }
            }
        }

        completeInitialization(options);
    }

#ifdef DRC_TRIVIAL_DESTRUCTOR
    RegExpObject::~RegExpObject()
    {
        m_global = false;
        m_lastIndex = 0;
        m_optionFlags = 0;
        m_hasNamedGroups = false;
    }
#endif

    // Note 'options' may be the same as m_source, and the options substring
    // will then start in the middle somewhere.  So don't go inspecting that
    // string too closely, it's used for the cache.
    //
    // If it is known that m_source and options hit the cache then only m_source
    // need have the correct value, all others will be loaded from the cache.
    // Otherwise, all fields apart from m_pcreInst must have correct values.

    void RegExpObject::completeInitialization(String* options)
    {
        AvmAssert(traits()->getSizeOfInstance() == sizeof(RegExpObject));

        bool found = false;
        RegexCacheEntry& r = core()->m_regexCache.findCachedRegex(found, m_source, options);

        if (found)
        {
            m_global = r.global;
            m_optionFlags = r.optionFlags;
            m_hasNamedGroups = r.hasNamedGroups;
            m_pcreInst = r.regex;
        }
        else
        {
            PCRE_STATE(toplevel());

            int errptr;
            const char *error;
            StUTF8String patternz(m_source);
            void* pcreInst = (void*)pcre_compile(patternz.c_str(), m_optionFlags, &error, &errptr, NULL);
            CompiledRegExp* regex = new (gc()) CompiledRegExp(pcreInst);

            if (!core()->m_regexCache.disabled())
            {
                r.pattern = m_source;
                r.options = options;
                r.global = m_global;
                r.optionFlags = m_optionFlags;
                r.hasNamedGroups = m_hasNamedGroups;
                r.regex = regex;
            }

            m_pcreInst = regex;
        }
    }

    // this = argv[0]
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom RegExpObject::call(int argc, Atom *argv)
    {
        // this call occurs when a regexp object is invoked directly as a function ala "/a|b/('dcab')"
        AvmCore *core = this->core();
        Atom inAtom = argc ? core->string(argv[1])->atom() : core->kEmptyString->atom();
        return AS3_exec(core->atomToString(inAtom));
    }

    /**
     * read a string.  No resource table stuff here, caller must take care of it.
     * @param code
     * @return
     */
    Atom RegExpObject::stringFromUTF8(const char* buffer, int len)
    {
        AvmAssert(len >= 0);
        return core()->newStringUTF8(buffer, len)->atom();
    }

    int RegExpObject::search(Stringp subject)
    {
        int matchIndex, matchLen;
        StIndexableUTF8String utf8Subject(subject);
        if (!_exec(subject, utf8Subject, 0, matchIndex, matchLen))
        {
            matchIndex = -1;
        }
        else
        {
            matchIndex = utf8Subject.toIndex(matchIndex);
        }
        return matchIndex;
    }

    int RegExpObject::numBytesInUtf8Character(const uint8_t *in)
    {
        unsigned int c = *in;
        switch(c>>4)
        {
            case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
            default:
                // 0xxx xxxx
                return 1;
            case 12: case 13:
                // 110xxxxx   10xxxxxx
                return 2;
            case 14:
                // 1110xxxx  10xxxxxx  10xxxxxx
                return 3;
            case 15:
                // 11110xxx  10xxxxxx  10xxxxxx  10xxxxxx
                return 4;
        }
    }


    ArrayObject* RegExpObject::split(Stringp subject, uint32_t limit)
    {
        ArrayObject *out = toplevel()->arrayClass()->newArray();
        StIndexableUTF8String utf8Subject(subject);

        int startIndex=0;
        int matchIndex;
        int matchLen;
        ArrayObject* matchArray;
        unsigned n=0;
        bool isEmptyRE = m_source->length() == 0;
        while ((matchArray = _exec(subject,
                                  utf8Subject,
                                  startIndex,
                                  matchIndex,
                                  matchLen)) != NULL)
        {
            // [cn 11/22/04] when match is made, but is length 0 we've matched the empty
            //  position between characters.  Although we've "matched", its zero length so just break out.
            if (matchLen == 0 ) {
                matchLen = 0;
                matchIndex = startIndex+numBytesInUtf8Character((uint8_t*)(utf8Subject.c_str())+startIndex); // +1char  will advance startIndex, extract just one char
                if( !isEmptyRE )
                {
                    // don't break if we're processing an empty regex - then we want to split the string into each character
                    // so we want the loop to continue
                    break;
                }
            }

            //[ed 8/10/04] don't go past end of string. not sure why pcre doesn't return null
            //for a match starting past the end.
            //[cn 12/3/04] because a regular expression which matches an empty position (space between characters)
            //  will match the empty position just past the last character.  This test is correct, though
            //  it needs to come before we do any setProperties to avoid a bogus xtra result.
            if (matchIndex+matchLen > utf8Subject.length()) {
                startIndex = matchIndex+matchLen;
                break;
            } else {
                out->setUintProperty(n++, stringFromUTF8(utf8Subject.c_str()+startIndex, matchIndex-startIndex));
                if (n >= limit)
                    break;
                for (uint32_t j=1; j<matchArray->getLength(); j++) {
                    out->setUintProperty(n++, matchArray->getUintProperty(j));
                    if (n >= limit)
                        break;
                }
                // Advance past this match
                startIndex = matchIndex+matchLen;
            }
        }

        // If we found no match, or we did find a match and are still under limit, and there is a remainder left, add it
        if ((unsigned)n < limit && startIndex <= utf8Subject.length()) {
            out->setUintProperty(n++, stringFromUTF8(utf8Subject.c_str()+startIndex, utf8Subject.length()-startIndex));
        }

        return out;
    }

    Atom RegExpObject::AS3_exec(Stringp subject)
    {
        if (!subject)
        {
            subject = core()->knull;
        }
        StIndexableUTF8String utf8Subject(subject);
        ArrayObject *result = _exec(subject, utf8Subject);
        return result ? result->atom() : nullStringAtom;
    }

    ArrayObject* RegExpObject::_exec(Stringp subject, StIndexableUTF8String& utf8Subject)
    {
        AvmAssert(subject != NULL);

        int matchIndex = 0, matchLen = 0;
        int startIndex = (get_global() ? utf8Subject.toUtf8Index(m_lastIndex) : 0);

        ArrayObject* result = _exec(subject,
                                   utf8Subject,
                                   startIndex,
                                   matchIndex,
                                   matchLen);
        if (get_global())
        {
            m_lastIndex = utf8Subject.toIndex(matchIndex+matchLen);
        }

        return result;
    }

    ArrayObject* RegExpObject::_exec(Stringp subject,
                                    StIndexableUTF8String& utf8Subject,
                                    int startIndex,
                                    int& matchIndex,
                                    int& matchLen)
    {
        AvmAssert(subject != NULL);

        int ovector[OVECTOR_SIZE];
        int results;
        int subjectLength = utf8Subject.length();

        PCRE_STATE(toplevel());
        if( startIndex < 0 ||
            startIndex > subjectLength ||
            (results = pcre_exec((pcre*)(m_pcreInst->regex),
                                NULL,
                                utf8Subject.c_str(),
                                subjectLength,
                                startIndex,
                                PCRE_NO_UTF8_CHECK,
                                ovector,
                                OVECTOR_SIZE)) < 0)
        {
            matchIndex = 0;
            matchLen = 0;
            return NULL;
        }

        AvmCore *core = this->core();
        ArrayObject *a = toplevel()->arrayClass()->newArray(results);

        a->setAtomProperty(core->kindex->atom(),
               core->intToAtom(utf8Subject.toIndex(ovector[0])));
        a->setAtomProperty(core->kinput->atom(),
               subject->atom());
        a->setLength(results);

        // set array slots
        for (int i=0; i<results; i++) {
            if (ovector[i*2] > -1) {
                int length = ovector[i*2 + 1] - ovector[i*2];
                Atom match = stringFromUTF8(utf8Subject.c_str()+ovector[i*2], length);
                a->setUintProperty(i, match);
            } else {
                a->setUintProperty(i, undefinedAtom);
            }
        }

        // handle named groups
        if (m_hasNamedGroups)
        {
            int entrySize;
            pcre_fullinfo((pcre*)(m_pcreInst->regex), NULL, PCRE_INFO_NAMEENTRYSIZE, &entrySize);

            int nameCount;
            pcre_fullinfo((pcre*)(m_pcreInst->regex), NULL, PCRE_INFO_NAMECOUNT, &nameCount);

            // this space is freed when (pcre*)m_pcreInst is freed
            char *nameTable;
            pcre_fullinfo((pcre*)(m_pcreInst->regex), NULL, PCRE_INFO_NAMETABLE, &nameTable);

            /* nameTable is a series of fixed length entries (entrySize)
               the first two bytes are the index into the ovector and the result
               is a null terminated string (the subgroup name) */
            for (int i = 0; i < nameCount; i++)
            {
                int nameIndex, length;
                nameIndex = (nameTable[0] << 8) + nameTable[1];
                length = ovector[nameIndex * 2 + 1] - ovector[ nameIndex * 2 ];

                Atom name = stringFromUTF8((nameTable+2), (uint32_t)VMPI_strlen(nameTable+2));
                name = core->internString(name)->atom();

                Atom value = stringFromUTF8(utf8Subject.c_str()+ovector[nameIndex*2], length);

                a->setAtomProperty(name, value);

                nameTable += entrySize;
            }
        }

        matchIndex = ovector[0];
        matchLen = ovector[1]-ovector[0];

        return a;
    }

    ArrayObject* RegExpObject::match(Stringp subject)
    {
        StIndexableUTF8String utf8Subject(subject);
        if (!get_global())
        {
            return _exec(subject, utf8Subject);
        }
        else
        {
            ArrayObject *a = toplevel()->arrayClass()->newArray();

            int oldLastIndex = m_lastIndex;
            m_lastIndex = 0;

            int n = 0;

            ArrayObject* matchArray;
            while (true)
            {
                int last = m_lastIndex;
                int matchIndex = 0, matchLen = 0;
                int startIndex = utf8Subject.toUtf8Index(m_lastIndex);

                matchArray = _exec(subject,
                                  utf8Subject,
                                  startIndex,
                                  matchIndex,
                                  matchLen);
                m_lastIndex = utf8Subject.toIndex(matchIndex+matchLen);

                if ((matchArray == NULL) || (last == m_lastIndex))
                    break;
                a->setUintProperty(n++, matchArray->getUintProperty(0));
            }

            if (m_lastIndex == oldLastIndex)
            {
                m_lastIndex++;
            }

            return a;
        }
    }

#define NUM_MATCHES 100

    Atom RegExpObject::replace(Stringp subject,
                               Stringp replacement)
    {
        StUTF8String utf8Subject(subject);
        StUTF8String utf8Replacement(replacement);

        int ovector[OVECTOR_SIZE];
        int subjectLength = utf8Subject.length();
        int lastIndex=0;

        StringBuffer resultBuffer(core());

        const char *src = utf8Subject.c_str();

        PCRE_STATE(toplevel());

        // get start/end index of all matches
        int matchCount;
        while (lastIndex <= subjectLength &&
               (matchCount = pcre_exec((pcre*)(m_pcreInst->regex), NULL, (const char*)src,
               subjectLength, lastIndex, PCRE_NO_UTF8_CHECK, ovector, OVECTOR_SIZE)) > 0)
        {
            int captureCount = matchCount-1;

            int matchIndex = ovector[0];
            int matchLen   = ovector[1]-ovector[0];

            // copy in stuff leading up to match
            resultBuffer.writeN(src+lastIndex, matchIndex-lastIndex);

            const char *ptr = utf8Replacement.c_str();
            while (*ptr) {
                if (*ptr == '$') {
                    switch (*(ptr+1)) {
                    case '$':
                        resultBuffer << '$';
                        ptr += 2;
                        break;
                    case '&':
                        resultBuffer.writeN(src+matchIndex, matchLen);
                        ptr += 2;
                        break;
                    case '`':
                        resultBuffer.writeN(src, matchIndex);
                        ptr += 2;
                        break;
                    case '\'':
                        resultBuffer << src+ovector[1];
                        ptr += 2;
                        break;
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
                        {
                            int i;
                            if (*(ptr+2) >= '0' && *(ptr+2) <= '9') {
                                int j = 10*(ptr[1]-'0')+(ptr[2]-'0');
                                if (j > captureCount) {
                                    // Gobbling up two digits would overflow the
                                    // capture count, so just use the one digit.
                                    i = ptr[1]-'0';
                                } else {
                                    i = j;
                                }
                            } else {
                                i = ptr[1]-'0';
                            }
                            if (i >= 1 && i <= captureCount) {
                                resultBuffer.writeN(src+ovector[i*2],
                                                   ovector[i*2+1]-ovector[i*2]);
                                ptr += (i >= 10) ? 3 : 2;
                            } else {
                                resultBuffer << *ptr++;
                            }
                        }
                        break;
                    default:
                        resultBuffer << *ptr++;
                        break;
                    }
                } else {
                    resultBuffer << *ptr++;
                }
            }

            int newLastIndex = ovector[0] + (ovector[1] - ovector[0]);

            // prevents infinite looping in certain cases
            fixReplaceLastIndex(src,
                                subjectLength,
                                matchLen,
                                newLastIndex,
                                resultBuffer);

            lastIndex = newLastIndex;

            if (!get_global())
            {
                break;
            }
        }

        // copy in stuff after last match
        if (lastIndex < subjectLength)
        {
            resultBuffer.writeN(src+lastIndex, subjectLength-lastIndex);
        }

        return stringFromUTF8((const char*)resultBuffer.c_str(), resultBuffer.length());
    }

    Atom RegExpObject::replace(Stringp subject,
                               ScriptObject* replaceFunction)
    {
        StUTF8String utf8Subject(subject);

        int ovector[OVECTOR_SIZE];
        int subjectLength = utf8Subject.length();
        int lastIndex=0;

        StringBuffer resultBuffer(core());

        const char *src = utf8Subject.c_str();

        PCRE_STATE(toplevel());

        // get start/end index of all matches
        int matchCount;
        while (lastIndex < subjectLength &&
               (matchCount = pcre_exec((pcre*)(m_pcreInst->regex), NULL, (const char*)src,
                         subjectLength, lastIndex, PCRE_NO_UTF8_CHECK, ovector, OVECTOR_SIZE)) > 0)
        {
            int captureCount = matchCount-1;

            int matchIndex = ovector[0];
            int matchLen   = ovector[1]-ovector[0];

            // copy in stuff leading up to match
            resultBuffer.writeN(src+lastIndex, matchIndex-lastIndex);

            // call the replace function
            Atom argv[NUM_MATCHES+4];
            int argc = captureCount+3;

            argv[0] = undefinedAtom;

            // ECMA 15.5.4.11: Argument 1 is the substring that matched.
            argv[1] = stringFromUTF8(src+matchIndex, matchLen);

            // ECMA 15.5.4.11: The next m arguments are all of the captures in the
            // MatchResult
            for (int i=1; i<=captureCount; i++)
            {
                argv[i+1] = stringFromUTF8(src+ovector[i*2], ovector[i*2+1]-ovector[i*2]);
            }

            // ECMA 15.5.4.11: Argument m+2 is the offset within string
            // where the match occurred
            argv[captureCount+2] = core()->uintToAtom(matchIndex);

            // ECMA 15.5.4.11: Argument m+3 is string
            argv[captureCount+3] = subject->atom();

            resultBuffer << core()->string(avmplus::op_call(toplevel(), replaceFunction, argc, argv));

            int newLastIndex = ovector[0] + (ovector[1] - ovector[0]);

            // prevents infinite looping in certain cases
            fixReplaceLastIndex(src,
                                subjectLength,
                                matchLen,
                                newLastIndex,
                                resultBuffer);

            lastIndex = newLastIndex;

            if (!get_global())
                break;
        }

        // copy in stuff after last match
        if (lastIndex < subjectLength)
        {
            resultBuffer.writeN(src+lastIndex, subjectLength-lastIndex);
        }

        return stringFromUTF8((const char*)resultBuffer.c_str(), resultBuffer.length());
    }

    void RegExpObject::fixReplaceLastIndex(const char *src,
                                           int subjectLength,
                                           int matchLen,
                                           int& newLastIndex,
                                           StringBuffer& resultBuffer)
    {
        if (matchLen == 0 && get_global())
        {
            // Advance one character
            if (newLastIndex < subjectLength)
            {
                uint32_t ch;
                int n = UnicodeUtils::Utf8ToUcs4((const uint8_t*)src+newLastIndex, subjectLength-newLastIndex, &ch, core()->currentBugCompatibility()->bugzilla609416);
                if (n <= 0)
                {
                    // Invalid UTF8 sequence, advance one uint8_t
                    n = 1;
                }
                resultBuffer.writeN(src+newLastIndex, n);
                newLastIndex += n;
            }
            else
            {
                newLastIndex++;
            }
        }
    }

    //
    // Accessors
    //

    bool RegExpObject::get_ignoreCase() { return hasOption(PCRE_CASELESS); }
    bool RegExpObject::get_multiline() { return hasOption(PCRE_MULTILINE); }
    bool RegExpObject::get_dotall() { return hasOption(PCRE_DOTALL); }
    bool RegExpObject::get_extended() { return hasOption(PCRE_EXTENDED); }

}
