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
 * $Revision: 2.10 $
 ***********************************************************************EHEADER*/



 
#include "headers.h"
 
/*--------------------------------------------------------------------------
 * hypre_GenerateLaplacian
 *--------------------------------------------------------------------------*/

hypre_CSRMatrix *
GenerateSeqLaplacian( int      nx,

                   int      ny,
                   int      nz, 
                   double  *value,
                   hypre_Vector **rhs_ptr,
                   hypre_Vector **x_ptr,
                   hypre_Vector **sol_ptr)
{
   hypre_CSRMatrix *A;
   hypre_Vector *rhs;
   hypre_Vector *x;
   hypre_Vector *sol;
   double *rhs_data;
   double *x_data;
   double *sol_data;

   int    *A_i;
   int    *A_j;
   double *A_data;

   int ix, iy, iz;
   int cnt;
   int row_index;
   int i, j;

   int grid_size;


   grid_size = nx*ny*nz;

   A_i = hypre_CTAlloc(int, grid_size+1);
   rhs_data = hypre_CTAlloc(double, grid_size);
   x_data = hypre_CTAlloc(double, grid_size);
   sol_data = hypre_CTAlloc(double, grid_size);

   for (i=0; i < grid_size; i++)
   {
      x_data[i] = 0.0;
      sol_data[i] = 0.0;
      rhs_data[i] = 1.0;
   }

   cnt = 1;
   A_i[0] = 0;
   for (iz = 0; iz < nz; iz++)
   {
      for (iy = 0;  iy < ny; iy++)
      {
         for (ix = 0; ix < nx; ix++)
         {
            A_i[cnt] = A_i[cnt-1];
            A_i[cnt]++;
            if (iz) 
               A_i[cnt]++;
            if (iy) 
               A_i[cnt]++;
            if (ix) 
               A_i[cnt]++;
            if (ix+1 < nx) 
               A_i[cnt]++;
            if (iy+1 < ny) 
               A_i[cnt]++;
            if (iz+1 < nz) 
               A_i[cnt]++;
            cnt++;
         }
      }
   }

   A_j = hypre_CTAlloc(int, A_i[grid_size]);
   A_data = hypre_CTAlloc(double, A_i[grid_size]);

   row_index = 0;
   cnt = 0;
   for (iz = 0; iz < nz; iz++)
   {
      for (iy = 0;  iy < ny; iy++)
      {
         for (ix = 0; ix < nx; ix++)
         {
            A_j[cnt] = row_index;
            A_data[cnt++] = value[0];
            if (iz) 
            {
               A_j[cnt] = row_index-nx*ny;
               A_data[cnt++] = value[3];
            }
            if (iy) 
            {
               A_j[cnt] = row_index-nx;
               A_data[cnt++] = value[2];
            }
            if (ix) 
            {
               A_j[cnt] = row_index-1;
               A_data[cnt++] = value[1];
            }
            if (ix+1 < nx) 
            {
               A_j[cnt] = row_index+1;
               A_data[cnt++] = value[1];
            }
            if (iy+1 < ny) 
            {
               A_j[cnt] = row_index+nx;
               A_data[cnt++] = value[2];
            }
            if (iz+1 < nz) 
            {
               A_j[cnt] = row_index+nx*ny;
               A_data[cnt++] = value[3];
            }
            row_index++;
         }
      }
   }

   A = hypre_CSRMatrixCreate(grid_size, grid_size,
                                A_i[grid_size]);
                                
   
   rhs = hypre_SeqVectorCreate(grid_size);
   hypre_VectorData(rhs) = rhs_data;

   x = hypre_SeqVectorCreate(grid_size);
   hypre_VectorData(x) = x_data;

   for (i=0; i < grid_size; i++)
      for (j=A_i[i]; j < A_i[i+1]; j++)
          sol_data[i] += A_data[j];

   sol = hypre_SeqVectorCreate(grid_size);
   hypre_VectorData(sol) = sol_data;

   hypre_CSRMatrixI(A) = A_i;
   hypre_CSRMatrixJ(A) = A_j;
   hypre_CSRMatrixData(A) = A_data;

   *rhs_ptr = rhs;
   *x_ptr = x;
   *sol_ptr = sol;

   return A;
}
