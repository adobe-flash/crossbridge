
/*!
 *************************************************************************************
 * \file q_offsets.c
 *
 * \brief
 *    read Quantization Offset matrix parameters from input file: q_OffsetMatrix.cfg
 *
 *************************************************************************************
 */
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "memalloc.h"

extern char *GetConfigFileContent (char *Filename, int error_type);

#define MAX_ITEMS_TO_PARSE  1000

int offset4x4_check[6] = { 0, 0, 0, 0, 0, 0 };
int offset8x8_check[2] = { 0, 0 };

static const char OffsetType4x4[15][24] = {
  "INTRA4X4_LUMA_INTRA",
  "INTRA4X4_CHROMAU_INTRA",
  "INTRA4X4_CHROMAV_INTRA",
  "INTRA4X4_LUMA_INTERP",
  "INTRA4X4_CHROMAU_INTERP",
  "INTRA4X4_CHROMAV_INTERP",
  "INTRA4X4_LUMA_INTERB",
  "INTRA4X4_CHROMAU_INTERB",
  "INTRA4X4_CHROMAV_INTERB",
  "INTER4X4_LUMA_INTERP",
  "INTER4X4_CHROMAU_INTERP",
  "INTER4X4_CHROMAV_INTERP",
  "INTER4X4_LUMA_INTERB",
  "INTER4X4_CHROMAU_INTERB",
  "INTER4X4_CHROMAV_INTERB"
};

static const char OffsetType8x8[5][24] = {
  "INTRA8X8_LUMA_INTRA",
  "INTRA8X8_LUMA_INTERP",
  "INTRA8X8_LUMA_INTERB",
  "INTER8X8_LUMA_INTERP",
  "INTER8X8_LUMA_INTERB"
};


int ****LevelOffset4x4Luma;
int *****LevelOffset4x4Chroma;
int ****LevelOffset8x8Luma;

int AdaptRndWeight;
int AdaptRndCrWeight;

short **OffsetList4x4input;
short **OffsetList8x8input;
short **OffsetList4x4;
short **OffsetList8x8;

void InitOffsetParam ();

const int OffsetBits = 11;

static const short Offset_intra_default_intra[16] = {
  682, 682, 682, 682,
  682, 682, 682, 682,
  682, 682, 682, 682,
  682, 682, 682, 682
};

static const short Offset_intra_default_chroma[16] = {
  682, 682, 682, 682,
  682, 682, 682, 682,
  682, 682, 682, 682,
  682, 682, 682, 682
};


static const short Offset_intra_default_inter[16] = {
  342, 342, 342, 342,
  342, 342, 342, 342,
  342, 342, 342, 342,
  342, 342, 342, 342,
};

static const short Offset_inter_default[16] = {
  342, 342, 342, 342,
  342, 342, 342, 342,
  342, 342, 342, 342,
  342, 342, 342, 342,
};

static const short Offset8_intra_default_intra[64] = {
  682, 682, 682, 682, 682, 682, 682, 682,
  682, 682, 682, 682, 682, 682, 682, 682,
  682, 682, 682, 682, 682, 682, 682, 682,
  682, 682, 682, 682, 682, 682, 682, 682,
  682, 682, 682, 682, 682, 682, 682, 682,
  682, 682, 682, 682, 682, 682, 682, 682,
  682, 682, 682, 682, 682, 682, 682, 682,
  682, 682, 682, 682, 682, 682, 682, 682
};

static const short Offset8_intra_default_inter[64] = {
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342
};

static const short Offset8_inter_default[64] = {
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342,
  342, 342, 342, 342, 342, 342, 342, 342
};

/*!
 ***********************************************************************
 * \brief
 *    Allocate Q matrix arrays
 ***********************************************************************
 */
void allocate_QOffsets ()
{
  int max_qp_per_luma = (3 + 6*(input->BitDepthLuma) - MIN_QP)/6 + 1;
  int max_qp_per_cr = (3 + 6*(input->BitDepthChroma) - MIN_QP)/6 + 1;
  int max_qp_per = imax(max_qp_per_luma,max_qp_per_cr);
  get_mem4Dint(&LevelOffset4x4Luma,      2, max_qp_per, 4, 4);
  get_mem5Dint(&LevelOffset4x4Chroma, 2, 2, max_qp_per, 4, 4);
  get_mem4Dint(&LevelOffset8x8Luma,      2, max_qp_per, 8, 8);

  get_mem2Dshort(&OffsetList4x4input, 15, 16);
  get_mem2Dshort(&OffsetList8x8input,  5, 64);
  get_mem2Dshort(&OffsetList4x4, 15, 16);
  get_mem2Dshort(&OffsetList8x8,  5, 64);

}

/*!
 ***********************************************************************
 * \brief
 *    Free Q matrix arrays
 ***********************************************************************
 */
void free_QOffsets ()
{
  int max_qp_per_luma = (3 + 6*(input->BitDepthLuma) - MIN_QP)/6 + 1;
  int max_qp_per_cr = (3 + 6*(input->BitDepthChroma) - MIN_QP)/6 + 1;
  int max_qp_per = imax(max_qp_per_luma,max_qp_per_cr);
  free_mem4Dint(LevelOffset4x4Luma,      2, max_qp_per);
  free_mem5Dint(LevelOffset4x4Chroma, 2, 2, max_qp_per);
  free_mem4Dint(LevelOffset8x8Luma,      2, max_qp_per);

  free_mem2Dshort(OffsetList8x8);
  free_mem2Dshort(OffsetList4x4);
  free_mem2Dshort(OffsetList8x8input);
  free_mem2Dshort(OffsetList4x4input);
}


/*!
 ***********************************************************************
 * \brief
 *    Check the parameter name.
 * \param s
 *    parameter name string
 * \param type
 *    4x4 or 8x8 offset matrix type
 * \return
 *    the index number if the string is a valid parameter name,         \n
 *    -1 for error
 ***********************************************************************
 */

int CheckOffsetParameterName (char *s, int *type)
{
  int i = 0;

  *type = 0;
  while ((OffsetType4x4[i] != NULL) && (i < 15))
  {
    if (0 == strcmp (OffsetType4x4[i], s))
      return i;
    else
      i++;
  }

  i = 0;
  *type = 1;
  while ((OffsetType8x8[i] != NULL) && (i < 5))
  {
    if (0 == strcmp (OffsetType8x8[i], s))
      return i;
    else
      i++;
  }

  return -1;
}

/*!
 ***********************************************************************
 * \brief
 *    Parse the Q Offset Matrix values read from cfg file.
 * \param buf
 *    buffer to be parsed
 * \param bufsize
 *    buffer size of buffer
 ***********************************************************************
 */
void ParseQOffsetMatrix (char *buf, int bufsize)
{
  char *items[MAX_ITEMS_TO_PARSE];
  int MapIdx;
  int item = 0;
  int InString = 0, InItem = 0;
  char *p = buf;
  char *bufend = &buf[bufsize];
  int IntContent;
  int i, j, range, type, cnt;
  short *OffsetList;

  while (p < bufend)
  {
    switch (*p)
    {
      case 13:
        p++;
        break;
      case '#':                 // Found comment
        *p = '\0';              // Replace '#' with '\0' in case of comment immediately following integer or string
        while (*p != '\n' && p < bufend)  // Skip till EOL or EOF, whichever comes first
          p++;
        InString = 0;
        InItem = 0;
        break;
      case '\n':
        InItem = 0;
        InString = 0;
      *p++ = '\0';
        break;
      case ' ':
      case '\t':              // Skip whitespace, leave state unchanged
        if (InString)
          p++;
        else
        {                     // Terminate non-strings once whitespace is found
          *p++ = '\0';
          InItem = 0;
        }
        break;

      case '"':               // Begin/End of String
        *p++ = '\0';
        if (!InString)
        {
          items[item++] = p;
          InItem = ~InItem;
        }
        else
          InItem = 0;
        InString = ~InString; // Toggle
        break;

      case ',':
        p++;
        InItem = 0;
        break;

      default:
        if (!InItem)
        {
          items[item++] = p;
          InItem = ~InItem;
        }
        p++;
    }
  }

  item--;

  for (i = 0; i < item; i += cnt)
  {
    cnt = 0;
    if (0 > (MapIdx = CheckOffsetParameterName (items[i + cnt], &type)))
    {
      snprintf (errortext, ET_SIZE,
        " Parsing error in config file: Parameter Name '%s' not recognized.",
        items[i + cnt]);
      error (errortext, 300);
    }
    cnt++;
    if (strcmp ("=", items[i + cnt]))
    {
      snprintf (errortext, ET_SIZE,
        " Parsing error in config file: '=' expected as the second token in each item.");
      error (errortext, 300);
    }
    cnt++;

    if (!type) //4x4 Matrix
    {
      range = 16;
      OffsetList = OffsetList4x4input[MapIdx];
      offset4x4_check[MapIdx] = 1; //to indicate matrix found in cfg file
    }
    else //8x8 matrix
    {
      range = 64;
      OffsetList = OffsetList8x8input[MapIdx];
      offset8x8_check[MapIdx] = 1; //to indicate matrix found in cfg file
    }

    for (j = 0; j < range; j++)
    {
      if (1 != sscanf (items[i + cnt + j], "%d", &IntContent))
      {
        snprintf (errortext, ET_SIZE,
          " Parsing error: Expected numerical value for Parameter of %s, found '%s'.",
          items[i], items[i + cnt + j]);
        error (errortext, 300);
      }

      OffsetList[j] = (short) IntContent; //save value in matrix
    }
    cnt += j;
    printf (".");
  }
}


/*!
 ***********************************************************************
 * \brief
 *    Initialise Q offset matrix values.
 ***********************************************************************
 */
void Init_QOffsetMatrix ()
{
  char *content;

  allocate_QOffsets ();

  if (input->OffsetMatrixPresentFlag)
  {
    printf ("Parsing Quantization Offset Matrix file %s ",
      input->QOffsetMatrixFile);
    content = GetConfigFileContent (input->QOffsetMatrixFile, 0);
    if (content != '\0')
      ParseQOffsetMatrix (content, strlen (content));
    else
    {
      printf
        ("\nError: %s\nProceeding with default values for all matrices.",
        errortext);
      input->OffsetMatrixPresentFlag = 0;
    }

    printf ("\n");

    free (content);
  }
  //! Now set up all offset params. This process could be reused if we wish to re-init offsets
  InitOffsetParam ();
}

/*!
 ************************************************************************
 * \brief
 *    Intit quantization offset params
 *
 * \par Input:
 *    none
 *
 * \par Output:
 *    none
 ************************************************************************
 */
void InitOffsetParam ()
{
  int k;

  if (input->OffsetMatrixPresentFlag)
  {
    memcpy(&(OffsetList4x4[0][0]),&(OffsetList4x4input[0][0]), 15 * 16 * sizeof(short));
    memcpy(&(OffsetList8x8[0][0]),&(OffsetList8x8input[0][0]),  5 * 64 * sizeof(short));
  }
  else
  {
    memcpy(&(OffsetList4x4[0][0]),&(Offset_intra_default_intra[0]), 16 * sizeof(short));
    for (k = 1; k < 3; k++)
      memcpy(&(OffsetList4x4[k][0]),&(Offset_intra_default_chroma[0]),  16 * sizeof(short));
    for (k = 3; k < 9; k++)
      memcpy(&(OffsetList4x4[k][0]),&(Offset_intra_default_inter[0]),  16 * sizeof(short));
    for (k = 9; k < 15; k++)
      memcpy(&(OffsetList4x4[k][0]),&(Offset_inter_default[0]),  16 * sizeof(short));

    memcpy(&(OffsetList8x8[0][0]),&(Offset8_intra_default_intra[0]), 64 * sizeof(short));
    memcpy(&(OffsetList8x8[1][0]),&(Offset8_intra_default_inter[0]), 64 * sizeof(short));
    memcpy(&(OffsetList8x8[2][0]),&(Offset8_intra_default_inter[0]), 64 * sizeof(short));
    memcpy(&(OffsetList8x8[3][0]),&(Offset8_inter_default[0]), 64 * sizeof(short));
    memcpy(&(OffsetList8x8[4][0]),&(Offset8_inter_default[0]), 64 * sizeof(short));
  }
}


/*!
 ************************************************************************
 * \brief
 *    Calculation of the quantization offset params at the frame level
 *
 * \par Input:
 *    none
 *
 * \par Output:
 *    none
 ************************************************************************
 */
void CalculateOffsetParam ()
{
  int i, j, k, temp;
  int qp_per;
  int img_type = (img->type == SI_SLICE ? I_SLICE : (img->type == SP_SLICE ? P_SLICE : img->type));

  int max_qp_per_luma = qp_per_matrix[(51 + img->bitdepth_luma_qp_scale - MIN_QP)] + 1;
  int max_qp_per_cr   = qp_per_matrix[(51 + img->bitdepth_chroma_qp_scale - MIN_QP)] + 1;

  AdaptRndWeight = input->AdaptRndWFactor[img->nal_reference_idc!=0][img_type];
  AdaptRndCrWeight = input->AdaptRndCrWFactor[img->nal_reference_idc!=0][img_type];
  for (k = 0; k < imax(max_qp_per_luma,max_qp_per_cr); k++)
  {
    qp_per = Q_BITS + k - OffsetBits;
    for (j = 0; j < 4; j++)
    {
      for (i = 0; i < 4; i++)
      {
        temp = (j << 2) + i;
        if (img_type == I_SLICE)
        {
          LevelOffset4x4Luma[1][k][j][i] =
            (int) OffsetList4x4[0][temp] << qp_per;
          LevelOffset4x4Chroma[0][1][k][j][i] =
            (int) OffsetList4x4[1][temp] << qp_per;
          LevelOffset4x4Chroma[1][1][k][j][i] =
            (int) OffsetList4x4[2][temp] << qp_per;
        }
        else if (img_type == B_SLICE)
        {
          LevelOffset4x4Luma[1][k][j][i] =
            (int) OffsetList4x4[6][temp] << qp_per;
          LevelOffset4x4Chroma[0][1][k][j][i] =
            (int) OffsetList4x4[7][temp] << qp_per;
          LevelOffset4x4Chroma[1][1][k][j][i] =
            (int) OffsetList4x4[8][temp] << qp_per;
        }
        else
        {
          LevelOffset4x4Luma[1][k][j][i] =
            (int) OffsetList4x4[3][temp] << qp_per;
          LevelOffset4x4Chroma[0][1][k][j][i] =
            (int) OffsetList4x4[4][temp] << qp_per;
          LevelOffset4x4Chroma[1][1][k][j][i] =
            (int) OffsetList4x4[5][temp] << qp_per;
        }

        if (img_type == B_SLICE)
        {
          LevelOffset4x4Luma[0][k][j][i] =
            (int) OffsetList4x4[12][temp] << qp_per;
          LevelOffset4x4Chroma[0][0][k][j][i] =
            (int) OffsetList4x4[13][temp] << qp_per;
          LevelOffset4x4Chroma[1][0][k][j][i] =
            (int) OffsetList4x4[14][temp] << qp_per;
        }
        else
        {
          LevelOffset4x4Luma[0][k][j][i] =
            (int) OffsetList4x4[9][temp] << qp_per;
          LevelOffset4x4Chroma[0][0][k][j][i] =
            (int) OffsetList4x4[10][temp] << qp_per;
          LevelOffset4x4Chroma[1][0][k][j][i] =
            (int) OffsetList4x4[11][temp] << qp_per;
        }
      }
    }
  }
}

 /*!
 ************************************************************************
 * \brief
 *    Calculate the quantisation offset parameters
 *
 ************************************************************************
 */
void CalculateOffset8Param ()
{
  int i, j, k, temp;
  int q_bits;

  int max_qp_per_luma = qp_per_matrix[(51 + img->bitdepth_luma_qp_scale - MIN_QP)] + 1;
  int max_qp_per_cr   = qp_per_matrix[(51 + img->bitdepth_chroma_qp_scale - MIN_QP)] + 1;

  for (k = 0; k < imax(max_qp_per_luma,max_qp_per_cr); k++)
  {
    q_bits = Q_BITS_8 + k - OffsetBits;
    for (j = 0; j < 8; j++)
    {
      for (i = 0; i < 8; i++)
      {
        temp = (j << 3) + i;
        if (img->type == I_SLICE)
          LevelOffset8x8Luma[1][k][j][i] =
          (int) OffsetList8x8[0][temp] << q_bits;
        else if (img->type == B_SLICE)
          LevelOffset8x8Luma[1][k][j][i] =
          (int) OffsetList8x8[2][temp] << q_bits;
        else
          LevelOffset8x8Luma[1][k][j][i] =
          (int) OffsetList8x8[1][temp] << q_bits;

        if (img->type == B_SLICE)
          LevelOffset8x8Luma[0][k][j][i] =
          (int) OffsetList8x8[4][temp] << q_bits;
        else
          LevelOffset8x8Luma[0][k][j][i] =
          (int) OffsetList8x8[3][temp] << q_bits;
      }
    }
  }
}

