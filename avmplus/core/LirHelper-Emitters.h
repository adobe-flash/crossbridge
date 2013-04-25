/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef __avmplus_LirHelper__ // should be included in LIrHelper.h!
#ifdef TODO
#define TODO_backup TODO
#undef TODO
#endif

    class LIREmitter{
    public:
        LIREmitter(LirHelper* lh):_lh(lh) {}// do nothing;
        virtual ~LIREmitter() {}

        virtual LIns* operator()(enum BuiltinType, LIns*,LIns* ) const  { return TODO("operator()(enum,LIns,LIns) NOT IMPLEMENTED"); }
        virtual LIns* operator()(enum BuiltinType, LIns* ) const        { return TODO("operator()(enum,LIns) NOT IMPLEMENTED");      }
        LIns* TODO(const char* msg) const                               { AvmAssertMsg(false,msg); (void)msg;return _lh->InsConstAtom(AtomConstants::undefinedAtom); }
    protected:
          LirHelper* _lh; // used only to give access to all 
    };

    // BasicLIREmitter can emit a generic unary or binary operation
    class BasicLIREmitter:public LIREmitter{
    public:
        BasicLIREmitter(LirHelper* lh, LOpcode op, const CallInfo* call) : LIREmitter(lh),_dblOp(op), _call(call) {}
        virtual LIns* operator()(enum BuiltinType bt, LIns* oper1,LIns* oper2) const;
        virtual LIns* operator()(enum BuiltinType bt,LIns* oper) const ;
    protected:
        LOpcode _dblOp;
        const CallInfo* _call;
    };

    class  ModuloLIREmitter : public LIREmitter {
    public:
        ModuloLIREmitter(LirHelper* lh):LIREmitter(lh) {}
        virtual LIns* operator()(enum BuiltinType bt, LIns* op1,LIns* op2) const;
        virtual LIns* operator()(enum BuiltinType, LIns*) const { return TODO("ModuloEmitter must have two operands"); }
    };

    class IncrementLIREmitter : public LIREmitter 
    {
    public:
        IncrementLIREmitter (LirHelper* lh, bool shouldDecrement=false):LIREmitter(lh),_shouldDecrement(shouldDecrement) {}
        virtual LIns* operator()(enum BuiltinType, LIns* ,LIns* ) const { return TODO("IncrementLIREmitter can't handle two operands"); }
        virtual LIns* operator()(enum BuiltinType bt,LIns* oper) const ;
    protected:
        bool _shouldDecrement;
    };

#ifdef TODO_backup
#define TODO TODO_backup
#undef TODO
#endif
#endif
