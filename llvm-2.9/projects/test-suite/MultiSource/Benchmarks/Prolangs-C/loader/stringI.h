/* %%%%%%%%%%%%%%%%%%%% (c) William Landi 1991 %%%%%%%%%%%%%%%%%%%%%%%%%%%% */
/* Permission to use this code is granted as long as the copyright */
/* notice remains in place. */
/* =============================== stringI.h =============================== */
/* Reads in one line of input gauranteeing that it will return a pointer to  */
/* the whole line, no matter how long it is (it uses CALLOC a version of     */
/* MALLOC).                                                                  */

/* --------------------------------- GET_LINE ------------------------------ */
/* 2 parameters:                                                             */
/*  1) char **LINE;               Points to the input line (string) when done*/
/*  2) FILE *STREAM;              Where to get the line from.                */
/* Read in a line from the input stream.                                     */
extern void GET_LINE();

