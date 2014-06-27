/* id.cpp */
/*
   Java Decompiler 
   Copyright (c) 1994-2003, Pete Ryland.
   Distributed under the GNU GPL Version 2.
   This package is available from http://pdr.cx/hbd/
*/

#include "id.h"

Id idnull  = {"null",  OBJECT,  IM, 0},
   idneg1  = {"-1",    INT,     IM, 0},
   id0i    = {"0",     INT,     IM, 0},
   id1i    = {"1",     INT,     IM, 0},
   id2i    = {"2",     INT,     IM, 0},
   id3i    = {"3",     INT,     IM, 0},
   id4i    = {"4",     INT,     IM, 0},
   id5i    = {"5",     INT,     IM, 0},
   id0L    = {"0L",    LONG,    IM, 0},
   id1L    = {"1L",    LONG,    IM, 0},
   id0f    = {"0.0f",  FLOAT,   IM, 0},
   id1f    = {"1.0f",  FLOAT,   IM, 0},
   id2f    = {"2.0f",  FLOAT,   IM, 0},
   id0d    = {"0.0d",  DOUBLE,  IM, 0},
   id1d    = {"1.0d",  DOUBLE,  IM, 0},
   idfalse = {"false", BOOLEAN, IM, 0},
   idtrue  = {"true",  BOOLEAN, IM, 0};
