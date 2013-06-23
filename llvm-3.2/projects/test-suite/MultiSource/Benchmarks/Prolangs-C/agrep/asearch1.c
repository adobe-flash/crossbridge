/* Copyright (c) 1991 Sun Wu and Udi Manber.  All Rights Reserved. */
#include "agrep.h"
 
extern unsigned Init1, Init[], Mask[], endposition, D_endpos;
extern unsigned NO_ERR_MASK;
extern int TRUNCATE, DELIMITER, AND, I, S, DD, INVERSE, FILENAMEONLY ;
extern char CurrentFileName[];
extern int num_of_matched;

extern int fill_buf(int fd, unsigned char *buf, int record_size);
extern void output(register CHAR *buffer, int i1, int i2, int j);

void asearch1(char old_D_pat[], int Text, register unsigned D)
{
  register unsigned end, i, r1, r3, r4, r5, CMask, D_Mask, k, endpos; 
  register unsigned r_NO_ERR;
  unsigned A[MaxError*2+1], B[MaxError*2+1];
  int D_length, ResidueSize, lasti, num_read,  FIRSTROUND, j=0;
  char buffer[BlockSize+Max_record+1];
   
  if(I == 0) Init1 = 037777777777;
  if(DD > D) DD = D+1;
  if(I  > D) I  = D+1;
  if(S  > D) S  = D+1;
  D_length = strlen(old_D_pat);
  buffer[Max_record-1] = '\n';
   
  lasti = Max_record;
  r_NO_ERR = NO_ERR_MASK;

  D_Mask = D_endpos;
  for(i=1 ; i<D_length; i++) D_Mask = (D_Mask << 1) | D_Mask;
  D_Mask = ~D_Mask;
  endpos = D_endpos;
  r3 = D+1; r4 = D*2;  /* to make sure in register */
  for(k=0; k < D;   k++) A[k] = B[k] = 0;
  for(k=D; k <= r4; k++) A[k] = B[k] = Init[0];
   
  while ((num_read = fill_buf(Text, buffer + Max_record, Max_record)) > 0)
  {
    i=Max_record; end = Max_record + num_read;
    if(FIRSTROUND) { i = Max_record -1 ;
			if(DELIMITER) {
				for(k=0; k<D_length; k++) {
					if(old_D_pat[k] != buffer[Max_record+k]) 						break;
				}
				if(k>=D_length) j--;
			}
                     FIRSTROUND = 0; }
    if(num_read < BlockSize) {
                      strncpy(buffer+Max_record+num_read, old_D_pat, D_length);
                      end = end + D_length;
                      buffer[end] = '\0';
    }
    while (i < end)
    {
        CMask = Mask[buffer[i++]];
              r1 = Init1 & B[D];
              A[D] = ((B[D] >> 1) & CMask )  | r1;
              for(k = r3; k <= r4; k++)  /* r3 = D+1, r4 = 2*D */
              { 
                  r5 = B[k];
                  r1 = Init1 & r5;
                  A[k] = ((r5 >> 1) & CMask) | B[k-I] |                                                (((A[k-DD] | B[k-S]) >>1) & r_NO_ERR) | r1 ; 
              }
        if(A[D] & endpos) {  
           j++;
           if(((AND == 1) && ((A[D*2] & endposition) == endposition)) ||                           ((AND == 0) && (A[D*2] & endposition)) ^ INVERSE )
                   { 
                     if(FILENAMEONLY) {
			num_of_matched++;
                        printf("%s\n", CurrentFileName);
                        return;       } 
                     if(lasti < Max_record + num_read)
                        output(buffer, lasti, i-D_length-1, j); 
                   }
           lasti = i - D_length;
           TRUNCATE = OFF;
           for(k = D; k <= r4 ; k++) A[k] = B[k] = Init[0];
           r1 = Init1 & B[D];
           A[D] = (((B[D] >> 1) & CMask )  | r1) & D_Mask;
           for(k = r3; k <= r4; k++)  /* r3 = D+1, r4 = 2*D */
              { 
                  r5 = B[k];
                  r1 = Init1 & r5;
                  A[k] = ((r5 >> 1) & CMask) | B[k-I] |                                                (((A[k-DD] | B[k-S]) >>1) & r_NO_ERR) | r1 ; 
              }
        }  /* end if (A[D]&endpos) */
        CMask = Mask[buffer[i++]];
              r1 = A[D] & Init1;
              B[D] = ((A[D] >> 1) & CMask) | r1;
              for(k = r3; k <= r4; k++)
              { 
                  r1 = A[k] & Init1;
                  B[k] = ((A[k] >> 1) & CMask) | A[k-I] |                                                (((B[k-DD] | A[k-S]) >>1)&r_NO_ERR) | r1 ; 
              }
        if(B[D] & endpos)  {  
             j++;
           if(((AND == 1) && ((B[r4] & endposition) == endposition)) ||                           ((AND == 0) && (B[r4] & endposition)) ^ INVERSE )
                   { if(FILENAMEONLY) {
                        num_of_matched++;
                        printf("%s\n", CurrentFileName);
                        return;       }
                     if(lasti < Max_record + num_read)
                        output(buffer, lasti, i-D_length-1, j); 
                   } 
           lasti = i-D_length; 
           TRUNCATE = OFF;
           for(k=D; k <= r4; k++) A[k] = B[k] = Init[0];
           r1 = Init1 & A[D];
           B[D] = (((A[D] >> 1) & CMask )  | r1) & D_Mask;
           for(k = r3; k <= r4; k++)  /* r3 = D+1, r4 = 2*D */
              { 
                  r5 = A[k];
                  r1 = Init1 & r5;
                  B[k] = ((r5 >> 1) & CMask) | A[k-I] |                                                (((B[k-DD] | A[k-S]) >>1) & r_NO_ERR) | r1 ; 
              }
        }  /* end if (B[D]&endpos) */
    }
    ResidueSize = Max_record + num_read - lasti;
    if(ResidueSize > Max_record) {
            ResidueSize = Max_record;
            TRUNCATE = ON;   
    }
    strncpy(buffer+Max_record-ResidueSize, buffer+lasti, ResidueSize);
    lasti = Max_record - ResidueSize;
    if(lasti < 0) lasti = 1;
    if(num_read < BlockSize) lasti = Max_record;
  }
  return;
}

