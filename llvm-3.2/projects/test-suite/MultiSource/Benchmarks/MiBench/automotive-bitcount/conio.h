/* +++Date last modified: 05-Jul-1997 */

/*
**  UNXCONIO.H - Port crucial DOS|Win|OS/2 non-blocking console I/O
**               functions to Unix/Posix.
**
**  public domain SNIPPETS header for use with Steve Poole's TERM_OPT.C
*/

#ifndef UNXCONIO__H
#define UNXCONIO__H

#include <stdio.h>
#include <unistd.h>

#define echo_on()  term_option(0)
#define echo_off() term_option(1)

int term_option();
int getch();

#endif /* UNXCONIO__H */
