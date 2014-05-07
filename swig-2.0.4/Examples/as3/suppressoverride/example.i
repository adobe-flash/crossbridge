/* File : example.i */
%module ExampleModule

// Because no destructor is exposed in the base class
// we must suppress the use of the AS3 override keyword
// when wrapping the derived class's destructor
%as3suppressoverride derived::~derived;
