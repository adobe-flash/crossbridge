
/*!
 *************************************************************************************
 * \file header.h
 *
 * \brief
 *    Prototypes for header.c
 *************************************************************************************
 */

#ifndef _HEADER_H_
#define _HEADER_H_

int SliceHeader();
int Partition_BC_Header();

int  writeERPS(SyntaxElement *sym, DataPartition *partition);
// int  SequenceHeader(FILE *outf);

#endif

