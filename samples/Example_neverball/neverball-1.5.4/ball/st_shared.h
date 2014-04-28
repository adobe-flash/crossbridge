#ifndef ST_SHARED_H
#define ST_SHARED_H

void shared_leave(int id);
void shared_paint(int id, float st);
void shared_timer(int id, float dt);
int  shared_point_basic(int id, int x, int y);
void shared_point(int id, int x, int y, int dx, int dy);
int  shared_stick_basic(int id, int a, int v);
void shared_stick(int id, int a, int v);
void shared_angle(int id, int x, int z);
int  shared_click(int b, int d);


#endif /* ST_SHARED_H */
