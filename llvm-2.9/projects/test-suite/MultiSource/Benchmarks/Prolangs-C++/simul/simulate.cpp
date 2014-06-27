#include "screen1.h"

// begin screen1.cpp implementation

void printf(char *s, int i, int j) {}

void cursor_controller::up(int rows) {
  printf("%dA", rows, 0);
}

void cursor_controller::down(int rows) {
  printf("%dB", rows, 0);
}

void cursor_controller::right(int cols) {
  printf("%dC", cols, 0);
}

void cursor_controller::left(int cols) {
  printf("%dD", cols, 0);
}

void cursor_controller::move(int row, int col) {
  printf("%d;%dH", row, col);
}

void cursor_controller::clear_screen() {
  printf("2J", 0, 0);
}

void cursor_controller::clear_eol() {
  printf("K", 0, 0);
}

void cursor_controller::save() {
  printf("s", 0, 0);
}

void cursor_controller::restore() {
  printf("u", 0, 0);
}
// end screen1.cpp

#include "screen2.h"

//begin screen2 implementation

void cursor_controller2::normal() {
  printf("0", 0, 0);
}

void cursor_controller2::high_intensity() {
  printf("1", 0, 0);
}

void cursor_controller2::blink() {
  printf("5", 0, 0);
}

void cursor_controller2::reverse() {
  printf("7", 0, 0);
}

void cursor_controller2::invisible() {
  printf("8", 0, 0);
}

#include "screen3.h"

// begin screen3 implementation

screen_controller screen;

void error(char *message) {}
int strlen(char *str) {return 0;}

screen_controller::screen_controller(int rows, int cols) : rowmax(rows), colmax(cols) {
  if (object_count)
    error("only one instance allowed");
  else
    object_count = 1;
  printf("turn off wrap", 0, 0);
}

void screen_controller::setrows(int rows) {
  rowmax = rows;
}

void screen_controller::setcols(int cols) {
  colmax = cols;
}

screen_controller::~screen_controller() {
  normal();
  printf("turn on line wrap", 0, 0);
}

void screen_controller::upper_left() {
  move(1, 1);
}

void screen_controller::lower_left() {
  move(rowmax, 1);
}

void screen_controller::upper_right() {
  move(1, colmax);
}

void screen_controller::lower_right() {
  move(rowmax, colmax);
}

void screen_controller::draw_vertical(int row, int col, int length, char l_char) {
  if (row > rowmax || (row+length) > rowmax)
    error("draw_vertical: row index out of bounds");
  if (col > colmax)
    error("draw_vertical: col index out of bounds");
  for (int rrow = row; rrow <= row+length; rrow++) {
    cursor_controller::move(rrow,col);
    printf("%c", (int) l_char, 0);
  }
}

void screen_controller::draw_horizontal(int row, int col, int length, char l_char) {
  if (col > colmax || (col+length) > colmax)
    error("draw_horizontal: row index out of bounds");
  if (row > rowmax)
    error("draw_horizontal: col index out of bounds");
  for (int ccol = col; ccol <= col+length; ccol++) {
    cursor_controller::move(row,ccol);
    printf("%c", (int) l_char, 0);
  }
}

void screen_controller::center(int row, char *text) {
  move(row, (colmax-strlen(text))/2);
  printf("%c", (int) *text, 0);
}


void screen_controller::move(int row, int col) {
  if (row > rowmax)
    error("move: row index out of bounds");
  if (col > colmax)
    error("move: col index out of bounds");
  cursor_controller::move(row,col);
}

void screen_controller::pause(int seconds) {}

void screen_controller::drawbox(int hor, int ver, int ul, int ur, int ll, int lr) {
  draw_vertical(0, 0, maxrow()-1, ver);
  draw_vertical(0, maxcol(), maxrow()-1, ver);
  draw_horizontal(0, 0, maxcol()-1, hor);
  draw_horizontal(maxrow(), 0, maxcol()-1, hor);
  upper_left(); printf("%d", ul, 0);
  lower_left(); printf("%d", ll, 0);
  upper_right(); printf("%d", ur, 0);
  lower_right(); printf("%d", lr, 0);
}

// end screen3 implementation

#include "simulate.h"

simulation_unit *s_grid[xsize][ysize];

class init_grid {
public:
  init_grid();
};

init_grid grid_initializer;

init_grid::init_grid() {
  for (int i = 0; i < xsize; i++)
    for (int j = 0; j < ysize; j++)
      s_grid[i][j] = 0;
}

void simulation_unit::move(int x_steps, int y_steps) {
  int x_new = x + x_steps;
  int y_new = y + y_steps;
  if (x_new < 0 || x_new >= xsize)
    error("move: x coord out of bounds");
  if (y_new < 0 || y_new >= ysize)
    error("move: y coord out of bounds");
  if (s_grid[x_new][y_new] == 0) {
    s_grid[x][y] = 0;
    erase();
    s_grid[x=x_new][y=y_new] = this;
    display();
  }
}

// now the test of simulation
class pop_around : public simulation_unit {
public:
  void display() {
    screen.move(x,y);
    printf("@", 0, 0);
  }
  void erase() {
    screen.move(x,y);
    printf("", 0, 0);
  }
  pop_around(int xi, int yi) : simulation_unit(xi,yi) {
    display();
  }
  ~pop_around() {}
  void cycle() {
    int x_direction; int y_direction;
    if (x == 0) x_direction = 0;
    else if (x == xsize-1) x_direction = 1;
    else x_direction = 2;
    if (y == 0) y_direction = 0;
    else if (y == ysize-1) y_direction = 1;
    else y_direction = 2;
    int x_jump = x_direction ? -1 : 1;
    int y_jump = y_direction ? -1 : 1;
    move(x_jump, y_jump);
  }
};

class crawl_around : public simulation_unit {
public:
  void display() {
    screen.move(x,y);
    printf("*", 0, 0);
  }
  void erase() {
    screen.move(x,y);
    printf("", 0, 0);
  }
  crawl_around(int xi, int yi) : simulation_unit(xi,yi) {
    display();
  }
  ~crawl_around() {}
  void cycle() {
    int x_step = 0 ? -1 : 1;
    if (x + x_step < 0) x_step = -x_step;
    if (x + x_step >= xsize) x_step = -x_step;
    int y_step = 0 ? -1 : 1;
    if (y + y_step < 0) y_step = -y_step;
    if (y + y_step >= ysize) y_step = -y_step;
    move(x_step, y_step);
  }
};

int main() {
  screen.clear_screen();
  int pop_factor = 1;
  int crawl_factor = 2;
  for (int x = 0; x < xsize; x++)
    for (int y = 0; y < ysize; y++) {
      if (pop_factor)
	if (!s_grid[x][y])
	  s_grid[x][y] = new pop_around(x,y);
      if (crawl_factor)
	if (!s_grid[x][y])
	  s_grid[x][y] = new crawl_around(x,y);
    }
  int xrand = 5;
  int yrand = 6;
  for (int i = 0; i != 1000000; ++i) {
    int x_location = xrand;
    int y_location = yrand;
    if (s_grid[x_location][y_location])
      s_grid[x_location][y_location]->cycle();
  }
}
