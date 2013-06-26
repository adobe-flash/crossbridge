/**** lexer.c ***************************************************************/


# include "global.h"
extern int Number(int t);
extern int Indentifier(int t);
extern int Equal(int t);
extern int Nequal(int t);
extern void error(char *m);
extern int LocalLookup(char s[]);
extern int GlobalLookup(char s[]);

int lexan(void)
{
  int t;  /* temp to check next char in input */
  
  while (1)
  {
    t = getc(stdin);
    if (t == ' ' || t == '\t')
      ;  /* strip out white space */

    else if (t == '\n')
      ++lineno;

    else if (isdigit(t) || t == '.' )
      return (Number(t));

    else if ( isalpha(t) || t == '_')
      return(Indentifier(t));

    else if (t == '=')
      return (Equal(t) );

    else if (t == '!')
      return (Nequal(t) );

    else if (t == EOF)
      return(DONE);

    else
    {
      NextTokenval = NONE;
      return(t);
    } /* end else */
  }  /* end while (1) */
}  /* end lexan */
 
/* ======================================================================= */

int Indentifier(int t)
{
  int b = 0; /* used as a index to lexbuf */

  if (t == '_')
  {
    lexbuf[b] = t;
    ++b;
    t = getc(stdin);

    if (t == '_')
    {
      error("Indentifier cannot begin with a double underscore");
      return(lookahead);
    }  /* end if (t == '_') */
  }   /* end if (t == '_') */

  if (isalpha(t))
  {
    lexbuf[b] = t;
    ++b;
    t = getc(stdin);
  }  /* end if (isalpha(t)) */
  else
  {
    error("improperly formed indentifier");
    return(lookahead);
  }  /* end else */

  while (isalpha(t) || isdigit(t) || t == '_')
  {
    lexbuf[b] = t;
    ++b;
    t = getc(stdin);
  }  /* end while */

  (void) ungetc (t, stdin);  
  lexbuf[b] = EOS;

  LocalIndex = LocalLookup(lexbuf);
  GlobalIndex = GlobalLookup(lexbuf);

  if (LocalIndex)
    return(LocalTable[LocalIndex].token);
  else if (GlobalIndex)
    return (GlobalTable[GlobalIndex].token);
  else
    return(ID);
}

/* ======================================================================== */

int Equal(int t)
{
  t = getc(stdin);
  if (t == '=')
    return(EQUAL);
  else
  {
    (void) ungetc(t,stdin);
    NextTokenval = NONE;
    return('=');
  } /* end else */
}  /* end Equal function */

/* ======================================================================= */

int Nequal(int t)
{
  t = getc(stdin);
  if (t == '=')
    return(NEQUAL);
  else
  {
    (void) ungetc(t,stdin);
    NextTokenval = NONE;
    return('!');
  }  /* end else */
}  /* end function Nequal */

/* ====================================================================== */

int Number(int t)
{
  int DecimalAsInt; /* temp holder of the number */
  int Exponent;
  int NumOfZeros;

  NextFtokenval = 0.0; 
  NextTokenval = 0;

  /* get interger portion of float or the whole integer */
  if (isdigit (t))
  {
    (void) ungetc(t, stdin);
    scanf("%d", &NextTokenval);
    t = getc(stdin);
    if ((t != '.') && (t != 'e') && (t != 'E'))
    {
      (void) ungetc(t, stdin);
      return (NUM);
    }
  } /* end if t is digit */

  /* NextTokenval set at this point or NUM has been returned */
  if (t == '.')
  {
    t = getc(stdin);
    NumOfZeros = 0;

    /* 
       counts the number of zeros left of decimal and right of the
       first non-zero digit.
    */
    while (t == '0')
    {
      ++NumOfZeros;
      t = getc(stdin);
    } /* end while t == 0 */

    /* reads the number after any leading zeros */
    if (isdigit (t))
    {
      (void) ungetc(t, stdin);
      scanf("%d", &DecimalAsInt);
      NextFtokenval = (float) DecimalAsInt;

      /* converts the decimal characters into a decimal of the number */
      while (NextFtokenval >= 1.0)
      {
        NextFtokenval /= 10;
      } /* end NextFtokenval > 1 */
      if (NumOfZeros > 0)
      {
        while (NumOfZeros > 0)
        {
          NextFtokenval /= 10;
          --NumOfZeros;
        } /* end num of zeros > 0 */
      } /* end if num of zeros > 0 */
      t = getc(stdin); /* not needed here, but used later */
    } /* end if t is digit */
  } /* end if t == . */
      
  /* tokenval and ftokenval set if needed, otherwise still zero */
  NextFtokenval += NextTokenval;
  if ((t == 'e') || (t == 'E'))
  {
    t = getc(stdin);
    if ((t == '+') || (t == '-') || (isdigit (t)))
    {
      (void) ungetc(t, stdin);
      scanf("%d", &Exponent);
      t = getc(stdin); /* used for error checking, but also used later */
      if (t == '.')
      {
        error("Exponents must be integer values");
        return(lookahead);
      } /* end if exponent value is a float */
      if (Exponent > 0)
      {
        while (Exponent > 0)
        {
          NextFtokenval *= 10;
          --Exponent;
        } /* end while */
      } /* end if Exponent > 0 */
      else
      {
        while (Exponent < 0)
        {
          NextFtokenval /= 10;
          ++Exponent;
        } /* end while */
      } /* end else Exponent > 0 */
    } /* end if t is +, -, or a digit */
  } /* if t == e or t == E */
  (void) ungetc(t, stdin);
  return(RNUM);
} /* end isdigit or . */
 
