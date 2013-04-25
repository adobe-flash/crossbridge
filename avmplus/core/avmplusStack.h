/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Stack__
#define __avmplus_Stack__


namespace avmplus
{
    /**
     * The Stack<T> template implements a simple stack, which can
     * be templated to support different types.
     */
    template <class T>
    class Stack
    {
    public:
        enum { kInitialCapacity = 128 };

        Stack()
        {
            data = new T[kInitialCapacity];
            len = 0;
            max = kInitialCapacity;
        }
        virtual ~Stack()
        {
            delete [] data;
        }
        void add(T value)
        {
            if (len >= max) {
                grow();
            }
            data[len++] = value;
        }
        bool isEmpty()
        {
            return len == 0;
        }
        T pop()
        {
            return data[--len];
        }
        void reset()
        {
            len = 0;
        }
        int size()
        {
            return len;
        }

    private:
        T *data;
        int len;
        int max;

        void grow()
        {
            int newMax = max * 5 / 4;
            T *newData = new T[newMax];
            for (int i=0; i<len; i++) {
                newData[i] = data[i];
            }
            delete [] data;
            data = newData;
            max = newMax;
        }
    };
}

#endif /* __avmplus_Stack__ */
