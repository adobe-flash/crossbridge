#ifndef HUD_H
#define HUD_H

/*---------------------------------------------------------------------------*/

void hud_init(void);
void hud_free(void);

void hud_paint(void);
void hud_timer(float);
void hud_update(int);

void hud_view_pulse(int);
void hud_view_timer(float);
void hud_view_paint();

/*---------------------------------------------------------------------------*/

#endif
