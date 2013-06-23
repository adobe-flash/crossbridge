/* this program is taken from Section 6.4
   in Bjarne Stroustrup's book "The C++ Programming Language" 2nd Edition. */
#include <iostream>
#include "bjarne.h"
using namespace std;

enum color {black='*', white=' '};

char screen[XMAX][YMAX];

void screen_init() {
    for (int y=0; y<YMAX; y++)
	for (int x=0; x<XMAX; x++)
	    screen[x][y]=white;
}

void screen_destroy() {}

inline int on_screen(int a, int b) { // clipping
    return 0<=a && a<XMAX && 0<=b && b<YMAX;
}

void put_point(int a, int b) {
    if (on_screen(a,b)) screen[a][b] = black;
}

void put_line(int x0, int y0, int x1, int y1) {
    int dx=1;
    int a = x1-x0;
    if (a < 0) dx = -1, a = -a;

    int dy=1;
    int b = y1-y0;
    if (b < 0) dy = -1, b = -b;

    int two_a = 2*a;
    int two_b = 2*b;
    int xcrit = -b + two_a;
    int eps = 0;
    for (;;) {
	put_point(x0,y0);
	if (x0==x1 && y0==y1) break;
	if (eps <= xcrit) x0 += dx, eps += two_b;
	if (eps>=a && a<=b) y0 += dy, eps -= two_a;
    }
}

void screen_clear() { screen_init(); }

void screen_refresh() {
    for (int y=YMAX-1; 0<=y; y--) {  // top to bottom
	for (int x=0; x<XMAX; x++)   // left to right
	    cout << screen[x][y];
	cout << '\n';
    }
}

struct  shape {
    static shape *list;
    shape *next;

    shape() { next = list; list = this; }

    virtual point *north() const = 0;
    virtual point *south() const = 0;
    virtual point *east() const = 0;
    virtual point *west() const = 0;
    virtual point *neast() const = 0;
    virtual point *seast() const = 0;
    virtual point *nwest() const = 0;
    virtual point *swest() const = 0;

    virtual void draw() = 0;
    virtual void move(int, int) = 0;
};

class line : public shape {
    point *w,*e;
public:
    point *north() const
	{  point *ret = new point((w->x+e->x)/2,e->y<w->y?w->y:e->y);
	   return ret;
	}
    point *south() const
        {  point *ret = new point((w->x+e->x)/2,e->y<w->y?e->y:w->y);
           return ret;
        }
    point *east() const {}
    point *west() const {}
    point *neast() const {}
    point *seast() const {}
    point *nwest() const {}
    point *swest() const {}

    void move (int a, int b) {
	w->x += a; w->y += b; e->x += a; e->y += b; }
    void draw () { put_line(w,e); }

    line(point *a, point *b) { w = a; e = b; }
    line(point *a, int l) { w = new point(a->x+l-1,a->y);e = a; }
};

class rectangle : public shape {
public:
    point *sw,*ne;
    point *north() const
        {  point *ret = new point((sw->x+ne->x)/2,ne->y);
           return ret;
        }
    point *south() const
        {  point *ret = new point((sw->x+ne->x)/2,sw->y);
           return ret;
        }
    point *east() const {}
    point *west() const {}
    point *neast() const {return ne;}
    point *seast() const {}
    point *nwest() const {}
    point *swest() const {return sw;}
   
    void move (int a, int b) {
        sw->x += a; sw->y += b; ne->x += a; ne->y += b; }
    void draw ();

    rectangle(point *, point *);
};

rectangle::rectangle(point *a, point *b) {
    if (a->x <= b->x) {
	if (a->y <= b->y) {
	    sw = a;
	    ne = b;
	}
	else {
	    sw = new point(a->x,b->y);
	    ne = new point(b->x,a->y);
	}
    }
    else {
	if (a->y <= b->y) {
	    sw = new point(b->x,a->y);
	    ne = new point(a->x,b->y);
	}
	else {
	    sw = b;
	    ne = a;
	}
    }
}

void rectangle::draw() {
    point nw(sw->x,ne->y);
    point se(ne->x,sw->y);
    put_line(&nw,ne);
    put_line(ne,&se);
    put_line(&se,sw);
    put_line(sw,&nw);
}

shape* shape::list = 0;

void shape_refresh() {
    screen_clear();
    for (shape *p = shape::list; p; p=p->next) p->draw();
    screen_refresh();
}

void stack(shape *p, const shape *q) { // put p on top of q
    point *n = q->north();
    point *s = p->south();
    p->move(n->x-s->x,n->y-s->y+1);
}

class myshape : public rectangle {
    line *l_eye;
    line *r_eye;
    line *mouth;
public:
    myshape(point *a, point *b) : rectangle(a,b) {
    int ll = neast()->x-swest()->x+1;
    int hh = neast()->y-swest()->y+1;
    point *l_eye_point = new point(swest()->x+2,swest()->y+hh*3/4);
    point *r_eye_point = new point(swest()->x+ll-4,swest()->y+hh*3/4);
    point *mouth_point = new point(swest()->x+2,swest()->y+hh/4);
    l_eye = new line(l_eye_point,2);
    r_eye = new line(r_eye_point,2);
    mouth = new line(mouth_point,ll-4);
    }

    void draw();
    void move(int, int);
};

void myshape::draw() {
    rectangle::draw();
    int a = (swest()->x+neast()->x)/2;
    int b = (swest()->y+neast()->y)/2;
    put_point(new point(a,b));
}

void myshape::move(int a, int b) {
    rectangle::move(a,b);
    l_eye->move(a,b);
    r_eye->move(a,b);
    mouth->move(a,b);
}

int main() {
    screen_init();
    point *point00 = new point(0,0);
    point *point1010 = new point(10,10);
    point *point015 = new point(0,15);
    point *point1510 = new point(15,10);
    point *point2718 = new point(27,18);
    shape *p1 = new rectangle(point00,point1010);
    shape *p2 = new line(point015,17);
    shape *p3 = new myshape(point1510,point2718);
    shape_refresh();
    p3->move(-10,-10);
    stack(p2,p3);
    stack(p1,p2);
    shape_refresh();
    screen_destroy();
    return 0;
}
