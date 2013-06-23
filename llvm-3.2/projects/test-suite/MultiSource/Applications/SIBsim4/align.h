/* $Id: align.h 34335 2007-02-15 23:46:09Z nicholas $
 *
 * Christian Iseli, LICR ITO, Christian.Iseli@licr.org
 *
 * Copyright (c) 2001-2004 Swiss Institute of Bioinformatics.
 * Copyright (C) 1998-2001  Liliana Florea.
 */
#ifndef SCRIPTLIB_H
#define SCRIPTLIB_H
extern void align_path(uchar *,uchar *,int,int,int,int,int,edit_script_p_t*,
	   edit_script_p_t*, int,int);
extern int  align_get_dist(uchar *,uchar *,int, int, int, int, int);
extern void Condense_both_Ends(edit_script_p_t *, edit_script_p_t *,
			       edit_script_p_t*);
extern void S2A(edit_script_p_t, int *, int);
extern void IDISPLAY(uchar *, uchar *, unsigned int, unsigned int, int *,
		     unsigned int, unsigned int, collec_p_t, int);
extern void Free_script(edit_script_p_t);

#endif /* SCRIPTLIB_H */
