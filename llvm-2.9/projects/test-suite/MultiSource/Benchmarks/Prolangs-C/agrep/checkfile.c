/*
 *  checkfile.c
 *    takes a file descriptor and checks to see if a file is a regular
 *    ascii file
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "checkfile.h"

#define MAXLINE 512

extern char Progname[];
extern int errno;

unsigned char ibuf[MAXLINE];

/**************************************************************************
*
*    check_file
*       input:  filename or path (null-terminated character string)
*       returns: int (0 if file is a regular file, non-0 if not)
*
*    uses stat(2) to see if a file is a regular file.
*
***************************************************************************/

int check_file(char *fname)
{
struct stat buf;


  if (stat(fname, &buf) != 0) {
    if (errno == ENOENT)
      return NOSUCHFILE;
    else
      return STATFAILED;  
    } else {
/*
      if (S_ISREG(buf.st_mode)) {
        if ((ftype = samplefile(fname)) == ISASCIIFILE) {
          return ISASCIIFILE;
        } else if (ftype == ISBINARYFILE) {
          return ISBINARYFILE;
        } else if (ftype == OPENFAILED) {
          return OPENFAILED;
        }
      }
      if (S_ISDIR(buf.st_mode)) {
        return ISDIRECTORY;
      }
      if (S_ISBLK(buf.st_mode)) {
        return ISBLOCKFILE;
      }
      if (S_ISSOCK(buf.st_mode)) {
        return ISSOCKET;
      }
*/
    }
  return 0;
}

/***************************************************************************
*
*  samplefile
*    reads in the first part of a file, and checks to see that it is
*    all ascii.
*
***************************************************************************/
/*
int samplefile(char *fname)
{
char *p;
int numread;
int fd;

  if ((fd = open(fname, O_RDONLY)) == -1) {
    fprintf(stderr, "open failed on filename %s\n", fname);
    return OPENFAILED;
  }
  if (numread = read(fd, ibuf, MAXLINE)) {
   close(fd);
   p = ibuf;
    while (isascii(*p++) && --numread);
    if (!numread) {
      return(ISASCIIFILE);
    } else {
      return(ISBINARYFILE);
    }
  } else {
    close(fd);
    return(ISASCIIFILE);
  }
}
*/
