/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_TraitsIterator__
#define __avmplus_TraitsIterator__

namespace avmplus
{
    class TraitsIterator
    {
    public:
        TraitsIterator(Traits * _traits) :
            traits(_traits ? _traits->getTraitsBindings() : NULL),
            index(0)
        {
        }

        bool getNext(Stringp& key, Namespacep& ns, Binding& value)
        {
            if (index == -1 || !traits)
            {
                return false;
            }
            while ((index = traits->next(index)) == 0)
            {
                // ascend to base traits
                traits = traits->base;
                if (!traits)
                {
                    // no more traits, bail out.
                    index = -1;
                    return false;
                }
                // restart iteration on base traits
                index = 0;
            }
            key   = traits->keyAt(index);
            ns    = traits->nsAt(index);
            value = traits->valueAt(index);
            return true;
        }

        Traits* currentTraits()
        {
            // this value changes as we walk up the traits chain
            if (traits) return traits->owner;
            return NULL;
        }

    private:
        // this is stack-allocated so we must not use GCMember<>
        TraitsBindingsp traits;
        int index;
    };
}

#endif /* __avmplus_TraitsIterator__ */
