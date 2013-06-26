
/*!
***************************************************************************
*
* \file symbol.h
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

extern void (*writeMB_typeInfo)      (SyntaxElement *se, DataPartition *dP);
extern void (*writeIntraPredMode)    (SyntaxElement *se, DataPartition *dP);
extern void (*writeB8_typeInfo)      (SyntaxElement *se, DataPartition *dP);
extern void (*writeRefFrame[6])      (SyntaxElement *se, DataPartition *dP);
extern void (*writeMVD)              (SyntaxElement *se, DataPartition *dP);
extern void (*writeCBP)              (SyntaxElement *se, DataPartition *dP);
extern void (*writeDquant)           (SyntaxElement *se, DataPartition *dP);
extern void (*writeCIPredMode)       (SyntaxElement *se, DataPartition *dP);
extern void (*writeFieldModeInfo)    (SyntaxElement *se, DataPartition *dP);
extern void (*writeMB_transform_size)(SyntaxElement *se, DataPartition *dP);
