#include <stdio.h>

#define p(x) fprintf(stderr,x);

void show_help(char *prog_name)
{
  fprintf(stderr,"Help/Manual for Dependency Analyzer");
  p("\nThis program is written by Peter Rundberg, Chalmers University of Technology\n")
  p("Questions about this software should be directed to biff@ce.chalmers.se\n\n")
  p("To determine the data hazards between loop iterations this program analyzes\n")
  p("trace data from a run of the program examined. The program also analyzes the\n")
  p("potential speedup obtainable from running the loop iterations in parallel.\n\n")  
  p("The program requires a number of settings to be done either on the\n")
  p("command line or in the settings file.\n\n") 
  p("Available flags. Many of these can be set in the settings file.\n");
  p("-c #      : CPU limit. Only effective with 'realistic restart'. 0==unlimited.\n");
  p("-C #      : CPU limit for run through several configurations.\n");
  p("-d        : Check for DATA dependencies (RAW)\n");
  p("-e #      : Epoch length (number of loop iterations per epoch).\n");
  p("-E        : Use epoch length as total number of epochs\n");
  p("-f        : Do NOT use forwarding in speedup analysis\n");
  p("-g #      : Specify Thread start penalty\n");
  p("-G #      : Specify Commit penalty\n");
  p("-h        : Show complete help message\n");
  p("-i #      : Print SpeedUp for 1 - Loop, 2 - Program, 3 - Both.\n");
  p("-k #      : The test kernel to be used in the test.\n");
  p("            1 - Optimum restart. 2 - Realistic restart. 3 - Both\n");
  p("-l #      : Max length of hazard top-list. -- NOT IMPLEMENTED --\n");
  p("-m        : Analyze instruction mix\n");
  p("-n        : Check for NAME dependencies (WAR & WAW) -- NOT IMPLEMENTED --\n");
  p("-o <FILE> : Output graphing data to FILE\n");
  p("-p #      : Specify load penalty\n");
  p("-P #      : Specify store penalty\n");
  p("-q        : Quiet mode (do not print processing status)\n");
  p("-s        : Do speedup analysis\n");
  p("-S        : Do speedup analysis, start early\n");
  p("-t #      : Specify the size of the def_hash_table.\n");
  p("-x <FILE> : Use other settings file. DEFAULT: analyzer.conf\n");
  p("-X        : Do not read settings from a file\n");
  p("\n");
}
