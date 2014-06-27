//********************************************************
// This file part of my Obsequi front end.
//********************************************************
//
// Copyright (C) 2001 Nathan Bullock
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//********************************************************
//  Email address - bullock@cs.ualberta.ca
//  Web address   - www.cs.ualberta.ca/~bullock
//********************************************************

#include "utils.h"

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctype.h>

#include "cppflags.h"
#include "interface.h"

// LLVM: define our own getline for portability
#define getline getline_llvm

// Crude approximation of gnu getline
ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
  char buffer[4096];
  bzero(buffer,4096);
  ssize_t result = 0;
  if (NULL == fgets(buffer, 4095, stream))
    result = -1;
  else
    result = strlen(buffer);
  if (result >= 0) {
    if (*lineptr == 0 )
      *lineptr = malloc(result+1);
    else
      *lineptr = realloc(*lineptr, result+1);
    memcpy(*lineptr, buffer, result+1);
  }
  return result;
}

//########################################################
// Function templates.
//########################################################

// handle command line options.
static void   decode_switches     (int argc, char **argv);

// signal handlers.
static void   sig_int_setup       ();
static void   sig_int_handler     (int sig);
static void   sig_alrm_setup      ();
static void   sig_alrm_handler    (int sig);
static void   sig_block           ();
static void   set_stop_alrm       (long minutes);

// main command loop functions.
static void   get_solve_command   ();

// lock file stuff.
static void   get_solve_command_from_lock_file();
static void   write_to_lock_file(char winner, const char* num_nodes);


//########################################################
// Global variables.
//########################################################

// Determines if we will have an alarm checking the input every so often.
static s32bit main_batch = 1;

// Determines if we should be reading a lock file and what it's name is.
static char  *lock_file = NULL;

// Determines if we should be stopping program after x minutes.
static long   stop_minutes = 0;

static char   main_whos_turn;


//########################################################
// Entry point to obsequi.
//########################################################
int
main(int argc, char** argv)
{
  decode_switches(argc, argv);

  if(lock_file){
    get_solve_command_from_lock_file();
    if(stop_minutes) set_stop_alrm(stop_minutes);
  } else {
    get_solve_command();
    //if(!main_batch) sig_alrm_setup();
  }
  
  sig_int_setup();
  
  // start solver.
  {
    s32bit row, col;
    u64bit nodes;
    s32bit score;
    const char*  num_nodes;
    
    score = search_for_move(main_whos_turn, &row, &col, &nodes);
    
    sig_block();
    
    num_nodes = u64bit_to_string(nodes);
    
    // print results.
    if(score >= 5000){
      printf("winner %c, move (%d,%d), nodes %s.\n",
             main_whos_turn, row, col, num_nodes);
    } else if(score <= 5000){
      printf("winner %c, move (%d,%d), nodes %s.\n",
             (main_whos_turn == 'V') ? 'H' : 'V',
             row, col, num_nodes);
    } else {
      printf("Undecided, Best score: %d, nodes %s.\n",
             score, num_nodes);
    }
    
    fflush(stdout);
    
    if(lock_file){
      char winner = 0;
      
      if(score >= 5000) winner = main_whos_turn;
      else if(score <= -5000) winner = (main_whos_turn == 'V') ? 'H' : 'V';
      else fatal_error(1, "Undecided.\n");
      
      write_to_lock_file(winner, num_nodes);
    }
  }
  
  // if not in batch mode don't exit until stdin has been closed.
  fcntl(STDIN_FILENO, F_SETFL, 0);
  if (!main_batch){
    while(getchar() != EOF);
  }
  
  return 0;
}


//########################################################
// Main input - output commands.
//########################################################

//========================================================
// This function reads stdin one line at a time.
// It initializes the solver engine when it finds a line of the format:
//
//  "solve rows m cols n bx0,y0;x1,y1;...;x,y f"
//    where:
//     - m is the number of rows.
//     - n is the number of cols.
//     - bx0,y0 is the position of a block on the board.
//     - f is H if it is horizontals turn and V if verticals turn.
//
// It discards all input which does not match this format and
//   writes an error message to stderr.
//========================================================
static void
get_solve_command()
{
  char*   line      = NULL, *blocks = NULL;
  size_t  line_size = 0, len;

  // Board info for the board which we are solving.
  u32bit num_rows;
  u32bit num_cols;
  s32bit board[30][30];
  char   c1, c2;

  s32bit r, c;
  while( (len = getline(&line, &line_size, stdin)) != -1){
    s32bit t;
    
    if(len > 0){
      if(blocks != NULL) blocks = realloc(blocks, line_size);
      else               blocks = malloc(line_size);
    } else continue;
    line[len - 1] = 0;
    
    t = sscanf(line, "solve rows %u cols %u %c%s %c",
               &num_rows, &num_cols, &c1, blocks, &c2);
    
    if(t != 3 && t != 5){
      fprintf(stderr, "Invalid command: '%s'.\n", line);
      continue;
    }
          
    if(num_rows > 30){
      fprintf(stderr, "Too many rows: %u > 30.\n", num_rows);
      continue;
    }
    
    if(num_cols > 30){
      fprintf(stderr, "Too many cols: %u > 30.\n", num_cols);
      continue;
    }
      
    if(num_cols * num_rows > 256){
      fprintf(stderr,
              "Search space too large: %u > 256.\n", num_cols*num_rows);
      continue;
    }

    for(r = 0; r < 30; r++)
      for(c = 0; c < 30; c++) board[r][c] = 0;
    
    if(t == 5) {
      char *tok;
      
      c1 = toupper(c1);
      if(c1 != 'B') {
        fprintf(stderr, "Invalid command: '%s'.\n", line);
        continue;
      }

      tok = strtok(blocks, ";");
      while(tok != NULL){
        if(sscanf(tok, "%u,%u", &r, &c) != 2){
          fprintf(stderr, "Invalid block string: '%s'.\n", tok);
          c1 = 0;
          break;
        }
        if(r >= num_rows || c >= num_cols){
          fprintf(stderr, "Invalid block string: '%s'.\n", tok);
          c1 = 0;
          break;
        }

        board[r][c] = 1;
        
        tok = strtok(NULL, ";");
      }
      if(c1 == 0) continue;
    } else {
      c2 = c1;
    }
    
    c2 = toupper(c2);
    if(c2 != 'V' && c2 != 'H'){
      fprintf(stderr, "Invalid players turn: %c.\n", c2);
      continue;
    }
    
    // everything should be initialized properly at this point.
    printf("Starting\n");
    fflush(stdout);
    break;
  }

  if(len == -1) fatal_error(1, "No valid command given.\n");

  /*
  main_num_rows  = num_rows;
  main_num_cols  = num_cols;

  for(r = 0; r < 30; r++)
    for(c = 0; c < 30; c++)
      main_board[r][c] = board[r][c];
  */
  main_whos_turn = c2;

  initialize_board(num_rows, num_cols, board);
  
  //print_external(); exit(0);

  free(line);
  free(blocks);
}


//========================================================
// Non blocking input loop.
//========================================================
static void
get_other_commands()
{
  static size_t  line_size;
  static char   *line;
  char c1;
  
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
  
  // get next line of input, exit at end of input.
  while (getline(&line, &line_size, stdin) > 0){

    printf(line);
    
    // stop the search.
    if(sscanf(line, "stop%c", &c1) == 1){
      exit(0);
    }

    /*
    // print info (whatever that is).
    else if(sscanf(line, "info%c", &c1) == 1){
      stringify_stack(&line, &line_size);
      fprintf(stderr, "%s", line);
    }
    */

    else {
      fprintf(stderr, "Invalid command.\n");
    }
  }

  fflush(stderr);
  clearerr(stdin);
}


//########################################################
// Obsequi takes a number of arguments
//  -e print flags used at compile time.
//  -h print out help message and exit.
//  -l specify file which contains a position to solve.
//  -t specify maximum time limit for solving position.
//        only works when using a "lock file" -l.
//  -v print version information.
//  -w wait for standard in to be closed before exiting.
//########################################################
static void
decode_switches(int argc, char **argv)
{
  int c;
  
  while ((c = getopt(argc, argv, "wehl:t:v")) != -1){
    switch(c){
    case 'e':
      printf(option_string);
      exit(0);
      
    case 'h':
      printf("No help available in this version.\n");
      exit(0);
      
    case 'l':
      lock_file = strdup(optarg);
      break;
      
    case 't':
      stop_minutes = atoi(optarg);
      break;
      
    case 'v':
      printf("No version info available.\n");
      exit(0);
      
    case 'w':
      main_batch = 0;
      break;
      
    default:
      fatal_error(1, "Invalid option: '-%c'.\n", c);
    }
  }

  if(optind < argc)
    fatal_error(1, "Extra unknown options on command line.\n");
}



//########################################################
// Signal handling stuff.
//########################################################

//========================================================
// sig int stuff
//========================================================
static void
sig_int_handler(int sig)
{
  const char* str;

  str = current_search_state();

  fprintf(stderr, "%s\n", str);

  fflush(stderr);
}

static void
sig_int_setup()
{
  struct sigaction   act;
  
  //  sigemptyset(&act.sa_mask);
  sigfillset(&act.sa_mask);
  act.sa_flags = 0;

  act.sa_handler = sig_int_handler;
  if ( sigaction(SIGINT, &act, NULL) )
    fatal_error(1, "`sigaction' failed.");
}


//========================================================
// sig alarm stuff
//========================================================
static void
sig_alrm_handler(int sig)
{
  get_other_commands();
}

static void
stop_alrm_handler(int sig)
{
  exit(0);
}

static void
set_stop_alrm(long minutes)
{
  struct sigaction   act;
  struct itimerval itime;

  //  sigemptyset(&act.sa_mask);
  sigfillset(&act.sa_mask);
  act.sa_flags = 0;

  act.sa_handler = stop_alrm_handler;
  if ( sigaction(SIGVTALRM, &act, NULL) )
    fatal_error(1, "`sigaction' failed.");

  itime.it_interval.tv_sec  = minutes * 60;
  itime.it_interval.tv_usec = 0;
  itime.it_value.tv_sec  = minutes * 60;
  itime.it_value.tv_usec = 0;
  
  if ( setitimer(ITIMER_VIRTUAL, &itime, NULL) )
    fatal_error(1, "`setitimer' failed.\n");
}

static void
sig_alrm_setup()
{
  struct sigaction   act;
  struct itimerval   itime;

  //  sigemptyset(&act.sa_mask);
  sigfillset(&act.sa_mask);
  act.sa_flags = 0;

  act.sa_handler = sig_alrm_handler;
  if ( sigaction(SIGALRM, &act, NULL) )
    fatal_error(1, "`sigaction' failed.");

  itime.it_interval.tv_sec  = 2;
  itime.it_interval.tv_usec = 0;
  itime.it_value.tv_sec  = 1;
  itime.it_value.tv_usec = 0;
  
  if ( setitimer(ITIMER_REAL, &itime, NULL) )
    fatal_error(1, "`setitimer' failed.\n");
}


//========================================================
// block all new signals stuff
//========================================================
static void
sig_block()
{
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGALRM);
  sigaddset(&set, SIGINT);

  sigprocmask(SIG_BLOCK, &set, NULL);
}


//########################################################
// Lock and read lock file.
//########################################################

//========================================================
// Some local variables for the lock file stuff.
//========================================================
static s32bit lock_file_offset;

static s32bit
next_valid_pos(const char *line, s32bit line_pos)
{
  s32bit i, state = 0;
  for(i = line_pos; line[i] != 0; i++){
    if(line[i] == ' ') {
      if(state != 0) state = 2;
    } else {
      if(state == 2) break;
      state = 1;
    }
  }
  return i;
}


static void
write_to_lock_file(char winner, const char* num_nodes)
{
  FILE* file = NULL;
  
  if( (file = fopen(lock_file, "r+")) == NULL )
    fatal_error(1, "Can't open file.\n");
  
  //  if(lockf(fileno(file), F_LOCK, 0) != 0)
  //  fatal_error(1, "Lock failed.\n");

  printf("%s\n", lock_file);

  fseek(file, lock_file_offset, SEEK_SET);
  fprintf(file, "%c %15s", winner, num_nodes);
  fflush(file);
  
  //  lockf(fileno(file), F_ULOCK, 0);
  fclose(file);
}


static void
get_solve_command_from_lock_file()
{
  FILE* file = NULL;
  
  char*   line      = NULL;
  size_t  line_size = 0, len, line_pos;

  // Board info for the board which we are solving.
  u32bit num_rows;
  u32bit num_cols;
  s32bit board[30][30];
  char player = 0;

  s32bit r, c;
  
  if( (file = fopen(lock_file, "r+")) == NULL )
    fatal_error(1, "Can't open file.\n");
  
  // if(lockf(fileno(file), F_LOCK, 0) != 0)
  //   fatal_error(1, "Lock failed.\n");

  printf("%s\n", lock_file);
  
  while( lock_file_offset = ftell(file),
         (len = getline(&line, &line_size, file)) != -1){
    // if line has a length of 0 line[0] will be '\0'.
    if(line[0] != 'A') continue;

    printf("%s", line);
    
    if(len <= 18) fatal_error(1, line);
    
    if(sscanf(line + 18, "(%d,%d)", &num_rows, &num_cols) != 2)
      fatal_error(1, "Invalid row and columns.\n%s\n", line);
    
    if(num_rows > 30)
      fatal_error(1, "Too many rows: %u > 30.\n", num_rows);
    if(num_cols > 30)
      fatal_error(1, "Too many cols: %u > 30.\n", num_cols);
    if(num_cols * num_rows > 128)
      fatal_error(1, "Search space too large: %u > 256.\n", num_cols*num_rows);
    
    for(r = 0; r < 30; r++)
      for(c = 0; c < 30; c++) board[r][c] = 0;
    
    line_pos = 18;
    
    while(1){
      s32bit ignore;
      
      line_pos = next_valid_pos(line, line_pos);
      
      if(line_pos >= len) break;
      
      if(sscanf(line + line_pos, ":%c:%d(%d,%d)", &player,
                &ignore, &r, &c) != 4)
        break;
      
      printf("%c %d %d\n", player, r, c);
      
      if(player == 'V'){
        if(board[c][r] == 1 ||  board[c+1][r] == 1)
          fatal_error(1, line);
        board[c][r] = board[c+1][r] = 1;
      } else if(player == 'H'){
        if(board[r][c] == 1 || board[r][c+1] == 1)
          fatal_error(1, line);
        board[r][c] = board[r][c+1] = 1;
      } else fatal_error(1, "Invalid player.\n");
    }
    
    // everything should be initialized properly at this point.
    printf("Starting\n");
    fflush(stdout);
    break;
  }

  if(len == -1) fatal_error(1, "No valid command given.\n");
  
  fseek(file, lock_file_offset, SEEK_SET);
  fprintf(file, "W");
  fflush(file);
  
  if(player == 'V') main_whos_turn = 'H';
  else if(player == 'H') main_whos_turn = 'V';
  else  fatal_error(1, "Invalid player.\n");

  initialize_board(num_rows, num_cols, board);
  
  free(line);

  //  lockf(fileno(file), F_ULOCK, 0);
  fclose(file);
}
