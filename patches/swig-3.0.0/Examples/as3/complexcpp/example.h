/* File : example.h */

#include <cstdio>
#include <iostream>
#include <stdint.h> 

// EXTENDS

typedef float float32;

class BaseClass
{
public:
	BaseClass()/*{}*/;

	virtual ~BaseClass() {}
    
	virtual void DrawCircle(float32 radius) = 0;
    
	virtual void DrawSegment() = 0;
    
	void DrawConcrete() {};

protected:
	int m_drawFlags;
};

class ExtendedClass : public BaseClass {
public:
	ExtendedClass() {}
	~ExtendedClass() {}
    void DrawCircle(float32 radius) {}
	void DrawSegment() {}
};

// COPY CONSTRUCTOR

class CRectangle {
    int x, y;
public:
    void set_values (int,int);
    int area () {return (x*y);}
    CRectangle();
    CRectangle(const CRectangle& c);
};

int createRectAndGetArea();
  
// PRINT POINTER

struct Bar {
	int num;
};

void printPointer(struct Bar *number); 

// AS3 IMPORT

void printNumber(int i);
void printNumber2(int i);  

// NESTED CPP

/** Design a motor car from various components */
/*struct MotorCar {

  struct DesignOpinion {
    bool itrocks;
    std::string reason;
    static int AceDesignCount;
    static int TotalDesignCount;
    static int PercentScore();
  };

  struct Wheels {
    enum Shape { Round, Square };
    Wheels(Shape shape, size_t count);
    DesignOpinion Opinion();
  private:
    Shape shape;
    size_t count;
  };

  struct WindScreen {
    WindScreen(bool opaque);
    DesignOpinion Opinion();
  private:
    bool opaque;
  };

  static MotorCar DesignFromComponents(const std::string &name, const Wheels &wheels, const WindScreen &windscreen);

  std::string Name() {
    return name;
  }

  std::string WillItWork();

private:
  MotorCar(const std::string &name, const Wheels &wheels, const WindScreen &windscreen);
  std::string name;
  Wheels wheels;
  WindScreen windscreen;
};*/