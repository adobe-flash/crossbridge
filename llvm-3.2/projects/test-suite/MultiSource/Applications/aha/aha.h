// Copyright (C) 2002 by Henry S. Warren, Jr.
const int debug = 0;            // 0 or 1; debugging printouts if 1.
const int counters = 1;         // 0 or 1; count number of evaluations.
#define NARGS 1                 // Number of args in userfun, 1 or 2.

/* A note about the registers:

They are divided into four groups.  The first group, starting with
register 0, holds ordinary immediate values.  The second group, starting
with register NIM, holds the shift immediate values.  The next 1 or 2
regs are the arguments to the user-defined function.  The last group
holds the results of computations done by the trial programs.

0        Start of ordinary immediate values (those given by IMMEDS)
NIM      Start of shift immediate values    (those given by SHIMMEDS)
RX       First (or only) user function argument
RY       Second user function argument
RI0      Result of instruction 0 goes here
RI0 + i  Result of instruction i goes here
where:
NIM   = number of ordinary immediate values
NSHIM = number of shift immediate values
*/

#define MAXNEG 0x80000000
#define MAXPOS 0x7FFFFFFF
#define NBSM 63                 // Shift mask.  Use 63 for mod 64
                                // shifts, or 31 for mod 32.

int trialx[] = {1, 0, -1, MAXNEG, MAXPOS, \
   MAXNEG + 1, MAXPOS - 1, 0x01234567, 0x89ABCDEF, -2, 2, -3, 3, \
   -64, 64, -5, -31415};
#if NARGS == 2
   int trialy[] = {0};
#endif
// First three values of IMMEDS must be 0, -1, and 1.
#define IMMEDS 0, -1, 1, MAXNEG, -2, 2, 3
#define SHIMMEDS 1, 2, 30, 31

int dummy1[] = {IMMEDS};        // These get optimized out of existence.
int dummy2[] = {SHIMMEDS};

#define NIM (int)(sizeof(dummy1)/sizeof(dummy1[0]))
#define NSHIM (int)(sizeof(dummy2)/sizeof(dummy2[0]))
#define RX (NIM + NSHIM)        // First (or only) user function argument
#define RY (RX + 1)             // Second user function argument
#define RI0 (RX + NARGS)        // Result of instruction 0 goes here

int unacceptable;               // Code below sets this to 1 for an
                                // unacceptable operation, such as
                                // divide by 0.  It is initially 0.

/* Collection of simulator routines for the instructions in the isa. */
int neg(int x, int y, int z) {return -x;}
int Not(int x, int y, int z) {return ~x;}
int pop(int xx, int y, int z) {
   unsigned x = xx;
   x = x - ((x >> 1) & 0x55555555);
   x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
   x = (x + (x >> 4)) & 0x0F0F0F0F;
   x = x + (x << 8);
   x = x + (x << 16);
   return x >> 24;
}

int nlz(int xx, int y, int z) {
   unsigned x = xx;
   int n;

   if (x == 0) return(32);
   n = 0;
   if (x <= 0x0000FFFF) {n = n +16; x = x <<16;}
   if (x <= 0x00FFFFFF) {n = n + 8; x = x << 8;}
   if (x <= 0x0FFFFFFF) {n = n + 4; x = x << 4;}
   if (x <= 0x3FFFFFFF) {n = n + 2; x = x << 2;}
   if (x <= 0x7FFFFFFF) {n = n + 1;}
   return n;
}

int rev(int xi, int y, int z) {
   unsigned x = xi;
   x = (x & 0x55555555) <<  1 | ((x >>  1) & 0x55555555);
   x = (x & 0x33333333) <<  2 | ((x >>  2) & 0x33333333);
   x = (x & 0x0F0F0F0F) <<  4 | ((x >>  4) & 0x0F0F0F0F);
   x = (x << 24) | ((x & 0xFF00) << 8) |
       ((x >> 8) & 0xFF00) | (x >> 24);
   return x;
}

int add (int x, int y, int z) {return x + y;}
int sub (int x, int y, int z) {return x - y;}
int mul (int x, int y, int z) {return x * y;}
/* For division overflow we return arbitrary values, hoping they fail
to be part of a solution.  (User must check solutions, in general.) */
int divide (int x, int y, int z) {
   if (y == 0 || (y == -1 && x == (int)0x80000000))
      {unacceptable = 1; return 0;}
   else return x/y;}
int divu(int x, int y, int z) {
   if (y == 0) {unacceptable = 1; return 0;}
   else return (unsigned)x/(unsigned)y;}
int And(int x, int y, int z) {return x & y;}
int Or (int x, int y, int z) {return x | y;}
int Xor(int x, int y, int z) {return x ^ y;}
int rotl(int x, int y, int z) {int s = y & NBSM;
   return x << s | (unsigned)x >> (32 - s);}
int shl (int x, int y, int z) {int s = y & NBSM;
   if (s >= 32) return 0; else return x << s;}
int shr(int x, int y, int z) {int s = y & NBSM;
   if (s >= 32) return 0; else return (unsigned)x >> s;}
int shrs(int x, int y, int z) {int s = y & NBSM;
   if (s >= 32) return x >> 31; else return x >> s;}
int cmpeq(int x, int y, int z) {return x == y;}
int cmplt(int x, int y, int z) {return x < y;}
int cmpltu(int x, int y, int z) {return (unsigned)(x) < (unsigned)(y);}
int seleq(int x, int y, int z) {return x == 0 ? y : z;}
int sellt(int x, int y, int z) {return x < 0 ? y : z;}
int selle(int x, int y, int z) {return x <= 0 ? y : z;}

                                // The machine's instruction set:
// Note: Commutative ops are commutative in operands 0 and 1.
struct {
   int  (*proc)(int, int, int); // Procedure for simulating the op.
   int  numopnds;               // Number of operands, 1 to 3.
   int  commutative;            // 1 if opnds 0 and 1 commutative.
   int  opndstart[3];           // Starting reg no. for each operand.
   char *mnemonic;              // Name of op, for printing.
   char *fun_name;              // Function name, for printing.
   char *op_name;               // Operator name, for printing.
} isa[] = {
   {neg,    1, 0, {RX,  0,  0}, "neg",   "-(",   ""     },  // Negate.
   {Not,   1, 0, {RX,  0,  0}, "not",   "~(",   ""     },  // One's-complement.
// {pop,    1, 0, {RX,  0,  0}, "pop",   "pop(", ""     },  // Population count.
// {nlz,    1, 0, {RX,  0,  0}, "nlz",   "nlz(", ""     },  // Num leading 0's.
// {rev,    1, 0, {RX,  0,  0}, "rev",   "rev(", ""     },  // Bit reversal.
   {add,    2, 1, {RX,  2,  0}, "add",   "(",    " + "  },  // Add.
   {sub,    2, 0, { 2,  2,  0}, "sub",   "(",    " - "  },  // Subtract.
   {mul,    2, 1, {RX,  3,  0}, "mul",   "(",    "*"    },  // Multiply.
   {divide, 2, 0, { 1,  3,  0}, "div",   "(",    "/"    },  // Divide signed.
   {divu,   2, 0, { 1,  1,  0}, "divu",  "(",    " /u " },  // Divide unsigned.
   {And,   2, 1, {RX,  2,  0}, "and",   "(",    " & "  },  // AND.
   {Or,    2, 1, {RX,  2,  0}, "or",    "(",    " | "  },  // OR.
   {Xor,   2, 1, {RX,  2,  0}, "xor",   "(",    " ^ "  },  // XOR.
// {rotl,   2, 0, { 1,NIM,  0}, "rotl",  "(",    " <<r "},  // Rotate shift left.
   {shl,    2, 0, { 1,NIM,  0}, "shl",   "(",    " << " },  // Shift left.
   {shr,    2, 0, { 1,NIM,  0}, "shr",   "(",    " >>u "},  // Shift right.
   {shrs,   2, 0, { 3,NIM,  0}, "shrs",  "(",    " >>s "},  // Shift right signed.
// {cmpeq,  2, 1, {RX,  0,  0}, "cmpeq", "(",    " == " },  // Compare equal.
// {cmplt,  2, 0, { 0,  0,  0}, "cmplt", "(",    " < "  },  // Compare less than.
// {cmpltu, 2, 0, { 1,  1,  0}, "cmpltu","(",    " <u " },  // Compare less than unsigned.
// {seleq,  3, 0, {RX,  0,  0}, "seleq", "seleq(", ", " },  // Select if = 0.
// {sellt,  3, 0, {RX,  0,  0}, "sellt", "sellt(", ", " },  // Select if < 0.
// {selle,  3, 0, {RX,  0,  0}, "selle", "selle(", ", " },  // Select if <= 0.
};

/* ------------------- End of user-setup Portion -------------------- */

#define MAXNUMI 5               // Max num of insns that can be tried.
#if NARGS == 1
int userfun(int);
#else
int userfun(int, int);
#endif

#define NTRIALX (int)(sizeof(trialx)/sizeof(trialx[0]))
#define NTRIALY (int)(sizeof(trialy)/sizeof(trialy[0]))

#if NARGS == 1
   int correct_result[NTRIALX];
#else
   int correct_result[NTRIALX][NTRIALY];
#endif

int corr_result;                // Correct result for current trial.

#define NUM_INSNS_IN_ISA (int)(sizeof(isa)/sizeof(isa[0]))

struct {                        // The current program.
   int op;                      // Index into isa.
   int opnd[3];                 // Operands of op.  Register numbers
                                // except if negative, it's the negative
                                // of a shift amount.
} pgm[MAXNUMI];

int numi;                       // Current size of the trial programs,
                                // must be from 1 to MAXNUMI.

/* GPR array:  First NIM slots hold ordinary immediate values (IMMEDS),
next NSHIM slots hold shift immediate values (SHIMMEDS), next NARGS
slots hold the arguments x and, optionally, y, and the last numi slots
hold the result of instructions 0 through numi - 1. */

int r[NIM + NSHIM + NARGS + MAXNUMI] = {IMMEDS, SHIMMEDS};
unsigned counter[MAXNUMI];        // Count num times insn at level i is
                                  // evaluated.
