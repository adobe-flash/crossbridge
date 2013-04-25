/*
** Copyright (c) 2013 Adobe Systems Inc

** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:

** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
*/

enum avm2_address_seg { SEG_DEFAULT, SEG_FS, SEG_GS };

struct avm2_address
{
  rtx base, index, disp;
  HOST_WIDE_INT scale;
  enum avm2_address_seg seg;
};

extern void init_cumulative_args (CUMULATIVE_ARGS *, tree, rtx, tree);
extern bool legitimate_constant_p (rtx);
extern int symbolic_reference_mentioned_p (rtx);
extern int legitimate_address_p (enum machine_mode, rtx, int);
extern int avm2_decompose_address (rtx, struct avm2_address *);
extern int avm2_return_pops_args (tree, tree, int);
extern rtx function_arg (CUMULATIVE_ARGS *, enum machine_mode, tree, int);

