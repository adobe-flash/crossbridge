/* File : example.h */

class CRectangle {
    int x, y;
public:
    void set_values (int,int);
    int area () {return (x*y);}
    CRectangle();
    CRectangle(const CRectangle& c);
};

int createRectAndGetArea();

  
