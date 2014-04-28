#ifndef COURSE_H
#define COURSE_H

/*---------------------------------------------------------------------------*/

#define COURSE_FILE "courses.txt"
#define MAXCRS 16

void course_init();
void course_free();

int  course_exists(int);
int  course_count(void);
void course_goto(int);
int  course_curr(void);
void course_rand(void);

const char *course_name(int);
const char *course_desc(int);
const char *course_shot(int);

/*---------------------------------------------------------------------------*/

#endif
