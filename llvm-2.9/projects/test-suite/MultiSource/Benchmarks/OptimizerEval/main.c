#include "global.h"
#define SAMPLES 5

#define CUTOFF 2.552

#define SLOW_FACTOR 10.0
/* SLOW_FACTOR was 100.0 for stock benchmark */

clock_t ticks_per_sec = 0;

int test(int ok,
         FunPar base_case,
         FunPar test_case,
         int *data,
         int n,
         char *id);
long int determine_iterations(FunPar base_case,
                              int *data);

void collect_timings(long int iterations,
                     FunPar routine,
                     int *data,
                     double *times);
double mean(double *times);
double variance(double mean, double *times);
extern void dead_test1(int *data);
extern void dead_test2(int *data);
extern void dead_test3(int *data);
extern void dead_test4(int *data);
extern void dead_test5(int *data);
extern void dead_test6(int *data);
extern void dead_test7(int *data);
extern void dead_test8(int *data);
extern void dead_test9(int *data);
extern void dead_test10(int *data);
extern void dead_test11(int *data);
extern void dead_result1(int *data);
extern void dead_result2(int *data);
extern void dead_result3(int *data);
extern void dead_result4(int *data);
extern void dead_result5(int *data);
extern void dead_result6(int *data);
extern void dead_result7(int *data);
extern void dead_result8(int *data);
extern void dead_result9(int *data);
extern void dead_result10(int *data);
extern void dead_result11(int *data);
extern void cprop_test1(int *data);
extern void cprop_test2(int *data);
extern void cprop_test3(int *data);
extern void cprop_test35(int *data);
extern void cprop_test4(int *data);
extern void cprop_test5(int *data);
extern void cprop_test6(int *data);
extern void cprop_test7(int *data);
extern void cprop_test8(int *data);
extern void cprop_test9(int *data);
extern void cprop_test10(int *data);
extern void cprop_test11(int *data);
extern void cprop_test12(int *data);
extern void cprop_test13(int *data);
extern void cprop_test14(int *data);
extern void cprop_result1(int *data);
extern void cprop_result2(int *data);
extern void cprop_result3(int *data);
extern void cprop_result4(int *data);
extern void cprop_result5(int *data);
extern void cprop_result6(int *data);
extern void cprop_result7(int *data);
extern void cprop_result8(int *data);
extern void cprop_result9(int *data);
extern void cprop_result10(int *data);
extern void cprop_result11(int *data);
extern void cprop_result12(int *data);
extern void cprop_result13(int *data);
extern void cprop_result14(int *data);
extern void vnum_test1(int *);
extern void vnum_test2(int *);
extern void vnum_test3(int *);
extern void vnum_test4(int *);
extern void vnum_test5(int *);
extern void vnum_test6(int *);
extern void vnum_test7(int *);
extern void vnum_test8(int *);
extern void vnum_test9(int *);
extern void vnum_test10(int *);
extern void vnum_test11(int *);
extern void vnum_test12(int *);
extern void vnum_result1(int *);
extern void vnum_result2(int *);
extern void vnum_result3(int *);
extern void vnum_result4(int *);
extern void vnum_result5(int *);
extern void vnum_result6(int *);
extern void vnum_result7(int *);
extern void vnum_result8(int *);
extern void vnum_result9(int *);
extern void vnum_result10(int *);
extern void vnum_result11(int *);
extern void vnum_result12(int *);
extern void motion_test1(int *);
extern void motion_test2(int *);
extern void motion_test3(int *);
extern void motion_test4(int *);
extern void motion_test5(int *);
extern void motion_test6(int *);
extern void motion_test7(int *);
extern void motion_test8(int *);
extern void motion_test9(int *);
extern void motion_test10(int *);
extern void motion_test11(int *);
extern void motion_result1(int *);
extern void motion_result2(int *);
extern void motion_result3(int *);
extern void motion_result4(int *);
extern void motion_result5(int *);
extern void motion_result6(int *);
extern void motion_result7(int *);
extern void motion_result8(int *);
extern void motion_result9(int *);
extern void motion_result10(int *);
extern void motion_result11(int *);
extern void strength_test1(int *);
extern void strength_test2(int *);
extern void strength_test3(int *);
extern void strength_test4(int *);
extern void strength_test5(int *);
extern void strength_test6(int *);
extern void strength_test7(int *);
extern void strength_test8(int *);
extern void strength_test9(int *);
extern void strength_test10(int *);
extern void strength_result1(int *);
extern void strength_result2(int *);
extern void strength_result3(int *);
extern void strength_result4(int *);
extern void strength_result5(int *);
extern void strength_result6(int *);
extern void strength_result7(int *);
extern void strength_result8(int *);
extern void strength_result9(int *);
extern void strength_result10(int *);

int main()
{
  {
    clock_t start, diff;
    {
      if (clock() == -1) {
        fputs("\nThe routine 'clock' is not supported\n", stderr);
        exit(-1);
      }
    }
    start = clock();
    diff = clock() - start;
    while (diff == 0)
      diff = clock() - start;
    ticks_per_sec =  CLOCKS_PER_SEC / diff;
    printf("Timer seems to run at %d ticks/second\n", ticks_per_sec);
  }
  puts("Begin tests, version 0.06");
  {
    int ok;
    puts("Dead code elimination");
    {
      int data[3] = { 0, 1, 2 };
      ok = test(1, dead_result1, dead_test1, data, 1, "basic block");
    }{
      int data[5] = { 0, 1, 2, 3, 4 };
      ok = test(ok, dead_result2, dead_test2, data, 2, "basic block");
    }{
      int data[4] = { 0, 1, 2 };
      ok = test(ok, dead_result3, dead_test3, data, 3, "across basic blocks");
    }{
      int data[5] = { 0, 1, 2, 3, 4 };
      ok = test(ok, dead_result4, dead_test4, data, 4, "across basic blocks");
    }{
      int data[3] = { 2, 1, 2 };
      ok = test(ok, dead_result5, dead_test5, data, 5, "around loops");
    }{
      int data[4] = { 2, 1, 2 };
      ok = test(ok, dead_result6, dead_test6, data, 6, "around loops");
    }{
      int data[3] = { 0, 1, 2 };
      ok = test(1, dead_result7, dead_test7, data, 7, "useless conditionals");
    }{
      int data[3] = { 2, 1, 2 };
      ok = test(ok, dead_result8, dead_test8, data, 8, "useless conditionals");
    }{
      int data[3] = { 0, 1, 2 };
      ok = test(1, dead_result9, dead_test9, data, 9, "useless loops (conservative)");
    }{
      int data[3] = { 2, 1, 2 };
      (void) test(ok, dead_result10, dead_test10, data, 10, "useless loops (aggressive)");
    }{
      int data[3] = { 0, 1, 2 };
      (void) test(1, dead_result11, dead_test11, data, 11, "partially dead");
    }
  }{
    int ok;
    puts("Constant propagation");
    {
      int data[1];
      ok = test(1, cprop_result1, cprop_test1, data, 1, "folding");
    }{
      int data[1];
      ok = test(ok, cprop_result2, cprop_test2, data, 2, "basic block");
    }{
      int data[3] = { 1 };
      ok = test(ok, cprop_result3, cprop_test3, data, 3, "extended basic blocks");
    }{
      int data[3] = { 1 };
      (void) test(ok, cprop_result4, cprop_test4, data, 4, "extended basic blocks");
    }{
      int data[4] = { 1 };
      ok = test(ok, cprop_result5, cprop_test5, data, 5, "dominators");
    }{
      int data[4] = { 1 };
      (void) test(ok, cprop_result6, cprop_test6, data, 6, "dominators");
    }{
      int data[4] = { 1 };
      ok = test(ok, cprop_result7, cprop_test7, data, 7, "dags");
    }{
      int data[4] = { 1 };
      (void) test(ok, cprop_result8, cprop_test8, data, 8, "dags (hard)");
    }{
      int data[3] = { 2 };
      ok = test(ok, cprop_result9, cprop_test9, data, 9, "loops");
    }{
      int data[2] = { 1, 2 };
      ok = test(1, cprop_result10, cprop_test10, data, 10, "conditional constants");
    }{
      int data[3] = { 2 };
      (void) test(ok, cprop_result11, cprop_test11, data, 11, "conditional constants");
    }{
      int data[2] = { 5 };
      ok = test(1, cprop_result12, cprop_test12, data, 12, "conditional-based assertions");
    }{
      int data[3] = { 10, 5 };
      (void) test(ok, cprop_result13, cprop_test13, data, 13, "conditional-based assertions");
    }{
      int data[4] = { 0, 1, 2 };
      (void) test(ok, cprop_result14, cprop_test14, data, 14, "reassociation");
    }
  }{
    int ok;
    puts("Value numbering");
    {
      int data[4] = {0, 1, 2, 3 };
      ok = test(1,  vnum_result1, vnum_test1, data, 1, "expressions");
    }{
      int data[4] = { 0, 1, 2, 3 };
      (void)  test(ok, vnum_result2, vnum_test2, data, 2, "expressions");
    }{
      int data[4] = { 0, 1, 2, 3 };
      ok = test(ok, vnum_result3, vnum_test3, data, 3, "basic block");
    }{
      int data[4] = { 0, 1, 2, 3 };
      (void) test(ok, vnum_result4, vnum_test4, data, 4, "basic block");
    }{
      int data[4] = { 0, 1, 2, 3 };
      ok = test(ok, vnum_result5, vnum_test5, data, 5, "extended basic block");
    }{
      int data[5] = { 0, 1, 2, 3, 4 };
      ok = test(ok, vnum_result6, vnum_test6, data, 6, "dominators");
    }{
      int data[5] = { 0, 1, 2, 3, 4 };
      ok = test(1,  vnum_result7, vnum_test7, data, 7, "global DAGs");
    }{
      int data[5] = { 0, 1, 2, 3, 4 };
      ok = test(ok, vnum_result8, vnum_test8, data, 8, "global loops");
    }{
      int data[3] = { 0, 0 };
      ok = test(1, vnum_result9, vnum_test9, data, 9, "conditional-based assertions");
    }{
      int data[4] = { 0, 0 };
      ok = test(ok, vnum_result10, vnum_test10, data, 10, "conditional-based assertions");
    }{
      int data[4] = { 0, 1, 2, 3 };
      ok = test(1, vnum_result11, vnum_test11, data, 11, "conditional value numbers");
    }{
      int data[4] = { 0, 1, 2, 3 };
      ok = test(ok, vnum_result12, vnum_test12, data, 12, "cprop + vnum");
    }
  }{
    int ok;
    puts("Code motion");
    {
      int data[5] = { 0, 1, 2, 3 };
      ok = test(1, motion_result1, motion_test1, data, 1, "DAGs");
    }{
      int data[6] = { 0, 1, 2, 3 };
      ok = test(ok, motion_result2, motion_test2, data, 2, "DAGs (edge placement)");
    }{
      int data[5] = { 0, 1, 2, 3 };
      ok = test(1, motion_result3, motion_test3, data, 3, "loops");
    }{
      int data[5] = { 0, 1, 2, 3 };
      (void) test(ok, motion_result4, motion_test4, data, 4, "loops (hoisting divide)");
    }{
      int data[5] = { 0, 1, 2, 3 };
      (void) test(ok, motion_result5, motion_test5, data, 5, "irreducible loops");
    }{
      int data[5] = { 0, 1, 0, 3 };
      (void) test(ok, motion_result6, motion_test6, data, 6, "reassociation");
    }{
      int data[5] = { 0, 1, 4, 10 };
      ok = !test(1, motion_result7, motion_test7, data, 7, "aggressive");
    }{
      int data[5] = { 0, 1, 2, 3 };
      (void) test(ok, motion_result8, motion_test8, data, 8, "loop rotation");
    }{
      int data[5] = { 0, 1, 2, 3 };
      (void) test(ok, motion_result9, motion_test9, data, 9, "loop rotation");
    }{
      int data[5] = { 0, 1, 2, 10 };
      (void) test(ok, motion_result10, motion_test10, data, 10, "invariant control structures");
    }{
      int data[5] = { 0, 1, 2, 10 };
      (void) test(ok, motion_result11, motion_test11, data, 11, "loop unswitching");
    }
  }{
    int ok;
    puts("Strength reduction");
    {
      int data[3] = { 0, 22, 2 };
      ok = test(1, strength_result1, strength_test1, data, 1, "iv * constant");
    }{
      int data[3] = { 1, 3, 2 };
      (void) test(ok, strength_result2, strength_test2, data, 2, "iv * rc");
    }{
      int data[4] = { 0, 1, 2, 100 };
      (void) test(ok, strength_result3, strength_test3, data, 3, "iv * iv");
    }{
      int data[4] = { 0, 1, 2, 100 };
      (void) test(ok, strength_result4, strength_test4, data, 4, "irreducible loop");
    }{
      int data[4] = { 0, 1, 2, 100 };
      (void) test(ok, strength_result5, strength_test5, data, 5, "control flow in loop");
    }{
      int data[4] = { 1, 1, 2, 100 };
      (void) test(ok, strength_result6, strength_test6, data, 6, "inc by rc");
    }{
      int data[4] = { 0, 1, 2, 100 };
      (void) test(ok, strength_result7, strength_test7, data, 7, "monotonic iv");
    }{
      int data[4] = { 0, 1, 1, 100 };
      (void) test(ok, strength_result8, strength_test8, data, 8, "mutual iv's");
    }{
      int data[4] = { 0, 1, 1, 100 };
      (void) test(ok, strength_result9, strength_test9, data, 9, "multiple strides");
    }{
      int data[4] = { 0, 1, 1, 20 };
      (void) test(ok, strength_result10, strength_test10, data, 10, "test replacement");
    }
  }
  puts("Tests completed");
  exit(0);
}
int test(int ok,
         FunPar base_case,
         FunPar test_case,
         int *data,
         int n,
         char *id)
{
  int ret_val = 0;
  printf("    %d) %s - ", n, id);
  if (1 /* always test, for now */ || ok) {
    double base_times[SAMPLES], test_times[SAMPLES];
    long int iterations = determine_iterations(base_case, data);
    collect_timings(iterations, test_case, data, test_times);
    collect_timings(iterations, base_case, data, base_times);
    {
      double base_mean = mean(base_times);
      double test_mean = mean(test_times);
      double result = test_mean - base_mean;
      double base_variance = variance(base_mean, base_times);
      double test_variance = variance(test_mean, test_times);
      double sum = base_variance + test_variance;
      if (sum == 0.0) {
        if (result == 0.0)     ret_val = 1;
        else if (result < 0.0) ret_val = -1;
      }
      else {
        result = result * sqrt(SAMPLES / sum);
        if (fabs(result) <= CUTOFF) ret_val = 1;
        else if (result < 0.0)      ret_val = -1;
      }
      if (ret_val > 0)       puts("yes");
      else if (ret_val == 0) puts("no");
      else                   puts("bad test");
      {
        int n = noisy(base_variance) || noisy(test_variance);
        if (n)
          puts("\t(warning -- the timings were not very consistent)");
        if (1 /* always print, for now */ || n || ret_val == -1)
          {
            int i;
            puts("\nTimings\n\tbase-case\ttest-case");
            for (i=0; i<SAMPLES; i++)
              printf("\t%-9.3f\t%-9.3f\n", base_times[i], test_times[i]);
            printf("means\t%-9.3f\t%-9.3f\n", base_mean, test_mean);
            printf("vars\t%-9.6f\t%-9.6f\n", base_variance, test_variance);
            printf("sds\t%-9.6f\t%-9.6f\n", sqrt(base_variance), sqrt(test_variance));
            printf("result\t%-9.6f\n\n", result);
          }
      }
    }
  }
  else puts("skipping");
  return ret_val;
}
int noisy(double variance)
{
  double std_dev = sqrt(variance);
  return std_dev > 2.0 / ticks_per_sec;
}
double mean(double *times)
{
  int i;
  double sum = 0.0;
  for (i=0; i<SAMPLES; i++)
    sum += times[i];
  return sum / SAMPLES;
}
double variance(double mean, double *times)
{
  int i;
  double sum = 0.0;
  for (i=0; i<SAMPLES; i++) {
    double diff = mean - times[i];
    sum += diff * diff;
  }
  return sum / (SAMPLES - 1);
}
long int determine_iterations(FunPar base_case,
                              int *data)
{
  long int iterations = 1;
  base_case(data);  /* an initial, untimed invocation */
  while (1) {
    long int i;
    clock_t start = clock();
    for (i=0; i<iterations; i++)
      base_case(data);
    if (((double) clock() - start) / CLOCKS_PER_SEC > SLOW_FACTOR / ticks_per_sec)
      return iterations;
    iterations += iterations;
  }
}
void collect_timings(long int iterations,
                     FunPar routine,
                     int *data,
                     double *times)
{
  int i;
  routine(data);  /* an initial, untimed invocation */
  for (i=0; i<SAMPLES; i++) {
    long int j;
    clock_t start = clock();
    for (j=0; j<iterations; j++)
      routine(data);
    times[i] = ((double) (clock() - start)) / CLOCKS_PER_SEC;
  }
}
