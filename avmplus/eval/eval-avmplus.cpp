/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_EVAL

#include "eval.h"

namespace avmplus
{
    class AvmplusHostContext : public RTC::HostContext
    {
    public:
        AvmplusHostContext(AvmCore* core, Toplevel* toplevel)
            : HostContext(core)
            , toplevel(toplevel)
            , nextstring(0)
        {
        }

        uint8_t* obtainStorageForResult(uint32_t nbytes);
        const wchar* readFileForEval(const wchar* basename, const wchar* filename, uint32_t* inputlen);
        void   freeInput(const wchar* input);
        void doubleToString(double d, char* buf, size_t bufsiz);
        bool stringToDouble(const char* s, double* d);
        void throwInternalError(const char* msgz);
        void throwSyntaxError(const char* msgz);
        
        ScriptBuffer script_buffer;

    private:
        Toplevel* const toplevel;
        wchar* strings[10];
        int nextstring;
    };
    
    uint8_t* AvmplusHostContext::obtainStorageForResult(uint32_t nbytes)
    {
        script_buffer = core->newScriptBuffer(nbytes);
        return script_buffer.getBuffer();
    }
    
    const wchar* AvmplusHostContext::readFileForEval(const wchar* basename, const wchar* filename, uint32_t* inputlen)
    {
        // FIXME: the mismatch between what eval needs and what the core API delivers is just stunning.
        if (nextstring == sizeof(strings)/sizeof(strings[0]))
            throwInternalError("includes too deeply nested");
        StUTF16String str(core->readFileForEval(core->newStringUTF16(basename), core->newStringUTF16(filename)));   // return value is already NUL-terminated
        wchar *s = new wchar[str.length()];
        VMPI_memcpy(s, str.c_str(), str.length()*sizeof(wchar));
        *inputlen = str.length();
        strings[nextstring++] = s;
        return s;
    }
    
    void AvmplusHostContext::freeInput(const wchar* input)
    {
        int i=0;
        while (i < nextstring && strings[i] != input)
            i++;
        if (i == nextstring) {
            AvmAssert(!"Bug in eval - freeing unallocated input");
            return;
        }
        delete [] strings[i];
        while (i < nextstring-1) {
            strings[i] = strings[i+1];
            i++;
        }
        nextstring--;
    }
    
    void AvmplusHostContext::doubleToString(double d, char* buf, size_t bufsiz)
    {
        StUTF8String s(MathUtils::convertDoubleToString(core, d));
        VMPI_strncpy(buf, s.c_str(), bufsiz);
        buf[bufsiz-1] = 0;
    }

    bool AvmplusHostContext::stringToDouble(const char* s, double* d)
    {
        return MathUtils::convertStringToDouble(core->newStringLatin1(s), d, false);
    }

    void AvmplusHostContext::throwSyntaxError(const char* msgz)
    {
        // FIXME - proper error ID?
        GCRef<SyntaxErrorObject> e = toplevel->syntaxErrorClass()->constructObject(core->newStringUTF8(msgz)->atom(), core->intToAtom(0));
        core->throwAtom(e->atom());
    }
    
    void AvmplusHostContext::throwInternalError(const char* msgz)
    {
        // FIXME - proper error ID?
        GCRef<ErrorObject> e = toplevel->errorClass()->constructObject(core->newStringUTF8(msgz)->atom(), core->intToAtom(0));
        core->throwAtom(e->atom());
    }
    
    ScriptBuffer compileProgram(AvmCore* core, Toplevel* toplevel, String* code, String* filename)
    {
        AvmplusHostContext context(core, toplevel);
        StUTF16String src(code);
        StUTF16String fn(filename);
        AvmAssert(src.c_str()[src.length()-1] == 0);
        RTC::Compiler compiler(&context, filename == NULL ? NULL : fn.c_str(), src.c_str(), src.length(), true);
        TRY(core, kCatchAction_Rethrow) {
            compiler.compile();
            return context.script_buffer;
        }
        CATCH(Exception *exception) {
            compiler.destroy();
            core->throwException(exception);
            /*NOTREACHED*/
        again:
            goto again;
        }
        END_CATCH
        END_TRY
    }
}

#endif // VMCFG_EVAL
