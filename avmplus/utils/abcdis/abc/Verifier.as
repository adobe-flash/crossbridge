/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package abc {
    
    import flash.utils.ByteArray
    
    import abc.Constants
    import abc.BasicBlock
    import abc.Types.ABCFile
    import abc.Types.MethodBody
    
    public final class Verifier {
        
        private var abcfile:ABCFile
        
        public function Verifier(abcfile:ABCFile) {
            this.abcfile = abcfile
        }
        
        public static function verify(abcfile:ABCFile):void {
            var v:Verifier = new Verifier(abcfile)
            v.verifyAll()
        }
        
        public function verifyAll():void {
            for each(var mb:MethodBody in abcfile.getMethodBodies()) {
                verifyBlock(mb.blocks[0], mb)
            }
        }

        public function verifyBlock(bb:BasicBlock, mb:MethodBody) : void {
            if(bb.state == null && bb.entryState == null) {
                // Block with no preds, create empty state
                bb.entryState = new FrameState(mb);
                bb.state = bb.entryState;
            } else if(bb.state == null) {
                // Block with a verified predecessor, copy entryState
                bb.state = bb.entryState.clone()
            } else {
                // Already verified
                return
            }
            for(var i:int=0; i<bb.getInstrs().length; i++) {
                var cur:Instruction = bb.getInstrs()[i]
                var v:AVMType,v1:AVMType,v2:AVMType,result:AVMType = null
                var type:AVMType = null
                
                switch (cur.getOpcode()) {
                    case Constants.OP_iflt:
                    case Constants.OP_ifle:
                    case Constants.OP_ifnlt:
                    case Constants.OP_ifnle:
                    case Constants.OP_ifgt:
                    case Constants.OP_ifge:
                    case Constants.OP_ifngt:
                    case Constants.OP_ifnge:
                    case Constants.OP_ifeq:
                    case Constants.OP_ifstricteq:
                    case Constants.OP_ifne:
                    case Constants.OP_ifstrictne:
                        bb.state.checkStack(2,0)
                        //coder->writeOp1(state, pc, opcode, imm24)
                        bb.state.pop(2)
                        //checkTarget(nextpc+imm24)
                        break

                    case Constants.OP_iftrue:
                    case Constants.OP_iffalse:
                        bb.state.checkStack(1,0)
                        //emitCoerce(AVMType.BOOL, sp)
                        //coder->writeOp1(state, pc, opcode, imm24)
                        bb.state.pop()
                        //checkTarget(nextpc+imm24)
                        break

                    case Constants.OP_jump:
                        //bb.state.checkStack(0,0)
                        //coder->writeOp1(state, pc, opcode, imm24)
                        //checkTarget(nextpc+imm24)  // target block
                        break

                    case Constants.OP_lookupswitch:

                        bb.state.checkStack(1,0)
                        //peekType(AVMType.INT)
                        //coder->write(state, pc, opcode)
                        bb.state.pop()
                        //checkTarget(pc+imm24)
                        //uint32_t case_count = 1 + imm30b
                        //if (pc + size + 3*case_count > code_end)
                        //verifyFailed(kLastInstExceedsCodeSizeError)
                        //for (uint32_t i=0 i < case_count i++)

                        //int off = AvmCore::readS24(pc+size)
                        //checkTarget(pc+off)
                        //size += 3

                        break
                    case Constants.OP_throw:
                        bb.state.checkStack(1,0)
                        //coder->write(state, pc, opcode)
                        bb.state.pop()
                        break

                    case Constants.OP_returnvalue:
                        bb.state.checkStack(1,0)
                        //emitCoerce(ms->returnTraits(), sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop()
                        break

                    case Constants.OP_returnvoid:
                        //bb.state.checkStack(0,0)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_pushnull:
                        bb.state.checkStack(0,1)
                        //coder->write(state, pc, opcode, AVMType.NULL)
                        bb.state.push(AVMType.NULL)
                        break

                    case Constants.OP_pushundefined:
                        bb.state.checkStack(0,1)
                        //coder->write(state, pc, opcode, AVMType.VOID)
                        bb.state.push(AVMType.VOID)
                        break

                    case Constants.OP_pushtrue:
                        bb.state.checkStack(0,1)
                        //coder->write(state, pc, opcode, AVMType.BOOL)
                        bb.state.push(AVMType.BOOL, true)
                        break

                    case Constants.OP_pushfalse:
                        bb.state.checkStack(0,1)
                        //coder->write(state, pc, opcode, AVMType.BOOL)
                        bb.state.push(AVMType.BOOL, true)
                        break

                    case Constants.OP_pushnan:
                        bb.state.checkStack(0,1)
                        //coder->write(state, pc, opcode, AVMType.NUMBER)
                        bb.state.push(AVMType.NUMBER, true)
                        break

                    case Constants.OP_pushshort:
                        bb.state.checkStack(0,1)
                        //coder->write(state, pc, opcode, AVMType.INT)
                        bb.state.push(AVMType.INT, true)
                        break

                    case Constants.OP_pushbyte:
                        bb.state.checkStack(0,1)
                        //coder->write(state, pc, opcode, AVMType.INT)
                        bb.state.push(AVMType.INT, true)
                        break

                    case Constants.OP_debugfile:
                        //bb.state.checkStack(0,0)
                        //#if defined(DEBUGGER) || defined(VTUNE)
                        //checkCpoolOperand(cur.getImm(0), kStringType)
                        //#endif
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_dxns:
                        //bb.state.checkStack(0,0)
                        //if (!info->setsDxns())
                        //verifyFailed(kIllegalSetDxns, core->toErrorString(info))
                        //checkCpoolOperand(cur.getImm(0), kStringType)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_dxnslate:
                        bb.state.checkStack(1,0)
                        //if (!info->setsDxns())
                        //verifyFailed(kIllegalSetDxns, core->toErrorString(info))
                        // codgen will call intern on the input atom.
                        //coder->write(state, pc, opcode)
                        bb.state.pop()
                        break

                    case Constants.OP_pushstring:
                        bb.state.checkStack(0,1)
                        //if (imm30 == 0 || imm30 >= pool->constantStringCount)
                        //verifyFailed(kCpoolIndexRangeError, core->toErrorString(cur.getImm(0)), core->toErrorString(pool->constantStringCount))
                        //coder->write(state, pc, opcode, AVMType.STRING)
                        bb.state.push(AVMType.STRING, abcfile.getString(cur.getImm(0)) != null)
                        break

                    case Constants.OP_pushint:
                        bb.state.checkStack(0,1)
                        //if (imm30 == 0 || imm30 >= pool->constantIntCount)
                        //verifyFailed(kCpoolIndexRangeError, core->toErrorString(cur.getImm(0)), core->toErrorString(pool->constantIntCount))
                        //coder->write(state, pc, opcode, AVMType.INT)
                        bb.state.push(AVMType.INT,true)
                        break

                    case Constants.OP_pushuint:
                        bb.state.checkStack(0,1)
                        //if (imm30 == 0 || imm30 >= pool->constantUIntCount)
                        //verifyFailed(kCpoolIndexRangeError, core->toErrorString(cur.getImm(0)), core->toErrorString(pool->constantUIntCount))
                        //coder->write(state, pc, opcode, UAVMType.INT)
                        bb.state.push(AVMType.UINT,true)
                        break

                    case Constants.OP_pushdouble:
                        bb.state.checkStack(0,1)
                        //if (imm30 == 0 || imm30 >= pool->constantDoubleCount)
                        //verifyFailed(kCpoolIndexRangeError, core->toErrorString(cur.getImm(0)), core->toErrorString(pool->constantDoubleCount))
                        //coder->write(state, pc, opcode, AVMType.NUMBER)
                        bb.state.push(AVMType.NUMBER, true)
                        break

                    case Constants.OP_pushnamespace:
                        bb.state.checkStack(0,1)
                        //if (imm30 == 0 || imm30 >= pool->constantNsCount)
                        //verifyFailed(kCpoolIndexRangeError, core->toErrorString(cur.getImm(0)), core->toErrorString(pool->constantNsCount))
                        //coder->write(state, pc, opcode, NAMESPACE_TYPE)
                        bb.state.push(AVMType.NS, abcfile.getNamespace(cur.getImm(0)) != null)
                        break

                    case Constants.OP_setlocal:

                        bb.state.checkStack(1,0)
                        bb.state.checkLocal(cur.getImm(0))
                        //coder->write(state, pc, opcode)
                        v = bb.state.getStackValue(0)
                        bb.state.setLocalType(cur.getImm(0), v, v.notNull)
                        bb.state.pop()
                        break


                    case Constants.OP_setlocal0:
                    case Constants.OP_setlocal1:
                    case Constants.OP_setlocal2:
                    case Constants.OP_setlocal3:

                        bb.state.checkStack(1,0)
                        bb.state.checkLocal(cur.getOpcode() - Constants.OP_setlocal0)
                        //coder->write(state, pc, opcode)
                        v = bb.state.getStackValue(0)
                        bb.state.setLocalType(cur.getOpcode() - Constants.OP_setlocal0, v, v.notNull)
                        bb.state.pop()
                        break

                    case Constants.OP_getlocal:

                        bb.state.checkStack(0,1)
                        v = bb.state.checkLocal(cur.getImm(0))
                        //coder->write(state, pc, opcode)
                        bb.state.push(v)
                        break

                    case Constants.OP_getlocal0:
                    case Constants.OP_getlocal1:
                    case Constants.OP_getlocal2:
                    case Constants.OP_getlocal3:

                        bb.state.checkStack(0,1)
                        v = bb.state.checkLocal(cur.getOpcode() - Constants.OP_getlocal0)
                        //coder->write(state, pc, opcode)
                        bb.state.push(v)
                        break

                    case Constants.OP_kill:

                        //bb.state.checkStack(0,0)
                        bb.state.checkLocal(cur.getImm(0))
                        //coder->write(state, pc, opcode, NULL)
                        //state->setType(imm30, NULL, false)
                        break


                    case Constants.OP_inclocal:
                    case Constants.OP_declocal:
                        //bb.state.checkStack(0,0)
                        bb.state.checkLocal(cur.getImm(0))
                        //emitCoerce(AVMType.NUMBER, imm30)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_inclocal_i:
                    case Constants.OP_declocal_i:
                        //bb.state.checkStack(0,0)
                        bb.state.checkLocal(cur.getImm(0))
                        //emitCoerce(AVMType.INT, imm30)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_newfunction:

                        bb.state.checkStack(0,1)
                        //MethodInfo* f = checkMethodInfo(cur.getImm(0))
                        //Traits* ftraits = core->traits.function_itraits
                        //const ScopeTypeChain* fscope = ScopeTypeChain::create(core->GetGC(), ftraits, scope, state, NULL, NULL)
                        // Duplicate function definitions aren't strictly legal, but can occur
                        // in otherwise "well formed" ABC due to old, buggy versions of ASC.
                        // Specifically, code of the form
                        //
                        //    public function simpleTest():void
                        //    {
                        //      var f:Function = function():void { }
                        //      f()
                        //      f = function functwo (x):void { }
                        //      f(8)
                        //    }
                        //
                        // could cause the second interior function ("functwo") to include a bogus, unused OP_newfunction
                        // call to itself inside the body of functwo. This caused the scope to get reinitialized
                        // and generally caused havok. However, we want to allow existing code of this form to continue
                        // to work, so check to see if we already have a declaringScope, and if so, require that
                        // it match this one.
                        //const ScopeTypeChain* curScope = f->declaringScope()
                        //if (curScope != NULL)

                        //if (!curScope->equals(fscope))

                        // if info->method_id() == imm30, f == info, and therefore
                        // curScope == scope -- don't redefine, don't fail verification,
                        // just accept it. see https://bugzilla.mozilla.org/show_bug.cgi?id=544370
                        //if (info->method_id() != int32_t(cur.getImm(0)))
                        //toplevel->throwVerifyError(kCorruptABCError)

                        //AvmAssert(curScope->equals(scope))

                        //AvmAssert(f->isResolved())

                        //else

                        //f->makeIntoPrototypeFunction(toplevel, fscope)

                        //#ifdef AVMPLUS_VERBOSE
                        //if (verbose)
                        //printScope("function-scope", f->declaringScope())
                        //#endif
                        //coder->writeOp1(state, pc, opcode, imm30, ftraits)
                        bb.state.push(AVMType.NULL) // bb.state.push(ftraits, true)
                        
                        break


                    case Constants.OP_getlex:

                        //if (state->scopeDepth + scope->size == 0)
                        //verifyFailed(kFindVarWithNoScopeError)
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(0, 1, &multiname)
                        //if (multiname.isRuntime())
                        //verifyFailed(kIllegalOpMultinameError, core->toErrorString(opcode), core->toErrorString(&multiname))
                        //emitFindProperty(OP_findpropstrict, multiname, imm30, pc)
                        //emitGetProperty(multiname, 1, imm30, pc)
                        bb.state.push(AVMType.NULL)
                        break


                    case Constants.OP_findpropstrict:
                    case Constants.OP_findproperty:
                        bb.state.popStackMulti(0, 1, abcfile.getMultiname(cur.getImm(0)))
                        //if (state->scopeDepth + scope->size == 0)
                        //verifyFailed(kFindVarWithNoScopeError)
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(0, 1, &multiname)
                        //emitFindProperty(opcode, multiname, imm30, pc)
                        break


                    case Constants.OP_newclass:

                        bb.state.checkStack(1, 1)
                        // must be a CONSTANT_Multiname
                        //Traits* ctraits = checkClassInfo(cur.getImm(0))
                        // the actual result type will be the static traits of the new class.
                        // make sure the traits came from this pool.  they have to because
                        // the class_index operand is resolved from the current pool.
                        //AvmAssert(ctraits->pool == pool)
                        //Traits *itraits = ctraits->itraits

                        // add a type constraint for the "this" scope of static methods
                        //const ScopeTypeChain* cscope = ScopeTypeChain::create(core->GetGC(), ctraits, scope, state, NULL, ctraits)

                        //if (state->scopeDepth > 0)

                        // innermost scope must be the base class object or else createInstance()
                        // will malfunction because it will use the wrong [base] class object to
                        // construct the instance.  See ScriptObject::createInstance()
                        //Traits* baseCTraits = state->scopeValue(state->scopeDepth-1).traits
                        //if (!baseCTraits || baseCTraits->itraits != itraits->base)
                        //verifyFailed(kCorruptABCError)


                        // add a type constraint for the "this" scope of instance methods
                        //const ScopeTypeChain* iscope = ScopeTypeChain::create(core->GetGC(), itraits, cscope, NULL, ctraits, itraits)

                        //ctraits->resolveSignatures(toplevel)
                        //itraits->resolveSignatures(toplevel)

                        // we must always set the scopes here, whether or not they have been set yet and
                        // whether or not the traits were resolved already.
                        //ctraits->setDeclaringScopes(cscope)
                        //itraits->setDeclaringScopes(iscope)

                        //#ifdef AVMPLUS_VERBOSE
                        //if (verbose)
                        //printScope("class-scope", cscope)
                        //#endif

                        //emitCoerce(CLASS_TYPE, state->sp())
                        //coder->writeOp1(state, pc, opcode, imm30, ctraits)
                        bb.state.pop_push(1, AVMType.NULL) // bb.state.pop_push(1, ctraits, true)
                        break


                    case Constants.OP_finddef:

                        // must be a CONSTANT_Multiname.
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(0, 1, &multiname)
                        //if (!multiname.isBinding())

                        // error, def name must be CT constant, regular name
                        //verifyFailed(kIllegalOpMultinameError, core->toErrorString(opcode), core->toErrorString(&multiname))

                        //MethodInfo* script = pool->getNamedScript(&multiname)
                        //Traits* resultType
                        //if (script != (MethodInfo*)BIND_NONE && script != (MethodInfo*)BIND_AMBIGUOUS) {
                        // found a single matching traits
                        //resultType = script->declaringTraits()
                        //else {
                        // no traits, or ambiguous reference.  use Object, anticipating
                        // a runtime exception
                        //resultType = OBJECT_TYPE

                        //coder->writeOp1(state, pc, opcode, imm30, resultType)
                        bb.state.push(AVMType.NULL) // bb.state.push(resultType, true)
                        break


                    case Constants.OP_setproperty:
                    case Constants.OP_initproperty:
                        bb.state.popStackMulti(2, 0, abcfile.getMultiname(cur.getImm(0)))
                        
                        // stack in: object [ns] [name] value
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname) // CONSTANT_Multiname
                        //checkStackMulti(2, 0, &multiname)

                        //uint32_t n=2
                        //checkPropertyMultiname(n, multiname)

                        //Traitsp declarer = NULL
                        //Value& obj = state->peek(n)
                        //Binding b = (opcode == OP_initproperty) ?
                        //toplevel->getBindingAndDeclarer(obj.traits, multiname, declarer) :
                        //toplevel->getBinding(obj.traits, &multiname)
                        //Traits* propTraits = readBinding(obj.traits, b)

                        //emitCheckNull(sp-(n-1))

                        //if (AvmCore::isSlotBinding(b) &&
                        // it's a var, or a const being set from the init function
                        //(!AvmCore::isConstBinding(b) ||
                        //(opcode == OP_initproperty && declarer->init == info)))

                        //emitCoerce(propTraits, state->sp())
                        //coder->writeOp2(state, pc, OP_setslot, (uint32_t)AvmCore::bindingToSlotId(b), sp-(n-1), propTraits)
                        //bb.state.pop(n)
                        break

                        // else: setting const from illegal context, fall through

                        // If it's an accessor that we can early bind, do so.
                        // Note that this cannot be done on String or Namespace,
                        // since those are represented by non-ScriptObjects
                        //if (AvmCore::hasSetterBinding(b))

                        // invoke the setter
                        //int disp_id = AvmCore::bindingToSetterId(b)
                        //const TraitsBindingsp objtd = obj.traits->getTraitsBindings()
                        //MethodInfo *f = objtd->getMethod(disp_id)
                        //AvmAssert(f != NULL)
                        //MethodSignaturep fms = f->getMethodSignature()
                        //emitCoerceArgs(f, 1)
                        //Traits* propType = fms->returnTraits()
                        //coder->writeOp2(state, pc, opcode, imm30, n, propType)
                        //bb.state.pop(n)
                        //break


                        //if( obj.traits == VECTORAVMType.INT  || obj.traits == VECTORUAVMType.INT ||
                        //obj.traits == VECTORDOUBLE_TYPE )

                        //bool attr = multiname.isAttr()
                        //Traits* indexType = state->value(state->sp()-1).traits

                        // NOTE a dynamic name should have the same version as the current pool
                        //bool maybeIntegerIndex = !attr && multiname.isRtname() && multiname.containsAnyPublicNamespace()
                        //if( maybeIntegerIndex && (indexType == UAVMType.INT || indexType == AVMType.INT) )

                        //if(obj.traits == VECTORAVMType.INT)
                        //emitCoerce(AVMType.INT, state->sp())
                        //else if(obj.traits == VECTORUAVMType.INT)
                        //emitCoerce(UAVMType.INT, state->sp())
                        //else if(obj.traits == VECTORDOUBLE_TYPE)
                        //emitCoerce(AVMType.NUMBER, state->sp())



                        // default - do getproperty at runtime

                        //coder->writeOp2(state, pc, opcode, imm30, n, propTraits)
                        //bb.state.pop(n)
                        break


                    case Constants.OP_getproperty:
                        bb.state.popStackMulti(1, 1, abcfile.getMultiname(cur.getImm(0)))
                        // stack in: object [ns [name]]
                        // stack out: value
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname) // CONSTANT_Multiname
                        //checkStackMulti(1, 1, &multiname)

                        //uint32_t n=1
                        //checkPropertyMultiname(n, multiname)
                        //emitGetProperty(multiname, n, imm30, pc)
                        break


                    case Constants.OP_getdescendants:
                        bb.state.popStackMulti(1, 1, abcfile.getMultiname(cur.getImm(0)))
                        // stack in: object [ns] [name]
                        // stack out: value
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(1, 1, &multiname)

                        //uint32_t n=1
                        //checkPropertyMultiname(n, multiname)
                        //emitCheckNull(sp-(n-1))
                        //coder->write(state, pc, opcode)
                        //bb.state.pop_push(1, AVMType.NULL) //bb.state.pop_push(n, NULL)
                        break


                    case Constants.OP_checkfilter:
                        bb.state.checkStack(1, 1)
                        //emitCheckNull(sp)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_deleteproperty:
                        bb.state.popStackMulti(1, 1, abcfile.getMultiname(cur.getImm(0)))
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(1, 1, &multiname)
                        //uint32_t n=1
                        //checkPropertyMultiname(n, multiname)
                        //emitCheckNull(sp-(n-1))
                        //coder->write(state, pc, opcode)
                        //bb.state.pop_push(n, AVMType.BOOL)
                        break


                    case Constants.OP_astype:

                        bb.state.checkStack(1, 1)
                        // resolve operand into a traits, and push that type.
                        //Traits *t = checkTypeName(cur.getImm(0)) // CONSTANT_Multiname
                        //int index = sp
                        //Traits* rhs = state->value(index).traits
                        //if (!canAssign(t, rhs))

                        //Traits* resultType = t
                        // result is typed value or null, so if type can't hold null,
                        // then result type is Object.
                        //if (t && t->isMachineType())
                        //resultType = OBJECT_TYPE
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(1, AVMType.NULL) // bb.state.pop_push(1, resultType)

                        break


                    case Constants.OP_astypelate:

                        bb.state.checkStack(2,1)
                        //Value& classValue = state->peek(1) // rhs - class
                        //Traits* ct = classValue.traits
                        //Traits* t = NULL
                        //if (ct && (t=ct->itraits) != 0)
                        //if (t->isMachineType())
                        //t = OBJECT_TYPE
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.NULL) // bb.state.pop_push(2, t)
                        break


                    case Constants.OP_coerce:

                        bb.state.checkStack(1,1)
                        v = bb.state.peek()
                        bb.state.setType(0, AVMType.typeFromName(abcfile.getMultiname(cur.getImm(0)), v.notNull), v.notNull)
                        //v = bb.state.getStackValue(0)
                        //type = bb.state.checkTypeName(cur.getImm(0))
                        //coder->write(state, pc, opcode, type)
                        //state.setType(0, type, v.notNull)
                        break

                    case Constants.OP_convert_b:
                    case Constants.OP_coerce_b:

                        bb.state.checkStack(1,1)
                        v = bb.state.getStackValue(0)
                        type = AVMType.BOOL
                        //coder->write(state, pc, opcode, type)
                        bb.state.setType(0, type, v.notNull)
                        break

                    case Constants.OP_coerce_o:

                        bb.state.checkStack(1,1)
                        v = bb.state.getStackValue(0)
                        type = AVMType.OBJECT
                        //coder->write(state, pc, opcode, type)
                        bb.state.setType(0, type, v.notNull)
                        break

                    case Constants.OP_coerce_a:

                        bb.state.checkStack(1,1)
                        v = bb.state.getStackValue(0)
                        type = AVMType.NULL
                        //coder->write(state, pc, opcode, type)
                        bb.state.setType(0, type, v.notNull)
                        break

                    case Constants.OP_convert_i:
                    case Constants.OP_coerce_i:

                        bb.state.checkStack(1,1)
                        v = bb.state.getStackValue(0)
                        type = AVMType.INT
                        //coder->write(state, pc, opcode, type)
                        bb.state.setType(0, type, v.notNull)
                        break

                    case Constants.OP_convert_u:
                    case Constants.OP_coerce_u:

                        bb.state.checkStack(1,1)
                        v = bb.state.getStackValue(0)
                        type = AVMType.UINT
                        //coder->write(state, pc, opcode, type)
                        bb.state.setType(0, type, v.notNull)
                        break

                    case Constants.OP_convert_d:
                    case Constants.OP_coerce_d:

                        bb.state.checkStack(1,1)
                        v = bb.state.getStackValue(0)
                        type = AVMType.NUMBER
                        //coder->write(state, pc, opcode, type)
                        bb.state.setType(0, type, v.notNull)
                        break

                    case Constants.OP_coerce_s:

                        bb.state.checkStack(1,1)
                        v = bb.state.getStackValue(0)
                        type = AVMType.STRING
                        //coder->write(state, pc, opcode, type)
                        bb.state.setType(0, type, v.notNull)
                        break

                    case Constants.OP_istype:
                        bb.state.checkStack(1,1)
                        // resolve operand into a traits, and test if value is that type
                        //checkTypeName(cur.getImm(0)) // CONSTANT_Multiname
                        //coder->write(state, pc, opcode)
                        bb.state.pop(1)
                        bb.state.push(AVMType.BOOL)
                        break

                    case Constants.OP_istypelate:
                        bb.state.checkStack(2,1)
                        //coder->write(state, pc, opcode)
                        // TODO if the only common base type of lhs,rhs is Object, then result is always false
                        bb.state.pop_push(2, AVMType.BOOL)
                        break

                    case Constants.OP_convert_o:
                        bb.state.checkStack(1,1)
                        // ISSUE should result be Object, laundering the type?
                        // ToObject throws an exception on null and undefined, so after this runs we
                        // know the value is safe to dereference.
                        //emitCheckNull(sp)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_convert_s:
                    case Constants.OP_esc_xelem:
                    case Constants.OP_esc_xattr:
                        bb.state.checkStack(1,1)
                        // this is the ECMA ToString and ToXMLString operators, so the result must not be null
                        // (ToXMLString is split into two variants - escaping elements and attributes)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(1, AVMType.STRING, true)
                        break

                    case Constants.OP_callstatic:

                        //  Ensure that the method is eligible for callstatic.
                        //  Note: This fails when called by verifyEarly(), since the
                        //  data structures being checked have not been initialized.
                        //  Need to either rearrange the initialization sequence or
                        //  mark this verify pass as "needs late retry."
                        //if ( ! abc_env->getMethod(cur.getImm(0)) )
                        //verifyFailed(kCorruptABCError)

                        //MethodInfo* m = checkMethodInfo(cur.getImm(0))
                        //const uint32_t argc = imm30b
                        bb.state.checkStack(cur.getImm(1)+1, 1)

                        //MethodSignaturep mms = m->getMethodSignature()
                        //if (!mms->paramTraits(0))

                        //verifyFailed(kDanglingFunctionError, core->toErrorString(m), core->toErrorString(info))


                        //emitCoerceArgs(m, argc)

                        //Traits *resultType = mms->returnTraits()
                        //emitCheckNull(sp-argc)
                        //coder->writeOp2(state, pc, OP_callstatic, (uint32_t)m->method_id(), argc, resultType)
                        bb.state.pop_push(cur.getImm(1)+1, AVMType.NULL) // bb.state.pop_push(argc+1, resultType)
                        break


                    case Constants.OP_call:

                        //const uint32_t argc = imm30
                        bb.state.checkStack(cur.getImm(0)+2, 1)
                        // don't need null check, AvmCore::call() uses toFunction() for null check.

                        //coder->writeOp1(state, pc, opcode, argc)
                        bb.state.pop_push(cur.getImm(0)+2, AVMType.NULL)
                        break


                    case Constants.OP_construct:

                        //const uint32_t argc = imm30
                        bb.state.checkStack(cur.getImm(0)+1, 1)

                        // don't need null check, AvmCore::construct() uses toFunction() for null check.
                        //Traits* ctraits = state->peek(argc+1).traits
                        //Traits* itraits = ctraits ? ctraits->itraits : NULL
                        //coder->writeOp1(state, pc, opcode, argc)
                        bb.state.pop_push(cur.getImm(0)+1, AVMType.NULL) // bb.state.pop_push(argc+1, itraits, true)
                        break


                    case Constants.OP_callmethod:
                        
                        //const uint32_t argc = imm30b
                        bb.state.checkStack(cur.getImm(1)+1,1)

                        //const int disp_id = imm30-1
                        //if (disp_id >= 0)

                        //Value& obj = state->peek(argc+1)
                        //if( !obj.traits )
                        //verifyFailed(kCorruptABCError)
                        //else
                        //verifyFailed(kIllegalEarlyBindingError, core->toErrorString(obj.traits))

                        //else

                        //verifyFailed(kZeroDispIdError)

                        break


                    case Constants.OP_callproperty:
                    case Constants.OP_callproplex:
                    case Constants.OP_callpropvoid:
                        bb.state.popStackMulti(cur.getImm(1)+1, 1, abcfile.getMultiname(cur.getImm(0)))
                        // stack in: obj [ns [name]] args
                        // stack out: result
                        //const uint32_t argc = imm30b
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(argc+1, 1, &multiname)
                        //checkCallMultiname(opcode, &multiname)

                        //uint32_t n = argc+1 // index of receiver
                        //checkPropertyMultiname(n, multiname)
                        //emitCallproperty(opcode, sp, multiname, imm30, imm30b, pc)
                        
                        if (cur.getOpcode() == Constants.OP_callpropvoid) {
                            bb.state.pop()
                        }
                        
                        break


                    case Constants.OP_constructprop:
                        bb.state.popStackMulti(cur.getImm(1)+1, 1, abcfile.getMultiname(cur.getImm(0)))
                        // stack in: obj [ns [name]] args
                        //const uint32_t argc = imm30b
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(argc+1, 1, &multiname)
                        //checkCallMultiname(opcode, &multiname)

                        //uint32_t n = argc+1 // index of receiver
                        //checkPropertyMultiname(n, multiname)


                        //Value& obj = state->peek(n) // make sure object is there
                        //Binding b = toplevel->getBinding(obj.traits, &multiname)
                        //Traits* ctraits = readBinding(obj.traits, b)
                        //emitCheckNull(sp-(n-1))
                        //coder->writeOp2(state, pc, opcode, imm30, argc, ctraits)

                        //Traits* itraits = ctraits ? ctraits->itraits : NULL
                        //bb.state.pop_push(n, itraits, itraits==NULL?false:true)
                        break


                    case Constants.OP_applytype:

                        // in: factory arg1..N
                        // out: type
                        //const uint32_t argc = imm30
                        bb.state.checkStack(cur.getImm(0)+1, 1)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(cur.getImm(0)+1, AVMType.NULL, true)
                        break


                    case Constants.OP_callsuper:
                    case Constants.OP_callsupervoid:
                        bb.state.popStackMulti(cur.getImm(1) + 1, 1, abcfile.getMultiname(cur.getImm(0)))
                        // stack in: obj [ns [name]] args
                        //const uint32_t argc = imm30b
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(argc+1, 1, &multiname)

                        //if (multiname.isAttr())
                        //verifyFailed(kIllegalOpMultinameError, core->toErrorString(&multiname))

                        //uint32_t n = argc+1 // index of receiver
                        //checkPropertyMultiname(n, multiname)

                        //Traits* base = emitCoerceSuper(sp-(n-1))
                        //const TraitsBindingsp basetd = base->getTraitsBindings()

                        //Binding b = toplevel->getBinding(base, &multiname)

                        //Traits *resultType = NULL
                        //if (AvmCore::isMethodBinding(b))

                        //int disp_id = AvmCore::bindingToMethodId(b)
                        //MethodInfo* m = basetd->getMethod(disp_id)
                        //if( !m ) verifyFailed(kCorruptABCError)
                        //MethodSignaturep mms = m->getMethodSignature()
                        //resultType = mms->returnTraits()


                        //emitCheckNull(sp-(n-1))
                        //coder->writeOp2(state, pc, opcode, imm30, argc, base)
                        //bb.state.pop_push(n, resultType)

                        if (cur.getOpcode() == Constants.OP_callsupervoid) {
                            bb.state.pop()
                        }

                        break


                    case Constants.OP_getsuper:
                        bb.state.popStackMulti(1, 1, abcfile.getMultiname(cur.getImm(0)))
                        // stack in: obj [ns [name]]
                        // stack out: value
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(1, 1, &multiname)
                        //uint32_t n=1
                        //checkPropertyMultiname(n, multiname)

                        //if (multiname.isAttr())
                        //verifyFailed(kIllegalOpMultinameError, core->toErrorString(&multiname))

                        //Traits* base = emitCoerceSuper(sp-(n-1))
                        //Binding b = toplevel->getBinding(base, &multiname)
                        //Traits* propType = readBinding(base, b)
                        //emitCheckNull(sp-(n-1))
                        //coder->writeOp2(state, pc, opcode, imm30, n, base)

                        //if (AvmCore::hasGetterBinding(b))

                        //int disp_id = AvmCore::bindingToGetterId(b)
                        //const TraitsBindingsp basetd = base->getTraitsBindings()
                        //MethodInfo *f = basetd->getMethod(disp_id)
                        //AvmAssert(f != NULL)
                        //MethodSignaturep fms = f->getMethodSignature()
                        //Traits* resultType = fms->returnTraits()
                        //bb.state.pop_push(n, resultType)

                        //else

                        //bb.state.pop_push(n, propType)

                        break


                    case Constants.OP_setsuper:
                        bb.state.popStackMulti(2, 0, abcfile.getMultiname(cur.getImm(0)))
                        // stack in: obj [ns [name]] value
                        //Multiname multiname
                        //checkConstantMultiname(imm30, multiname)
                        //checkStackMulti(2, 0, &multiname)
                        //uint32_t n=2
                        //checkPropertyMultiname(n, multiname)

                        //if (multiname.isAttr())
                        //verifyFailed(kIllegalOpMultinameError, core->toErrorString(&multiname))

                        //Traits* base = emitCoerceSuper(sp-(n-1))
                        //emitCheckNull(sp-(n-1))
                        //coder->writeOp2(state, pc, opcode, imm30, n, base)
                        //bb.state.pop(n)
                        break


                    case Constants.OP_constructsuper:

                        // stack in: obj, args ...
                        //const uint32_t argc = imm30
                        bb.state.checkStack(cur.getImm(0)+1, 0)

                        //int32_t ptrIndex = sp-argc
                        //Traits* baseTraits = emitCoerceSuper(ptrIndex) // check receiver

                        //MethodInfo *f = baseTraits->init
                        //AvmAssert(f != NULL)

                        //emitCoerceArgs(f, argc)
                        //emitCheckNull(sp-argc)
                        //coder->writeOp2(state, pc, opcode, 0, argc, baseTraits)
                        bb.state.pop(cur.getImm(0)+1)
                        break


                    case Constants.OP_newobject:

                        //uint32_t argc = imm30
                        bb.state.checkStack(2 * cur.getImm(0), 1)
                        //int n=0
                        //while (argc-- > 0)

                        //n += 2
                        //peekType(AVMType.STRING, n) // name will call intern on it

                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2 * cur.getImm(0), AVMType.OBJECT, true)
                        break


                    case Constants.OP_newarray:
                        bb.state.checkStack(cur.getImm(0), 1)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(cur.getImm(0), AVMType.ARRAY, true)
                        break

                    case Constants.OP_pushscope:

                        bb.state.checkStack(1,0)
                        //if (state->scopeDepth+1 > max_scope)
                        //verifyFailed(kScopeStackOverflowError)

                        //Traits* scopeTraits = state->peek().traits
                        //if (scope->fullsize > (scope->size+state->scopeDepth))

                        // extra constraints on type of pushscope allowed
                        //Traits* requiredType = scope->getScopeTraitsAt(scope->size+state->scopeDepth)
                        //if (!scopeTraits || !scopeTraits->subtypeof(requiredType))

                        //verifyFailed(kIllegalOperandTypeError, core->toErrorString(scopeTraits), core->toErrorString(requiredType))



                        //emitCheckNull(sp)
                        //coder->writeOp1(state, pc, opcode, scopeBase+state->scopeDepth)
                        bb.state.pop()
                        //state->setType(scopeBase+state->scopeDepth, scopeTraits, true, false)
                        //state->scopeDepth++
                        break


                    case Constants.OP_pushwith:

                        bb.state.checkStack(1,0)

                        //if (state->scopeDepth+1 > max_scope)
                        //verifyFailed(kScopeStackOverflowError)

                        //emitCheckNull(sp)
                        //coder->writeOp1(state, pc, opcode, scopeBase+state->scopeDepth)

                        //Traits* scopeTraits = state->peek().traits
                        bb.state.pop()
                        //state->setType(scopeBase+state->scopeDepth, scopeTraits, true, true)

                        //if (state->withBase == -1)
                        //state->withBase = state->scopeDepth

                        //state->scopeDepth++
                        break


                    case Constants.OP_newactivation:

                        bb.state.checkStack(0, 1)
                        //if (!info->needActivation())
                        //verifyFailed(kInvalidNewActivationError)
                        //Traits* atraits = info->resolveActivation(toplevel)
                        //coder->write(state, pc, opcode, atraits)
                        bb.state.push(AVMType.NULL) // bb.state.push(atraits, true)
                        break

                    case Constants.OP_newcatch:

                        bb.state.checkStack(0, 1)
                        //if (!info->abc_exceptions() || imm30 >= (uint32_t)info->abc_exceptions()->exception_count)
                        //verifyFailed(kInvalidNewActivationError)
                        // FIXME better error msg
                        //ExceptionHandler* handler = &info->abc_exceptions()->exceptions[imm30]
                        //coder->write(state, pc, opcode)
                        bb.state.push(AVMType.NULL) // bb.state.push(AVMType.NULL, true) //bb.state.push(handler->scopeTraits, true)
                        break

                    case Constants.OP_popscope:
                        //bb.state.checkStack(0,0)
                        //if (state->scopeDepth-- <= outer_depth)
                        //verifyFailed(kScopeStackUnderflowError)

                        //coder->write(state, pc, opcode)

                        //if (state->withBase >= state->scopeDepth)
                        //state->withBase = -1
                        break

                    case Constants.OP_getscopeobject:
                        bb.state.checkStack(0,1)

                        // local scope
                        //if (imm8 >= state->scopeDepth)
                        //verifyFailed(kGetScopeObjectBoundsError, core->toErrorString(imm8))

                        //coder->writeOp1(state, pc, opcode, imm8)

                        // this will copy type and all attributes too
                        bb.state.push(AVMType.NULL) // bb.state.push(state->scopeValue(imm8))
                        break

                    case Constants.OP_getouterscope:

                        bb.state.checkStack(0,1)
                        //const ScopeTypeChain* scope = info->declaringScope()
                        //int captured_depth = scope->size
                        //if (captured_depth > 0)

                        // enclosing scope
                        bb.state.push(AVMType.NULL) // bb.state.push(scope->getScopeTraitsAt(cur.getImm(0)), true)

                        //else

                        //#ifdef _DEBUG
                        //if (pool->isBuiltin)
                        //core->console << "getouterscope >= depth (" << imm30 << " >= " << state->scopeDepth << ")\n"
                        //#endif
                        //verifyFailed(kGetScopeObjectBoundsError, core->toErrorString(0))

                        //coder->write(state, pc, opcode)
                        break


                    case Constants.OP_getglobalscope:
                        bb.state.checkStack(0,1)
                        //coder->write(state, pc, OP_getglobalscope)
                        //checkGetGlobalScope() // after coder->write because mutates stack that coder depends on
                        bb.state.push(AVMType.NULL)
                        break

                    case Constants.OP_getglobalslot:

                        bb.state.checkStack(0,1)
                        //Value& obj = state->peek()
                        //uint32_t index = imm30-1
                        //checkGetGlobalScope()
                        //checkEarlySlotBinding(obj.traits)
                        //Traits* slotTraits = checkSlot(obj.traits, index)
                        //emitCheckNull(index)
                        //coder->writeOp1(state, pc, OP_getglobalslot, index)
                        bb.state.pop_push(1, AVMType.NULL) // bb.state.pop_push(1, slotTraits)
                        break


                    case Constants.OP_setglobalslot:

                        // FIXME need test case
                        //if (!state->scopeDepth && !scope->size)
                        //verifyFailed(kNoGlobalScopeError)
                        //Traits *globalTraits = scope->size > 0 ? scope->getScopeTraitsAt(0) : state->scopeValue(0).traits
                        bb.state.checkStack(1,0)
                        //checkEarlySlotBinding(globalTraits)
                        //Traits* slotTraits = checkSlot(globalTraits, imm30-1)
                        //emitCoerce(slotTraits, state->sp())
                        //coder->writeOp1(state, pc, opcode, imm30-1, slotTraits)
                        bb.state.pop()
                        break


                    case Constants.OP_getslot:

                        bb.state.checkStack(1,1)
                        //Value& obj = state->peek()
                        //checkEarlySlotBinding(obj.traits)
                        //Traits* slotTraits = checkSlot(obj.traits, imm30-1)
                        //emitCheckNull(state->sp())
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(1, AVMType.NULL) // bb.state.pop_push(1, slotTraits)
                        break


                    case Constants.OP_setslot:

                        bb.state.checkStack(2,0)
                        //Value& obj = state->peek(2) // object
                        // if code isn't in pool, its our generated init function which we always
                        // allow early binding on
                        //if(pool->isCodePointer(info->abc_body_pos()))
                        //checkEarlySlotBinding(obj.traits)
                        //Traits* slotTraits = checkSlot(obj.traits, imm30-1)
                        //emitCoerce(slotTraits, state->sp())
                        //emitCheckNull(state->sp()-1)
                        //coder->write(state, pc, opcode)
                        bb.state.pop(2)
                        break


                    case Constants.OP_pop:
                        bb.state.checkStack(1,0)
                        //coder->write(state, pc, opcode)
                        bb.state.pop()
                        break

                    case Constants.OP_dup:

                        bb.state.checkStack(1, 2)
                        v = bb.state.peek()
                        //coder->write(state, pc, opcode)
                        bb.state.push(v)
                        break


                    case Constants.OP_swap:

                        bb.state.checkStack(2,2)
                        v1 = bb.state.getStackValue(0)
                        v2 = bb.state.getStackValue(1)
                        //coder->write(state, pc, opcode)
                        bb.state.pop(2)
                        bb.state.push(v1)
                        bb.state.push(v2)
                        break


                    case Constants.OP_lessthan:
                    case Constants.OP_greaterthan:
                    case Constants.OP_lessequals:
                    case Constants.OP_greaterequals:

                        // if either the LHS or RHS is a number type, then we know
                        // it will be a numeric comparison.
                        //Value& rhs = state->peek(1)
                        //Value& lhs = state->peek(2)
                        //Traits *lhst = lhs.traits
                        //Traits *rhst = rhs.traits
                        //if (rhst && rhst->isNumeric() && lhst && !lhst->isNumeric())

                        // convert lhs to Number
                        //emitCoerce(AVMType.NUMBER, state->sp()-1)

                        //else if (lhst && lhst->isNumeric() && rhst && !rhst->isNumeric())

                        // promote rhs to Number
                        //emitCoerce(AVMType.NUMBER, state->sp())

                        //coder->write(state, pc, opcode, AVMType.BOOL)
                        bb.state.pop_push(2, AVMType.BOOL)
                        break


                    case Constants.OP_equals:
                    case Constants.OP_strictequals:
                    case Constants.OP_instanceof:
                    case Constants.OP_in:
                        bb.state.checkStack(2,1)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.BOOL)
                        break

                    case Constants.OP_not:
                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.BOOL, sp)
                        //coder->write(state, pc, opcode, AVMType.BOOL)
                        bb.state.pop_push(1, AVMType.BOOL)
                        break

                    case Constants.OP_add:

                        bb.state.checkStack(2,1)
                        
                        //Value& rhs = state->peek(1)
                        //Value& lhs = state->peek(2)
                        //Traits* lhst = lhs.traits
                        //Traits* rhst = rhs.traits
                        //if ((lhst == AVMType.STRING && lhs.notNull) || (rhst == AVMType.STRING && rhs.notNull))

                        //coder->write(state, pc, OP_add, AVMType.STRING)
                        //bb.state.pop_push(2, AVMType.STRING, true)

                        //else if (lhst && lhst->isNumeric() && rhst && rhst->isNumeric())

                        //coder->write(state, pc, OP_add, AVMType.NUMBER)
                        //bb.state.pop_push(2, AVMType.NUMBER)

                        //else

                        //coder->write(state, pc, OP_add, OBJECT_TYPE)
                        // NOTE don't know if it will return number or string, but
                        // neither will be null
                        //bb.state.pop_push(2, AVMType.OBJECT, true)


                        v2 = bb.state.getStackValue(0);
                        v1 = bb.state.getStackValue(1);
                        result = AVMType.NULL;
                        if ((v1 == AVMType.STRING && v1.notNull) || (v2 == AVMType.STRING && v2.notNull))
                            result = AVMType.STRING;
                        else if (v1.isNumeric && v2.isNumeric)
                            result = AVMType.NUMBER;
                        else
                            result = AVMType.OBJECT;
                        
                        bb.state.pop_push(2, result, true)

                        break


                    case Constants.OP_modulo:
                    case Constants.OP_subtract:
                    case Constants.OP_divide:
                    case Constants.OP_multiply:
                        bb.state.checkStack(2,1)
                        //emitCoerce(AVMType.NUMBER, sp-1)
                        //emitCoerce(AVMType.NUMBER, sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.NUMBER)
                        break

                    case Constants.OP_negate:
                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.NUMBER, sp)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_increment:
                    case Constants.OP_decrement:
                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.NUMBER, sp)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_increment_i:
                    case Constants.OP_decrement_i:
                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_add_i:
                    case Constants.OP_subtract_i:
                    case Constants.OP_multiply_i:
                        bb.state.checkStack(2,1)
                        //emitCoerce(AVMType.INT, sp-1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.INT)
                        break

                    case Constants.OP_negate_i:
                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_bitand:
                    case Constants.OP_bitor:
                    case Constants.OP_bitxor:
                        bb.state.checkStack(2,1)
                        //emitCoerce(AVMType.INT, sp-1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.INT)
                        break

                        // ISSUE do we care if shift amount is signed or not?  we mask
                        // the result so maybe it doesn't matter.
                        // CN says see tests e11.7.2, 11.7.3, 9.6
                    case Constants.OP_lshift:
                    case Constants.OP_rshift:
                        bb.state.checkStack(2,1)
                        //emitCoerce(AVMType.INT, sp-1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.INT)
                        break

                    case Constants.OP_urshift:
                        bb.state.checkStack(2,1)
                        //emitCoerce(AVMType.INT, sp-1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.UINT)
                        break

                    case Constants.OP_bitnot:
                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_typeof:
                        bb.state.checkStack(1,1)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(1, AVMType.STRING, true)
                        break

                    case Constants.OP_nop:
                    case Constants.OP_bkpt:
                    case Constants.OP_bkptline:
                    case Constants.OP_timestamp:
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_debug:
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_label:
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_debugline:
                        //coder->write(state, pc, opcode)
                        break

                    case Constants.OP_nextvalue:
                    case Constants.OP_nextname:
                        bb.state.checkStack(2,1)
                        //peekType(AVMType.INT, 1)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.NULL)
                        break

                    case Constants.OP_hasnext:
                        bb.state.checkStack(2,1)
                        //peekType(AVMType.INT,1)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(2, AVMType.INT)
                        break

                    case Constants.OP_hasnext2:

                        bb.state.checkStack(0,1)
                        bb.state.checkLocal(cur.getImm(0))
                        v = bb.state.checkLocal(cur.getImm(0))
                        //if (imm30 == imm30b)
                        //verifyFailed(kInvalidHasNextError)
                        //if (v.traits != AVMType.INT)
                        //verifyFailed(kIllegalOperandTypeError, core->toErrorString(v.traits), core->toErrorString(AVMType.INT))
                        //coder->write(state, pc, opcode)
                        //state->setType(imm30, NULL, false)
                        bb.state.push(AVMType.BOOL)
                        break


                        // sign extends
                    case Constants.OP_sxi1:
                    case Constants.OP_sxi8:
                    case Constants.OP_sxi16:
                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(1, AVMType.INT)
                        break

                        // loads
                    case Constants.OP_li8:
                    case Constants.OP_li16:
                        //if (pc+1 < code_end &&
                        //((opcode == OP_li8 && pc[1] == OP_sxi8) || (opcode == OP_li16 && pc[1] == OP_sxi16)))

                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, (opcode == OP_li8) ? OP_lix8 : OP_lix16)
                        bb.state.pop_push(1, AVMType.INT)
                        // ++pc // do not skip the sign-extend if it's the target
                        // of an implicit label, skipping it would cause verification failure.
                        // instead, just emit it, and rely on LIR to ignore sxi instructions
                        // in these situations.
                        break

                        // else fall thru
                    case Constants.OP_li32:
                    case Constants.OP_lf32:
                    case Constants.OP_lf64:

                        result = (cur.getOpcode() == Constants.OP_lf32 || cur.getOpcode() == Constants.OP_lf64) ? AVMType.NUMBER : AVMType.INT
                        bb.state.checkStack(1,1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop_push(1, result)
                        break


                        // stores
                    case Constants.OP_si8:
                    case Constants.OP_si16:
                    case Constants.OP_si32:
                    case Constants.OP_sf32:
                    case Constants.OP_sf64:
                        bb.state.checkStack(2,0)
                        //emitCoerce((opcode == OP_sf32 || opcode == OP_sf64) ? AVMType.NUMBER : AVMType.INT, sp-1)
                        //emitCoerce(AVMType.INT, sp)
                        //coder->write(state, pc, opcode)
                        bb.state.pop(2)
                        break
                    
                    default:
                        bb.state.verifyError = "unhandled opcode: " + Constants.opNames[cur.getOpcode()]
                }
                
                cur.setState(bb.state)

                // Stop verifying this block after we hit the first instruction
                // that causes a verify error
                if(bb.state.verifyError != null) {
                    return;
                }
            }
            
            // Reaching here means that the block was successfully verified.
            // Propagate the final framestate to any successor blocks, if any
            // successor block already has framestate then we compare ours to
            // ensure it matches. If it does not match then the successor block
            // fails verification.
            for each(var b:BasicBlock in bb.getSuccs()) {
                if(b.propagateFrameState(bb.state)) {
                    verifyBlock(b, mb)
                }
            }
        }
    }
}