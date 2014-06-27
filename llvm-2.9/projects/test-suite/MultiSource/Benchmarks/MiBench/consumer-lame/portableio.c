/* Copyright (C) 1988-1991 Apple Computer, Inc.
 * All Rights Reserved.
 *
 * Warranty Information
 * Even though Apple has reviewed this software, Apple makes no warranty
 * or representation, either express or implied, with respect to this
 * software, its quality, accuracy, merchantability, or fitness for a 
 * particular purpose.  As a result, this software is provided "as is,"
 * and you, its user, are assuming the entire risk as to its quality
 * and accuracy.
 *
 * This code may be used and freely distributed as long as it includes
 * this copyright notice and the warranty information.
 *
 *
 * Motorola processors (Macintosh, Sun, Sparc, MIPS, etc)
 * pack bytes from high to low (they are big-endian).
 * Use the HighLow routines to match the native format
 * of these machines.
 *
 * Intel-like machines (PCs, Sequent)
 * pack bytes from low to high (the are little-endian).
 * Use the LowHigh routines to match the native format
 * of these machines.
 *
 * These routines have been tested on the following machines:
 *	Apple Macintosh, MPW 3.1 C compiler
 *	Apple Macintosh, THINK C compiler
 *	Silicon Graphics IRIS, MIPS compiler
 *	Cray X/MP and Y/MP
 *	Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 *
 * $Id: portableio.c 33045 2007-01-09 23:44:35Z lattner $
 *
 * $Log$
 * Revision 1.1  2007/01/09 23:44:35  lattner
 * Readd mibench
 *
 * Revision 1.1.1.1  2007/01/09 02:55:51  evancheng
 * Add selected tests from MiBench 1.0 to LLVM test suite.
 *
 * Revision 1.1.1.1  1999/11/24 08:43:35  markt
 * initial checkin of LAME
 * Starting with LAME 3.57beta with some modifications
 *
 * Revision 2.6  91/04/30  17:06:02  malcolm
 */

#include	<stdio.h>
#include	<math.h>
#include	"portableio.h"

/****************************************************************
 * Big/little-endian independent I/O routines.
 ****************************************************************/


int
ReadByte(FILE *fp)
{
	int	result;

	result = getc(fp) & 0xff;
	if (result & 0x80)
		result = result - 0x100;
	return result;
}


int
Read16BitsLowHigh(FILE *fp)
{
	int	first, second, result;

	first = 0xff & getc(fp);
	second = 0xff & getc(fp);

	result = (second << 8) + first;
#ifndef	THINK_C42
	if (result & 0x8000)
		result = result - 0x10000;
#endif	/* THINK_C */
	return(result);
}


int
Read16BitsHighLow(FILE *fp)
{
	int	first, second, result;

	first = 0xff & getc(fp);
	second = 0xff & getc(fp);

	result = (first << 8) + second;
#ifndef	THINK_C42
	if (result & 0x8000)
		result = result - 0x10000;
#endif	/* THINK_C */
	return(result);
}


void
Write8Bits(FILE *fp, int i)
{
	putc(i&0xff,fp);
}


void
Write16BitsLowHigh(FILE *fp, int i)
{
	putc(i&0xff,fp);
	putc((i>>8)&0xff,fp);
}


void
Write16BitsHighLow(FILE *fp, int i)
{
	putc((i>>8)&0xff,fp);
	putc(i&0xff,fp);
}


int
Read24BitsHighLow(FILE *fp)
{
	int	first, second, third;
	int	result;

	first = 0xff & getc(fp);
	second = 0xff & getc(fp);
	third = 0xff & getc(fp);

	result = (first << 16) + (second << 8) + third;
	if (result & 0x800000)
		result = result - 0x1000000;
	return(result);
}

#define	Read32BitsLowHigh(f)	Read32Bits(f)


int
Read32Bits(FILE *fp)
{
	int	first, second, result;

	first = 0xffff & Read16BitsLowHigh(fp);
	second = 0xffff & Read16BitsLowHigh(fp);

	result = (second << 16) + first;
#ifdef	CRAY
	if (result & 0x80000000)
		result = result - 0x100000000;
#endif	/* CRAY */
	return(result);
}


int
Read32BitsHighLow(FILE *fp)
{
	int	first, second, result;

	first = 0xffff & Read16BitsHighLow(fp);
	second = 0xffff & Read16BitsHighLow(fp);

	result = (first << 16) + second;
#ifdef	CRAY
	if (result & 0x80000000)
		result = result - 0x100000000;
#endif
	return(result);
}


void
Write32Bits(FILE *fp, int i)
{
	Write16BitsLowHigh(fp,(int)(i&0xffffL));
	Write16BitsLowHigh(fp,(int)((i>>16)&0xffffL));
}


void
Write32BitsLowHigh(FILE *fp, int i)
{
	Write16BitsLowHigh(fp,(int)(i&0xffffL));
	Write16BitsLowHigh(fp,(int)((i>>16)&0xffffL));
}


void
Write32BitsHighLow(FILE *fp, int i)
{
	Write16BitsHighLow(fp,(int)((i>>16)&0xffffL));
	Write16BitsHighLow(fp,(int)(i&0xffffL));
}

void ReadBytes(FILE	*fp, char *p, int n)
{
	while (!feof(fp) & (n-- > 0))
		*p++ = getc(fp);
}

void ReadBytesSwapped(FILE *fp, char *p, int n)
{
	register char	*q = p;

	while (!feof(fp) & (n-- > 0))
		*q++ = getc(fp);

	for (q--; p < q; p++, q--){
		n = *p;
		*p = *q;
		*q = n;
	}
}

void WriteBytes(FILE *fp, char *p, int n)
{
	while (n-- > 0)
		putc(*p++, fp);
}

void WriteBytesSwapped(FILE *fp, char *p, int n)
{
	p += n-1;
	while (n-- > 0)
		putc(*p--, fp);
}

defdouble
ReadIeeeFloatHighLow(FILE *fp)
{
	char	bits[kFloatLength];

	ReadBytes(fp, bits, kFloatLength);
	return ConvertFromIeeeSingle(bits);
}

defdouble
ReadIeeeFloatLowHigh(FILE *fp)
{
	char	bits[kFloatLength];

	ReadBytesSwapped(fp, bits, kFloatLength);
	return ConvertFromIeeeSingle(bits);
}

defdouble
ReadIeeeDoubleHighLow(FILE *fp)
{
	char	bits[kDoubleLength];

	ReadBytes(fp, bits, kDoubleLength);
	return ConvertFromIeeeDouble(bits);
}

defdouble
ReadIeeeDoubleLowHigh(FILE *fp)
{
	char	bits[kDoubleLength];

	ReadBytesSwapped(fp, bits, kDoubleLength);
	return ConvertFromIeeeDouble(bits);
}

defdouble
ReadIeeeExtendedHighLow(FILE *fp)
{
	char	bits[kExtendedLength];

	ReadBytes(fp, bits, kExtendedLength);
	return ConvertFromIeeeExtended(bits);
}

defdouble
ReadIeeeExtendedLowHigh(FILE *fp)
{
	char	bits[kExtendedLength];

	ReadBytesSwapped(fp, bits, kExtendedLength);
	return ConvertFromIeeeExtended(bits);
}

void
WriteIeeeFloatLowHigh(FILE *fp, defdouble num)
{
	char	bits[kFloatLength];

	ConvertToIeeeSingle(num,bits);
	WriteBytesSwapped(fp,bits,kFloatLength);
}

void
WriteIeeeFloatHighLow(FILE *fp, defdouble num)
{
	char	bits[kFloatLength];

	ConvertToIeeeSingle(num,bits);
	WriteBytes(fp,bits,kFloatLength);
}

void
WriteIeeeDoubleLowHigh(FILE *fp, defdouble num)
{
	char	bits[kDoubleLength];

	ConvertToIeeeDouble(num,bits);
	WriteBytesSwapped(fp,bits,kDoubleLength);
}

void
WriteIeeeDoubleHighLow(FILE *fp, defdouble num)
{
	char	bits[kDoubleLength];

	ConvertToIeeeDouble(num,bits);
	WriteBytes(fp,bits,kDoubleLength);
}

void
WriteIeeeExtendedLowHigh(FILE *fp, defdouble num)
{
	char	bits[kExtendedLength];

	ConvertToIeeeExtended(num,bits);
	WriteBytesSwapped(fp,bits,kExtendedLength);
}


void
WriteIeeeExtendedHighLow(FILE *fp, defdouble num)
{
	char	bits[kExtendedLength];

	ConvertToIeeeExtended(num,bits);
	WriteBytes(fp,bits,kExtendedLength);
}


