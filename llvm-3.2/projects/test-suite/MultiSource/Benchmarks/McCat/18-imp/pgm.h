/*
MODULE     : pgm.h
DESCRIPTION: Header file to be used with pgm.c.
AUTHOR     : Fady Habra, 8715030, fady@cs.mcgill.ca
*/

#ifdef PGM
#define EXT
#else
#define EXT extern
#endif

typedef struct					/* image information object */
{
  char valid;					/* valid image info or not  */
  char *filename;				/* image filename	    */
  char *imgname;				/* image name		    */
  FILE *fp;					/* file pointer		    */
  int height;					/* height		    */
  int width;					/* width		    */
  int maxgray;					/* maximum gray value	    */
  int threshold;				/* Otsu threshold	    */
  unsigned char *data;				/* image data		    */
  double *R;					/* blurred data		    */
  float *f;					/* fp data for L_canny()    */
  float *var;					/* horiz. variance bitmap   */
  float *cedge;					/* Canny edge data	    */
  int *lmap;					/* label map		    */
  double Rmin;					/* minimum R value	    */
  double Rmax;					/* maximum R value	    */
  int *hist;					/* histogram values (h*w)   */
  double *p;					/* normalized histogram     */
  int *hsig;					/* horizontal signature (w) */
  int *vsig;					/* vertical signature (h)   */
} ImgT;

typedef ImgT	*ImgP;				/* ptr to image info object */

						/* PGM return codes	    */
#define	PGM_OK		0			/* success		    */
#define PGM_NOT_FOUND	-1			/* file not found	    */
#define PGM_NOT_PGM	-2			/* invalid PGM file	    */
#define	PGM_NO_DATA	-3			/* no data to process	    */

						/* image types		    */
#define PGM_IDATA	0			/* raw image data	    */
#define PGM_IBLUR	1			/* blurred image	    */
#define PGM_CEDGE	2			/* Canny edges		    */
#define PGM_IVAR	3			/* horizontal variance	    */

EXT void PGM_InitImage(ImgP, char *);
EXT void PGM_FreeImage(ImgP);
EXT void PGM_PrintInfo(ImgP);
EXT int PGM_LoadImage(ImgP);
EXT int PGM_WriteBinary(ImgP);
EXT int PGM_WriteImage(ImgP, char);

#ifdef PGM
int PGM_Open(ImgP);
void PGM_Close(ImgP);
int PGM_GetValue(ImgP);
#endif

#undef EXT
