/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_XMLClass__
#define __avmplus_XMLClass__


namespace avmplus
{
    /**
     * class XMLClass
     */
    class GC_AS3_EXACT(XMLClass, ClassClosure)
    {
    protected:
        XMLClass(VTable* cvtable);
        
    public:
#ifdef DRC_TRIVIAL_DESTRUCTOR
        ~XMLClass();
#endif

        REALLY_INLINE static XMLClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) XMLClass(cvtable);
        }

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv);

        Atom ToXML(Atom arg);

        // static props/funcs off the XML object
        // XML.ignoreComments
        // XML.ignoreProcessingInstructions
        // XML.ignoreWhitespace
        // XML.prettyPrinting
        // XML.prettyIndent
        // XML.settings (in AS)
        // XML.setSettings ([settings]) (in AS)
        // XML.defaultSettings() (in AS)

        void set_ignoreComments(bool32 ignoreFlag);
        bool get_ignoreComments();

        void set_ignoreProcessingInstructions(bool32 ignoreFlag);
        bool get_ignoreProcessingInstructions();

        void set_ignoreWhitespace(bool32 ignoreFlag);
        bool get_ignoreWhitespace();

        void set_prettyPrinting(bool32 prettyPrinting);
        bool get_prettyPrinting();

        void set_prettyIndent(int indent);
        int get_prettyIndent();

        bool okToPrettyPrint() { return (get_prettyPrinting() && (m_prettyIndent >= 0)); }

        enum flags
        {
            kFlagIgnoreComments = 0x01,
            kFlagIgnoreProcessingInstructions = 0x02,
            kFlagIgnoreWhitespace = 0x04,
            kFlagPrettyPrinting = 0x08
        };
        
    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(XMLClass)

    public:
        int m_prettyIndent;
        uint8_t m_flags;

        GC_DATA_END(XMLClass)

    private:
        DECLARE_SLOTS_XMLClass;
    // ------------------------ DATA SECTION END
    };

    /**
     * class QName
     */
    class GC_AS3_EXACT(QNameClass, ClassClosure)
    {
    protected:
        QNameClass(VTable* cvtable);

    public:
        REALLY_INLINE static QNameClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) QNameClass(cvtable);
        }

        // this = argv[0]
        // arg1 = argv[1]
        // argN = argv[argc]
        Atom call(int argc, Atom* argv);

    // ------------------------ DATA SECTION BEGIN
    private:
        GC_NO_DATA(QNameClass)

        DECLARE_SLOTS_QNameClass;
    // ------------------------ DATA SECTION END
    };
}

#endif /* __avmplus_XMLClass__ */
