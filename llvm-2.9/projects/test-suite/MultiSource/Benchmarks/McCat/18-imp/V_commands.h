/*--------------------------------------------------------------------------
 * commands.h      V Includes file about the command table
 *
 * V
 * University of Illinois at Urbana-Champaign
 *
 * History:
 *          - Completed V1.0 August 1994
 *          - Modified for V2.0 October 1995 (cph)
 *
 * Id: V.h,v 1.0 1994/07/08 05:55:20 hjiang Exp hjiang $
 * (Included in V.h)
 * only the file with COMMAND_HOME allocates memory for the COMMAND table
 *-------------------------------------------------------------------------- */
#ifndef _commands_h_
#define _commands_h_

/* -----------------------------------------------------------------------
 * TO THE V PROGRAMMER:
 *
 * In this file (commands.h) all new V routines should be recorded for
 * incorporation into V.  To do this:
 * (1) Add routine prototypes for the desired platforms in Section 1,
 *     using existing functions as examples.
 * (2) Add the new V commands to the command table in Section 2 below,
 *     using the existing commands as examples
 * ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 * SECTION 1: ROUTINE PROTOTYPES
 * 
 * All new routines which are added to V should be prototyped to avoid
 * argument inconsistency and to avoid compilation errors.
 *
 * ----------------------------------------------------------------------- */
/* ------------------------- 
 * CALLING INTERFACES
 * ------------------------- */
extern int V_about    (int, char **, char *);
extern int V_abs      (int, char **, char *);
extern int V_add      (int, char **, char *);
extern int V_canny    (int, char **, char *);
extern int V_center   (int, char **, char *); 
extern int V_chunk    (int, char **, char *);
extern int V_conj     (int, char **, char *);
extern int V_create   (int, char **, char *);
extern int V_displ    (int, char **, char *);
extern int V_div      (int, char **, char *);
extern int V_dump2file(int, char **, char *); 
extern int V_dump2term(int, char **, char *);
extern int V_ft       (int, char **, char *);
extern int V_ft1d     (int, char **, char *);
extern int V_ft2d     (int, char **, char *);
extern int V_ft3d     (int, char **, char *);
extern int V_ft4d     (int, char **, char *);
extern int V_ftshift  (int, char **, char *);
extern int V_ftshift1d(int, char **, char *);
extern int V_ftshift2d(int, char **, char *);
extern int V_glue     (int, char **, char *);
extern int V_group    (int, char **, char *);
extern int V_gplot    (int, char **, char *);
extern int V_gplot2d  (int, char **, char *);
extern int V_gslim    (int, char **, char *);
extern int V_hftrec   (int, char **, char *); 
extern int V_history  (int, char **, char *);
extern int V_help     (int, char **, char *);
extern int V_hsvd     (int, char **, char *);
extern int V_i        (int, char **, char *);
extern int V_imag     (int, char **, char *);
extern int V_label    (int, char **, char *);
extern int V_linbrd   (int, char **, char *);
extern int V_linscl   (int, char **, char *);
extern int V_lpsvd    (int, char **, char *);
extern int V_man      (int, char **, char *);
extern int V_merge    (int, char **, char *);
extern int V_mirror   (int, char **, char *);
extern int V_movie    (int, char **, char *); 
extern int V_mult     (int, char **, char *);
extern int V_o        (int, char **, char *);
extern int V_pendpar  (int, char **, char *);
extern int V_phase    (int, char **, char *);
extern int V_pishft   (int, char **, char *);
extern int V_pishft1d (int, char **, char *);
extern int V_pishft2d (int, char **, char *);
extern int V_pishft3d (int, char **, char *);
extern int V_pishft4d (int, char **, char *);
extern int V_pop      (int, char **, char *);
extern int V_push     (int, char **, char *);
extern int V_ramp     (int, char **, char *);
extern int V_random   (int, char **, char *);
extern int V_real     (int, char **, char *);
extern int V_redim    (int, char **, char *);
extern int V_reorder  (int, char **, char *);
extern int V_replace  (int, char **, char *);
extern int V_rigr     (int, char **, char *);
extern int V_rmdc     (int, char **, char *);
extern int V_scale    (int, char **, char *);
extern int V_settype  (int, char **, char *);
extern int V_show_reg (int, char **, char *);
extern int V_slim     (int, char **, char *);
extern int V_slim_cond(int, char **, char *);
extern int V_slim_psf (int, char **, char *);
extern int V_split    (int, char **, char *);
extern int V_stack    (int, char **, char *);
extern int V_stats    (int, char **, char *);
extern int V_stuff    (int, char **, char *);
extern int V_sub      (int, char **, char *);
extern int V_swap     (int, char **, char *);
extern int V_synfid   (int, char **, char *);
extern int V_thresh   (int, char **, char *);
extern int V_trigr    (int, char **, char *);
extern int V_trunc    (int, char **, char *); 
extern int V_uniffill (int, char **, char *);
extern int V_unstack  (int, char **, char *);
extern int V_unwrap   (int, char **, char *);
extern int V_vpnls    (int, char **, char *);
extern int V_window1d (int, char **, char *);
extern int V_wt1d     (int, char **, char *);
extern int V_wt2d     (int, char **, char *);
extern int V_xchg     (int, char **, char *);
extern int V_xtrp     (int, char **, char *);
extern int V_zeropad  (int, char **, char *);
extern int V_zphase   (int, char **, char *);

/* ------------------------- 
 * IO ROUTINES
 * ------------------------- */
extern int L_dump_par(FILE *, char *, int, int *, float *, int);
extern int L_dump_data(FILE *, char *, int, int *, int, float *, int);
extern int L_gplot(V_struct *, V_struct *, int, int, 
		   int, int, double, double, int *);
extern int zoom1d(float *, int, float *, int);
extern int L_gplot2d(float *, float **, int, int *, int *, int *, 
		     int, int, float, float, int *);
extern int L_reorder(float *data, int data_type, int ndim, int *dim,
		     int *rdim, int *info);
extern int L_getsisco(char *filename, float **data, int *ndim,
		      int **dim, char *err);

/* ------------------------- 
 * LP ROUTINES
 * ------------------------- */
extern int L_hsvd(V_struct *, V_struct *, int, int, int,
		  float, float, char *);
extern int L_lpsvd(V_struct *, V_struct *, int, int, int,
		   float, float, char *);
extern int L_zphase(int, int *, float *);

/* ------------------------- 
 * MAIN ROUTINES
 * ------------------------- */
extern int title(void);
extern int init_path(char *);
extern void catchINT(void);
extern void catchFRE(void);
extern void catchBUS(void);
extern void catchSEGV(void);
extern void init_history(void);
extern void update_history(char *);
extern char *get_history(int);
extern int  GetPath(char *, char *, char *);
extern int call_cmd(char *, int, char **, char *);
extern int display_allcom(void);
extern int valid_com(char *);
extern int indexa(int, int *, int *);
extern void indices(int, int *, int, int *);
extern int nelements(int, int *);
extern int parse_input(char *, int *, char **, char *);
extern int parse_cmd(char *, int *, char *, char **, char *);
extern int print_err(char *);
extern int reg_create(char *);
extern int reg_pop(char *);
extern int reg_init(int, int, int, int *, char *);
extern int reg_push(char *);
extern int reg_redim(int, int, int, int *, char *);
extern int reg_label(int, char *, char *);
extern int reg_settype(int, int, char *);
extern int reg_load(int, float *, char *);
extern int reg_unload(int, float **, char *);
extern int reg_swap(int *, char *);
extern int reg_swapn(int *, int, char *);
extern int reg_count(void);

/* ------------------------- 
 * MATH ROUTINES
 * ------------------------- */
extern int L_hamming_filter(float *, int, int);
extern int L_hanning_filter(float *, int, int);
extern int L_bartlett_filter(float *, int, int);
extern int L_ramp_filter(float *, int, int, float);
extern int L_scale(float *, int, int *, float **, float *, int, char *);
extern int L_unwrap(float *, int, int *, int, float, char *);

/* ------------------------------
 * VARIABLE PROJECTION ROUTINES
 * ------------------------------ */
extern int L_vpnls(V_struct *, V_struct *, V_struct *, 
		   int, int, double, double, int, int, char *);
extern int is_1d_data(V_struct *);
extern int same_spatial_dim(V_struct *, V_struct *);

/* ------------------------- 
 * RIGR ROUTINES
 * ------------------------- */
extern int L_rigr(float *, int, float *, int, int, int, int, int, int,
		  float, int, int, float *, int *);
extern int L_hftrec(float **, float *, int, int *, int, int *, char *);

/* ------------------------- 
 * SLIM ROUTINES
 * ------------------------- */
extern int L_gslim(V_struct *, V_struct *, V_struct *, 
		   int, int, float, int, char *);
extern int L_slim(V_struct *, V_struct *, int, int, int, int, 
		  int *, int *, char *);
extern int L_slim_cond(V_struct *, V_struct *, int, int, char *);
extern int L_slim_psf(V_struct *, int, int*, char *);
extern int slim_csi (int,int*,int*,int,double*,int*,int*,double*,int,char*);
extern int slim_cond(int,int*,int*,int,int,int,double*,char*);
extern int slim_psf(int,int*,int*,int,int*,int*,double*,char*);
extern int Gmatrix  (int,int*,int*,int,int*,int*,double*);
extern int Gmatrix1d(int,int*,int*,int,int*,int*,double*);
extern int Gmatrix2d(int,int*,int*,int,int*,int*,double*);
extern int Gmatrix3d(int,int*,int*,int,int*,int*,double*);
extern double sinc(double);
extern int chunkgmatrix(double*,double*,int,int,int,int);
extern int topbit(int);

/* ------------------------- 
 * TRANSFORM ROUTINES
 * ------------------------- */
extern int L_ft1d(float *, int, int, int, int *, int, int, int *);
extern int L_canny(float, float *, int, int, float **, char *);
extern int L_wt1d(int kernel, float *wt, int n, int order, int type);
extern int L_wt2d(int kernel, float *wt, int n, int order, int type);
extern int L_ftshift1d(float *data, int ndim, int *dim);
extern int L_ftshift2d(float *data, int ndim, int *dim);

#endif


/* -----------------------------------------------------------------------
 * SECTION 2. COMMAND TABLE DEFINITION
 *
 * All new V routines should be added to this list of commands in order
 * for them to be incorporated.  Insert all new commands in the 
 * alphabetically correct positions, following the format of the other
 * commands, i.e. "commandname", V_routine.  The routine V_routine will
 * be called when the user calls "commandname".
 * 
 * ----------------------------------------------------------------------- */
#ifdef COMMAND_HOME

COMMAND cmdtable[] = {
  "about",        V_about,
  "abs",          V_abs,
  "add",          V_add,
  "cabs",         V_abs,
  "canny",        V_canny,
  "center",       V_center,
  "cft1d",        V_ft1d,
  "cft2d",        V_ft2d,
  "cft3d",        V_ft3d,
  "cft4d",        V_ft4d,
  "chunk",        V_chunk,
  "conj",         V_conj,
  "create",       V_create,
  "displ",        V_displ,
  "div",          V_div, 
  "dump2file",    V_dump2file,
  "dump2term",    V_dump2term,
  "ft",           V_ft,
  "ft1d",         V_ft1d,
  "ft2d",         V_ft2d,
  "ft3d",         V_ft3d,
  "ft4d",         V_ft4d,
  "ftshift",      V_ftshift,
  "ftshift1d",    V_ftshift1d,
  "ftshift2d",    V_ftshift2d,
  "glue",         V_glue,
  "gplot",        V_gplot,
  "gplot2d",      V_gplot2d,
  "group",        V_group,
  "gslim",        V_gslim,
  "h",            V_history,
  "help",         V_help,
  "hftrec",       V_hftrec,
  "history",      V_history,
  "hsvd",         V_hsvd,
  "i",            V_i,
  "imag",         V_imag,
  "label",        V_label,
  "linbrd",       V_linbrd,
  "linscl",       V_linscl,
  "lpsvd",        V_lpsvd,
  "man",          V_man,
  "merge",        V_merge,
  "mirror",       V_mirror,
  "movie",        V_movie,
  "mult",         V_mult,
  "o",            V_o,
  "pendpar",      V_pendpar,
  "phase",        V_phase,
  "pishft",       V_pishft,
  "pishft1d",     V_pishft1d, 
  "pishft2d",     V_pishft2d, 
  "pishft3d",     V_pishft3d, 
  "pishft4d",     V_pishft4d, 
  "pop",          V_pop,
  "print",        V_dump2term,
  "push",         V_push,
  "ramp",         V_ramp,
  "random",       V_random,
  "real",         V_real,
  "redim",        V_redim,
  "reorder",      V_reorder,
  "replace",      V_replace,
  "rigr",         V_rigr,
  "rmdc",         V_rmdc,
  "scale",        V_scale,
  "settype",      V_settype, 
  "show_reg",     V_show_reg, 
  "slim",         V_slim,
  "slim_cond",    V_slim_cond,
  "slim_psf",     V_slim_psf,
  "split",        V_split,
  "stack",        V_stack,
  "stats",        V_stats,
  "stuff",        V_stuff,
  "sub",          V_sub,
  "swap",         V_swap,
  "synfid",       V_synfid,
  "thresh",       V_thresh,
  "trigr",        V_trigr,
  "trunc",        V_trunc, 
  "uniffill",     V_uniffill,
  "unstack",      V_unstack,
  "unwrap",       V_unwrap,
  "vpnls",        V_vpnls,
  "window1d",     V_window1d,
  "wt1d",         V_wt1d,
  "wt2d",         V_wt2d,
  "xchg",         V_xchg,
  "xtrp",         V_xtrp,
  "zeropad",      V_zeropad,
  "zphase",       V_zphase,
  NULL, 
  NULL };
#else
extern COMMAND *cmdtable;
#endif









