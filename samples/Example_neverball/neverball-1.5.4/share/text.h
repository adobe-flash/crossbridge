#ifndef TEXT_H
#define TEXT_H

#include <SDL.h>

/*---------------------------------------------------------------------------*/

int text_add_char(Uint32, char *, int);
int text_del_char(char *);
int text_length(const char *);

/*---------------------------------------------------------------------------*/

#endif
