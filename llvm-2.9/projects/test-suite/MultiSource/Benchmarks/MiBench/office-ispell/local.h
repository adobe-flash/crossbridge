/*
 * $Id: local.h 33046 2007-01-09 23:57:19Z lattner $
 */

/*
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
 */

/*
 * This file is a sample local.h file.  It shows what I believe nearly every
 * site will want to include in their local.h.  You will probably want to
 * expand this file;  see "config.X" to learn of #defines that you might
 * like to add to.
 */

/*
 * $Log$
 * Revision 1.1  2007/01/09 23:57:18  lattner
 * initial recheckin of mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:59:02  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.14  1995/01/08  23:23:56  geoff
 * Do some minor clarification of the instructional comments.
 *
 * Revision 1.13  1994/05/17  06:37:25  geoff
 * Add one more item of warning advice to the comments.
 *
 * Revision 1.12  1994/02/07  06:00:00  geoff
 * Add a warning about shell processing restrictions
 *
 * Revision 1.11  1994/01/25  07:11:50  geoff
 * Get rid of all old RCS log lines in preparation for the 3.1 release.
 *
 */

/*
 * WARNING WARNING WARNING
 *
 * This file is *NOT* a normal C header file!  Although it uses C
 * syntax and is included in C programs, it is also processed by shell
 * scripts that are very stupid about format.
 *
 * Do not try to use #if constructs to configure this file for more
 * than one configuration.  Do not place whitespace after the "#" in
 * "#define".  Do not attempt to disable lines by commenting them out.
 * Do not use backslashes to reduce the length of long lines.
 * None of these things will work the way you expect them to.
 *
 * WARNING WARNING WARNING
 */

#define MINIMENU	/* Display a mini-menu at the bottom of the screen */
#define NO8BIT		/* Remove this if you use ISO character sets */
#undef USG		/* Define this on System V */

/*
 * Important directory paths
 */
#define BINDIR	"/usr/local/bin"
#define LIBDIR	"."
#define ELISPDIR "/usr/local/lib/emacs/site-lisp"
#define TEXINFODIR "/usr/local/info"
#define MAN1DIR	"/usr/local/man/man1"
#define MAN4DIR	"/usr/local/man/man4"

/*
 * Place any locally-required #include statements here
 */
