// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define GEN_SYNC_1(W, T, N, OP) \
T __sync_##N##_##W(T *ptr, T n);

#define GEN_SYNC_0(T, N, OP) \
T __sync_##N(T *ptr, T n);

#define GEN_SYNC(N, OP) \
  GEN_SYNC_1(1, unsigned char, N, OP) \
  GEN_SYNC_1(2, unsigned short, N, OP) \
  GEN_SYNC_1(4, unsigned int , N, OP) \
  GEN_SYNC_0(unsigned int , N, OP)

GEN_SYNC(fetch_and_add, val = (result = cur) + n)
GEN_SYNC(fetch_and_sub, val = (result = cur) - n)
GEN_SYNC(fetch_and_or, val = (result = cur) | n)
GEN_SYNC(fetch_and_xor, val = (result = cur) ^ n)
GEN_SYNC(fetch_and_and, val = (result = cur) & n)
GEN_SYNC(fetch_and_nand, val = ~(result = cur) & n)

GEN_SYNC(add_and_fetch, result = val = cur + n)
GEN_SYNC(sub_and_fetch, result = val = cur - n)
GEN_SYNC(or_and_fetch, result = val = cur | n)
GEN_SYNC(xor_and_fetch, result = val = cur ^ n)
GEN_SYNC(and_and_fetch, result = val = cur & n)
GEN_SYNC(nand_and_fetch, result = val = ~cur & n)

unsigned int __sync_val_compare_and_swap(volatile unsigned int *ptr, unsigned int oldval, unsigned int newval);
bool __sync_bool_compare_and_swap(volatile unsigned int *ptr, unsigned int oldval, unsigned int newval);
