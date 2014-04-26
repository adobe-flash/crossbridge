/* APPLE LOCAL begin radar 4125900 */
/* Test interaction of optimization on memory references in CW asm block. */

/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks -O2" } */

void Process()
{
	long BufferSize;
	long ControlRate;
	long lBlocks=(BufferSize/ControlRate)+(BufferSize?1:0);

	asm
	{
  	  lwz r23, lBlocks
  	  lwz r24, BufferSize
	}
}                                                                            
/* APPLE LOCAL end radar 4125900 */
