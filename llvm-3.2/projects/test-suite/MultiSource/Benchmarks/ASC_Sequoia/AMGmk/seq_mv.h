/*BHEADER**********************************************************************
 * Copyright (c) 2006   The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * Written by the HYPRE team. UCRL-CODE-222953.
 * All rights reserved.
 *
 * This file is part of HYPRE (see http://www.llnl.gov/CASC/hypre/).
 * Please see the COPYRIGHT_and_LICENSE file for the copyright notice, 
 * disclaimer, contact information and the GNU Lesser General Public License.
 *
 * HYPRE is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License (as published by the Free Software
 * Foundation) version 2.1 dated February 1999.
 *
 * HYPRE is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the terms and conditions of the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Revision: 2.14 $
 ***********************************************************************EHEADER*/





#ifndef hypre_MV_HEADER
#define hypre_MV_HEADER

#include "utilities.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 *
 * Header info for CSR Matrix data structures
 *
 * Note: this matrix currently uses 0-based indexing.
 *
 *****************************************************************************/

#ifndef hypre_CSR_MATRIX_HEADER
#define hypre_CSR_MATRIX_HEADER

/*--------------------------------------------------------------------------
 * CSR Matrix
 *--------------------------------------------------------------------------*/

typedef struct
{
   double  *data;
   int     *i;
   int     *j;
   int      num_rows;
   int      num_cols;
   int      num_nonzeros;

  /* for compressing rows in matrix multiplication  */
   int     *rownnz;
   int      num_rownnz;

   /* Does the CSRMatrix create/destroy `data', `i', `j'? */
   int      owns_data;

} hypre_CSRMatrix;

/*--------------------------------------------------------------------------
 * Accessor functions for the CSR Matrix structure
 *--------------------------------------------------------------------------*/

#define hypre_CSRMatrixData(matrix)         ((matrix) -> data)
#define hypre_CSRMatrixI(matrix)            ((matrix) -> i)
#define hypre_CSRMatrixJ(matrix)            ((matrix) -> j)
#define hypre_CSRMatrixNumRows(matrix)      ((matrix) -> num_rows)
#define hypre_CSRMatrixNumCols(matrix)      ((matrix) -> num_cols)
#define hypre_CSRMatrixNumNonzeros(matrix)  ((matrix) -> num_nonzeros)
#define hypre_CSRMatrixRownnz(matrix)       ((matrix) -> rownnz)
#define hypre_CSRMatrixNumRownnz(matrix)    ((matrix) -> num_rownnz)
#define hypre_CSRMatrixOwnsData(matrix)     ((matrix) -> owns_data)

#endif


/******************************************************************************
 *
 * Header info for Vector data structure
 *
 *****************************************************************************/

#ifndef hypre_VECTOR_HEADER
#define hypre_VECTOR_HEADER

/*--------------------------------------------------------------------------
 * hypre_Vector
 *--------------------------------------------------------------------------*/

typedef struct
{
   double  *data;
   int      size;

   /* Does the Vector create/destroy `data'? */
   int      owns_data;

   /* For multivectors...*/
   int   num_vectors;  /* the above "size" is size of one vector */
   int   multivec_storage_method;
   /* ...if 0, store colwise v0[0], v0[1], ..., v1[0], v1[1], ... v2[0]... */
   /* ...if 1, store rowwise v0[0], v1[0], ..., v0[1], v1[1], ... */
   /* With colwise storage, vj[i] = data[ j*size + i]
      With rowwise storage, vj[i] = data[ j + num_vectors*i] */
   int  vecstride, idxstride;
   /* ... so vj[i] = data[ j*vecstride + i*idxstride ] regardless of row_storage.*/

} hypre_Vector;

/*--------------------------------------------------------------------------
 * Accessor functions for the Vector structure
 *--------------------------------------------------------------------------*/

#define hypre_VectorData(vector)      ((vector) -> data)
#define hypre_VectorSize(vector)      ((vector) -> size)
#define hypre_VectorOwnsData(vector)  ((vector) -> owns_data)
#define hypre_VectorNumVectors(vector) ((vector) -> num_vectors)
#define hypre_VectorMultiVecStorageMethod(vector) ((vector) -> multivec_storage_method)
#define hypre_VectorVectorStride(vector) ((vector) -> vecstride )
#define hypre_VectorIndexStride(vector) ((vector) -> idxstride )

#endif

/* csr_matrix.c */
hypre_CSRMatrix *hypre_CSRMatrixCreate ( int num_rows , int num_cols , int num_nonzeros );
int hypre_CSRMatrixDestroy ( hypre_CSRMatrix *matrix );
int hypre_CSRMatrixInitialize ( hypre_CSRMatrix *matrix );
int hypre_CSRMatrixSetDataOwner ( hypre_CSRMatrix *matrix , int owns_data );
int hypre_CSRMatrixSetRownnz ( hypre_CSRMatrix *matrix );
hypre_CSRMatrix *hypre_CSRMatrixRead ( char *file_name );
int hypre_CSRMatrixPrint ( hypre_CSRMatrix *matrix , char *file_name );
int hypre_CSRMatrixCopy ( hypre_CSRMatrix *A , hypre_CSRMatrix *B , int copy_data );
hypre_CSRMatrix *hypre_CSRMatrixClone ( hypre_CSRMatrix *A );
hypre_CSRMatrix *hypre_CSRMatrixUnion ( hypre_CSRMatrix *A , hypre_CSRMatrix *B , int *col_map_offd_A , int *col_map_offd_B , int **col_map_offd_C );

/* csr_matvec.c */
int hypre_CSRMatrixMatvec ( double alpha , hypre_CSRMatrix *A , hypre_Vector *x , double beta , hypre_Vector *y );
int hypre_CSRMatrixMatvecT ( double alpha , hypre_CSRMatrix *A , hypre_Vector *x , double beta , hypre_Vector *y );
int hypre_CSRMatrixMatvec_FF( double alpha , hypre_CSRMatrix *A , hypre_Vector *x , double beta , hypre_Vector *y , int *CF_marker_x , int *CF_marker_y , int fpt );

/* vector.c */
hypre_Vector *hypre_SeqVectorCreate ( int size );
hypre_Vector *hypre_SeqMultiVectorCreate ( int size , int num_vectors );
int hypre_SeqVectorDestroy ( hypre_Vector *vector );
int hypre_SeqVectorInitialize ( hypre_Vector *vector );
int hypre_SeqVectorSetDataOwner ( hypre_Vector *vector , int owns_data );
hypre_Vector *hypre_SeqVectorRead ( char *file_name );
int hypre_SeqVectorPrint ( hypre_Vector *vector , char *file_name );
int hypre_SeqVectorSetConstantValues ( hypre_Vector *v , double value );
int hypre_SeqVectorCopy ( hypre_Vector *x , hypre_Vector *y );
hypre_Vector *hypre_SeqVectorCloneDeep ( hypre_Vector *x );
hypre_Vector *hypre_SeqVectorCloneShallow ( hypre_Vector *x );
int hypre_SeqVectorScale ( double alpha , hypre_Vector *y );
int hypre_SeqVectorAxpy ( double alpha , hypre_Vector *x , hypre_Vector *y );
double hypre_SeqVectorInnerProd ( hypre_Vector *x , hypre_Vector *y );
double hypre_VectorSumElts ( hypre_Vector *vector );

/* laplace.c */
hypre_CSRMatrix *GenerateSeqLaplacian(int nx, int ny , int nz , double *values , hypre_Vector **x_ptr, hypre_Vector **y_ptr, hypre_Vector **sol_ptr) ;

/* relax.c */
int hypre_BoomerAMGSeqRelax( hypre_CSRMatrix *A , hypre_Vector *x, hypre_Vector *y) ;

#ifdef __cplusplus
}
#endif

#endif

