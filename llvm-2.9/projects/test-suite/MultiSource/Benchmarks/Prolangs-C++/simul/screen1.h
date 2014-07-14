class cursor_controller {
 public:
  // simple cursor motion:
  void up(int rows = 1);
  void down(int rows = 1);
  void right(int cols = 1);
  void left(int cols = 1); //
  void move(int row, int col); // absolute positioning
  void home() {move(1,1);}
  // erasing portions of the screen:
  void clear_screen() ; // also send cursor home
  void clear_eol();
  void save();
  void restore();
};

