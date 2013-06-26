/*
MODULE     : textloc.h
DESCRIPTION: Header file to be used with textloc.c.
AUTHOR     : Fady Habra, 8715030, fady@cs.mcgill.ca
*/

#ifdef TEXTLOC
#define EXT
#else
#define EXT extern
#endif

#ifdef BOGUS
#define VSPREAD_THRESHOLD	((float) (2.0/100))
/* #define VAR_THRESHOLD		30 */
#define SAME_ROW_THRESHOLD	3
#define SAME_ROW_V		3
#define SAME_ROW_H		30
#define MAX_CHAR_SIZE		50
#define MIN_CHAR_SIZE		5

#define KILL_SMALL_COMP
#define SMALL_THRESHOLD		10
#endif

#define E_FALLING	0
#define E_RISING	1

typedef struct _PixT	PixT, *PixP;

struct _PixT
{
  int x;
  int y;
  PixP prev;
};

typedef struct _CompT	CompT, *CompP, **CompI;

struct _CompT					/* connected component type  */
{
  int id;					/* component id		     */
  int n;					/* number of pixels in comp. */
  int avg_row;					/* average row of pixels     */
  int x_max;					/* maximum x coordinate	     */
  int y_max;					/* maximum y coordinate	     */
  int x_min;					/* minimum x coordinate	     */
  int y_min;					/* minimum y coordinate	     */
  int y_left;					/* left endpoint y coord     */
  int y_right;					/* right endpoint y coord    */
  float sv_above;				/* sum of variances above    */
  float sv_below;				/* sum of variances below    */
  char type;					/* rising/falling edge	     */
  PixP pixels;					/* linked list of pixels     */
  CompP paired;					/* paired opposite comp.     */
  char extended;				/* extension checked flag    */
  CompP prev;					/* previous connected comp   */
};

/* External function prototypes. */

EXT void ConvertToFloat(ImgP);
EXT void HorzVariance(ImgP, int);
EXT CompP BuildConnectedComponents(ImgP, int);
EXT void PrintConnectedComponents(CompP);
EXT void WriteConnectedComponentsToPGM(CompP, ImgP);
EXT void FreeConnectedComponents(CompP);
EXT void EliminateLargeSpreadComponents(CompI, ImgP);
EXT void MergeRowComponents(CompI, ImgP);
EXT void PairComponents(CompI, ImgP);
EXT void ComputeBoundingBoxes(CompP, ImgP);

/* Internal function prototypes. */

void MergeComponents(CompP, CompP, CompP, CompI, ImgP);
void MergeToLeft(CompP, CompI, ImgP);
char Overlap(CompP, CompP);

