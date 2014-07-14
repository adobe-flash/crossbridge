/* Copyright (c) 1991 Sun Wu and Udi Manber.  All Rights Reserved. */
#include "agrep.h"

extern unsigned Init1, Init[], Mask[], endposition, D_endpos, AND, NO_ERR_MASK;
extern int DELIMITER, FILENAMEONLY, INVERSE;
extern CHAR CurrentFileName[];
extern int I, num_of_matched, TRUNCATE;

extern int fill_buf(int fd, unsigned char *buf, int record_size);
extern void output(register CHAR *buffer, int i1, int i2, int j);

void asearch0(CHAR old_D_pat[], int text, register unsigned D)
{
  register unsigned i, c, r1, r2, CMask, r_NO_ERR, r_Init1,  end, endpos; 
  unsigned A[MaxError+2], B[MaxError+2];
  unsigned D_Mask;
  int D_length, FIRSTROUND, ResidueSize, lasti, l, k, j=0;
  int printout_end;
  CHAR buffer[BlockSize+Max_record+1];
  
  D_length = strlen(old_D_pat);
  buffer[Max_record-1] = '\n';
  D_Mask = D_endpos;
  for ( i=1; i<D_length; i++) D_Mask = (D_Mask<<1) | D_Mask;
  D_Mask = ~D_Mask;

  r_Init1 = Init1; /* put Init1 in register */
  r_NO_ERR = NO_ERR_MASK; /* put NO_ERR_MASK in register */
  endpos = D_endpos;    
  FIRSTROUND = ON;
  for(k=0; k<=D; k++) A[k] = B[k] = Init[0];
  lasti = Max_record;

  while ((l = fill_buf(text, buffer + Max_record, Max_record)) > 0)
  { i = Max_record; end = Max_record + l ;
    if (FIRSTROUND) { 
        i = Max_record - 1;
        FIRSTROUND = OFF; }
    if (l < BlockSize) {
        strncpy(buffer+end, old_D_pat, D_length);
        buffer[end+D_length] = '\0';
        end = end + D_length; }
    while (i < end )
    {
        c = buffer[i++];
        CMask = Mask[c];
              r1 = B[0] & r_Init1;
              A[0] = (((B[0] >> 1)) & CMask | r1 ) ;
              for(k=1; k<=D; k++) {
                     r1 = r_Init1 & B[k];
                     r2 = B[k-1] | (((A[k-1]|B[k-1])>>1) & r_NO_ERR);
                     A[k] = ((B[k] >> 1) & CMask) | r2 | r1;
              }
        if(A[0] & endpos) {
           j++;  
           r1 = A[D];
           if(((AND == 1) && ((r1 & endposition) == endposition)) ||                           ((AND == 0) && (r1 & endposition)) ^ INVERSE )
                 {    
                      if(FILENAMEONLY) {
                         num_of_matched++;
                         printf("%s\n", CurrentFileName);
                         return;  }
                      printout_end = i - D_length - 1;           
                      if(!(lasti >= Max_record + l - 1))
                         output(buffer, lasti, printout_end, j);
                 }
           lasti = i - D_length; /* point to starting position of D_pat */
           for(k=0; k<= D; k++) {
              B[k] = Init[0];
           }
           r1 = B[0] & r_Init1;
           A[0] = (((B[0]>>1) & CMask) | r1) & D_Mask;
           for(k=1; k<= D; k++) {
              r1 = Init1 & B[k];
              r2 = B[k-1] | (((A[k-1] | B[k-1])>>1)&r_NO_ERR);
              A[k] = (((B[k]>>1)&CMask) | r1 | r2) ;
           }
        }
        c = buffer[i++];
        CMask = Mask[c];
              r1   = r_Init1 & A[0];
              B[0] = ((A[0] >> 1 ) & CMask) | r1;
              for(k=1; k<=D; k++) {
                     r1 = r_Init1 & A[k];
                     r2 = A[k-1] | (((A[k-1]|B[k-1])>>1) & r_NO_ERR);
                     B[k] = ((A[k] >> 1) & CMask) | r2 | r1;
              }
        if(B[0] & endpos) {
           j++;  
           r1 = B[D];
           if(((AND == 1) && ((r1 & endposition) == endposition)) ||                           ((AND == 0) && (r1 & endposition)) ^ INVERSE )
                 { 
                    if(FILENAMEONLY) {
                       num_of_matched++;
                       printf("%s\n", CurrentFileName);
                       return; }
                    printout_end = i - D_length -1 ; 
                    if(!(lasti >= Max_record + l - 1))
                       output(buffer, lasti, printout_end, j);
                 }
           lasti = i - D_length ;
           for(k=0; k<= D; k++) {
              A[k] = Init[0];
           }
           r1 = A[0] & r_Init1; 
           B[0] = (((A[0]>>1)&CMask) | r1) & D_Mask;
           for(k=1; k<= D; k++) {
              r1 = r_Init1 & A[k];
              r2 = A[k-1] | (((A[k-1] | B[k-1])>>1)&r_NO_ERR);
              B[k] = (((A[k]>>1)&CMask) | r1 | r2) ;
           }
        }
    }
    if(l < BlockSize) {
           lasti = Max_record;
    }
    else {
       ResidueSize = Max_record + l - lasti;
       if(ResidueSize > Max_record) {
          ResidueSize = Max_record;
          TRUNCATE = ON;         }
       strncpy(buffer+Max_record-ResidueSize, buffer+lasti, ResidueSize);
       lasti = Max_record - ResidueSize;
       if(lasti == 0)     lasti = 1; 
    }
  }
  return;
}

void asearch(CHAR old_D_pat[], int text, register unsigned D)
{
  register unsigned i, c, r1, r2, CMask, r_NO_ERR, r_Init1; 
  register unsigned A0, B0, A1, B1, endpos;
  unsigned A2, B2, A3, B3, A4, B4;
  unsigned A[MaxError+1], B[MaxError+1];
  unsigned D_Mask;
  unsigned end;
  int D_length, FIRSTROUND, ResidueSize, lasti, l, k, j=0;
  int printout_end;
  CHAR buffer[2*Max_record+1];
     
  if (I == 0) Init1 = 037777777777;
  if(D > 4) {
         asearch0(old_D_pat, text, D); 
         return;  }
  D_length = strlen(old_D_pat);
  buffer[Max_record-1] = '\n';
  D_Mask = D_endpos;
  for ( i=1; i<D_length; i++) D_Mask = (D_Mask<<1) | D_Mask;
  D_Mask = ~D_Mask;

  r_Init1 = Init1; /* put Init1 in register */
  r_NO_ERR = NO_ERR_MASK; /* put NO_ERR_MASK in register */
  endpos = D_endpos;    
  FIRSTROUND = ON;
  A0 = B0 = A1 = B1 = A2 = B2 = A3 = B3 = A4 = B4 = Init[0];
  for(k=0; k<=D; k++) A[k] = B[k] = Init[0];
  lasti = Max_record;

  while ((l = fill_buf(text, buffer + Max_record, Max_record)) > 0)
  { i = Max_record; end = Max_record + l ;
    if (FIRSTROUND) { 
        i = Max_record - 1;
	if(DELIMITER) {
		for(k=0; k<D_length; k++) {
					if(old_D_pat[k] != buffer[Max_record+k]) 						break;
		}
		if(k>=D_length) j--;
	}
        FIRSTROUND = OFF; }
    if (l < BlockSize) {
        strncpy(buffer+end, old_D_pat, D_length);
        buffer[end+D_length] = '\0';
        end = end + D_length; }
    while (i < end )
    {
        c = buffer[i];
        CMask = Mask[c];
              r1 = r_Init1 & B0;
              A0 = ((B0 >>1 ) & CMask) | r1;
              r1 = r_Init1 & B1;
              r2 =  B0 | (((A0 | B0) >> 1) & r_NO_ERR); 
              A1 = ((B1 >>1 ) & CMask) | r2 | r1 ;  
                     if(D == 1) goto Nextchar;
              r1 = r_Init1 & B2;
              r2 =  B1 | (((A1 | B1) >> 1) & r_NO_ERR); 
              A2 = ((B2 >>1 ) & CMask) | r2 | r1 ;  
                     if(D == 2) goto Nextchar;
              r1 = r_Init1 & B3;
              r2 =  B2 | (((A2 | B2) >> 1) & r_NO_ERR); 
              A3 = ((B3 >>1 ) & CMask) | r2 | r1 ;  
                     if(D == 3) goto Nextchar;
              r1 = r_Init1 & B4;
              r2 =  B3 | (((A3 | B3) >> 1) & r_NO_ERR); 
              A4 = ((B4 >>1 ) & CMask) | r2 | r1 ;  
                     if(D == 4) goto Nextchar;
Nextchar: i=i+1;
        if(A0 & endpos) {
           j++;  r1 = A0;
           if ( D == 1) r1 = A1;
           if ( D == 2) r1 = A2;
           if ( D == 3) r1 = A3;
           if ( D == 4) r1 = A4;
           if(((AND == 1) && ((r1 & endposition) == endposition)) ||                           ((AND == 0) && (r1 & endposition)) ^ INVERSE )
                 {    
                      if(FILENAMEONLY) {
                         num_of_matched++;
                         printf("%s\n", CurrentFileName);
                         return;  }
                      printout_end = i - D_length - 1;           
                      if(!(lasti >= Max_record + l - 1))
                         output(buffer, lasti, printout_end, j);
                 }
           lasti = i - D_length; /* point to starting position of D_pat */
           TRUNCATE = OFF;
           for(k=0; k<= D; k++) {
              B[k] = Init[0];
           }
           r1 = B[0] & Init1;
           A[0] = (((B[0]>>1) & CMask) | r1) & D_Mask;
           for(k=1; k<= D; k++) {
              r1 = Init1 & B[k];
              r2 = B[k-1] | (((A[k-1] | B[k-1])>>1)&r_NO_ERR);
              A[k] = (((B[k]>>1)&CMask) | r1 | r2) ;
           }
           A0 = A[0]; B0 = B[0]; A1 = A[1]; B1 = B[1]; A2 = A[2]; B2 = B[2];
           A3 = A[3]; B3 = B[3]; A4 = A[4]; B4 = B[4];
        }
        c = buffer[i];
        CMask = Mask[c];
              r1 = r_Init1 & A0;
              B0 = ((A0 >> 1 ) & CMask) | r1;
#ifdef DEBUG
	printf("Mask = %o, B0 = %o\n", CMask, B0);
#endif
              r1 = r_Init1 & A1;
              r2 =  A0 | (((A0 | B0) >> 1) & r_NO_ERR); 
              B1 = ((A1 >>1 ) & CMask) | r2 | r1 ;  
                     if(D == 1) goto Nextchar1;
              r1 = r_Init1 & A2;
              r2 =  A1 | (((A1 | B1) >> 1) & r_NO_ERR); 
              B2 = ((A2 >>1 ) & CMask) | r2 | r1 ;  
                     if(D == 2) goto Nextchar1;
              r1 = r_Init1 & A3;
              r2 =  A2 | (((A2 | B2) >> 1) & r_NO_ERR); 
              B3 = ((A3 >>1 ) & CMask) | r2 | r1 ;  
                     if(D == 3) goto Nextchar1;
              r1 = r_Init1 & A4;
              r2 =  A3 | (((A3 | B3) >> 1) & r_NO_ERR); 
              B4 = ((A4 >>1 ) & CMask) | r2 | r1 ;  
                     if(D == 4) goto Nextchar1;
Nextchar1: i=i+1;
        if(B0 & endpos) {
           j++;  r1 = B0;
           if ( D == 1) r1 = B1;
           if ( D == 2) r1 = B2;
           if ( D == 3) r1 = B3;
           if ( D == 4) r1 = B4;
           if(((AND == 1) && ((r1 & endposition) == endposition)) ||                           ((AND == 0) && (r1 & endposition)) ^ INVERSE )
                 { 
                    if(FILENAMEONLY) {
                       num_of_matched++;
                       printf("%s\n", CurrentFileName);
                       return; }
                    printout_end = i - D_length -1 ; 
                    if(!(lasti >= Max_record + l - 1))
                       output(buffer, lasti, printout_end, j);
                 }
           lasti = i - D_length ;
           TRUNCATE = OFF;
           for(k=0; k<= D; k++) {
              A[k] = Init[0];
           }
           r1 = A[0] & Init1; 
           B[0] = (((A[0]>>1)&CMask) | r1) & D_Mask;
           for(k=1; k<= D; k++) {
              r1 = Init1 & A[k];
              r2 = A[k-1] | (((A[k-1] | B[k-1])>>1)&r_NO_ERR);
              B[k] = (((A[k]>>1)&CMask) | r1 | r2) ;
           }
           A0 = A[0]; B0 = B[0]; A1 = A[1]; B1 = B[1]; A2 = A[2]; B2 = B[2];
           A3 = A[3]; B3 = B[3]; A4 = A[4]; B4 = B[4];
        }
    }
    if(l < BlockSize) {
           lasti = Max_record ;
    }
    else {
       ResidueSize = Max_record + l - lasti;
       if(ResidueSize > Max_record) {
          ResidueSize = Max_record;
          TRUNCATE = ON;         }
       strncpy(buffer+Max_record-ResidueSize, buffer+lasti, ResidueSize);
       lasti = Max_record - ResidueSize;
       if(lasti == 0)     lasti = 1; 
    }
  }
  return;
}




/*
int fill_buf(int fd, unsigned char *buf, int record_size)
{
int num_read=1;
int total_read=0;
	while(total_read < record_size && num_read > 0) {
		num_read = read(fd, buf+total_read, 4096);
		total_read = total_read + num_read;
	}
	return(total_read);
}
*/
