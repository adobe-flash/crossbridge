#ifndef __FUNCTS_H
#define __FUNCTS_H

#include "types.h"

void speedup_test(FILE *fp);
uint32 imix_test(FILE *fp);
void specul_time_o(epoch_t *epoch, int num_epochs, FILE *graphfile, int show_speedup, int thread_pen, int commit_pen);
void specul_time_r(epoch_t *epoch, int num_epochs, int cpulimit, FILE *graphfile, int show_speedup, int thread_pen, int commit_pen);
void find_hard_raws();

#endif /* __FUNCTS_H */
