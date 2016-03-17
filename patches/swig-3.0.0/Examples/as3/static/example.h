/* File : example.h */


class Circle {
private:
  double radius;
public:
  Circle(double r) : radius(r) { };
  virtual double area(void);
  virtual double perimeter(void);
  static double getPi();
};





  
