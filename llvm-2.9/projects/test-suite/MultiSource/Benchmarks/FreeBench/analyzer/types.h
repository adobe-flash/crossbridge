#ifndef __TYPES_H
#define __TYPES_H

typedef unsigned long uint32;  /* At least 32 bits wide */
#if (0x40000000lu << 2) == 0            /* If long is just 32 bits wide */
typedef unsigned long long uint64;
#else
typedef unsigned long uint64;
#endif

typedef enum {
  load_op = 1,
  store_op = 2
} load_store_t;

typedef struct hard_raw {
  struct hard_raw *next;
  struct hard_raw *prev;
  uint32 address;
  int w_epoch;
  int w_place;
  uint32 w_issue; 
  int r_epoch;
  int r_place;
  uint32 r_issue; 
} hard_raw_t;

typedef struct {
  load_store_t load_store;
  uint32 address;
  uint32 issue_no;
  void *next;
} trans_t;

typedef struct {
  void *next;
  uint32 address;
  int epoch;
  int place_in_epoch;
} def_list_t;

typedef struct {
  trans_t *trans; 
  trans_t *last;
  int first_trans;
  uint32 start_time;
  uint32 end_time;
  uint32 run_time;
  uint32 stall_time;
} epoch_t;

typedef struct {
  void *next;
  uint32 address;
  int accesser;
} conf_list_t;

void conflict_list(uint32 address);
def_list_t * def_list_lookup(uint32 address);
void def_list_mod(uint32 address, int epoch, int place_in_epoch);
void hard_raw_mod(uint32 address, int w_epoch, int w_place, uint32 w_issue, int r_epoch, int r_place, uint32 r_issue);

#endif /* __TYPES_H */
