#!/usr/bin/env python
# -*- Mode: Python; indent-tabs-mode: nil -*-
# vi: set ts=4 sw=4 expandtab:

# borrowed from http://gist.github.com/240957
# no license information found

# Support for comments ; added by Adobe.

from string import whitespace

atom_end = set('()"\'') | set(whitespace)
def parse(sexp):
    stack, i, length = [[]], 0, len(sexp)
    while i < length:
        c = sexp[i]

        #print c, stack
        if c == ';':
          while i + 1 < length and sexp[i + 1] != '\n':
            i += 1
        else:
          reading = type(stack[-1])
          if reading == list:
              if c == '(': stack.append([])
              elif c == ')':
                  stack[-2].append(stack.pop())
                  if stack[-1][0] == ('quote',): stack[-2].append(stack.pop())
              elif c == '"': stack.append('')
              elif c == "'": stack.append([('quote',)])
              elif c in whitespace: pass
              else: stack.append((c,))
          elif reading == str:
              if c == '"':
                  stack[-2].append(stack.pop())
                  if stack[-1][0] == ('quote',): stack[-2].append(stack.pop())
              elif c == '\\':
                  i += 1
                  stack[-1] += sexp[i]
              else: stack[-1] += c
          elif reading == tuple:
              if c in atom_end:
                  atom = stack.pop()
                  if atom[0][0].isdigit(): stack[-1].append(eval(atom[0]))
                  else: stack[-1].append(atom[0])
                  if stack[-1][0] == ('quote',): stack[-2].append(stack.pop())
                  continue
              else: stack[-1] = ((stack[-1][0] + c),)
        i += 1
    return stack.pop()
