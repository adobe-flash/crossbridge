/****  init.c  ********************************************************/

#include "global.h"
extern int GlobalInsert(char s[], int tok, int type, int size, int function,
                        int functionlabel);

/* contains all keywords to be added to the symbol table */
struct entry keywords[] = {
  { "int", INT, 0, 0, 0, 0 },
  { "float", FLOAT, 0, 0, 0, 0 },
  { "while", WHILE, 0, 0, 0, 0 },
  { "if", IF, 0, 0, 0, 0 },
  { "else", ELSE, 0, 0, 0, 0 },
  { "read", READ, 0, 0, 0, 0 },
  { "write", WRITE, 0, 0, 0, 0 },
  { "writeln", WRITELN, 0, 0, 0, 0 },
  { "return", RETURN, 0, 0, 0, 0 },
  { "readf", READF, 0, 0, 0, 0 },
  { "readi", READI, 0, 0, 0, 0 },
  { "writef", WRITEF, 0, 0, 0, 0 },
  { "writei", WRITEI, 0, 0, 0, 0 },
  {  0, 0, 0, 0, 0, 0 }
  };

void init(void)
{
  /* loads keywords into symtable */
  struct entry *p;
  for (p = keywords; p->token; p++)
    (void) GlobalInsert(p->lexptr, p->token, p->type, p->size, 0, 0);

  lineno = 1;
  LabelCounter = 1;
  DecCount = 2;
  ErrorFlag = 0;
  NumberC = 0;
}





