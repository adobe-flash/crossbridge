// module fib "fib.es"

class Fib {
    function Fib(n) {
       if (n < 2)
           val = n;
       else
           val = (new Fib(n-1)).val + (new Fib(n-2)).val
    }
    var val;
}
print((new Fib(10)).val)
