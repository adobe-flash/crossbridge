// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 637993: rehashing a GCHashtable in the midst of iteration
// is unsound; here we check that we are guarding against it.

%%component mmgc
%%category bugzilla_637993

%%decls

const static size_t elem_count = 1000;
int32_t *elems;
MMgc::GCHashtable m_table;

void add_first_half() {
    for (size_t i=0; i < elem_count/2; i++)
        m_table.put(&elems[i], &elems[i+1]);
}

void add_second_half() {
    for (size_t i=elem_count/2; i < elem_count; i++)
        m_table.put(&elems[i], &elems[i+1]);
}

%%test delete_during_iteration_okay_if_norehash
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        while (it.nextKey()) {
            m_table.remove(it.value(), /*allowrehash=*/false);
        }
    }
    m_table.clear();
    delete elems;
    %%verify true
         ;

%%explicit delete_during_iteration_asserts_if_rehash
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        while (it.nextKey()) {
            m_table.remove(it.value());
        }
    }
    m_table.clear();
    delete elems;
    %%verify false
         ;

// This test is a trivial success; it is meant to be compared against
// the cases that *fail* below, in order to make it clear what is
// wrong with the intentionally asserting cases.
%%test rehash_after_iteration_succeeds
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        it.nextKey();
        it.nextKey();
    }
    add_second_half(); // rule satisfied; (Iterator is out of scope).
    m_table.clear();
    delete elems;
    %%verify true
         ;

  // (This test should definitely assert.)
%%explicit rehash_during_iteration_assert_fails_1
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        it.nextKey();
        add_second_half(); // this is where we break the rule
        it.nextKey();
    }

    m_table.clear();
    delete elems;
    // we should never get here, the assertion should happen up above.
    %%verify false
          ;

// This test will assert even though the iteration is "done",
// because the rule is that we cannot modify the hashtable while
// any iterator is still "in scope"
%%explicit rehash_during_iteration_assert_fails_2
    elems = new int32_t[elem_count];
    add_first_half();
    {
        MMgc::GCHashtable::Iterator it(&m_table);
        it.nextKey();
        it.nextKey();
        add_second_half(); // this is where we break the rule
    }

    m_table.clear();
    delete elems;
    // we should never get here, the assertion should happen up above.
    %%verify false
          ;
