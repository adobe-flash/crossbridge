/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-maltivec -faltivec" } */

extern void abort();

int main()
{
        vector unsigned int tiny = (vector unsigned int) ((vector signed int) (0x00800000, 0xBF800000, 0x3E800000, 0));
        unsigned int af[4] = {
		       (signed int)0x00800000,
                       (signed int)0xBF800000,
                       (signed int)0x3E800000,
                       (signed int)0};

	union u {
	  vector unsigned int vf;
	  unsigned int   ai[4];
	}vu;
	int i;

	vu.vf = tiny;

	for (i=0; i < 4; i++)
	   if (af[i] != vu.ai[i])
	     abort();

	return 0;
}

