// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component avmplus
%%category basics

%%test unsigned_int

// Does right shift of unsigned quantities work?
%%verify (int)(~0U >> 1) > 0

%%test signed_int

// Does right shift of signed quantities work?
%%verify (-1 >> 1) == -1

// verify that the "latin1" literal string calls work properly for hi-bit latin1 chars
%%test equalsLatin1
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    bool equals = s->equalsLatin1("ev\xADident");
%%verify equals == true

%%test containsLatin1
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    bool found = s->containsLatin1("\xAD");
%%verify found == true

%%test indexOfLatin1
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    int index = s->indexOfLatin1("\xAD");
%%verify index == 2

%%test matchesLatin1
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    bool matches1 = s->matchesLatin1("\xADi", 2, 2);
%%verify matches1 == true

%%test matchesLatin1_caseless
    Stringp s = core->newConstantStringLatin1("ev\xADident");
    bool matches2 = s->matchesLatin1_caseless("\xADIDENT", 2, 2);
%%verify matches2 == true


%%test bug562101
// XMLParser omits the last char of a DOCTYPE node
Stringp str = core->newConstantStringLatin1("<?xml version=\"1.0\"?><!DOCTYPE greeting SYSTEM><greeting>Hello, world!</greeting>");
XMLParser parser(core, str);
MMgc::GC *gc = core->GetGC();
XMLTag tag(gc);
int m_status;
bool pass = false;
while ((m_status = parser.getNext(tag)) == XMLParser::kNoError)
{
    switch (tag.nodeType)
    {
        case XMLTag::kDocTypeDeclaration:
        {
            pass = false;
            pass = tag.text->equalsLatin1("<!DOCTYPE greeting SYSTEM>");
        }
    break;
    }
}
%%verify pass == true

    // FIXME: this needs a "register this object with the GC" mechanism; this abuse of the GCRoot mechanism
    // is no longer allowed
//%%test bug610022
//    Stringp str = core->newConstantStringLatin1("some string that is likely to be unique");
//    WeakRefList<String> list(core->GetGC(), 0);
    // We are going to skip scanning the stack (so that "str" won't hold the string in place)
    // but that means we need a root to ensure that "list" doesn't also get collected.
    //
    //MMgc::GCRoot root(core->GetGC(), &list, sizeof(list));
    //list.add(str);
    //str = NULL;
    //core->GetGC()->Collect(/*scanStack*/false);
    //int removed = list.removeCollectedItems();
    //int count = list.length();
//%%verify removed == 1 && count == 0


