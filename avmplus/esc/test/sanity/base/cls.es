class A { 
  var x; 
  function A (x) 
//    : x=x 
  {}
  print('static hi') 
}
print (new A(10).x)
class B { var y; function B(x) : y=x {}; print('static hi from B')}
print (new B(10).y)
