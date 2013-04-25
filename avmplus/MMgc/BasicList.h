/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __BasicList__
#define __BasicList__

namespace MMgc
{
    template<typename T, int growthIncrement=4>
    class BasicList
    {
    public:
        BasicList() : count(0),
                      capacity(0),
                      items(NULL),
                      iteratorCount(0),
                      holes(false),
                      cursor(0)
        {
        }

        ~BasicList()
        {
            Destroy();
        }

        void Destroy()
        {
            if ( items )
            {
                VMPI_free(items);
                items = NULL;
            }
            count = capacity = iteratorCount = 0;
            holes = false;
            cursor = 0;
        }

        bool Add(T item);

        void Remove(T item)
        {
            if (holes && iteratorCount == 0)
                Compact();
            uint32_t i=0;
            while (i < Limit() && items[i] != item)
                i++;
            if (i == Limit())
            {
                GCAssertMsg(false, "Bug: should not try to remove something that's not in the list");
                return;
            }

            //  If the item we deleted occured is our cursor, set our cursor to the next item
            if ( i == cursor)
            {
                do {
                    cursor ++;
                }while (cursor < capacity && items[cursor] == NULL);

                //  If our "next item" is the last item, then reset the list to the front.
                if (cursor == capacity)
                {
                    cursor = 0;
                }
            }

            items[i] = NULL;
            count--;
            if (i != count)
                holes = true;
        }

        T Get(uint32_t i) const
        {
            GCAssertMsg(i < Limit(), "Index out of bounds");
            return items[i];
        }

        //   Call "SetCursor" on the list to have future iterators begin iterating from that position in the list.
        //   The iterator will still iterate through all items in the list once by wrapping to the front when the end is hit.
        void SetCursor( uint32_t newCursor ) { cursor = newCursor;}
        uint32_t GetCursor ( ){ return cursor;}
        uint32_t Count() const { return count; }

        // iterator needs this to give complete iteration in the face of in-flight mutation
        uint32_t Limit() const { return holes ? capacity : count; }

        void IteratorAttach() { iteratorCount++; }
        void IteratorDettach()
        {
            iteratorCount--;
            if (holes && iteratorCount == 0)
                Compact();
        }

    protected:
        // no impl
        BasicList(const BasicList& other);
        BasicList& operator=(const BasicList& other);

    private:

        void Compact()
        {
            // i iterates through destintation slots and j iterates through source slots
            // j skips over holes i doesn't
            // at the end i = count
            uint32_t i=0,j=1;
            while (j < capacity)
            {
                if (items[i] == NULL)
                {
                    if (items[j] != NULL)
                    {
                        if (cursor == j) cursor = i;
                        items[i++] = items[j++];
                        items[j-1] = NULL; // NULL it so next item goes in this hole
                    }
                    else
                    {
                        j++;
                    }
                }
                else
                {
                    i++,j++;
                }
            }
            GCAssert(i == count);
            holes = false;
        }

        uint32_t count, capacity;
        T *items;
        uint32_t iteratorCount;
        bool holes;
        uint32_t cursor;

    };

    //  The BasicListIterator will iterate through items in the BasicList starting with the List's "cursor position"
    //  if "SetCursor" is never called on the BasicList passed into this BasicListIterator, then the list will iterate starting
    //  at position 0.  The iterator will still iterate through all items in the list once by wrapping to the front when the end is hit.
    template<typename T>
    class BasicListIterator
    {
    public:
        BasicListIterator(BasicList<T>& bl) : bl(bl), bListEnd(false)
        {
            index = bl.GetCursor();
            bl.IteratorAttach();

        }
        ~BasicListIterator()
        {
            bl.IteratorDettach();
        }
        T next()
        {
            if (bListEnd) return NULL;

            T t = NULL;
            if (index >= bl.GetCursor())
            {
                // iterate over holes until end
                while (index < bl.Limit() && t == NULL)
                {
                    t = bl.Get(index++);
                }

                if (index == bl.Limit() && bl.GetCursor() != 0)
                {
                    index = 0;
                }
            }
            else
            {
                while(index < bl.GetCursor() && t == NULL)
                {
                    t = bl.Get(index++);
                }
                if (index == bl.GetCursor())
                {
                    bListEnd = true;
                }
            }
            return t;
        }
        void MarkCursorInList()
        {
            while (index < bl.Limit() && bl.Get(index) == NULL)
            {
                index ++;
            }
            bl.SetCursor( (index < bl.Limit()) ? index : 0);
        }

    private:
        uint32_t index;
        BasicList<T> &bl;
        bool bListEnd;
    };
}

#endif /* __GCStack__ */
