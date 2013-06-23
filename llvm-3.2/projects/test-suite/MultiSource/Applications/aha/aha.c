                       /* A Hacker's Assistant */

// Copyright (C) 2002 by Henry S. Warren, Jr.
#include <stdlib.h>
#include <stdio.h>
#include "aha.h"

// --------------------------- print_expr ------------------------------

void
print_expr(int opn)
{
   int i, j, k;

   if (opn < RX) {                      // Immediate value.
      if (-31 <= r[opn] && r[opn] <= 31) printf("%d", r[opn]);
      else                               printf("0x%X", r[opn]);
   }
   else if (opn == RX) printf("x");     // First argument.
#if NARGS >= 2
   else if (opn == RY) printf("y");     // Second argument.
#endif
   else {                               // opn is an instruction.
      i = opn - RI0;
      k = pgm[i].op;
      printf("%s", isa[k].fun_name);
      for (j = 0; j < isa[k].numopnds; j++) {
         print_expr(pgm[i].opnd[j]);
         if (j < isa[k].numopnds - 1) printf("%s", isa[k].op_name);
         else                         printf(")");
      }
   }
}

// --------------------------- print_pgm -------------------------------

void
print_pgm()
{
   int i, j, k, opndj;

   for (i = 0; i < numi; i++) {
      k = pgm[i].op;
      printf("   %-5s r%d,", isa[k].mnemonic, i + 1);
      for (j = 0; j < isa[k].numopnds; j++) {
         opndj = pgm[i].opnd[j];
         if (opndj < RX) {
            opndj = r[opndj];
            if (opndj >= -31 && opndj <= 31) printf("%d", opndj);
            else printf("0x%X", opndj);
         }
         else if (opndj == RX) printf( "rx");
#if NARGS > 1
         else if (opndj == RY) printf("ry");
#endif
         else printf("r%d", opndj - RI0 + 1);
         if (j < isa[k].numopnds - 1) printf(",");
      }
      if (debug)
         printf("     ==> %d (0x%X)\n", r[i+RI0], r[i+RI0]);
      else printf("\n");
   } // end for i

   /* Now print the program as an expression. */

   printf("   Expr: ");
   print_expr(numi - 1 + RI0);
   printf("\n");
}

// -------------------- simulate_one_instruction -----------------------

static inline void
simulate_one_instruction(int i)
{
   int arg0, arg1, arg2;

   arg0 = r[pgm[i].opnd[0]];
   arg1 = r[pgm[i].opnd[1]];
   arg2 = r[pgm[i].opnd[2]];

   r[i + RI0] = (*isa[pgm[i].op].proc)(arg0, arg1, arg2);
   if (counters) counter[i] = counter[i] + 1;
   return;
}

// ----------------------------- check ---------------------------------

int
check(int i)
{
  int kx;
   static int itrialx;          // Init 0.
#if NARGS == 2
   static int itrialy;
#endif

   if (debug) {
#if NARGS == 1
      printf("\nSimulating with trial arg x = %d (0x%X):\n",
         r[RX],r[RX]);
#else
      printf("\nSimulating with (x, y) = (%d, %d) ((0x%X, 0x%X)):\n",
         r[RX], r[RY], r[RX], r[RY]);
#endif
   }
L:
      simulate_one_instruction(i);              // Simulate i'th insn,
      if (i < numi - 1) {i = i + 1; goto L;}    // and more if req'd
      if (unacceptable) {       // E.g., if divide by 0:
         if (debug) printf("Unacceptable program (invalid operation).\n");
         unacceptable = 0;
         return 0;
      }

   if (debug) {
      print_pgm(2);
      printf("Computed result = %d, correct result = %d, %s\n",
      r[numi-1+RI0], corr_result, r[numi-1+RI0] == corr_result ? "ok" : "fail");
   }
   if (r[numi-1+RI0] != corr_result)    // If not the correct
      return 0;                         // result, failure.

   // Got the correct result.  Check this program using all trial values.

   for (kx = 0; kx < NTRIALX - 1; kx++) {
      itrialx += 1;
      if (itrialx >= NTRIALX) itrialx = 0;
      r[RX] = trialx[itrialx];
#if NARGS == 1
      corr_result = correct_result[itrialx];
#else
   for (int ky = 0; ky < NTRIALY - 1; ky++) {
      itrialy += 1;
      if (itrialy >= NTRIALY) itrialy = 0;
      r[RX] = trialx[itrialx];
      r[RY] = trialy[itrialy];
      corr_result = correct_result[itrialx][itrialy];
#endif

      /* Now we simulate the current program, i.e., the instructions
      from 0 to numi-1.  The result of instruction i goes in
      register i + RI0. */

      if (debug) {
#if NARGS == 1
         printf("\nContinuing this pgm with arg x = %d (0x%X):\n",
            r[RX], r[RX]);
#else
         printf("\nContinuing this pgm with (x, y) = (%d, %d) ((0x%X, 0x%X)):\n",
            r[RX], r[RY], r[RX], r[RY]);
#endif
      }
      for (i = 0; i < numi; i++) {      // Simulate program from
         simulate_one_instruction(i);   // beginning to end.
      }
      if (unacceptable) {unacceptable = 0; return 0;}
      if (debug) {
         print_pgm(2);
         printf("Computed result = %d, correct result = %d, %s\n",
         r[numi+RI0-1], corr_result, r[numi+RI0-1] == corr_result ? "ok" : "fail");
      }
      if (r[numi+RI0-1] != corr_result) return 0;
#if NARGS == 2
   }  // end ky
#endif
   }  // end kx
   return 1;                    // Passed all tests, found a
                                // probably correct program.
}

// -------------------------- fix_operands -----------------------------

void
fix_operands(int i)
{

/* This program fixes instruction i so that:

(1) if it is the last instruction, at least one operand uses the
    result of the immediately preceding instruction, and furthermore if
    the second from last instruction does not use the result of its
    predecsssor, then the last instruction must use that result also.
(2) not all operands are immediate values, and (We assume it would be
    a waste of time to process an instruction with all immediate
    operands).
(3) if it is commutative, operand 0 >= operand 1,

   It does these fixes by "increasing" the instruction by a minimal
amount, so that the incrementing of instructions is kept in order and no
legitimate instructions are skipped.
   A hard part to understand is the logic of (1) above.  Let us assume
for illustration that the program has four instructions (numi = 4).
Then when this subroutine is called to process the last instruction (i =
numi - 1), the operands may be in any of the configurations shown below.
The last instruction sets r4, the second from last instruction sets r3,
and its predecessor sets r2.  ii denotes a register containing an
immediate value, or a register <= RY; in particular ii < r2.  We assume
the last instruction ("op") has three input operands, as that is the
more difficult case, and that the second from last instruction does not
use r2.  Therefore the last instruction must be altered so that it uses
both r2 and r3.

operand:        0  1  2                0  1  2
         op r4,ii,ii,ii   ==>   op r4,r3,r2,ii   Add r2 and r3.
         op r4,ii,r2,ii   ==>   op r4,r3,r2,ii   Add r3.
         op r4,ii,r3,ii   ==>   op r4,r2,r3,ii   Add r2.
         op r4,ii,ii,r2   ==>   op r4,r3,ii,r2   Add r3.
         op r4,ii,r2,r2   ==>   op r4,r3,r2,r2   Add r3.
         op r4,ii,r3,r2   ==>     no change
         op r4,ii,ii,r3   ==>   op r4,r2,ii,r3   Add r2.
         op r4,ii,r2,r3   ==>     no change
         op r4,ii,r3,r3   ==>   op r4,r2,r3,r3

   These are the only possibilities.  The first input operand cannot be
r2 or r3, because if it were, then it must have just been incremented
from r1 or r2 resp., and in this case "increment" does not call
"fix_operands."
   The first row above means that if none of the last instruction's
operands are r2 or r3, then the change that adds r2 and r3 and that
"minimizes" the resulting instruction is to change operand 0 to r3 and
operand 1 to r2.  The second row shows a case in which r2 is already
present, but r3 is not.  The minimal change is to change operand 0 to r3.
   Examination of all the possibilities reveals that a workable simple
rule is:
   (1) If r3 is not used, then change operand 0 to be r3.
   (2) Then, if r2 is not used, change operand 0 to r2 unless that
       decreases the instruction, in which case change operand 1 to r2.
   These rules are coded in the block headed by "if (i == numi - 1)".
It might seem that the program should test that pgm[i].opnd[0] is not
equal to rs or rt; however, as noted above operand 0 is never equal
to those registers at this point.
   This scheme is sufficient to ensure that if numi = 3, no trial
program has an unused computed value.  If numi = 4, a small percentage
of trial programs will have an unused computed value.  Incorporation
of the r2 part of it improved the execution time by about a factor of
1.4 if numi = 3, and a factor of 1.8 if numi = 4.  If numi = 5, there
is probably a substantial percentage of trial programs with one or
more unused computed values; it hasn't been tried. */

   int rs, rt, k;

   k = pgm[i].op;

   if (i == numi - 1) {         // If this is the last insn:
      rs = numi + RI0 - 2;      // Second from last reg.
      if (pgm[i].opnd[1] != rs && pgm[i].opnd[2] != rs) {
         pgm[i].opnd[0] = rs;
      }
      rt = rs - 1;              // Third from last reg.
      if (pgm[i-1].opnd[0] != rt && pgm[i-1].opnd[1] != rt &&
         pgm[i-1].opnd[2] != rt && pgm[i].opnd[1] != rt &&
         pgm[i].opnd[2] != rt && rt >= RI0) {

         // The last instruction needs to reference rt.

         if (pgm[i].opnd[0] < rt) pgm[i].opnd[0] = rt;
         else if (isa[k].numopnds > 1) pgm[i].opnd[1] = rt;

         // else (unary op), forget it.
      }
   }

   if (isa[k].commutative) {
      if (pgm[i].opnd[0] < pgm[i].opnd[1])
         pgm[i].opnd[0] = pgm[i].opnd[1];
      return;                   // No need to do next check, as opnd[0]
   }                            // is always a reg containing a variable.

   if (i != numi - 1) {
      if (pgm[i].opnd[0] < RX && pgm[i].opnd[1] < RX &&
         pgm[i].opnd[2] < RX) {
         if (isa[k].commutative) abort();
         pgm[i].opnd[0] = RX;
      }
   }
}

// --------------------------- increment -------------------------------

static inline int
increment(void)
{

   /* This routine "increments" the instruction list, in a manner
   similar to counting.  The instruction list changes basically
   like this:

      i0  r0,r0       i0  r0,r0       i0  r0,r0       i0  r0,r0
      i0  r0,r0  ==>  i0  r0,r0  ==>  i0  r0,r0  ==>  i0  r0,r0  etc.
      i0  r0,r0       i0  r1,r0       i0  r2,r0       i0  r0,r1

   The bottom left operand is tested.  If it has not reached its
   maximum value, it is incremented.  If it has reached its maximum
   value, it is reset to its starting value and the operand to its right
   is incremented if possible.  If all operands have reached their
   maxima, the last instruction is replaced with the next instruction
   in the isa list, if possible, etc.
      The returned value is the lowest index i of the instructions
   modified, or -1 if the instruction list cannot be incremented anymore
   ("done").
      As far as incrementing goes, there are only three types of operands:

   1. Goes through the ordinary immediate values, skips the shift
      immediate values, and then goes through the registers.
   2. Goes through the shift immediate values followed by the registers.
   3. Goes through the registers only.

   Which range an operand is in can be determined by its register number
   alone, so we don't need operand types in the ISA.  However, opnd[0]
   of a commutative op is an exception in that it doesn't go through
   all the register values; it skips register values for which it is
   less than opnd[1].
      There's no doubt a faster way to program this, maybe by using
   some fairly large tables. */

   int i, j, k, opndj, nopnds;

   for (i = numi - 1; i >= 0; i--) {
      k = pgm[i].op;
      nopnds = isa[k].numopnds;
      for (j = 0; j < nopnds; j++) {
         opndj = pgm[i].opnd[j];

         if (opndj < NIM - 1) {         // If ordinary imm. and not last,
            pgm[i].opnd[j] += 1;        // increment the operand.
            break;
         }
         else if (opndj == NIM - 1) {   // If last ordinary imm. operand,
            pgm[i].opnd[j] = RX;        // skip to first register.
            break;
         }
         else if (opndj < i + RI0 - 1) {// If shift imm. or reg and not
            pgm[i].opnd[j] += 1;        // last, increment the operand.
            break;
         }
                                        // We're at the end for opnd j.
         pgm[i].opnd[j] = isa[k].opndstart[j];       // Reset it and
                                        // increment next operand to
                                        // its right.
      } // end for j

      if (j == 0)                       // If we just incremented the
         return i;                      // leftmost operand, return; the
                                        // following check is not necessary.
      if (j < nopnds) {

         /* We just incremented some operand other than the rightmost,
         which means we reset one or more operands.  Must ensure that if
         the instruction is commutative then opnd[0] >= opnd[1], that
         the operands are not all immediate values, and if this is the
         last instruction, that at least one operand refers to the
         second from last instruction and possibly to the instruction
         before that.  */

         fix_operands(i);
         return i;
      }

      /* Have gone through all of insn i's opnds.
      Increment the instruction itself (if possible). */

      if (k < NUM_INSNS_IN_ISA - 1) {
         k = k + 1;             // Increment to next isa instruction.
         pgm[i].op = k;
         pgm[i].opnd[0] = isa[k].opndstart[0];
         pgm[i].opnd[1] = isa[k].opndstart[1];
         pgm[i].opnd[2] = isa[k].opndstart[2];

         fix_operands(i);
         return i;
      }

      /* Cannot increment to next isa insn.  Reset it to the first
      isa insn and look at next insn down in the program.  Furthermore,
      if the insn being reset is the last insn in the program, make
      its first opnd pick up the previous insn's result. */

      pgm[i].op = 0;                    // Index first insn in isa.
      pgm[i].opnd[0] = isa[0].opndstart[0];
      pgm[i].opnd[1] = isa[0].opndstart[1];
      pgm[i].opnd[2] = isa[0].opndstart[2];

      fix_operands(i);
   } // end for i
   return -1;                   // Return "done" indication.
}

// ----------------------------- search --------------------------------

int
search(void)
{

   int ok, i, num_solutions;

#if NARGS == 1
   r[RX] = trialx[0];                   // Must initialize these for
   corr_result = correct_result[0];     // speed-up thing in "check."
#else
   r[RX] = trialx[0];
   r[RY] = trialy[0];
   corr_result = correct_result[0][0];
#endif
   num_solutions = 0;
   i = 0;
   do {
      ok = check(i);            // Simulate the program from i on.
      if (ok) {
         num_solutions += 1;
         printf("\nFound a %d-operation program:\n", numi);
         print_pgm(3);
         if (num_solutions == 5) return num_solutions; // bail out early
      }
      i = increment();          // Increment to next program.
   } while (i >= 0);
   return num_solutions;
}

// -------------------------- Main Program -----------------------------

int main(int argc, char *argv[]) {
   int i, num_sol = 0;

   for (numi = 1; numi <= MAXNUMI && num_sol == 0; ++numi) {
     printf("Searching for programs with %d operations.\n", numi);

     // Compute all the correct answers and save them in an array.

     for (i = 0; i < NTRIALX; i++) {
#if NARGS == 1
       correct_result[i] = userfun(trialx[i]);
#else
       for (int j = 0; j < NTRIALY; j++)
         correct_result[i][j] = userfun(trialx[i], trialy[j]);
#endif
     }

     /* Preload the instruction array with the first instruction and
        the lowest register number, with copies of this instruction
        filling the whole array from 0 to numi - 1. */
     
     for (i = 0; i < numi; i++) {
       pgm[i].op = 0;                    // Index first insn in isa.
       pgm[i].opnd[0] = isa[0].opndstart[0];
       pgm[i].opnd[1] = isa[0].opndstart[1];
       pgm[i].opnd[2] = isa[0].opndstart[2];
       
       /* Ensure that the instruction does not have all immediate
          operands, etc. */
       
       fix_operands(i);
     }
     
     // Check the above program, generate the next, check it, etc.
     num_sol = search();
     
     printf("Found %d solutions.\n", num_sol);
     if (counters) {
       int total = 0;
       printf("Counters = ");
       for (i = 0; i < numi; i++) {
         printf("%d, ", counter[i]);
         total = total + counter[i];
       }
       printf("total = %d\n", total);
     }
   }
   return 0;
}
