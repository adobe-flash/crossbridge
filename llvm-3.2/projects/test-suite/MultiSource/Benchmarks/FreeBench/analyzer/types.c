#include <stdio.h>
#include <stdlib.h>
#include "types.h"
 
extern conf_list_t *list;
extern int first;

extern def_list_t **def_table;
extern int def_table_size;

extern hard_raw_t *hard_raw_list;

void hard_raw_mod(uint32 address, int w_epoch, int w_place, uint32 w_issue, int r_epoch, int r_place, uint32 r_issue)
{
  static hard_raw_t *last;
  static int hr_empty=1;
  
  if (hr_empty) { 
    hard_raw_list=(hard_raw_t *)malloc(sizeof(hard_raw_t));
    if (!hard_raw_list) {
      fprintf(stderr,"ALERT: \tOut of memory, aborting...\n");
      exit(1);
    }
    hard_raw_list->next=NULL;
    hard_raw_list->prev=NULL;
    hard_raw_list->address=address;
    hard_raw_list->w_epoch=w_epoch;
    hard_raw_list->w_place=w_place;
    hard_raw_list->w_issue=w_issue;
    hard_raw_list->r_epoch=r_epoch;
    hard_raw_list->r_place=r_place;
    hard_raw_list->r_issue=r_issue;
    hr_empty=0;
    last=hard_raw_list;
    return;
  }
  
  last->next=(hard_raw_t *)malloc(sizeof(hard_raw_t));
  last->next->prev=last;
  last=last->next;
  if (!last) {
    fprintf(stderr,"ALERT: \tOut of memory, aborting...\n");
    exit(1);
  }
  last->next=NULL;
  last->address=address;
  last->w_epoch=w_epoch;
  last->w_place=w_place;
  last->w_issue=w_issue;
  last->r_epoch=r_epoch;
  last->r_place=r_place;
  last->r_issue=r_issue;
}

uint32 def_hash (uint32 address)
{
  return address%def_table_size;
}

def_list_t *def_list_lookup(uint32 address)
{
  uint32 place;
  def_list_t *counter;

  place = def_hash(address);

  counter=def_table[place];
  while (counter!=NULL) {
    if (address==counter->address) {
      return counter;
    } 
    counter=(def_list_t *)counter->next;
  }
  return NULL;
}

void def_list_mod(uint32 address, int epoch, int place_in_epoch)
{
  def_list_t *temp=NULL;
  def_list_t *counter;
   
  uint32 place;

  place = def_hash(address);
  
  if (def_table[place]==NULL) {
    temp=(def_list_t *)malloc(sizeof(def_list_t));
    if (!temp) {
      fprintf(stderr,"ALERT: \tOut of memory, aborting...\n");
      exit(1);
    }
    temp->next=NULL;
    temp->address=address;
    temp->epoch=epoch;
    temp->place_in_epoch=place_in_epoch;
    def_table[place]=temp;
  } else {
    for (counter=def_table[place];counter!=NULL;counter=(def_list_t *)counter->next) { 
      temp=counter;
      if (address==counter->address) {
	counter->epoch=epoch;
	counter->place_in_epoch=place_in_epoch;
	return;
      }
    }
    counter=(def_list_t *)malloc(sizeof(def_list_t));
    if (!counter) {
      fprintf(stderr,"ALERT: \tOut of memory, aborting...\n");
      exit(1);
    }
    counter->next=NULL;
    counter->address=address;
    counter->epoch=epoch;
    counter->place_in_epoch=place_in_epoch;
    temp->next=counter;
  }

}

void conflict_list(uint32 address)
{
  static conf_list_t *next=NULL;
  conf_list_t *temp=NULL;
  conf_list_t *counter;
  int make_new=1;
  
  if (first) {
    list=(conf_list_t *)malloc(sizeof(conf_list_t));
    if (!list) {
      fprintf(stderr,"ALERT: \tOut of memory, aborting...\n");
      exit(1);
    }
    list->next=NULL;
    list->address=address;
    list->accesser=1;
    first=0;
    next=(conf_list_t *)list;
    return;
  }
  
  for (counter=list;counter!=NULL;counter=(conf_list_t *)counter->next) {   
    temp=counter;
    if (address==counter->address) {
      counter->accesser++;
      make_new=0;
      break;
    }
  }
  
  if (make_new) {
    next=(conf_list_t *)malloc(sizeof(conf_list_t));
    if (!next) {
      fprintf(stderr,"ALERT: \tOut of memory, aborting...\n");
      exit(1);
    }
    
    next->next=NULL;
    next->address=address;
    next->accesser=1;
    temp->next=next;
  }
}
