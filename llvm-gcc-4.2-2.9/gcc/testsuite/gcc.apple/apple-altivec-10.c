/* APPLE LOCAL file AltiVec */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-faltivec -Wno-format" } */
extern void abort();

#include <stdio.h>

int main()
{
  char str[128];
  vector unsigned char
  vuchar = (vector unsigned char)((vector unsigned int)(0x001f001d, 0x001f001d, 0x001f001d, 0x001f001d));

  vector unsigned short
  vushort =  (vector unsigned short)((vector unsigned int)(0x001f001d, 0x001f001d, 0x001f001d, 0x001f001d));

  vector unsigned char
  vushort_to_char = (vector unsigned char)
		     ((vector unsigned short)(0x001f,0x001d, 0x001f,0x001d, 0x001f,0x001d, 0x001f,0x001d));

  vector signed char
  vschar = (vector signed char)((vector signed int)(0x001f001d, 0x001f001d, 0x001f001d, 0x001f001d));

  vector signed short
  vsshort =  (vector signed short)((vector signed int)(0x001f001d, 0x001f001d, 0x001f001d, 0x001f001d));

  vector signed char
  vsshort_to_char = (vector signed char)
		     ((vector signed short)(0x001f,0x001d, 0x001f,0x001d, 0x001f,0x001d, 0x001f,0x001d));

  vector unsigned int
  vui = (vector unsigned int)((vector unsigned short)(0x011f,0x021d, 0x031f,0x041d, 0x051f,0x061d, 0x071f,0x081d));

  vector unsigned int
  vui1 = (vector unsigned int)
          ((vector unsigned char)(0x01,0x1f,0x02,0x1d, 0x03,0x1f,0x04,0x1d, 
				  0x05,0x1f,0x06,0x1d, 0x07,0x1f,0x08,0x1d));

   vector unsigned short
   vus = (vector unsigned short)
	  ((vector unsigned char)(0x01,0x1f,0x02,0x1d, 0x03,0x1f,0x04,0x1d, 
	  0x05,0x1f,0x06,0x1d, 0x07,0x1f,0x08,0x1d));

   vector float
   vf =   (vector float)
           ((vector unsigned int)(0x001f001d, 0x001f001d, 0x001f001d, 0x001f001d));

   sprintf(str, "%vhu", vuchar);
   if (strcmp (str, "31 29 31 29 31 29 31 29"))
     abort();
   sprintf(str, "%vhu", vushort);
   if (strcmp (str, "31 29 31 29 31 29 31 29"))
     abort();
   sprintf(str, "%vhu", vushort_to_char);
   if (strcmp (str, "31 29 31 29 31 29 31 29"))
     abort();
   sprintf(str, "%vhu", vschar);
   if (strcmp (str, "31 29 31 29 31 29 31 29"))
     abort();
   sprintf(str, "%vhu", vsshort);
   if (strcmp (str, "31 29 31 29 31 29 31 29"))
     abort();
   sprintf(str, "%vhu", vsshort_to_char);
   if (strcmp (str, "31 29 31 29 31 29 31 29"))
     abort();
   sprintf(str, "%vd", vui);
   if (strcmp (str, "1 31 2 29 3 31 4 29 5 31 6 29 7 31 8 29"))
     abort();
   sprintf(str, "%vd", vui1);
   if (strcmp (str, "1 31 2 29 3 31 4 29 5 31 6 29 7 31 8 29"))
     abort();
   sprintf(str, "%vd", vus);
   if (strcmp (str, "1 31 2 29 3 31 4 29 5 31 6 29 7 31 8 29"))
     abort();
   sprintf(str, "%vhu", vf);
   if (strcmp (str, "31 29 31 29 31 29 31 29"))
     abort();
   return 0;
}


