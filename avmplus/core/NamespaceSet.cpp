/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

namespace avmplus
{
    /*static*/ NamespaceSet* NamespaceSet::_create(MMgc::GC* gc, uint32_t count)
    {
        AvmAssert(count <= 0x7fffffff); // should be impossible since ABC only allow U30...
        size_t extra = (count >= 1 ? count-1 : 0)*sizeof(Namespacep);
        NamespaceSet* nsset = new (gc, MMgc::kExact, extra) NamespaceSet();
        nsset->_countAndFlags = count<<1;
        return nsset;
    }

    /*static*/ const NamespaceSet* NamespaceSet::create(MMgc::GC* gc, Namespacep ns)
    {
        NamespaceSet* nsset = new (gc, MMgc::kExact) NamespaceSet();
        nsset->_countAndFlags = (1<<1) | (ns->isPublic() ? 1 : 0);
        nsset->_namespaces[0] = ns;
        return nsset;
    }

    PrintWriter& NamespaceSet::print(PrintWriter& prw) const
    {
        prw << "{";
        bool comma = false;
        for (NamespaceSetIterator iter(this); iter.hasNext();)
        {
            if (comma)
                prw << ",";
            Namespacep ns = iter.next();
            (ns->isPublic()) ? prw << "public"
                             : prw << ns->getURI();
            comma = true;
        }
        return prw << "}";
    }
}
