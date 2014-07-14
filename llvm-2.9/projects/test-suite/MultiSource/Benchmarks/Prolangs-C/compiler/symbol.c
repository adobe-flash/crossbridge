/****  symbol.c  *****************************************************/

#include "global.h"

extern void error(char *m);
extern void emit(int t, int tval, float rval);

char Glexemes[STRMAX]; /* char array to hold global lexemes */
char Llexemes[STRMAX]; /* char array to hold local lexemes */
/* struct entry LocalTable[SYMMAX];  define the size of the local table */
/* struct entry GlobalTable[SYMMAX]; define the size of the global table */
int Glastchar = - 1;   /* last used position in Glexemes */
int Llastchar = - 1;   /* last used position in Llexemes */
int Glastentry = 0;    /* last used position in global table */
int Llastentry = 0;   /* last usedentry in local table */

/* ---------------------------------------------------------------- */

void OutputLocal(void) /* function used for testing only */
{
  int i; /* temporary index */

  printf("\n\nLOCAL SYMBOL TABLE\n------------------\n");
  for (i = 0; i <= Llastentry; i++)
  {
    printf("%s\t%d\t%d\t%d\t%d\t%d\n", LocalTable[i].lexptr,
          LocalTable[i].token,
          LocalTable[i].type,
          LocalTable[i].size,
          LocalTable[i].function,
          LocalTable[i].functionlabel);
  } /* end for */
  printf("\n\n");
} /* end OutputLocal */

/* ----------------------------------------------------------------- */

void OutputGlobal(void) /* function used for testing only */
{
  int i; /* temporary index */

  printf("\n\nGLOBAL SYMBOL TABLE\n-------------------\n");
  for (i = 0; i <= Glastentry; i++)
  {
    printf("%s\t%d\t%d\t%d\t%d\t%d\n", 
          GlobalTable[i].lexptr,
          GlobalTable[i].token,
          GlobalTable[i].type,
          GlobalTable[i].size,
          GlobalTable[i].function,
          GlobalTable[i].functionlabel);
  } /* end for */
  printf("\n\n");
} /* end OutputGlobal */

/* ----------------------------------------------------------------- */


int GlobalLookup(char s[])         /* returns position of entry for s */
{
  int p;
  for (p = Glastentry; p > 0; p = p - 1)
    if (strcmp(GlobalTable[p].lexptr, s) == 0)
      return p;
  return 0;
}

/* ----------------------------------------------------------------- */

int LocalLookup(char s[])         /* returns position of entry for s */
{
  int p;
  for (p = Llastentry; p > 0; p = p - 1)
    if (strcmp(LocalTable[p].lexptr, s) == 0)
      return p;
  return 0;
}

/* ----------------------------------------------------------------- */

int GlobalInsert(char s[], int tok, int type, int size, int function, int functionlabel)

 /* returns position of entry for s */
{
  int len;

  len = strlen(s);     /* strlen computes length of s */
  if (Glastentry + 1 >= SYMMAX)
  {
    error("symbol table full");
    return (0);
  }
  if (Glastchar + len + 1 >= STRMAX)
  {
    error("lexemes array full");
    return (0);
  }
  Glastentry = Glastentry + 1;
  GlobalTable[Glastentry].token = tok;
  GlobalTable[Glastentry].lexptr = &Glexemes[Glastchar + 1];
  Glastchar = Glastchar + len + 1;
  (void) strcpy(GlobalTable[Glastentry].lexptr, s);
  GlobalTable[Glastentry].type = type;
  GlobalTable[Glastentry].size = size;
  GlobalTable[Glastentry].function = function;
  GlobalTable[Glastentry].functionlabel = functionlabel;
  return Glastentry;
}

/* ----------------------------------------------------------------- */

int LocalInsert(char s[], int tok, int type, int size,int function,int functionlabel)

 /* returns position of entry for s */

{
  int len;
 
  len = strlen(s);     /* strlen computes length of s */
  if (Llastentry + 1 >= SYMMAX)
  {
    error("symbol table full");
    return (0);
  }
  if (Llastchar + len + 1 >= STRMAX)
  {
    error("lexemes array full");
    return (0);
  }
  Llastentry = Llastentry + 1;
  LocalTable[Llastentry].token = tok;
  LocalTable[Llastentry].lexptr = &Llexemes[Llastchar + 1];
  Llastchar = Llastchar + len + 1;
  (void) strcpy(LocalTable[Llastentry].lexptr, s);
  LocalTable[Llastentry].type = type;
  LocalTable[Llastentry].size = size;
  LocalTable[Llastentry].function = function;
  LocalTable[Llastentry].functionlabel = functionlabel;
  return Llastentry;
}

/* ------------------------------------------------------------------ */

void ParamInt(void)
/* This function changes all PARAM fields to INT in local table */

{
  int p;

  for (p = Llastentry; p > 0; p = p - 1)
    if (LocalTable[p].type == PARAM)
      LocalTable[p].type = INT;
} /* end ParamInt */


/* ------------------------------------------------------------------ */

void LocalReset(void) /* this function effectively erases the local symbol table */
{
  Llastchar = - 1;
  Llastentry = 0;
} /* end LocalReset */

/* ------------------------------------------------------------------ */

void AllBodsParsed(void)
{
  int p;
  int tmpflag = 0;

  for (p = Glastentry; p > 0; --p)
  {
    if (GlobalTable[p].function > 0)
    {
      emit(NOTDEC, p, 0.0);
      tmpflag = 1;
    } /* end if statement */
  }  /* end for loop */
  ErrorFlag = tmpflag;
}  /* end function */

/* ----------------------------------------------------------- */

void CheckMain(void)
{
  int temp;
  
  temp = GlobalLookup("main");

  if (GlobalTable[temp].function > 0)
  {
    error("Main never declared");
    GlobalTable[temp].function = -1;
  }
  ErrorFlag = 0; /* will get set again in parse's call to AllBodsParsed */
} /* end check main */
