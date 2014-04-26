/* APPLE LOCAL file AltiVec */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */

extern void abort();

vector signed int
vandc(vector signed int a, vector signed int b)
{
        return vec_andc(a, b);
}

int main ()
{
	char buf [1024];
	vector signed int a1 = (vector signed int) (0XFFFFFFFF);
	vector signed int b1 = (vector signed int) (0X0);

 	if (!vec_all_eq (vandc (a1, b1), a1))
	  abort();
	return 0;
}

