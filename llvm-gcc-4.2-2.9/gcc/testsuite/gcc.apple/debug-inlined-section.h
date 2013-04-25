/* APPLE LOCAL file, Radar 6275985  */

extern int globl;

static __inline__ __attribute__((always_inline)) get_length (struct mystruct *d) {
  if (globl && malloc (globl))
   {
     return getData (d) + globl;
   }
  else
   return getData (d) * globl;
}

extern int globl;

