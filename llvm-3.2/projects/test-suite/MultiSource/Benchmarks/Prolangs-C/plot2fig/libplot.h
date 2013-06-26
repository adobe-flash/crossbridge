/* plot, unix plot to graphics device translators.
   Copyright (C) 1989 Free Software Foundation, Inc.

plot is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone for the
consequences of using it or for whether it serves any particular purpose or
works at all, unless he says so in writing.  Refer to the GNU General Public
License for full details.

Everyone is granted permission to copy, modify and redistribute plot, but
only under the conditions described in the GNU General Public License.  A copy
of this license is supposed to have been given to you along with plot so
you can know your rights and responsibilities.  It should be in a file named
COPYING.  Among other things, the copyright notice and this notice must be
preserved on all copies.  */

#ifndef _PLOT3_H
#define _PLOT3_H

/* This elides the argument prototypes if the compiler does not
   supprort them. The name protarg is chosen in hopes that it will not
   collide with any others. */
#ifdef __STDC__
#define protarg(a)	a
#else
#define protarg(a)	()
#endif

/* the standard library of plot functions */
extern int arc protarg ((int xc, int yc, int x0, int y0, int x1, int y1));
extern int circle protarg ((int x, int y, int r));
extern int closepl protarg (());
extern int cont protarg ((int x, int y));
extern int erase protarg (());
extern int label protarg ((char *s));
extern int line protarg ((int x0, int y0, int x1, int y1));
extern int linemod protarg ((char *s));
extern int move_nasko protarg ((int x, int y));
extern int openpl protarg (());
extern int point protarg ((int x, int y));
extern int space protarg ((int x0, int y0, int x1, int y1));

/* Extensions to the library for postscript */
extern int alabel protarg ((int x_justify, int y_justify, char *s));
extern int color protarg ((int red, int green, int blue));
extern int draw_line protarg (());
extern int fill protarg ((int level));
extern int fontname protarg ((char *s));
extern int fontsize protarg ((int size));
extern int rotate protarg ((int x, int y, int angle));

/* internally used routines */
extern int draw_line protarg (());

#endif /* _PLOT3_H */
