#ifndef CHARSTREAM_H
#define CHARSTREAM_H

#define CHARSTREAM_STARTSIZE 16
#define CHARSTREAM_INIT {NULL,0,0}

/* typedefs */
typedef struct charsequence {
  char *buf;
  size_t size,pos;
} charsequence;

/* prototypes */
void  charsequence_reset(charsequence *);
void  charsequence_push(charsequence *,char);
char  charsequence_pop(charsequence *);
char *charsequence_val(charsequence *);

#endif
