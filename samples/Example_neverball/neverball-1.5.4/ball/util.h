#ifndef UTIL_H
#define UTIL_H

#include "set.h"

/*---------------------------------------------------------------------------*/

#define GUI_BIT      (1 << 24)

#define GUI_MSK(i)   ((i) | GUI_BIT)
#define GUI_UNMSK(i) ((i) & ~GUI_BIT)
#define GUI_ISMSK(i) ((i) & GUI_BIT ? 1 : 0)

#define GUI_NULL GUI_MSK(0)
#define GUI_BACK GUI_MSK(1)
#define GUI_PREV GUI_MSK(2)
#define GUI_NEXT GUI_MSK(3)
#define GUI_BS   GUI_MSK(4)
#define GUI_CL   GUI_MSK(5)
#define GUI_NAME GUI_MSK(6)

#define GUI_MOST_COINS  GUI_MSK(8)
#define GUI_BEST_TIMES  GUI_MSK(16)
#define GUI_FAST_UNLOCK GUI_MSK(32)

void gui_score_set(int);
int  gui_score_get(void);
int  gui_score_next(int);

void gui_score_board(int, unsigned int, int, int);
void set_score_board(const struct score *, int,
                     const struct score *, int,
                     const struct score *, int);

void gui_keyboard(int);
void gui_keyboard_lock(void);
char gui_keyboard_char(char);

int  gui_navig(int, int, int);
int  gui_maybe(int, const char *, int, int, int);

/*---------------------------------------------------------------------------*/

#endif
