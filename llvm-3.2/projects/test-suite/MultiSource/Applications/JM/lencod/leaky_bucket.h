
/*!
 ***************************************************************************
 *
 * \file leaky_bucket.h
 *
 * \brief
 *    Header for Leaky Buffer parameters
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Shankar Regunathan                   <shanre@microsoft.com>
 **************************************************************************/

#ifndef _LEAKY_BUCKET_H_
#define _LEAKY_BUCKET_H_


/* Leaky Bucket Parameter Optimization */
#ifdef _LEAKYBUCKET_
int get_LeakyBucketRate(unsigned long NumberLeakyBuckets, unsigned long *Rmin);
void PutBigDoubleWord(unsigned long dw, FILE *fp);
void write_buffer(unsigned long NumberLeakyBuckets, unsigned long Rmin[], unsigned long Bmin[], unsigned long Fmin[]);
void Sort(unsigned long NumberLeakyBuckets, unsigned long *Rmin);
void calc_buffer();
#endif

#endif

