#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse_settings.h"

#define NUM_TOKENS 17
char *pattern[NUM_TOKENS]={"LOAD_PENALTY","STORE_PENALTY",
			   "DATA","NAME",
			   "SPEED","EARLY_SPEED",
			   "NO_FORWARDING","INST_MIX",
			   "QUIET","EPOCH_LENGTH",
			   "KERNEL","CPULIMIT",
			   "CONFIGS","SHOW_SPEEDUP",
			   "THREAD_PEN","COMMIT_PEN",
			   "EPOCH_LENGTH_AS_NUM_EPOCHS"};
int settings[NUM_TOKENS];

int parse_settings(char *filename)
{
  FILE *fp;
  char *line;
  int i=0;

  line=(char *)malloc(100*sizeof(char));  
  for (i=0;i<NUM_TOKENS;i++)  
    settings[i]=-1;

  if ((fp=fopen(filename,"r"))==NULL) {
    fprintf(stderr,"Could not find file %s\n",filename);
    exit(1);
  }
  
  while(!feof(fp)) {
    i=0;
    fgets(line,100,fp);
    if (line[0]=='#') {
      continue;
    }
    strtok(line," ");
    while (i<NUM_TOKENS) {
      if (!strcmp(pattern[i],line)) {
	settings[i]=atoi(&line[strlen(line)+1]);
      }
      i++;
    }
  }

  free(line);

  return 0;
}
