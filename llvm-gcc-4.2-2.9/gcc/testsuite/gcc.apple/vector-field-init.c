/* APPLE LOCAL begin radar 4188876 */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-faltivec" } */
extern void abort();
int routine(void) 
{
	int i;
	float t = 1.23;
	struct
	{
	  float w __attribute((vector_size(16)));
	  vector float w1;
	} u = { { t, t, t, t }, { t, t, t, t } }; /* { dg-error "Initializer is a non-const vector type" } */

	struct
	{
	  int i;
	  struct inner {
	    vector float w1;
	  } IN;
	  double f;
	} u1 = { 1, {{ t, t, t, t }}, 1.2345 }; /* { dg-error "Initializer is a non-const vector type" } */

	union u
	{
	  float fa[4];
	  float vf __attribute((vector_size(16)));
	} uv;

	uv.vf = u.w;

	for (i=0; i < 3; i++)
	  if (uv.fa[0] != t)
	    return 1;
	return 0;
}

int main()
{
	if (routine())
	  abort();
	return 0;
}
/* APPLE LOCAL end radar 4188876 */
