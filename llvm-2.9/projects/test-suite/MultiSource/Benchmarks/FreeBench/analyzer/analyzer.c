#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "version.h"
#include "types.h"
#include "functs.h"
#include "help.h"
#include "parse_settings.h"

#define TOP_LIST_LEN 10
#define EPOCH_LENGTH 1

int list_len=0;
int top_list_len=TOP_LIST_LEN;
int epoch_length=EPOCH_LENGTH;
int def_table_size=10007;

extern int settings[];

/***************************************
 * This program is modified for use in * 
 * benchmarking puposes. The complete  *
 * program was used in computer system *
 * research at Chalmers University of  *
 * Technology, Sweden. Chalmers has    *
 * nothing to do with this program.    *
 *                                     *
 * Plese feel free to distribute this  *
 * program as you like.                *
 *                                     *
 * Peter Rundberg, biff@ce.chalmers.se *
 ***************************************/


conf_list_t *list=NULL;
int first=1;
def_list_t **def_table=NULL;
hard_raw_t *hard_raw_list=NULL;


void init_def_table(int def_size) 
{
  int i;

  def_table=(def_list_t **)malloc(def_size*sizeof(def_list_t *));
 
  if (def_table==NULL) {
    fprintf(stderr,"Out of memory...\n");
    exit(1);
  }

  for (i=0;i<def_size;i++)
    def_table[i]=NULL;
}

void error(char *name)
{
  exit(1);
}

int main(int c, char *v[])
{
  FILE *fp,*graphfile=NULL;
  /* Benchmark stuff */
  char **indata_lines;
  int counter=0;
  uint32 mem_ops=0;
  /* End Benchmark stuff */
  int varv=-1;   /* We want to start counting at 0 */
  int place_in_varv=0;
  int place_in_epoch=0;
  int varv_in_epoch;
  char string[100];
  uint32 address;
  uint32 issue_no;
  int i;
  /* Settings */
  int data=1,name=0,verbose=0,speed=1,forward=1,fastspeed=0,imix=1;
  int load_pen=0,store_pen=0,nofile=1,kernel=3,cpulimit=0,configs=0;
  int show_speedup=3,thread_pen=0,commit_pen=0,use_epoch_length_as_num_of_epochs=0;
  /* End -- settings */
  uint32 data_pen_tot=0;
  epoch_t *epoch=NULL;
  int num_epochs=0,loops=0;
  trans_t *trans;
  load_store_t load_store; 
  char *settingsfile="analyzer.conf"; 

  fprintf(stderr,"Compile date: %s\n", COMPDATE);
  fprintf(stderr,"Compiler switches: %s\n", CFLAGS);

  if (c<2) {
    error(v[0]);
  }
 
  if (!nofile) {  
    parse_settings(settingsfile);
    
    /* Applying the setting determined by the settingsfile */
    load_pen=settings[0];
    store_pen=settings[1];
    data=settings[2];
    name=settings[3];
    speed=settings[4];
    fastspeed=settings[5];
    forward=!settings[6];
    imix=settings[7];
    verbose=!settings[8];
    epoch_length=settings[9];
    kernel=settings[10];
    cpulimit=settings[11];
    configs=settings[12];
    show_speedup=settings[13];
    thread_pen=settings[14];
    commit_pen=settings[15];
    use_epoch_length_as_num_of_epochs=settings[16];
  }
 
  init_def_table(def_table_size);

  fp=fopen(v[c-1],"r");
  if (!fp) {
    fprintf(stderr,"ERROR: \tCould not find file: %s\n\n",v[c-1]);
    error(v[0]);
  }
  
  do {
    fgets(string,100,fp);
  } while (string[0]!='E');
  
  num_epochs = atoi(string+5);
  loops=num_epochs;
  if (loops<epoch_length) {
    fprintf(stderr,"ERROR: Number of loop iterations is less than epoch length: %d<%d\n",num_epochs,epoch_length);
    exit(0); 
  }
  if (use_epoch_length_as_num_of_epochs) {
    epoch_length=num_epochs/epoch_length;
  }    
   
  if (num_epochs%epoch_length)
    num_epochs = num_epochs/epoch_length+1;
  else
    num_epochs = num_epochs/epoch_length;
  

  if (num_epochs==0) {
    fprintf(stderr,"File %s is empty\n",v[c-1]);
    exit(0);    
  }
  printf("Num_epochs: %d\n",num_epochs);
  rewind(fp);

  if (fastspeed)
    speedup_test(fp);
  
  if (imix)
    mem_ops=imix_test(fp);
  
  if (data || name || speed) {
    epoch=(epoch_t *)malloc(num_epochs*sizeof(epoch_t));
    if (!epoch) {
      fprintf(stderr,"ALERT: \tOut of memory, aborting...\n");
      exit(1);
    }
    for (i=0;i<num_epochs;i++) {
      epoch[i].trans=NULL;
      epoch[i].last=NULL;
      epoch[i].first_trans=1;
    }

    /* Read the indata file to memory before processing begins. */
    /* To eliminate effects of a slow disk subsystem. */
    /* We only want to benchmark CPU & Memory performance. */
    {
      char *indata_line;
      
      indata_lines=(char **)malloc((mem_ops+loops+10)*sizeof(char *));
      while(!feof(fp)) {
	indata_line=(char *)malloc(50*sizeof(char));
	fgets(indata_line,50,fp);
	indata_lines[counter++]=indata_line;
      }
    }
    
    counter=0;

    sscanf(indata_lines[counter++],"%s %lu",string,&issue_no);
 
  /* fprintf(stderr,"DEBUG: \tLabel %s at cycle %lu...\n",string,issue_no); */
    epoch[0].start_time=issue_no;
    varv_in_epoch=-1;
    place_in_varv=0;
    while (1) {
      if (!strcmp(string,"START:")) { 
	varv++;
	varv_in_epoch++;
	
	if (varv_in_epoch==epoch_length) {
	  place_in_varv=0;
	  varv_in_epoch=0;

	  sprintf(string,"%lx", address);

	  sscanf(string,"%lu",&issue_no);

	  issue_no+=data_pen_tot;
	  epoch[varv/epoch_length].start_time=issue_no;
	  if (varv/epoch_length)
	    epoch[varv/epoch_length-1].end_time=issue_no;
	}
	while(1) {

	  sscanf(indata_lines[counter++],"%s %lx %lu",string,&address,&issue_no);

	  issue_no+=data_pen_tot; 
	  if (!strcmp(string,"LD:")) {
	    data_pen_tot+=(uint32)load_pen;
	    load_store=load_op;
	    trans=(trans_t *)malloc(sizeof(trans_t));
	    trans->load_store=load_op;
	    trans->address=address;
	    trans->issue_no=issue_no-epoch[varv/epoch_length].start_time;
	    trans->next=NULL;
	    if (epoch[varv/epoch_length].first_trans) { 
	      epoch[varv/epoch_length].trans=trans;
	      epoch[varv/epoch_length].last=trans;
	      epoch[varv/epoch_length].first_trans=0;
	    } else {
	      epoch[varv/epoch_length].last->next=trans;
	      epoch[varv/epoch_length].last=trans;
	    }
	  } else if (!strcmp(string,"ST:")) {
	    data_pen_tot+=(uint32)store_pen;
	    load_store=store_op;
	    trans=(trans_t *)malloc(sizeof(trans_t));
	    trans->load_store=store_op;
	    trans->address=address;
	    trans->issue_no=issue_no-epoch[varv/epoch_length].start_time; 
	    trans->next=NULL;
	    def_list_mod(address,varv/epoch_length,place_in_varv); 
	    if (epoch[varv/epoch_length].first_trans) { 
	      epoch[varv/epoch_length].trans=trans; 
	      epoch[varv/epoch_length].last=trans;
	      epoch[varv/epoch_length].first_trans=0;
	    } else {
	      epoch[varv/epoch_length].last->next=trans; 
	      epoch[varv/epoch_length].last=trans;
	    }
	  } else {
	    break;
	  }
	  /* Place to put processing... */
	  if (data || speed) {
	    /* Search the epochs read so far for RAW conflicts */
	    if (load_store==load_op) {  
	      for (i=0;i<varv/epoch_length;i++) {
		trans=epoch[i].trans;
		place_in_epoch=-1;
		while (trans!=NULL) {   
		  place_in_epoch++;
		  if (trans->load_store==store_op && trans->address==address) {
		    def_list_t *def_placeholder=NULL;
		    def_placeholder=def_list_lookup(address);
		    if (def_placeholder!=NULL) {
		      if ((def_placeholder->epoch==i) && 
			  (def_placeholder->place_in_epoch==place_in_epoch)) {
			conflict_list(address);
			hard_raw_mod(address, i, place_in_epoch, trans->issue_no, varv/epoch_length, place_in_varv, issue_no-epoch[varv/epoch_length].start_time);
		      }
		    }
		  }
		  trans=(trans_t *)trans->next;
		}
	      }
	    } else {
	      
	    }
	  }
	  if (name) {
	    fprintf(stderr,"ALERT: \tName dependecy testing not implemented\n");
	    exit(1);
	  }
	  if (verbose && !(varv%10))
	    fprintf(stderr,"\rProcessing %3.2f %%    ", 100*(float)varv/(float)loops);
	  place_in_varv++;
	}
      } else if (!strcmp(string,"END:")) {
	if (verbose)
	  fprintf(stderr,"\rProcessing 100.00 %%    \n");
	break;
      } else {
	if (verbose)
	  fprintf(stderr,"\n");
	fprintf(stderr,"ERROR: \tWrong format on file %s\n",v[c-1]);
	exit(1);
      }
    }
  }
  
  
  if (data) {
    conf_list_t *conf_iterator=list;
    while (conf_iterator!=NULL) {
      printf("%d RAW:s for 0x%lx\n",conf_iterator->accesser, conf_iterator->address);
      conf_iterator=(conf_list_t *)conf_iterator->next;
    }
  }
  
  if (speed) { 
    epoch[varv/epoch_length].end_time=epoch[varv/epoch_length].start_time+epoch[varv/epoch_length].last->issue_no;
    
    if (forward)
      find_hard_raws();
    
    speedup_test(fp);
    
    if (kernel ==1) {
      specul_time_o(epoch, num_epochs,graphfile,show_speedup,thread_pen,commit_pen);
    } else if (kernel == 2) {
      specul_time_r(epoch, num_epochs, cpulimit,graphfile,show_speedup,thread_pen,commit_pen);
      if (configs) {
	for (i=1;i<=configs;) {
	  specul_time_r(epoch, num_epochs, i,graphfile,show_speedup,thread_pen,commit_pen);
	  i=i*2;
	}
      }
    } else if (kernel == 3) {
      specul_time_o(epoch, num_epochs,graphfile,show_speedup,thread_pen,commit_pen);
      specul_time_r(epoch, num_epochs, cpulimit,graphfile,show_speedup,thread_pen,commit_pen);
      if (configs) {
	for (i=1;i<=configs;) {
	  specul_time_r(epoch, num_epochs, i,graphfile,show_speedup,thread_pen,commit_pen);
	  i=i*2;
	}
      }
    } else {
      fprintf(stderr,"ALERT: \tNo such kernel present\n");
      exit(1);
    }
    
  }


  return 0;
}




