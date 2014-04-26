/* APPLE LOCAL begin radar 4232296 */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-mfix-and-continue -O2" } */

static unsigned char _gammaLUT[256];

static void SillyTest()
{
 _gammaLUT[0] = 0;
}


static void BuildYUVDataFromNormal()
{
  int i;

   for ( i = 0; i < 256; i += 1 )
    _gammaLUT[i] = i;
}

void PluginStart();
void PluginStart() {
 BuildYUVDataFromNormal();
}

int	main()
{
	SillyTest();
	return 0;
}
/* APPLE LOCAL end radar 4232296 */
