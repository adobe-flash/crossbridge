
/*!
***************************************************************************
*
* \file symbol.c
*
* \brief
*    Generic Symbol writing interface
*
* \date
*    18 Jan 2006
*
* \author
*    Karsten Suehring   suehring@hhi.de
**************************************************************************/

#include "global.h"
#include "symbol.h"

void (*writeMB_typeInfo)      (SyntaxElement *se, DataPartition *dP);
void (*writeIntraPredMode)    (SyntaxElement *se, DataPartition *dP);
void (*writeB8_typeInfo)      (SyntaxElement *se, DataPartition *dP);
void (*writeRefFrame[6])      (SyntaxElement *se, DataPartition *dP);
void (*writeMVD)              (SyntaxElement *se, DataPartition *dP);
void (*writeCBP)              (SyntaxElement *se, DataPartition *dP);
void (*writeDquant)           (SyntaxElement *se, DataPartition *dP);
void (*writeCIPredMode)       (SyntaxElement *se, DataPartition *dP);
void (*writeFieldModeInfo)    (SyntaxElement *se, DataPartition *dP);
void (*writeMB_transform_size)(SyntaxElement *se, DataPartition *dP);
