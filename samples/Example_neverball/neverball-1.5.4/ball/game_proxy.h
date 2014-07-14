#ifndef GAME_PROXY_H
#define GAME_PROXY_H

#include "cmd.h"

void       game_proxy_enq(const union cmd *);
union cmd *game_proxy_deq(void);
void       game_proxy_clr(void);

#endif
