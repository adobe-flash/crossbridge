/****  emitter.c  *******************************************************/

#include "global.h"

void emit(int t, int tval, float rval)   /*  generates output */
{
  if (ErrorFlag)
    return;

  ++NumberC;
  if (NumberC >= 8)
   {
     printf("\n");
     NumberC = 0;
   }          /* end if NumberC */

  switch(t) {
  case RADD: 
    printf("RADD, ");
    break;
  case ADD:
    printf("ADD, ");
    break;
  case RSUB: 
    printf("RSUB, ");
    break;
  case SUB:
    printf("SUB, ");
    break;
  case RMUL: 
    printf("RMUL, ");
    break;
  case MUL:
    printf("MUL, ");
    break;
  case RDIV:
    printf("RDIV, ");
    break;
  case DIV:
    printf("DIV, ");
    break;
  case PUSH_LOC_VAR_ADDR:
    printf("'%s#%d, ", LocalTable[tval].lexptr, Scope);
    break;
  case PUSH_GLO_VAR_ADDR:
    printf("'%s#0, ", GlobalTable[tval].lexptr);
    break;
  case WRITEI:
    printf("WRITEI, ");
    break;
  case WRITEF:
    printf("WRITER, ");
    break;
  case WRITELN:
    printf("NEWLINE, ");
    break;
  case STORE:
    printf("ST, ");
    break;    
  case IST:
    printf("IST, ");
    break;
  case STH:
    printf("STH, ");
    break;
  case VAL:
    printf("VAL, ");
    break;
  case VALB:
    printf("VALB, ");
    break;
  case PUSH_GLO_VAR_VALUE:
    printf("@%s#0, ",GlobalTable[tval].lexptr);
    break;
  case PUSH_LOC_VAR_VALUE:
    printf(">%s#%d, ", LocalTable[tval].lexptr, Scope);
    break;
  case READ:
    printf("READ, ");
    break;
  case NUM:
    printf("%d, ", tval);
  break;
  case RNUM:
    printf("%16.4e, ", rval); 
  break;
  case GLO_DECL:  /* here tval is a subscript to the symbol table */
    printf("%s#0 = %d, ", GlobalTable[tval].lexptr, DecCount);
    break;
  case LOC_DECL:
    printf("%s#%d = %d, ", LocalTable[tval].lexptr, Scope, offset);
    break;
  case NEG:
    printf("NEG, ");
    break;
  case RNEG:
    printf("RNEG, ");
    break;
  case INT:
    printf("FIX, "); 
    break;
  case FLOAT:
    printf("FLOAT, "); 
    break;
  case POP:
    printf("POP, "); 
    break;
  case LABEL_CODE:
    printf("$%d: ", tval);
    break;
  case PUSH_CODE_LABEL:
    printf("$%d, ", tval);
    break;
  case COMP:
    printf("COMP, ");
    break;
  case SWAP:
    printf("SWAP, ");
    break;
  case BEQ:
    printf("BEQ, ");
    break;
  case BNE:
    printf("BNE, ");
    break; 
  case B:
    printf("B, ");
    break;
  case STHB:
    printf("STHB, ");
    break;
  case LABEL_FUNC:
    printf("%s: ", GlobalTable[tval].lexptr);
    break;
  case ISTB:
    printf("ISTB, ");
    break;
  case STORE_RA:
    printf("RA%s = 0, ", GlobalTable[tval].lexptr);
    printf("'RA%s, ", GlobalTable[tval].lexptr);
    printf("istb, ");
    break;
  case SAVE_FRAMESIZE:
    printf("frame%s = %d, ", GlobalTable[tval].lexptr, offset);
    break;
  case PUSH_FRAMESIZE:
    printf("'frame%s, ", GlobalTable[tval].lexptr);
    break;
  case IB:
    printf("IB, ");
    break;
  case DB:
    printf("DB, ");
    break;
  case PUSH_RA:
    printf(">RA%s, ", GlobalTable[tval].lexptr);
    break;
  case BEGIN:
    printf("\n%%%d, START: $3, sb, $2, $1, b,\n", DecCount);
    NumberC = 0;
    break;
  case END:
    printf("\n$2: STOP, $3: ->START.\n");
    break;
  case NOTDEC:
    printf("\n Function %s called but not parsed.\n",GlobalTable[tval].lexptr);
    break;
  default:
    printf("token %d, tval %d, rval %f\n", t, tval, rval);
  }
}
