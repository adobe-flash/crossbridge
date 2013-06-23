#define ever (;;)
#define true 1
#define false 0
#define bool int
#define l 0
#define r 1

/* vor.c */
void clean_up();
void getpoint(int x,int y);
int set_node();
int do_quit();
void build_menu(void);
void get_file();
void add_point(point p);
point compute_v(CHpoints *P);
void add_infinit_points_to_K(CHpoints *S);
void add_edge(int p1,int p2);
void draw_sec(CHpoints *p);
CHpoints *maximize_radius_and_angle(CHpoints *S);
void alg2();
void construct_vor();

/* ch.c */
bool empty();
bool point_equal(point p1,point p2);
int determinant(point p1, point p2, point p3);
bool visible(int direction,point p1,point p2,point p3);
CHpoints *get_points_on_hull(DCEL_segment *left,DCEL_segment *right);
void add_segments(DCEL_segment *n,DCEL_segment *act,DCEL_segment *first,
		  int direction);
CHpoints *construct_ch();

/* splay.c */
void traverse(splay_node *root);
void free_tree(splay_node *root);
splay_node *init(void);
void *insert(splay_node **root, point p);
point delete_min(splay_node **root);

/* splay2.c */
void CHtraverse(CHsplay_node *root);
void CHfree_tree(CHsplay_node *root);
CHsplay_node *CHinit(void);
void *CHinsert(CHsplay_node **root, CHpoints *p);
CHpoints *CHdelete_max(CHsplay_node **root);
void CHdelete(CHsplay_node **root, key key);

/* pointlist.c */
void point_list_insert(CHpoints **PL, point p);
CHpoints *before(CHpoints *P);
CHpoints *next(CHpoints *P);
double angle(CHpoints *p1, CHpoints *p2, CHpoints *p3);
void point_list_print(CHpoints *PL);
void number_points(CHpoints *PL);
CHpoints *remove_points(CHpoints *PL);

/* intersect.c */
dpoint midpoint(point p1, point p2);
point vector(point p1, point p2);
int length2(point p1, point p2);
double calculate_c(point normalvector,dpoint midpoint);
dpoint intersect(point n1, point n2, double c1, double c2);
dpoint centre(point p1, point p2, point p3);
double radius2(point p,dpoint centre);
