/* File : example.cxx */
#include <stdio.h>
#include "example.h"


void CRectangle::set_values (int a, int b) {
    x = a;
    y = b;
}

// constructor
CRectangle::CRectangle(){
    x = 2;
    y = 2;
}

// copy constructor
CRectangle::CRectangle(const CRectangle& c){
    printf("in copy constructor\n"); // output of this test should include this string
    x = c.x;
    y = c.y;
}

int createRectAndGetArea() {
    // call the normal constructor
    CRectangle rect1;
    rect1.set_values(3, 4);
    
    // call the copy constructor
    CRectangle rect2 = rect1;
    
    // return the area (should be 3*4=12)
    return rect2.area();
}
