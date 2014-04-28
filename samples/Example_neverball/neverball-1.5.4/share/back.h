#ifndef BACK_H
#define BACK_H

/*---------------------------------------------------------------------------*/

#define BACK_DIST 256.0f
#define FAR_DIST  512.0f

void back_init(const char *, int);
void back_free(void);
void back_draw(float);

/*---------------------------------------------------------------------------*/

#endif
