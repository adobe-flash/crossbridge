// Borland C++

/* vpoint.h--Example from Getting Started */

// version of point.h with virtual functions for use with VCIRCLE
// vpoint.h contains two classes:
// class Location describes screen locations in X and Y coordinates
// class Point describes whether a point is hidden or visible

enum Boolean { False, True };

class Location {
protected:          // allows derived class to access private data
   int X;
   int Y;

public:             // these functions can be accessed from outside
   Location(int InitX, int InitY);
   int GetX(void);
   int GetY(void);
};

class Point : public Location {      // derived from class Location
// public derivation means that X and Y are protected within Point

   protected:
   Boolean Visible;  // classes derived from Point will need access    

public:
   Point(int InitX, int InitY) : Location(InitX,InitY) { // constructor
	Visible = False;                  // make invisible by default
   }
   virtual void Show(void);
   virtual void Hide(void);
   Boolean IsVisible(void);
   void MoveTo(int NewX, int NewY);
};
