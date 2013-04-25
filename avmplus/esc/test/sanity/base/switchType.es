class A { }
class B { }
switch type (new A) {
case (x:A) { print('A') }
case (x:B) { print('B') }
case (x:*) { print('*') }
}
