/* File : example.h */

class CVector {
private:
  double x,y,z;
public:
  CVector() : x(0), y(0), z(0) { };
  CVector(double x, double y, double z) : x(x), y(y), z(z) { };
  friend CVector operator+(const CVector &a, const CVector &b);
  char *print();
};

class VectorArray {
private:
  CVector *items;
  int     maxsize;
public:
  VectorArray(int maxsize);
  ~VectorArray();
  CVector &operator[](int);
  int size();
};



  
