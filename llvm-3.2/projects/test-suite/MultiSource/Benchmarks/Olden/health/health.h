/* For copyright information, see olden_v1.0/COPYRIGHT */

/****************************************************************
 * health.h: header file for health.c                           *
 ****************************************************************/


#ifndef _HEALTH
#define _HEALTH

#include <stdio.h>
#include <stdlib.h>

#define chatting printf

#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876

extern int  max_level;
extern long max_time;
extern long long seed;

struct Results {
  float                   total_patients;
  float                   total_time;
  float                   total_hosps; 
};

struct Patient {
  int                    hosps_visited;
  int                    time;
  int                    time_left;
  struct Village         *home_village;
};

struct List {
  struct List            *forward;
  struct Patient         *patient;
  struct List            *back;
};

struct Hosp {
#if 1
  int                    personnel; 
  int                    free_personnel; 
  int                    num_waiting_patients; 
  struct List            waiting; 
  struct List            assess; 
  struct List            inside;
  struct List            up;
#else
  int                    free_personnel; 
  struct List            waiting;
  struct List            assess;
  struct List            inside;
  struct List            up;
  int                    personnel; 
  int                    num_waiting_patients; 
#endif
};
 
struct Village {
#if 1
  struct Village         *forward[4];
  struct Village         *back;
  struct List            returned;
  struct Hosp            hosp;   
  int                    label;
  long long              seed;
#else
  struct Hosp            hosp;   
  long                   seed;
  struct Village         *forward[4];
  int                    label;
  struct List            returned;
  struct Village         *back;
#endif
};

struct Village *alloc_tree(int level, int label, struct Village *back);
void dealwithargs(int argc, char *argv[]);
float my_rand(long long idum);
struct Patient *generate_patient(struct Village *village);
void put_in_hosp(struct Hosp *hosp, struct Patient *patient);
void addList(struct List *list, struct Patient *patient);
void removeList(struct List *list, struct Patient *patient);
struct List *sim(struct Village *village);
void check_patients_inside(struct Village *village, struct List *list);
struct List *check_patients_assess(struct Village *village, struct List *list);
void check_patients_waiting(struct Village *village, struct List *list);
float get_num_people(struct Village *village);
float get_total_time(struct Village *village);
float get_total_hosps(struct Village *village);
struct Results get_results(struct Village *village);

#endif

