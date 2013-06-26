
/*!
 *************************************************************************************
 * \file q_matrix.c
 *
 * \brief
 *    read q_matrix parameters from input file: q_matrix.cfg
 *
 *************************************************************************************
 */
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "memalloc.h"

extern char *GetConfigFileContent (char *Filename, int error_type);

#define MAX_ITEMS_TO_PARSE  1000

extern const int quant_coef[6][4][4];
extern const int dequant_coef[6][4][4];

extern const int quant_coef8[6][8][8];
extern const int dequant_coef8[6][8][8];


int matrix4x4_check[6] = {0, 0, 0, 0, 0, 0};
int matrix8x8_check[2] = {0, 0};

static const char MatrixType4x4[6][20] =
{
  "INTRA4X4_LUMA",
  "INTRA4X4_CHROMAU",
  "INTRA4X4_CHROMAV",
  "INTER4X4_LUMA",
  "INTER4X4_CHROMAU",
  "INTER4X4_CHROMAV"
};

static const char MatrixType8x8[2][20] =
{
  "INTRA8X8_LUMA",
  "INTER8X8_LUMA",
};

int ****LevelScale4x4Luma;
int *****LevelScale4x4Chroma;
int ****LevelScale8x8Luma;

int ****InvLevelScale4x4Luma;
int *****InvLevelScale4x4Chroma;
int ****InvLevelScale8x8Luma;

short ScalingList4x4input[6][16];
short ScalingList8x8input[2][64];
short ScalingList4x4[6][16];
short ScalingList8x8[2][64];

short UseDefaultScalingMatrix4x4Flag[6];
short UseDefaultScalingMatrix8x8Flag[2];


int *qp_per_matrix;
int *qp_rem_matrix;

static const short Quant_intra_default[16] =
{
 6,13,20,28,
13,20,28,32,
20,28,32,37,
28,32,37,42
};

static const short Quant_inter_default[16] =
{
10,14,20,24,
14,20,24,27,
20,24,27,30,
24,27,30,34
};

static const short Quant8_intra_default[64] =
{
 6,10,13,16,18,23,25,27,
10,11,16,18,23,25,27,29,
13,16,18,23,25,27,29,31,
16,18,23,25,27,29,31,33,
18,23,25,27,29,31,33,36,
23,25,27,29,31,33,36,38,
25,27,29,31,33,36,38,40,
27,29,31,33,36,38,40,42
};

static const short Quant8_inter_default[64] =
{
 9,13,15,17,19,21,22,24,
13,13,17,19,21,22,24,25,
15,17,19,21,22,24,25,27,
17,19,21,22,24,25,27,28,
19,21,22,24,25,27,28,30,
21,22,24,25,27,28,30,32,
22,24,25,27,28,30,32,33,
24,25,27,28,30,32,33,35
};


/*!
 ***********************************************************************
 * \brief
 *    Check the parameter name.
 * \param s
 *    parameter name string
 * \param type
 *    4x4 or 8x8 matrix type
 * \return
 *    the index number if the string is a valid parameter name,         \n
 *    -1 for error
 ***********************************************************************
 */
int CheckParameterName (char *s, int *type)
{
  int i = 0;

  *type = 0;
  while ((MatrixType4x4[i] != NULL) && (i<6))
  {
    if (0==strcmp (MatrixType4x4[i], s))
      return i;
    else
      i++;
  }

  i = 0;
  *type = 1;
  while ((MatrixType8x8[i] != NULL) && (i<2))
  {
    if (0==strcmp (MatrixType8x8[i], s))
      return i;
    else
      i++;
  }

  return -1;
}

/*!
 ***********************************************************************
 * \brief
 *    Parse the Q matrix values read from cfg file.
 * \param buf
 *    buffer to be parsed
 * \param bufsize
 *    buffer size of buffer
 ***********************************************************************
 */
void ParseMatrix (char *buf, int bufsize)
{
  char *items[MAX_ITEMS_TO_PARSE];
  int MapIdx;
  int item = 0;
  int InString = 0, InItem = 0;
  char *p = buf;
  char *bufend = &buf[bufsize];
  int IntContent;
  int i, j, range, type, cnt;
  short *ScalingList;

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
        *p++='\0';
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

  for (i=0; i<item; i+=cnt)
  {
    cnt=0;
    if (0 > (MapIdx = CheckParameterName (items[i+cnt], &type)))
    {
      snprintf (errortext, ET_SIZE, " Parsing error in config file: Parameter Name '%s' not recognized.", items[i+cnt]);
      error (errortext, 300);
    }
    cnt++;
    if (strcmp ("=", items[i+cnt]))
    {
      snprintf (errortext, ET_SIZE, " Parsing error in config file: '=' expected as the second token in each item.");
      error (errortext, 300);
    }
    cnt++;

    if (!type) //4x4 Matrix
    {
      range = 16;
      ScalingList = ScalingList4x4input[MapIdx];
      matrix4x4_check[MapIdx] = 1; //to indicate matrix found in cfg file
    }
    else //8x8 matrix
    {
      range = 64;
      ScalingList = ScalingList8x8input[MapIdx];
      matrix8x8_check[MapIdx] = 1; //to indicate matrix found in cfg file
    }

    for(j=0; j<range; j++)
    {
      if (1 != sscanf (items[i+cnt+j], "%d", &IntContent))
      {
        snprintf (errortext, ET_SIZE, " Parsing error: Expected numerical value for Parameter of %s, found '%s'.", items[i], items[i+cnt+j]);
        error (errortext, 300);
      }

      ScalingList[j] = (short)IntContent; //save value in matrix
    }
    cnt+=j;
    printf (".");
  }
}

/*!
 ***********************************************************************
 * \brief
 *    Check Q Matrix values. If invalid values found in matrix,
 *    whole matrix will be patch with default value 16.
 ***********************************************************************
 */
void PatchMatrix(void)
{
  short *ScalingList;
  int i, cnt, fail;

  for(i=0; i<6; i++)
  {
    if(input->ScalingListPresentFlag[i])
    {
      ScalingList=ScalingList4x4input[i];
      if(matrix4x4_check[i])
      {
        fail=0;
        for(cnt=0; cnt<16; cnt++)
        {
          if(ScalingList[cnt]<0 || ScalingList[cnt]>255) // ScalingList[0]=0 to indicate use default matrix
          {
            fail=1;
            break;
          }
        }

        if(fail) //value of matrix exceed range
        {
          printf("\n%s value exceed range. (Value must be 1 to 255)\n", MatrixType4x4[i]);
          printf("Setting default values for this matrix.");
          if(i>2)
            memcpy(ScalingList, Quant_inter_default, sizeof(short)*16);
          else
            memcpy(ScalingList, Quant_intra_default, sizeof(short)*16);
        }
      }
      else //matrix not found, pad with default value
      {
        printf("\n%s matrix definition not found. Setting default values.", MatrixType4x4[i]);
        if(i>2)
          memcpy(ScalingList, Quant_inter_default, sizeof(short)*16);
        else
          memcpy(ScalingList, Quant_intra_default, sizeof(short)*16);
      }
    }

    if((i<2) && input->ScalingListPresentFlag[i+6])
    {
      ScalingList=ScalingList8x8input[i];
      if(matrix8x8_check[i])
      {
        fail=0;
        for(cnt=0; cnt<64; cnt++)
        {
          if(ScalingList[cnt]<0 || ScalingList[cnt]>255) // ScalingList[0]=0 to indicate use default matrix
          {
            fail=1;
            break;
          }
        }

        if(fail) //value of matrix exceed range
        {
          printf("\n%s value exceed range. (Value must be 1 to 255)\n", MatrixType8x8[i]);
          printf("Setting default values for this matrix.");
          if(i==7)
            memcpy(ScalingList, Quant8_inter_default, sizeof(short)*64);
          else
            memcpy(ScalingList, Quant8_intra_default, sizeof(short)*64);
        }
      }
      else //matrix not found, pad with default value
      {
        printf("\n%s matrix definition not found. Setting default values.", MatrixType8x8[i]);
        if(i==7)
          memcpy(ScalingList, Quant8_inter_default, sizeof(short)*64);
        else
          memcpy(ScalingList, Quant8_intra_default, sizeof(short)*64);
      }
    }
  }
}

/*!
 ***********************************************************************
 * \brief
 *    Allocate Q matrix arrays
 ***********************************************************************
 */
void allocate_QMatrix ()
{
  int bitdepth_qp_scale = 6*(input->BitDepthLuma - 8);
  int i;

  if ((qp_per_matrix = (int*)malloc((MAX_QP + 1 +  bitdepth_qp_scale)*sizeof(int))) == NULL)
    no_mem_exit("init_global_buffers: qp_per_matrix");
  if ((qp_rem_matrix = (int*)malloc((MAX_QP + 1 +  bitdepth_qp_scale)*sizeof(int))) == NULL)
    no_mem_exit("init_global_buffers: qp_per_matrix");

  for (i = 0; i < MAX_QP + bitdepth_qp_scale + 1; i++)
  {
    qp_per_matrix[i] = i / 6;
    qp_rem_matrix[i] = i % 6;
  }

  get_mem4Dint(&LevelScale4x4Luma,      2, 6, 4, 4);
  get_mem5Dint(&LevelScale4x4Chroma, 2, 2, 6, 4, 4);
  get_mem4Dint(&LevelScale8x8Luma,      2, 6, 8, 8);

  get_mem4Dint(&InvLevelScale4x4Luma,      2, 6, 4, 4);
  get_mem5Dint(&InvLevelScale4x4Chroma, 2, 2, 6, 4, 4);
  get_mem4Dint(&InvLevelScale8x8Luma,      2, 6, 8, 8);
}

/*!
 ***********************************************************************
 * \brief
 *    Free Q matrix arrays
 ***********************************************************************
 */
void free_QMatrix ()
{
  free(qp_rem_matrix);
  free(qp_per_matrix);

  free_mem4Dint(LevelScale4x4Luma,      2, 6);
  free_mem5Dint(LevelScale4x4Chroma, 2, 2, 6);
  free_mem4Dint(LevelScale8x8Luma,      2, 6);

  free_mem4Dint(InvLevelScale4x4Luma,      2, 6);
  free_mem5Dint(InvLevelScale4x4Chroma, 2, 2, 6);
  free_mem4Dint(InvLevelScale8x8Luma,      2, 6);
}


/*!
 ***********************************************************************
 * \brief
 *    Initialise Q matrix values.
 ***********************************************************************
 */
void Init_QMatrix (void)
{
  char *content;


  allocate_QMatrix ();

  if(input->ScalingMatrixPresentFlag)
  {
    printf ("Parsing QMatrix file %s ", input->QmatrixFile);
    content = GetConfigFileContent(input->QmatrixFile, 0);
    if(content!='\0')
      ParseMatrix(content, strlen (content));
    else
      printf("\nError: %s\nProceeding with default values for all matrices.", errortext);

    PatchMatrix();
    printf("\n");

    memset(UseDefaultScalingMatrix4x4Flag, 0, 6 * sizeof(short));
    UseDefaultScalingMatrix8x8Flag[0]=UseDefaultScalingMatrix8x8Flag[1]=0;

    free(content);
  }
}

/*!
 ************************************************************************
 * \brief
 *    For calculating the quantisation values at frame level
 *
 * \par Input:
 *    none
 *
 * \par Output:
 *    none
 ************************************************************************
 */
void CalculateQuantParam(void)
{
  int i, j, k, temp;
  int present[6];
  int no_q_matrix=FALSE;

  if(!active_sps->seq_scaling_matrix_present_flag && !active_pps->pic_scaling_matrix_present_flag) //set to no q-matrix
    no_q_matrix=TRUE;
  else
  {
    memset(present, 0, sizeof(int)*6);

    if(active_sps->seq_scaling_matrix_present_flag)
      for(i=0; i<6; i++)
        present[i] = active_sps->seq_scaling_list_present_flag[i];

    if(active_pps->pic_scaling_matrix_present_flag)
      for(i=0; i<6; i++)
      {
        if((i==0) || (i==3))
          present[i] |= active_pps->pic_scaling_list_present_flag[i];
        else
          present[i] = active_pps->pic_scaling_list_present_flag[i];
      }
  }

  if(no_q_matrix==TRUE)
  {
    for(k=0; k<6; k++)
      for(j=0; j<4; j++)
        for(i=0; i<4; i++)
        {
          LevelScale4x4Luma[1][k][j][i]         = quant_coef[k][j][i];
          InvLevelScale4x4Luma[1][k][j][i]      = dequant_coef[k][j][i]<<4;

          LevelScale4x4Chroma[0][1][k][j][i]    = quant_coef[k][j][i];
          InvLevelScale4x4Chroma[0][1][k][j][i] = dequant_coef[k][j][i]<<4;

          LevelScale4x4Chroma[1][1][k][j][i]    = quant_coef[k][j][i];
          InvLevelScale4x4Chroma[1][1][k][j][i] = dequant_coef[k][j][i]<<4;

          // Inter
          LevelScale4x4Luma[0][k][j][i]         = quant_coef[k][j][i];
          InvLevelScale4x4Luma[0][k][j][i]      = dequant_coef[k][j][i]<<4;

          LevelScale4x4Chroma[0][0][k][j][i]    = quant_coef[k][j][i];
          InvLevelScale4x4Chroma[0][0][k][j][i] = dequant_coef[k][j][i]<<4;

          LevelScale4x4Chroma[1][0][k][j][i]    = quant_coef[k][j][i];
          InvLevelScale4x4Chroma[1][0][k][j][i] = dequant_coef[k][j][i]<<4;
        }
  }
  else
  {
    for(k=0; k<6; k++)
      for(j=0; j<4; j++)
        for(i=0; i<4; i++)
        {
          temp = (j<<2)+i;
          if((!present[0]) || UseDefaultScalingMatrix4x4Flag[0])
          {
            LevelScale4x4Luma[1][k][j][i]         = (quant_coef[k][j][i]<<4)/Quant_intra_default[temp];
            InvLevelScale4x4Luma[1][k][j][i]      = dequant_coef[k][j][i]*Quant_intra_default[temp];
          }
          else
          {
            LevelScale4x4Luma[1][k][j][i]         = (quant_coef[k][j][i]<<4)/ScalingList4x4[0][temp];
            InvLevelScale4x4Luma[1][k][j][i]      = dequant_coef[k][j][i]*ScalingList4x4[0][temp];
          }

          if(!present[1])
          {
            LevelScale4x4Chroma[0][1][k][j][i]    = LevelScale4x4Luma[1][k][j][i];
            InvLevelScale4x4Chroma[0][1][k][j][i] = InvLevelScale4x4Luma[1][k][j][i];
          }
          else
          {
            LevelScale4x4Chroma[0][1][k][j][i]    = (quant_coef[k][j][i]<<4)/(UseDefaultScalingMatrix4x4Flag[1] ? Quant_intra_default[temp]:ScalingList4x4[1][temp]);
            InvLevelScale4x4Chroma[0][1][k][j][i] = dequant_coef[k][j][i]*(UseDefaultScalingMatrix4x4Flag[1] ? Quant_intra_default[temp]:ScalingList4x4[1][temp]);
          }

          if(!present[2])
          {
            LevelScale4x4Chroma[1][1][k][j][i]    = LevelScale4x4Chroma[0][1][k][j][i];
            InvLevelScale4x4Chroma[1][1][k][j][i] = InvLevelScale4x4Chroma[0][1][k][j][i];
          }
          else
          {
            LevelScale4x4Chroma[1][1][k][j][i]    = (quant_coef[k][j][i]<<4)/(UseDefaultScalingMatrix4x4Flag[2] ? Quant_intra_default[temp]:ScalingList4x4[2][temp]);
            InvLevelScale4x4Chroma[1][1][k][j][i] = dequant_coef[k][j][i]*(UseDefaultScalingMatrix4x4Flag[2] ? Quant_intra_default[temp]:ScalingList4x4[2][temp]);
          }

          if((!present[3]) || UseDefaultScalingMatrix4x4Flag[3])
          {
            LevelScale4x4Luma[0][k][j][i]         = (quant_coef[k][j][i]<<4)/Quant_inter_default[temp];
            InvLevelScale4x4Luma[0][k][j][i]      = dequant_coef[k][j][i]*Quant_inter_default[temp];
          }
          else
          {
            LevelScale4x4Luma[0][k][j][i]         = (quant_coef[k][j][i]<<4)/ScalingList4x4[3][temp];
            InvLevelScale4x4Luma[0][k][j][i]      = dequant_coef[k][j][i]*ScalingList4x4[3][temp];
          }

          if(!present[4])
          {
            LevelScale4x4Chroma[0][0][k][j][i]    = LevelScale4x4Luma[0][k][j][i];
            InvLevelScale4x4Chroma[0][0][k][j][i] = InvLevelScale4x4Luma[0][k][j][i];
          }
          else
          {
            LevelScale4x4Chroma[0][0][k][j][i]    = (quant_coef[k][j][i]<<4)/(UseDefaultScalingMatrix4x4Flag[4] ? Quant_inter_default[temp]:ScalingList4x4[4][temp]);
            InvLevelScale4x4Chroma[0][0][k][j][i] = dequant_coef[k][j][i]*(UseDefaultScalingMatrix4x4Flag[4] ? Quant_inter_default[temp]:ScalingList4x4[4][temp]);
          }

          if(!present[5])
          {
            LevelScale4x4Chroma[1][0][k][j][i]    = LevelScale4x4Chroma[0][0][k][j][i];
            InvLevelScale4x4Chroma[1][0][k][j][i] = InvLevelScale4x4Chroma[0][0][k][j][i];
          }
          else
          {
            LevelScale4x4Chroma[1][0][k][j][i]    = (quant_coef[k][j][i]<<4)/(UseDefaultScalingMatrix4x4Flag[5] ? Quant_inter_default[temp]:ScalingList4x4[5][temp]);
            InvLevelScale4x4Chroma[1][0][k][j][i] = dequant_coef[k][j][i]*(UseDefaultScalingMatrix4x4Flag[5] ? Quant_inter_default[temp]:ScalingList4x4[5][temp]);
          }
        }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Calculate the quantisation and inverse quantisation parameters
 *
 ************************************************************************
 */
void CalculateQuant8Param()
{
  int i, j, k, temp;
  int present[2];
  int no_q_matrix=FALSE;

  if(!active_sps->seq_scaling_matrix_present_flag && !active_pps->pic_scaling_matrix_present_flag) //set to default matrix
    no_q_matrix=TRUE;
  else
  {
    memset(present, 0, sizeof(int)*2);

    if(active_sps->seq_scaling_matrix_present_flag)
      for(i=0; i<2; i++)
        present[i] = active_sps->seq_scaling_list_present_flag[i+6];

      if(active_pps->pic_scaling_matrix_present_flag)
        for(i=0; i<2; i++)
          present[i] |= active_pps->pic_scaling_list_present_flag[i+6];
  }

  if(no_q_matrix==TRUE)
  {
    for(k=0; k<6; k++)
      for(j=0; j<8; j++)
        for(i=0; i<8; i++)
        {
          LevelScale8x8Luma[1][k][j][i]         = quant_coef8[k][j][i];
          InvLevelScale8x8Luma[1][k][j][i]      = dequant_coef8[k][j][i]<<4;

          LevelScale8x8Luma[0][k][j][i]         = quant_coef8[k][j][i];
          InvLevelScale8x8Luma[0][k][j][i]      = dequant_coef8[k][j][i]<<4;
        }
  }
  else
  {
    for(k=0; k<6; k++)
      for(j=0; j<8; j++)
        for(i=0; i<8; i++)
        {
          temp = (j<<3)+i;
          if((!present[0]) || UseDefaultScalingMatrix8x8Flag[0])
          {
            LevelScale8x8Luma[1][k][j][i]    = (quant_coef8[k][j][i]<<4)/Quant8_intra_default[temp];
            InvLevelScale8x8Luma[1][k][j][i] = dequant_coef8[k][j][i]*Quant8_intra_default[temp];
          }
          else
          {
            LevelScale8x8Luma[1][k][j][i]    = (quant_coef8[k][j][i]<<4)/ScalingList8x8[0][temp];
            InvLevelScale8x8Luma[1][k][j][i] = dequant_coef8[k][j][i]*ScalingList8x8[0][temp];
          }

          if((!present[1]) || UseDefaultScalingMatrix8x8Flag[1])
          {
            LevelScale8x8Luma[0][k][j][i]    = (quant_coef8[k][j][i]<<4)/Quant8_inter_default[temp];
            InvLevelScale8x8Luma[0][k][j][i] = dequant_coef8[k][j][i]*Quant8_inter_default[temp];
          }
          else
          {
            LevelScale8x8Luma[0][k][j][i]    = (quant_coef8[k][j][i]<<4)/ScalingList8x8[1][temp];
            InvLevelScale8x8Luma[0][k][j][i] = dequant_coef8[k][j][i]*ScalingList8x8[1][temp];
          }
        }
  }
}

