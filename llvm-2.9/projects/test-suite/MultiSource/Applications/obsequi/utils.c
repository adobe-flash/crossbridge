
#include "utils.h"

#include <stdarg.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdarg.h>
#include <regex.h>


//########################################################
// This function prints a message to both stderr and
//   to the file ".fatal_error".
//
// If ERR_NUM != 0 then exit is called with the value ERR_NUM.
//########################################################
extern void
_fatal_error_aux(const char *file, const s32bit line, const s32bit err_num,
                 const char *format, ... )
{
  va_list ap;
  static FILE* err_file = NULL;
  char *msg_type = (err_num == 0) ? "WARNING: " : "ERROR: ";

  // only need to open this if not already open.
  if(err_file == NULL)  err_file = fopen(".fatal_error", "w");
  if(err_file == NULL) fprintf(stderr, "Couldn't open \".fatal_error\".\n");
  
  if(err_num == 0) {
    // Check size of file, we don't want to end up filling up the whole disk.
    s32bit size = ftell(err_file);
    if(size == -1)
      fatal_error(1, "size == -1.\n");
    
    else if(size > (2<<24) + 2000 ){
      return;
    }
    
    else if(size > (2<<24)){
      fprintf(stderr, "Log file getting too large.\n");
      fprintf(err_file, "Log file getting too large.\n");
    }
  }
  
  // start writing the real message.
  fprintf(stderr, msg_type);
  fprintf(err_file, msg_type);
  
  va_start (ap, format);
  vfprintf (stderr, format, ap);
  vfprintf (err_file, format, ap);
  va_end (ap);
  
  fprintf(stderr, "> File: %s, Line: %d.\n", file, line);
  fprintf(err_file, "> File: %s, Line: %d.\n", file, line);

  fflush(stderr);
  fflush(err_file);
    
  if (err_num != 0)  exit(err_num);
}


//########################################################
// These are dynamic sprintf function, they malloc space as needed.
//########################################################

//========================================================
// This function stores the string created from FORMAT and the
//   following args to the string STR, starting at position OFFSET.
//========================================================
extern s32bit
Asprintf(char **str, s32bit *len, s32bit offset, const char *format, ... )
{
  va_list ap;
  s32bit nchars;
  
  va_start (ap, format);

  // if no space allocated yet, allocate some.
  if(*len == 0){ *len = 128; *str = malloc(*len); }
  
  while (1) {
    // attempt printing in the allocated space
    nchars = vsnprintf (*str + offset, *len - offset, format, ap);
        
    if (nchars < *len - offset) break;
    
    // if not enough space allocate some more.
    *len += 128;
    *str = realloc(*str, *len);
  }
  
  va_end (ap);
  return nchars;
}

//========================================================
// This function stores the string created from FORMAT and the
//   following args to the end of the string STR.
//========================================================
#if 0
extern s32bit
asprintf_my(char **str, s32bit offset, const char *format, ... )
{
  va_list ap;
  s32bit nchars, len = ALLOCED(*str,char);
  
  va_start (ap, format);

  // if no space allocated yet, allocate some.
  if(len == 0){ len = 128; *str = malloc(len); }
  
  while (1) {
    // attempt printing in the allocated space
    nchars = vsnprintf (*str + offset, len - offset, format, ap);
        
    if (nchars < len - offset) break;
    
    // if not enough space allocate some more.
    len += 128;
    *str = realloc(*str, len);
  }
  
  va_end (ap);
  return nchars;
}
#endif


//########################################################
// Return a string with the printed value of a 64 bit number.
//########################################################
extern const char*
u64bit_to_string(u64bit val)
{
  static char   big_num[80];    // plenty large
  s32bit        vals[10];       // plenty large
  s32bit        i = 0, offset;
  
  do {
    vals[i] = val % 1000;
    val = val / 1000;
    i++;
  } while(val != 0);
  
  offset = sprintf(big_num, "%d", vals[--i]);
  
  while(i != 0)
    offset += sprintf(big_num + offset, ",%03d", vals[--i]);
  
  return big_num;
}


//########################################################
// I'm not sure why I have this function.
//  I suppose it may come in useful for performance analysis.
//########################################################
extern void
null_command() {}

