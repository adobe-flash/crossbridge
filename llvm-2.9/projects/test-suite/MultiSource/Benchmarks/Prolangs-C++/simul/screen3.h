static int object_count;

class screen_controller : public cursor_controller2 {
  int rowmax, colmax;
public:
  screen_controller(int rows=25, int cols=80);
  void setrows(int rows);
  void setcols(int cols);
  int maxrow() { return rowmax;}
  int maxcol() { return colmax;}
  ~screen_controller();
  void upper_left();
  void lower_left();
  void upper_right();
  void lower_right();
  void draw_vertical(int row, int col, int length, char lc);
  void draw_horizontal(int row, int col, int length, char lc);
  void center(int row, char *text);
  void move(int row, int col);
  void pause(int seconds=1);
  void drawbox(int hor, int ver, int ul, int ur, int ll, int lr);
};

extern screen_controller screen;
