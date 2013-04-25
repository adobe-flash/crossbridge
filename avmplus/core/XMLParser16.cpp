/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
    //
    // XMLParser
    //

    bool XMLParser::skipWhiteSpace()
    {
        bool eof = atEnd();
        while (!eof)
        {
            wchar ch = m_str[m_pos];
            if (!String::isSpace(ch))
                break;
            m_pos++;
            eof = atEnd();
        }
        return !eof;
    }

    // !!@ I'm not sure what this was supposed to do originally but I've rewritten it
    // to remove the leading and trailing white space for text elements.
    // "     5     4     3     " becomes "5     4     3"
    // This is to simulate the E4X XML parser
    static Stringp _condenseWhitespace(Stringp text)
    {
        StringIndexer str(text);
        // leading space
        int32_t start = 0;
        while (start < text->length() && String::isSpace(str[start]))
            start++;
        // trailing space
        int32_t end = text->length() - 1;
        while ((end > start) && String::isSpace(str[end]))
            end--;
        return text->substring(start, end + 1);
    }

    int XMLParser::getNext(XMLTag& tag)
    {
        wchar ch;
        int32_t start, end;
        tag.reset();

        // If there's nothing left, exit.
        if (atEnd()) {
            return XMLParser::kEndOfDocument;
        }

        // R41
        // If the ignore whitespace flag is on, don't produce
        // all-whitespace text nodes.
        if (m_ignoreWhite)
        {
            if (!skipWhiteSpace())
                return XMLParser::kEndOfDocument;
        }
        // end R41

        // If it starts with <, it's an XML element.
        // If it doesn't, it must be a text element.
        start = m_pos;
        ch = m_str[m_pos];
        if (ch != '<')
        {
            // Treat it as text.  Scan up to the next < or until EOF.
            m_pos = m_str->indexOfCharCode('<', m_pos + 1);
            if (m_pos < 0)
                m_pos = m_str->length();

            tag.text = unescape(start, m_pos, false);

            // Condense whitespace if desired
            if (m_ignoreWhite && m_condenseWhite)
                tag.text = _condenseWhitespace(tag.text);

            tag.nodeType = XMLTag::kTextNodeType;
            return XMLParser::kNoError;
        }

        // Is this a <?xml> declaration?
        start = m_pos;
        if (m_str->matchesLatin1_caseless("<?xml ", 6, start))
        {
            end = m_str->indexOfLatin1("?>", 2, start + 6);
            if (end >= 0)
            {
                // We have the end of the XML declaration
                // !!@ changed to not return <?...?> parts
                tag.text = m_str->substring(start + 2, end);
                m_pos = end + 2;
                tag.nodeType = XMLTag::kXMLDeclaration;
                return XMLParser::kNoError;
            }
            return XMLParser::kUnterminatedXMLDeclaration;
        }

        // Is this a <!DOCTYPE> declaration?
        if (m_str->matchesLatin1_caseless("<!DOCTYPE", 8, start))
        {
            // Scan forward for '>', but check for embedded <>
            int32_t depth = 0;
            end = start + 1;
            while (!atEnd())
            {
                ch = m_str[end++];
                if (ch == '<')
                    depth++;
                else if (ch == '>')
                {
                    if (!depth)
                    {
                        // We've reached the end of the DOCTYPE.
                        tag.text = m_str->substring(start, end);
                        tag.nodeType = XMLTag::kDocTypeDeclaration;
                        m_pos = end;
                        return XMLParser::kNoError;
                    }
                    depth--;
                }
                m_pos = end;
            }
            return XMLParser::kUnterminatedDocTypeDeclaration;
        }

        // Is this a CDATA section?
        if (m_str->matchesLatin1_caseless("<![CDATA[", 9, start))
        {
            start += 9;
            end = m_str->indexOfLatin1("]]>", 3, start);
            if (end >= 0)
            {
                // We have the end of the CDATA section.
                tag.text = m_str->substring(start, end);
                tag.nodeType = XMLTag::kCDataSection;
                m_pos = end + 3;
                return XMLParser::kNoError;
            }
            return XMLParser::kUnterminatedCDataSection;
        }

        // Is this a processing instruction?
        if (m_str->matchesLatin1("<?", 2, start))
        {
            // Scan forward for "?>"
            start += 2;
            end = m_str->indexOfLatin1("?>", 2, start);
            if (end >= 0)
            {
                // We have the end of the processing instruction.
                tag.text = m_str->substring(start, end);
                tag.nodeType = XMLTag::kProcessingInstruction;
                m_pos = end + 2;
                return XMLParser::kNoError;
            }
            return XMLParser::kUnterminatedProcessingInstruction;
        }

        // Advance past the "<"
        start = ++m_pos;

        // Is this a comment?  Return a comment tag->
        if (m_str->matchesLatin1("!--", 3, start))
        {
            // Skip up to '-->'.
            start += 3;
            end = m_str->indexOfLatin1("-->", 3, start);
            if (end >= 0)
            {
                tag.text = m_str->substring(start, end);
                tag.nodeType = XMLTag::kComment;
                m_pos = end  + 3;
                return XMLParser::kNoError;
            }
            // Got to the end of the buffer without finding a new tag->
            return XMLParser::kUnterminatedComment;
        }

        // Extract the tag name.  Scan up to ">" or whitespace.
        start = m_pos;
        while (!atEnd())
        {
            ch = m_str[m_pos];
            if (ch == '>' || String::isSpace(ch))
                break;
            if (ch == '/' && (m_pos < m_str->length() - 1) && m_str[m_pos+1] == '>')
            {
                // Found close of an empty element.
                // Exit!
                break;
            }
            m_pos++;
        }
        if (atEnd() || m_pos == start)
            // Premature end, or empty tag name
            return XMLParser::kMalformedElement;

        tag.text = unescape(start, m_pos, true);
        tag.nodeType = XMLTag::kElementType;

        // Extract attributes.
        for (;;)
        {
            if (!skipWhiteSpace())
                // Premature end!
                return XMLParser::kMalformedElement;

            ch = m_str[m_pos];
            if (ch == '>')
                break;

            if (ch == '/' && (m_pos < m_str->length() - 1) && m_str[m_pos+1] == '>')
            {
                // Found close of an empty element.
                // Exit!
                tag.empty = true;
                ch = m_str[++m_pos];
                break;
            }

            // Extract the attribute name.
            start = m_pos;
            while (!String::isSpace(ch) && ch != '=' && ch != '>')
            {
                m_pos++;
                if (atEnd())
                    // Premature end!
                    return XMLParser::kMalformedElement;
                ch = m_str[m_pos];
            }
            if (start == m_pos)
                // Empty attribute name?
                return XMLParser::kMalformedElement;

            Stringp attributeName = unescape(start, m_pos, true);

            if (!skipWhiteSpace())
                // No attribute value, error!
                return XMLParser::kMalformedElement;

            ch = m_str[m_pos++];
            if (ch != '=')
                // No attribute value, error!
                return XMLParser::kMalformedElement;

            if (!skipWhiteSpace())
                // No attribute value, error!
                return XMLParser::kMalformedElement;

            wchar delimiter = m_str[m_pos++];
            // Extract the attribute value.
            if (delimiter != '"' && delimiter != '\'')
                // Error; no opening quote for attribute value.
                return XMLParser::kMalformedElement;

            // Extract up to the next quote.
            start = m_pos;
            ch = 0;
            while (ch != delimiter)
            {
                if (atEnd() || ch == '<')
                    // '<' is not permitted in an attribute value
                    // Changed this from kMalformedElement to kUnterminatedAttributeValue for bug 117058(105422)
                    return XMLParser::kUnterminatedAttributeValue;
                ch = m_str[m_pos++];
            }

            Stringp attributeValue = unescape(start, m_pos - 1, false);

            AvmAssert(attributeName->isInterned());
            tag.attributes.add(attributeName);
            tag.attributes.add(attributeValue);
        }

        // Advance past the end > of this element.
        if (ch == '>')
            m_pos++;

        return XMLParser::kNoError;
    }

    Stringp XMLParser::unescape(int32_t start, int32_t last, bool intern)
    {
        Stringp dest = core->kEmptyString;

        if (start == last)
            return dest;

        int32_t bgn = m_str->indexOfCharCode('&', start, last);
        if (bgn < 0)
        {
            return intern ?
                    core->internSubstring(m_str.str(), start, last) :
                    m_str->substring(start, last);
        }

        int32_t end = start;
        while (bgn >= start && bgn < last)
        {
            int32_t ampBgn = bgn;
            int32_t ampEnd = m_str->indexOfCharCode(';', ++bgn, last);
            if (ampEnd < 0)
                // &xxx without semicolon - we are done
                break;
            // add the text between the last sequence and this sequence
            dest = dest->append(m_str->substring(end, bgn-1));
            end = ampEnd;
            int32_t len = end - bgn;
            // an &xx; sequence is at least two characters
            bool ok = true;
            if (len >= 2)
            {
                int32_t ch = m_str[bgn];
                if (ch == '#')
                {
                    // Parse a &#xx; decimal sequence.  Or a &#xDD hex sequence
                    ch = m_str[++bgn];
                    len--;
                    int base = 10;
                    if (len >= 2 && ch == 'x')
                        base = 16, bgn++, len--;
                    if (len > 0)
                    {
                        int32_t value = 0;
                        while (len-- && ok)
                        {
                            ch = m_str[bgn++];
                            if (ch >= 'A' && ch <= 'F')
                                ch -= 7;
                            else if (ch >= 'a' && ch <= 'f')
                                ch -= ('a' - 'A' + 7);
                            ch -= '0';
                            if (ch >= 0 && ch < base)
                                value = (value * base) + ch;
                            else
                                ok = false;
                            if (value > 0xFFFF)
                                ok = false;
                        }
                        if (ok)
                        {
                            wchar c = (wchar) value;
                            // note: this code is allowed to construct a string
                            // containing illegal UTF16 sequences!
                            dest = dest->append16(&c, 1);
                            bgn = ++end;
                        }
                    }
                }
                else if (len <= 4) // Our xmlEntities are only 4 characters or less
                {
                    Atom entityAtom = core->internSubstring(m_str.str(), bgn, end)->atom();
                    Atom result = core->xmlEntities->get(entityAtom);
                    if (result != undefinedAtom)
                    {
                        AvmAssert(atomIsIntptr(result));
                        wchar c = (wchar) atomGetIntptr(result);
                        // note: this code is allowed to construct a string
                        // containing illegal UTF16 sequences!
                        dest = dest->append16(&c, 1);
                        bgn = ++end;
                    }
                    else
                        ok = false;
                }
                else
                    ok = false;
            }
            else
                ok = false;
            if (!ok)
            {
                bgn = end + 1;
                // in this case we don't want to just throw out the
                // the value entirely as that would break existing content
                if (ampBgn < end)
                    dest = dest->append(m_str->substring(ampBgn, end));
            }
            bgn = m_str->indexOfCharCode('&', bgn, last);
        }
        // add any remaining text
        if (end < last)
            dest = dest->append(m_str->substring(end, last));

        if (intern)
            dest = core->internString(dest);

        return dest;
    }

    /*
        This provides backwards-compatibility for a rather obscure case:
        The old XMLParser considered any null terminator to end the parse,
        regardless of actual string length. Some buggy SWFs take a random
        ByteArray and try to convert it to XML. The old parser would stop parsing
        at the first null, but the new one won't, and thus is much likely to
        throw an exception (since random binary rarely parses as XML), while the
        old one would return quietly if there happened to be a null character
        before the first '<' character. Rather than add null-char checking
        back into the parser, let's just do a quick pre-check for a null char,
        and if one is found, truncate the string there.

        (Watson #2471228)
    */
    static Stringp truncateAtFirstNullChar(AvmCore* core, Stringp in)
    {
        const BugCompatibility* bugCompatibility = core->currentBugCompatibility();
        if (bugCompatibility->bugzilla526662)
            return in;
            
        int32_t const pos = in->indexOfCharCode(0);
        if (pos > 0)
        {
            in = in->substr(0, pos);
        }
        else if (pos == 0)
        {
            in = core->kEmptyString;
        }
        return in;
    }

    XMLParser::XMLParser(AvmCore* core, Stringp str) : m_str(truncateAtFirstNullChar(core, str)), m_pos (0)
    {
        this->core = core;

        if (!core->xmlEntities)
        {
            // Lazy creation of the XML entities table.
            core->xmlEntities = HeapHashtable::create(core->GetGC());

            const char *entities = "&amp\0\"quot\0'apos\0<lt\0>gt\0\xA0nbsp\0";

            while (*entities)
            {
                core->xmlEntities->add(core->internConstantStringLatin1(entities+1)->atom(),
                               core->intToAtom(*entities));
                while (*entities++) {
                    // do nothing
                }
            }
        }
    }

    void XMLParser::parse(bool ignoreWhite /*=false*/ )
    {
        m_pos = 0;
        m_ignoreWhite = ignoreWhite;
    }

    bool XMLTag::nextAttribute(uint32_t& index,
                               Stringp& name,
                               Stringp& value)
    {
        if (index >= attributes.length()) {
            return false;
        }
        name  = attributes.get(index++);
        value = attributes.get(index++);
        return true;
    }

} // namespace
