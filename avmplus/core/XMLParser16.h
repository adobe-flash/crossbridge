/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_XMLParser16__
#define __avmplus_XMLParser16__

namespace avmplus
{
    /**
     * XMLTag represents a single XML tag.  It is output by the
     * XMLParser class, the XML parser.
     */
    class XMLTag
    {
    public:
        XMLTag(MMgc::GC *gc) : attributes(gc, 0)
        {
            reset();
        }

        void reset()
        {
            text = NULL;
            nodeType = kNoType;
            empty = false;
            attributes.clear();
        }

        enum TagType {
            kNoType                = 0,
            // These match the W3C XML DOM SPEC
            // http://www.w3.org/TR/2000/REC-DOM-Level-2-Core-20001113/core.html
            // (Search for ELEMENT_NODE)
            kElementType           = 1,
            kTextNodeType          = 3,
            kCDataSection          = 4,
            kProcessingInstruction = 7,
            kComment               = 8,
            kDocTypeDeclaration    = 10,

            // This does not match the W3C XML DOM SPEC
            kXMLDeclaration        = 13
        };

        Stringp text;
        enum TagType nodeType;
        bool empty;
        RCList<String> attributes;

        /**
         * nextAttribute is used to iterate over the
         * attributes of a XML tag
         */
        bool nextAttribute(uint32_t& index,
                           Stringp& name,
                           Stringp& value);
    };

    /**
     * XMLParser is a XML parser which takes 16-bit wide characters
     * as input.  The parser operates in "pull" fashion, returning a
     * single tag or text node on each call to the GetNext method.
     *
     * This XML parser is used to support E4X in AVM+.
     */
    class XMLParser
    {
    public:
        XMLParser(AvmCore *core, Stringp str);
        ~XMLParser()
        {
            core = NULL;
            m_pos = 0;
            m_ignoreWhite = false;
            m_condenseWhite = false;
        }

        void parse(bool ignoreWhite = false);

        int getNext(XMLTag& tag);

        enum {
            kNoError                           = 0,
            kEndOfDocument                     = -1,
            kUnterminatedCDataSection          = -2,
            kUnterminatedXMLDeclaration        = -3,
            kUnterminatedDocTypeDeclaration    = -4,
            kUnterminatedComment               = -5,
            kMalformedElement                  = -6,
            kOutOfMemory                       = -7,
            kUnterminatedAttributeValue        = -8,
            kUnterminatedElement               = -9,
            kElementNeverBegun                 = -10,
            kUnterminatedProcessingInstruction = -11
        };

                AvmCore*        core;

        inline  bool            getCondenseWhite() const { return m_condenseWhite; }
        inline  void            setCondenseWhite(bool flag) { m_condenseWhite = flag; }

    private:
        inline  bool            atEnd() const { return (m_pos >= m_str->length()); }
        // get a substring, and remove &xx; entities
                Stringp         unescape (int32_t start, int32_t end, bool bIntern);
        // skip a prefix, return true if skipped
                bool            skipPrefix (int32_t pos, const char* prefix, int32_t len);
        // skip white space, return false if at end
                bool            skipWhiteSpace();

                StringIndexer   m_str;
                int32_t         m_pos;
                bool            m_ignoreWhite;
                bool            m_condenseWhite;
    };
}

#endif
