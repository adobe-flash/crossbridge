#include <stdio.h>
#include <string.h>
#include "types.h"
#include "functs.h"

extern hard_raw_t *hard_raw_list;

uint32 prog_time, loop_time;

void speedup_test(FILE *fp)
{
  char string[100],ostring[100],slask[100];
  uint32 address;
  uint32 issue_no1, issue_no2, issue_no3;
  
  rewind(fp);

  fscanf(fp,"%s %lu",string,&issue_no1);

  do {
    strcpy(ostring,string);
    fgets(string,100,fp);
  } while (string[0]!='E');

  sscanf(ostring,"%s %lx %lu",slask,&address,&issue_no2);
  sscanf(string,"%s %lu %lu",slask,&address,&issue_no3);

  loop_time=issue_no2-issue_no1;
  printf("Time for loop: %lu issues\n",loop_time);
  prog_time=issue_no3;
  printf("Time for program: %lu issues\n",prog_time);

  printf("Loop is %0.3g %% of program\n",(double)(issue_no2-issue_no1)/(double)issue_no3*100);
  rewind(fp);
}

uint32 imix_test(FILE *fp)
{
  char string[100],ostring[100],slask[100];
  uint32 address;
  uint32 issue_no1, issue_no2;
  uint32 data_access=0;

  rewind(fp); 

  fgets(string, 100, fp);

  fscanf(fp,"%s %lu",string,&issue_no1);
  do {
    if (string[2]==':')
      data_access++;
    strcpy(ostring,string);
    fgets(string,100,fp);
  } while (string[0]!='E');

  sscanf(ostring,"%s %lx %lu",slask,&address,&issue_no2);

  /* printf("Instuction mix: Data accesses are %0.3g %% of loop\n",(double)data_access/(double)(issue_no2-issue_no1)*100); */
  rewind(fp);
  return data_access;
}

void find_hard_raws()
{
  hard_raw_t *iter=NULL;
  
  iter=hard_raw_list;
  while (iter!=NULL) {
    if (iter->r_issue>iter->w_issue) {
      if (iter->prev!=NULL) {  
	iter->prev->next=iter->next;  
      }
      iter=iter->next;
    } else {
      iter=iter->next;
    }
  }
}

/* This function calculates the "optimal" execution time with speculation */
void specul_time_o(epoch_t *epoch, int num_epochs, FILE *graphfile, int show_speedup, int thread_pen, int commit_pen)
{
  int i;
  uint32 max;
  uint32 restarts=0;

  for (i=0;i<num_epochs;i++) {
    epoch[i].run_time=epoch[i].end_time-epoch[i].start_time;
    epoch[i].stall_time=0;  
  }

  {
    hard_raw_t *iter=NULL;
    iter=hard_raw_list; 
    while(iter!=NULL) {
      if ((iter->w_issue+epoch[iter->w_epoch].stall_time) > epoch[iter->r_epoch].stall_time) {
	epoch[iter->r_epoch].stall_time=iter->w_issue+epoch[iter->w_epoch].stall_time;
	restarts++;
      }
      iter=iter->next;
    }
  }

  for (i=0;i<num_epochs;i++) {
    epoch[i].run_time=epoch[i].run_time+epoch[i].stall_time;
  }
  
  max=epoch[0].run_time;
  for (i=0;i<num_epochs;i++) {  
    if (epoch[i].run_time>max)
       max=epoch[i].run_time;
  }
  
  printf("OPTIMUM RESTART RESULTS\n");
  max+=(restarts+num_epochs)*thread_pen+num_epochs*commit_pen;

  printf("Time for speculative loop is %lu issues\n",max);

  if (show_speedup == 1 || show_speedup == 3)
    printf("Potential speedup for loop: %0.3g times\n", (double)loop_time/(double)max);
  if (show_speedup == 2 || show_speedup == 3)
    printf("Potential speedup for program: %0.3g times\n", (double)prog_time/(max+prog_time-loop_time));
  if (graphfile!=NULL) { 
    if (show_speedup == 1 || show_speedup == 3)
      fprintf(graphfile,"optloop: %0.3g\n", (double)loop_time/(double)max);
    if (show_speedup == 2 || show_speedup == 3)
      fprintf(graphfile,"optprog: %0.3g\n", (double)prog_time/(double)(max+prog_time-loop_time));
  }
}

/* These functions calculates the "realistic" execution time with speculation */
void specul_time_r(epoch_t *epoch, int num_epochs, int cpulimit, FILE *graphfile, int show_speedup, int thread_pen, int commit_pen)
{
  int i;
  uint32 max=0;
  uint32 restarts=0;
  
  if (cpulimit==0) {  /* "Unlimited" amount of CPUs simulated */
    printf("REALISTIC RESTART RESULTS -- Unlimited amount of CPUs\n");
    for (i=0;i<num_epochs;i++) {
      epoch[i].run_time=epoch[i].end_time-epoch[i].start_time;
      epoch[i].stall_time=0; 
    }
    
    {
      hard_raw_t *iter=NULL;
      iter=hard_raw_list; 
      while(iter!=NULL) {
	if ((iter->w_issue+epoch[iter->w_epoch].stall_time) > epoch[iter->r_epoch].stall_time) {
	  epoch[iter->r_epoch].stall_time=epoch[iter->w_epoch].run_time+epoch[iter->w_epoch].stall_time;
	  restarts++;
	}
	iter=iter->next;
      }
    }
    
    for (i=0;i<num_epochs;i++) {
      epoch[i].run_time=epoch[i].run_time+epoch[i].stall_time;
    }
    
    max=epoch[0].run_time;
    for (i=0;i<num_epochs;i++) {  
      if (epoch[i].run_time>max)
	max=epoch[i].run_time;
    }
  } else { /* Limited amount of CPUs simulated */
    int j;
    uint32 current_stall=0;
    printf("REALISTIC RESTART RESULTS -- %d CPUs\n",cpulimit);
    for (i=0;i<num_epochs;i++) {
      epoch[i].run_time=epoch[i].end_time-epoch[i].start_time; 
      epoch[i].stall_time=0;
    }
    for (i=cpulimit;i<num_epochs;i+=cpulimit) {
      current_stall=0;
      for (j=i-cpulimit;j<i;j++) {
	if (epoch[j].run_time+epoch[j].stall_time>current_stall)
	  current_stall=epoch[j].run_time+epoch[j].stall_time;
      }
      for (j=i;j<cpulimit+i;j++) {
	epoch[j].stall_time=current_stall;
      }
    }
    {
      hard_raw_t *iter=NULL;
      iter=hard_raw_list; 
      while(iter!=NULL) {
	if ((iter->w_issue+epoch[iter->w_epoch].stall_time) > epoch[iter->r_epoch].stall_time) {
	  epoch[iter->r_epoch].stall_time+=epoch[iter->w_epoch].run_time+epoch[iter->w_epoch].stall_time;
	  restarts++;
	}
	iter=iter->next;
      }
    }
    for (i=0;i<num_epochs;i++) {
      epoch[i].run_time=epoch[i].run_time+epoch[i].stall_time;
    }
    
    max=epoch[0].run_time;
    for (i=0;i<num_epochs;i++) {  
      if (epoch[i].run_time>max)
	max=epoch[i].run_time;
    }

  }
  max+=(restarts+num_epochs)*thread_pen+num_epochs*commit_pen;

  printf("Time for speculative loop is %lu issues\n",max);

  if (show_speedup == 1 || show_speedup == 3)
    printf("Potential speedup for loop: %0.3g times\n", (double)loop_time/(double)max);
  if (show_speedup == 2 || show_speedup == 3)
    printf("Potential speedup for program: %0.3g times\n", (double)prog_time/(double)(max+prog_time-loop_time));
  if (graphfile!=NULL) {
    if (show_speedup == 1 || show_speedup == 3)
      fprintf(graphfile,"realloop: %d %0.3g\n", cpulimit, (double)loop_time/(double)max);
    if (show_speedup == 2 || show_speedup == 3)
      fprintf(graphfile,"realprog: %d %0.3g\n", cpulimit, (double)prog_time/(double)(max+prog_time-loop_time));
  }
}
