/*
 * $Id: msgs.h 33046 2007-01-09 23:57:19Z lattner $
 *
 * Copyright 1992, 1993, Geoff Kuenning, Granada Hills, CA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All modifications to the source code must be clearly marked as
 *    such.  Binary redistributions based on modified source code
 *    must be clearly marked as modified versions in the documentation
 *    and/or other materials provided with the distribution.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 *      This product includes software developed by Geoff Kuenning and
 *      other unpaid contributors.
 * 5. The name of Geoff Kuenning may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GEOFF KUENNING AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL GEOFF KUENNING OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * Messages header file.
 *
 * This file contains all text strings that are written by any of the
 * C programs in the ispell package.  The strings are collected here so that
 * you can have the option of translating them into your local language for
 * the benefit of your users.
 *
 * Anyone who goes to the effort of making a translation may wish to return
 * the translated strings to me, geoff@ITcorp.com, so that I can include
 * them in a later distribution under #ifdef control.
 *
 * Besides the strings in this header file, you may also want to translate
 * the strings in version.h, which give the version and copyright information.
 * However, any translation of these strings MUST accurately preserve the
 * legal rights under international law;  you may wish to consult a lawyer
 * about this since you will be responsible for the results of any
 * incorrect translation.
 *
 * Most of the strings below are simple printf format strings.  If the printf
 * takes more than one parameter, the string is given as a parameterized
 * macro in case your local language needs a different word order.
 */

/*
 * $Log$
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:59:03  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.31  1994/12/27  23:08:57  geoff
 * Add a message to be issued if a word contains illegal characters.
 *
 * Revision 1.30  1994/10/25  05:46:40  geoff
 * Improve a couple of error messages relating to affix flags.
 *
 * Revision 1.29  1994/10/04  03:46:23  geoff
 * Add a missing carriage return in the help message
 *
 * Revision 1.28  1994/09/16  05:07:00  geoff
 * Add the BAD_FLAG message, and start a sentence in another message with
 * an uppercase letter.
 *
 * Revision 1.27  1994/07/28  05:11:38  geoff
 * Log message for previous revision: add BHASH_C_ZERO_COUNT.
 *
 * Revision 1.26  1994/07/28  04:53:49  geoff
 *
 * Revision 1.25  1994/05/24  04:54:36  geoff
 * Add error messages for affix-flag checking.
 *
 * Revision 1.24  1994/01/25  07:12:42  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

/*
 * The following strings are used in numerous places:
 */
#define BAD_FLAG	"\r\nIllegal affix flag character '%c'\r\n"
#define CANT_OPEN	"Can't open %s\r\n"
#define CANT_CREATE	"Can't create %s\r\n"
#define WORD_TOO_LONG(w) "\r\nWord '%s' too long at line %d of %s, truncated\r\n", \
			  w, __LINE__, __FILE__

/*
 * The following strings are used in buildhash.c:
 */
#define BHASH_C_NO_DICT		"No dictionary (%s)\n"
#define BHASH_C_NO_COUNT	"No count file\n"
#define BHASH_C_BAD_COUNT	"Bad count file\n"
#define BHASH_C_ZERO_COUNT	"No words in dictionary\n"
    /* I think this message looks better when it's nearly 80 characters wide,
     * thus the ugly formatting in the next two defines.  GK 9-87 */
#define BHASH_C_BAFF_1(max, excess) \
  "    Warning:  this language table may exceed the maximum total affix length\nof %d by up to %d bytes.  You should either increase MAXAFFIXLEN in config.X\nor shorten your largest affix/strip string difference.  (This is the\n", \
				  max, excess
#define BHASH_C_BAFF_2 \
  "difference between the affix length and the strip length in a given\nreplacement rule, or the affix length if there is no strip string\nin that rule.)\n"
#define BHASH_C_OVERFLOW	"Hash table overflowed by %d words\n"
#define BHASH_C_CANT_OPEN_DICT "Can't open dictionary\n"
#define BHASH_C_NO_SPACE	"Couldn't allocate hash table\n"
#define BHASH_C_COLLISION_SPACE "\ncouldn't allocate space for collision\n"
#define BHASH_C_COUNTING	"Counting words in dictionary ...\n"
#define BHASH_C_WORD_COUNT	"\n%d words\n"
#define BHASH_C_USAGE		"Usage:  buildhash [-s] dict-file aff-file hash-file\n\tbuildhash -c count aff-file\n"

/*
 * The following strings are used in correct.c:
 */
#define CORR_C_HELP_1		"Whenever a word is found that is not in the dictionary,\r\n"
#define CORR_C_HELP_2		"it is printed on the first line of the screen.  If the dictionary\r\n"
#define CORR_C_HELP_3		"contains any similar words, they are listed with a number\r\n"
#define CORR_C_HELP_4		"next to each one.  You have the option of replacing the word\r\n"
#define CORR_C_HELP_5		"completely, or choosing one of the suggested words.\r\n"
    /* You may add HELP_6 through HELP_9 if your language needs more lines */
#define CORR_C_HELP_6		""
#define CORR_C_HELP_7		""
#define CORR_C_HELP_8		""
#define CORR_C_HELP_9		""
#define CORR_C_HELP_COMMANDS	"\r\nCommands are:\r\n\r\n"
#define CORR_C_HELP_R_CMD	"R       Replace the misspelled word completely.\r\n"
#define CORR_C_HELP_BLANK	"Space   Accept the word this time only.\r\n"
#define CORR_C_HELP_A_CMD	"A       Accept the word for the rest of this session.\r\n"
#define CORR_C_HELP_I_CMD	"I       Accept the word, and put it in your private dictionary.\r\n"
#define CORR_C_HELP_U_CMD	"U       Accept and add lowercase version to private dictionary.\r\n"
#define CORR_C_HELP_0_CMD	"0-n     Replace with one of the suggested words.\r\n"
#define CORR_C_HELP_L_CMD	"L       Look up words in system dictionary.\r\n"
#define CORR_C_HELP_X_CMD	"X       Write the rest of this file, ignoring misspellings,\r\n        and start next file.\r\n"
#define CORR_C_HELP_Q_CMD	"Q       Quit immediately.  Asks for confirmation.\r\n        Leaves file unchanged.\r\n"
#define CORR_C_HELP_BANG	"!       Shell escape.\r\n"
#define CORR_C_HELP_REDRAW	"^L      Redraw screen.\r\n"
#define CORR_C_HELP_SUSPEND	"^Z      Suspend program.\r\n"
#define CORR_C_HELP_HELP	"?       Show this help screen.\r\n"
#define CORR_C_HELP_TYPE_SPACE	"-- Type space to continue --"

#define CORR_C_FILE_LABEL	"              File: %s"
#define CORR_C_READONLY		"[READONLY]"
#define CORR_C_MINI_MENU	"[SP] <number> R)epl A)ccept I)nsert L)ookup U)ncap Q)uit e(X)it or ? for help\r\n"
#define CORR_C_CONFIRM_QUIT	"Are you sure you want to throw away your changes? "
#define CORR_C_REPLACE_WITH	"Replace with: "
#define CORR_C_LOOKUP_PROMPT	"Lookup string ('*' is wildcard): "
#define CORR_C_MORE_PROMPT	"-- more --"
#define CORR_C_BLANK_MORE	"\r           \r"
#define CORR_C_END_LOOK		"--end--"

/*
 * The following strings are used in defmt.c:
 */
#define DEFMT_C_TEX_MATH_ERROR	"****ERROR in parsing TeX math mode!\r\n"
#define DEFMT_C_LR_MATH_ERROR	"***ERROR in LR to math-mode switch.\n"

/*
 * The following strings are used in icombine.c:
 */
#define ICOMBINE_C_BAD_TYPE	"icombine:  unrecognized formatter type '%s'\n"
#define ICOMBINE_C_USAGE	"Usage:  icombine [-T suffix] [aff-file] < wordlist\n"

/*
 * The following strings are used in ispell.c:
 */
#define ISPELL_C_USAGE1		"Usage: %s [-dfile | -pfile | -wchars | -Wn | -t | -n | -x | -b | -S | -B | -C | -P | -m | -Lcontext | -M | -N | -Ttype | -V] file .....\n"
#define ISPELL_C_USAGE2		"       %s [-dfile | -pfile | -wchars | -Wn | -t | -n | -Ttype] -l\n"
#ifndef USG
#define ISPELL_C_USAGE3		"       %s [-dfile | -pfile | -ffile | -Wn | -t | -n | -s | -B | -C | -P | -m | -Ttype] {-a | -A}\n"
#else
#define ISPELL_C_USAGE3		"       %s [-dfile | -pfile | -ffile | -Wn | -t | -n | -B | -C | -P | -m | -Ttype] {-a | -A}\n"
#endif
#define ISPELL_C_USAGE4		"       %s [-dfile] [-wchars | -Wn] -c\n"
#define ISPELL_C_USAGE5		"       %s [-dfile] [-wchars] -e[1-4]\n"
#define ISPELL_C_USAGE6		"       %s [-dfile] [-wchars] -D\n"
#define ISPELL_C_USAGE7		"       %s -v\n"
#define ISPELL_C_TEMP_DISAPPEARED "temporary file disappeared (%s)\r\n"
#define ISPELL_C_BAD_TYPE	"ispell:  unrecognized formatter type '%s'\n"
#define ISPELL_C_NO_FILE	"ispell:  specified file does not exist\n"
#define ISPELL_C_NO_FILES	"ispell:  specified files do not exist\n"
#define ISPELL_C_CANT_WRITE	"Warning:  Can't write to %s\r\n"
#define ISPELL_C_OPTIONS_ARE	"Compiled-in options:\n"

/*
 * The following strings are used in lookup.c:
 */
#define LOOKUP_C_CANT_READ	"Trouble reading hash table %s\r\n"
#define LOOKUP_C_NULL_HASH	"Null hash table %s\r\n"
#define LOOKUP_C_SHORT_HASH(name, gotten, wanted) \
				"Truncated hash table %s:  got %d bytes, expected %d\r\n", \
				  name, gotten, wanted
#define LOOKUP_C_BAD_MAGIC(name, wanted, gotten) \
				"Illegal format hash table %s - expected magic 0x%x, got 0x%x\r\n", \
				  name, wanted, gotten
#define LOOKUP_C_BAD_MAGIC2(name, wanted, gotten) \
				"Illegal format hash table %s - expected magic2 0x%x, got 0x%x\r\n", \
				  name, wanted, gotten
#define LOOKUP_C_BAD_OPTIONS(gotopts, gotchars, gotlen, wantedopts, wantedchars, wantedlen) \
				"Hash table options don't agree with buildhash - 0x%x/%d/%d vs. 0x%x/%d/%d\r\n", \
				  gotopts, gotchars, gotlen, \
				  wantedopts, wantedchars, wantedlen
#define LOOKUP_C_NO_HASH_SPACE	"Couldn't allocate space for hash table\r\n"
#define LOOKUP_C_BAD_FORMAT	"Illegal format hash table\r\n"
#define LOOKUP_C_NO_LANG_SPACE	"Couldn't allocate space for language tables\r\n"

/*
 * The following strings are used in makedent.c:
 */
#define MAKEDENT_C_NO_WORD_SPACE "\r\nCouldn't allocate space for word '%s'\r\n"
#define MAKEDENT_C_BAD_WORD_CHAR "\r\nWord '%s' contains illegal characters\r\n"

/*
 * The following strings are used in parse.y:
 */
#define PARSE_Y_8_BIT		"Eighth bit ignored (recompile ispell without NO8BIT)"
#define PARSE_Y_NO_WORD_STRINGS	"wordchars statement may not specify string characters"
#define PARSE_Y_UNMATCHED	"Unmatched charset lengths"
#define PARSE_Y_NO_BOUNDARY_STRINGS "boundarychars statement may not specify string characters"
#define PARSE_Y_LONG_STRING	"String character is too long"
#define PARSE_Y_NULL_STRING	"String character must have nonzero length"
#define PARSE_Y_MANY_STRINGS	"Too many string characters"
#define PARSE_Y_NO_SUCH_STRING	"No such string character"
#define PARSE_Y_MULTIPLE_STRINGS "Alternate string character was already defined"
#define PARSE_Y_LENGTH_MISMATCH	"Upper and lower versions of string character must be same length"
#define PARSE_Y_WRONG_NROFF	"Incorrect character count in nroffchars statement"
#define PARSE_Y_WRONG_TEX	"Incorrect character count in TeXchars statement"
#define PARSE_Y_DOUBLE_COMPOUND	"Compoundwords option may only appear once"
#define PARSE_Y_LONG_FLAG	"Flag must be single character"
#define PARSE_Y_BAD_FLAG	"Flag must be alphabetic"
#define PARSE_Y_DUP_FLAG	"Duplicate flag"
#define PARSE_Y_NO_SPACE	"Out of memory"
#define PARSE_Y_NEED_BLANK	"Single characters must be separated by a blank"
#define PARSE_Y_MANY_CONDS	"Too many conditions;  8 maximum"
#define PARSE_Y_EOF		"Unexpected EOF in quoted string"
#define PARSE_Y_LONG_QUOTE	"Quoted string too long, max 256 characters"
#define PARSE_Y_ERROR_FORMAT(file, lineno, error) \
				"%s line %d: %s\n", file, lineno, error
#define PARSE_Y_MALLOC_TROUBLE	"yyopen:  trouble allocating memory\n"
#define PARSE_Y_UNGRAB_PROBLEM	"Internal error:  ungrab buffer overflow"
#define PARSE_Y_BAD_DEFORMATTER	"Deformatter must be either 'nroff' or 'tex'"
#define PARSE_Y_BAD_NUMBER	"Illegal digit in number"

/*
 * The following strings are used in term.c:
 */
#define TERM_C_SMALL_SCREEN	"Screen too small:  need at least %d lines\n"
#define TERM_C_NO_BATCH		"Can't deal with non-interactive use yet.\n"
#define TERM_C_CANT_FORK	"Couldn't fork, try later.\r\n"
#define TERM_C_TYPE_SPACE	"\n-- Type space to continue --"

/*
 * The following strings are used in tree.c:
 */
#define TREE_C_CANT_UPDATE	"Warning: Cannot update personal dictionary (%s)\r\n"
#define TREE_C_NO_SPACE		"Ran out of space for personal dictionary\r\n"
#define TREE_C_TRY_ANYWAY	"Continuing anyway (with reduced performance).\r\n"

/*
 * The following strings are used in unsq.c:
 */
#define UNSQ_C_BAD_COUNT	"Illegal count character 0x%x\n"
#define UNSQ_C_SURPRISE_EOF	"Unexpected EOF\n"
