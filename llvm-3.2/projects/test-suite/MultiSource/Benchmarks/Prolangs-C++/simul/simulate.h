#define xsize 25
#define ysize 80

class simulation_unit {
protected:
  int x, y;
public:
  virtual void display() {}
  virtual void erase() {}
  simulation_unit(int x_loc, int y_loc) : x(x_loc), y(y_loc) {
    display();
  }
  virtual ~simulation_unit() {erase();}
  virtual void cycle() {}
  void move(int x_steps, int y_steps);
};

extern simulation_unit *s_grid[xsize][ysize];
