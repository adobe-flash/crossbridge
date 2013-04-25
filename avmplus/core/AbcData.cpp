/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include "BuiltinNatives.h"

#include "builtin.cpp"

namespace avmplus
{
    namespace NativeID
    {
#ifdef DEBUG
        static Binding getBinding(Traits* t, int nameId, const TraitsBindings*& tb)
        {
            Multiname name;
            t->pool->parseMultiname(name, nameId);
            if (name.isNsset())
                name.setNamespace(name.getNsset()->nsAt(0));

            AvmAssert(!name.isNsset());

            tb = t->getTraitsBindings();
            return tb->findBinding(name.getName(), name.getNamespace());
        }
        
        uint32_t SlotOffsetsAndAsserts::getSlotOffset(Traits* t, int nameId)
        {
            const TraitsBindings* tb;
            Binding b = getBinding(t, nameId, tb);

            AvmAssert(AvmCore::isSlotBinding(b));
            int slotId = AvmCore::bindingToSlotId(b);
            return tb->getSlotOffset(slotId);
        }

        uint32_t SlotOffsetsAndAsserts::getMethodIndex(Traits* t, int nameId)
        {
            const TraitsBindings* tb;
            Binding b = getBinding(t, nameId, tb);

            AvmAssert(AvmCore::isMethodBinding(b));
            return AvmCore::bindingToMethodId(b);
        }

        uint32_t SlotOffsetsAndAsserts::getGetterIndex(Traits* t, int nameId)
        {
            const TraitsBindings* tb;
            Binding b = getBinding(t, nameId, tb);

            AvmAssert(AvmCore::hasGetterBinding(b));
            return AvmCore::bindingToGetterId(b);
        }

        uint32_t SlotOffsetsAndAsserts::getSetterIndex(Traits* t, int nameId)
        {
            const TraitsBindings* tb;
            Binding b = getBinding(t, nameId, tb);

            AvmAssert(AvmCore::hasSetterBinding(b));
            return AvmCore::bindingToSetterId(b);
        }
#endif
    }
}
