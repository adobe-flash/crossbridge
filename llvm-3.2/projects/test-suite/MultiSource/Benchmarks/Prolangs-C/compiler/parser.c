/****  parser.c  ***************************************************/

#include "global.h"

extern void emit(int t, int tval, float rval);
extern void error(char *m);
extern int lexan(void);
extern int GlobalInsert(char s[], int tok, int type, int size, int function,
                        int functionlabel);
extern void CheckMain(void);
extern void AllBodsParsed(void);
extern void LocalReset(void);
extern int LocalInsert(char s[], int tok, int type, int size,int function,
		       int functionlabel);
extern void ParamInt(void);
 
int term(void);
void match(int t);
int expr(void);
void PushArrayCellAddr(void);
void DeclOrF(void);
void GlobalScopeAndInsert(int type);
void GlobalVarList(int type);
void exprTest(void);
void exprAssg(void);
void parseAStmt(void);
void parse(void);
int factor(void);
void ExprList(void);
void FunctionList(void);
void FunctionDef(int type);
void MoreParams(void);
void ParamList(void);
void DecList(void);
void VarList(int type);
void FstmtList(void);

int term(void)
{
  int t; /* temporary holder of tokens */
  int leftExpr, rightExpr;

  leftExpr = factor();
  if (ErrorFlag)
    return -99;
  while(lookahead == '*' || lookahead == '/')
  {
      t = lookahead;
      match(lookahead); 
      if (ErrorFlag)
        return -99;
      rightExpr = factor(); 
      if (ErrorFlag)
        return -99;
      if (leftExpr == rightExpr)
      {
        if (leftExpr)
        {
          if (t == '*')
            emit(RMUL, 0, 0.0);
          else 
            emit(RDIV, 0, 0.0);
          leftExpr = 1;
        }
        else
        {
          if (t == '*')
            emit(MUL, 0, 0.0);
          else
            emit(DIV, 0, 0.0);         
          leftExpr = 0;
        }
      }
      else
      {
        if (leftExpr)
        {
          emit(FLOAT, 0, 0.0);
          if (t == '*')
            emit(RMUL, 0, 0.0);
          else
            emit(RDIV, 0, 0.0);
        }
        else
        {
          emit(NUM, 1, 0.0);
          emit(IST, 0, 0.0);
          emit(FLOAT, 0, 0.0);
          emit(NUM, 1, 0.0);
          emit(VAL, 0, 0.0);
          if (t == '*')
            emit(RMUL, 0, 0.0);
          else
            emit(RDIV, 0, 0.0);
        }
        leftExpr = 1;
      } /* end else leftExpr == rightExpr */
  }
  if (FloatFlag)
    return(1);
  else
    return (leftExpr);
}
/* ========================================================================= */

void match(int t) /* verify that t was the last token, then get next token */
             /* token to verify as last found */
{
  if (lookahead == t)
  {
    PreviousLookahead = lookahead;
    PreviousTokenval = tokenval;
    PreviousFtokenval = ftokenval;

    lookahead = NextLookahead;
    tokenval = NextTokenval;
    ftokenval = NextFtokenval;

    if (NextLookahead != DONE)
      NextLookahead = lexan();
  }
  else 
  {
    switch (lookahead)
    {
      case WHILE:
      case IF:
        error("Missing ; at end of previous statement");
        break;
    }
    if (ErrorFlag)
      return;
    switch (t)
    {
      case ')':
        error("Missing right parenthesis or illegal operator");
        break;
      default:
        printf("\n In default of match lookahead = %d",lookahead);
        error("syntax error");
        break;
    } /* end switch */
  }   /* else */  
}

/* ------------------------------------------------------------------- */

int expr(void)
{
  int t;  /* temporary holder of tokens */
  int leftExpr, rightExpr;

  leftExpr = term();
  if (ErrorFlag)
    return -99;
  while(lookahead == '+' || lookahead == '-')
  {
      t = lookahead;
      match(lookahead);
      if (ErrorFlag)
        return -99;
      rightExpr = term();
      if (ErrorFlag)
        return -99;
      if (leftExpr == rightExpr)
      {
        if (leftExpr)
        {
          if (t == '+')
            emit(RADD, 0, 0.0);
          else
            emit(RSUB, 0, 0.0);
          leftExpr = 1;
        }
        else
        {
          if (t == '+')
            emit(ADD, 0, 0.0);
          else
            emit(SUB, 0, 0.0);
          leftExpr = 0;
        }
      }
      else
      {
        if (leftExpr)
        {
          emit(FLOAT, 0, 0.0);
          if (t == '+')
            emit(RADD, 0, 0.0);
          else
            emit(RSUB, 0, 0.0);
        }
        else
        {
          emit(NUM, 1, 0.0);
          emit(IST, 0, 0.0);
          emit(FLOAT, 0, 0.0);
          emit(NUM, 1, 0.0);
          emit(VAL, 0, 0.0);
          if (t == '+')
            emit(RADD, 0, 0.0);
          else
            emit(RSUB, 0, 0.0);
        }
        leftExpr = 1;
      } /* end else leftExpr == rightExpr */
  }
  if (FloatFlag)
    return(1);
  else
    return(leftExpr);

}
/* ========================================================================= */


 
void PushArrayCellAddr(void) /* calculate address of array cell on stack */
{
  int tempTokenval;

  if ( !(ArrayParsed))
  {
    ArrayParsed = 1;
    tempTokenval = tokenval;
    if (LocalIndex)
      emit(PUSH_LOC_VAR_ADDR, LocalIndex, 0.0);
    else if (GlobalIndex)
      emit(PUSH_GLO_VAR_ADDR, GlobalIndex, 0.0);
    else
    {
      error("Compiler error, array lexeme not found in any table");
      return;
    }
    match(ARRAY);
    if (ErrorFlag)
      return;
    match('[');
    if (ErrorFlag)
      return;
    FloatFlag = 0;
    FloatFlag = expr();
    if (ErrorFlag)
      return;
    if (lookahead != ']')
    {
      error("Missing ']'");
      return;
    }
    if (FloatFlag)
    {
      error("Array index must be an integer value");
      return;
    }
    emit(ADD, 0, 0.0);
    lookahead = ARRAY;
    tokenval = tempTokenval;
  } /* end if !ArrayParsed */
  else
    ArrayParsed = 0;
} /* end PushArrayCellAddr */

/* ============================================================= */

void DeclOrF(void)
{
  int type;   /* stores type to call other functions with */
  int temp;  

  Scope = 0;  /* set scope to zero to mean global variables */
   
  while(1)
  {
    if (lookahead == ID)
    {
      emit(BEGIN, 0, 0.0);
      LabelCounter = 4; /* first 3 label used by emit BEGIN */
      /* insert an implicit call to main into the symbol tabel */
      temp = GlobalInsert ("main",ID,INT,0,1,1);
      if (ErrorFlag)
        return;
      if (strcmp("main", lexbuf) == 0)
         GlobalIndex = temp;
      FunctionDef (INT);
      if (ErrorFlag)
        return;
      return;
    }
    else if (lookahead == INT)
    {
      match (INT);
      if (ErrorFlag)
        return;
    }
    else
    {
      match (FLOAT);
      if (ErrorFlag)
        return;
    }
 
    if (NextLookahead == '(')
    {
      emit(BEGIN, 0, 0.0);
      LabelCounter = 4; /* first 3 labels used by emit BAGIN */
      /* insert an implicit call to main into the symbol tabel */
      temp = GlobalInsert ("main",ID,INT,0,1,1);
      if (ErrorFlag)
        return;
      if (strcmp("main", lexbuf) == 0)
        GlobalIndex = temp;
      FunctionDef(PreviousLookahead);
      if (ErrorFlag)
        return;
      return;
    }
    else
    {
      type = PreviousLookahead;
      GlobalScopeAndInsert (type);
      if (ErrorFlag)
        return;
      GlobalVarList (type);
      if (ErrorFlag)
        return;
      match(';');
      if (ErrorFlag)
        return;
    } /* end if != '(' */
  } /* end while(1) */
} /* end DeclOrF */

/* ============================================================= */

void GlobalScopeAndInsert(int type)
{
  int tempGlobalIndex;

  if (LocalIndex)
  {
    error("Compiler error! global lexeme found in local symbol table");
    return;
  } /* end LocalIndex */
  else if (GlobalIndex)
  {
    if (GlobalTable[GlobalIndex].function)
    {
      error("Attempting to define a function name as an identifier");
      return;
    } /* end if */

    else
    {
      error("Attempting to define a global variable twice");
      return;
    } /* end else */
  } /* end else ifGlobalIndex */
  else
  {
    if (NextLookahead == '[')
    {
      tempGlobalIndex = GlobalIndex = GlobalInsert(lexbuf, ARRAY, type,0,0,0);
      if (ErrorFlag)
        return;
      match(ID);
      if (ErrorFlag)
        return;
      match('[');
      if (ErrorFlag)
        return;
      if (lookahead == NUM)
      {
        GlobalTable[tempGlobalIndex].size = tokenval;
        match(NUM);
      }  /* end if (lookahead == num) */
      else
      {
        error("Improperly formed array definition");
        return;
      } /* end else */
      match(']');
      if (ErrorFlag)
        return;
      emit(GLO_DECL, tempGlobalIndex, 0.0);
      DecCount += GlobalTable[tempGlobalIndex].size;
    } /* end if next == '[' */
    else  /* regular ID */
    {
      GlobalIndex = GlobalInsert(lexbuf, ID, type, 1, 0, 0);
      if (ErrorFlag)
        return;
      emit(GLO_DECL, GlobalIndex, 0.0);
      ++DecCount;
      match(ID);
      if (ErrorFlag)
        return;
    } /* end else */
  } /* end else */
} /* end GlobalScopeAndInsert */

/* ============================================================= */

void GlobalVarList(int type)
{
  while (lookahead == ',')
  {
    match(',');
    if (ErrorFlag)
      return;
    if (lookahead == ID || lookahead == ARRAY)
    {
      GlobalScopeAndInsert (type);
      if (ErrorFlag)
        return;
    } /* end if ID or ARRAY */
    else
    {
      error("Expected an identifier or array");
      return;
    } /* end else */
  } /* end while loop */
} /* end GlobalVarList */

/* ============================================================= */

void exprTest(void)
{
  int tempTrue, tempDone;
  int tempOperation;
  int TempFloatFlag;

  FloatFlag = 0;
  FloatFlag = expr();
  if (ErrorFlag)
    return;

  while ((lookahead == EQUAL) || (lookahead == NEQUAL))
  {
    tempOperation = lookahead;
    if (lookahead == EQUAL)
      match(EQUAL);
    else
      match(NEQUAL);
    if (ErrorFlag)
      return;
    TempFloatFlag = FloatFlag;
    if (TempFloatFlag)
    {
      FloatFlag = 0;
      FloatFlag = expr();
      if (!(FloatFlag))
      { emit(FLOAT, 0, 0.0);
        FloatFlag = 1;
      }
    }
    else
    {
      FloatFlag = expr();
      if (FloatFlag)
      {
        emit(NUM, 1, 0.0);
        emit(IST, 0, 0.0);
        emit(FLOAT, 0, 0.0);
        emit(NUM, 1, 0.0);
        emit(VAL, 0, 0.0);
      }
    } 
    if (ErrorFlag)
      return;
    emit(COMP, 0, 0.0);
    tempTrue = LabelCounter;
    ++LabelCounter;
    tempDone = LabelCounter;
    ++LabelCounter;
    emit(PUSH_CODE_LABEL, tempTrue, 0.0);
    if (tempOperation == EQUAL)
      emit(BEQ, 0, 0.0);
    else
      emit(BNE, 0, 0.0);
    emit(NUM, 0, 0.0);
    emit(PUSH_CODE_LABEL, tempDone, 0.0);
    emit(B, 0, 0.0);
    emit(LABEL_CODE, tempTrue, 0.0);
    emit(NUM, 1, 0.0);
    emit(LABEL_CODE, tempDone, 0.0);
  } /* end if */
} /* end exprTest */

/* ========================================================================= */

void exprAssg(void)
{
  int  tempLocalIndex, tempGlobalIndex; /* temp indices during recursion */

  switch (lookahead)
  {
    case ARRAY:
      tempLocalIndex = LocalIndex;
      tempGlobalIndex = GlobalIndex;
      PushArrayCellAddr();
      if (ErrorFlag)
        return;
      if (NextLookahead == '=')
      {
        ArrayParsed = 0;
        match(ARRAY);
        if (ErrorFlag)
          return;
        match('=');
        if (ErrorFlag)
          return;
        FloatFlag = 0;
        exprAssg();
        if (ErrorFlag)
          return;
        if (tempLocalIndex)
        {
          if ((LocalTable[tempLocalIndex].type == INT) && (FloatFlag))
          {
            emit(INT, 0, 0.0);
            FloatFlag = 0;
          }
          else if ((LocalTable[tempLocalIndex].type == FLOAT) && ( !FloatFlag))
          {
            emit(FLOAT, 0, 0.0);
            FloatFlag = 1;
          }
          emit(STHB ,0 ,0.0);
        }
        else if (tempGlobalIndex)
        {
          if ((GlobalTable[tempGlobalIndex].type == INT) && (FloatFlag))
          {
            emit(INT, 0, 0.0);
            FloatFlag = 0;
          }
          else if((GlobalTable[tempGlobalIndex].type == FLOAT) && (!FloatFlag))
          {
            emit(FLOAT, 0, 0.0);
            FloatFlag = 1;
          }
          emit(STH, 0, 0.0);
        }
        emit(SWAP, 0, 0.0);
        emit(POP, 0, 0.0);
      } /* end if NextLookahead == '=' */
      else
      {
        exprTest();
        if (ErrorFlag)
          return;
      } /* end else of if lookahead == '=' */
      break;
    case ID:
      tempLocalIndex = LocalIndex;
      tempGlobalIndex = GlobalIndex;

      if (NextLookahead == '=')
      {
        if (LocalIndex)
          emit(PUSH_LOC_VAR_ADDR, LocalIndex, 0.0);
        else if (GlobalIndex)
          emit(PUSH_GLO_VAR_ADDR, GlobalIndex, 0.0);
        else
        {
          error("Compiler error, variable lexeme not found in any table");
          return;
        }
        match(ID);
        if (ErrorFlag)
          return;
        match('=');
        if (ErrorFlag)
          return;
        FloatFlag = 0;
        exprAssg();
        if (ErrorFlag) 
          return;
        if (tempLocalIndex)
        {
          if ((LocalTable[tempLocalIndex].type == INT) && (FloatFlag))
          {
            emit(INT, 0, 0.0);
            FloatFlag = 0;
          }
          else if ((LocalTable[tempLocalIndex].type == FLOAT) && ( !FloatFlag))
          {
            emit(FLOAT, 0, 0.0);
            FloatFlag = 1;
          }
          emit(STHB, 0, 0.0);
        }
        else if (tempGlobalIndex)
        {
          if ((GlobalTable[tempGlobalIndex].type == INT) && (FloatFlag))
          {
            emit(INT, 0, 0.0);
            FloatFlag = 0;
          }
          else if((GlobalTable[tempGlobalIndex].type == FLOAT) && (!FloatFlag))
          {
            emit(FLOAT, 0, 0.0);
            FloatFlag = 1;
          }
          emit(STH, 0, 0.0);
        }
        emit(SWAP, 0, 0.0);
        emit(POP, 0, 0.0);
      }
      else
      {
        exprTest();
        if (ErrorFlag)
          return;
      }
      break;
    default:
      exprTest();
      if (ErrorFlag)
        return;
  } /* end switch */
  return;
} /* end exprAssg */

/* ========================================================================= */

void parseAStmt(void) /* parse a single statement */
{
  int tempLabel1, tempLabel2;

  switch (lookahead)
  {
    case WHILE:
      match(WHILE);
      if (ErrorFlag)
        return;
      tempLabel1 = LabelCounter;
      ++LabelCounter;
      tempLabel2 = LabelCounter;
      ++LabelCounter;
      emit(LABEL_CODE, tempLabel1, 0.0);
      emit(PUSH_CODE_LABEL, tempLabel2, 0.0);
      match('(');
      if (ErrorFlag)
        return;
      FloatFlag = 0;
      exprAssg();
      if (ErrorFlag)
        return;
      match(')');
      if (ErrorFlag)
        return;
      if (FloatFlag)
        emit(RNUM, 0, 0.0); /* push 0.0  */
      else
        emit(NUM, 0 ,0.0); /* push 0 */
      emit(COMP, 0, 0.0);
      emit(BEQ, 0, 0.0);
      parseAStmt();
      if (ErrorFlag)
        return;
      emit(PUSH_CODE_LABEL, tempLabel1, 0.0);
      emit(B, 0, 0.0);
      emit(LABEL_CODE, tempLabel2, 0.0);
      break;
    case IF:
      match(IF);
      if (ErrorFlag)
        return;
      tempLabel1 = LabelCounter;
      ++LabelCounter;
      emit(PUSH_CODE_LABEL, tempLabel1, 0.0);
      match('(');
      if (ErrorFlag)
        return;
      FloatFlag = 0;
      exprAssg();
      if (ErrorFlag)
        return;
      match(')');
      if (ErrorFlag)
        return;
      if (FloatFlag)
        emit(RNUM, 0, 0.0);  /* push 0.0 */
      else
        emit(NUM, 0, 0.0);  /* push 0 */
      emit(COMP, 0, 0.0);
      emit(BEQ, 0, 0.0);
      parseAStmt();
      if (ErrorFlag)
        return;
      if (lookahead == ELSE)
      {
        match (ELSE);
        if (ErrorFlag)
          return;
        tempLabel2 = LabelCounter;
	++LabelCounter;
        emit(PUSH_CODE_LABEL, tempLabel2, 0.0);
        emit(B, 0, 0.0);
        emit(LABEL_CODE, tempLabel1, 0.0);
        parseAStmt();
        if (ErrorFlag)
          return;
        emit(LABEL_CODE, tempLabel2, 0.0);
      } /* end if lookahead == ELSE */
      else
      {
        emit(LABEL_CODE, tempLabel1, 0.0);
      } /* end else lookahead == ELSE */
      break;
    case '{':
      match('{');
      do
      {
        parseAStmt();
        if (ErrorFlag)
          ErrorFlag = 0;
      } while ((lookahead != '}') && (lookahead != DONE));
      match('}');
      if (ErrorFlag)
        return;
      break;
    case RETURN:
      if (NextLookahead == ';')
      {
        match(RETURN);
        if (ErrorFlag)
          return;
        match(';');
        if (ErrorFlag)
          return;
        if (GlobalTable[FuncNameIndex].type == INT)
          emit(NUM, 1, 0.0);
	else
          emit(RNUM, 0, 1.0);
      } /* end if */
      else
      {
        match(RETURN);
        if (ErrorFlag)
          return;
        exprAssg();
        if (ErrorFlag)
          return;
        match(';');
        if (ErrorFlag)
          return;
      } /* end else */
      emit(PUSH_CODE_LABEL, ReturnLabel, 0.0);
      emit(B, 0, 0.0);
      break;
    default:  /* assignment statement */
      exprAssg();
      if (ErrorFlag)
        return;
      emit(POP, 0, 0.0);
      match(';');
      return;
  } /* end switch */
  return;
} /* end parseAStmt */

/* ========================================================================= */

void parse(void) /* parse a list of functions */
{
  /* prime NextLookahead */
  lookahead = 1;
  match (lookahead);
  if (ErrorFlag)
    ErrorFlag = 0;

  /* prime lookahead */
  lookahead = 1;
  match (lookahead);
  if (ErrorFlag)
    ErrorFlag = 0;

  DeclOrF(); /* parses the global var. declarations and the first function */
  if (ErrorFlag)
    return;
  FunctionList();
  if (ErrorFlag)
    return;
  CheckMain();
  AllBodsParsed();
  if (ErrorFlag)
    return;
  emit(END, 0, 0.0);
  return;
} /* end parse */
/* ========================================================================= */


int factor(void)
{
  int temp;
  int tempLocalIndex, tempGlobalIndex;
  char templexbuf[30];
  
  switch(lookahead) {
  case '(':
    match('('); 
    if (ErrorFlag)
      return -99;
    switch (lookahead)
    {
      case INT:
        match (INT);
        if (ErrorFlag)
          return -99;
        match (')');
        if (ErrorFlag)
          return -99;

        temp = factor();
        if (ErrorFlag)
          return -99;
        if (temp)
          emit(INT, 0, 0.0);
        return(0);
        break;

      case FLOAT:
        match (FLOAT);
        if (ErrorFlag)
          return -99;
        match (')');
        if (ErrorFlag)
          return -99;

        temp = factor();
        if (ErrorFlag)
          return -99;
        if ( !temp)
          emit(FLOAT, 0, 0.0);
        return(1); 
        break;

      default:
        FloatFlag = 0;
        exprAssg();
        if (ErrorFlag)
          return -99;
        match(')');
        if (ErrorFlag)
          return -99;
        if (FloatFlag)
          return(1);
        else
          return(0);
        break;
    } /* end switch */
    break;

  case NUM:
    emit(NUM, tokenval, 0.0);
    match(NUM);
    if (ErrorFlag)
      return -99;
    return(0); /* meaning false float flag */
    break;

  case RNUM:
    emit(RNUM, tokenval, ftokenval);
    match(RNUM);
    if (ErrorFlag)
      return -99;
    return(1); /* meaning true float flag */
    break;

  case ID:
    if (NextLookahead == '(')
    {
      /* function call */
      tempLocalIndex = LocalIndex;
      tempGlobalIndex = GlobalIndex;
      strcpy (templexbuf, lexbuf);
      match(ID);
      if (ErrorFlag)
        return -99;
      match('(');
      if (ErrorFlag)
        return -99;
      ExprList();
      if (ErrorFlag)
        return -99;
      match(')');
      if (ErrorFlag)
        return -99;

      /* prepare to branch to function */
      CallReturnAddr = LabelCounter;
      ++LabelCounter;
      emit(PUSH_CODE_LABEL, CallReturnAddr, 0.0);
      emit(PUSH_FRAMESIZE, FuncNameIndex, 0.0);
      emit(IB, 0, 0.0);

      /* branch to function */
      if (tempLocalIndex)
      {
        error("Function called is to a local variable");
        return -99;
      } /* end LocalIndex */
      else if (tempGlobalIndex)
      {
        if (!(GlobalTable[tempGlobalIndex].function))
        {
          error("Function called is to a global variable");
          return -99;
        } /* end if !function */
        else
        {
          emit(PUSH_CODE_LABEL,GlobalTable[tempGlobalIndex].functionlabel,0.0);
        } /* end else !function */
      } /* end GlobalIndex */
      else /* lexeme not in any symbol table */
      {
        tempGlobalIndex = GlobalInsert (templexbuf,ID,INT,0,1,LabelCounter);
        if (ErrorFlag)
          ErrorFlag = 0;
        ++LabelCounter;
        emit(PUSH_CODE_LABEL, GlobalTable[tempGlobalIndex].functionlabel, 0.0);
      } /* end insert function name */
      emit(B, 0, 0.0);

      /* return sequence */
      emit(LABEL_CODE, CallReturnAddr, 0.0);
      emit(PUSH_FRAMESIZE, FuncNameIndex, 0.0);
      emit(DB, 0, 0.0);
      if (GlobalTable[tempGlobalIndex].function < 0) /* function defined */
      {
        if (GlobalTable[tempGlobalIndex].type == INT)
          return(0);
        else
          return(1);
      }
      else /* default return type of undefined function is INT */
        return(0);
    } /* end if NextLookahead == ')' */
    else if ( !(LocalIndex || GlobalIndex))
    {
      error("attempting to use an undefined variable");
      return -99;
    }

    if (LocalIndex)
      emit(PUSH_LOC_VAR_VALUE, LocalIndex, 0.0);
    else if (GlobalIndex)
      emit(PUSH_GLO_VAR_VALUE, GlobalIndex, 0.0);
    else
    {
      error("Compiler error, lexeme was not found in any table");
      break;
    }
    tempLocalIndex = LocalIndex;
    tempGlobalIndex = GlobalIndex;
    match(ID);
    if (ErrorFlag)
      return -99;
    if (tempLocalIndex)
    {
      if (LocalTable[tempLocalIndex].type == FLOAT)
        return(1);
      else
        return(0);
    }
    else
    {
      if (GlobalTable[tempGlobalIndex].type == FLOAT)
        return(1);
      else
        return(0);
    }
    break;
  case ARRAY:
    tempLocalIndex = LocalIndex;
    tempGlobalIndex = GlobalIndex;
    PushArrayCellAddr();
    if (ErrorFlag)
      return -99;
    ArrayParsed = 0;
    match(ARRAY);
    if (ErrorFlag)
      return -99;
    if (tempLocalIndex)
      emit(VALB, 0, 0.0);
    else if (tempGlobalIndex)
      emit(VAL, 0, 0.0);
    else
    {
      error("using an undefined array referance");
      return -99;
    }

    if (tempLocalIndex)
    {
      if (LocalTable[tempLocalIndex].type == FLOAT)
        return(1);
      else
        return(0);
    }
    else
    {
      if (GlobalTable[tempGlobalIndex].type == FLOAT)
        return(1);
      else
        return(0);
    }
    break;
  case '-':
    match('-'); 
    if (ErrorFlag)
      return -99;
    temp=factor(); 
    if (ErrorFlag)
      return -99;
    if (temp)
      emit(RNEG, 0, 0.0);
    else
      emit(NEG, 0, 0.0);
    return(temp);
    break;
  case READF:
    match(READF);
    if (ErrorFlag)
      return -99;
    match ('(');
    if (ErrorFlag)
      return -99;
    match (')');
    if (ErrorFlag)
      return -99;
    emit(READ, 0, 0.0);
    emit(FLOAT, 0, 0.0);
    return(1);
    break;
  case READI:
    match(READI);
    if (ErrorFlag)
      return -99;
    match('(');
    if (ErrorFlag)
      return -99;
    match(')');
    if (ErrorFlag)
      return -99;
    emit(READ, 0, 0.0);
    emit(INT, 0, 0.0);
    return(0);
    break;
  case WRITEF:
    match (WRITEF);
    if (ErrorFlag)
      return -99;
    match('(');
    if (ErrorFlag)
      return -99;
    FloatFlag = 0;
    exprAssg();
    if (ErrorFlag)
      return -99;
    if ( !(FloatFlag))
    {
      emit(FLOAT, 0, 0.0);
      FloatFlag = 1;
    }
    match(')');
    if (ErrorFlag)
      return -99;
    emit(WRITEF, 0, 0.0);
    emit(RNUM, 1, 1.0);
    return(1);
    break;
  case WRITEI:
    match(WRITEI);
    if (ErrorFlag)
      return -99;
    match('(');
    if (ErrorFlag)
      return -99;
    FloatFlag = 0;
    exprAssg();
    if (ErrorFlag)
      return -99;
    if (FloatFlag)
    {
      emit(INT, 0, 0.0);
      FloatFlag = 0;
    }
    match(')');
    if (ErrorFlag)
      return -99;
    emit(WRITEI, 0, 0.0);
    emit(NUM, 1, 0.0);
    return(0);
    break;
  case WRITELN:
    match(WRITELN);
    if (ErrorFlag)
      return -99;
    match('(');
    if (ErrorFlag)
      return -99;
    match(')');
    if (ErrorFlag)
      return -99;
    emit(WRITELN, 0, 0.0);
    emit(NUM, 1, 0.0);
    return(0);
    break;
  default:
    /* error used to be called from here */
    error("Illegal operator");
    return(0);
  }
  return -99;
}

/* ------------------------------------------------------------------ */

void ExprList(void)
{
  if (lookahead != ')') 
  {
    exprAssg();
    if (ErrorFlag)
      return;
  
    while (lookahead == ',')
    {
      match(',');
      if (ErrorFlag)
        return;
      exprAssg();
      if (ErrorFlag)
        return;
    } /* end while */
  } /* end if ) */
} /* end ExprList */

/* ========================================================================= */


void FunctionList(void)
{
  do
  {
    ErrorFlag = 0;
    FloatFlag = 0;
    LocalReset();
    switch(lookahead)
    {
      case ID:
       FunctionDef(INT);
       if (ErrorFlag)
         return;
       break;
      case INT:
       match(INT);
       if (ErrorFlag)
         return;
       FunctionDef(INT);
       if (ErrorFlag)
         return;
       break;
      case FLOAT:
       match(FLOAT);
       if (ErrorFlag)
         return;
       FunctionDef(FLOAT);
       if (ErrorFlag)
         return;
      case DONE:
       break;
      default:
       error("Unexpected token found");
       return;
       break;
    }  /*  end for switch statement */
  } 
  while (lookahead != DONE);
}  /* end FunctionList */

/* ----------------------------------------------------------------------- */

void FunctionDef(int type)
{
  offset = 1;
  if (lookahead != ID)
  {
    error("Invalid function defintion structure. Expected ID");
    return;
  }  /*  end if lookahead  */
  
  if (LocalIndex)
  {
    error("Compiler error, function name in local symbol table");
    return;
  }  /* end if LocalIndex */
  else if (GlobalIndex)
  {
    if (!(GlobalTable[GlobalIndex].function))
    {
      error("Attempting to redefine a global variable as a function name");
      return;
    }   /* end if GlobalTable */
    else if (GlobalTable[GlobalIndex].function < 0)
    {
      error("Redefining a function.");
      return;
    }    /*  end else if GlobalTable */
    else if (GlobalTable[GlobalIndex].function > 0)
    /* function is a postive value, already called not defined */
    {
      GlobalTable[GlobalIndex].type = type;
      FuncNameIndex = GlobalIndex;
      GlobalTable[GlobalIndex].function = -1;
    }   /* end else if function > 0 */
    else
    {
      error("Compiler error, functiondef is confused");
      return;
    }
  } /* end else if (GlobalIndex) */
  else /* ID not in any tabel */
  {
    GlobalIndex = GlobalInsert (lexbuf,ID,type,0,-1,LabelCounter);
    if (ErrorFlag)
      ErrorFlag = 0;
    FuncNameIndex = GlobalIndex;
    ++LabelCounter;
  }  /* end else */

  Scope = GlobalTable[GlobalIndex].functionlabel;

  /* label function */
  emit(LABEL_FUNC, GlobalIndex, 0.0);
  emit(LABEL_CODE, Scope, 0.0);

  /* store return address */
  emit(STORE_RA, FuncNameIndex, 0.0);

  ReturnLabel = LabelCounter;
  ++LabelCounter;

  match(ID);
  if (ErrorFlag)
    ErrorFlag = 0;
  match('(');
  if (ErrorFlag)
    ErrorFlag = 0;
  if ( strcmp(GlobalTable[FuncNameIndex].lexptr, "main") == 0)
  {
    if (lookahead == ')')
    {
      match(')');
      if (ErrorFlag)
        return;
    } /* end if lookahead == ')' */
    else
    {
      error("Function main cannot have parameters.");
      return;
    } /* end else */
  } /* end if function is main */
  else
  {
    ParamList();
    if (ErrorFlag)
      ErrorFlag = 0;
    match(')');
    if (ErrorFlag)
      ErrorFlag = 0;
  } /* end else (function is something other than main) */
  DecList();
  if (ErrorFlag)
    ErrorFlag = 0;
  emit(SAVE_FRAMESIZE, FuncNameIndex, 0.0);
  FstmtList();
  if (ErrorFlag)
    return;
  if (GlobalTable[FuncNameIndex].type == INT)
    emit(NUM, 1, 0.0);
  else
    emit(RNUM, 0, 1.0);
  emit(LABEL_CODE, ReturnLabel, 0.0);
  emit(PUSH_RA, FuncNameIndex, 0.0);
  emit(B, 0, 0.0);
} /* end function Def */

/* ---------------------------------------------------------------------- */

void MoreParams(void)
{
  int tempLocalIndex;

  if (lookahead == ',')
  {
    match(',');
    if (ErrorFlag)
      return;
    if (lookahead != ID)
    {
      error("Improperly formed parameter list");
      return;
    }
    if (GlobalIndex)
    {

      if (GlobalTable[GlobalIndex].function)
      {
        error("Cannot use a function name as parameter");
        return;
      }  /* end if GlobalTable */
    }   /* GlobalIndex */

    if (LocalIndex)
    {
      error("Trying to use a prameter twice");
      return;
    }  /* end else if (LocalIndex) */

    else
    {
      tempLocalIndex = LocalIndex = LocalInsert (lexbuf,ID,PARAM,1,0,0);
      if (ErrorFlag)
        return;
    }  /* end else */
    match (ID);
    if (ErrorFlag)
      return;
    MoreParams();
    if (ErrorFlag)
      return;
    emit(LOC_DECL, tempLocalIndex, 0.0);
    emit(PUSH_LOC_VAR_ADDR, tempLocalIndex, 0.0);
    emit(ISTB, 0, 0.0);
    ++offset;
  } /* end if lookahead == ',' */
} /* end MoreParams() */


/* ---------------------------------------------------------------------- */

void ParamList(void)
{
  int tempLocalIndex;

  if (lookahead == ID)
  {
    if (GlobalIndex)
    {

      if (GlobalTable[GlobalIndex].function)
      {
        error("Cannot use a function name as parameter");
        return;
      }  /* end if GlobalTable */
    }   /* GlobalIndex */

    if (LocalIndex)
    {
      error("Trying to use a parameter twice");
      return;
    }  /* end else if (LocalIndex) */

    else
    {
      tempLocalIndex = LocalIndex = LocalInsert (lexbuf,ID,PARAM,1,0,0);
      if (ErrorFlag)
        return;
    }  /* end else */
    match (ID);
    if (ErrorFlag)
      return;
    MoreParams();
    if (ErrorFlag)
      return;
    emit(LOC_DECL, tempLocalIndex, 0.0);
    emit(PUSH_LOC_VAR_ADDR, tempLocalIndex, 0.0);
    emit(ISTB, 0, 0.0);
    ++offset;
  } /* end if lookahead == ID */
} /* end paramlist() */

/* ------------------------------------------------------------------ */

void DecList(void)
{
  while ((lookahead == INT) || (lookahead == FLOAT))
  {
    match (lookahead); /* obviously must be INT or FLOAT */
    if (ErrorFlag)
      return;
    VarList (PreviousLookahead);
    if (ErrorFlag)
      return;
  }  /* end while ((lookahead == INT) || (loookahead == FLOAT)) */
  ParamInt();
}  /* end function DecList */

/* --------------------------------------------------------------------- */

void VarList(int type)
{
  int tempLocalIndex;

  do
  {
    if (GlobalIndex)
    {
  
      if (GlobalTable[GlobalIndex].function)
      {
        error ("Cannot use a function name as a parameter");
        return;
      }  /* end if GlogalTable */
    }  /* end if (GlobalIndex) */

    if (LocalIndex)
    {

      if (LocalTable[LocalIndex].type == PARAM)
      {
        if (NextLookahead == '[')
          error ("Cannot pass an array as a parameter");
        else
        {
          LocalTable[LocalIndex].type = type;
          match(ID);
          if (ErrorFlag)
            return;
        } /* end else */
      } /* end if LocalTable[LocalIndex] */
     
      else
      {
        error ("Trying to redefine a variable");
        return;
      }  /* end else */
    }  /* end if LocalIndex */
   
    else  /* variable is not entered as a local so insert */
    {
      if (NextLookahead == '[')
      {
        tempLocalIndex = LocalIndex = LocalInsert (lexbuf,ARRAY,type,0,0,0);
        if (ErrorFlag)
          return;
        lookahead = ARRAY;
        match (ARRAY);
        if (ErrorFlag)
          return;
        match ('[');
        if (ErrorFlag)
          return;
        if (lookahead == NUM)
        {
          LocalTable[tempLocalIndex].size = tokenval;
          if (ErrorFlag)
            return;
        }
        else
        {
          error("Array declarations requires an integer within []");
          return;
        }
        match (NUM);
        if (ErrorFlag)
          return;
        match (']');
        if (ErrorFlag)
   	  return;
        emit(LOC_DECL, tempLocalIndex, 0.0);
        offset += LocalTable[tempLocalIndex].size;
      } /* end if nextlook */
     
      else
      {
        LocalIndex = LocalInsert(lexbuf,ID,type,1,0,0);
        if (ErrorFlag)
          return;
        emit(LOC_DECL, LocalIndex, 0.0);
        ++offset;
        match (ID);
        if (ErrorFlag)
          return;
      } /* end else */
    } /* end else */

   switch (lookahead)
   {
     case ',':
       match (',');
       if (ErrorFlag)
 	 return;
       break;
     case ';':
       match (';');
       if (ErrorFlag)
	 return;
       return;
       break;
     default:
       error ("badly formed declarations");
       return;
   } /* end switch (lookahead) */
 } while ((lookahead == ID) || (lookahead == ARRAY));
 error("can only use ID  or ARRAAY");
 return;
} /* end function VarList */

/* ------------------------------------------------------------------- */

void FstmtList(void)
{
  match ('{');
  if (ErrorFlag)
    ErrorFlag = 0;
  
  do
  {
    parseAStmt();
    if (ErrorFlag)
      ErrorFlag = 0;
  }
  while ((lookahead != '}') && (lookahead != DONE));
  
  if (lookahead == DONE)
  {
     error("Unexpected end of file");
     return;
  } /* end if */
  else
  {
    match ('}');
    if (ErrorFlag)
      return;
  } /* end else */
} /* end function FstmtList */ 	
