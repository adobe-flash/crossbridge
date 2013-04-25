/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    PrintWriter::StreamAdapter::StreamAdapter()
    {
        gcStream = NULL;
        nongcStream = NULL;
    }

    void PrintWriter::StreamAdapter::set(GCOutputStream* s)
    {
        MMgc::GC* gc = MMgc::GC::GetGC(s);
        if (gc->IsPointerToGCPage(this))
            WB(gc, gc->FindBeginningFast(this), &gcStream, s);
        else
            gcStream=s;
        nongcStream=NULL;
    }

    void PrintWriter::StreamAdapter::set(NonGCOutputStream* s)
    {
        gcStream=NULL;
        nongcStream=s;
    }

    void PrintWriter::StreamAdapter::write(const char* utf8)
    {
        if (gcStream)
            gcStream->write(utf8);
        else if (nongcStream)
            nongcStream->write(utf8);
    }

    void PrintWriter::StreamAdapter::writeN(const char* utf8, size_t charCount)
    {
        if (gcStream)
            gcStream->writeN(utf8, charCount);
        else if (nongcStream)
            nongcStream->writeN(utf8, charCount);
    }

    // The 2 base methods below (write/writeN) forward the request to the underlying
    // m_stream.  For optimal performance you should check the m_stream methods and
    // ensure that you're not performing unnecessary copies.  For example, if you have
    // a null terminated string and are using a ConsoleOutputStream its best to call
    // write() as opposed to writeN().  The latter copies to a intermediate buffer,
    // while the former does not.

    // ------------------------------------------------------------

    void PrintWriter::write(const char* utf8)
    {
        m_stream.write(utf8);
    }

    void PrintWriter::writeN(const char* utf8, size_t count)
    {
        m_stream.writeN(utf8, count);
    }

    // ------------------------------------------------------------

    // only called from Date.print()
    void PrintWriter::writeUTF16(const void* buffer, size_t count)
    {
        for (size_t i=0; i<count; i++)
        {
            *this << ((wchar*)buffer)[i];
        }
    }

    PrintWriter& PrintWriter::operator<< (hexAddr value)
    {
        writeHexAddr(value.getValue());
        return *this;
    }

    PrintWriter& PrintWriter::operator<< (asAtomHex a)
    {
        writeAtomHex(a._atom);
        return *this;
    }

    PrintWriter& PrintWriter::operator<< (asAtom a)
    {
        writeAtom(a._atom);
        return *this;
    }

    PrintWriter& PrintWriter::operator<< (asUTF16 a)
    {
        writeUTF16(a._buf, a._len);
        return *this;
    }

    PrintWriter& PrintWriter::operator<< (const char *str)
    {
        write(str);
        return *this;
    }

    PrintWriter& PrintWriter::operator<< (bool b)
    {
        *this << (b ? "true" : "false");
        return *this;
    }

    PrintWriter& PrintWriter::operator<< (char value)
    {
        char buf[2] = { value, '\0' };
        if (value) write(buf);
        return *this;
    }

    PrintWriter& PrintWriter::operator<< (wchar value)
    {
        // Encode the character as UTF-8
        if (value < 0x80) {
            *this << (char)value;
        } else {
            uint8_t Octets[7];
            size_t OctetsLen = UnicodeUtils::Ucs4ToUtf8((uint32_t)value, Octets);
            Octets[OctetsLen] = '\0';
            write((const char*)Octets);
        }
        return *this;
    }

    PrintWriter& PrintWriter::operator<< (int32_t value)
    {
        switch (value) {
            case 0:
                return *this << "0";
            case 1:
                return *this << "1";
            default:
            {
                char buffer[MathUtils::kMinSizeForInt32_t_base10_toString];
                int32_t len = sizeof(buffer);
                char* p = MathUtils::convertIntegerToStringBuffer((intptr_t)value, buffer, len, 10, MathUtils::kTreatAsSigned);
                return *this << p;
            }
        }
    }

    PrintWriter& PrintWriter::operator<< (uint64_t value)
    {
        switch (value) {
            case 0:
                return *this << "0";
            case 1:
                return *this << "1";
            default:
            {
                char buffer[MathUtils::kMinSizeForInt64_t_toString];
                int32_t len = sizeof(buffer);
                char* p = MathUtils::convertIntegerToStringBuffer((intptr_t) value, buffer, len, 10, MathUtils::kTreatAsUnsigned);
                return *this << p;
            }
        }
    }

    PrintWriter& PrintWriter::operator<< (int64_t value)
    {
        switch (value) {
            case 0:
                return *this << "0";
            case 1:
                return *this << "1";
            default:
            {
                char buffer[MathUtils::kMinSizeForInt64_t_toString];
                int32_t len = sizeof(buffer);
                char* p = MathUtils::convertIntegerToStringBuffer((intptr_t) value, buffer, len, 10, MathUtils::kTreatAsSigned);
                return *this << p;
            }
        }
    }

#if defined AVMPLUS_MAC && defined AVMPLUS_64BIT
    PrintWriter& PrintWriter::operator<< (ptrdiff_t value)
    {
        return *this << (int64_t)value;
    }
#endif

    PrintWriter& PrintWriter::operator<< (uint32_t value)
    {
        switch (value) {
            case 0:
                return *this << "0";
            case 1:
                return *this << "1";
            default:
            {
                char buffer[MathUtils::kMinSizeForInt32_t_base10_toString];
                int32_t len = sizeof(buffer);
                intptr_t wideVal = (intptr_t) value;
            #ifdef AVMPLUS_64BIT
                wideVal = (intptr_t)(uint32_t)value;
            #endif
                char* p = MathUtils::convertIntegerToStringBuffer(wideVal, buffer, len, 10, MathUtils::kTreatAsUnsigned);
                return *this << p;
            }
        }
    }

    /**
     * format the value of an atom for debugging.
     */
    void PrintWriter::writeAtom(Atom atom)
    {
        if (!AvmCore::isNull(atom))
        {
            switch (atomKind(atom))
            {
            case kNamespaceType:
                *this << AvmCore::atomToNamespace(atom);
                break;
            case kObjectType:
#ifdef AVMPLUS_VERBOSE
                *this << AvmCore::atomToScriptObject(atom);
#else
                this->writeAtomHex(atom);
#endif
                break;
            case kStringType:
                *this << "\"";
                if (AvmCore::isString(atom)) *this << AvmCore::atomToString(atom);
                *this << "\"";
                break;
            case kSpecialBibopType:
#ifdef VMCFG_FLOAT
                if(atom == AtomConstants::undefinedAtom){
                    *this << "undefined";
                    break;
                }
                if (bibopKind(atom) == kBibopFloatType) {
                    *this << AvmCore::atomToFloat(atom);
                    break;
                }
                if (bibopKind(atom) == kBibopFloat4Type) {
                    float4_t v = * (float4_t*)atomPtr(atom);
                    *this << ((float)f4_x(v)) << "," << ((float)f4_y(v)) << "," << ((float)f4_z(v)) << "," <<  ((float)f4_w(v));
                    break;
                }
                AvmAssertMsg(false,"Unknown bibop type!!!");
#endif // VMCFG_FLOAT
                *this << "undefined";
                break;
            case kBooleanType:
                *this << (((atom & ~7) != 0) ? "true" : "false");
                break;
            case kIntptrType:
                *this << (int64_t) atomGetIntptr(atom); // might be slow on 32b
                break;
            case kDoubleType:
                AvmAssert(atom != kDoubleType); // this would be a null pointer to double
                *this << AvmCore::atomToDouble(atom);
                break;
            default:
                AvmAssertMsg(0, "Was ist das?");
            }
        }
        else
        {
            *this << "null";
        }
    }

    void PrintWriter::writeAtomHex(Atom atom)
    {
        char buffer[MathUtils::kMinSizeForInt64_t_toString];
        int32_t len = sizeof(buffer);
        char* p = MathUtils::convertIntegerToStringBuffer((intptr_t)atom, buffer, len, 16, MathUtils::kTreatAsUnsigned);
        *this << p;
    }

    PrintWriter& PrintWriter::operator<< (double value)
    {
        // this could be an expensive call ... avoid it
        return *this << MathUtils::convertDoubleToString(m_core, value);
    }

    #define PRINT_STAR_OPERATOR_SUPPORT(x,r)                \
        PrintWriter& PrintWriter::operator<< (const x* obj) \
        {                                                   \
            if (!obj) { *this<<r; return *this; }           \
            return obj->print(*this);                       \
        }

    #define PRINT_AMP_OPERATOR_SUPPORT(x)                   \
        PrintWriter& PrintWriter::operator<< (const x& obj) \
        {                                                   \
            return obj.print(*this);                        \
        }

    #define PRINT_STAR_OPERATOR_SUPPORT_NULL(x) PRINT_STAR_OPERATOR_SUPPORT(x,"null")

#ifdef AVMPLUS_VERBOSE
    PRINT_STAR_OPERATOR_SUPPORT(String,"(null)")
    PRINT_STAR_OPERATOR_SUPPORT(Traits,"*")
    PRINT_STAR_OPERATOR_SUPPORT_NULL(ScriptObject)
    PRINT_STAR_OPERATOR_SUPPORT_NULL(MethodInfo)
    PRINT_STAR_OPERATOR_SUPPORT_NULL(Namespace)
    PRINT_STAR_OPERATOR_SUPPORT_NULL(NamespaceSet)
    PRINT_STAR_OPERATOR_SUPPORT_NULL(ScopeTypeChain)
    PRINT_STAR_OPERATOR_SUPPORT_NULL(ScopeChain)
    PRINT_STAR_OPERATOR_SUPPORT_NULL(MethodRecognizer)
    PRINT_AMP_OPERATOR_SUPPORT(Multiname)
#else
    PRINT_STAR_OPERATOR_SUPPORT(String,"(null)")
    PRINT_STAR_OPERATOR_SUPPORT(Traits,"*")
    PRINT_STAR_OPERATOR_SUPPORT_NULL(MethodInfo)
    PRINT_STAR_OPERATOR_SUPPORT_NULL(Namespace)
    PRINT_STAR_OPERATOR_SUPPORT_NULL(ScopeTypeChain)
    PRINT_AMP_OPERATOR_SUPPORT(Multiname)
#endif


#undef PRINT_STAR_OPERATOR_SUPPORT
#undef PRINT_STAR_OPERATOR_SUPPORT_NULL
#undef PRINT_AMP_OPERATOR_SUPPORT

    void PrintWriter::writeHexNibble(uint8_t value)
    {
        if (value < 10) {
            *this << (char)(value+'0');
        } else {
            *this << (char)(value+'A'-10);
        }
    }

    void PrintWriter::writeHexByte(uint8_t value)
    {
        writeHexNibble(value>>4);
        writeHexNibble(value&0x0f);
    }

    void PrintWriter::writeHexWord(uint16_t value)
    {
        writeHexByte((uint8_t)(value>>8));
        writeHexByte((uint8_t)(value&0xff));
    }

    void PrintWriter::writeHexAddr(uintptr_t value)
    {
#ifdef AVMPLUS_64BIT
        writeHexByte(uint8_t((value>>56) & 0xff));
        writeHexByte(uint8_t((value>>48) & 0xff));
        writeHexByte(uint8_t((value>>40) & 0xff));
        writeHexByte(uint8_t((value>>32) & 0xff));
#endif
        writeHexByte(uint8_t((value>>24) & 0xff));
        writeHexByte(uint8_t((value>>16) & 0xff));
        writeHexByte(uint8_t(value>>8));
        writeHexByte(uint8_t(value&0xff));
    }

    void PrintWriter::formatP(const char* format, Stringp arg1, Stringp arg2, Stringp arg3)
    {
        while (*format)
        {
            if (*format == '%')
            {
                switch (*++format)
                {
                    case '1':
                        if (arg1) *this << arg1;
                        break;

                    case '2':
                        if (arg2) *this << arg2;
                        break;

                    case '3':
                        if (arg3) *this << arg3;
                        break;

                    default:
                        AvmAssertMsg(0, "Invalid specifier; should be between '1' and '3'");
                        break;
                }
            }
            else
            {
                *this << *format;
            }
            format++;
        }
    }
}
