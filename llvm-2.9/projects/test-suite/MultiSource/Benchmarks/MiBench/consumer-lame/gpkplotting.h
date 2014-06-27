#include <gtk/gtk.h>

/* allocate a graphing widget */
GtkWidget *gpk_plot_new(int width,int height);

/* graph a function in the graphing widged */
void gpk_graph_draw(GtkWidget *widget,
		   int n, gdouble *xcord, gdouble *ycord,
		   gdouble xmn, gdouble ymn,gdouble xmx,gdouble ymx,
		   int clear, char * title,GdkColor *color);

/* draw a rectangle in the graphing widget */
void gpk_rectangle_draw(GtkWidget *widget,                  /* plot on this widged */
			gdouble xcord[2], gdouble ycord[2], /* corners */
			gdouble xmn,gdouble ymn,            /* coordinates of corners */
			gdouble xmx,gdouble ymx,
			GdkColor *color);                         /* color to use */

/* make a bar graph in the graphing widged */
void gpk_bargraph_draw(GtkWidget *widget,
		   int n, gdouble *xcord, gdouble *ycord,
		   gdouble xmn, gdouble ymn,gdouble xmx,gdouble ymx,
		   int clear, char * title, int bwidth,GdkColor *color);

/* set forground color  */
void setcolor(GtkWidget *widget, GdkColor *color,int red,int green,int blue);

