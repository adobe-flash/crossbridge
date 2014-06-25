/* File : example.h */

#include <cstdio>
#include <iostream>

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
