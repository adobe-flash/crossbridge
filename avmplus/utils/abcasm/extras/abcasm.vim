" * -*- Mode: vim; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
" /*
"  *  This Source Code Form is subject to the terms of the Mozilla Public
"  *  License, v. 2.0. If a copy of the MPL was not distributed with this
"  *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
"  */
" Vim syntax file
" Language:	Adobe ABC Assembler
" Maintainer:	Tom Harwood <tharwood@adobe.com>
" Last Change:	2009 Mar 3

" Usage: add the following (uncommented) line to filetype.vim, and
" put this syntax file in the syntax folder.

"au BufNewFile,BufRead *.abs			setf abcasm

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn case ignore

syn keyword asmDir .script_info
syn keyword asmDir .method_info
syn keyword asmDir .method_body_info
syn keyword asmDir .trait
syn keyword asmDir .exception

syn match asmLabel		"[a-z_][a-z0-9_]*:"he=e-1
syn match asmIdentifier		"[a-z_][a-z0-9_]*"

syn match decNumber		"0\+[1-7]\=[\t\n$,; ]"
syn match decNumber		"[1-9]\d*"
syn match hexNumber		"0[xX][0-9a-fA-F]\+"

syn match asmComment		"//.*"hs=s+1
syn region asmComment start=/\/\*/ end=/\*\//

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_abcasm_syntax_inits")
  if version < 508
    let did_abcasm_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink asmSection	Special
  HiLink asmLabel	Label
  HiLink asmComment	Comment
  HiLink asmDirective	Statement

  HiLink hexNumber	Number
  HiLink decNumber	Number
  HiLink octNumber	Number
  HiLink binNumber	Number

  HiLink asmSpecialComment Comment
  HiLink asmIdentifier Identifier
  HiLink asmType	Type

  delcommand HiLink
endif

let b:current_syntax = "abcasm"

