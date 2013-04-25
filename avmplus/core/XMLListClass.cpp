/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    XMLListClass::XMLListClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        AvmAssert(traits()->getSizeOfInstance() == sizeof(XMLListClass));
        createVanillaPrototype();
    }

    // E4X 10.4, pg 36
    Atom XMLListClass::ToXMLList(Atom arg)
    {
        AvmCore* core = this->core();

        if (AvmCore::isNullOrUndefined(arg))
        {
            toplevel()->throwTypeError(
                       (arg == undefinedAtom) ? kConvertUndefinedToObjectError :
                                            kConvertNullToObjectError);
            return arg;
        }

        if (AvmCore::isXMLList(arg))
        {
            return arg;
        }
        else if (AvmCore::isXML(arg))
        {
            XMLObject *x = AvmCore::atomToXMLObject(arg);
            Multiname m;
            bool bFound = x->getQName (&m);
            Atom parent = x->parent();
            if (parent == undefinedAtom)
                parent = nullObjectAtom;
            XMLListObject *xl = XMLListObject::create(core->GetGC(), toplevel()->xmlListClass(), parent, bFound ? &m : 0);

            xl->_append (arg);
            return xl->atom();
        }
        else
        {
            Toplevel* toplevel = this->toplevel();

            Stringp s = core->string(arg);

            if (s->matchesLatin1("<>", 2, 0) && s->matchesLatin1("</>", 3, s->length()-3))
                s = s->substr(2, s->length() - 5);

            Namespace *defaultNamespace = toplevel->getDefaultNamespace();
            // We handle this step in the XMLObject constructor to avoid concatenation huge strings together
            // parentString = <parent xnlns=defaultNamespace> s </parent>
            // 3. Parse parentString as a W3C element information info e
            // 4. If the parse fails, throw a SyntaxError exception
            // 5. x = toXML(e);
            //StringBuffer parentString (core);
            //parentString << "<parent xmlns=\"";
            //parentString << defaultNamespace->getURI();
            //parentString << "\">";
            //parentString << s;
            //parentString << "</parent>";
            XMLObject *x = XMLObject::create(core->GetGC(), toplevel->xmlClass(), s, defaultNamespace);

            XMLListObject *xl = XMLListObject::create(core->GetGC(), toplevel->xmlListClass());
            for (uint32_t i = 0; i < x->getNode()->_length(); i++)
            {
                E4XNode *c = x->getNode()->_getAt (i);
                c->setParent (NULL);

                // !!@ trying to emulate rhino behavior here
                // Add the default namespace to our top element.
                Namespace *ns = toplevel->getDefaultNamespace();
                c->_addInScopeNamespace (core, ns, core->findPublicNamespace());
                xl->_appendNode (c);
            }
            return xl->atom();
        }
    }

    // E4X 13.5.2, pg 87
    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom XMLListClass::construct(int argc, Atom* argv)
    {
        AvmCore* core = this->core();
        if ((!argc) || AvmCore::isNullOrUndefined(argv[1]))
        {
            return ToXMLList (core->kEmptyString->atom());
        }

        // if args[0] is xmllist, create new list and call append
        if (AvmCore::isXMLList(argv[1]))
        {
            XMLListObject *l = XMLListObject::create(core->GetGC(), toplevel()->xmlListClass());
            l->_append (argv[1]);
            return l->atom();
        }

        return ToXMLList(argv[1]);
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom XMLListClass::call(int argc, Atom* argv)
    {
        if ((!argc) || AvmCore::isNullOrUndefined(argv[1]))
        {
            return ToXMLList (core()->kEmptyString->atom());
        }

        return ToXMLList(argv[1]);
    }
}
