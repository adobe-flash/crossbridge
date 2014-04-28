#ifndef BASE_IMAGE_H
#define BASE_IMAGE_H

/*---------------------------------------------------------------------------*/

void  image_size(int *, int *, int, int);

void *image_load(const char *, int *, int *, int *);

void *image_next2(const void *, int, int, int, int *, int *);
void *image_scale(const void *, int, int, int, int *, int *, int);
void  image_white(      void *, int, int, int);
void *image_flip (const void *, int, int, int, int, int);

/*---------------------------------------------------------------------------*/

#endif
